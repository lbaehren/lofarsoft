/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2005 by Sven Lafebre                                    *
 *   s.lafebre@astro.ru.nl                                                 *
 *                                                                         *
 *   Edited by: Jaap Kroes     J.Kroes@student.science.ru.nl               *
 *              Pim Schellart  P.Schellart@student.science.ru.nl           *
 *              Lars Baehren   baehren@astron.nl                           *
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

#include <sstream>
#include <fstream>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <Data/Data.h>
#include <Data/LopesEvent.h>

using namespace std;

namespace CR {  //  Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  // ----------------------------------------------------------------- LopesEvent
  
  LopesEvent::LopesEvent()
    : DataReader (1)
  {
    /*
      Assign some value to the internal variables, at least to indicate they 
      haven't been assigned a proper value yet
    */
    init ();
    
    /*
      Initialize the parameters required by the DataReader
    */
    setStreams ();
  }
  
  // ----------------------------------------------------------------- LopesEvent
  
  LopesEvent::LopesEvent (const char* name)
    : DataReader (1)
  {
    init (name);
  }
  
  // ----------------------------------------------------------------- LopesEvent
  
  LopesEvent::LopesEvent (string const &name,
			  uint const &blocksize)
    : DataReader (blocksize)
  {
    init (name.c_str());
  }
  
  // ----------------------------------------------------------------- LopesEvent
  
