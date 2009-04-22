/***************************************************************************
 *   Copyright (C) 2006                                                    *
 *   Lars B"ahren (bahren@astron.nl)                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

/* $Id$*/

#include "DynamicSpectrum.h"

namespace CR {  // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  // ------------------------------------------------------------ DynamicSpectrum
  
  DynamicSpectrum::DynamicSpectrum ()
  {
    dynamicSpectrum_p.resize(1,1);
  }
  
  // ------------------------------------------------------------ DynamicSpectrum
  
  DynamicSpectrum::DynamicSpectrum (const Vector<double>& crval,
				    const Vector<double>& cdelt,
				    const Vector<String>& units)
  {
    dynamicSpectrum_p.resize(1,1);

    setFrequencyAxis (crval(0),cdelt(0),units(0));
    setTimeAxis      (crval(1),cdelt(1),units(1));
  }
  
  // ------------------------------------------------------------ DynamicSpectrum
  
  DynamicSpectrum::DynamicSpectrum (casa::ObsInfo obsInfo,
				    casa::LinearCoordinate timeAxis,
				    casa::SpectralCoordinate freqAxis)
  {
    obsInfo_p  = obsInfo;
    timeAxis_p = timeAxis;
    freqAxis_p = freqAxis;
  }
	
  // ------------------------------------------------------------ DynamicSpectrum
	
  DynamicSpectrum::DynamicSpectrum (CR::LOFAR_TBB ts2, int nrblocks, int antennanr)
	{
		//Default values, to be adjusted:
		//int antennanr = 0;
		//int nrblocks = 500;

		// Make the dynamic spectrum
		const double alphaHanning = 0.5;
		cout << "Hanning filter set to " << alphaHanning << endl;
		ts2.setHanningFilter(alphaHanning);
		int maxnrblocks = (int) ts2.data_length()(antennanr)/ts2.blocksize();
		if(nrblocks == 0 || nrblocks > maxnrblocks){
			nrblocks = maxnrblocks;
		}
		int fftsize = (int) ts2.fftLength();
		dynamicSpectrum_p.resize(nrblocks,fftsize);
		for(int blocknr=0; blocknr < nrblocks; blocknr++){
			ts2.setBlock(blocknr);
			Matrix<DComplex> ft = ts2.calfft();
			Matrix<Double> absft = amplitude(ft);
			//cout << setprecision(8) << absft.column(0) << endl ;
			dynamicSpectrum_p.row(blocknr) = absft.column(antennanr);
		}

		
		// Create time and frequency axes.
		casa::Vector<double> freqs = ts2.frequencyValues(antennanr); 			
		double samplefreq = ts2.sample_frequency_value()(antennanr);
		string frequnit = ts2.sample_frequency_unit()(antennanr);
		double timestep = 1 / samplefreq * ts2.blocksize();
		string timeunit;
		if(frequnit == "MHz"){
			timeunit = "us";
		} else if(frequnit == "Hz"){
			timeunit = "s";
		} else if(frequnit == "kHz"){
			timeunit = "ms";
		} else{ timeunit = "1/" + frequnit;}
		
		
		setFrequencyAxis( freqs(0), freqs(1)-freqs(0), frequnit);
		setTimeAxis( 0, timestep, timeunit);

        // set outputfilename (to be added)

	}
  
  // ------------------------------------------------------------ DynamicSpectrum
  
