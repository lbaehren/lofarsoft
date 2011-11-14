{
//gROOT->Reset();
 
ifstream inputfile;
inputfile.open("LPDA_Directivity.txt");
int entries = 36;
double zenith[entries];
double Gain036_orig[entries]; // measrued gain at 36 MHz
double Gain050_orig[entries]; // measrued gain at 50 MHz
double Gain066_orig[entries]; // measrued gain at 66 MHz
double Gain100_orig[entries]; // measrued gain at 100 MHz

double Gain036_rel[entries]; // relative gain at 36 MHz
double Gain050_rel[entries]; // relative gain at 50 MHz
double Gain066_rel[entries]; // relative gain at 66 MHz
double Gain100_rel[entries]; // relative gain at 100 MHz

double Gain036_fs[entries]; //  gain at 36 MHz for field strength
double Gain050_fs[entries]; //  gain at 50 MHz for field strength
double Gain066_fs[entries]; //  gain at 66 MHz for field strength
double Gain100_fs[entries]; //  gain at 100 MHz for field strength

double Gain036_power[entries]; //  gain at 36 MHz for power
double Gain050_power[entries]; //  gain at 50 MHz for power
double Gain066_power[entries]; //  gain at 66 MHz for power
double Gain100_power[entries]; //  gain at 100 MHz for power

double Gain036_fs_ir[entries]; //  gain at 36 MHz  for field strength relative to isotropic radiator
double Gain050_fs_ir[entries]; //  gain at 50 MHz  for field strength relative to isotropic radiator
double Gain066_fs_ir[entries]; //  gain at 66 MHz  for field strength relative to isotropic radiator
double Gain100_fs_ir[entries]; //  gain at 100 MHz for field strength relative to isotropic radiator

double tmp;
for(int i=0; i<entries; i++){
  
 inputfile >> tmp >> Gain036_orig[i] >> Gain050_orig[i] >> Gain066_orig[i] >> Gain100_orig[i];
 zenith[i] = (tmp * TMath::Pi() / 180.0);
 for(int j=0; j<8; j++) inputfile >> tmp;
 
 Gain036_rel[i] = Gain036_orig[0] - Gain036_orig[i];
 Gain050_rel[i] = Gain050_orig[0] - Gain050_orig[i];
 Gain066_rel[i] = Gain066_orig[0] - Gain066_orig[i];
 Gain100_rel[i] = Gain100_orig[0] - Gain100_orig[i];
 
 Gain036_fs[i] = pow(10,Gain036_rel[i]/20.0);
 Gain050_fs[i] = pow(10,Gain050_rel[i]/20.0);
 Gain066_fs[i] = pow(10,Gain066_rel[i]/20.0);
 Gain100_fs[i] = pow(10,Gain100_rel[i]/20.0);

 Gain036_power[i] = pow(10,Gain036_rel[i]/10.0);
 Gain050_power[i] = pow(10,Gain050_rel[i]/10.0);
 Gain066_power[i] = pow(10,Gain066_rel[i]/10.0);
 Gain100_power[i] = pow(10,Gain100_rel[i]/10.0);
 
 Gain036_fs_ir[i] = pow(10,(Gain036_rel[i] + 5.5) / 20.0);
 Gain050_fs_ir[i] = pow(10,(Gain050_rel[i] + 5.5) / 20.0);
 Gain066_fs_ir[i] = pow(10,(Gain066_rel[i] + 5.5) / 20.0);
 Gain100_fs_ir[i] = pow(10,(Gain100_rel[i] + 5.5) / 20.0);
 
//cout << zenith[i] << " -- " << Gain066_fs_ir[i]  << " -- " << Gain066_fs[i] /*<< Gain050_orig[i] << Gain066_orig[i] <<  Gain100_orig[i]*/ << endl;
}

if(0){
  TCanvas *can_orig = new TCanvas("orig");
  TGraphPolar *polar036_orig = new TGraphPolar(entries,zenith,Gain036_orig);
  polar036_orig->Draw("AL");
}

TLegend leg_rel = TLegend(0.827586, 0.737288, 0.989943, 0.991525);

TCanvas *can_rel = new TCanvas("rel");
TPaveText polar_title_rel(0.548851, 0.0127119, 0.721264, 0.0868644,"NDC");
polar_title_rel.AddText("gain / dB");

TGraphPolar *polar100_rel = new TGraphPolar(entries,zenith,Gain100_rel);
polar100_rel->SetLineColor(4);
polar100_rel->SetLineWidth(2);
polar100_rel->Draw("AOL");
TGraphPolar *polar036_rel = new TGraphPolar(entries,zenith,Gain036_rel);
polar036_rel->Draw("SAME");
polar036_rel->SetLineColor(1);
polar036_rel->SetLineWidth(2);
TGraphPolar *polar050_rel = new TGraphPolar(entries,zenith,Gain050_rel);
polar050_rel->SetLineColor(2);
polar050_rel->SetLineWidth(2);
polar050_rel->Draw("SAME");
TGraphPolar *polar066_rel = new TGraphPolar(entries,zenith,Gain066_rel);
polar066_rel->SetLineColor(3);
polar066_rel->SetLineWidth(2);
polar066_rel->SetTitle("gain / dB");
polar066_rel->Draw("SAME");

leg_rel.AddEntry(polar036_rel," 36 MHz","L");
leg_rel.AddEntry(polar050_rel," 50 MHz","L");
leg_rel.AddEntry(polar066_rel," 66 MHz","L");
leg_rel.AddEntry(polar100_rel,"100 MHz","L");
leg_rel->Draw("SAME");

polar_title_rel.Draw("same");

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

TCanvas *can_fs = new TCanvas("fs");
TPaveText polar_title_fs(0.548851, 0.0127119, 0.91954, 0.0868644,"NDC");
polar_title_fs.AddText("gain (field strength)");

TGraphPolar *polar100_fs = new TGraphPolar(entries,zenith,Gain100_fs);
polar100_fs->SetLineColor(4);
polar100_fs->SetLineWidth(2);
polar100_fs->Draw("AOL");
TGraphPolar *polar036_fs = new TGraphPolar(entries,zenith,Gain036_fs);
polar036_fs->Draw("SAME");
polar036_fs->SetLineColor(1);
polar036_fs->SetLineWidth(2);
TGraphPolar *polar050_fs = new TGraphPolar(entries,zenith,Gain050_fs);
polar050_fs->SetLineColor(2);
polar050_fs->SetLineWidth(2);
polar050_fs->Draw("SAME");
TGraphPolar *polar066_fs = new TGraphPolar(entries,zenith,Gain066_fs);
polar066_fs->SetLineColor(3);
polar066_fs->SetLineWidth(2);
polar066_fs->SetTitle("gain (field strength)");
polar066_fs->Draw("SAME");

leg_rel->Draw("SAME");
polar_title_fs.Draw("same");

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

TCanvas *can_power = new TCanvas("power");
TPaveText polar_title_power(0.548851, 0.0127119, 0.91954, 0.0868644,"NDC");
polar_title_power.AddText("gain (power)");

TGraphPolar *polar100_power = new TGraphPolar(entries,zenith,Gain100_power);
polar100_power->SetLineColor(4);
polar100_power->SetLineWidth(2);
polar100_power->Draw("AOL");
TGraphPolar *polar036_power = new TGraphPolar(entries,zenith,Gain036_power);
polar036_power->Draw("SAME");
polar036_power->SetLineColor(1);
polar036_power->SetLineWidth(2);
TGraphPolar *polar050_power = new TGraphPolar(entries,zenith,Gain050_power);
polar050_power->SetLineColor(2);
polar050_power->SetLineWidth(2);
polar050_power->Draw("SAME");
TGraphPolar *polar066_power = new TGraphPolar(entries,zenith,Gain066_power);
polar066_power->SetLineColor(3);
polar066_power->SetLineWidth(2);
polar066_power->SetTitle("gain (power)");
polar066_power->Draw("SAME");

leg_rel->Draw("SAME");
polar_title_power.Draw("same");

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

TCanvas *can_power = new TCanvas("fs_ir");
TPaveText polar_title_fs_ir(0.548851, 0.0127119, 0.91954, 0.0868644,"NDC");
polar_title_fs_ir.AddText("gain (field strength) relative to isotropic radiator");

TGraphPolar *polar100_fs_ir = new TGraphPolar(entries,zenith,Gain100_fs_ir);
polar100_fs_ir->SetLineColor(4);
polar100_fs_ir->SetLineWidth(2);
polar100_fs_ir->Draw("AOL");
TGraphPolar *polar036_fs_ir = new TGraphPolar(entries,zenith,Gain036_fs_ir);
polar036_fs_ir->SetLineColor(1);
polar036_fs_ir->SetLineWidth(2);
polar036_fs_ir->Draw("SAME");
TGraphPolar *polar050_fs_ir = new TGraphPolar(entries,zenith,Gain050_fs_ir);
polar050_fs_ir->SetLineColor(2);
polar050_fs_ir->SetLineWidth(2);
polar050_fs_ir->Draw("SAME");
TGraphPolar *polar066_fs_ir = new TGraphPolar(entries,zenith,Gain066_fs_ir);
polar066_fs_ir->SetLineColor(3);
polar066_fs_ir->SetLineWidth(2);
polar066_fs_ir->SetTitle("gain (field strength) relative to isotropic radiator");
polar066_fs_ir->Draw("SAME");

leg_rel->Draw("SAME");
polar_title_fs_ir.Draw("same");

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool code_output = false; // output of C code for implementation of the amp. calibration

if(code_output){
 cout << "  int entries = " << entries+1 << ";" << endl;
 cout << "  double ZENITH[entries], gain_fs_036[entries], gain_fs_050[entries], gain_fs_066[entries], gain_fs_100[entries]; "<< endl << endl;
 for(int i=0; i<entries; i++){
  zenith[i] = ( zenith[i] * 180.0 / TMath::Pi() );
  printf("  ZENITH[%2i] = %3.1f;\tgain_fs_036[%2i] = %3.5f;\tgain_fs_050[%2i] = %3.5f;\tgain_fs_066[%2i] = %3.5f;\tgain_fs_100[%2i] = %3.5f;\n",i, zenith[i],i,Gain036_fs_ir[i],i,Gain050_fs_ir[i],i,Gain066_fs_ir[i],i,Gain100_fs_ir[i]);
 }
  printf("  ZENITH[%2i] = %3.1f;\tgain_fs_036[%2i] = %3.5f;\tgain_fs_050[%2i] = %3.5f;\tgain_fs_066[%2i] = %3.5f;\tgain_fs_100[%2i] = %3.5f;\n",entries, zenith[0],entries,Gain036_fs_ir[0],entries,Gain050_fs_ir[0],entries,Gain066_fs_ir[0],entries,Gain100_fs_ir[0]);
  
}




}

