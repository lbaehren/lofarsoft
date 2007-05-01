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


/* $Id: ObservationMeta.cc,v 1.5 2006/11/27 10:54:04 bahren Exp $ */

#include <Observation/ObservationMeta.h>

// =============================================================================
//
//  Construction / Destruction
//
// =============================================================================

ObservationMeta::ObservationMeta (String filename)
{
  ObservationMeta::readMetafile (filename);
}

ObservationMeta::ObservationMeta (String metaFile, String antposFile)
{
  ObservationMeta::readMetafile (metaFile);
  ObservationMeta::setAntennaPositions(antposFile);
}

ObservationMeta::ObservationMeta (String filename, GlishRecord& datarec)
{
  ObservationMeta::readMetafile (filename);
  ObservationMeta::metadata (datarec);
}

ObservationMeta::~ObservationMeta () {}

// =============================================================================
//
//  Location of the metadata file
//
// =============================================================================

void ObservationMeta::setMetafile (String path)
{
  String filename = CR::fileFromPath(path);
  String dirname  = CR::dirFromPath(path);
  filename_  = filename;
  directory_ = dirname;
}

String ObservationMeta::metafile ()
{ 
  String sep = "/";
  String metafile = directory_ + sep + filename_;
  return metafile;
}

String ObservationMeta::directory () { return directory_; }

// =============================================================================
//
//  Data I/O
//
// =============================================================================

Bool ObservationMeta::readMetafile ()
{
  Bool ok = True;
  char s[256];
  String keyword;
  ifstream metafile;
  GlishRecord record;
  GlishArray value;
  String filename = ObservationMeta::metafile();

  metafile.open(filename.c_str(),ios::in);

  /* Parse the first half of the metafile; this contains everything up to the
     '#EOF' marker, i.e. the section containing all settings for the
     experiment. */
  while (!metafile.getline(s,256).eof() && ok) {
    // try to extract the keyword
    try {
      keyword = strtok (s, "=");
    } catch (AipsError x) {
      cerr << x.getMesg() << endl;
    }
    // check if keyword could be extracted
    if (keyword != "[OBSERVATION]" && keyword != "#EOF") {
      try {
	value = strtok (NULL, "\n");
	record.add(keyword,value);
      } catch (AipsError x) {
	cerr << x.getMesg() << endl;
      }
    } else {
      // Guard against reading beyound the EOF marker
      if (keyword == "#EOF") ok = False;
    }
  }
  
  // Use the copy extracted metainformation to the internal data area
  ObservationMeta::metadata (record);

  /* Parse the second part of the metafile; this contains the list of
     triggertimes at which an observation actually was started, as well as a 
     list of files resulting from this observation. */

  // set of the string buffers
  IPosition nofAntennas = antennas_.shape();
  Vector<String> triggertime(iterations_);
  Matrix<String> datafiles (iterations_,nofAntennas(0));
  Int iteration=-1,datafile=0;

  datafiles = "";
  triggertime = "";

  metafile.seekg (0, ios::beg);
  while (!metafile.getline(s,256).eof()) {
    // try to extract the keyword
    keyword = strtok (s, "=");
    if (ok) {
      String metavalue = strtok (NULL, "\n");
      if (keyword == "file") {
 	datafiles(iteration,datafile) = metavalue;
 	++datafile;
      }
      else {
	++iteration;
	datafile = 0;
	//
	triggertime(iteration) = metavalue;
      }
    }
    // watch out for '#EOF' marker separation the two halves of the meta file
    if (keyword != "[OBSERVATION]" && keyword == "#EOF") ok = True;
  }

  ObservationMeta::triggertime (triggertime);
  ObservationMeta::setDatafiles (datafiles);
  
  metafile.close();

  return ok;
}

Bool ObservationMeta::readMetafile (String filename)
{
  Bool ok; 

  ObservationMeta::setMetafile (filename);

  ok = ObservationMeta::readMetafile ();

  return ok;
}

// =============================================================================
//
//  Conversion to LOPES Tools metafile format
//
// =============================================================================

void ObservationMeta::lopesheader ()
{
  String sep = "/";
  String filename = directory_ + sep + LOPESMETA;
  ofstream lopesmeta (filename.c_str(),ios::out);
  ObservationMeta::lopesheader (lopesmeta);
  lopesmeta.close();
}

