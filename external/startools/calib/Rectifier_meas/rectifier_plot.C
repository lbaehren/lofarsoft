{
gROOT->Reset();

ifstream in;
in.open("Meas_data_ascii.txt");

int entries = 34;
double Uenv[entries], Uhf[entries];


for(int i=0; i<entries; i++){
      in >> Uenv[i] >> Uhf[i];
//      Uenv[i] /= 1000;
//      Uhf[i] /= 1000;      
      if (!in.good()) break;
//      cout << i << endl;
   }
   
TGraph *graph = new TGraph(entries,Uenv,Uhf);
graph->SetTitle("Rectifier Characteristic");
graph->GetXaxis()->SetTitle("RF Ampl. / mV");
graph->GetYaxis()->SetTitle("Envelope Ampl. / mV");
graph->GetYaxis()->CenterTitle();
graph->GetXaxis()->SetRangeUser(0,300);
graph->GetYaxis()->SetRangeUser(0,1000);
TCanvas can;
graph->Draw("A*");

TF1 fit("fit","[0]+[1]*x+[2]*x^2",60,160);
//TF1 fit("fit","[0]+[1]*x+[2]*x^2",40,225);
fit.SetParameter(0,0);
fit.FixParameter(0,0);
//fit.SetParLimits(0,0,100);

fit.SetParameter(1,0);
//fit.SetParLimits(1,0,100);
fit.FixParameter(1,0);

fit.SetParameter(2,16.13/1000);
//fit.SetParLimits(2,0,100);
//fit.FixParameter(2,16.13);
fit.SetLineWidth(3);
fit.SetLineColor(2);
graph->Fit("fit","R");

TF1 func("func","16.13/1000.0*x^2",0,450);
func.SetLineColor(2);
func.SetLineStyle(2);
//func->Draw("SAMEL");

double arg = 5.09;
double k = 5.24e5.0 / arg;
double p = -fit.GetParameter(1)/(fit.GetParameter(2)*sqrt(k)*2);
double uenv = 300;

cout << "oliver " << endl;
cout << 3.44e-4.0*sqrt(arg)*sqrt(uenv)*sqrt(1000) << endl;
cout <<  sqrt(uenv/16.13*1000.0) << endl << endl;

cout << "fit" << endl;
cout <<  (p + sqrt(p*p - (-uenv-fit.GetParameter(0))/(fit.GetParameter(2)*k))) << endl;
cout <<  (p - sqrt(p*p - (-uenv-fit.GetParameter(0))/(fit.GetParameter(2)*k))) << endl << endl;

double p2 = -1*(fit.GetParameter(1)/(fit.GetParameter(2)*2));
cout <<  (p2 + sqrt(p2*p2 - (-fit.GetParameter(0)-uenv)/(fit.GetParameter(2)))) << endl;
cout <<  (p2 - sqrt(p2*p2 - (-fit.GetParameter(0)-uenv)/(fit.GetParameter(2)))) << endl;

}
