/***************************************************************************
 *
 *   Copyright (C) 1999 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "psrephem.h"
#include "tempo++.h"

#include "ephio.h"
#include "strutil.h"
#include "dirutil.h"

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>

#include <iostream>
using namespace std;

Legacy::psrephem::psrephem()
{
  init ();
  zero ();
}

Legacy::psrephem::~psrephem()
{
  destroy();
}

bool Pulsar::Parameters::verbose = 0;

void Legacy::psrephem::init()
{
  nontempo11.erase();
  parmStatus    = NULL;
  value_str     = NULL;
  value_double  = NULL;
  value_integer = NULL;
  error_double  = NULL;

  tempo11 = false;
}

void Legacy::psrephem::size_dataspace()
{
//   if (!tempo11)  {
//     destroy ();
//     return;
//   }

  if (parmStatus != NULL)  {
    // the arrays have already been initialized.  zero them
    if (verbose)
      fprintf (stderr, "Legacy::psrephem::size_dataspace zero\n");
    zero_work();
    return;
  }
  
  parmStatus    = new int    [EPH_NUM_KEYS];
  value_double  = new double [EPH_NUM_KEYS];
  value_str     = new string [EPH_NUM_KEYS];
  value_integer = new int    [EPH_NUM_KEYS];
  error_double  = new double [EPH_NUM_KEYS];
  
  zero_work();
}

void Legacy::psrephem::zero()
{
  tempo11 = true;
  size_dataspace ();
}


void Legacy::psrephem::zero_work()
{
  string tempo_safe_string (" ");
  
  for (int i=0;i<EPH_NUM_KEYS;i++) {
    parmStatus   [i] = 0;
    value_double [i] = 0.0;
    value_integer[i] = 0;
    value_str    [i] = tempo_safe_string;
    error_double [i]  = 0.0;
  }
}

void Legacy::psrephem::destroy()
{
  if (parmStatus)  {
    delete [] parmStatus;
    delete [] value_double;
    delete [] value_integer;
    delete [] error_double;
    delete [] value_str;
  }
  init ();
}

Legacy::psrephem::psrephem (const char* filename)
{
  init();
  if (load (filename) < 0) {
    fprintf (stderr, "Legacy::psrephem::psrephem error loading %s.\n", filename);
    throw Error(FailedCall, "Legacy::psrephem::psrephem");
  }
}

int Legacy::psrephem::load (const std::string& filename)
{
  if (verbose) 
    cerr << "Legacy::psrephem::load enter" << endl;

  tempo11 = 1;
  size_dataspace();
  
  if (verbose)
    cerr << "Legacy::psrephem::load eph_rd (" << filename << ")" << endl;

  char ephemstr [EPH_NUM_KEYS][EPH_STR_LEN];

  eph_rd (const_cast<char*>(filename.c_str()),
	  parmStatus, ephemstr, value_double, 
	  value_integer, error_double);

  int all_zero = 1;
  for (int i=0;i<EPH_NUM_KEYS;i++)
  {
    if (parmStatus[i])
    {
      if (parmTypes[i] == EPH_TYPE_STRING)
	value_str[i] = ephemstr [i];
      all_zero = 0;
    }
  }
  if (all_zero) {
    if (verbose)
      fprintf (stderr, "Legacy::psrephem::load WARNING "
	       "tempo11-style load of '%s' failed\n", filename.c_str());
    return old_load (filename);
  }

  if (verbose)
    fprintf (stderr,"Legacy::psrephem::load tempo11-style loaded '%s' ok\n", filename.c_str());

  return 0;
}

int Legacy::psrephem::old_load (const std::string& filename)
{
  tempo11 = 0;
  size_dataspace();

  FILE* fptr = fopen(filename.c_str(),"r");
  if (fptr == NULL) {
    fprintf (stderr, "Legacy::psrephem::old_load error fopen(%s)", filename.c_str());
    perror (":");
  }
  if (stringload (&nontempo11, fptr) < 0)  {
    fprintf (stderr, "Legacy::psrephem::old_load error\n");
    return -1;
  }
  fclose(fptr);
  return 0;
}

int Legacy::psrephem::unload (const std::string& filename) const
{
//   if (!tempo11)
//     return old_unload (filename);

  if (verbose)
    cerr << "Legacy::psrephem::unload copying strings" << endl;

  char ephemstr [EPH_NUM_KEYS][EPH_STR_LEN];

  for (int ieph=0; ieph<EPH_NUM_KEYS; ieph++)
  {
    if (parmStatus[ieph] && parmTypes[ieph] == EPH_TYPE_STRING)
    {
      assert (value_str[ieph].length() < EPH_STR_LEN -1);
      strncpy (ephemstr[ieph], value_str[ieph].c_str(), EPH_STR_LEN);
      assert (ephemstr[ieph][EPH_STR_LEN-1] == '\0');
    }
    else
      ephemstr[ieph][0] = '\0';
  }

  if (verbose)
    cerr << "Legacy::psrephem::unload calling eph_wr" << endl;

  int istat = eph_wr (const_cast<char*>(filename.c_str()), parmStatus, ephemstr,
		      value_double, value_integer, error_double);
  if (!istat) {
    fprintf (stderr, "Legacy::psrephem::unload error eph_wr '%s'\n", filename.c_str());
    return -1;
  }

  if (verbose)
    fprintf (stderr, "Legacy::psrephem::unload unloaded %s ok\n", filename.c_str());

  return 0;
}

int Legacy::psrephem::old_unload (const std::string& filename) const
{
  if (tempo11)
    return -1;

  FILE* fptr = fopen (filename.c_str(),"w");
  if (fptr == NULL) {
    fprintf (stderr, "Legacy::psrephem::old_unload error fopen(%s)", filename.c_str());
    perror (":");
    return -1;
  }
  if (fwrite (nontempo11.c_str(), 1, nontempo11.length(), fptr) 
	< nontempo11.length())  {
    perror ("Legacy::psrephem::old_unload error");
    return -1;
  }
  fclose (fptr);
  return 0;
}


string Legacy::psrephem::psrname() const
{
  if (parmStatus[EPH_PSRJ]==1)
    return value_str[EPH_PSRJ];

  else if (parmStatus[EPH_PSRB]==1)		
    return value_str[EPH_PSRB];
 
  return "unknown";
}


double Legacy::psrephem::get_dm() const
{
  // Removed tempo11 check because it's now possible
  // to obtain dm for pre-tempo11 ephemeris files
  //  if (tempo11 && parmStatus[EPH_DM])
  if (parmStatus[EPH_DM])
    return value_double[EPH_DM];

  return 0;
}

void Legacy::psrephem::set_dm( double dm )
{
  if (!parmStatus[EPH_DM])
    parmStatus[EPH_DM] = 1;

  value_double[EPH_DM] = dm;
}

double Legacy::psrephem::jra() const
{
  if (parmStatus[EPH_RAJ])
    return value_double[EPH_RAJ];
  
  throw Error(InvalidParam, "Legacy::psrephem::jra",
	      "Error determining pulsar RA");
}

double Legacy::psrephem::jdec() const
{
  if (parmStatus[EPH_DECJ])
    return value_double[EPH_DECJ];
  
  throw Error(InvalidParam, "Legacy::psrephem::jdec",
	      "Error determining pulsar DEC");
}

double Legacy::psrephem::omega() const
{
  if (tempo11 && parmStatus[EPH_OM])
    return value_double[EPH_OM];
  
  return 0.0;
}

double Legacy::psrephem::omdot() const
{
  if (tempo11 && parmStatus[EPH_OMDOT])
    return value_double[EPH_OMDOT];

  return 0.0;  
}

double Legacy::psrephem::ecc() const
{
  if (tempo11 && parmStatus[EPH_E])
    return value_double[EPH_E];
  
  return 0.0;
}

double Legacy::psrephem::t0() const
{
  if (tempo11 && parmStatus[EPH_T0])
  {
    MJD current_epoch = MJD (value_integer[EPH_T0],
			     value_double [EPH_T0]);
    return current_epoch.in_days();
  }
  
  throw Error(InvalidParam, "Legacy::psrephem::t0",
	      "Error determining pulsar T0");
}

double Legacy::psrephem::a1() const
{
  if (tempo11 && parmStatus[EPH_A1])
    return (value_double[EPH_A1]);
  
  throw Error(InvalidParam, "Legacy::psrephem::a1",
	      "Error determining pulsar A1");
}

double Legacy::psrephem::pb() const
{
  if (tempo11 && parmStatus[EPH_PB])
    return (value_double[EPH_PB]);
  
  throw Error(InvalidParam, "Legacy::psrephem::pb",
	      "Error determining pulsar PB");
}

void Legacy::psrephem::nofit()
{
  if (!tempo11)
    return;

  for (int i=0;i<EPH_NUM_KEYS;i++) {
    if (parmStatus[i]==2) parmStatus[i]=1;
  }
}

void Legacy::psrephem::fitall()
{
  if (!tempo11)
    return;

  for (int i=0;i<EPH_NUM_KEYS;i++) {
    if (parmStatus[i]==1) parmStatus[i]=2;
  }
}

void Legacy::psrephem::efac (float fac)
{
  if (!tempo11)
    return;

  for (int i=0;i<EPH_NUM_KEYS;i++)
    if (parmStatus[i]==2) error_double[i]*=fac;
}

Pulsar::Parameters* Legacy::psrephem::clone () const
{
  return new psrephem (*this);
}

bool Legacy::psrephem::equals (const Pulsar::Parameters* p) const
{
  const psrephem* that = dynamic_cast<const psrephem*>(p);
  if (!that)
    return false;
  return *this == *that;
}

void Legacy::psrephem::load (FILE* fptr)
{
  string total;
  if (stringload (&total, fptr) < 0)
    throw Error (FailedSys, "Legacy::psrephem::load(FILE*)", "stringload");

  load (&total);
}

void Legacy::psrephem::unload (FILE* fptr) const
{
  string out;
  if (!tempo11)
    out = nontempo11;
  else if (unload(&out) < 0)
    throw Error (FailedCall, "Legacy::psrephem::unload(FILE*)",
		 "unload(string*) failed");

  size_t size = out.length();
  size_t bout = fwrite (out.c_str(), 1, size, fptr);
  if (bout < size)
    throw Error (FailedSys, "Legacy::psrephem::unload(FILE*)", "fprintf");

  fflush (fptr);
}

//! Return the name of the source
std::string Legacy::psrephem::get_name () const
{
  return psrname ();
}
  
//! Return the coordinates of the source
sky_coord Legacy::psrephem::get_coordinates () const
{
  return sky_coord ( jra()*2.0*M_PI, jdec()*2.0*M_PI );
}

//! Return the dispersion measure
double Legacy::psrephem::get_dispersion_measure () const
{
  if (parmStatus[EPH_DM])
    return value_double[EPH_DM];

  return 0;
}

//! Return the rotation measure
double Legacy::psrephem::get_rotation_measure () const
{
  if (parmStatus[EPH_RM])
    return value_double[EPH_RM];

  return 0;
}

void f2cstr (char* str, unsigned length)
{
  unsigned i = length-1;
 
  while (str[i] == ' ')
  {
    str[i] = '\0';
    if (i == 0)
      break;
    i--;
  }
}

/* convertunits declaration removed from header file; needs config.h to be
   defined on every architecture */