void ObservationMeta::lopesheader (std::ostream& s)
{
  IPosition nofAntennas = antennas_.shape();
  Int antenna;
  String date,time;
 
  /* Write the standard header information required for the LOPES Tools to
     create a new data object. */
  s << "#Measurement: 1" << endl;
  s << "#Project: ITS" << endl;
  s << "#DataType: TIM40f" << endl;
  s << "#FrequencyUnit: 1e6" << endl;
  s << "#SamplerateUnit: 1e6" << endl;
  s << "#Samplerate: 80" << endl;
  s << "#Bandwidth: 40" << endl;
  s << "#Weather: Undefined" << endl;
  s << "#ADCMaxChann: 2048" << endl;
  s << "#ADCMinChann: -2048" << endl;
  s << "#NyquistZone: 1" << endl;
  s << "#MaxVolt: 1" << endl;
  s << "#LocalCalFile: empty.tab" << endl;
  s << "#AntennaCalFile: empty.tab" << endl;
  s << "#Observatory: LOFAR-ITS" << endl;
  s << "#Location: Exloo (NL)" << endl;
  s << "#TIMoffset: 0" << endl;
  s << "#CalFileFreqUnit: 1e6" << endl;
  
  /* Add the ITS experiment specific metadata */
  s << "#ITSdescription: " << description_ << endl;
  s << "#ITSexperiment_type: " << type_ << endl;
  s << "#ITSplugin1: " << plugin1_ << endl;
  s << "#ITSplugin2: " << plugin2_ << endl;
  s << "#ITSbasename:  " << basename_ << endl;
  s << "#ITSstarttime: " << starttime_ << endl;
  s << "#ITSinterval:  " << interval_ << endl;
  s << "#ITScapturemode: " << capturemode_ << endl;
  s << "#ITScapturesize:  " << capturesize_ << endl;
  // Array elements (antennae) used in the observation
  s << "#ITSantennas:";
  for (int ant=0; ant<nofAntennas(0); ant++) s << " " << antennas_(ant);
  s << endl;

  s << "#ITSsignextension: " << signextension_ << endl;
  s << "#ITSskipcapture: " << skipcapture_ << endl;
  s << "#ITSiterations: " << iterations_ << endl;
  s << "#ITScurrent_status:  scheduled" << endl;
  s << "#ITScurrent_iteration:  0" << endl;
  s << "#ITSobservation_id: " << observationID_ << endl;
  s << "#ITSobservation_owner:  " << owner_ << endl;
  
  ObservationMeta::extractFromISO8601(date,time,submittime_);  
  s << "#ITSsubmittime: " << submittime_ << endl;
  s << "#Time: " << date << endl;
  s << "#Timezone: +00" << endl;

  // Add information per iteration and used antenna
  for (int iter=0; iter<iterations_; iter++) {
    ObservationMeta::extractFromISO8601(date,time,triggertime_(iter));  
    s << "#Date: " << date << "/" << time << endl;    
    for (int ant=0; ant<nofAntennas(0); ant++) {
      antenna = antennas_(ant);
      if (datafiles_(iter,ant) != "") {
	s << "#ITSfile: " << datafiles_(iter,ant) << endl;
      }
      s << "#AntPos: "
	<< antennaPositions_(antenna,1) << " , "
	<< -antennaPositions_(antenna,0)<< " , "
	<< antennaPositions_(antenna,2) << endl;
      s << "#presync:  0" << endl;
      s << "#AntNum: " << ant+1 << endl;
      s << "#AntName: " << antenna << endl;
      if (datafiles_(iter,ant) != "") {
	s << "#Files: " << ObservationMeta::nameDatafile(iter,ant) << endl;
      }
    }
  }
  
}


// =============================================================================
//
//  Member data I/O via Glish record
//
// =============================================================================

