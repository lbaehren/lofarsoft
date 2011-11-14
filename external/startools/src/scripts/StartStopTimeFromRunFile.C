{
gROOT->Reset();

char time[1024];
int entries = (int)Tevent->GetEntries();
unsigned int run_id=0;
int day, month, year, hour, min, sec;

Theader->SetBranchAddress("run_id",&run_id);

Theader->GetEntry(0);
cout << "Run_id = " << run_id << endl;

Tevent->SetBranchAddress("timestamp",time);

Tevent->GetEntry(0);
sscanf (time,"CH0 %d.%d.%d %d:%d:%d.%*d",&day, &month, &year, &hour, &min, &sec);
cout << "Start = " << day << "." << month << "." << year << " " << hour << ":" << min << ":" << sec << " - " << time << endl;

Tevent->GetEntry(entries-1);
sscanf (time,"CH0 %d.%d.%d %d:%d:%d.%*d",&day, &month, &year, &hour, &min, &sec);
cout << "Stop = " << day << "." << month << "." << year << " " << hour << ":" << min << ":" << sec << " - " << time << endl;

cout << "Entries = " << (double)(entries/Tchannel_profile->GetEntries()) << endl << endl;

}
