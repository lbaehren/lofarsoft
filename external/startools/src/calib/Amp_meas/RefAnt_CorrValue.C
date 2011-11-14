//calculates the correction values (CorrValues) of the output of CalibrationD30

const unsigned int Max_NoCh = 20;			/*!<max number of channels per antenna cluster, is not an absolute limit*/
const unsigned int Max_SpecRes = 400000;		/*!<max number of frequency for correction*/

//! struct of calibration output
  struct cal{
    float CorrValue[Max_SpecRes];	/*!<frequency dependet correction values for the field strength*/
    int channel_id[Max_NoCh];		/*!<channel id*/
    float MaxHF[Max_NoCh];		/*!<maximum of HF data in the time domain of the calibration pulses*/
    float MaxEnv[Max_NoCh];		/*!<maximum of the envelope in the time domain of the calibrationi pulses*/
    bool ADCoverflow;			/*!<flag for the ADC overflow of the calibrated channel*/
    int CalCh;				/*!<channel id of the measured channel*/
    int event_id;			/*!<event id*/
  };


void RefAnt_CorrValue(){

 bool code_output_calibration_cpp = false;
 bool code_output_cal_compare = false;


// TFile *input = new TFile(filename,"READ");
// TTree *Tcal = (TTree*)input->Get("Tcal");

 TChain *Tcal = new TChain("Tcal");
 Tcal->Add("~/scratch2/Calib/D30/Cal_out_D30_CTR_NS.root");
 Tcal->Add("~/scratch2/Calib/D30/Cal_out_D30_CTR_EW.root");
 Tcal->Add("~/scratch2/Calib/D30/Cal_out_D30_300_NS.root");
 Tcal->Add("~/scratch2/Calib/D30/Cal_out_D30_300_EW.root");
 Tcal->Add("~/scratch2/Calib/D30/Cal_out_D30_360_NS.root");
 Tcal->Add("~/scratch2/Calib/D30/Cal_out_D30_360_EW.root");
 
 struct cal cal;
 int NoCorrValue;
 int NoChannels;
 Tcal->SetBranchAddress("NoChannels",&NoChannels);
 Tcal->SetBranchAddress("NoCorrValue",&NoCorrValue);
 Tcal->SetBranchAddress("CorrValue",&cal.CorrValue);
 Tcal->SetBranchAddress("channel_id",&cal.channel_id);
 Tcal->SetBranchAddress("MaxHF",&cal.MaxHF);
 Tcal->SetBranchAddress("MaxEnv",&cal.MaxEnv);  
 Tcal->SetBranchAddress("ADCoverflow",&cal.ADCoverflow);  
 Tcal->SetBranchAddress("CalCh",&cal.CalCh);
 Tcal->SetBranchAddress("event_id",&cal.event_id);
 Tcal->GetEntry(0);
 
 TProfile *his1 = new TProfile("his1","Correction Values for Amplitude Calibration (Ref. Ant.)",40,39.5,79.5,0,100);
 his1->GetXaxis()->SetTitle(" f / MHz ");
 his1->GetYaxis()->SetTitle(" correction value ");
 TProfile *his2 = new TProfile("his2","Correction Values for Amplitude Calibration (Ref. Ant.)",40,39.5,79.5,0,100);
 his2->GetXaxis()->SetTitle(" f / MHz ");
 his2->GetYaxis()->SetTitle(" 1/ correction value ");
 TH1F *his3 = new TH1F("his3","Correction Values of all Frequencies (Ref. Ant.)",100,0,6);
 his3->GetYaxis()->SetTitle("#");
 his3->GetXaxis()->SetTitle("correction value");
 TProfile *his4 = new TProfile("his4","Rejected Ccorrection Values for Amplitude Calibration (Ref. Ant.)",40,39.5,79.5,0,100);
 his4->GetXaxis()->SetTitle(" f / MHz ");
 his4->GetYaxis()->SetTitle("correction value");
 
 int rejected_values = 0;

 for(int j=0; j<Tcal->GetEntries(); j++){
   Tcal->GetEntry(j);
   if(!cal.ADCoverflow)
     for(int i=0; i<NoCorrValue; i++){
       //correction for wrong identified frequency amplitudes
       if(cal.CorrValue[i]<2.5 ){
         his1->Fill(41+i,    cal.CorrValue[i]);
         his2->Fill(41+i,1.0/cal.CorrValue[i]);
         his3->Fill(cal.CorrValue[i]);
       }
       else{
         rejected_values++;
	 his4->Fill(41+i, cal.CorrValue[i]);
       }
     }
 }
 TCanvas *can3 = new TCanvas("can3");
 his3->Draw();
 TCanvas *can1 = new TCanvas("can1");
 his1->Draw();
 TCanvas *can2 = new TCanvas("can2");
 his2->Draw();
 TCanvas *can4 = new TCanvas("can4");
 his4->Draw();
 
 cout << " rejected CorrValues = " << rejected_values << endl;
 	

 if(code_output_calibration_cpp){
   cout << endl << endl << endl;
   printf("  int entries = %i;\n",NoCorrValue);
   printf("  float *freq = new float [entries];\nfloat *CorrValue = new float [entries];\n");
   for(int i=0; i<NoCorrValue; i++){
     printf("  freq[%2i] = %3.1f;\tCorrValue[%2i] = %3.5f;\n",i,41+i,i,his1->GetBinContent(i+2));
   }
 }
 
 if(code_output_cal_compare){
//   printf("\n  float freq_ref[%i], corr_ref[%i];\n",NoCorrValue, NoCorrValue);
   for(int i=0; i<NoCorrValue; i++){
     printf("  freq_ref[%2i] = %3.1f; \t corr_ref[%2i] = %3.5f;\n",i,41+i,i,his2->GetBinContent(i+2));
   }
 }
 
}
