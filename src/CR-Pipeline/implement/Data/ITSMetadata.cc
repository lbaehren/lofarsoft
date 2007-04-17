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

/* $Id: ITSMetadata.cc,v 1.8 2006/10/31 18:24:08 bahren Exp $*/

#include <fstream>
#include <Data/ITSMetadata.h>

// ==============================================================================
//
//  Construction
//
// ==============================================================================

ITSMetadata::ITSMetadata ()
  : isOk_p (False)
{
  init ();
}

ITSMetadata::ITSMetadata (String const &metafile)
  : isOk_p (False)
{
  init ();
  setMetafile (metafile);
}

ITSMetadata::ITSMetadata (ITSMetadata const &other)
  : isOk_p (False)
{
  copy (other);
}

void ITSMetadata::init () 
{
  metafile_p       = "UNDEFINED";
  directory_p      = "UNDEFINED";
  description_p    = "UNDEFINED";
  experimentType_p = "UNDEFINED";
  plugin1_p        = "UNDEFINED";
  plugin2_p        = "UNDEFINED";
  basename_p       = "UNDEFINED";
  starttime_p      = "UNDEFINED";

  interval_p         = 0;
  captureMode_p      = ITSMetadata::IMMEDIATE;
  captureSize_p      = ITSMetadata::S16_512K;
  signExtension_p    = True;
  skipcapture_p      = False;
  iterations_p       = 0;
  currentIteration_p = 0;
  observationId_p    = 0;
  observationOwner_p = "UNDEFINED";

 }

// ==============================================================================
//
//  Destruction
//
// ==============================================================================

ITSMetadata::~ITSMetadata ()
{
  destroy();
}

// ==============================================================================
//
//  Operators
//
// ==============================================================================

ITSMetadata &ITSMetadata::operator= (ITSMetadata const &other)
{
  if (this != &other) {
    destroy ();
    copy (other);
  }
  return *this;
}

// ------------------------------------------------------------------------- copy

void ITSMetadata::copy (ITSMetadata const& other)
{
  try {
    // Variables for book-keeping and control
    isOk_p      = other.isOk_p;
    metafile_p  = other.metafile_p;
    directory_p = other.directory_p;
    //
    datafiles_p.resize (other.datafiles_p.shape());
    datafiles_p = other.datafiles_p;
    // Experiment meta information
    basename_p         = other.basename_p;
    currentIteration_p = other.currentIteration_p;
    currentStatus_p    = other.currentStatus_p;
    description_p      = other.description_p;
    experimentType_p   = other.experimentType_p;
    interval_p         = other.interval_p;
    observationId_p    = other.observationId_p;
    observationOwner_p = other.observationOwner_p;
    plugin1_p          = other.plugin1_p;
    plugin2_p          = other.plugin2_p;
    starttime_p        = other.starttime_p;
    //
    antennas_p.resize(other.antennas_p.shape());
    antennas_p = other.antennas_p;
    //
    antennaHasValidData_p.resize(other.antennaHasValidData_p.shape());
    antennaHasValidData_p = other.antennaHasValidData_p;
  } catch (AipsError x) {
    cerr << "[ITSMetadata::copy]" << x.getMesg() << endl;
    isOk_p = False;
  }
}

void ITSMetadata::destroy ()
{;}

// ==============================================================================
//
//  Parameters
//
// ==============================================================================

// ------------------------------------------------------------------ setMetafile

void ITSMetadata::setMetafile (const String& metafile)
{
  Bool status (True);
  String metafile_tmp (metafile);

  // check if the file is actually there
  ifstream infile;

  try {
    infile.open(metafile.c_str(), ios::in);
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    status = False;
  }
  
  if (!infile || infile == NULL) {
    cerr << "[ITSMetadata::setMetafile] Error opening file " << metafile << endl;
    status = False;
    //
    metafile_tmp = adjustPathToMetafile (metafile);
    setMetafile (metafile_tmp);
  } else {
    /*
      We need some additional testing at this point. Even though the file exists
      and can be read, it still might be the wrong file (e.g. information
      extracted from the Glish data record would point to "lopeshdr.hdr" instead
      to "experiment.meta").
    */
    string buffer ("");
    getline (infile,buffer);
    // by design all ITS experiment.meta files start with a common identifier
    if (buffer != "[OBSERVATION]") {
      std::cerr << "[ITSMetadata::setMetafile] Missing identifier in metafile"
		<< std::endl;
      //
      metafile_tmp = adjustPathToMetafile (metafile);
      //
      std::cerr << " File path adjusted to : " << metafile_tmp << std::endl;
      //
      ifstream infile_tmp;
      infile_tmp.open(metafile.c_str(), ios::in);
      if (!infile_tmp || infile_tmp == NULL) {
	status = False;
      } else {
	// close the file stream
	infile_tmp.close();
      }
    }
    infile.close();
  }
  
  if (status) {
    metafile_p = metafile_tmp;
    
    // Decompose path to meta file into directory and filename
    directory_p  = CR::dirFromPath(metafile_tmp);
    
    // Read in the information from the experiment meta file
    readMetafile ();
  }

  if (infile.is_open()) {
    infile.close();
  }

  isOk_p = status;
}