void ObservationMeta::metadata (GlishRecord& record)
{
  GlishArray gtmp;
  String s;

  if (record.exists("observation_owner")) {
    String owner;
    gtmp = record.get("observation_owner");
    gtmp.get(owner);
    ObservationMeta::owner(owner);
  }
  if (record.exists("basename")) {
    String basename;
    gtmp = record.get("basename");
    gtmp.get(basename);
    ObservationMeta::basename(basename);
  }
  if (record.exists("starttime")) {
    String starttime;
    gtmp = record.get("starttime");
    gtmp.get(starttime);
    ObservationMeta::starttime(starttime);
  }
  if (record.exists("submittime")) {
    String submittime;
    gtmp = record.get("submittime");
    gtmp.get(submittime);
    ObservationMeta::submittime(submittime);
  }
  if (record.exists("description")) {
    String description;
    gtmp = record.get("description");
    gtmp.get(description);
    ObservationMeta::description(description);
  }
  if (record.exists("antennas")) {
    Vector<Int> antennas;
    gtmp = record.get("antennas");
    gtmp.get(s);
    antennas = ObservationMeta::string2vector (s);
    ObservationMeta::antennas(antennas);
  }
  if (record.exists("antposFile")) {
    String antposFile;
    gtmp = record.get("antposFile");
    gtmp.get(antposFile);
    ObservationMeta::setAntennaPositions(antposFile);
  }
  if (record.exists("iterations")) {
    Int iterations;
    gtmp = record.get("iterations");
    gtmp.get(s);
    iterations = atoi (s.c_str());
    ObservationMeta::iterations(iterations);
  }
  if (record.exists("interval")) {
    Int interval;
    gtmp = record.get("interval");
    gtmp.get(s);
    interval = atoi (s.c_str());
    ObservationMeta::interval(interval);
  }
  if (record.exists("capturemode")) {
    String capturemode;
    gtmp = record.get("capturemode");
    gtmp.get(capturemode);
    ObservationMeta::capturemode(capturemode);
  }
  if (record.exists("capturesize")) {
    String capturesize;
    gtmp = record.get("capturesize");
    gtmp.get(capturesize);
    ObservationMeta::capturesize(capturesize);
  }
  if (record.exists("experiment_type")) {
    String type;
    gtmp = record.get("experiment_type");
    gtmp.get(type);
    ObservationMeta::type(type);
  }
  if (record.exists("plugin1")) {
    String plugin1;
    gtmp = record.get("plugin1");
    gtmp.get(plugin1);
    ObservationMeta::plugin1(plugin1);
  }
  if (record.exists("plugin2")) {
    String plugin2;
    gtmp = record.get("plugin2");
    gtmp.get(plugin2);
    ObservationMeta::plugin2(plugin2);
  }
  if (record.exists("signextension")) {
    Bool signextension;
    gtmp = record.get("signextension");
    gtmp.get(s);
    signextension = ObservationMeta::string2boolean (s);
    ObservationMeta::signextension(signextension);
  }
  if (record.exists("skipcapture")) {
    Bool skipcapture;
    gtmp = record.get("skipcapture");
    gtmp.get(s);
    skipcapture = ObservationMeta::string2boolean (s);
    ObservationMeta::skipcapture(skipcapture);
  }
  // variables set during experiment
  if (record.exists("observation_id")) {
    Int observationID;
    gtmp = record.get("observation_id");
    gtmp.get(s);
    observationID = atoi (s.c_str());
    ObservationMeta::observationID(observationID);
  }
  if (record.exists("triggertime")) {
    Vector<String> triggertime;
    gtmp = record.get("triggertime");
    gtmp.get(triggertime);
    ObservationMeta::triggertime(triggertime);
  }

}

GlishRecord ObservationMeta::metadata () 
{
  GlishRecord record;

  // add field per member data
  record.add("observation_owner",owner_.c_str());
  record.add("basename",basename_.c_str());
  record.add("starttime",starttime_.c_str());
  record.add("description",description_.c_str());
  record.add("antennas",antennas_);
  record.add("antposFile",antposFile_);
  record.add("iterations",iterations_);
  record.add("interval",interval_);
  record.add("capturemode",capturemode_.c_str());
  record.add("capturesize",capturesize_.c_str());
  record.add("type",type_.c_str());
  record.add("plugin1",plugin1_.c_str());
  record.add("plugin2",plugin2_.c_str());
  record.add("signextension",signextension_);
  record.add("skipcapture",skipcapture_);

  // Return the created record
  return record;
}

