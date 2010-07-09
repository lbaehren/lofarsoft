/*-------------------------------------------------------------------------*
 | $Id::    NewClass.cc 4824 2010-05-06 09:07:23Z baehren                $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2010                                                    *
 *   Andreas Horneffer (A.Horneffer@astro.ru.nl)                           *
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

#include <Bindings/pycr_IO.h>

#include <Bindings/pycr_numpycasa.h>
#include <IO/LopesEventIn.h>
#include <IO/LOFAR_TBB.h>
#include <Data/tbbctlIn.h>

namespace CR { // Namespace CR -- begin
  namespace PYCR { // Namespace PYCR -- begin

    // --------------------------------------------------------------- CRFileOpen
    DataReader & CRFileOpen(std::string filename) { 
      std::string ext, filetype;
      int size=filename.size();
      int pos=filename.rfind('.',size);
      pos++;
      if (pos < size) {
	ext = filename.substr(pos,size-pos);
      };
      if (ext=="event") {
	filetype = "LOPESEvent";
      } else if (ext=="h5") {
	filetype = "LOFAR_TBB";
      } else if (ext=="dat") {
	filetype = "tbbctl";
      }
      return CRFileOpenType(filename, filetype);
    };
    
    DataReader & CRFileOpenType(std::string Filename, std::string filetype) { 
      bool opened;
      //Create the a pointer to the DataReader object and store the pointer
      CR::DataReader* drp;
      if (filetype == "LOPESEvent") {
	CR::LopesEventIn* lep = new CR::LopesEventIn;
	opened=lep->attachFile(Filename);
	drp=lep;
	cout << "Opening LOPES File="<<Filename<<endl; drp->summary();
      } else if (filetype == "LOFAR_TBB") {
	drp = new CR::LOFAR_TBB(Filename,1024);
	opened= drp != NULL;
	cout << "Opening LOFAR File="<<Filename<<endl; drp->summary();
      } else if (filetype == "tbbctl") {
	CR::tbbctlIn* tbbp = new CR::tbbctlIn;
	opened= tbbp->attachFile(casa::Vector<casa::String>(1,casa::String(Filename)));
	drp = tbbp;
	cout << "Opening tbbctl-dump File="<<Filename<<endl; drp->summary();
      } else {
	cout << "CR::PYCR::CRFileOpen: Error: Unknown Filetype = " << filetype  << ", Filename=" << Filename << endl;
	opened=false;
      }
      if (!opened){
	cout << "CR::PYCR::CRFileOpen: Error: Opening file " << Filename << " failed." << endl;
	CR::LopesEventIn* lep = new CR::LopesEventIn; //Make a dummy data reader ....
	drp=lep;
      };
      return *drp;
    };

    // --------------------------------------------------------------- CRsummary
    void CRsummary(DataReader & dr) {
      dr.summary();
    }
    
    // --------------------------------------------------------------- CRFileRead
    bool CRFileRead(DataReader &dr, std::string Datatype, bpl::numeric::array &pydata ){
      for (int i=0; Datatype[i]; i++) {
	Datatype[i]= tolower(Datatype[i]);
      };
      if (Datatype == "time"){
	casa::Vector<double> data;
 	casa::IPosition outshape(1,dr.blocksize());
 	casaFromNumpyResize(pydata, data, outshape);
 	dr.timeValues(data);
      } else if (Datatype == "frequency"){
	casa::Vector<double> data;
	casa::IPosition outshape(1,dr.fftLength());
	casaFromNumpyResize(pydata, data, outshape);
	data = dr.frequencyValues(); //this should do a copy...
      } else if (Datatype == "fx"){
	casa::Matrix<double> data;
	casa::IPosition outshape(2,dr.blocksize(),dr.nofSelectedAntennas());
	casaFromNumpyResize(pydata, data, outshape);
	dr.fx(data);
      } else if (Datatype == "voltage"){
	casa::Matrix<double> data;
	casa::IPosition outshape(2,dr.blocksize(),dr.nofSelectedAntennas());
	casaFromNumpyResize(pydata, data, outshape);
	dr.voltage(data);
      } else if (Datatype == "fft"){
	casa::Matrix< std::complex<double> > data;
	casa::IPosition outshape(2,dr.fftLength(),dr.nofSelectedAntennas());
	casaFromNumpyResize(pydata, data, outshape);
	dr.fft(data);
      } else if (Datatype == "calfft"){
	casa::Matrix< std::complex<double> > data;
	casa::IPosition outshape(2,dr.fftLength(),dr.nofSelectedAntennas());
	casaFromNumpyResize(pydata, data, outshape);
	dr.calfft(data);
      } else {
	std::string fields;
	fields = fields + "help" 
	  + ", " + "Time" + ", " + "Frequency"
	  + ", " + "Fx" + ", " + "Voltage"
	  + ", " + "FFT" + ", " + "CalFFT";
	std::cout << "CR::PYCR::CRFileRead: Datatype=" << Datatype << " is unknown or not yet implemented!" << std::endl;
	std::cout << "    Available Datatypes are: " << fields << std::endl;
      };
      
      return true;
    }
    
    
    // -------------------------------------------------------- CRFileGetParameter
    bpl::object CRFileGetParameter(DataReader &dr, std::string key){
      for (int i=0; key[i]; i++) {
	key[i]= tolower(key[i]);
      };
      // scalar values from methods
      if (key== "nofantennas") {
	return bpl::object(dr.nofAntennas());
      } else if (key== "nofselectedchannels") {
	return bpl::object(dr.nofSelectedChannels());
      } else if (key== "nofselectedantennas") {
	return bpl::object(dr.nofSelectedAntennas());
      } else if (key== "blocksize") {
	return bpl::object(dr.blocksize());
      } else if (key== "fftlength") {
	return bpl::object(dr.fftLength());
      } else if (key== "block") {
	return bpl::object(dr.block());
      } else if (key== "stride") {
	return bpl::object(dr.stride());
      } else if (key== "nyquistzone") {
	return bpl::object(dr.nyquistZone());
      } else if (key== "sampleinterval") {
	return bpl::object(dr.sampleInterval());
      } else if (key== "referencetime") {
	return bpl::object(dr.referenceTime());
      } else if (key== "samplefrequency") {
	return bpl::object(dr.sampleFrequency());
      } else if (key== "nofbaselines") {
	return bpl::object(dr.nofBaselines());
      // arrays from methods
      } else if (key== "antennas") {
       	return numpyFromCasa(dr.antennas());
      } else if (key== "selectedantennas") {
       	return numpyFromCasa(dr.selectedAntennas());
      } else if (key== "selectedchannels") {
       	return numpyFromCasa(dr.selectedChannels());
      } else if (key== "positions") {
       	return numpyFromCasa(dr.positions());
      } else if (key== "increment") {
       	return numpyFromCasa(dr.increment());
      } else if (key== "frequencyvalues") {
       	return numpyFromCasa(dr.frequencyValues());
      } else if (key== "frequencyrange") {
       	return numpyFromCasa(dr.frequencyRange());
      } else if (key== "shift") {
       	return numpyFromSTL(dr.shift());
      // scalar values from the header record
      } else if (key== "date") {
	cout << "CRFileGetParameter: date " << dr.headerRecord().asDouble("Date") << endl;
	return bpl::object(dr.headerRecord().asuInt("Date"));
      } else if (key== "observatory") {
	return bpl::object(dr.headerRecord().asString("Observatory"));
      } else if (key== "filesize") {
	cout << "CRFileGetParameter: filesize " << dr.headerRecord().asDouble("Filesize") << endl;
	return bpl::object(dr.headerRecord().asInt("Filesize"));
      } else if (key== "ddate") {
	return bpl::object(dr.headerRecord().asDouble("dDate"));
      } else if (key== "presync") {
	return bpl::object(dr.headerRecord().asInt("presync"));
      } else if (key== "tl") {
	return bpl::object(dr.headerRecord().asInt("TL"));
      } else if (key== "ltl") {
	return bpl::object(dr.headerRecord().asInt("LTL"));
      } else if (key== "eventclass") {
	return bpl::object(dr.headerRecord().asInt("EventClass"));
      } else if (key== "samplefreq") {
	return bpl::object(dr.headerRecord().asInt("SampleFreq"));
      } else if (key== "startsample") {
	return bpl::object(dr.headerRecord().asuInt("StartSample"));
      // array from the header record
      } else if (key== "antennaids") {
       	return numpyFromCasa(dr.headerRecord().asArrayInt("AntennaIDs"));
      } else if (key== "sample_offset") {
       	return numpyFromCasa(dr.headerRecord().asArrayInt("SAMPLE_OFFSET"));
      } else if (key== "sample_number") {
       	return numpyFromCasa(dr.headerRecord().asArrayuInt("SAMPLE_NUMBER"));
      } else if (key== "time") {
       	return numpyFromCasa(dr.headerRecord().asArrayuInt("TIME"));
      };
      // Generate help string with all the fields
      std::string fields("help");
      fields = fields + ", " + "keywords"
	// scalar values from methods
	+ ", " + "nofAntennas" + ", " + "nofSelectedChannels" 
	+ ", " + "nofSelectedAntennas" + ", " + "blocksize" 
	+ ", " + "fftLength" + ", " + "block" 
	+ ", " + "stride" + ", " + "nyquistZone" 
	+ ", " + "sampleInterval" + ", " + "referenceTime" 
	+ ", " + "sampleFrequency" + ", " + "nofBaselines"
	// arrays from methods
	+ ", " + "antennas" + ", " + "selectedAntennas" 
	+ ", " + "selectedChannels" + ", " + "positions" 
	+ ", " + "increment" + ", " + "frequencyValues" 
	+ ", " + "frequencyRange" + ", " + "shift"
	// scalar values from the header record
	+ ", " + "Date" + ", " + "Observatory" 
	+ ", " + "Filesize" + ", " + "dDate" 
	+ ", " + "presync" + ", " + "TL" 
	+ ", " + "LTL" + ", " + "EventClass" 
	+ ", " + "SampleFreq" + ", " + "StartSample" 
	// array from the header record
	+ ", " + "AntennaIDs" + ", " + "SAMPLE_OFFSET" 
	+ ", " + "SAMPLE_NUMBER" + ", " + "TIME"; 
      if (key== "keywords") {
       	return bpl::object(fields);
      };
      if (key== "help") {
	cout << "CRFileGetParameter" << " - available keywords: "<< fields <<endl;
       	return bpl::object(true);
      };
      // Default for unknown keyword:
      cout << "CRFileGetParameter" << " Unknown keyword: " << key << endl;
      cout << " -- available keywords: "<< fields <<endl;
      return bpl::object(false);
    }


    // -------------------------------------------------------- CRFileSetParameter
    bool CRFileSetParameter(DataReader &dr, std::string key, bpl::object &pyob) {
      for (int i=0; key[i]; i++) {
	key[i]= tolower(key[i]);
      };
      // scalar values from methods
      if (key == "block") {
	dr.setBlock( bpl::extract<int>(pyob) );
      } else if (key == "blocksize") {
	dr.setBlocksize( bpl::extract<int>(pyob) );
      } else if (key == "startblock") {
	dr.setStartBlock( bpl::extract<int>(pyob) );
      } else if (key == "stride") {
	dr.setStride( bpl::extract<int>(pyob) );
      } else if (key == "sampleoffset") {
	dr.setSampleOffset( bpl::extract<int>(pyob) );
      } else if (key == "nyquistzone") {
	dr.setNyquistZone( bpl::extract<int>(pyob) );
      } else if (key == "shift") {
	dr.setShift( bpl::extract<int>(pyob) );
      } else if (key == "referencetime") {
	dr.setReferenceTime( bpl::extract<double>(pyob) );
      } else if (key == "samplefrequency") {
	dr.setSampleFrequency( bpl::extract<double>(pyob) );
      } else if (key=="shiftvector") {	
	//cout << "CRFileSetParameter:shiftvector: data:" <<STLVecFromPyob<int>(pyob) << endl; 
	dr.setShift(STLVecFromPyob<int>(pyob));
      } else if (key=="selectedantennas") {
	//cout << "CRFileSetParameter:selectedantennas: data:" << casaFromPyob<uint>(pyob) << endl; 
	dr.setSelectedAntennas(casaFromPyob<uint>(pyob));
      } else {
	std::string fields;
	fields = fields + "help" 
	  + ", " + "Block" + ", " + "Blocksize"
	  + ", " + "StartBlock" + ", " + "Stride"
	  + ", " + "SampleOffset" + ", " + "NyquistZone"
	  + ", " + "Shift" + ", " + "ReferenceTime"
	  + ", " + "SampleFrequency" + ", " + "ShiftVector"
	  + ", " + "SelectedAntennas" + ", " + "";	
	if (key== "help") {
	  cout << "CRFileSetParameter" << " - available keywords: "<< fields <<endl;
	} else {
	  // Default for unknown keyword:
	  cout << "CRFileSetParameter" << " Unknown keyword: " << key << endl;
	  cout << " -- available keywords: "<< fields <<endl;
	};
      };
      return true;
    }
  
    // -------------------------------------------------------- export_DataReader
    void export_DataReader() {  
      bpl::class_<CR::DataReader>("DataReader")
	.def("summary",&CR::PYCR::CRsummary)
	.def("crRead",&CR::PYCR::CRFileRead)
	.def("crFileGetParameter",&CR::PYCR::CRFileGetParameter)
	.def("crFileSetParameter",&CR::PYCR::CRFileSetParameter)
	;   
      def("crFileOpen", &CR::PYCR::CRFileOpen,bpl::return_internal_reference<>());
      def("crFileOpen", &CR::PYCR::CRFileOpenType,bpl::return_internal_reference<>());
      
    }
    
  } // Namespace PYCR -- end
  
} // Namespace CR -- end