string ITSMetadata::adjustPathToMetafile (string const &path)
{
  string newPath;
  
  newPath = CR::dirFromPath(path);
  newPath += "/experiment.meta";
  
  return newPath;
}

// -------------------------------------------------------------------- datafiles

Vector<String> ITSMetadata::datafiles (bool const &fullPath) const
{
  if (fullPath) {
    Vector<String> files (datafiles_p.shape(),directory_p);
    //
    files += Vector<String>(files.shape(),"/");
    files += datafiles_p;
    //
    return files;
  } else {
    return datafiles_p;
  }
}

// ----------------------------------------------------------------- setDatafiles

bool ITSMetadata::setDatafiles (String const &filenames)
{
  bool status (true);
  uint n (0);
  
  try {
    // decompose the input string into the substrings
    Vector<String> datafiles = CR::getSubstrings(filenames," ");
    // keep in mind, that the input string contains one empty entry, which needs
    // needs to be skipped when storing the information internally
    uint nofDatafiles (datafiles.nelements()-1);

    // resize the internal storage array
    datafiles_p.resize (nofDatafiles);
    
    for (n=0; n<nofDatafiles; n++) {
      datafiles_p(n) = datafiles(n+1);
      // -- control feedback (uncommend to list filenames) --
//       cout << n << " :: " << directory_p << "/" << datafiles_p(n) <<endl;
    }
  } catch (AipsError x) {
    cerr << "[ITSMetadata::setDatafiles]" << x.getMesg() << endl;
    status = false;
  }

  return checkDatafiles ();
}

// --------------------------------------------------------------- checkDatafiles

bool ITSMetadata::checkDatafiles ()
{
  bool status (true);
  char buffer;
  uint n (0);

  Vector<String> files (datafiles(true));
  uint nofDatafiles (files.nelements());
  uint nofValidDatafiles (nofDatafiles);

  antennaHasValidData_p.resize (nofDatafiles);
  antennaHasValidData_p = true;

  // go through the list of data files and check if they are ok
  
  for (n=0; n<nofDatafiles; n++) {
    // open stream for this file and try to read data
    ifstream datafile;
    datafile.open (files(n).c_str(), ios::in);
    datafile.get(buffer);
    if (!datafile || datafile == NULL) {
      nofValidDatafiles--;
      antennaHasValidData_p(n) = false;
    }      
    datafile.close();
  }

  // update the internal list of datafiles based on the outcome of the above 
  // testing

  files = datafiles (false);
  datafiles_p.resize(nofValidDatafiles);

  uint i (0);

  for (n=0; n<nofDatafiles; n++) {
    if (antennaHasValidData_p(n)) {
      datafiles_p(i) = files(n);
      i++;
    } else {
      cout << "[ITSMetadata::checkDatafiles] Removing "
	   << files(n)
	   << " ("
	   << n
	   << ") from list of data files." << endl;
    }
  }
  
  return status;
}

// ----------------------------------------------------------------- hasValidData

Bool ITSMetadata::hasValidData (uint const &antenna) const
{
  if (antenna > antennas_p.nelements()) {
    return False;
  } else if (!antennaHasValidData_p(antenna)) {
    return False;
  } else {
    return True;
  }
}

// ------------------------------------------------------------------ setAntennas