void ObservationMeta::metadata (std::ostream& s = std::cout)
{
  s << "\n[ObservationMeta]\n" << endl;
  s << " - Location of the metafile .. : " << directory_ << endl;
  s << " - Name of the metafile ...... : " << filename_ << endl;
  s << " - Name of LOPES metafile .... : " << LOPESMETA << endl;
  s << " - Original basename ......... : " << basename_ << endl;
  s << " - Observation owner ......... : " << owner_ << endl;
  s << " - Experiment starttime ...... : " << starttime_ << endl;
  s << " - Experiment submission time  : " << submittime_ << endl;
  s << " - Experiment description .... : " << description_ << endl;
  s << " - Selected antennas ......... : " << antennas_ << endl;
  s << " - Antenna position file ..... : " << antposFile_ << endl;
  s << " - Number of iterations ...... : " << iterations_ << endl;
  s << " - Iteration interval .. [sec] : " << interval_ << endl;
  s << " - Capture mode .............. : " << capturemode_ << endl;
  s << " - Capture size .............. : " << capturesize_ << endl;
  s << " - Experiment type ........... : " << type_ << endl;
  s << " - Plugin1 ................... : " << plugin1_ << endl;
  s << " - Plugin2 ................... : " << plugin2_ << endl;
  s << " - Sign extension ............ : " << signextension_ << endl;
  s << " - Skip capture .............. : " << skipcapture_ << endl;
  s << " - Observation identifier .... : " << observationID_ << endl;
  s << " - Shape of triggertimes array : " << triggertime_.shape() << endl;
  s << " - Shape of datafiles array .. : " << datafiles_.shape() << endl;
}

// =============================================================================
//
//  Setup of the array antennae
//
// =============================================================================

void ObservationMeta::antennas (Vector<Int>& antennas)
{
  Int nelem;
  antennas.shape(nelem);
  antennas_.resize(nelem);
  antennas_ = antennas;
}

Vector<Int> ObservationMeta::antennas () { return antennas_; }

Vector<Int> ObservationMeta::antennas (Int shift)
{
  IPosition shape = antennas_.shape();
  Vector<Int> antennas(shape);
  
  for (Int i=0; i<shape(0); i++) antennas(i) = antennas_(i) + shift;

  return antennas;
}

void ObservationMeta::setAntennaPositions (String filename)
{
  // store the provided filename
  antposFile_ = filename;

  // try to read in the data from the file
  try {
    ObservationMeta::readAntennaPositions ();
  } catch (AipsError x) {
    cerr << "[ObservationMeta::antennaPositions] " << x.getMesg() << endl;
  }

}

Matrix<Double> ObservationMeta::antennaPositions () { return antennaPositions_; }

Bool ObservationMeta::readAntennaPositions ()
{
  Int nofCols,nofRows;
  IPosition antposShape;
  ifstream antposFile;
  
  /* Use the AIPS++ ArrayIO methods to read in the antenna position file */
  try {
    antposFile.open(antposFile_.c_str(),ios::in);
  } catch (AipsError x) {
    cerr << "[ObservationMeta::antennaPositions] " << x.getMesg() << endl;
    return False;
  }
  
  try {
    antposFile >> nofRows >> nofCols;
    antennaPositions_.resize(nofRows,nofCols);
    for (int row=0; row<nofRows; row++) {
      for (int col=0; col<nofCols; col++) {
	antposFile >> antennaPositions_(row,col);
      }
    }
  } catch (AipsError x) {
    cerr << "[ObservationMeta::antennaPositions] " << x.getMesg() << endl;
    return False;
  }
  
  return True;
}

// =============================================================================
//
//  Setup of the AVIARY Plugins
//
// =============================================================================

void ObservationMeta::plugins (Vector<String>& plugins) {
  IPosition shape = plugins.shape();
  if (shape == 2) {
    ObservationMeta::plugin1 (plugins(0));
    ObservationMeta::plugin2 (plugins(1));
  } else {
    cerr << "[ObservationMeta::plugins] Only found single plugin option." << endl;
    ObservationMeta::plugin1 (plugins(0));
  }
}

void ObservationMeta::plugin1 (String plugin1) { 
  ObservationMeta::parsePluginOptions (plugin1);
  plugin1_ = plugin1;
}

String ObservationMeta::plugin1 () { return plugin1_; } 

void ObservationMeta::plugin2 (String plugin2) {
  ObservationMeta::parsePluginOptions (plugin2);
  plugin2_ = plugin2;
}

String ObservationMeta::plugin2 () { return plugin2_;}

void ObservationMeta::parsePluginOptions (String str)
{
  String tmp,sep = " ";
  unsigned int nofChars = str.length();
  unsigned int start=0;
  unsigned int loc=0;
  Int num = CR::nofSubstrings(str,sep);
  Vector<String> substrings(num);

  num = start = 0;
  for (unsigned int i=0; i<nofChars; i++) {
    loc = str.find(sep,i);
    if (i == loc) {
      tmp.assign(str,start,loc-start);
      substrings(num) = tmp;
      // update counters
      ++num;
      start = loc+1;
    }
  }
  tmp.assign(str,start,loc-nofChars);
  substrings(num) = tmp;
  
}

// =============================================================================
//
//  Time stamps
//
// =============================================================================

