/*-------------------------------------------------------------------------*
| $Id: template-class.h,v 1.20 2007/06/13 09:41:37 bahren Exp           $ |
*-------------------------------------------------------------------------*
***************************************************************************
*   Copyright (C) 2007                                                    *
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

#include <ApplicationSupport/DataReaderTools.h>

// ==============================================================================
//
//  Assignment of pointer to object
//
// ==============================================================================

DataReader* DataReaderTools::assignFromRecord (GlishRecord &rec,
					       bool const &verbose)
{
  bool status;
  uint nofMissingInputs (0);
  DataReader *dr = new DataReader;

  // -- parse the record ---------------------------------------------

  String datatype;
  String basefile;
  uint blocksize;
  Vector<uint> antennas;
  Vector<bool> antennaSelection;
  Vector<uint> selectedAntennas;
  Vector<uint> selectedChannels;
  Vector<Double> adc2voltage;
  uint nyquistZone   (1);
  Double samplingRate (80e06);

  if (!getDatatype(datatype,rec)) {
    nofMissingInputs++;
    std::cerr << "[assignFromRecord] Missing field datatype" << std::endl;
  }
  
  if (!getBasefile(basefile,rec)) {
    nofMissingInputs++;
    std::cerr << "[assignFromRecord] Missing field basefile" << std::endl;
  }
  
  if (!getBlocksize(blocksize,rec)) {
    nofMissingInputs++;
    std::cerr << "[assignFromRecord] Missing field blocksize" << std::endl;
  }
  
  if (!getNyquistZone(nyquistZone,rec)) {
    nofMissingInputs++;
    std::cerr << "[assignFromRecord] Missing field nyquistZone" << std::endl;
  }
  
  if (!getSamplingRate(samplingRate,rec)) {
    nofMissingInputs++;
    std::cerr << "[assignFromRecord] Missing field samplingRate" << std::endl;
  }
  
  if (!getAntennas(antennas,rec)) {
    nofMissingInputs++;
    std::cerr << "[assignFromRecord] Missing field antennas" << std::endl;
  }
  
  if (!getAntennaSelection(antennaSelection,rec)) {
    nofMissingInputs++;
    std::cerr << "[assignFromRecord] Missing field antennaSelection" << std::endl;
  }
  
  if (!getSelectedAntennas(selectedAntennas,rec)) {
    nofMissingInputs++;
    std::cerr << "[assignFromRecord] Missing field selectedAntennas" << std::endl;
  }
  
  if (!getSelectedChannels(selectedChannels,rec)) {
    nofMissingInputs++;
    std::cerr << "[assignFromRecord] Missing field selectedChannels" << std::endl;
  }
  
  if (!getAdc2voltage(adc2voltage,rec)) {
    nofMissingInputs++;
    std::cerr << "[assignFromRecord] Missing field adc2voltage" << std::endl;
  }

  Matrix<DComplex> fft2calfft (DataReaderTools::fft2calfft (rec));

  // -- feedback (optional) ------------------------------------------

  if (verbose) {
    cout << "[DataReaderTools::assignFromRecord]" << endl;
    cout << " - datatype                = " << datatype            << endl;
    cout << " - basefile                = " << basefile            << endl;
    cout << " - blocksize               = " << blocksize           << endl;
    cout << " - antennas                = " << antennas            << endl;
    cout << " - antenna selection       = " << antennaSelection    << endl;
    cout << " - selected antennas       = " << selectedAntennas    << endl;
    cout << " - shape(selectedChannels) = " << selectedChannels.shape()
	 << endl;
    cout << " - shape(adc2voltage)      = " << adc2voltage.shape()
	 << endl;
    cout << " - shape(fft2calfft)       = " << fft2calfft.shape()
	 << endl;
  }

  // -- Do NOT attempt creating an object, if required information were missing 
  //    in the input record; in this case just return a default DataReader
  //    object -- this will not do anything useful, but at least it won't break
  //    the rest of the code.
  
  if (nofMissingInputs > 0) {
    cerr << "[DataReaderTools::assignFromRecord]" << endl;
    cerr << " - The Glish record provided on input was incomplete; found "
	 << nofMissingInputs
	 << " missing input parameters!"
	 << endl;
  } else {
    if (datatype == "LopesEvent") {
      try {
	LopesEvent *le = new LopesEvent (basefile,
					 blocksize,
					 adc2voltage,
					 fft2calfft);
	dr = le;
      } catch (AipsError x) {
	cerr << "[DataReaderTools::assignFromRecord] LopesEvent: "
	     << x.getMesg() << endl;
      }
    }
    else if (datatype == "ITSCapture") {
      try {
	ITSCapture *capture = new ITSCapture (basefile,
					      blocksize,
					      adc2voltage,
					      fft2calfft);
	dr = capture;
      } catch (AipsError x) {
	cerr << "[DataReaderTools::assignFromRecord] ITSCapture: "
	     << x.getMesg() << endl;
      }
    }
    else if (datatype == "ITSBeam") {
      try {
	ITSBeam *beam = new ITSBeam (basefile,
				     blocksize,
				     adc2voltage,
				     fft2calfft);
	dr = beam;
      } catch (AipsError x) {
	cerr << "[DataReaderTools::assignFromRecord] ITSBeam: "
	     << x.getMesg() << endl;
      }
    }
    else if (datatype == "NDABeam") {
      try {
	NDABeam *beam = new NDABeam (basefile,
				     blocksize,
				     adc2voltage,
				     fft2calfft);
	dr = beam;
      } catch (AipsError x) {
	cerr << "[DataReaderTools::assignFromRecord] NDABeam: "
	     << x.getMesg() << endl;
      }
    }
    else if (datatype == "ITSCorrelation") {
    }
    else if (datatype == "nuMoon") {
    }
    else {
      cerr << "[DataReaderTools::assignFromRecord] Undefined data type!" << endl;
    }
    
    dr->setNyquistZone  (nyquistZone);
    dr->setSampleFrequency (samplingRate);

    status = dr->setAntennas(antennas,
			     antennaSelection);
    status = dr->setSelectedChannels(selectedChannels);
  }

  return dr;
}


// ==============================================================================
//
// Extraction of individual parameters
//
// ==============================================================================

bool DataReaderTools::getDatatype (String &datatype,
				   GlishRecord &rec)
{
  GlishArray gtmp;
  bool status (true);
  
  if (rec.exists("datatype")) {
    gtmp = rec.get("datatype");
    gtmp.get(datatype);
  } else {
    status = false;
  }
  
  return status;
}

// --------------------------------------------------------------------- basefile

bool DataReaderTools::getBasefile (String &basefile,
				   GlishRecord &rec)
{
  GlishArray gtmp;
  bool status (true);
  
  if (rec.exists("basefile")) {
    gtmp = rec.get("basefile");
    gtmp.get(basefile);
  } else {
    status = false;
  }
  
  return status;
}

// -------------------------------------------------------------------- blocksize

bool DataReaderTools::getBlocksize (uint &blocksize,
				    GlishRecord &rec)
{
  GlishArray gtmp;
  int tmp (0);
  bool status (true);
  
  if (rec.exists("blocksize")) {
    gtmp = rec.get("blocksize");
    gtmp.get(tmp);
  } else {
    status = false;
  }

  blocksize = uint(tmp);
  
  return status;
}

// ------------------------------------------------------------------ nyquistZone

bool DataReaderTools::getNyquistZone (uint &nyquistZone,
				      GlishRecord &rec)
{
  GlishArray gtmp;
  int tmp (0);
  bool status (true);
  
  if (rec.exists("nyquistZone")) {
    gtmp = rec.get("nyquistZone");
    gtmp.get(tmp);
  } else {
    status = false;
  }
  
  nyquistZone = uint(tmp);
  
  return status;
}

// ----------------------------------------------------------------- samplingRate

bool DataReaderTools::getSamplingRate (Double &samplingRate,
				       GlishRecord &rec)
{
  GlishArray gtmp;
  bool status (true);
  
  if (rec.exists("samplingRate")) {
    gtmp = rec.get("samplingRate");
    gtmp.get(samplingRate);
  } else {
    status = false;
  }
  
  return status;
}

// ------------------------------------------------------------------ adc2voltage

bool DataReaderTools::getAdc2voltage (Vector<Double> &adc2voltage,
				      GlishRecord &rec)
{
  GlishArray gtmp;
  bool status (true);
  
  if (rec.exists("adc2voltage")) {
    gtmp = rec.get("adc2voltage");
    gtmp.get(adc2voltage);
  } else {
    status = false;
    adc2voltage.resize(1);
    adc2voltage = 1.0;
  }
  
  return status;
}

// ------------------------------------------------------------------- fft2calfft

Matrix<DComplex> DataReaderTools::fft2calfft (GlishRecord &rec)
{
  GlishArray gtmp;
  Matrix<DComplex> fft2calfft;
  
  if (rec.exists("fft2calfft")) {
    gtmp = rec.get("fft2calfft");
    gtmp.get(fft2calfft);
  } else {
    fft2calfft.resize(1,1);
    fft2calfft = 1.0;
  }
  
  return fft2calfft;
}

// --------------------------------------------------------------------- antennas

bool DataReaderTools::getAntennas (Vector<uint> &antennas,
				   GlishRecord &rec)
{
  GlishArray gtmp;
  Vector<int> tmp;
  bool status (true);
  
  if (rec.exists("antennas")) {
    gtmp = rec.get("antennas");
    gtmp.get(tmp);
  } else {
    status = false;
    //
    tmp.resize(1);
    tmp = 0;
  }

  /*
    Copy the values from the temporary array; this is not really elegant, but
    since Glish does not support "uint" as data type, we need the extra
    conversion step.
   */
  
  uint nelem (tmp.nelements());
  antennas.resize (nelem);

  for (uint n(0); n<nelem; n++) {
    antennas(n) = tmp(n);
  }
  
  return status;
}

