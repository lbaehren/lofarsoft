/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2006                                                    *
 *   Lars Bahren (bahren@astron.nl)                                        *
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

#include <IO/ITS_Capture.h>

// ==============================================================================
//
//  Construction
//
// ==============================================================================

// ------------------------------------------------------------------ ITS_Capture

ITS_Capture::ITS_Capture ()
{
  ITSMetadata meta;
  metadata_p = meta;
}

// ------------------------------------------------------------------ ITS_Capture

ITS_Capture::ITS_Capture (String const &metafile)
{
  setMetafile (metafile);
}

// ------------------------------------------------------------------ ITS_Capture

ITS_Capture::ITS_Capture (String const &metafile,
			  uint const &blocksize)
  : DataReader (blocksize)
{
  setMetafile (metafile);
}

// ------------------------------------------------------------------ ITS_Capture

ITS_Capture::ITS_Capture (String const &metafile,
			  uint const &blocksize,
			  Vector<Double> const &adc2voltage,
			  Matrix<DComplex> const &fft2calfft)
  : DataReader (blocksize,
		adc2voltage,
		fft2calfft)
{
  setMetafile (metafile);
}

// ------------------------------------------------------------------ ITS_Capture

ITS_Capture::ITS_Capture (ITS_Capture const &other)
  : DataReader (other)
{
  copy (other);
}

// ==============================================================================
//
//  Destruction
//
// ==============================================================================

// ----------------------------------------------------------------- ~ITS_Capture

ITS_Capture::~ITS_Capture ()
{
  destroy();
}

// ---------------------------------------------------------------------- destroy

void ITS_Capture::destroy ()
{;}

// ==============================================================================
//
//  Operators
//
// ==============================================================================

ITS_Capture& ITS_Capture::operator= (ITS_Capture const &other)
{
  if (this != &other) {
    destroy ();
    copy (other);
  }
  return *this;
}

// ------------------------------------------------------------------------- copy

void ITS_Capture::copy (ITS_Capture const &other)
{
  metadata_p = other.metadata_p;
  datatype_p = other.datatype_p;
}

// ==============================================================================
//
//  Parameters
//
// ==============================================================================

void ITS_Capture::setMetafile (String const &metafile)
{
  bool status (true);
  
  // make a connection to the metafile and get its contents
  try {
    metadata_p.setMetafile (metafile);
  } catch (AipsError x) {
    cerr << "[ITS_Capture::setMetafile] " << x.getMesg() << endl;
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

bool ITS_Capture::setStreams ()
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
    Configure the DataIterator objects: for ITS_Capture data, the values are
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

//_______________________________________________________________________________
//                                                                             fx

/*!
  \return fx -- Raw ADC time series, [Counts]
*/
Matrix<Double> ITS_Capture::fx ()
{
  Matrix<Double> data (blocksize_p,
		       DataReader::nofSelectedAntennas());

  ITS_Capture::fx (data);

  return data;
}

//_______________________________________________________________________________
//                                                                             fx

/*!
  \retval fx -- Raw ADC time series, [Counts]
*/
void ITS_Capture::fx (Matrix<Double> &data)
{
  uint i (0);
  int errstat(0);
  short tmpData[blocksize_p];
  uint nofSelectedAntennas (DataReader::nofSelectedAntennas());
  casa::IPosition shape (2,blocksize_p,nofSelectedAntennas);

  /* Check the shape of the array returning the data */
  if (data.shape() != shape) {
    data.resize(shape);
  }
  
  // -----------------------------------------------------------------
  // Go to the position in the file, from to start reading data
  
  for (uint file (0); file<nofSelectedAntennas; file++) {
    
    try{
      fileStream_p[selectedAntennas_p(file)].seekg(iterator_p[selectedAntennas_p(file)].position(), ios::beg);
    } catch (AipsError x) {
      cerr << "[ITS_Capture::fx] " << x.getMesg() << endl;
      errstat = 1;
    }
    
    try{
      fileStream_p[selectedAntennas_p(file)].read ((char*)tmpData,
						   sizeof (datatype_p)*blocksize_p);
    } catch (AipsError x) {
      errstat = 2;
      cerr << "[ITS_Capture::fx] " << x.getMesg() << endl;
    }
    
    if (errstat == 0) {
      for (i=0; i<blocksize_p; i++) {
	data(i,file) = tmpData[i];
      }
    } else {
      data = 0.0;
    }
  }
}
