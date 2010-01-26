/***************************************************************************
 *
 *   Copyright (C) 1999 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "toa.h"
#include "polyco.h"
#include "tempo++.h"

#include "strutil.h"
#include "Error.h"

#include <iostream>
#include <algorithm>

#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>

using namespace std;

const float Tempo::toa::UNSET = -999.0;

Tempo::toa::toa (char* datastr)
{
  if (load (datastr) < 0)
    throw Error (FailedCall, "Tempo::toa (char*)", "load(%s) failed", datastr);
}

Tempo::toa::toa (Format fmt)
{
  init ();
  format = fmt;
}

Tempo::toa::toa (const toa & in_toa)
{
  init ();
  this->operator=(in_toa);
}

Tempo::toa& Tempo::toa::operator = (const toa & in_toa)
{
  if (this == &in_toa)
   return *this;

  channel = in_toa.channel;
  subint = in_toa.subint;
  frequency = in_toa.frequency;
  arrival = in_toa.arrival;
  error = in_toa.error; 
  telescope = in_toa.telescope;
  phs = in_toa.phs;
  dmc = in_toa.dmc;
  observatory[0] = in_toa.observatory[0];
  observatory[1] = in_toa.observatory[1]; 
  ston = in_toa.ston;
  pa = in_toa.pa;
  bw = in_toa.bw;
  dur = in_toa.dur;
  dm = in_toa.dm;

  ci = in_toa.ci;
  di = in_toa.di;
  
  auxinfo = in_toa.auxinfo;
  
  format = in_toa.format;
  state = in_toa.state;

  resid = in_toa.resid;

  phase_shift = in_toa.phase_shift;
  phase_info = in_toa.phase_info;

  return *this;
}

void Tempo::toa::set_telescope (const std::string& telcode)
{
  telescope = Tempo::code( telcode );

  if (verbose)
    cerr << "Tempo::toa::set_telescope name=" << telcode
	 << " code=" << telescope << endl;
}

// ////////////////////////////////////////////////////////////////////////
//
// Parkes_load,unload - load/unload TOA in the Parkes Format
// 
//   columns     item
//     1-1     must be blank
//     26-34   Observing frequency (MHz)
//     35-55   TOA (decimal point must be in column 42)
//     56-63   Phase offset (fraction of P0, added to TOA)
//     64-71   TOA uncertainty
//     80-80   Observatory (one-character code)
//
// ////////////////////////////////////////////////////////////////////////

int Tempo::toa::Parkes_load (const char* instring)
{
  if (verbose)
    cerr << "Tempo::toa::Parkes_load" << endl;

  destroy ();

  if ((instring[0]=='C') || (instring[0]=='c')) {
    state = Deleted;
    return parkes_parse (instring+27);
  }
  else {
    return parkes_parse (instring+26);
  }
}

int Tempo::toa::parkes_parse (const char* instring)
{
  int scanned = sscanf (instring, "%lf %s %f %f %c",
 			&frequency, datestr, &phs, &error, &telescope);

  if (state != Deleted && scanned < 5) {
    // an invalid line was not commented out
    if (verbose) cerr << "Tempo::toa::parkes_parse(char*) Error scan:"
		      << scanned << "/5 '" << instring << "'" << endl;
    return -1;
  }
  if (state == Deleted && scanned < 5) {
    // The TOA line is probably a comment
    frequency = 0.0;
    arrival.Construct("0.0");
    phs = 0.0;
    error = 0.0;
    telescope = 'z';
    auxinfo += instring;
    auxinfo = auxinfo.substr(0,auxinfo.length()-1);
    format = Comment;

    phase_shift = 0.0;
    phase_info = false;
    channel = 0;
    subint = 0;

    return 0;
  }
  if (arrival.Construct(datestr) < 0)  {
    if (verbose) cerr << "Tempo::toa::parkes_parse(char*) Error MJD parsing '" 
		      << datestr << "'" << endl;
    return -1;
  }

  if (!valid())
    return -1;

  format = Parkes;
  return 0;
}

string Tempo::toa::Psrclock_unload () const
{
  sizebuf (128);
  Parkes_unload (buffer);
  return string(buffer);
}

int Tempo::toa::Parkes_unload (char* outstring) const
{
  if (state == Deleted) {
    outstring[0]='C';
    outstring[1]=' ';
    if (auxinfo.length())
      strcpy (outstring+2, auxinfo.c_str()); 
    for (int ic=auxinfo.length()+2; ic<27; ic++)
      outstring [ic] = ' ';
    return parkes_out (outstring+26);
  }
  else {
    outstring[0]=' ';
    if (auxinfo.length())
      strcpy (outstring+1, auxinfo.c_str());
    for (int ic=auxinfo.length()+1; ic<26; ic++)
      outstring [ic] = ' ';
    return parkes_out (outstring+25);
  }
}

int Tempo::toa::parkes_out (char* outstring) const
{
  // output the basic line
  sprintf (datestr, "%8.7f", frequency);
  sprintf (outstring, " %8.8s  %s   %5.2f %7.2f        %1c",
 	   datestr, arrival.printdays(13).c_str(), phs, error, telescope);
  return 0;
}

int Tempo::toa::Parkes_unload (FILE* outstream) const
{
  sizebuf (128);
  Parkes_unload (buffer);
  fprintf (outstream, "%s\n", buffer);
  return 0;
}

// ////////////////////////////////////////////////////////////////////////
//
// Princeton_load,unload - load/unload TOA in the Princeton Format
// 
//   columns     item
//    1-1     Observatory (one-character code)
//    2-2     must be blank
//    16-24   Observing frequency (MHz)
//    25-44   TOA (decimal point must be in column 30 or column 31)
//    45-53   TOA uncertainty 
//    69-78   DM correction (pc cm^-3)
//
// ////////////////////////////////////////////////////////////////////////

int Tempo::toa::Princeton_load (const char* instring)
{
  if (verbose)
    cerr << "Tempo::toa::Princeton_load" << endl;

  destroy ();

  telescope = instring[0];

  int scanned = sscanf (instring+15, "%lf %s %f %f",
 			&frequency, datestr, &error, &dmc);

  if (scanned < 4) {
    // an invalid line was not commented out
    if (verbose) cerr << "Tempo::toa::Princeton_load(char*) Error scanning '"
		      << instring << "'" << endl;
    return -1;
  }
  if (arrival.Construct(datestr) < 0)  {
    if (verbose) cerr << "Tempo::toa::Princeton_load(char*) Error MJD parsing '"
		      << datestr << "'" << endl;
    return -1;
  }

  if (!valid())
    return -1;

  format = Princeton;
  return 0;
}

int Tempo::toa::Princeton_unload (char* outstring) const
{
  // Blank line w/ spaces
  memset(outstring, ' ', 78);
  outstring[78] = '\0';

  sprintf (outstring, "%c %13.13s %8.3f %19.19s  %7.3f               %9.5f",
      telescope, auxinfo.c_str(), frequency, 
      arrival.printdays(19).c_str(), error, dmc);
  return 0;
}

int Tempo::toa::Princeton_unload (FILE* outstream) const
{
  sizebuf (128);
  Princeton_unload (buffer);
  fprintf (outstream, "%s\n", buffer);
  return 0;
}

// ////////////////////////////////////////////////////////////////////////
//
// Tempo2_load,unload - load/unload TOA in the Tempo2 format
// 
//
// ////////////////////////////////////////////////////////////////////////

int Tempo::toa::Tempo2_unload (char* outstring) const
{

  // output the basic line
  string fname,flags; 

  fname = auxinfo.substr(0,auxinfo.find(" "));
  if (auxinfo.find(" ")!=string::npos) 
	  flags = auxinfo.substr(auxinfo.find(" "));

  bool subint_given = false;
  bool chan_given = false;

  string::size_type i_sub = flags.find("-subint");
  if (i_sub != string::npos) {
	  flags.erase(i_sub, 8); // remove '-subint' from flags
      subint_given = true;
  }

  string::size_type i_chan = flags.find("-chan");
  if (i_chan != string::npos) {
	  flags.erase(i_chan, 6); // remove '-chan' from flags
      chan_given = true;
  }

  sprintf(outstring, "%s %8.3f %s %7.3f ", fname.c_str(), frequency,
          arrival.printdays(13).c_str(), error);

  if (phase_info) {
      sprintf(outstring, "%s @", outstring);
  } else {
      sprintf(outstring, "%s %c", outstring, telescope);

      if (subint_given)
          sprintf(outstring, "%s -subint %d", outstring, subint);

      if (chan_given)
          sprintf(outstring, "%s -chan %d", outstring, channel);

      sprintf(outstring, "%s %s", outstring, flags.c_str());
  }

  return 0;
}

int Tempo::toa::Tempo2_unload (FILE* outstream) const
{
  sizebuf (128 + auxinfo.length());
  Tempo2_unload (buffer);
  fprintf (outstream, "%s\n", buffer);

  return 0;
}


// ////////////////////////////////////////////////////////////////////////
//
// Psrclock_load,unload - unload TOA in the Psrclock Format
// 
// ////////////////////////////////////////////////////////////////////////

int Tempo::toa::Psrclock_load (const char* instring)
{
  if (verbose)
    cerr << "Tempo::toa::Psrclock_load ";

  destroy ();

  string whitespace (" \n\t");
  
  string parse;
  if (instring[0] == 'C' || instring[0] == 'c') {
    parse = instring + 2;
    state = Deleted;
  }
  else {
    parse = instring + 1;
  }  
  
  if (parse.length() < 25) {
    auxinfo = parse;
    auxinfo = auxinfo.substr(0,auxinfo.length()-1);
    format = Comment;
    cerr << "Parsing a comment" << endl;
    return 0;
  }
  else
    auxinfo = parse.substr(0,24);
  
  if (verbose)
    cerr << "Tempo::toa::Psrclock_load Parkes format = '" 
	 << parse << "'" << endl;
  
  if (state == Deleted) {
    if (parkes_parse (instring+26) < 0)
      return -1;
  }
  else {
    if (parkes_parse (instring+25) < 0)
      return -1;
  }
  
  // LOAD AUX
  format = Psrclock;

  if (verbose) {
    cerr << "Tempo::toa::Psrclock_loaded ";
    unload (stderr);
  }
  return 0;
}

int Tempo::toa::Psrclock_unload (char* outstring) const
{
  if (state == Deleted) {
    outstring[0]='C';
    outstring[1]=' ';
    sprintf (outstring+2, "%s", auxinfo.c_str());
    for (int ic=auxinfo.length()+2; ic<27; ic++)
      outstring [ic] = ' ';
    return parkes_out (outstring + 26);
  }
  else {
    outstring[0]=' ';
    sprintf (outstring+1, "%s", auxinfo.c_str());
    for (int ic=auxinfo.length()+1; ic<26; ic++)
      outstring [ic] = ' ';
    return parkes_out (outstring + 25);
  }
}

int Tempo::toa::Psrclock_unload (FILE* outstream) const
{
  sizebuf (82 + auxinfo.length());
  Psrclock_unload (buffer);
  fprintf (outstream, "%s\n", buffer);
  return 0;
}

// ////////////////////////////////////////////////////////////////////////
//
// Deal with TEMPO commands (eg. jumps)
//
// ////////////////////////////////////////////////////////////////////////

int Tempo::toa::Command_load (const char* instring)
{
  init();

  format = Command;
  string temp = instring;
  auxinfo = temp.substr(0, temp.length()-1);

  return 0;
}

int Tempo::toa::Command_unload (char* outstring) const
{
  if ( sprintf(outstring, "%s", auxinfo.c_str()) > 1 )
    return 0;
  else
    return -1;
}

int Tempo::toa::Command_unload (FILE* outstream) const
{
  sizebuf (81 + auxinfo.length());
  Command_unload (buffer);
  fprintf (outstream, "%s\n", buffer);
  return 0;
}

int Tempo::toa::Comment_unload (char* outstring) const
{
  if ( sprintf(outstring, "C %s", auxinfo.c_str()) > 1 )
    return 0;
  else
    return -1;
}

int Tempo::toa::Comment_unload (FILE* outstream) const
{
  sizebuf (82 + auxinfo.length());
  Comment_unload (buffer);
  fprintf (outstream, "%s\n", buffer);
  return 0;
}

// ////////////////////////////////////////////////////////////////////////
//
// generic load,unload - load/unload TOA in appropriate format
//
// these functions will usually be called at the high level
// 
// ////////////////////////////////////////////////////////////////////////

int Tempo::toa::load (const char* instring)
{
  if ( !instring )
    return -1;
  
  if ( instring[0] == 'J' || instring[0] == 'S' || instring[0] == 'M' || 
       instring[0] == 'E' || instring[0] == 'D' || instring[0] == 'F' ||
       instring[0] == 'I' || instring[0] == 'N' || instring[0] == 'P' ||
       instring[0] == 'T' || instring[0] == 'Z' )
    return Command_load( instring );
  
  else if ( isdigit( instring[0] ) )
    return Princeton_load( instring );
  
  else
    return Psrclock_load( instring );

  //else
  //  return Parkes_load( instring );
}

// Return values:
//    0 if toa was loaded, 
//    1 if end of file reached, 
//   -1 if error occurs

int Tempo::toa::load (FILE * instream)
{
  // start with a length of line
  size_t chunk = 128;
  sizebuf (chunk);

  buffer[0] = '\0';

  char*  startbuf = buffer;
  size_t inbuf = 0;

  while (fgets (startbuf, int(bufsz-inbuf), instream) != NULL) {
    size_t rlen = strlen (buffer);
    if ((rlen < bufsz-1) || (buffer[rlen-1] == '\n'))
      break;

    // the buffer was filled but a newline was not encountered
    inbuf = bufsz-1;
    sizebuf (bufsz+chunk);
    startbuf = buffer + inbuf;
  }
  if (ferror (instream)) {
    perror ("Tempo::toa::load(FILE*) error");
    return -1;
  }
  if (buffer[0] == '\0')
    return 1;

  if (verbose)
    cerr << "Tempo::toa::load(FILE*) to parse '" << buffer << "'" << endl;

  return load (buffer);
}

int Tempo::toa::unload (FILE* outstream, Format fmt) const
{
  if (fmt == Unspecified)
    fmt = format;
  
  switch (fmt) {
  case Comment:
    if (verbose) cerr << "Unloading Comment Format" << endl;
    return Comment_unload (outstream);
  case Command:
    if (verbose) cerr << "Unloading Command Format" << endl;
    return Command_unload (outstream);
  case Parkes:
    if (verbose) cerr << "Unloading Parkes Format" << endl;
    return Parkes_unload (outstream);
  case Princeton:
    if (verbose) cerr << "Unloading Princeton Format" << endl;
    return Princeton_unload (outstream);
  case Psrclock:
    if (verbose) cerr << "Unloading Psrclock Format" << endl;
    return Psrclock_unload (outstream);
  case Tempo2:
    if (verbose) cerr << "Unloading Tempo2 Format" << endl;
    return Tempo2_unload (outstream);
  default:
    cerr << "Tempo::toa::unload undefined format" << endl;
    return -1;
  }
}

int Tempo::toa::unload (char* outstring, Format fmt) const
{
  if (fmt == Unspecified)
    fmt = format;

  switch (fmt) {
  case Comment:
    if (verbose) cerr << "Unloading Comment Format" << endl;
    return Comment_unload (outstring);
  case Command:
    if (verbose) cerr << "Unloading Command Format" << endl;
    return Command_unload (outstring);
  case Parkes:
    if (verbose) cerr << "Unloading Parkes Format" << endl;
    return Parkes_unload (outstring);
  case Princeton:
    if (verbose) cerr << "Unloading Princeton Format" << endl;
    return Princeton_unload (outstring);
  case Psrclock:
    if (verbose) cerr << "Unloading Psrclock Format" << endl;
    return Psrclock_unload (outstring);
  case Tempo2:
    if (verbose) cerr << "Unloading Tempo2 Format" << endl;
    return Tempo2_unload (outstring);
  default:
    cerr << "Tempo::toa::unload undefined format" << endl;
    return -1;
  }
}

// ////////////////////////////////////////////////////////////////////////
//
// Tempo_unload - unload minimal TOA data in appropriate format
// 
// ////////////////////////////////////////////////////////////////////////

int Tempo::toa::Tempo_unload (FILE* outstream) const
{
  switch (format) {
  case Comment:
    return Comment_unload (outstream);  
  case Command:
    return Command_unload (outstream);
  case Parkes:
    return Parkes_unload (outstream);
  case Psrclock:
    return Parkes_unload (outstream);
  case Princeton:
    return Princeton_unload (outstream);
  case Tempo2:
    return Tempo2_unload (outstream);
  default:
    if (verbose) 
      cerr << "Tempo::toa::Tempo_unload undefined format" << endl;
    return -1;
  }
}

int Tempo::toa::Tempo_unload (char* outstring) const
{
  switch (format) {
  case Comment:
    return Comment_unload (outstring);
  case Command:
    return Command_unload (outstring);
  case Parkes:
    return Parkes_unload (outstring);
  case Psrclock:
    return Parkes_unload (outstring);
  case Princeton:
    return Princeton_unload (outstring);
  case Tempo2:
    return Tempo2_unload (outstring);
  default:
    if (verbose) 
      cerr << "Tempo::toa::Tempo_unload undefined format" << endl;
    return -1;
  }
}

#if POLYCO_INTERFACE_RESTORED

// ////////////////////////////////////////////////////////////////////////
//
// Tempo::toa::shift
//
// shifts a time of arrival to the nearest zero phase
// 
// ////////////////////////////////////////////////////////////////////////

double Tempo::toa::shift (const polyco& poly) const
{ 
  double toashift = poly.phase (this->arrival, frequency).fracturns();
  if (toashift >.5) toashift -= 1.0;
  return (toashift*poly.period (this->arrival));
}

#endif

// ////////////////////////////////////////////////////////////////////////
//
// vector load/unload - load vector of toa objects from file
//
// ////////////////////////////////////////////////////////////////////////

int Tempo::toa::load (const char* filename, vector<toa>* toas)
{
  FILE* fp = fopen(filename, "r");
  if (fp==NULL){
    cerr << "\n\nTempo::toa::load - error opening '" << filename << "'::";
    perror ("");
    cerr << endl;
    return -1;
  }
  int ret = load (fp, toas);
  fclose (fp);
  return ret;
}

int Tempo::toa::load (FILE* instream, vector<toa>* toas)
{
  toa tmp_toa;
  while (tmp_toa.load(instream) == 0)
    toas->push_back(tmp_toa);
  return 0;
}

int Tempo::toa::unload(const char* filename, const vector<toa>& toas, Format fmt)
{
  FILE * fp;

  if((fp = fopen(filename, "w"))==NULL){
    cerr << "Tempo::toa::unload - error opening file " << filename << endl;
    return -1;
  }
  int ret = unload (fp, toas, fmt);
  fclose(fp);
  return ret;
}

int Tempo::toa::unload(FILE* outstream, const vector<toa>& toas, Format fmt)
{
  for (unsigned i=0; i<toas.size(); ++i) {
    if (verbose)
      cerr << "Tempo::toa::unload - unloading toa " << i << endl;
    if (toas[i].unload(outstream, fmt) < 0)
      return -1;
  }
  return 0;
}

// ////////////////////////////////////////////////////////////////////////
//
// low-level toa stuff
// 
// ////////////////////////////////////////////////////////////////////////

bool   Tempo::toa::verbose = false;
char*  Tempo::toa::buffer = NULL;
size_t Tempo::toa::bufsz = 0;
char   Tempo::toa::datestr [25];

void Tempo::toa::init()
{
  channel = 0;
  subint = 0;
  phase_shift = 0.0;
  phase_info = false;

  format = Unspecified;
  // auxdata = NULL;
  resid.valid = false;

  frequency       = 0.0;
  arrival         = MJD (0.0,0.0,0.0);
  error           = 0.0;
  telescope       = '0';
  phs             = 0.0;
  dmc             = 0.0;
  observatory [0] = '\0';

  ston = UNSET;
  pa   = UNSET;
  bw   = UNSET;
  dur  = UNSET;
  dm   = UNSET;

  ci = 7;
  di = 0;

  auxinfo.erase();

  state = Tempo::toa::Normal;
}

void Tempo::toa::destroy()
{
  // if (auxdata.is_only())
    // delete (toaInfo*) auxdata;

  // auxdata = NULL;
  init ();
}

void Tempo::toa::sizebuf (size_t length)
{
  if (bufsz < length) {
    void* temp = realloc (buffer, length);
    if (temp == NULL) {
      cerr << "Tempo::toa::sizebuf Cannot allocate buffer space" << endl;
      throw ("bad_alloc");
    }
    buffer = (char*) temp;
    bufsz = length;
  }
}

bool Tempo::toa::valid()
{
  if (format == Command)
    return 1;
  
  if (frequency < 20.0) {
    if (verbose) cerr << "Tempo::toa::load(char*) Error: FREQUENCY=" << frequency
		      << " is too small." << endl;
    return 0;
  }
  if (arrival < MJD (40000,0,0.0)) {
    if (verbose) cerr << "Tempo::toa::load(char*) Error: MJD=" << arrival 
		      << " is too small" << endl;
    return 0;
  }
  if ((error < 0.0) || (error > 999999999.0)) {
    if (verbose) cerr << "Tempo::toa::load(char*) Error: ERROR=" << error 
		      << " is weird." << endl;
    return 0;
  }

  // passed all tests, return true
  return 1;
}