// (a) Time at which the experiment was submitted to the queue

void ObservationMeta::submittime (String submittime) { submittime_ = submittime; }

String ObservationMeta::submittime () { return submittime_; }

void ObservationMeta::submittime (Quantity& qtime)
{
  MVTime mvtime(ObservationMeta::iso8601ToTime (submittime_));
  Quantity q(mvtime);
  qtime = q;
}

void ObservationMeta::submittime (MEpoch& mtime)
{
  MVTime mvtime (ObservationMeta::iso8601ToTime (submittime_));
  MVEpoch mvepoch (mvtime);
  MEpoch mepoch (mvepoch);
  mtime = mepoch;
}

// (b) Time at which the experiment is to be started.

void ObservationMeta::starttime (String starttime) { starttime_ = starttime; }

String ObservationMeta::starttime () { return starttime_; }

// (c) Time(s) at which data where captured

void ObservationMeta::triggertime (const Vector<String>& triggertime)
{
  IPosition shape = triggertime.shape();
  
  if (shape(0) != iterations_) {
    cerr << "[ObservationMeta::triggertime] Warning, array size mismatch." << endl;
    cerr << " - Number of iterations  : " << iterations_ << endl;
    cerr << " - Shape of input vector : " << shape << endl;
  }

  triggertime_.resize(iterations_);
  triggertime_ = triggertime;
  
}

Vector<String> ObservationMeta::triggertime () { return triggertime_; }

void ObservationMeta::triggertime (Vector<Quantity>& epoch)
{
  IPosition shape = triggertime_.shape();
  epoch.resize(shape);

  for (int i=0; i<shape(0); i++) {
    MVTime mvtime(ObservationMeta::iso8601ToTime (triggertime_(i)));
    Quantity q(mvtime);
    epoch(i) = q;
  }
}

void ObservationMeta::triggertime (Vector<MEpoch>& epoch)
{
  IPosition shape = triggertime_.shape();
  epoch.resize(shape);

  for (int i=0; i<shape(0); i++) {
    MVTime mvtime(ObservationMeta::iso8601ToTime (triggertime_(i)));
    MVEpoch mvepoch (mvtime);
    MEpoch mepoch (mvepoch);
    epoch(i) = mepoch;
  }
}

void ObservationMeta::extractFromISO8601 (String& date,
					  String& time,
					  const String iso)
{
  unsigned int nofChars = iso.length();
  unsigned int loc = iso.find( "T", 0);
  unsigned int locBar = iso.find( "-", 0);
  string sub1,sub2;
  
  sub1.assign(iso,0,loc);  
  sub2.assign(iso,loc+1,nofChars-loc-2);

  nofChars = sub1.length();
  for (unsigned int i=0; i<nofChars; i++) {
    locBar = sub1.find( "-", i);
    if (i == locBar) sub1.replace(i,1,"/");
  }
  
  date = sub1;
  time = sub2;

}

Time ObservationMeta::iso8601ToTime (const String iso)
{
  String date,time;
  Int year=0,month=0,day=0,hour=0,min=0;
  Double sec=0.0;

  // extract date and time from the the iso8601 time string
  ObservationMeta::extractFromISO8601 (date,time,iso);
  Vector<String> subDate = CR::getSubstrings(date,"/");
  Vector<String> subTime = CR::getSubstrings(time,":");
  
  try {
    year  = atoi (subDate(0).c_str());
    month = atoi (subDate(1).c_str());
    day   = atoi (subDate(2).c_str());
    hour  = atoi (subTime(0).c_str());
    min   = atoi (subTime(1).c_str());
    sec   = atof (subTime(2).c_str());
  } catch (AipsError x) {
    cerr << "[ObservationMeta::iso8601ToMeasure] " << x.getMesg() << endl;
  }
  
  Time t (year,month,day,hour,min,sec);

  return t;
}


// =============================================================================
//
//  Data generated during the experiment
//
// =============================================================================

void ObservationMeta::setDatafiles (const Matrix<String>& datafiles)
{
  IPosition shape = datafiles.shape();  // [iterations,files]
  IPosition nofAntennas = antennas_.shape();

  if (shape(0) != iterations_) {
    cerr << "[ObservationMeta::setDatafiles] Warning, array size mismatch." << endl;
    cerr << " - Number of iterations . : " << iterations_ << endl;
    cerr << " - First input array axis : " << shape(0) << endl;
  }
  else if (shape(1) != nofAntennas(0)) {
    cerr << "[ObservationMeta::setDatafiles] Warning, array size mismatch." << endl;
    cerr << " - Number of antennae .... : " << nofAntennas(0) << endl;
    cerr << " - Second input array axis : " << shape(1) << endl;
  }
  else {
    datafiles_.resize(shape);
    datafiles_ = datafiles;
  }

}

