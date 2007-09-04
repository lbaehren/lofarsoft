
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <fstream>
#include <libgen.h>

#include <TROOT.h>
#include <TTree.h>
#include <TFile.h>

#include <libpq-fe.h>

#include <LopesStar/util.h>

using namespace std;


//! Writes events from database to a csv file
/*!
 Write database information from several events to a csv file. 
 Test data for Oliver
 
 \n Explanation of some variables:
 \n\b event_begin - event id to start with
 \n\b event_stop - event id to stop with
*/



int main(int argc, char *argv[])
{
  char *csvOutFileName="$HOME/analysis/csvOliver_Run5_Mitte_100Events.txt";
  char Command[1024];
  char Command2[1024];
  char timestamp[100];
  int window=1024, event_id, run_id, channel_profile_id;
  Int_t channel_id=0, channel_no=0, count=0;

//******************************
  UInt_t event_begin=390000;	
  UInt_t event_end=390100;	
  Int_t noOfAntennas=7;		
//******************************

  if (event_end<event_begin) {cerr << "check begin and end\n" ; exit(0);}
  ofstream csvFile;
  csvFile.open(csvOutFileName);
  sprintf(Command,"csv File Format Version 2\ndatabase antenna data to csv for Oliver\n\n");
  csvFile << Command;
  sprintf(Command, "Ch 1\tCh 2\tCh 3\t Ch 4\tCh 5\t Ch 6\tCh 7\t Ch 8\n");
  csvFile << Command;
  

  PGresult *res;
  PGresult *res2;  
  PGresult *res3;  
  Int_t  Nrows, Nrows2;
  size_t length;
  Short_t *DTrace;
  UInt_t c=0;
  
  //set up the db connection
  PGconn *conn;
#if 0 
#define CONN 
  conn = PQconnectdb("host=localhost port=5432 dbname=radac user=web-radac password=test123");
#else
  //conn = PQconnectdb("host=ipekroemerstar1.ka.fzk.de port=5432 dbname=radac user=radac password=test");
  //conn = PQconnectdb("host=localhost port=5432 dbname=radac user=radac password=test");
  conn = PQconnectdb("host=ipelopes1.ka.fzk.de port=5432 dbname=radac user=web-radac password=test123");
#endif  


  if(PQstatus(conn) == CONNECTION_BAD){
    cout << "Bad connection ... check settings." << endl;
    exit(1);
  }

  PQexec(conn,"BEGIN;");

  sprintf(Command2,"SELECT size FROM window_sizes, profiles, runs, events WHERE run_id=runs.id "
  		  "AND runs.profile_id = profiles.id AND window_size_id = window_sizes.id "
		  "AND events.id = %u;",event_begin);
  
  res3 = PQexec(conn,Command2);
  ResCheck(res3);
  window = atoi( PQgetvalue(res3,0,0) );
  cout << "window " << window << endl;
  PQfreemem(res3);

  UShort_t trace[9][window];
  for(int i=0; i<9; i++) for(int j=0; j<window; j++) trace[i][j]=0;

  res2 = PQexec(conn,"SELECT id, channel_no FROM channel_profiles;");
  ResCheck(res2);
  Nrows2 = PQntuples(res2);
  

  
  
  sprintf(Command2,"SELECT event_id, run_id, time, channel_profile_id, trace "
  		  "FROM samples, events WHERE (event_id BETWEEN %u AND %u) "
		  "AND events.id=samples.event_id ORDER BY samples.id;",event_begin, event_end);
  

  
  res = PQexec(conn,Command2);
  ResCheck(res);
  Nrows = PQntuples(res);
  
  for(Int_t i=0; i<Nrows; i++){
    event_id = atoi (PQgetvalue(res,i,0));
    run_id = atoi (PQgetvalue(res,i,1));
    strcpy(timestamp, PQgetvalue(res,i,2) );
    channel_profile_id = atoi (PQgetvalue(res,i,3));
    //cout << "channel_profile_id " << channel_profile_id << endl;

    for(Int_t w=0; w<Nrows2; w++){
      channel_id = atoi (PQgetvalue(res2,w,0));
      channel_no = atoi (PQgetvalue(res2,w,1));
      //cout << "channel_id " << channel_id << " - channel_no " << channel_no << endl;
      if(channel_id == channel_profile_id) {c = channel_no; /*cout << "channel " << c << endl;*/}
    }
    
    
    length = (size_t) PQgetlength(res,i,4);
    DTrace = (Short_t*) PQunescapeBytea((const unsigned char*) PQgetvalue(res,i,4), &length );
   
    //for(int q=0; q<10; q++)  cout << hex << "0x" << DTrace[q] << " " ; cout << dec << endl;
    if(DTrace[0]!=0x4711) {
       cerr << "Dataformat error ...";
       exit(1);
    }
    // cut off the first element after checked it.
    for(Int_t j=0; j<window;j++ ) trace[c][j] =  DTrace[j+1];
    PQfreemem(DTrace); 
    
    count++;
    if(count==noOfAntennas){
      count=0;
      csvFile << "\nEvent ID: " << event_id << "\ttimestamp: " << timestamp << endl;
      for(int u=0; u<window; u++){
        sprintf(Command,"%i\t%i\t%i\t%i\t%i\t%i\t0\t%i\n",trace[1][u],trace[2][u],trace[3][u],trace[4][u],trace[5][u],trace[6][u],trace[8][u]);
        csvFile << Command;
      }

    }
    
    
  }
  
  PQexec(conn,"COMMIT;");
  PQfinish(conn);


  csvFile.close();
  PQfreemem(res);
  PQfreemem(res2);

}
