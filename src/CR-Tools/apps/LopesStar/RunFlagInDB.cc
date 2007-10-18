#include <iostream>

#include <TROOT.h>
#include <TFile.h>
#include <TH2F.h>
#include <TChain.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TGraph.h>
#include <TStopwatch.h>

#include <libpq-fe.h>

#include <LopesStar/util.h>

using namespace std;

//! run flag input into DB
/*!
copies the result from RunFlagOut into the database
*/



int main(int argc, char *argv[])
{
  setenv("TZ","UTC",1);
  tzset();


  printf("Asch@ipe.fzk.de (build %s %s)\n", __DATE__, __TIME__);  

  
  char *RunFlagName="$HOME/analyisis/muell.root";
  int daq_id = 0;

  
  
  int c;
  opterr = 0;

  while ((c = getopt (argc, argv, "hi:d:")) != -1) //: after var ->Waits for input
    switch (c)
      {
     case 'd':
	daq_id = atoi (optarg) ;
	break;
     case 'i':
	RunFlagName = optarg ;
	break;
      case '?':
        if (isprint (optopt))
          fprintf (stderr, "Unknown option `-%c'.\nTry '-h'\n", optopt);
        else
          fprintf (stderr,
                   "Unknown option character `\\x%x'.\nTry '-h'\n",
                   optopt);
        return 1;
      case 'h':
        cout << endl;
	cout << " Overview: run flag input into DB\n";
	cout << "=================================\n\n";
	cout << " -h	HELP -- you already tried :-)\n";
	cout << " -i	input file name in RunFlagOut format (default: " << RunFlagName << ")\n";
	cout << " -d	DAQ number of the database to write in\n";
	cout << "\n\nmailto: asch@ipe.fzk.de\n";
	exit(1);
        break;      
      default:
	abort ();
      }


  if(daq_id==0) {
   cerr << "DAQ ID is missing - try the -d option" << endl;
   exit(1);
  }
  
  TChain *TAnaFlag = new TChain("TAnaFlag");
  CreateTChain(TAnaFlag, RunFlagName);

  struct AnaFlag AnaFlag;
  CreateTAnaFlag(TAnaFlag, &AnaFlag);
  

//set up the database communication
  PGconn *conn;
  if(daq_id==17) conn = PQconnectdb("host=ipelopes1.ka.fzk.de port=5432 dbname=radac user=radac password=test");
  if(daq_id==19) conn = PQconnectdb("host=ipelopes_c19.ka.fzk.de port=5432 dbname=radac user=web-radac password=test123");

  if(PQstatus(conn) == CONNECTION_BAD){
    cout << "Bad connection ... check settings." << endl;
    exit(1);
  }
  else cout << "Connection .. done" << endl;
  
  PGresult *res;
  char Command[1024];
  
  // setup timer
  TStopwatch timer;
  timer.Start();


  res = PQexec(conn,"BEGIN;");
#ifdef HAVE_POSTGRESQL  
  ResCheck(res);
#endif
  PQfreemem(res);
  
  for(int i=0; i<TAnaFlag->GetEntries(); i++){
    TAnaFlag->GetEntry(i);
    
    sprintf(Command,"INSERT INTO scratch.flags_events (event_id, flag_id, detect_channels) VALUES (%d , %d , %d);"
    		,AnaFlag.event_id, AnaFlag.ana_index, AnaFlag.DetCh);

    res = PQexec(conn,Command);
    ResCheck(res);
    PQfreemem(res);
  
    for(int j=0; j<AnaFlag.DetCh; j++){
      sprintf(Command,"INSERT INTO scratch.flags_samples (event_id, channel_profile_id) VALUES (%d , %d);"
    		,AnaFlag.event_id, AnaFlag.channel_id[j]);
		

      res = PQexec(conn,Command);
      ResCheck(res);
      PQfreemem(res);
  
      
    }//end for over j
  }//end for over i
  
  res = PQexec(conn,"COMMIT;");
  ResCheck(res);
  PQfreemem(res);
  
  PQfinish(conn);

  // stop timer and print results
   timer.Stop();
   Double_t rtime = timer.RealTime();
   Double_t ctime = timer.CpuTime();
   printf("\nRealTime=%f seconds, CpuTime=%f seconds\n", rtime, ctime);
  
  //do not forget to clean up
  delete TAnaFlag;
}