// ------------------------------------------------------------- antennaSelection

bool DataReaderTools::getAntennaSelection (Vector<bool> &antennaSelection,
					   GlishRecord &rec)
{
  GlishArray gtmp;
  bool status (true);
  
  if (rec.exists("antennaSelection")) {
    gtmp = rec.get("antennaSelection");
    gtmp.get(antennaSelection);
  } else {
    status = false;
    //
    antennaSelection.resize(1);
    antennaSelection = true;
  }
  
  return status;
}

// ------------------------------------------------------------- selectedAntennas

bool DataReaderTools::getSelectedAntennas (Vector<uint> &selectedAntennas,
					   GlishRecord &rec)
{
  GlishArray gtmp;
  bool status (true);
  Vector<int> tmp;
  
  if (rec.exists("selectedAntennas")) {
    gtmp = rec.get("selectedAntennas");
    gtmp.get(tmp);
  } else {
    status = false;
    tmp.resize(1);
    tmp = 0;
  }
  
  uint nelem (tmp.nelements());
  selectedAntennas.resize (nelem);

  for (uint n(0); n<nelem; n++) {
    selectedAntennas(n) = tmp(n);
  }
  
  return status;
}

// ------------------------------------------------------------- selectedChannels

bool DataReaderTools::getSelectedChannels (Vector<uint> &selectedChannels,
					   GlishRecord &rec)
{
  GlishArray gtmp;
  bool status (true);
  Vector<int> tmp;
  
  if (rec.exists("selectedChannels")) {
    gtmp = rec.get("selectedChannels");
    gtmp.get(tmp);
  } else {
    status = false;
    tmp.resize(1);
    tmp = 0;
  }
  
  uint nelem (tmp.nelements());
  selectedChannels.resize (nelem);

  for (uint n(0); n<nelem; n++) {
    selectedChannels(n) = tmp(n);
  }
  
  return status;
}
