#include <casa/OS/Directory.h>
#include <casa/OS/DirectoryIterator.h>
#include <casa/Inputs/Input.h>
#include <casa/Logging/LogFilter.h>
#include <images/Images/ImageRegrid.h>
#include <images/Images/FITSImage.h>
#include <images/Images/PagedImage.h>
#include <images/Images/ImageStatistics.h>
#include <images/Images/ImageSummary.h>


using namespace casa;
using namespace std;


//////////////////////////////////////
// Globals. I don't plan mutlithreading, and it's just easier this way
Input inputs(1);
Interpolate2D::Method interp;
Double mem_limit;

// target file
CoordinateSystem target_cs;
IPosition target_shape;

// input files metadata
Int in_count;
vector<CoordinateSystem> in_cs;
vector<IPosition> in_shape;
vector<String> in_fname;

// data for regridded images
vector<IPosition> rg_bbox;


/////////////////////////////////////
// Forward declarations
void parse_template(String &fname, LogIO os);
void scan_inputs(String &dirname, LogIO os);
void find_target_bbox(LogIO os, Bool auto_bb);
void regrid_and_coadd(String &fname, LogIO os);

IPosition _find_bbox(DirectionCoordinate tc, DirectionCoordinate sc, IPosition shape);
IPosition _join_bboxes(IPosition &bb1, IPosition &bb2);
IPosition _xsect_bboxes(IPosition &bb1, IPosition &bb2);

/////////////////////////////////////
// main
int main(int argc, const char **argv)
{
  inputs.version("TEST0");

  inputs.create("indir", "", "directory with images to process", "InDir");
  inputs.create("out", "out.fits", "Output image name", "String");
  inputs.create("template", "", "template file defining target coordinate systems for mosaic image", "InFile");
  inputs.create("autoBB", "True", "autodetect target image size", "Bool");
  inputs.create("interp", "2", "pixel interpolation method: Nearest(0), Linear(1), Cubic(2)", "Int");
  inputs.create("memlimit", "-1", "memory limit for temporary arrays", "Double");

  inputs.readArguments(argc, argv);

  // create logging stream
  LogIO os;
  if (inputs.debug(1))
    LogSink::globalSink().filter(LogFilter(LogMessage::DEBUGGING));

  ////////////////////////////////
  // First read & parse target image template
  String tmpl_name = inputs.getString("template");
  if (tmpl_name == "")
    throw AipsError("template filename must be given");
  parse_template (tmpl_name, os);

  ////////////////////////////////
  // Now scan input files & store their metadata
  String in_dir = inputs.getString("indir");
  if (in_dir == "")
    throw AipsError("directory with input files must be given");
  scan_inputs(in_dir, os);

  ///////////////////////////////
  // Calculate ref. pixel & size of target image
  Bool auto_bb = inputs.getBool("autoBB");
  find_target_bbox(os, auto_bb);

  ///////////////////////////////
  // Set some input parameters
  Int idx = inputs.getInt("interp");
  mem_limit = inputs.getDouble("memlimit");
  // it's a hack, but it's easier this way :)
  if (idx >=0 && idx < 3) {
    interp = (Interpolate2D::Method) idx;
  } else
    throw AipsError("wrong interpolation method");

  ///////////////////////////////
  // Generate output image
  String out_name = inputs.getString("out");
  if (out_name == "")
    throw AipsError("output file name must be given");
  regrid_and_coadd(out_name, os);

  return 0;
}