#define convertunits F77_FUNC (convertunits,CONVERTUNITS)
extern "C" int convertunits (double* value, double* error,
			     int* status, int* convert);

int Legacy::psrephem::load (string* instr)
{
  if (verbose)
    cerr << "Legacy::psrephem::load (string*) *****" << endl
	 << *instr << "***** END" << endl;

  tempo11 = 1;
  size_dataspace();

  nontempo11 = *instr;  // just in case parsing fails
  int old_ephem = 0;

  vector<string> eph_lines;

  vector<char> ephemblock (EPH_NUM_KEYS * EPH_STR_LEN, ' ');
  vector<int>  correct (EPH_NUM_KEYS, 0);

  while (instr -> length() > 1)
  {
    // get the next line from the incoming text
    string line ( stringtok (*instr, "\n") );
    if (verbose)
      cerr << "Legacy::psrephem::load '" << line << "' len=" << line.length() 
	   << " instr.len=" << instr -> length() << endl;

    if (line.length() < 1)
      continue;

    // store the line in a vector for processing later
    eph_lines.push_back (line);

    eph_rd_str (parmStatus, &(ephemblock[0]), value_double, value_integer,
		error_double, &(correct[0]), &old_ephem, 
		const_cast<char*>( line.c_str() ));
  }

  // convertunits_ defined in ephio.f
  convertunits (value_double, error_double, parmStatus, &(correct[0]));
  if (verbose)
    cerr << "Legacy::psrephem::load units converted" << endl;

  bool all_zero = 1;
  for (int ieph=0; ieph<EPH_NUM_KEYS; ieph++)
  {
    if (parmStatus[ieph] == 0)
      continue;

    char* strval = &(ephemblock[ieph * EPH_STR_LEN]);
    f2cstr (strval, EPH_STR_LEN);
    value_str[ieph] = strval;

    all_zero = 0;
  }
  
  if (all_zero)
  {
    if (verbose)
      cerr << "Legacy::psrephem::load WARNING "
	"tempo11-style load of '" << nontempo11 << "' failed" << endl;
		
    // From here, need to parse the older style ephem
    read_old_ephem_str (eph_lines, parmStatus, 
                        value_str, value_double, value_integer);		
		
    tempo11 = false;
  }

  return 0;
}

