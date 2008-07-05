
#include <iostream>
#include <TROOT.h>
#include <TTree.h>
#include <TFile.h>
#include <time.h>

using namespace std;

//! some test algorithm of parsing the timestamp of the database
/*!
 simple in and output of timestamps.
*/


int main(int argc, char *argv[])
{
  char *RootFileName="$HOME/lopes/data/run6_backup_00.root";
  char timestamp[100];
  char Name[1024];
  
  TFile *file = new TFile (RootFileName,"READ");
  TTree *Tevent = (TTree*)file->Get("Tevent");
  
  unsigned int profile_id, event_id;
  
  Tevent->SetBranchAddress("channel_profile_id",&profile_id);
  Tevent->SetBranchAddress("event_id",&event_id);  
  Tevent->SetBranchAddress("timestamp",timestamp);
  
  int year=0, month=0, day=0, hour=0, min=0, sec=0, subsec=0;
  double SEC=0;
  
  
  Long64_t entries;
  entries=Tevent->GetEntries();
  for(int i=0; i<3; i++){
    Tevent->GetEntry(i*7);
    cout << "TS: " << timestamp << "\tCh: " << profile_id << "\tEvent_id: " << event_id << endl;
    sscanf (timestamp,"%d-%d-%d %d:%d:%lf",&year, &month, &day, &hour, &min, &SEC);
    sprintf(Name,"%lf",SEC);
    cout << "year: " << year << "\tmonth: " << month << "\tday: " << day ;
    cout << "\thour: " << hour << "\tmin: " << min << "\tsec: " << Name << endl;
    
     sec = (int)SEC;
  subsec = (int)((SEC-sec)*1000000);
  sprintf(Name,"Sec %i \tSubSec %i\n",sec,subsec);
  cout << Name;
 
  }
  
  time_t STARTime_t;
  struct tm STARTime;
  
  STARTime.tm_year = year-1900;
  STARTime.tm_mon = month-1;
  STARTime.tm_mday = day;
  STARTime.tm_hour = hour;
  STARTime.tm_min = min;
  STARTime.tm_sec = (int)SEC;
  
  STARTime_t = mktime(&STARTime);
  
  cout << "ctime " << ctime(&STARTime_t) << endl;
  cout << STARTime_t << endl;
  
  sec = (int)SEC;
  subsec = (int)((SEC-sec)*1000000);
  sprintf(Name,"Sec %i \tSubSec %i\n",sec,subsec);
  cout << Name;


  
}