//////////////////////////////////
// Actually regrid all input images and add them up
void regrid_and_coadd(String &fname, LogIO os)
{
  os << LogOrigin("", "regrid_and_coadd");
  os << "Building mosaic" << LogIO::POST;

  ImageRegrid<Float> regridder;
  TempImage<Float> mosaic(target_shape, target_cs, mem_limit);
  TempLattice<Float> weights(target_shape, mem_limit);
  mosaic.makeMask("mask", True, True, True, False);
  mosaic.set(0);
  weights.set(0);

  for (int i = 0; i < in_count; ++i) {
    os << "Regridding file " << in_fname[i] << LogIO::POST;

    // skip images which don't contribute
    if (rg_bbox[i].nelements() == 0)
      continue;

    CoordinateSystem tcs = target_cs;
    Vector<Double> ref_pixel = tcs.referencePixel();
    IPosition shape = target_shape;
    IPosition bbox = rg_bbox[i];

    // adjust ref_pixel so that origin of tcs matches expected origin of reprojected bbox 
    shape[0] = bbox[1] - bbox[0];
    shape[1] = bbox[3] - bbox[2];
    ref_pixel[0] -= bbox[0];
    ref_pixel[1] -= bbox[2];
    tcs.setReferencePixel(ref_pixel);

    // regrid input image
    FITSImage src(in_fname[i]);
    TempImage<Float> tmp(shape, tcs, mem_limit);
    //PagedImage<Float> tmp(shape, tcs, String(in_fname[i])+".img");
    tmp.makeMask("mask");
    regridder.regrid(tmp, interp, IPosition(2, 0, 1), src);

    // add regridded image to the resulting mosaic
    // first we select only required part of the mosaic
    IPosition start(shape.nelements(), Slicer::MimicSource);
    IPosition end(shape.nelements(), Slicer::MimicSource);
    start[0] = bbox[0]; end[0] = bbox[1] - bbox[0];
    start[1] = bbox[2]; end[1] = bbox[3] - bbox[2];
    Slicer slice(start, end);
    SubImage<Float> subImg(mosaic, slice, True);
    SubLattice<Float> subWeights(weights, slice, True);
    // now adding images:
    // 1. add new image with proper weight
    // 2. update mosaic mask to unmask newly added pixels
    // 3. update weights
    Float w = 1;
    subImg.copyData(LatticeExpr<Float>(subImg + w * tmp));
    subImg.pixelMask().copyData(LatticeExpr<Bool>(mask(subImg) || mask(tmp)));
    subWeights.copyData(LatticeExpr<Float>(iif(mask(tmp), subWeights + w, subWeights)));
  }

  // and the final step -- normalize resulting mosaic
  mosaic.copyData(LatticeExpr<Float>(iif(mask(mosaic), mosaic/weights, 0)));

  // and write out FITS file
  String error;
  Bool res = ImageFITSConverter::ImageToFITS (error, mosaic, fname,
					      // soo many others just to change some of them
					      64, True, True, -32, 1.0, -1.0,
					      True, // allow overwrite
					      False, True);
  if (!res)
    throw AipsError(error);
}


//////////////////////////////////
// Parse template file & build target coordinate systems from it
void parse_template(String &fname, LogIO os)
{
  os << LogOrigin("","parse_template");
  os << "Reading template file " << fname << LogIO::POST;

  ifstream file(fname.c_str());
  if(!file)
    throw AipsError("Can't open file \"" + fname + "\"");

  os << LogIO::DEBUGGING << "Reading template file " + fname << LogIO::POST;
  string buf;
  vector<String> lines;
  FitsKeywordList kwl;
  int line_no = 0;

  while (getline(file, buf)) {
    ++line_no;
    if (buf.size() < 80)  // wcslib parser wants 80-character strings. Pad with spaces as needed
      buf.append(80-buf.size(), ' ');
    lines.push_back(buf);
    kwl.parse(buf.c_str(), buf.size());
    if (kwl.no_parse_errs()) {
      // this is slightly funnty spot -- FitsKeywordList parser seems to be somewhat stricter than
      // wcslib. In particular it's trying to enforce number formatting & spacing.
      // As it makes generating/hand-typing of template files more tedious, I decided to hide it's 
      // error messages. Anyway we only need it to extract NAXIS keywords.
      for (int i = 0; i < kwl.no_parse_errs(); ++i)
	os << LogIO::DEBUGGING << "line " << line_no << ": " << kwl.parse_err(i) << LogIO::POST;
    }
  }

  os << LogIO::DEBUGGING << "Building CoordinateSystem" << LogIO::POST;
  Vector<String> header(lines);
  Record headerRec;
  Int stokesFITSValue = 1;
  Bool dropStokes = True;
  uInt whichRep = 0;
  IPosition shape;
  CoordinateSystem cSys;

  // first we need to setup shape
  FitsKeyword *kw = kwl(FITS::NAXIS);
  if (!kw)
    throw AipsError("Incorrect template file: Can't extract NAXIS keyword");
  int ndims = kw->asInt();
  shape.resize(ndims);
  for (int i = 0; i < ndims; ++i) {
    kw = kwl(FITS::NAXIS, i+1);
    if (!kw)
      throw AipsError(String("Incorrect template file: NAXIS") + i + " keyword missing or incorrect");
    shape[i] = kw->asInt();
  }

  // and now we call the routine to parse FITS header & build coordinate system of it
  cSys = ImageFITSConverter::getCoordinateSystem(stokesFITSValue, headerRec, header, 
						 os, whichRep, shape, dropStokes);

  target_cs = cSys;
  target_shape = shape;
}


