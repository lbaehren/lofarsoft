/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2006                                                    *
 *   Lars Bahren (bahren@astron.nl)                                        *
 *   Andreas Nigl (anigl@astron.nl)                                        *
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

#include <Data/NDABeam.h>

// ==============================================================================
//
//  Construction
//
// ==============================================================================

NDABeam::NDABeam ()
{
  ITSMetadata meta;
  metadata_p = meta;
}

NDABeam::NDABeam (String const &metafile)
{
  setMetafile (metafile);
}

NDABeam::NDABeam (String const &metafile,
		  uint const &blocksize)
  : DataReader (blocksize)
{
  setMetafile (metafile);
}

NDABeam::NDABeam (String const &metafile,
		  uint const &blocksize,
		  Vector<Double> const &adc2voltage,
		  Matrix<DComplex> const &fft2calfft)
  : DataReader (blocksize,
		adc2voltage,
		fft2calfft)
{
  setMetafile (metafile);
}

NDABeam::NDABeam (NDABeam const &other)
{
  copy (other);
}

// ==============================================================================
//
//  Destruction
//
// ==============================================================================

NDABeam::~NDABeam ()
{
  destroy();
}

void NDABeam::destroy ()
{;}

// ==============================================================================
//
//  Operators
//
// ==============================================================================

NDABeam& NDABeam::operator= (NDABeam const &other)
{
  if (this != &other) {
    destroy ();
    copy (other);
  }
  return *this;
}

void NDABeam::copy (NDABeam const &other)
{
  metadata_p = other.metadata_p;
}

// ==============================================================================
//
//  Parameters
//
// ==============================================================================

void NDABeam::setMetafile (String const &metafile)
{
  bool status (true);
  
  // make a connection to the metafile and get its contents
  try {
    metadata_p.setMetafile (metafile);
  } catch (AipsError x) {
    cerr << "[NDABeam::setMetafile] " << x.getMesg() << endl;
    status = false;
  }
  
  // Set up the file streams by which to connect to the data stored on disk
  if (status) {
    status = setStreams ();
  }
}

// ==============================================================================
//
//  Methods
//
// ==============================================================================

// ------------------------------------------------------------------- setStreams

bool NDABeam::setStreams ()
{
  bool status (true);
  short var (0);

  uint blocksize (blocksize_p);
  Vector<uint> antennas (metadata_p.antennas());
  Vector<Double> adc2voltage (DataReader::ADC2Voltage());
  Matrix<DComplex> fft2calfft (DataReader::fft2calfft());
  Vector<String> filenames (metadata_p.datafiles(true));
  CR::DataIterator *iterator;
  
  /*
    Configure the DataIterator objects: for NDABeam data, the values are
    stored as short integer without any header information preceeding the
    data within the data file.
  */
  
  uint nofStreams (filenames.nelements());
  iterator = new CR::DataIterator[nofStreams];
  
  for (uint file (0); file<nofStreams; file++) {
    // data are stored as short integer
    iterator[file].setStepWidth(sizeof(var));
    // no header preceeding data
    iterator[file].setDataStart(0);
  }
  
  /*
    Setup of the conversion arrays
  */
  
  uint nofAntennas (antennas.nelements());
  uint fftLength (blocksize/2+1);
  IPosition shape (fft2calfft.shape());

  if (adc2voltage.nelements() != nofAntennas) {
    double weight (adc2voltage(0));
    adc2voltage.resize (nofAntennas);
    adc2voltage = weight;
  }
  
  if (uint(shape(0)) != fftLength ||
      uint(shape(1)) != nofAntennas) {
    fft2calfft.resize (fftLength,nofAntennas);
    fft2calfft = 1.0;
  }
  
  // -- call to DataReader::init(...) ------------------------------------------

  DataReader::init (blocksize,
		    antennas,
		    adc2voltage,
		    fft2calfft,
		    filenames,
		    iterator);
  DataReader::setNyquistZone (1);
  
  return status;
}

// --------------------------------------------------------------------------- fx

Matrix<Double> NDABeam::fx ()
{
  uint i (0);
  int errstat(0);
  short tmpData[blocksize_p];
  uint nofSelectedAntennas (DataReader::nofSelectedAntennas());
  // Data vector returned after reading is completed
  Matrix<Double> data (blocksize_p,
		      nofSelectedAntennas);
  
  // -----------------------------------------------------------------
  // Go to the position in the file, from to start reading data
  
  for (uint file (0); file<nofSelectedAntennas; file++) {
    
    try{
      fileStream_p[selectedAntennas_p(file)].seekg(iterator_p[selectedAntennas_p(file)].position(), ios::beg);
    } catch (AipsError x) {
      cerr << "[NDABeam::fx] " << x.getMesg() << endl;
      errstat = 1;
    }
    
    try{
      fileStream_p[selectedAntennas_p(file)].read ((char*)tmpData,
			       sizeof (short)*blocksize_p);
    } catch (AipsError x) {
      errstat = 2;
      cerr << "[NDABeam::fx] " << x.getMesg() << endl;
    }
    
    if (errstat == 0) {
      for (i=0; i<blocksize_p; i++) {
	data(i,file) = tmpData[i];
      }
    } else {
      data = 0.0;
    }
  }

  return data;
}
