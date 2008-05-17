#include <iostream.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

void TempLoggerHelp(){
  cout << "void CreateTempLoggerOutFile(char *outfilename=output_temp_logger.root)" << endl << endl;;
  cout << "FillTempLoggerTree(\nchar *outfilename = output_temp_logger.root,\nchar *infilename = temp.txt)" << endl << endl;
  cout << "PlotTempLoggerTree(char *inputfilename= output_temp_logger.root, char *location, int DAQ=0)" << endl << endl;

}

void CreateTempLoggerOutFile(char *outfilename="output_temp_logger.root"){
  
  TFile *output = new TFile(outfilename,"RECREATE","TempLoggerData");
  
  char location[128];
  char date[128];
  char time[128];
  int timestamp=0;
  float ftemp=0;
  int index=0;
  int daq=0;

  TTree *TempLogger = new TTree("TempLogger","Temperature Logger Data");
  
  TempLogger->Branch("date",&date,"date/C");
  TempLogger->Branch("location",&location,"location/C");
  TempLogger->Branch("time",&time,"time/C");
  TempLogger->Branch("timestamp",&timestamp,"timestamp/I");
  TempLogger->Branch("index",&index,"index/I");
  TempLogger->Branch("daq",&daq,"daq/I");
  TempLogger->Branch("temp",&ftemp,"temperature/F");
  
  TempLogger->Write();
  output->Close();

  delete output;
}

void FillTempLoggerTree(
	char *outfilename = "output_temp_logger.root",
	char *infilename = "temp.txt"){

TFile *output = new TFile(outfilename,"UPDATE","TempLoggerData");

ifstream in;
in.open(infilename);
if(in.good()) cout << "File open" << endl;
else cout << "File NOT open..." << endl;

char filedump[1024];
char location[128];
char date[128];
char time[128];
char cdum[128];

struct tm LoggerTime;
int idum=0;
int timestamp;
float ftemp=0;
int index;
int daq;

TTree *TempLogger = (TTree*)output->Get("TempLogger");
TempLogger->SetBranchAddress("date",&date);
TempLogger->SetBranchAddress("location",&location);
TempLogger->SetBranchAddress("time",&time);
TempLogger->SetBranchAddress("timestamp",&timestamp);
TempLogger->SetBranchAddress("index",&index);
TempLogger->SetBranchAddress("daq",&daq);
TempLogger->SetBranchAddress("temp",&ftemp);

while (1) {
  in >> idum >> date >> time >> cdum >> location >> index >> daq;

  if(!in.good()) break;

  sscanf(date,"%d.%d.%d",&LoggerTime.tm_mday, &LoggerTime.tm_mon, &LoggerTime.tm_year);
  LoggerTime.tm_year -= 1900;
  LoggerTime.tm_mon -=1;
  sscanf(time,"%d:%d:%d",&LoggerTime.tm_hour, &LoggerTime.tm_min, &LoggerTime.tm_sec);
  if( (sscanf(cdum,"%f,%d",&ftemp,&idum)==2) )
    ftemp += (float)(idum/10.0);
  else sscanf(cdum,"%f",&ftemp);
  
  timestamp = mktime(&LoggerTime);
  
  TempLogger->Fill();
}//end while

TempLogger->Write(TempLogger->GetName(),TObject::kWriteDelete);
output->Close();

//don't forget to clean up
delete output;

}

void PlotTempLoggerTree(char *inputfilename="output_temp_logger.root", char *location, int daq_no=0){
  gSystem->Setenv("TZ","UTC");
  
  TFile *file = new TFile (inputfilename,"OPEN");

  char filedump[1024];
  char location_tree[128];
  char date[128];
  char time[128];

  int timestamp;
  float ftemp=0;
  int index;
  int daq;

  TTree *TempLogger = (TTree*)file->Get("TempLogger");
  TempLogger->SetBranchAddress("date",&date);
  TempLogger->SetBranchAddress("location",&location_tree);
  TempLogger->SetBranchAddress("time",&time);
  TempLogger->SetBranchAddress("timestamp",&timestamp);
  TempLogger->SetBranchAddress("index",&index);
  TempLogger->SetBranchAddress("daq",&daq);
  TempLogger->SetBranchAddress("temp",&ftemp);

  int i=0;
  while(1){
    TempLogger->GetEntry(i);
    i++; 
    if(i==TempLogger->GetEntries()){

      cout << "PlotTree: GetEntry() error" << endl;
      break;
    }

    if(strncmp(&location_tree,location,strlen(&location))==0) break;
  }
  if(strncmp(&location_tree,location,strlen(&location))!=0) break;

  char selection[128];
  sprintf(selection,"index==%d && daq==%d",index,daq);
  char name[128];
  sprintf(name,"D%d - %s Antenna",daq, location);

//  cout << selection << " " << name << endl;

  TCanvas *can_CTR = new TCanvas();
  TempLogger->Draw("temp:timestamp",selection,"L");
  htemp->GetXaxis()->SetTimeDisplay(1);
  htemp->GetXaxis()->SetTimeFormat("%F1900-01-01 00:00:00");
  htemp->SetTitle(name);
  htemp->GetXaxis()->SetTitle("time - UTC");
  htemp->GetYaxis()->SetTitle("temperature / °C");
  
  delete file;
}
