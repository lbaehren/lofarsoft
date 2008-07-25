#include <images/Images/PagedImage.h>
#include <images/Images/ImageInfo.h>
#include <images/Images/ImageRegion.h>
#include <coordinates/Coordinates/CoordinateSystem.h>
#include <coordinates/Coordinates/CoordinateUtil.h>
#include <lattices/Lattices/ArrayLattice.h>
#include <lattices/Lattices/LatticeIterator.h>
#include <lattices/Lattices/LCRegion.h>
#include <lattices/Lattices/LCBox.h>

#include <measures/Measures/MeasConvert.h>
#include <measures/Measures/MeasTable.h>
#include <measures/Measures/MPosition.h>
#include <measures/Measures/MDirection.h>
#include <measures/Measures/Stokes.h>
#include <measures/Measures/MBaseline.h>
#include <measures/Measures/MEpoch.h>
#include <measures/Measures/MeasFrame.h>
#include <casa/Quanta/MVTime.h>

#include <casa/Arrays/Array.h>
#include <casa/Arrays/ArrayIO.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/ArrayLogical.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Exceptions/Error.h>
#include <scimath/Functionals/Polynomial.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Slicer.h>
#include <casa/Quanta/QLogical.h>
#include <tables/Tables/TableDesc.h>
#include <tables/Tables/SetupNewTab.h>
#include <tables/Tables/Table.h>
#include <tables/Tables/TableRecord.h>
#include <casa/Utilities/Assert.h>
#include <casa/Utilities/DataType.h>
#include <casa/BasicSL/String.h>
#include <casa/Utilities/Regex.h>
#include <casa/BasicSL/Constants.h>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <assert.h>

using namespace std;
using namespace casa;


enum PixelType
{
    NON_PHYSICAL = 0,
    ALWAYS_UP,
    NEVER_UP,
    SOMETIMES_UP,
    N_PixelType
};


class PixelInfo
{
public:
    PixelInfo()
        :   m_type(NON_PHYSICAL),
            m_ra_center(0.0),
            m_ra_range(0.0)
    {}
    
    PixelInfo(PixelType type, double center, double range)
        :   m_type(type),
            m_ra_center(center),
            m_ra_range(range)
    {}
          
    PixelType               m_type;
    double                  m_ra_center;
    double                  m_ra_range;
};


// CoordinateSystem.toWorldMany() does not work as specified. In particular,
// the failures array is always set to all false and its length is inconsistent.
void makePixelInfoMap(const CoordinateSystem &csys, IPosition size,
    double zenith_dec, vector<vector<PixelInfo> > &map,
    double min_el = 0.0)
{
    assert(0.0 <= min_el && min_el < C::pi_2);
    
    // Initialize map.
    map.resize(size(0));
    for(int i = 0; i < size(0); ++i)
    {
        map[i].resize(size(1));
    }

    Vector<Double> world = csys.referenceValue();
    Vector<Double> pixel = csys.referencePixel();

    cout << "Precalculating pixel info map" << flush;
    for(Int x = 0; x < size(0); ++x)
    {
        for(Int y = 0; y < size(1); ++y)
        {
            pixel(0) = x;
            pixel(1) = y;

            // CoodinateSystem::toWorld()
            // DEC range [-pi/2,pi/2]
            // RA range [-pi,pi]
            if(!csys.toWorld(world, pixel))
            {
                map[x][y] = PixelInfo(NON_PHYSICAL, 0.0, 0.0);
            }
            else
            {
                double min_ad = fabs(world(1) - zenith_dec);
                // One can either go via the north or via the south pole.
                double max_ad = min(C::pi - zenith_dec - world(1),
                    C::pi + zenith_dec + world(1));

                if(min_ad > max_ad)
                {
                    // When the local zenith declination and/or the pixel declination
                    // is at a pole (+/- 90 deg), min_ad is equal to max_ad in theory.
                    // However, due to numerical error they may not be equal in practice.
                    // In particular, min_ad may be larger than max_ad, which violates
                    // the precondition min_ad <= max_ad.
                    // If min_ad > max_ad, the difference should be very small. If it
                    // indeed is very small, we set max_ad to min_ad and continue.
                    // Otherwise, a precondition is violated which indicates a bug in
                    // the implementation and therefore the program will be aborted.

                    assert(min_ad - max_ad < 1e-15);
                    max_ad = min_ad;
                }

                // Everything with an angular distance larger than the clipping
                // value should be masked.
                double ad_clip = C::pi_2 - min_el;
                
                if(min_ad > ad_clip)
                {
                    // Never rises.
                    // (Rising is interpreted as going above min_el).
                    map[x][y] = PixelInfo(NEVER_UP, 0.0, 0.0);
                }
                else if(max_ad < ad_clip)
                {
                    // Never sets.
                    // (Setting is interpreted as going below min_el).
                    map[x][y] = PixelInfo(ALWAYS_UP, 0.0, 0.0);
                }
                else
                {
                    // Compute the RA range for which the pixel is above min_el.
                    double cos_delta_ra =
                        (cos(ad_clip) - sin(zenith_dec) * sin(world(1)))
                        / (cos(zenith_dec) * cos(world(1)));
                    assert(-1.0 <= cos_delta_ra && cos_delta_ra <= 1.0);

                    double delta_ra = acos(cos_delta_ra); // range [0, pi]
                    assert(0.0 <= delta_ra && delta_ra <= C::pi);

                    map[x][y] =
                        PixelInfo(SOMETIMES_UP, world(0), 2.0 * delta_ra);
                }
            }
        }
        
        if(x % 100 == 0)
        {
            cout << "." << flush;
        }
    }
    cout << " done." << endl;
}