void Legacy::psrephem::read_old_ephem_str(vector<string>& lines, 
                                  int *pstatus,
                                  string *val_str, 
																	double * val_double, 
																	int * val_int) {
	
	// Start at 1 because the first line is just a row of dashes -----
	int i = 1;

	////////////
	// Line 1
	
	// Name
	string psrname = lines[i].substr(0, 19);
	
	// get rid of white space
	string::size_type start = psrname.find_first_not_of (" \t");
	string::size_type end = psrname.find_last_not_of(" \t");
	
	if (start != string::npos && end != string::npos) {
		psrname = psrname.substr(start, end-start+1);
		pstatus[EPH_PSRB] = 1;
		val_str[EPH_PSRB] = psrname;
		
		if (psrname.length() > 7) {
			
			pstatus[EPH_PSRJ] = 1;
			val_str[EPH_PSRJ] = psrname;
		}

	}
	
	
	// RAJ
	string raj_str = lines[i].substr(19, 20);
	string::size_type j = raj_str.find(".", 0);
	
	if (j != string::npos) {
	
		string raj_sec_str = raj_str.substr(j-2, raj_str.length() - (j-2));
		string raj_min_str = raj_str.substr(j-4, 2);
		string raj_hour_str = raj_str.substr(0, 
		   raj_str.length() - (raj_sec_str.length() + raj_min_str.length()));

		double raj_hour = atof(raj_hour_str.c_str());
		double raj_min = atof(raj_min_str.c_str());
		double raj_sec = atof(raj_sec_str.c_str());

		raj_str = raj_hour_str + ":" + raj_min_str + ":" + raj_sec_str;
		double raj = (raj_hour + raj_min/60 + raj_sec/3600)/24;

		pstatus[EPH_RAJ] = 1;
		val_str[EPH_RAJ] = raj_str;
		val_double[EPH_RAJ] = raj;
	
	}
	
	// DECJ
	string decj_str = lines[i].substr(40, 20);
	j = decj_str.find(".", 0);
	
	if (j != string::npos) {
		string decj_sec_str = decj_str.substr(j-2, decj_str.length() - (j-2));
		string decj_min_str = decj_str.substr(j-4, 2);
		string decj_deg_str = decj_str.substr(0, 
		decj_str.length() - (decj_sec_str.length() + decj_min_str.length()));

		double decj_deg = atof(decj_deg_str.c_str());
		double decj_min = atof(decj_min_str.c_str());
		double decj_sec = atof(decj_sec_str.c_str());

		decj_str = decj_deg_str + ":" + decj_min_str + ":" + decj_sec_str;
		double decj = (decj_deg + decj_min/60 + decj_sec/3600) / 360;

		pstatus[EPH_DECJ] = 1;
		val_str[EPH_DECJ] = decj_str;
		val_double[EPH_DECJ] = decj;
	}
		
	// PMRA / PMDEC
	string pmra_str = lines[i].substr(60, 10);
	start = pmra_str.find_first_of ("1234567890");
	
	if (start != string::npos) {
		double pmra = atof(pmra_str.c_str());
		pstatus[EPH_PMRA] = 1;
		val_str[EPH_PMRA] = pmra_str;
		val_double[EPH_PMRA] = pmra;
		
	}
	
	string pmdec_str = lines[i].substr(70, 10);
	start = pmdec_str.find_first_of ("1234567890");
	
	if (start != string::npos) {
		double pmdec = atof(pmdec_str.c_str());
		pstatus[EPH_PMDEC] = 1;
		val_str[EPH_PMDEC] = pmdec_str;
		val_double[EPH_PMDEC] = pmdec;
	}
	
	
	
	i++;
	
	//////////////
	// Line 2
	
	string p0_str = lines[i].substr(1,19);
	start = p0_str.find_first_of ("1234567890");
	
	double p0 = 0;
	if (start != string::npos) {
		p0 = atof(p0_str.c_str());
		pstatus[EPH_F] = 1;
		val_str[EPH_F] = p0_str;
		val_double[EPH_F] = 1/p0;
	}
	
	string p1_str = lines[i].substr(20, 20);
	start = p1_str.find_first_of ("1234567890");
	double p1 = 0;
	if (start != string::npos) {
		p1 = atof(p1_str.c_str());
		pstatus[EPH_F1] = 1;
		val_str[EPH_F1] = p1_str;
		val_double[EPH_F1] = p1/pow(p0,2);
		
	}
		
	string pepoch_str = lines[i].substr(40, 20);
	start = pepoch_str.find_first_of ("1234567890");
	if (start != string::npos) {
		int pepoch = atoi(pepoch_str.c_str());
	
		pstatus[EPH_PEPOCH] = 1;
		val_str[EPH_PEPOCH] = pepoch_str;
		val_int[EPH_PEPOCH] = pepoch;
	}
	
	
	string p2_str = lines[i].substr(60, 10);
	start = p2_str.find_first_of ("1234567890");
	double p2 = 0;
	if (start != string::npos) {
		p2 = atof(p2_str.c_str());
		
		pstatus[EPH_F2] = 1;
		val_str[EPH_F2] = p2_str;
		
		
		val_double[EPH_F2] = -p2/pow(p0,2) + 2*pow(p1,2)/pow(p0,3);
		
	}
		
	string px_str = lines[i].substr(70, 10);
	start = px_str.find_first_of ("1234567890");
	if (start != string::npos) {
		double px = atof(px_str.c_str());
		
		pstatus[EPH_PX] = 1;
		val_str[EPH_PX] = px_str;
		val_double[EPH_PX] = px;
				
	}
	i++;
	
	////////////
	// Line 3
	
	
	string dm_str = lines[i].substr(8,12);
	start = dm_str.find_first_of ("1234567890");

	if (start != string::npos) {
		double dm = atof(dm_str.c_str());
		
		pstatus[EPH_DM] = 1;
		val_str[EPH_DM] = dm_str;
		val_double[EPH_DM] = dm;
		
	}
}																