bool ITSMetadata::setAntennas (String const &antennas)
{
  bool status (true);

  try {
    // decompose the input string into the substrings
    Vector<String> antennaNumbers = CR::getSubstrings(antennas," ");
    // keep in mind, that the input string contains one empty entry, which needs
    // needs to be skipped when storing the information internally
    uint nofAntennas (antennaNumbers.nelements()-1);
    // resize the internal storage array
    antennas_p.resize (nofAntennas);

    for (uint n (0); n<nofAntennas; n++) {
       antennas_p(n) = uint(std::atol(antennaNumbers(n).c_str()));
    }

//     cout << " - nof. antennas     = " << nofAntennas << endl;
//     cout << " - antennas (string) = " << antennas    << endl;
//     cout << " - antennas (vector) = " << antennas_p  << endl;

  } catch (AipsError x) {
    cerr << "[ITSMetadata::setAntennas]" << x.getMesg() << endl;
    status = false;
  }
  
  return status;
}

// --------------------------------------------------------------------- antennas

Vector<uint> ITSMetadata::antennas (bool const &validDataOnly) const
{
  if (validDataOnly) {
    uint nofValidDatafiles (ntrue(antennaHasValidData_p));
    Vector<uint> antennaNumbers (nofValidDatafiles);
    uint nofAntennas (antennas_p.nelements());
    uint i (0);
    
    for (uint n(0); n<nofAntennas; n++) {
      if (antennaHasValidData_p(n)) {
	antennaNumbers(i) = antennas_p(n);
	i++;
      }
    }
    
    return antennaNumbers;
  } else {
    return antennas_p;
  }
}

// ==============================================================================
//
//  Methods
//
// ==============================================================================

// ----------------------------------------------------------------- readMetafile

void ITSMetadata::readMetafile ()
{
  bool status (true);
  uint lineNumber (0);
  string buffer;
  string keyword;
  string metaValue;
  string antennas ("");
  string dataFilenames ("");
  ifstream infile (metafile_p.c_str());
  
  if (infile.is_open() && infile.good()) {
    while (! infile.eof() && infile.good() && status) {
      lineNumber++;
      // read one line from the file
      try {
	getline (infile,buffer);
      } catch (AipsError x) {
	cerr << x.getMesg() << endl;
	status = false;
      }
      // try to extract the keyword
      if (! infile.eof() && infile.good() && status && buffer != "") {
	try {
	  keyword = strtok (CR::string2char(buffer), "=");
	} catch (AipsError x) {
	  cerr << x.getMesg() << endl;
	  keyword = "";
	}
	// try to extract the keyword
	if (keyword != "[OBSERVATION]" && keyword != "#EOF") {
	  try {
	    // need to handled separately (see documentation)
	    if (keyword == "file") {
	      metaValue = CR::fileFromPath(strtok (NULL, "\n"));
	      dataFilenames += " ";
	      dataFilenames += metaValue;
	    }
	    else if (keyword == "antennas") {
	      antennas = strtok (NULL, "\n");
	    }
	    else if (keyword == "basename") {
	      basename_p = strtok (NULL, "\n");
	    }
	    else if (keyword == "description") {
	      description_p = strtok (NULL, "\n");
	    }
	    else if (keyword == "interval") {
	      metaValue = strtok (NULL, "\n");
	      interval_p = atoi (metaValue.c_str());
	    }
	    else if (keyword == "iterations") {
	      metaValue = strtok (NULL, "\n");
	      iterations_p = atoi (metaValue.c_str());
	    }
 	    else if (keyword == "observation_id") {
	      char * pEnd;
 	      metaValue = strtok (NULL, "\n");
	      observationId_p = strtol (metaValue.c_str(),&pEnd,0);
 	    }
	    else if (keyword == "observation_owner") {
	      observationOwner_p = strtok (NULL, "\n");
	    }
	    else if (keyword == "plugin1") {
	      plugin1_p = strtok (NULL, "\n");
	    }
	    else if (keyword == "plugin2") {
	      plugin2_p = strtok (NULL, "\n");
	    }
	    else {
	      metaValue = strtok (NULL, "\n");
	    }
	  } catch (AipsError x) {
	    cerr << x.getMesg() << endl;
	    metaValue = "";
	  }
	  // -- control output --
//  	  cout << lineNumber << " :: " << keyword<< " :: " << metaValue << endl;
	}
      }
    }
  } else {
    cerr << "[ITSMetadata] Unable to open file." << endl;
    status = false;
  }

  // process the list of data files
  status = setDatafiles (dataFilenames);
  status = setAntennas (antennas);

  infile.close();
  isOk_p = status;
}

