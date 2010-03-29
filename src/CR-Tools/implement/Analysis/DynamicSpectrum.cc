/***************************************************************************
 *   Copyright (C) 2006                                                    *
 *   Lars B"ahren (bahren@astron.nl)                                       *
 *   Copyright (C) 2009                                                    *
 *   Sander ter Veen (s.terveen@astro.ru.nl)                               *        
 *
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

#include "DynamicSpectrum.h"

namespace CR {  // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================

  //_____________________________________________________________________________
  //                                                              DynamicSpectrum
  
  DynamicSpectrum::DynamicSpectrum ()
  {
    dynamicSpectrum_p.resize(1,1);
  }
  
  //_____________________________________________________________________________
  //                                                              DynamicSpectrum
  
  /*!
    \param crval -- [freq,time] 
    \param cdelt -- [freq,time] 
    \param units -- [freq,time] 
  */
  DynamicSpectrum::DynamicSpectrum (const Vector<double>& crval,
				    const Vector<double>& cdelt,
				    const Vector<String>& units)
  {
    dynamicSpectrum_p.resize(1,1);

    setFrequencyAxis (crval(0),cdelt(0),units(0));
    setTimeAxis      (crval(1),cdelt(1),units(1));
  }
  
  //_____________________________________________________________________________
  //                                                              DynamicSpectrum
  
  /*!
    \param obsInfo  -- 
    \param timeAxis -- 
    \param freqAxis -- 
  */
  DynamicSpectrum::DynamicSpectrum (casa::ObsInfo obsInfo,
				    casa::LinearCoordinate timeAxis,
				    casa::SpectralCoordinate freqAxis)
  {
    obsInfo_p  = obsInfo;
    timeAxis_p = timeAxis;
    freqAxis_p = freqAxis;
  }
  
  //_____________________________________________________________________________
  //                                                              DynamicSpectrum
  
  /*!
    \param ts2       -- LOFAR_TBB timeseries
    \param nrblocks  -- Number of timesteps in the spectrum (optional, default 500)
    \param antennanr -- Antennanr from the dataset used (optional, default 0)
  */
  DynamicSpectrum::DynamicSpectrum (CR::LOFAR_TBB ts2,
				    int nrblocks,
				    int antennanr)
  {
    //Default values, to be adjusted:
    //int antennanr = 0;
    //int nrblocks = 500;
    
    // Make the dynamic spectrum
    const double alphaHanning = 0.5;
    cout << "Hanning filter set to " << alphaHanning << endl;
    ts2.setHanningFilter(alphaHanning);
    int maxnrblocks = (int) ts2.data_length()[antennanr]/ts2.blocksize();
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
    //cout << freqs(0) << std::endl;
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
  
  //_____________________________________________________________________________
  //                                                              DynamicSpectrum
  
  /*!
    \param other -- Another DynamicSpectrum object from which to create this
           new one.
  */
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

  // ------------------------------------------------------------------- totalPower
  
  Vector<double> DynamicSpectrum::totalPower ()
  {
    casa::IPosition shape (dynamicSpectrum_p.shape());
    Vector<double> totalPower (shape(0));

    /* Collapse the dynamic spectrum along the time-axis */
	// indices can be confusing but it works  
    for (int freq(0); freq<shape(0); freq++) {
		double tSum = 0;
		for( int time(0); time<shape(1); time++) {
			tSum += dynamicSpectrum_p(freq,time)*dynamicSpectrum_p(freq,time);
		}
		totalPower(freq) = fabs(tSum);
    }
    
    // return the result
    return totalPower;
  }

  // -------------------------------------------------------------- averageSpectrum
  
  Vector<double> DynamicSpectrum::averageSpectrum ()
  {
    casa::IPosition shape (dynamicSpectrum_p.shape());
    Vector<double> averageSpectrum (shape(1));

    /* Collapse the dynamic spectrum along the frequency axis */
	// indices can be confusing but it works  
    for (int n(0); n<shape(1); n++) {
		double fSum = 0;
		for( int freq(0); freq<shape(0); freq++){
			fSum += dynamicSpectrum_p(freq,n)*dynamicSpectrum_p(freq,n);
		}
			averageSpectrum(n) = abs(fSum); //sum(dynamicSpectrum_p.row(n)
			       //*dynamicSpectrum_p.row(n)));
    }
    
    // return the result
    return averageSpectrum;
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
    /*
     * Convert to floats and take the logarithm. If that value is zero, replace
     * it with the minimumvalue.
     */
    //float minimum = (float) min(dynamicSpectrum_p);
    casa::Matrix<float> dynamicSpectrum_float(dynamicSpectrum_p.shape());

    for (uint j=0; j<dynamicSpectrum_float.ncolumn(); j++) {
      for (uint i=0; i<dynamicSpectrum_float.nrow(); i++) {
	dynamicSpectrum_float(i,j) = (float) log(dynamicSpectrum_p(i,j));
      }      
    }
    
    casa::IPosition shape = dynamicSpectrum_float.shape();
    int status            = 0;
    long naxis            = shape.nelements();
    uint nelements        = dynamicSpectrum_float.nrow() * dynamicSpectrum_float.ncolumn();
    long firstelement = 1;
    long naxes[2];
    char *ctype[2];
    // char *cunit[] = {"sec","Hz"};
    // get the incrementvalues and units for the axes 
    std::string cd1(casa::String::toString(timeAxis_p.increment()(0)));
    std::string cd2(casa::String::toString(freqAxis_p.increment()(0)));

    naxes[0] = shape(0);
    naxes[1] = shape(1);
    ctype[0] = "TIME";
    ctype[1] = "FREQ";
    
    std::string tunit(timeAxis_p.worldAxisUnits()(0));
    std::string funit(freqAxis_p.worldAxisUnits()(0));
    std::string fnul(casa::String::toString(freqAxis_p.referenceValue()(0)));
    
    // copy the values to char*, as it's the only thing CFITSIO can handle;
    cout << "timeunit: " << tunit << "   frequnit: " << funit << std::endl;
    char *cunit0, *cunit1, *cd11, *cd12, *cval1;
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
    
    cval1 = new char[fnul.length()+1];
    fnul.copy(cval1, string::npos);
    cval1[fnul.length()] = '\0';
    
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
	  //std::cout << " -- Set the keywords for the coordinate type" << std::endl;
	  ffukys (fptr, "CTYPE1", ctype[0], "Time axis", &status);
	  ffukys (fptr, "CTYPE2", ctype[1], "Frequency axis", &status);
	  
	  //std::cout << " -- Set the keywords for the units on the coordinate axes"<< std::endl;
	  ffukys (fptr, "CUNIT1", cunit0, "Time axis units", &status);
	  ffukys (fptr, "CUNIT2", cunit1, "Frequency axis units", &status);
	  
	  ffukys (fptr, "CD1_1", cd11, "Time axis scaling", &status);
	  ffukys (fptr, "CD2_2", cd12, "Frequency axis scaling", &status);
	  
	  ffukys (fptr, "CRVAL2", cval1, "Frequency start", &status);
	  
	  //ADD start value CRVAL1 by start pixel CRPIX1 to complete the picture. This is needed for the second nyquist zone
	  //CRVAL = (PIXEL - CRPIX1)*CD1_1 + CRVAL1
	  //


	  

	  
	  //Add frequency spectrum
	  Vector<double> avSpectrum = log(averageSpectrum());
	  
	  casa::IPosition shape_F = avSpectrum.shape();
	  long naxis_F            = shape_F.nelements();
	  uint nelements_F        = avSpectrum.nelements();
	  long naxes_F[2]         = { shape_F(0), shape_F(1)};
	  long firstelement_F = 1; 
	  
	  casa::Bool storage_F;
	  casa::Double* datapointer_F = avSpectrum.getStorage(storage_F);
	  
	  fits_create_img (fptr, DOUBLE_IMG, naxis_F, naxes_F, &status);
	  fits_write_img (fptr, TDOUBLE, firstelement_F, nelements_F, datapointer_F, &status);
	  avSpectrum.putStorage(datapointer_F, storage_F);	  

	  
	  

	  ffukys (fptr, "CTYPE1", ctype[1], "Frequency axis", &status);
	  ffukys (fptr, "CUNIT1", cunit1, "Frequency axis units", &status);
	  ffukys (fptr, "CD1_1", cd12, "Frequency axis scaling", &status);
      ffukys (fptr, "BUNIT", "log(averageSpectrum)", "Power axis", &status);
      ffukys (fptr, "CRVAL1", cval1, "Frequency start", &status);
	  ffukys (fptr, "EXTNAME", "Average Spectrum (logscale)", "Frequency spectrum averaged over time in logarithmic scale", &status);

	  //Add powerspectrum
	  Vector<double> totPower = log(totalPower());
	  
	  casa::IPosition shape_P = totPower.shape();
	  long naxis_P            = shape_P.nelements();
	  uint nelements_P        = totPower.nelements();
	  long naxes_P[2]         = { shape_P(0), shape_P(1)};
	  long firstelement_P = 1; 
	  
	  casa::Bool storage_P;
	  casa::Double* datapointer_P = totPower.getStorage(storage_P);

	  
	  fits_create_img (fptr, DOUBLE_IMG, naxis_P, naxes_P, &status);
	  fits_write_img (fptr, TDOUBLE, firstelement_P, nelements_P, datapointer_P, &status);
	  totPower.putStorage(datapointer_P, storage_P);	 
	  ffukys (fptr, "CTYPE1", ctype[0], "Time axis", &status);
	  ffukys (fptr, "CUNIT1", cunit0, "Time axis units", &status);
	  ffukys (fptr, "CD1_1", cd11, "Time axis scaling", &status);
	  ffukys (fptr, "BUNIT", "log(totalPower)", "Power axis", &status);
	  ffukys (fptr, "EXTNAME", "Total Power (logscale)", "Total power for each time in logarithmic scale", &status);



	  fits_close_file (fptr, &status);  
    return true;
  }
#endif
  
}  // Namespace CR -- end
