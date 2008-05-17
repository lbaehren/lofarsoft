{
gROOT->Reset();
int NoZeros = 7;
int order = 126*(NoZeros+1)+1;
int order_hp = 126*10*(NoZeros+1)+1;
float coeffbp[order];
float coefflp[order];
float coeffhp[order_hp];
float xaxis_hp[order_hp];
float xaxis[order];
float integral;

for(int i=0; i<(order-1)/2; i++){
  xaxis[i+(order-1)/2+1]=i+1;
  xaxis[(order-1)/2-1-i]=-i-1;
}
xaxis[(order-1)/2]=0;

for(int i=0; i<(order_hp-1)/2; i++){
  xaxis_hp[i+(order_hp-1)/2+1]=i+1;
  xaxis_hp[(order_hp-1)/2-1-i]=-i-1;
}
xaxis_hp[(order_hp-1)/2]=0;

char name[1024];
sprintf(name,"1/(80e6*(%i+1))",NoZeros);
//***************************************************************
//*************          bandpass        ************************
//***************************************************************

TF1 f_delta("f_delta","40e6"); //bandwidth
TF1 f_0("f_0","60e6");//middle frequency of the filter

TF1 sample_rate("sample_rate",name);

TF1 sinc("sinc","TMath::Sin(TMath::Pi()*f_delta*sample_rate*x)/(TMath::Pi()*f_delta*sample_rate*x)");
TF1 CosF("CosF","TMath::Cos(2*TMath::Pi()*f_0*sample_rate*x)");
TF1 bandpass("bandpass","2*f_delta*sample_rate*sinc*CosF");

for(int i=0; i<(order-1)/2; i++) {
   coeffbp[i+(order-1)/2+1] = bandpass.Eval(i+1);
   coeffbp[(order-1)/2-1-i] = coeffbp[i+(order-1)/2+1];
}

TF1 ZeroElementbp("ZeroElementbp","2*f_delta*sample_rate");
  coeffbp[(order-1)/2] = ZeroElementbp.Eval(1);

TCanvas canbp("canbp","canbp");
TGraph graphbp(order,xaxis, coeffbp);
graphbp.SetTitle("bandpass");
graphbp.GetXaxis().SetTitle("arbitary units");
graphbp.GetYaxis().SetTitle("filter coeff");
graphbp.Draw("AL");

integral=0;
for(int i=0; i<order; i++) integral += coeffbp[i];
cout << "bandpass integral = " << integral << endl;


//***************************************************************
//*************         low-pass           **********************
//***************************************************************

TF1 f_lim("f_lim","40e6");//limit frequency of the filter

TF1 sincLP("sincLP","(TMath::Sin(2*TMath::Pi()*f_lim*sample_rate*x)/(2*TMath::Pi()*f_lim*sample_rate*x))");
TF1 lowpass("lowpass","2*f_lim*sample_rate*sincLP");

for(int i=0; i<(order-1)/2; i++) {
   coefflp[i+(order-1)/2+1] = lowpass.Eval(i+1);
   coefflp[(order-1)/2-1-i] = coefflp[i+(order-1)/2+1];
}

TF1 ZeroElementlp("ZeroElementlp","2*f_lim*sample_rate");
  coefflp[(order-1)/2] = ZeroElementlp.Eval(1);

TCanvas canlp("canlp","canlp");
TGraph graphlp(order,xaxis, coefflp);
graphlp.SetTitle("low-pass");
graphlp.GetXaxis().SetTitle("arbitary units");
graphlp.GetYaxis().SetTitle("filter coeff");
graphlp.Draw("AL");

integral=0;
for(int i=0; i<order; i++) integral += coefflp[i];
cout << "low pass integral = " << integral << endl;


//***************************************************************
//*************         high-pass          **********************
//***************************************************************

TF1 f_lim_hp("f_lim_hp","5e5");//limit frequency of the filter

TF1 sincHP("sincHP","TMath::Sin(2*TMath::Pi()*f_lim_hp*(sample_rate)*x)/(2*TMath::Pi()*f_lim_hp*(sample_rate)*x)");
TF1 highpass("highpass","(1-2*f_lim_hp*sample_rate)*sincHP");

for(int i=0; i<(order_hp-1)/2; i++) {
   coeffhp[i+(order_hp-1)/2+1] = highpass.Eval(i+1);
   coeffhp[(order_hp-1)/2-1-i] = coeffhp[i+(order_hp-1)/2+1];
}

TF1 ZeroElementhp("ZeroElementhp","1-2*f_lim_hp*sample_rate");
  coeffhp[(order_hp-1)/2] = ZeroElementhp.Eval(1);


TCanvas canhp("canhp","canhp");
TGraph graphhp(order_hp,xaxis_hp, coeffhp);
graphhp.SetTitle("high-pass");
graphhp.GetXaxis().SetTitle("arbitary units");
graphhp.GetYaxis().SetTitle("filter coeff");
graphhp.Draw("AL");

integral=0;
for(int i=0; i<order_hp; i++) integral += coeffhp[i];
cout << "high pass integral = " << integral << endl;

}