inline double shift(double origin, double x)
{
    double x_new = fmod(x - origin, C::_2pi);
    if(x_new < 0.0)
    {
        x_new += C::_2pi;
    }
    return x_new;
}


void processImage(const string &filename, const MPosition &position,
    const MEpoch &start, const MEpoch &end, Matrix<Float> &mask,
    Matrix<Float> &accumulator, const vector<vector<PixelInfo> > &pixel_info, bool normalize)
{
    double duration = end.getValue().getTime("s").getValue() -
        start.getValue().getTime("s").getValue();

    assert(0.0 < duration && duration < 24.0 * 3600.0);

    cout << endl;
    cout << "Image   : " << filename << endl;
    cout << "Start   : " << MVTime::Format(MVTime::YMD, 6) << MVTime(start.getValue())
        << endl;
    cout << "Duration: " << duration / 3600.0 << " hour(s)" << endl;

    String im_name(filename);
    PagedImage<Float> im(im_name);

    // Check preconditions.
    const CoordinateSystem &csys = im.coordinates();
    Vector<String> axes(csys.worldAxisNames());
    assert(axes.nelements() >= 2
        && axes(0) == String("Right Ascension")
        && axes(1) == String("Declination"));
    assert((im.shape()(0) == accumulator.shape()(0))
        && (im.shape()(1) == accumulator.shape()(1)));

    // Compute RA range of local zenith.
    pair<double, double> zenith_ra;

    MeasFrame frame;
    frame.set(position);
    frame.set(start);

    // Create conversion engine.
    MDirection dir(MVDirection(0.0, C::pi_2),
        MDirection::Ref(MDirection::AZEL, frame));
        
    MDirection::Convert converter(dir,
        MDirection::Ref(MDirection::J2000));
    
    // Compute right ascension and declination at start of measurement.
    MDirection radec(converter());
    Vector<Double> vec_radec = radec.getValue().getAngle("rad").getValue();
    zenith_ra.first = vec_radec(0);

    // Update reference frame to end time.
    frame.set(end);

    // Compute right ascension and declination at end of measurement.
    radec = converter();
    vec_radec = radec.getValue().getAngle("rad").getValue();
    zenith_ra.second = vec_radec(0);

    cout << "Local zenith RA range: [" << setprecision(10) << zenith_ra.first
        << ", " << zenith_ra.second << "]" << endl;

    // Get pixel data.
    Array<Float> pixel_data_tmp;
    im.get(pixel_data_tmp, true);
    Matrix<Float> pixel_data(pixel_data_tmp);

    // Normalize the image if requested.
    if(normalize)
    {
//        Float min, max;
//        minMax(min, max, pixel_data);
//        cout << "Value range: [" << min << ", " << max << "]" << endl;

//        Float range = max - min;
//        if(range > 0.0)

        Float peak = max(pixel_data);
        if(peak > 0.0)
        {
            pixel_data /= peak;
        }
    }

    // Process image.
    cout << "Processing image" << flush;

    // Take the RA of the local zenith at the start of the measurement as the
    // origin.
    double zenith_end = shift(zenith_ra.first, zenith_ra.second);
    
    for(Int x = 0; x < pixel_data.shape()(0); ++x)
    {
        for(Int y = 0; y < pixel_data.shape()(1); ++y)
        {
            if(pixel_info[x][y].m_type == ALWAYS_UP)
            {
                // Just add pixels that are always up to the accumulator. The
                // mask is updated when writing the output image.
                accumulator(x, y) += pixel_data(x, y);
            }
            else if(pixel_info[x][y].m_type == SOMETIMES_UP)
            {
                // Compute overlap between the RA range for which the pixel is
                // up, and the RA range covered by the local zenith during the
                // measurement.
                double ra_start = shift(zenith_ra.first, 
                    pixel_info[x][y].m_ra_center - 0.5 * pixel_info[x][y].m_ra_range);
                double ra_end = shift(zenith_ra.first,
                    pixel_info[x][y].m_ra_center + 0.5 * pixel_info[x][y].m_ra_range);
            
                double overlap = 0.0;
                if(ra_start >= zenith_end && ra_end >= zenith_end)
                {
                    if(ra_start >= ra_end)
                    {
                        overlap = zenith_end;
                    }
                }
                else if(ra_start <= zenith_end && ra_start <= ra_end)
                {
                    overlap = min(zenith_end, ra_end) - ra_start;
                }
                else
                {
                    overlap = ra_end;

                    if(ra_start < zenith_end)
                    {
                        overlap += zenith_end - ra_start;
                    }
                }
                overlap /= zenith_end;
            
                if(overlap >= 1.0)
                {
                    // Only include a pixel if it is up during the entire
                    // measurement.
                    accumulator(x, y) += pixel_data(x, y);
                    mask(x, y) += 1.0;
                }
            }
        }

        if(x % 100 == 0)
        {
            cout << "." << flush;
        }
    }
    cout << " done." << endl;
}