int Legacy::psrephem::unload (string* outstr) const
{
  if (!tempo11)  {
    *outstr += nontempo11;
    return nontempo11.size();
  }

  if (parmStatus == NULL)
    return 0;

  const int buflen = 80;
  char buffer[buflen];
  string newline ("\n");

  char ephstr [EPH_STR_LEN];
  unsigned bytes_out = 0;

  for (int ieph=0; ieph < EPH_NUM_KEYS; ieph++)
  {
    if (!parmStatus[ieph])
      continue;

    if (parmTypes[ieph] == EPH_TYPE_STRING)
    {
      assert (value_str[ieph].length() < EPH_STR_LEN - 1);
      strncpy (ephstr, value_str[ieph].c_str(), EPH_STR_LEN);
      assert (ephstr[EPH_STR_LEN-1] == '\0');
    }
    else
      ephstr[0] = '\0';

    eph_wr_str (buffer, buflen, ieph, parmStatus[ieph], 
		ephstr, value_double[ieph], value_integer[ieph],
		error_double[ieph]);

    // if (verbose)
    //cerr << "   '" << buffer << "'" << endl;

    string lineout = buffer + newline;
    *outstr += lineout;
    bytes_out += lineout.size();
  }

  return bytes_out;
}

double Legacy::psrephem::p(void) const
{
  if (!tempo11)
    return -1;

  if ((value_double[EPH_F])!=0.0)  {
    return (1.0/value_double[EPH_F]);
  } 
  else
    fprintf (stderr,"Legacy::psrephem::p warning rotation frequency 0.0\n");
  return(1.0);
}