Matrix<String> ObservationMeta::datafiles () { return datafiles_; }
  

// =============================================================================
//
//  Misc. member data
//
// =============================================================================

void ObservationMeta::owner (String owner) { owner_ = owner; }

void ObservationMeta::basename (String basename) { basename_ = basename; }

String ObservationMeta::basename () { return basename_; }

void ObservationMeta::description (String description) {
  description_ = description;
}

void ObservationMeta::iterations (int iterations) { 
  iterations_ = iterations;
  triggertime_.resize(iterations);
}

void ObservationMeta::interval (int interval) { interval_ = interval; }

int ObservationMeta::interval () { return interval_; }

void ObservationMeta::capturemode (String capturemode) {
  capturemode_ = capturemode;
}

void ObservationMeta::capturesize (String capturesize) {
  capturesize_ = capturesize;
}

String ObservationMeta::capturesize () { return capturesize_; }

void ObservationMeta::type (String type) { type_ = type; }

String ObservationMeta::type () { return type_; }

// Sign extension

void ObservationMeta::signextension (Bool signextension) {
  signextension_ = signextension;
}

void ObservationMeta::signextension (String s)
{
  Bool signextension;
  signextension = ObservationMeta::string2boolean (s);
  ObservationMeta::signextension (signextension);
}

Bool ObservationMeta::signextension () { return signextension_; }

// Skip capture

void ObservationMeta::skipcapture (Bool skipcapture) {
  skipcapture_ = skipcapture;
}

void ObservationMeta::skipcapture (String s)
{
  Bool skipcapture;
  skipcapture = ObservationMeta::string2boolean (s);
  ObservationMeta::skipcapture (skipcapture);
}

Bool ObservationMeta::skipcapture () { return skipcapture_; }

void ObservationMeta::observationID (const Int observationID) {
  observationID_ = observationID;
}

Int ObservationMeta::observationID () { return observationID_; }


// =============================================================================
//
//  Helper routines for conversion of data types
//
// =============================================================================

Bool ObservationMeta::string2boolean (String s)
{
  Bool boolean;

  if ((s == "True") || (s == "true") || (s == "1")) boolean = True;
  else if ((s == "False") || (s == "false") || (s == "0")) boolean = False;
  else boolean = True;

  return boolean;
}

Vector<Int> ObservationMeta::string2vector (String s)
{
  Vector<Int> offsets,vect;
  Int lengthString;
  Int nelem = 0;
  String tmp;
  Bool verboseON = False;

  // Determine the number of elements stored within the string
  lengthString = int(s.length());
  for (int i=0; i<lengthString; ++i) {
    tmp = s.substr(i,1);
    if (tmp == " ") ++nelem;
  }

  vect.resize(nelem);
  offsets.resize(nelem);
  vect = 0;

  nelem = 0;
  for (int i=0; i<int(s.length()); ++i) {
    tmp = s.substr(i,1);
    if (tmp == " ") {
      offsets(nelem) = i;
      ++nelem;
    }
  }
  
  // Parse the input string to extract the vector elements
  Int offset,length;
  for (int i=1; i<nelem; ++i) {
    offset = offsets(i-1)-1;
    length = offsets(i)-offsets(i-1)-1;
    if (length == 2) --offset;
    tmp = s.substr(offset,length);
    vect(i-1) = atoi (tmp.c_str());
  }
  offset = offsets(nelem-1);
  length = offsets(nelem-1)-offsets(nelem-2)-1;
  offset -= length;
  tmp = s.substr(offset,length);
  vect(nelem-1) = atoi (tmp.c_str());

  /* if required give some feedback */
  if (verboseON) {
    cout << "[ObservationMeta::string2vector]";
    cout << " Input string ......... : " << s << endl;
    cout << " Length of input string : " << int(s.length()) << endl;
    cout << " Number of elements ... : " << nelem << endl;
    cout << " Offsets for splitting  : " << offsets << endl;
  }

  return vect;
}

// ------------------------------------------------------------------ nameDatafile

String ObservationMeta::nameDatafile (Int iter,
				      Int ant)
{
  String file = datafiles_(iter,ant);
  return CR::fileFromPath(file);
}