  DynamicSpectrum::DynamicSpectrum (DynamicSpectrum const& other)
  {
    copy (other);
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  DynamicSpectrum::~DynamicSpectrum ()
  {
    destroy();
  }
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================

  // ------------------------------------------------------------------ operator=
  
  DynamicSpectrum &DynamicSpectrum::operator= (DynamicSpectrum const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }
  
  // ----------------------------------------------------------------------- copy

  void DynamicSpectrum::copy (DynamicSpectrum const& other)
  {
    dynamicSpectrum_p.resize(other.dynamicSpectrum_p.shape());
    dynamicSpectrum_p = other.dynamicSpectrum_p;
    //
    obsInfo_p  = other.obsInfo_p;
    timeAxis_p = other.timeAxis_p;
    freqAxis_p = other.freqAxis_p;
  }
  
  // -------------------------------------------------------------------- destroy

  void DynamicSpectrum::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================

  // ---------------------------------------------------------------- setTimeAxis
  
  void DynamicSpectrum::setTimeAxis (const double& crval,
				     const double& cdelt,
				     const String& unit)
  {
    Quantum<double> referenceValue (crval,unit);
    Quantum<double> increment (cdelt,unit);
    
    setTimeAxis (referenceValue,
		 increment);
  }
  
  // ---------------------------------------------------------------- setTimeAxis
  
  void DynamicSpectrum::setTimeAxis (const Quantum<double>& crval,
				     const Quantum<double>& cdelt)
  {
    Vector<Quantum<double> > refVal (1);
    Vector<Quantum<double> > increment (1);
    Vector<double> refPix (1,0.0);
    casa::Matrix<double> pc(1,1);
    Vector<String> name(1);
    
    // set proper values
    refVal(0)     = crval;
    increment(0)  = cdelt;
    pc            = 0;
    pc.diagonal() = 1.0;
    name          = "time";
    
    casa::LinearCoordinate lc (name,
			       refVal,
			       increment,
			       pc,
			       refPix);
    timeAxis_p = lc;
  }
  
  // ----------------------------------------------------------- setFrequencyAxis
  
  void DynamicSpectrum::setFrequencyAxis (const double& crval,
					  const double& cdelt,
					  const String& unit)
  {
    Quantum<double> referenceValue (crval,unit);
    Quantum<double> increment (cdelt,unit);
    
    setFrequencyAxis (referenceValue,
		      increment);
  }
  
  // ----------------------------------------------------------- setFrequencyAxis

  void DynamicSpectrum::setFrequencyAxis (const Quantum<double>& crval,
					  const Quantum<double>& cdelt)
  {
    double crpix (0.0);
    Quantum<double> restfreq (0.0,"Hz");
    
    casa::SpectralCoordinate sc (casa::MFrequency::TOPO,
				 crval,
				 cdelt,
				 crpix,
				 restfreq);
    freqAxis_p = sc;
  }
  
	
  // ----------------------------------------------------------- setSpectrum
	
	void DynamicSpectrum::setSpectrum ( casa::Matrix<double> const &data)
	{
		dynamicSpectrum_p.resize(data.shape());
		dynamicSpectrum_p = data;
		
    }
	
  // ----------------------------------------------------------- setFilename 	
	
	void DynamicSpectrum::setFilename ( std::string &filename )
	{
		filename_p = filename;
	}
	
  // ----------------------------------------------------------- coordinateSystem

  casa::CoordinateSystem DynamicSpectrum::coordinateSystem ()
  {
    casa::CoordinateSystem cs;
    
    //   cs.setObsInfo (csys_p.obsInfo());
    cs.addCoordinate (freqAxis_p);
    cs.addCoordinate (timeAxis_p);
    
    return cs;
  }
  
	
  // -------------------------------------------------------------------- summary

  void DynamicSpectrum::summary (std::ostream &os)
  {
    os << "[DynamicSpectrum] Summary of object properties" << std::endl;
    os << "-- Shape = " << dynamicSpectrum_p.shape() << std::endl;
  }
  
  // ==============================================================================
  //
  //  Methods
  //
  // ==============================================================================

  // -------------------------------------------------------------- averageSpectrum
  
  Vector<double> DynamicSpectrum::averageSpectrum ()
  {
    casa::IPosition shape (dynamicSpectrum_p.shape());
    Vector<double> averageSpectrum (shape(0));

    /* Collapse the dynamic spectrum along the time-axis */
    for (int freq(0); freq<shape(0); freq++) {
		double fSum = 0;
		for( int time(0); time<shape(1); time++) {
			fSum += dynamicSpectrum_p(freq,time)*dynamicSpectrum_p(freq,time);
		}
		averageSpectrum(freq) = fabs(fSum);//sum(dynamicSpectrum_p.column(freq)
										// *dynamicSpectrum_p.column(freq)));
    }
    
    // return the result
    return averageSpectrum;
  }

  // ------------------------------------------------------------------- totalPower
  
  Vector<double> DynamicSpectrum::totalPower ()
  {
    casa::IPosition shape (dynamicSpectrum_p.shape());
    Vector<double> totalPower (shape(1));

    /* Collapse the dynamic spectrum along the frequency axis */
    for (int n(0); n<shape(1); n++) {
		double tSum = 0;
		for( int freq(0); freq<shape(0); freq++){
			tSum += dynamicSpectrum_p(freq,n)*dynamicSpectrum_p(freq,n);
		}
			totalPower(n) = abs(tSum); //sum(dynamicSpectrum_p.row(n)
			       //*dynamicSpectrum_p.row(n)));
    }
    
    // return the result
    return totalPower;
  }

  // ---------------------------------------------------------------------- toImage

  bool DynamicSpectrum::toImage ()
  {
    return true;
  }

  // ----------------------------------------------------------------------- toFITS

#ifdef HAVE_CFITSIO
  bool DynamicSpectrum::toFITS ()
  {
	  // convert to floats and take the logarithm. If that value is zero, replace it woth the minimumvalue.
	  //float minimum = (float) min(dynamicSpectrum_p);
	  casa::Matrix<float> dynamicSpectrum_float;
	  dynamicSpectrum_float.resize(dynamicSpectrum_p.shape());
	  for (uint j=0; j<dynamicSpectrum_float.ncolumn(); j++) {
		  for (uint i=0; i<dynamicSpectrum_float.nrow(); i++) {
			  dynamicSpectrum_float(i,j) = (float) log(dynamicSpectrum_p(i,j));
		  }      
	  }
	  

    casa::IPosition shape = dynamicSpectrum_float.shape();
    int status            = 0;
    long naxis            = shape.nelements();
	uint nelements        = dynamicSpectrum_float.nrow() * dynamicSpectrum_float.ncolumn();
    long naxes[2]         = { shape(0), shape(1)};
	long firstelement = 1;
    char *ctype[] = {"TIME","FREQ"};
	 // char *cunit[] = {"sec","Hz"};
	  std::string cd1(casa::String::toString(timeAxis_p.increment()(0)));
	  //casa::String timeunit = casa::String::toString(timeAxis_p.worldAxisUnits()(0));
	  std::string cd2(casa::String::toString(freqAxis_p.increment()(0)));
	  //casa::String frequnit = freqAxis_p.worldAxisUnits()(0)
	  //frequnit.append(" ");
	  //frequnit.append(freqAxis_p.worldAxisUnits()(0));
	  std::string tunit(timeAxis_p.worldAxisUnits()(0));
	  std::string funit(freqAxis_p.worldAxisUnits()(0));
	  
	  

	  	 //char *cunit[] = {tunit.c_str(), funit.c_str()};
	 // cunit[1] = frequnit.c_str();
	  cout << "timeunit: " << tunit << "   frequnit: " << funit << std::endl;
	  char *cunit0, *cunit1, *cd11, *cd12;
	  cunit0 = new char[tunit.length()+1];
	  tunit.copy(cunit0, string::npos);
	  cunit0[tunit.length()] = '\0';
	  cunit1 = new char[funit.length()+1];
	  funit.copy(cunit1, string::npos);
	  cunit1[funit.length()] = '\0';
	  cd11 = new char[cd1.length()+1];
	  cd1.copy(cd11, string::npos);
	  cd11[cd1.length()]='\0';
	  cd12 = new char[cd2.length()+1];
	  cd2.copy(cd12, string::npos);
	  cd12[cd2.length()]='\0';
	  
	  std::string outfile;
    fitsfile *fptr;
	  std::cout << "shape: " << shape << std::endl;
	  std::cout << "nelements: " << nelements << std::endl;  
    /* Adjust the filename such that existing data are overwritten */
    outfile = "!" + filename_p + ".fits";
	  std::cout << "write to: " << outfile.c_str() << std::endl;
    /* Create FITS file on disk */
    fits_create_file (&fptr, outfile.c_str(), &status);
	 		
    /* Create primary array to take up the image data */
    fits_create_img (fptr, FLOAT_IMG, naxis, naxes, &status);
	 
	  
	//NOTE: getStorage() is used to get a pointer to the actual data as the matrix also has MetaData. 
	casa::Bool storage;
	casa::Float* datapointer = dynamicSpectrum_float.getStorage(storage);
	  
	  
	fits_write_img (fptr, TFLOAT, firstelement, nelements, datapointer, &status);
    //clean up the storage
	dynamicSpectrum_float.putStorage(datapointer, storage);

 	//set keywords for the axis
	  std::cout << " -- Set the keywords for the coordinate type" << std::endl;
	  ffukys (fptr, "CTYPE1", ctype[0], "Time axis", &status);
	  ffukys (fptr, "CTYPE2", ctype[1], "Frequency axis", &status);
	  
	  std::cout << " -- Set the keywords for the units on the coordinate axes"
	  << std::endl;
	  ffukys (fptr, "CUNIT1", cunit0, "Time axis units", &status);
	  ffukys (fptr, "CUNIT2", cunit1, "Frequency axis units", &status);
	  
	  ffukys (fptr, "CD1_1", cd11, "Time axis scaling", &status);
	  ffukys (fptr, "CD2_2", cd12, "Frequency axis scaling", &status);
	  
	  //ADD start value CRVAL1 by start pixel CRPIX1 to complete the picture. This is needed for the second nyquist zone
	  //CRVAL = (PIXEL - CRPIX1)*CD1_1 + CRVAL1
	  //
	  fits_close_file (fptr, &status);  
    return true;
  }
#endif
  
}  // Namespace CR -- end
