#include <iostream>
#include <string>
#include <sstream>
#include <list>

#include <Analysis/analyseLOPESevent2.h>
#include <crtools.h>

using CR::analyseLOPESevent2;
using CR::LopesEventIn;

/*!
  \file simpleSkymap.cc

  \ingroup CR_Applications

  \brief Generates a skymap from a given LOPES-event"
 
  \author Moses Ender
 
  \date 2009/03/08

  <h3>Usage</h3>

  \verbatim
   ./simpleSkymap -c config.txt lopes1.event [ lopes2.event ...]
  \endverbatim
  
  At least one event is required but you can also provide several events at once.
 
  <h3>Content of config-file</h3>
  beamtype = CC
  numberOfBlocks = 1024 
  startBlock = 1
  stopBlock = 1
  az_start = 310
  az_end = 360
  az_incr = 0.5
  ze_start = 60
  ze_end = 90
  ze_incr = 0.5
  distance = 4000
  caltablepath = /home/ender/Software/usg-skymapper/data/lopes/LOPES-CalTable

  numberOfBlocks fixes the blocksize. startBlock and stopBlock specify the timerange inside the event.

*/


int main(int argc, char *argv[])
{
  int status (0);

  // description or help
  if(argc<2||string(argv[1]).find("help") != string::npos)
  {
    cout<<"simpleSkymap:"<<endl
        <<"Usage: ./simpleSkymap -c config.txt lopes1.event [ lopes2.event ...]"<<endl<<endl;
    exit(1);
  }

  try{
  string configfile="";
  list<string> fnames;

  // read command line arguments
  for(int i=1; i<argc; i++)
    if(string(argv[i]).compare("-c")==0) 
	configfile=argv[++i];
    else
    {
	fnames.push_back(string(argv[i]));

	//check if arguments are lopes events
	if(fnames.back().find(".event")==string::npos)
	{
	    cout<<fnames.back()<<" does not seem to be a LOPES event file\n";
	    fnames.pop_back();
	}
    }


  // Default configuration values
  double az_min = 0,
         az_max = 360,
         az_d = 1.,
         ze_min = 0,
         ze_max = 90,
         ze_d = 1.,
  distance = 10000.;
  int tvcal = 0;
  string beamtype="CC";
  String caltablepath="/home/ender/Software/usg/data/lopes/LOPES-CalTable";
  Vector<Int> FlaggedAntIDs;
  Vector<Bool> AntennaSelection;
  bool hanning = 0;
  int noblocks = 4,
      startblock = 1,
      endblock = 1;


  if(!configfile.empty())
  {
    // parsing config file ...
    ifstream cfile(configfile.c_str());
    char buf[256];
    stringstream tmp;
    string tmpstr;
    while(cfile.getline(buf,256))
    {
      tmp.clear();
      tmp.str(buf);
      tmp>>tmpstr;

      if(tmpstr.compare("beamtype")==0) {tmp>>tmpstr>>beamtype; continue;}
      if(tmpstr.compare("numberOfBlocks")==0) {tmp>>tmpstr>>noblocks; continue;}
      if(tmpstr.compare("startBlock")==0) {tmp>>tmpstr>>startblock; continue;}
      if(tmpstr.compare("stopBlock")==0) {tmp>>tmpstr>>endblock; continue;}
      if(tmpstr.compare("az_start")==0) {tmp>>tmpstr>>az_min; continue;}
      if(tmpstr.compare("az_end")==0) {tmp>>tmpstr>>az_max; continue;}
      if(tmpstr.compare("az_incr")==0) {tmp>>tmpstr>>az_d; continue;}
      if(tmpstr.compare("ze_start")==0) {tmp>>tmpstr>>ze_min; continue;}
      if(tmpstr.compare("ze_end")==0) {tmp>>tmpstr>>ze_max; continue;}
      if(tmpstr.compare("ze_incr")==0) {tmp>>tmpstr>>ze_d; continue;}
      if(tmpstr.compare("distance")==0) {tmp>>tmpstr>>distance; continue;}
      if(tmpstr.compare("caltablepath")==0) {tmp>>tmpstr>>caltablepath; continue;}
      if(tmpstr.compare("TVcal")==0) {tmp>>tmpstr>>tvcal; continue;}
      if(tmpstr.compare("useHanningFilter")==0) {tmp>>tmpstr>>hanning; continue;}
      cout<<"Option unkown: "<<tmpstr<<endl;
    }

  }
  
  // perform some checks...
  if(endblock<startblock) { cerr<<"simpleSkymap: error: end block before start block\n"; exit(1);}
  if(az_max<az_min) { cerr<<"simpleSkymap: error: az start larger then az stop\n"; exit(1);}
  if(ze_max<ze_min) { cerr<<"simpleSkymap: error: ze start larger then ze stop\n"; exit(1);}


  // Set observatory record to LOPES
  Record obsrec;
  obsrec.define("LOPES",caltablepath);

  analyseLOPESevent2 event;
  event.setPlotInterval(-80e-6,80e-6);
  event.initPipeline(obsrec);

  // loop over all events in list fnames...
  while(fnames.size()!=0)
  {
    cout<<"processing "<<fnames.front()<<endl;

    // setup event
    event.SetupEvent(fnames.front(), tvcal, FlaggedAntIDs, AntennaSelection, 80.e6 ,0., true, true, true, true, true, false, false );

    // generate skymap
    event.simpleSkymap(fnames.front(), noblocks, startblock, endblock, az_min, az_max, az_d, ze_min, ze_max, ze_d, AntennaSelection, distance, beamtype, hanning);

    //remove first element from filename list
    fnames.pop_front();
  }

  cout<<"No more events in filename list...\n";
  }catch(AipsError x) 
  {
    cerr << "simpleSkymap: " << x.getMesg() << endl;
    status=1;
  }
  return status;
}