///////////////////////////////////////
// Scan input files caching their metadata
void scan_inputs(String &dirname, LogIO os)
{
  os << LogOrigin("","scan_inputs");
  os << "Scanning input directory " << dirname << LogIO::POST;

  Directory indir(dirname);
  DirectoryIterator iter(indir);

  // iterate all files in given directory
  while (!iter.pastEnd()) {
    File file = iter.file();

    os << LogIO::DEBUGGING << "Going to open file " << file.path().originalName() << LogIO::POST;
    bool status = true;
    CoordinateSystem cs;
    IPosition shape;

    // try opening image & store it's metadata
    try {
      FITSImage img(file.path().originalName());
      cs = img.coordinates();
      shape = img.shape();
    } catch (AipsError err) {
      status=false;
      os << LogIO::DEBUGGING << err.getMesg() << LogIO::POST;
      os << LogIO::WARN << "Skipping file " << file.path().originalName() << ". Not a FITS file?" << LogIO::POST;
    }

    if (status) {
      ++in_count;
      in_cs.push_back(cs);
      in_shape.push_back(shape);
      in_fname.push_back(file.path().originalName());
    }

    ++iter;
  }

  if (in_count == 0)
    throw AipsError("No valid images found");

  os << "Found " << in_count << " images to process." << LogIO::POST;
}


void find_target_bbox(LogIO os, Bool auto_bb)
{
  os << LogOrigin("", "find_target_bbox");
  os << "Calculating geometry of target images" << LogIO::POST;

  // geometry calculations use are done on direction coordinate
  DirectionCoordinate tcs = target_cs.directionCoordinate(0);

  // reference pixel: for auto_bb case we set it to 0,0 and will adjust it later
  Vector<Double> ref_pixel(2, 0.);
  if (!auto_bb)
    ref_pixel = tcs.referencePixel();
  else
    tcs.setReferencePixel(ref_pixel);

  // now we project each input image onto tcs and calculate it's bbox
  for (int idx = 0; idx < in_count; ++idx)
    rg_bbox.push_back(_find_bbox(tcs, in_cs[idx].directionCoordinate(0), in_shape[idx]));

  // for auto_bb case calculate bbox encopassing all images and adjust ref_pixel & bboxes accordingly
  // in this case all input images "fit" into target image by definition
  if (auto_bb) {
    IPosition bb = rg_bbox[0];
    for (int idx = 1; idx < in_count; ++idx)
      bb = _join_bboxes(bb, rg_bbox[idx]);

    ref_pixel.assign(target_cs.referencePixel());
    int a = ref_pixel[0] = -bb[0];
    int b = ref_pixel[1] = -bb[2];
    target_cs.setReferencePixel(ref_pixel);

    target_shape[0] = bb[1] - bb[0];
    target_shape[1] = bb[3] - bb[2];

    IPosition offset(4, a, a, b, b);
    for (int idx = 0; idx < in_count; ++idx)
      rg_bbox[idx] += offset;
  }

  // case 2: predefined ref_pixel & image size
  // we need to figure which input inpages contribute to the target image and
  // their bbox in the target coordinate system
  if (!auto_bb) {
    ref_pixel.assign(target_cs.referencePixel());
    IPosition target_bbox(4, 0, target_shape[0], 0, target_shape[1]);

    for (int idx = 0; idx < in_count; ++idx) {
      IPosition tmp = _xsect_bboxes(rg_bbox[idx], target_bbox);
      rg_bbox[idx].resize(tmp.nelements());
      rg_bbox[idx] = tmp;
    }
  }

  // DEBUG
  //cout << "shape " << target_shape << endl;
  //cout << "rpix " << ref_pixel << endl;
  //for (int i = 0; i < in_count; ++i)
  //  cout << rg_bbox[i] << endl;
}


