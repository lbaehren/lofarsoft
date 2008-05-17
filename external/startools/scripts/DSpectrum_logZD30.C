{
gROOT->Reset();
gSystem->Setenv("TZ","UTC");

bool log_scale=true;

gStyle->SetPalette(1);
gStyle->SetOptStat(0);
if(log_scale) gStyle->SetOptLogz();


TCanvas canEvent_NS;
canEvent_NS->Divide(2,2);
TCanvas canEvent_EW;
canEvent_EW->Divide(2,2);

TCanvas canTime_NS;
canTime_NS->Divide(2,2);
TCanvas canTime_EW;
canTime_EW->Divide(2,2);

canEvent_NS->cd(1);
spectrum_00->Draw("colz");
canEvent_NS->cd(2);
spectrum_02->Draw("colz");
canEvent_NS->cd(3);
spectrum_04->Draw("colz");
canEvent_NS->cd(4);
spectrum_06->Draw("colz");

canEvent_EW->cd(1);
spectrum_01->Draw("colz");
canEvent_EW->cd(2);
spectrum_03->Draw("colz");
canEvent_EW->cd(3);
spectrum_05->Draw("colz");
canEvent_EW->cd(4);
spectrum_07->Draw("colz");

canTime_NS->cd(1);
spectrum_08->Draw("colz");
canTime_NS->cd(2);
spectrum_10->Draw("colz");
canTime_NS->cd(3);
spectrum_12->Draw("colz");
canTime_NS->cd(4);
spectrum_14->Draw("colz");

canTime_EW->cd(1);
spectrum_09->Draw("colz");
canTime_EW->cd(2);
spectrum_11->Draw("colz");
canTime_EW->cd(3);
spectrum_13->Draw("colz");
canTime_EW->cd(4);
spectrum_15->Draw("colz");

}