void usage()
{
    cout << "Usage: stitch [options] <file list>"
        << " <ouput (skeleton) AIPS++ image>" << endl;
    cout << "Options:" << endl;
    cout << "    -n normalize images (default: false)" << endl;
    cout << "    -e min. elevation (deg, default: 0)" << endl;
}


int main(int argc, char **argv)
{
    if(argc < 3)
    {
        usage();
        return -1;
    }
    
    double min_el = 0.0;
    bool normalize = false;
    
    // Parse option(s).
    int idx = 1;
    while(idx < (argc - 2))
    {
        string arg(argv[idx]);

        if(arg.size() != 2 || arg[0] != '-')
        {
            ++idx;
            continue;
        }
        
        switch(arg[1])
        {
        case 'n':
            normalize = true;
            break;

	case 'e':
            {
                ++idx;
                if(idx >= (argc - 2))
                {
                    cout << "error: no value for option: " << arg << endl;
                    return -1;
                }
 
                istringstream iss(argv[idx]);
                iss >> min_el;
                if(iss.fail() || min_el < 0.0 || min_el >= 90.0)
                {
                    cout << "error: invalid value for option: " << arg << endl;
                    return -1;
                }
            }
            break;
             
        default:
            cout << "error: invalid option: " << arg << endl;
            return -1;
        }

        ++idx;
    }
    
    cout << "Normalize images: " << (normalize ? "yes" : "no") << endl;
    cout << "Minimal elevation: " << min_el << " degrees" << endl;
    min_el = (min_el * C::pi) / 180.0;

    // Start processing.
    bool init = true;
    double zenith_dec;
    IPosition im_size;
    Matrix<Float> im_mask;
    Matrix<Float> im_data;
    vector<vector<PixelInfo> > pixel_info;
    MPosition position;
    Float im_count = 0.0;
    
    ifstream in(argv[argc - 2]);
    while(in.good())
    {
        string line;

        if(getline(in, line))
        {
            istringstream iss(line);
            
            string filename;
            MVTime mvt_start, mvt_end;
            
            iss >> filename >> mvt_start >> mvt_end;

            if(!iss.fail() && !filename.empty() && !(filename[0] == '#'))
            {
                double q_start, q_end;

                q_start = mvt_start.get("s").getValue();
                q_end = mvt_end.get("s").getValue();
                assert(q_start > 0.0 && q_end > 0.0 && q_start < q_end);

                MEpoch start(mvt_start.get("s"), MEpoch::Ref(MEpoch::UTC));
                MEpoch end(mvt_end.get("s"), MEpoch::Ref(MEpoch::UTC));

                if(init)
                {
                    init = false;
                    
                    // Initialize accumulator and mask.
                    String name(filename);
                    PagedImage<Float> im(name);
                    
                    // Get observation info
                    const CoordinateSystem &csys = im.coordinates();
                    ObsInfo info(csys.obsInfo());
                    assert(MeasTable::Observatory(position, info.telescope()));
                    cout << "Telescope: " << info.telescope() << endl;

                    // Compute DEC of local zenith (constant in time).
                    MeasFrame frame;
                    frame.set(position);
                    frame.set(start);
                    
                    // Create conversion engine.
                    MDirection dir(MVDirection(0.0, C::pi_2),
                        MDirection::Ref(MDirection::AZEL, frame));
        
                    MDirection::Convert converter(dir,
                        MDirection::Ref(MDirection::J2000));
                        
                    MDirection radec(converter());
                    Vector<Double> vec_radec =
                        radec.getValue().getAngle("rad").getValue();
                    
                    zenith_dec = vec_radec(1);
                    cout << "Local zenith DEC: " << setprecision(10)
                        << zenith_dec << endl;
                    
                    // Sanity check the world axes.
                    Vector<String> axes(csys.worldAxisNames());
                    assert(axes.nelements() >= 2
                        && axes(0) == String("Right Ascension")
                        && axes(1) == String("Declination"));
                    
                    // Determine image size.
                    im_size = IPosition(2, im.shape()(0), im.shape()(1));
                    cout << "Image size: " << im_size(0) << " x " << im_size(1)
                        << endl;
                        
                    // Initialize mask and accumulator.
                    im_mask = Matrix<Float>(im_size, 0.0);
                    im_data = Matrix<Float>(im_size, 0.0);

                    // Make pixel map.
                    makePixelInfoMap(csys, im_size, zenith_dec, pixel_info,
                        min_el);
                }

                processImage(filename, position, start, end, im_mask, im_data,
                    pixel_info, normalize);
                im_count += 1.0;
            }   
        }
    }
    in.close();

    assert(im_count > 0.0);
    
    String filename(argv[argc - 1]);
    PagedImage<Float> out(filename);

    cout << endl;
    cout << "Output image: " << argv[argc - 1] << endl;
    
    // Check preconditions.
    const CoordinateSystem &csys = out.coordinates();
    Vector<String> axes(csys.worldAxisNames());
    assert(axes.nelements() == 4
        && axes(0) == String("Right Ascension")
        && axes(1) == String("Declination"));
    assert((out.shape()(0) == im_size(0))
        && (out.shape()(1) == im_size(1)));

    cout << "Writing output image" << flush;
    // Create image mask, define it as region, set it as default mask, and
    // initialize it to False.
    String name("STITCH_MASK");
    ImageRegion region = out.makeMask(name, True, True, True, False);
    LCRegion &mask = region.asMask();

    for(Int x = 0; x < im_size(0); ++x)
    {
        for(Int y = 0; y < im_size(1); ++y)
        {
            if(pixel_info[x][y].m_type == ALWAYS_UP)
            {
                mask.putAt(True, IPosition(4, x, y, 0, 0));
                im_data(x, y) /= im_count;
            }
            else if(pixel_info[x][y].m_type == SOMETIMES_UP
                && im_mask(x, y) > 0.0)
            {
                mask.putAt(True, IPosition(4, x, y, 0, 0));
                im_data(x, y) /= im_mask(x, y);
            }
        }

        if(x % 100 == 0)
        {
            cout << "." << flush;
        }
    }

    // Write averaged image data.
    out.put(im_data);
    out.flush();
    cout << " done." << endl;

    return 0;
}