  LopesEvent::LopesEvent (string const &name,
			  uint const &blocksize,
			  Vector<Double> const &adc2voltage,
			  Matrix<DComplex> const &fft2calfft)
    : DataReader (blocksize,
		  adc2voltage,
		  fft2calfft)
  {
    init (name.c_str());
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  LopesEvent::~LopesEvent()
  {
    if (length_ > 0) {
      for (uint i = 0; i < length_; i ++) {
	delete data_[i];
      }
    }
    
    delete [] data_;
  }
  
  // ============================================================================
  //
  //  Initialization of internal parameters
  //
  // ============================================================================
  
  // ----------------------------------------------------------------------- init
  
  void LopesEvent::init ()
  {
    length_ = 1;
    
    data_    = new Data*[length_];
    data_[0] = new Data(1,1);
    
    filename_    = "unset";
    version_     = 0;
    timeJDR_     = 0;
    timeTL_      = 0;
    timeLTL_     = 0;
    observatory_ = 0;
    presync_     = 0;
    dataType_    = 0;
    evType_      = Unspecified;
    
    DataReader::setNyquistZone (2);
  }
  
  // ----------------------------------------------------------------------- init
  
  void LopesEvent::init (const char* name)
  {
    fstream handle;
    
    try {
      int handle_beg (0);
      int handle_end (0);
      
      openEvent(name, &handle, &handle_beg, &handle_end);
      readHeaderHere(&handle);
      readContentsHere(&handle, &handle_beg, &handle_end);
      handle.close();
      // DataReader data stream settings
      setStreams ();
    } catch (string s) {
      cerr << s << endl;
    }
    
    DataReader::setNyquistZone (2);
  }
  
  // ============================================================================
  //
  //  Initialize access to event file
  //
  // ============================================================================
  
  // ---------------------------------------------------------------- createEvent
  
  void LopesEvent::createEvent(const char* name,
			       fstream* handle) {
    // Create event on disk write only
    handle->open(name, ios::out | ios::binary);
    if (! *handle || *handle == NULL) {
      cerr << "Could not open file " << name << " for writing." << endl
	   << "Check permissions" <<endl;
    }
  }
  
  // ------------------------------------------------------------------ openEvent
  
  void LopesEvent::openEvent(const char* name,
			     fstream* handle,
			     int* handle_beg,
			     int* handle_end)
  {
    // Open event read & write
    handle->open(name, ios::in | ios::out | ios::binary);
    if (! *handle || *handle == NULL) {
      cerr << "Could not open file '" << name << "'." << endl;
      throw string("No such file.");
    }
    filename_ = (string)name;
    
    uint bytes;
    // Read supposed file size
    handle->read((char*)&bytes, sizeof(uint));
    
    // Get real file size
    handle->seekg(0, ios::end);
    *handle_end = handle->tellg();
    handle->seekg(0, ios::beg);
    *handle_beg = handle->tellg();
    
    int f_size = *handle_end - *handle_beg;
    if (f_size < 48) {
      cerr << "  ERROR: This file is too small for a Lopes event file." << endl;
      throw string("File too small.");
    }
    
    // Check if file size is correct
    if ((int)bytes != f_size) {
      cerr << "  WARNING: File size does not match." << endl;
    }
    
    // Skip first int (we have this already)
    handle->read((char*)&bytes, sizeof(uint));
    
    // Initialize data set length
    length_ = 0;
  }
  
  
  // ============================================================================
  //
  //  Read operations
  //
  // ============================================================================
  
  // ----------------------------------------------------------------- readHeader
  
  void LopesEvent::readHeader(const char* name)
  {
    fstream handle;
    
    try {
      int handle_beg, handle_end;
      
      openEvent(name, &handle, &handle_beg, &handle_end);
      readHeaderHere(&handle);
      handle.close();
    } catch (string s) {
      cerr << s << endl;
    }
    
  }
  
  // ------------------------------------------------------------- readHeaderHere

void LopesEvent::readHeaderHere(fstream* handle)
{  
  // Continue reading header
  handle->read((char*)&version(),   sizeof(uint));
  
  // This library reads and writes LOPES event files of version 1, 2 and 3.
  // If it's different, stop reading further.
  if (version() != 1 && version() != 2 && version() != 3) {
    cerr << "File version mismatch (" << version()
	 << "). Creating empty data set." << endl;
    return;
  }
  uint tmp;
  // Read header
  handle->read((char*)&timeJDR(),   sizeof(uint));
  handle->read((char*)&timeTL(),    sizeof(uint));
  handle->read((char*)&dataType(),  sizeof(uint));
  handle->read((char*)&tmp,         sizeof(uint));
  evType() = (EvType)tmp;
  handle->read((char*)&blocksize_,  sizeof(int));
  handle->read((char*)&presync(),   sizeof(uint));
  // Eventfile version check
  if (version() > 1) {
    handle->read((char*)&timeLTL(), sizeof(uint));
  } else {
    handle->seekg(4, ios::cur);
    timeLTL() = 0;
  }
  
  // Eventfile version check & skip empty positions for future header entrys
  if (version() > 2) {
    handle->read((char*)&observatory(), sizeof(uint));
    handle->seekg(8, ios::cur);
  } else {
    handle->seekg(12, ios::cur);
    // Set standard observatory number 0 (LOPES)
    observatory() = 0;
  }
}

// ------------------------------------------------------------- readContentsHere

void LopesEvent::readContentsHere (fstream* handle,
				   int* handle_beg,
				   int* handle_end)
{
  // Now start with the real data: first, let's see what's in the file
  uint c_id (0);
  uint c_len (0);
  bool cont (true);

  int start_dat = handle->tellg();

  length_ = 0;
  while (handle->tellg() < int(*handle_end - 8) && cont) {
    length_ ++;
    handle->read((char*)&c_id,  sizeof(uint));
    handle->read((char*)&c_len, sizeof(uint));
    uint t = handle->tellg();
    handle->seekg(sizeof(short)*c_len, ios::cur);
    if ((int)t == handle->tellg()) cont = false;
  }
  
  // Now we know how many data sets there are, make placeholders for them
  data_ = new Data*[length_];
  
  // Go back to start of first data set header
  handle->seekg(start_dat, ios::beg);
  
  // Start reading in data
  for (uint i = 0; i < length_; i ++) {
    handle->read((char*)&c_id,  sizeof(uint));
    handle->read((char*)&c_len, sizeof(uint));
    // Data is assumed to be 16 bit
    if (handle->tellg() <= int(*handle_end - sizeof(short)*c_len)) {
      data_[i] = new Data(c_len, c_id);
      handle->read((char*)dataSet(i)->data_, sizeof(short)*c_len);
    } else {
      cerr << "Data file ended unexpectedly." << endl;
      throw string("Unexpected end of data file.");
    }
  }
}

// ==============================================================================
//
//  Write operations
//
// ==============================================================================

// ------------------------------------------------------------------- writeEvent

void LopesEvent::writeEvent(const char* name)
{
  fstream handle;
  int handle_beg, handle_end;
  
  createEvent(name, &handle);
  
  writeHeaderHere(&handle, &handle_beg, &handle_end);
  writeContentsHere(&handle);
  
  handle.close();  
}

// ------------------------------------------------------------------ writeHeader

void LopesEvent::writeHeader(const char* name)
{
  fstream handle;
  int handle_beg, handle_end;
  
  openEvent(name, &handle, &handle_beg, &handle_end);
  
  // Go back to beginning of file
  handle.seekg(0, ios::beg);
  
  writeHeaderHere(&handle, &handle_beg, &handle_end);
  
  handle.close();
}

// -------------------------------------------------------------- writeHeaderHere

void LopesEvent::writeHeaderHere(fstream* handle,
				 int* handle_beg,
				 int* handle_end)
{
  // Calculate filesize
  uint f_size = *handle_end - *handle_beg;
  
  // Write header
  handle->write((char*)&f_size,  sizeof(uint));
  handle->write((char*)&version(),  sizeof(uint));
  handle->write((char*)&timeJDR(),  sizeof(uint));
  handle->write((char*)&timeTL(),  sizeof(uint));
  handle->write((char*)&dataType(),  sizeof(uint));
  handle->write((char*)&evType(),  sizeof(uint));
  handle->write((char*)&blocksize_ ,  sizeof(int));
  handle->write((char*)&presync(),  sizeof(uint));
  if (version() > 1) {
    handle->write((char*)&timeLTL(),  sizeof(uint));
  } else {
    timeLTL() = 0;
    handle->write((char*)&timeLTL(),  sizeof(uint));
  }
  if (version() > 2) {
    handle->write((char*)&observatory(),  sizeof(uint));
  }

  // Write empty space reserved for aditional headers
  uint empty = 0;
  if (version() > 2) {
    handle->write((char*)&empty,  2*sizeof(uint));
  } else {
    handle->write((char*)&empty,  3*sizeof(uint));
  }
}

// ------------------------------------------------------------ writeContentsHere

void LopesEvent::writeContentsHere (fstream* handle)
{
  // Now start with the real data
  
  for (uint i = 0; i < length_; i ++) {
    writeChannelHere(handle, i);
  }
}

// ------------------------------------------------------------- writeChannelHere

void LopesEvent::writeChannelHere (fstream* handle,
				   uint c_num)
{
  uint c_id (0);
  uint c_len (0);
  
  c_id  = dataSet(c_num)->id();
  c_len = dataSet(c_num)->length();
  // Write channel information
  handle->write((char*)&c_id, sizeof(uint));
  handle->write((char*)&c_len, sizeof(uint));
  // Write channel data
  // Data is assumed to be 16 bit
  handle->write((char*)dataSet(c_num)->data_, sizeof(short)*c_len);
}


// ==============================================================================
//
//  Access to internal data fields
//
// ==============================================================================

// ---------------------------------------------------------------------- dataSet

Data* LopesEvent::dataSet(uint set)
{
  if (set >= 0 && set < length_) {
    return data_[set];
  } else {
    cerr << "  WARNING: Dataset number out of range." << endl;
    return NULL;
  }
}

// ------------------------------------------------------------------ dataSetById

Data* LopesEvent::dataSetById(uint id)
{
  for (uint i = 0; i < length_; i ++) {
    if (id == dataSet(i)->id()) {
      return dataSet(i);
    }
  }
  cerr << "  WARNING: Unknown dataset id." << endl;
  return NULL;
}

// ==============================================================================
//
//  Summary of the data stored in the object
//
// ==============================================================================

void LopesEvent::summary (std::ostream &os)
{
  os << " - Filename ........................ : " << filename_    << std::endl;
  os << " - Length of the data reord ........ : " << length_      << std::endl;
  os << " - Version number of the data record : " << version_     << std::endl;
  os << " - KASCADE timestamp (JDR) ......... : " << timeJDR_     << std::endl;
  os << " - KASCADE timestamp (TL) .......... : " << timeTL_      << std::endl;
  os << " - Type of the data record ......... : " << dataType_    << std::endl;
  os << " - Type of the recorded event ...... : " << evType_      << std::endl;
  os << " - Blocksize ............. [samples] : " << blocksize_   << std::endl;
  os << " - Presync for the sync signal ..... : " << presync_     << std::endl;
  os << " - Timestamp from menable card ..... : " << timeLTL_     << std::endl;
  os << " - Observatory ID .................. : " << observatory_ << std::endl;
}

// ==============================================================================
//
// Functionality within the DataReader framework
//
// ==============================================================================

// ------------------------------------------------------------------- setStreams

Bool LopesEvent::setStreams ()
{
  bool status (true);

  uint nofSamples (blocksize_p);
  Vector<uint> antennas (length_);
  Vector<Double> adc2voltage (DataReader::ADC2Voltage());
  Matrix<DComplex> fft2calfft (DataReader::fft2calfft());
  Vector<String> filenames (length_);
  DataIterator *iterator;

  /*
    Set the filenames
  */
  try {
    filenames = filename_;
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
  }
  
  /*
    Set up the DataIterator objects and the antenna numbers
  */
  ifstream infile;
  uint headerSize (48);
  uint channelID (0);
  uint channelLength (0);

  iterator = new DataIterator [length_];

  try {
    infile.open(filename_.c_str(), ios::in | ios::binary);
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
  }

  try {
    for (uint antenna(0); antenna<length_; antenna++) {
      // antenna number - zero based here
      antennas(antenna) = antenna;
      
      // Go to the end of the header segment
      infile.seekg(headerSize, ios::beg);
      // Go to the end of the data header
      infile.seekg(antenna*8, ios::cur);
      // Forward to the end of the data segment
      infile.seekg(2*channelLength*antenna, ios::cur);
      // Read channel ID and length of the data segment
      infile.read((char*)&channelID, sizeof(uint));
      infile.read((char*)&channelLength, sizeof(uint));
      //
//       iterator[antenna].setDataStart(infile.tellg());
      iterator[antenna].setDataStart(0);
      iterator[antenna].setStepWidth(1);
    }
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
  }
  
  infile.close();

  /*
    Setup of the conversion arrays
  */
  
  uint fftLength (nofSamples/2+1);
  IPosition shape (fft2calfft.shape());

  if (adc2voltage.nelements() != length_) {
    double weight (adc2voltage(0));
    adc2voltage.resize (length_);
    adc2voltage = weight;
  }
  
  if (uint(shape(0)) != fftLength ||
      uint(shape(1)) != length_) {
    fft2calfft.resize (fftLength,length_);
    fft2calfft = 1.0;
  }

  // -- feedback ---------------------------------------------------------------

//   cout << " - blocksize [samples] = " << nofSamples << endl;
//   cout << " - antennas            = " << antennas   << endl;
//   cout << " - FFT length          = " << fftLength  << endl;
//   cout << " - shape(adc2voltage)  = " << adc2voltage.shape() << endl;
//   cout << " - shape(fft2calfft)   = " << fft2calfft.shape()  << endl;

  // -- call to DataReader::init(...) ------------------------------------------

  DataReader::init (nofSamples,
		    antennas,
		    adc2voltage,
		    fft2calfft,
		    filenames,
		    iterator);
  
  return status;
}

// --------------------------------------------------------------------------- fx

Matrix<Double> LopesEvent::fx ()
{
  uint antenna (0);
  uint sample (0);
  uint nofSelectedAntennas (DataReader::nofSelectedAntennas());
  Matrix<Double> fx (blocksize_p,nofSelectedAntennas);

  /* If data are extracted from the internal data array, the original stepping
     width of the dataIterator needs to be compensated for. */
  uint offset (0);
  
  for (antenna=0; antenna<nofSelectedAntennas; antenna++) {
    // get the data for this antenna
    Data *data = dataSet(selectedAntennas_p(antenna));
    // adjust the offset in the data segment
    offset = iterator_p[selectedAntennas_p(antenna)].position()/iterator_p[selectedAntennas_p(antenna)].stepWidth();
    for (sample=0; sample<blocksize_p; sample++) {
      //      fx(sample,antenna) = data->data(iterator_p[selectedAntennas_p(antenna)].position()+sample);
      fx(sample,antenna) = data->data(offset+sample);
    }
  }
  
  return fx;
}

}  //  Namespace CR -- end