double Legacy::psrephem::p_err(void) const
{
  if (!tempo11)
    return -1;

  if ((value_double[EPH_F])!=0.0){
    return (-1.0/value_double[EPH_F]/value_double[EPH_F]
	    *error_double[EPH_F]);
  } else
    fprintf(stderr,"Legacy::psrephem::p_err warning rotation frequency 0.0\n");
  return(1.0);
}

Legacy::psrephem::psrephem (const psrephem & p2)
{
  init ();
  *this = p2;
}

Legacy::psrephem & Legacy::psrephem::operator = (const psrephem & p2)
{
  if (this == &p2)
    return *this;

  destroy();
  init();
//   if(!p2.parmStatus)
//     return(*this);

  tempo11 = p2.tempo11;
  size_dataspace();
  
//  if (tempo11) {
    for (int i=0;i<EPH_NUM_KEYS;i++){
      parmStatus[i]=p2.parmStatus[i];
      value_double[i]=p2.value_double[i];
      value_integer[i]=p2.value_integer[i];
      error_double[i]=p2.error_double[i];
      value_str[i]=p2.value_str[i];
    }
//   }
//   else {
    nontempo11 = p2.nontempo11;
//  }
  return *this;
}

bool operator == (const Legacy::psrephem &e1, const Legacy::psrephem &e2)
{
  if (e1.parmStatus==NULL && e2.parmStatus==NULL)
    return 1;

  if ((e1.parmStatus==NULL && e2.parmStatus!=NULL) ||
      (e1.parmStatus!=NULL && e2.parmStatus==NULL))
    return 0;

  if (e1.tempo11 != e2.tempo11) {
    if (Legacy::psrephem::verbose)
      cerr << "Legacy::psrephem::operator== unequal tempo11 flags" << endl;
    return 0;
  }

  if (!e1.tempo11)
    return e1.nontempo11 == e2.nontempo11;

  for (int ieph=0;ieph<EPH_NUM_KEYS;ieph++) {

    if (e1.parmStatus[ieph] != e2.parmStatus[ieph]) {
      if (Legacy::psrephem::verbose)
	cerr << "Legacy::psrephem::operator== unequal parmStatus["
	     << parmNames[ieph] << "]" << endl;
      return 0;
    }
    
    if (e1.value_double[ieph] != e2.value_double[ieph]) {
      if (Legacy::psrephem::verbose)
	cerr << "Legacy::psrephem::operator== unequal value_double["
	     << parmNames[ieph] << "]" << endl;
      return 0;
    }
    
    if (e1.value_integer[ieph] != e2.value_integer[ieph]) {
      if (Legacy::psrephem::verbose)
	cerr << "Legacy::psrephem::operator== unequal value_integer["
	     << parmNames[ieph] << "]" << endl;
      return 0;
    }
    
    if (e1.error_double[ieph] != e2.error_double[ieph]) {
      if (Legacy::psrephem::verbose)
	cerr << "Legacy::psrephem::operator== unequal value_integer["
	     << parmNames[ieph] << "]" << endl;
      return 0;
    }
    
    if (e1.value_str[ieph] != e2.value_str[ieph])  {
      if (Legacy::psrephem::verbose)
	cerr << "Legacy::psrephem::operator== unequal value_integer["
	     << parmNames[ieph] << "]" << endl;
      return 0;
    }
    
  }

  return true;

}

