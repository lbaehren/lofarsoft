{
gROOT->Reset();

char *data_name="FZKMAST_IPE20060119.txt";
ifstream data;
data.open(data_name);

char ctmp[512];
char date[512];
char clock[512];

int entries = 6164;
float tem002[entries];
float tem060[entries];
float tem130[entries];
float tem200[entries];
float time[entries];
for(int i=0; i<entries; i++){
 time[i] = 600*i;
 tem002[i]=0;
 tem060[i]=0;
 tem130[i]=0;
 tem200[i]=0;
}
for(int i=0; i<9; i++) data >> ctmp;
for(int i=0; i<entries; i++) {
   data >> date >> clock >> tem002[i] >> tem060[i] >> tem130[i] >> tem200[i];
   data >> ctmp >> ctmp >> ctmp >> ctmp >> ctmp;

   if(tem002[i]<-999.0 && i>2 )tem002[i]= (tem002[i-1]+tem002[i-2]) / 2.0;
   if(tem060[i]<-999.0 && i>2 )tem060[i]= (tem060[i-1]+tem060[i-2]) / 2.0;
   if(tem130[i]<-999.0 && i>2 )tem130[i]= (tem130[i-1]+tem130[i-2]) / 2.0;
   if(tem200[i]<-999.0 && i>2 )tem200[i]= (tem200[i-1]+tem200[i-2]) / 2.0;
}

data.close();

TGraph *TEM002 = new TGraph(entries,time,tem002);
TEM002->SetLineColor(1);
TEM002->SetTitle("Temp. Kennlinien - Mast FZK - 19.01. to 02.03.06");
TEM002->GetXaxis()->SetTitle("time / s");
TEM002->GetYaxis()->SetTitle("Temp / °C");

TGraph *TEM060 = new TGraph(entries,time,tem060);
TEM060->SetLineColor(4);

TGraph *TEM130 = new TGraph(entries,time,tem130);
TEM130->SetLineColor(3);

TGraph *TEM200 = new TGraph(entries,time,tem200);
TEM200->SetLineColor(7);

TLegend *leg = new TLegend(0.784483, 0.794492, 0.994253, 0.985169);
leg->AddEntry(TEM002,"Temp, 002m","l");
leg->AddEntry(TEM060,"Temp, 060m","l");
leg->AddEntry(TEM130,"Temp, 130m","l");
leg->AddEntry(TEM200,"Temp, 200m","l");

TCanvas *can = new TCanvas("can","can");

TEM002->Draw("Al");
TEM060->Draw();
TEM130->Draw();
TEM200->Draw();
leg->Draw();

//************************************************************************************


char *data_name_run007="FZKMAST_RUN007.txt";
ifstream data_run007;
data_run007.open(data_name_run007);
int entries_run007 = 440;

for(int i=0; i<9; i++) data_run007 >> ctmp;
for(int i=0; i<entries_run007; i++) {
   data_run007 >> date >> clock >> tem002[i] >> tem060[i] >> tem130[i] >> tem200[i];
   data_run007 >> ctmp >> ctmp >> ctmp >> ctmp >> ctmp;

   if(tem002[i]<-999.0 && i>2 )tem002[i]= (tem002[i-1]+tem002[i-2]) / 2.0;
   if(tem060[i]<-999.0 && i>2 )tem060[i]= (tem060[i-1]+tem060[i-2]) / 2.0;
   if(tem130[i]<-999.0 && i>2 )tem130[i]= (tem130[i-1]+tem130[i-2]) / 2.0;
   if(tem200[i]<-999.0 && i>2 )tem200[i]= (tem200[i-1]+tem200[i-2]) / 2.0;
}

data_run007.close();


TGraph *TEM002_run007 = new TGraph(entries_run007,time,tem002);
TEM002_run007->SetLineColor(1);
TEM002_run007->SetTitle("Temp. Kennlinien - Mast FZK - Run 7");
TEM002_run007->GetXaxis()->SetTitle("time / s");
TEM002_run007->GetYaxis()->SetTitle("Temp / °C");

TGraph *TEM060_run007 = new TGraph(entries_run007,time,tem060);
TEM060_run007->SetLineColor(4);

TGraph *TEM130_run007 = new TGraph(entries_run007,time,tem130);
TEM130_run007->SetLineColor(3);

TGraph *TEM200_run007 = new TGraph(entries_run007,time,tem200);
TEM200_run007->SetLineColor(7);


TLegend *leg_run007 = new TLegend(0.784483, 0.794492, 0.994253, 0.985169);
leg_run007->AddEntry(TEM002,"Temp, 002m","l");
leg_run007->AddEntry(TEM060,"Temp, 060m","l");
leg_run007->AddEntry(TEM130,"Temp, 130m","l");
leg_run007->AddEntry(TEM200,"Temp, 200m","l");

TCanvas *can_run007 = new TCanvas("can_run007","can_run007");

TEM002_run007->Draw("Al");
TEM060_run007->Draw();
TEM130_run007->Draw();
TEM200_run007->Draw();
leg_run007->Draw();


//**************************************************************************

char *data_name_run005="FZKMAST_RUN005.txt";
ifstream data_run005;
data_run005.open(data_name_run005);
int entries_run005 = 398;

for(int i=0; i<9; i++) data_run005 >> ctmp;
for(int i=0; i<entries_run005; i++) {
   data_run005 >> date >> clock >> tem002[i] >> tem060[i] >> tem130[i] >> tem200[i];
   data_run005 >> ctmp >> ctmp >> ctmp >> ctmp >> ctmp;

   if(tem002[i]<-999.0 && i>2 )tem002[i]= (tem002[i-1]+tem002[i-2]) / 2.0;
   if(tem060[i]<-999.0 && i>2 )tem060[i]= (tem060[i-1]+tem060[i-2]) / 2.0;
   if(tem130[i]<-999.0 && i>2 )tem130[i]= (tem130[i-1]+tem130[i-2]) / 2.0;
   if(tem200[i]<-999.0 && i>2 )tem200[i]= (tem200[i-1]+tem200[i-2]) / 2.0;
}

data_run005.close();


TGraph *TEM002_run005 = new TGraph(entries_run005,time,tem002);
TEM002_run005->SetLineColor(1);
TEM002_run005->SetTitle("Temp-. & Triggerratenverlauf - Run 5");
TEM002_run005->GetXaxis()->SetTitle("time / s");
TEM002_run005->GetYaxis()->SetTitle("Temp / °C");

TGraph *TEM060_run005 = new TGraph(entries_run005,time,tem060);
TEM060_run005->SetLineColor(4);

TGraph *TEM130_run005 = new TGraph(entries_run005,time,tem130);
TEM130_run005->SetLineColor(3);

TGraph *TEM200_run005 = new TGraph(entries_run005,time,tem200);
TEM200_run005->SetLineColor(7);


TLegend *leg_run005 = new TLegend(0.784483, 0.794492, 0.994253, 0.985169);
leg_run005->AddEntry(TEM002,"Temp, 002m","l");
leg_run005->AddEntry(TEM060,"Temp, 060m","l");
leg_run005->AddEntry(TEM130,"Temp, 130m","l");
leg_run005->AddEntry(TEM200,"Temp, 200m","l");

TCanvas *can_run005 = new TCanvas("can_run005","can_run005");

TEM002_run005->Draw("Al");
TEM060_run005->Draw();
TEM130_run005->Draw();
TEM200_run005->Draw();
leg_run005->Draw();


//**************************************************************************

char *data_name_run007="FZKMAST_RUN007.txt";
ifstream data_run007;
data_run007.open(data_name_run007);
int entries_run007 = 438;

for(int i=0; i<9; i++) data_run007 >> ctmp;
for(int i=0; i<entries_run007; i++) {
   data_run007 >> date >> clock >> tem002[i] >> tem060[i] >> tem130[i] >> tem200[i];
   data_run007 >> ctmp >> ctmp >> ctmp >> ctmp >> ctmp;

   if(tem002[i]<-999.0 && i>2 )tem002[i]= (tem002[i-1]+tem002[i-2]) / 2.0;
   if(tem060[i]<-999.0 && i>2 )tem060[i]= (tem060[i-1]+tem060[i-2]) / 2.0;
   if(tem130[i]<-999.0 && i>2 )tem130[i]= (tem130[i-1]+tem130[i-2]) / 2.0;
   if(tem200[i]<-999.0 && i>2 )tem200[i]= (tem200[i-1]+tem200[i-2]) / 2.0;
}

data_run007.close();


TGraph *TEM002_run007 = new TGraph(entries_run007,time,tem002);
TEM002_run007->SetLineColor(1);
TEM002_run007->SetTitle("Temp-. & Triggerratenverlauf - Run 7");
TEM002_run007->GetXaxis()->SetTitle("time / s");
TEM002_run007->GetYaxis()->SetTitle("Temp / °C");

TGraph *TEM060_run007 = new TGraph(entries_run007,time,tem060);
TEM060_run007->SetLineColor(4);

TGraph *TEM130_run007 = new TGraph(entries_run007,time,tem130);
TEM130_run007->SetLineColor(3);

TGraph *TEM200_run007 = new TGraph(entries_run007,time,tem200);
TEM200_run007->SetLineColor(7);


TLegend *leg_run007 = new TLegend(0.784483, 0.794492, 0.994253, 0.985169);
leg_run007->AddEntry(TEM002,"Temp, 002m","l");
leg_run007->AddEntry(TEM060,"Temp, 060m","l");
leg_run007->AddEntry(TEM130,"Temp, 130m","l");
leg_run007->AddEntry(TEM200,"Temp, 200m","l");

TCanvas *can_run007 = new TCanvas("can_run007","can_run007");

TEM002_run007->Draw("Al");
TEM060_run007->Draw();
TEM130_run007->Draw();
TEM200_run007->Draw();
leg_run007->Draw();

//**************************************************************************

char *data_name_run008="FZKMAST_RUN008.txt";
ifstream data_run008;
data_run008.open(data_name_run008);
int entries_run008 = 540;

for(int i=0; i<9; i++) data_run008 >> ctmp;
for(int i=0; i<entries_run008; i++) {
   data_run008 >> date >> clock >> tem002[i] >> tem060[i] >> tem130[i] >> tem200[i];
   data_run008 >> ctmp >> ctmp >> ctmp >> ctmp >> ctmp;

   if(tem002[i]<-999.0 && i>2 )tem002[i]= (tem002[i-1]+tem002[i-2]) / 2.0;
   if(tem060[i]<-999.0 && i>2 )tem060[i]= (tem060[i-1]+tem060[i-2]) / 2.0;
   if(tem130[i]<-999.0 && i>2 )tem130[i]= (tem130[i-1]+tem130[i-2]) / 2.0;
   if(tem200[i]<-999.0 && i>2 )tem200[i]= (tem200[i-1]+tem200[i-2]) / 2.0;
}

data_run008.close();


TGraph *TEM002_run008 = new TGraph(entries_run008,time,tem002);
TEM002_run008->SetLineColor(1);
TEM002_run008->SetTitle("Temp-. & Triggerratenverlauf - Run 8");
TEM002_run008->GetXaxis()->SetTitle("time / s");
TEM002_run008->GetYaxis()->SetTitle("Temp / °C");

TGraph *TEM060_run008 = new TGraph(entries_run008,time,tem060);
TEM060_run008->SetLineColor(4);

TGraph *TEM130_run008 = new TGraph(entries_run008,time,tem130);
TEM130_run008->SetLineColor(3);

TGraph *TEM200_run008 = new TGraph(entries_run008,time,tem200);
TEM200_run008->SetLineColor(7);


TLegend *leg_run008 = new TLegend(0.784483, 0.794492, 0.994253, 0.985169);
leg_run008->AddEntry(TEM002,"Temp, 002m","l");
leg_run008->AddEntry(TEM060,"Temp, 060m","l");
leg_run008->AddEntry(TEM130,"Temp, 130m","l");
leg_run008->AddEntry(TEM200,"Temp, 200m","l");

TCanvas *can_run008 = new TCanvas("can_run008","can_run008");

TEM002_run008->Draw("Al");
TEM060_run008->Draw();
TEM130_run008->Draw();
TEM200_run008->Draw();
leg_run008->Draw();

//**************************************************************************

char *data_name_run011="FZKMAST_RUN011.txt";
ifstream data_run011;
data_run011.open(data_name_run011);
int entries_run011 = 568;

for(int i=0; i<9; i++) data_run011 >> ctmp;
for(int i=0; i<entries_run011; i++) {
   data_run011 >> date >> clock >> tem002[i] >> tem060[i] >> tem130[i] >> tem200[i];
   data_run011 >> ctmp >> ctmp >> ctmp >> ctmp >> ctmp;

   if(tem002[i]<-999.0 && i>2 )tem002[i]= (tem002[i-1]+tem002[i-2]) / 2.0;
   if(tem060[i]<-999.0 && i>2 )tem060[i]= (tem060[i-1]+tem060[i-2]) / 2.0;
   if(tem130[i]<-999.0 && i>2 )tem130[i]= (tem130[i-1]+tem130[i-2]) / 2.0;
   if(tem200[i]<-999.0 && i>2 )tem200[i]= (tem200[i-1]+tem200[i-2]) / 2.0;
}

data_run011.close();


TGraph *TEM002_run011 = new TGraph(entries_run011,time,tem002);
TEM002_run011->SetLineColor(1);
TEM002_run011->SetTitle("Temp-. & Triggerratenverlauf - Run 11");
TEM002_run011->GetXaxis()->SetTitle("time / s");
TEM002_run011->GetYaxis()->SetTitle("Temp / °C");

TGraph *TEM060_run011 = new TGraph(entries_run011,time,tem060);
TEM060_run011->SetLineColor(4);

TGraph *TEM130_run011 = new TGraph(entries_run011,time,tem130);
TEM130_run011->SetLineColor(3);

TGraph *TEM200_run011 = new TGraph(entries_run011,time,tem200);
TEM200_run011->SetLineColor(7);


TLegend *leg_run011 = new TLegend(0.784483, 0.794492, 0.994253, 0.985169);
leg_run011->AddEntry(TEM002,"Temp, 002m","l");
leg_run011->AddEntry(TEM060,"Temp, 060m","l");
leg_run011->AddEntry(TEM130,"Temp, 130m","l");
leg_run011->AddEntry(TEM200,"Temp, 200m","l");

TCanvas *can_run011 = new TCanvas("can_run011","can_run011");

TEM002_run011->Draw("Al");
TEM060_run011->Draw();
TEM130_run011->Draw();
TEM200_run011->Draw();
leg_run011->Draw();

//**************************************************************************

char *data_name_run012="FZKMAST_RUN012.txt";
ifstream data_run012;
data_run012.open(data_name_run012);
int entries_run012 = 398;

for(int i=0; i<9; i++) data_run012 >> ctmp;
for(int i=0; i<entries_run012; i++) {
   data_run012 >> date >> clock >> tem002[i] >> tem060[i] >> tem130[i] >> tem200[i];
   data_run012 >> ctmp >> ctmp >> ctmp >> ctmp >> ctmp;

   if(tem002[i]<-999.0 && i>2 )tem002[i]= (tem002[i-1]+tem002[i-2]) / 2.0;
   if(tem060[i]<-999.0 && i>2 )tem060[i]= (tem060[i-1]+tem060[i-2]) / 2.0;
   if(tem130[i]<-999.0 && i>2 )tem130[i]= (tem130[i-1]+tem130[i-2]) / 2.0;
   if(tem200[i]<-999.0 && i>2 )tem200[i]= (tem200[i-1]+tem200[i-2]) / 2.0;
}

data_run012.close();


TGraph *TEM002_run012 = new TGraph(entries_run012,time,tem002);
TEM002_run012->SetLineColor(1);
TEM002_run012->SetTitle("Temp-. & Triggerratenverlauf - Run 12");
TEM002_run012->GetXaxis()->SetTitle("time / s");
TEM002_run012->GetYaxis()->SetTitle("Temp / °C");

TGraph *TEM060_run012 = new TGraph(entries_run012,time,tem060);
TEM060_run012->SetLineColor(4);

TGraph *TEM130_run012 = new TGraph(entries_run012,time,tem130);
TEM130_run012->SetLineColor(3);

TGraph *TEM200_run012 = new TGraph(entries_run012,time,tem200);
TEM200_run012->SetLineColor(7);


TLegend *leg_run012 = new TLegend(0.784483, 0.794492, 0.994253, 0.985169);
leg_run012->AddEntry(TEM002,"Temp, 002m","l");
leg_run012->AddEntry(TEM060,"Temp, 060m","l");
leg_run012->AddEntry(TEM130,"Temp, 130m","l");
leg_run012->AddEntry(TEM200,"Temp, 200m","l");

TCanvas *can_run012 = new TCanvas("can_run012","can_run012");

TEM002_run012->Draw("Al");
TEM060_run012->Draw();
TEM130_run012->Draw();
TEM200_run012->Draw();
leg_run012->Draw();

//**************************************************************************

char *data_name_run013="FZKMAST_RUN013.txt";
ifstream data_run013;
data_run013.open(data_name_run013);
int entries_run013 = 703;

for(int i=0; i<9; i++) data_run013 >> ctmp;
for(int i=0; i<entries_run013; i++) {
   data_run013 >> date >> clock >> tem002[i] >> tem060[i] >> tem130[i] >> tem200[i];
   data_run013 >> ctmp >> ctmp >> ctmp >> ctmp >> ctmp;

   if(tem002[i]<-999.0 && i>2 )tem002[i]= (tem002[i-1]+tem002[i-2]) / 2.0;
   if(tem060[i]<-999.0 && i>2 )tem060[i]= (tem060[i-1]+tem060[i-2]) / 2.0;
   if(tem130[i]<-999.0 && i>2 )tem130[i]= (tem130[i-1]+tem130[i-2]) / 2.0;
   if(tem200[i]<-999.0 && i>2 )tem200[i]= (tem200[i-1]+tem200[i-2]) / 2.0;
}

data_run013.close();


TGraph *TEM002_run013 = new TGraph(entries_run013,time,tem002);
TEM002_run013->SetLineColor(1);
TEM002_run013->SetTitle("Temp-. & Triggerratenverlauf - Run 13");
TEM002_run013->GetXaxis()->SetTitle("time / s");
TEM002_run013->GetYaxis()->SetTitle("Temp / °C");

TGraph *TEM060_run013 = new TGraph(entries_run013,time,tem060);
TEM060_run013->SetLineColor(4);

TGraph *TEM130_run013 = new TGraph(entries_run013,time,tem130);
TEM130_run013->SetLineColor(3);

TGraph *TEM200_run013 = new TGraph(entries_run013,time,tem200);
TEM200_run013->SetLineColor(7);


TLegend *leg_run013 = new TLegend(0.784483, 0.794492, 0.994253, 0.985169);
leg_run013->AddEntry(TEM002,"Temp, 002m","l");
leg_run013->AddEntry(TEM060,"Temp, 060m","l");
leg_run013->AddEntry(TEM130,"Temp, 130m","l");
leg_run013->AddEntry(TEM200,"Temp, 200m","l");

TCanvas *can_run013 = new TCanvas("can_run013","can_run013");

TEM002_run013->Draw("Al");
TEM060_run013->Draw();
TEM130_run013->Draw();
TEM200_run013->Draw();
leg_run013->Draw();

//**************************************************************************


char *data_name_run014="FZKMAST_RUN014.txt";
ifstream data_run014;
data_run014.open(data_name_run014);
int entries_run014 = 827;

for(int i=0; i<7; i++) data_run014 >> ctmp;
for(int i=0; i<entries_run014; i++) {
   data_run014 >> date >> clock >> tem002[i] >> tem060[i] >> tem130[i] >> tem200[i];
   data_run014 >> ctmp >> ctmp >> ctmp;

   if(tem002[i]<-999.0 && i>2 )tem002[i]= (tem002[i-1]+tem002[i-2]) / 2.0;
   if(tem060[i]<-999.0 && i>2 )tem060[i]= (tem060[i-1]+tem060[i-2]) / 2.0;
   if(tem130[i]<-999.0 && i>2 )tem130[i]= (tem130[i-1]+tem130[i-2]) / 2.0;
   if(tem200[i]<-999.0 && i>2 )tem200[i]= (tem200[i-1]+tem200[i-2]) / 2.0;
}

data_run014.close();


TGraph *TEM002_run014 = new TGraph(entries_run014,time,tem002);
TEM002_run014->SetLineColor(1);
TEM002_run014->SetTitle("Temp-. & Triggerratenverlauf - Run 14");
TEM002_run014->GetXaxis()->SetTitle("time / s");
TEM002_run014->GetYaxis()->SetTitle("Temp / °C");

TGraph *TEM060_run014 = new TGraph(entries_run014,time,tem060);
TEM060_run014->SetLineColor(4);

TGraph *TEM130_run014 = new TGraph(entries_run014,time,tem130);
TEM130_run014->SetLineColor(3);

TGraph *TEM200_run014 = new TGraph(entries_run014,time,tem200);
TEM200_run014->SetLineColor(7);


TLegend *leg_run014 = new TLegend(0.784483, 0.794492, 0.994253, 0.985169);
leg_run014->AddEntry(TEM002,"Temp, 002m","l");
leg_run014->AddEntry(TEM060,"Temp, 060m","l");
leg_run014->AddEntry(TEM130,"Temp, 130m","l");
leg_run014->AddEntry(TEM200,"Temp, 200m","l");

TCanvas *can_run014 = new TCanvas("can_run014","can_run014");

TEM002_run014->Draw("Al");
TEM060_run014->Draw();
TEM130_run014->Draw();
TEM200_run014->Draw();
leg_run014->Draw();

//**************************************************************************


char *data_name_run016="FZKMAST_RUN016.txt";
ifstream data_run016;
data_run016.open(data_name_run016);
int entries_run016 = 991;

for(int i=0; i<7; i++) data_run016 >> ctmp;
for(int i=0; i<entries_run016; i++) {
   data_run016 >> date >> clock >> tem002[i] >> tem060[i] >> tem130[i] >> tem200[i];
   data_run016 >> ctmp >> ctmp >> ctmp;

   if(tem002[i]<-999.0 && i>2 )tem002[i]= (tem002[i-1]+tem002[i-2]) / 2.0;
   if(tem060[i]<-999.0 && i>2 )tem060[i]= (tem060[i-1]+tem060[i-2]) / 2.0;
   if(tem130[i]<-999.0 && i>2 )tem130[i]= (tem130[i-1]+tem130[i-2]) / 2.0;
   if(tem200[i]<-999.0 && i>2 )tem200[i]= (tem200[i-1]+tem200[i-2]) / 2.0;
}

data_run016.close();


TGraph *TEM002_run016 = new TGraph(entries_run016,time,tem002);
TEM002_run016->SetLineColor(1);
TEM002_run016->SetTitle("Temp-. & Triggerratenverlauf - Run 16");
TEM002_run016->GetXaxis()->SetTitle("time / s");
TEM002_run016->GetYaxis()->SetTitle("Temp / °C");

TGraph *TEM060_run016 = new TGraph(entries_run016,time,tem060);
TEM060_run016->SetLineColor(4);

TGraph *TEM130_run016 = new TGraph(entries_run016,time,tem130);
TEM130_run016->SetLineColor(3);

TGraph *TEM200_run016 = new TGraph(entries_run016,time,tem200);
TEM200_run016->SetLineColor(7);


TLegend *leg_run016 = new TLegend(0.784483, 0.794492, 0.994253, 0.985169);
leg_run016->AddEntry(TEM002,"Temp, 002m","l");
leg_run016->AddEntry(TEM060,"Temp, 060m","l");
leg_run016->AddEntry(TEM130,"Temp, 130m","l");
leg_run016->AddEntry(TEM200,"Temp, 200m","l");

TCanvas *can_run016 = new TCanvas("can_run016","can_run016");

TEM002_run016->Draw("Al");
TEM060_run016->Draw();
TEM130_run016->Draw();
TEM200_run016->Draw();
leg_run016->Draw();

//**************************************************************************


char *data_name_run017="FZKMAST_RUN017.txt";
ifstream data_run017;
data_run017.open(data_name_run017);
int entries_run017 = 722;

for(int i=0; i<7; i++) data_run017 >> ctmp;
for(int i=0; i<entries_run017; i++) {
   data_run017 >> date >> clock >> tem002[i] >> tem060[i] >> tem130[i] >> tem200[i];
   data_run017 >> ctmp >> ctmp >> ctmp;

   if(tem002[i]<-999.0 && i>2 )tem002[i]= (tem002[i-1]+tem002[i-2]) / 2.0;
   if(tem060[i]<-999.0 && i>2 )tem060[i]= (tem060[i-1]+tem060[i-2]) / 2.0;
   if(tem130[i]<-999.0 && i>2 )tem130[i]= (tem130[i-1]+tem130[i-2]) / 2.0;
   if(tem200[i]<-999.0 && i>2 )tem200[i]= (tem200[i-1]+tem200[i-2]) / 2.0;
}

data_run017.close();


TGraph *TEM002_run017 = new TGraph(entries_run017,time,tem002);
TEM002_run017->SetLineColor(1);
TEM002_run017->SetTitle("Temp-. & Triggerratenverlauf - Run 17");
TEM002_run017->GetXaxis()->SetTitle("time / s");
TEM002_run017->GetYaxis()->SetTitle("Temp / °C");

TGraph *TEM060_run017 = new TGraph(entries_run017,time,tem060);
TEM060_run017->SetLineColor(4);

TGraph *TEM130_run017 = new TGraph(entries_run017,time,tem130);
TEM130_run017->SetLineColor(3);

TGraph *TEM200_run017 = new TGraph(entries_run017,time,tem200);
TEM200_run017->SetLineColor(7);


TLegend *leg_run017 = new TLegend(0.784483, 0.794492, 0.994253, 0.985169);
leg_run017->AddEntry(TEM002,"Temp, 002m","l");
leg_run017->AddEntry(TEM060,"Temp, 060m","l");
leg_run017->AddEntry(TEM130,"Temp, 130m","l");
leg_run017->AddEntry(TEM200,"Temp, 200m","l");

TCanvas *can_run017 = new TCanvas("can_run017","can_run017");

TEM002_run017->Draw("Al");
TEM060_run017->Draw();
TEM130_run017->Draw();
TEM200_run017->Draw();
leg_run017->Draw();

//**************************************************************************


char *data_name_run018="FZKMAST_RUN018.txt";
ifstream data_run018;
data_run018.open(data_name_run018);
int entries_run018 = 721;

for(int i=0; i<7; i++) data_run018 >> ctmp;
for(int i=0; i<entries_run018; i++) {
   data_run018 >> date >> clock >> tem002[i] >> tem060[i] >> tem130[i] >> tem200[i];
   data_run018 >> ctmp >> ctmp >> ctmp;

   if(tem002[i]<-999.0 && i>2 )tem002[i]= (tem002[i-1]+tem002[i-2]) / 2.0;
   if(tem060[i]<-999.0 && i>2 )tem060[i]= (tem060[i-1]+tem060[i-2]) / 2.0;
   if(tem130[i]<-999.0 && i>2 )tem130[i]= (tem130[i-1]+tem130[i-2]) / 2.0;
   if(tem200[i]<-999.0 && i>2 )tem200[i]= (tem200[i-1]+tem200[i-2]) / 2.0;
}

data_run018.close();


TGraph *TEM002_run018 = new TGraph(entries_run018,time,tem002);
TEM002_run018->SetLineColor(1);
TEM002_run018->SetTitle("Temp-. & Triggerratenverlauf - Run 18");
TEM002_run018->GetXaxis()->SetTitle("time / s");
TEM002_run018->GetYaxis()->SetTitle("Temp / °C");

TGraph *TEM060_run018 = new TGraph(entries_run018,time,tem060);
TEM060_run018->SetLineColor(4);

TGraph *TEM130_run018 = new TGraph(entries_run018,time,tem130);
TEM130_run018->SetLineColor(3);

TGraph *TEM200_run018 = new TGraph(entries_run018,time,tem200);
TEM200_run018->SetLineColor(7);


TLegend *leg_run018 = new TLegend(0.784483, 0.794492, 0.994253, 0.985169);
leg_run018->AddEntry(TEM002,"Temp, 002m","l");
leg_run018->AddEntry(TEM060,"Temp, 060m","l");
leg_run018->AddEntry(TEM130,"Temp, 130m","l");
leg_run018->AddEntry(TEM200,"Temp, 200m","l");

TCanvas *can_run018 = new TCanvas("can_run018","can_run018");

TEM002_run018->Draw("Al");
TEM060_run018->Draw();
TEM130_run018->Draw();
TEM200_run018->Draw();
leg_run018->Draw();

//**************************************************************************


char *data_name_run019="FZKMAST_RUN019.txt";
ifstream data_run019;
data_run019.open(data_name_run019);
int entries_run019 = 1283;

for(int i=0; i<7; i++) data_run019 >> ctmp;
for(int i=0; i<entries_run019; i++) {
   data_run019 >> date >> clock >> tem002[i] >> tem060[i] >> tem130[i] >> tem200[i];
   data_run019 >> ctmp >> ctmp >> ctmp;

   if(tem002[i]<-999.0 && i>2 )tem002[i]= (tem002[i-1]+tem002[i-2]) / 2.0;
   if(tem060[i]<-999.0 && i>2 )tem060[i]= (tem060[i-1]+tem060[i-2]) / 2.0;
   if(tem130[i]<-999.0 && i>2 )tem130[i]= (tem130[i-1]+tem130[i-2]) / 2.0;
   if(tem200[i]<-999.0 && i>2 )tem200[i]= (tem200[i-1]+tem200[i-2]) / 2.0;
}

data_run019.close();


TGraph *TEM002_run019 = new TGraph(entries_run019,time,tem002);
TEM002_run019->SetLineColor(1);
TEM002_run019->SetTitle("Temp-. & Triggerratenverlauf - Run 19");
TEM002_run019->GetXaxis()->SetTitle("time / s");
TEM002_run019->GetYaxis()->SetTitle("Temp / °C");

TGraph *TEM060_run019 = new TGraph(entries_run019,time,tem060);
TEM060_run019->SetLineColor(4);

TGraph *TEM130_run019 = new TGraph(entries_run019,time,tem130);
TEM130_run019->SetLineColor(3);

TGraph *TEM200_run019 = new TGraph(entries_run019,time,tem200);
TEM200_run019->SetLineColor(7);


TLegend *leg_run019 = new TLegend(0.784483, 0.794492, 0.994253, 0.985169);
leg_run019->AddEntry(TEM002,"Temp, 002m","l");
leg_run019->AddEntry(TEM060,"Temp, 060m","l");
leg_run019->AddEntry(TEM130,"Temp, 130m","l");
leg_run019->AddEntry(TEM200,"Temp, 200m","l");

TCanvas *can_run019 = new TCanvas("can_run019","can_run019");

TEM002_run019->Draw("Al");
TEM060_run019->Draw();
TEM130_run019->Draw();
TEM200_run019->Draw();
leg_run019->Draw();

//**************************************************************************


char *data_name_run020="FZKMAST_RUN020_nichtkomplett.txt";
ifstream data_run020;
data_run020.open(data_name_run020);
int entries_run020 = 501;

for(int i=0; i<7; i++) data_run020 >> ctmp;
for(int i=0; i<entries_run020; i++) {
   data_run020 >> date >> clock >> tem002[i] >> tem060[i] >> tem130[i] >> tem200[i];
   data_run020 >> ctmp >> ctmp >> ctmp;

   if(tem002[i]<-999.0 && i>2 )tem002[i]= (tem002[i-1]+tem002[i-2]) / 2.0;
   if(tem060[i]<-999.0 && i>2 )tem060[i]= (tem060[i-1]+tem060[i-2]) / 2.0;
   if(tem130[i]<-999.0 && i>2 )tem130[i]= (tem130[i-1]+tem130[i-2]) / 2.0;
   if(tem200[i]<-999.0 && i>2 )tem200[i]= (tem200[i-1]+tem200[i-2]) / 2.0;
}

data_run020.close();


TGraph *TEM002_run020 = new TGraph(entries_run020,time,tem002);
TEM002_run020->SetLineColor(1);
TEM002_run020->SetTitle("Temp-. & Triggerratenverlauf - Run 20");
TEM002_run020->GetXaxis()->SetTitle("time / s");
TEM002_run020->GetYaxis()->SetTitle("Temp / °C");

TGraph *TEM060_run020 = new TGraph(entries_run020,time,tem060);
TEM060_run020->SetLineColor(4);

TGraph *TEM130_run020 = new TGraph(entries_run020,time,tem130);
TEM130_run020->SetLineColor(3);

TGraph *TEM200_run020 = new TGraph(entries_run020,time,tem200);
TEM200_run020->SetLineColor(7);


TLegend *leg_run020 = new TLegend(0.784483, 0.794492, 0.994253, 0.985169);
leg_run020->AddEntry(TEM002,"Temp, 002m","l");
leg_run020->AddEntry(TEM060,"Temp, 060m","l");
leg_run020->AddEntry(TEM130,"Temp, 130m","l");
leg_run020->AddEntry(TEM200,"Temp, 200m","l");

TCanvas *can_run020 = new TCanvas("can_run020","can_run020");

TEM002_run020->Draw("Al");
TEM060_run020->Draw();
TEM130_run020->Draw();
TEM200_run020->Draw();
leg_run020->Draw();

}