//////////////////////////////////////////
// Project image sized 'shape' from coordinates 'sc' to 'tc' and
// calculate it's bbox in new coordinates
IPosition _find_bbox(DirectionCoordinate tc, DirectionCoordinate sc, IPosition shape)
{
  const int STEP = 5;
  int npixels = ((shape[0]+STEP-1)/STEP)*((shape[1]+STEP-1)/STEP);
  Matrix<Double> pixel(IPosition(2, 2, npixels), 0.);
  Matrix<Double> world;
  Vector<Bool> failures;

  // set units to radians -- this might save us some conversions later
  Vector<String> units(2);
  units = "rad";
  tc.setWorldAxisUnits(units);
  sc.setWorldAxisUnits(units);

  // fill in pixel grid with STEP
  int idx = 0;
  for (int i = 0; i < shape[0]; i+=STEP) {
    for (int j = 0; j < shape[1]; j+=STEP) {
      pixel(0, idx) = i;
      pixel(1, idx) = j;
      ++idx;
    }
  }
  AlwaysAssert(npixels == idx, AipsError);

  // transform to world coordinates
  sc.toWorldMany(world, pixel, failures);

  // clean up failed conversions
  {
    int nfailed = 0;
    for (int i = 0; i < world.ncolumn(); ++i)
      if (failures[i])
	++nfailed;
    if (nfailed > 0) {
      Matrix<Double> nworld(IPosition(2, 2, world.ncolumn() - nfailed), 0.);
      int ni = 0;
      for (int i = 0; i < world.ncolumn(); ++i)
	if (!failures[i]) {
	  nworld(0, ni) = world(0, i);
	  nworld(1, ni) = world(1, ni);
	  ++ni;
	}
      world.resize(nworld.shape());
      world = nworld;
    }
  }

  // FIXME: Convert world coordinates of sc to world coordinates of tc
  if (tc.directionType() != sc.directionType()) {
    MDirection::Convert conv("rad", sc.directionType(), tc.directionType());
    Vector<Double> tmp(2);
    MDirection tmp2;
    for (int i = 0; i < world.ncolumn(); ++i) {
      tmp[0] = world(0, i);
      tmp[1] = world(1, i);
      tmp2 = conv(tmp);
      tmp = tmp2.getAngle().getValue("rad");
      world(0,i) = tmp[0];
      world(1,i) = tmp[1];
    }
  }

  // transform back to pixel coordinates (using target coordinate system)
  tc.toPixelMany(pixel, world, failures);

  // one more round of cleanup
  {
    int nfailed = 0;
    for (int i = 0; i < pixel.ncolumn(); ++i)
      if (failures[i])
	++nfailed;
    if (nfailed > 0) {
      Matrix<Double> npixel(IPosition(2, 2, pixel.ncolumn() - nfailed), 0.);
      int ni = 0;
      for (int i = 0; i < npixel.ncolumn(); ++i)
	if (!failures[i]) {
	  npixel(0, ni) = pixel(0, i);
	  npixel(1, ni) = pixel(1, ni);
	  ++ni;
	}
      pixel.resize(npixel.shape());
      pixel = npixel;
    }
  }

  // and now we can calculate boundingbox
  AlwaysAssert(pixel.ncolumn() > 0, AipsError);
  int x1min, x1max, x2min, x2max;
  x1min = x1max = pixel(0, 0);
  x2min = x2max = pixel(1, 0);
  for (int i = 0; i < pixel.ncolumn(); ++i) {
    x1min = min(x1min, (int)pixel(0, i));
    x1max = max(x1max, (int)pixel(0, i));
    x2min = min(x2min, (int)pixel(1, i));
    x2max = max(x2max, (int)pixel(1, i));
  }

  // just to be sure we add a border around it
  int b = 2*STEP;

  return IPosition(4, x1min-b, x1max+b, x2min-b, x2max+b);
}


/////////////////////////////////////
// Calculate bbox which encompasses bb1 and bb2
IPosition _join_bboxes(IPosition &bb1, IPosition &bb2)
{
  IPosition res(4,
		min(bb1[0], bb2[0]),
		max(bb1[1], bb2[1]),
		min(bb1[2], bb2[2]),
		max(bb1[3], bb2[3]));
    return res;
}


/////////////////////////////////////
// Calculate bbox which encompasses bb1 and bb2
// returns empty (bb.empty() == True) object if they don't intersect
IPosition _xsect_bboxes(IPosition &bb1, IPosition &bb2)
{
  IPosition res(4,
		max(bb1[0], bb2[0]),
		min(bb1[1], bb2[1]),
		max(bb1[2], bb2[2]),
		min(bb1[3], bb2[3]));
  if (res[1] - res[0] <= 0 || res[3] - res[2] <= 0)
    return IPosition();
  else
    return res;
}