bool operator != (const Legacy::psrephem &e1, const Legacy::psrephem &e2) {
  return ! operator == (e1,e2);
}

ostream& operator<< (ostream& ostr, const Legacy::psrephem& eph)
{  
  string out;
  eph.unload (&out);
  ostr << out;
  return ostr;
}

// ///////////////////////////////////////////////////////////////////////
//
// set/get functions
//
// ///////////////////////////////////////////////////////////////////////

string Legacy::psrephem::get_string  (int ephind)
{
  if (parmTypes[ephind] != 0)
    throw Error (InvalidParam, "Legacy::psrephem::get_string",
                 "%s is not a string", parmNames[ephind]);

  if (tempo11 && parmStatus[ephind])
    return value_str[ephind];
  else
    return "";
}

double Legacy::psrephem::get_double  (int ephind)
{
  if (parmTypes[ephind] != 1)
    throw Error (InvalidParam, "Legacy::psrephem::get_double",
                 "%s is not a double", parmNames[ephind]);

  if (tempo11 && parmStatus[ephind])
    return value_double[ephind];
  else
    return 0.0;
}

MJD Legacy::psrephem::get_MJD (int ephind)
{
  if (parmTypes[ephind] != 4)
    throw Error (InvalidParam, "Legacy::psrephem::get_MJD",
                 "%s is not a MJD", parmNames[ephind]);

  if (tempo11 && parmStatus[ephind])
    return MJD (value_integer[ephind], value_double[ephind]);
  else
    return MJD::zero;
}

Angle Legacy::psrephem::get_Angle (int ephind)
{
  if (parmTypes[ephind] != 2 && parmTypes[ephind] != 3)
    throw Error (InvalidParam, "Legacy::psrephem::get_Angle",
                 "%s is not an  Angle", parmNames[ephind]);

  if (tempo11 && parmStatus[ephind]) {
    Angle ret;
    ret.setTurns (value_double[ephind]);
    return ret;
  }
  else
    return Angle();
}

int Legacy::psrephem::get_integer (int ephind)
{
  if (parmTypes[ephind] != 5)
    throw Error (InvalidParam, "Legacy::psrephem::get_integer",
                 "%s is not an integer", parmNames[ephind]);

  if (tempo11 && parmStatus[ephind])
    return value_integer[ephind];
  else
    return 0;
}

void Legacy::psrephem::set_string (int ephind, const string& value)
{
  if (parmTypes[ephind] != 0)
    throw Error (InvalidParam, "Legacy::psrephem::get_string",
                 "%s is not a string", parmNames[ephind]);

  value_str[ephind] = value;

  if (!parmStatus[ephind])
    parmStatus[ephind] = 1;
}

void Legacy::psrephem::set_double (int ephind, double value)
{
  if (parmTypes[ephind] != 1)
    throw Error (InvalidParam, "Legacy::psrephem::get_double",
                 "%s is not a double", parmNames[ephind]);

  value_double[ephind] = value;

  if (!parmStatus[ephind])
    parmStatus[ephind] = 1;
}

void Legacy::psrephem::set_MJD (int ephind, const MJD& value)
{
  if (parmTypes[ephind] != 4)
    throw Error (InvalidParam, "Legacy::psrephem::get_MJD",
                 "%s is not a MJD", parmNames[ephind]);

  value_integer[ephind] = value.intday();
  value_double[ephind] = value.fracday();

  if (!parmStatus[ephind])
    parmStatus[ephind] = 1;
}

void Legacy::psrephem::set_Angle (int ephind, const Angle& value)
{
  if (parmTypes[ephind] != 2 && parmTypes[ephind] != 3)
    throw Error (InvalidParam, "Legacy::psrephem::get_Angle",
                 "%s is not an  Angle", parmNames[ephind]);

  value_double[ephind] = value.getTurns();

  if (!parmStatus[ephind])
    parmStatus[ephind] = 1;
}

void Legacy::psrephem::set_integer (int ephind, int value)
{
  if (parmTypes[ephind] != 5)
    throw Error (InvalidParam, "Legacy::psrephem::get_integer",
                 "%s is not an integer", parmNames[ephind]);

  value_integer[ephind] = value;

  if (!parmStatus[ephind])
    parmStatus[ephind] = 1;
}

