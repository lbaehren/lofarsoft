//frequency dependent correction values of the electronics, determined by lab measurements
{
//  gROOT->Reset();
  bool code_output_calibration_cpp = false;
  bool code_output_calib_compare = false;
  bool draw_sum = true;
  int line_width = 2;
  
  char temp[512];
  float temp1, temp2;
  
//Amplitude Part
//**************
  // bandfilter
  ifstream input_amp_bp;
  input_amp_bp.open("HF_Bandpass_rev02/AMPLITUDE.TXT");
  
  while(1){
    input_amp_bp >> temp;
    if( strcmp(temp,"Imag\"")==0 ) break;
  }
  
  int count_amp_bp = 0;
  int entries = 250;
  float freq_amp_bp[entries], amp_bp[entries];
  
  while(1){
    input_amp_bp >> temp1 >> temp2 >> temp;
    if(!input_amp_bp.good()) break;
    freq_amp_bp[count_amp_bp] = temp1 / (1.0e6);
    amp_bp[count_amp_bp] = temp2+40;
    count_amp_bp++;
  }
  
  TGraph *g_amp_bp = new TGraph(count_amp_bp, freq_amp_bp, amp_bp);
  g_amp_bp->SetLineColor(2);
  g_amp_bp->SetLineWidth(line_width);
  g_amp_bp->SetTitle("amplitude distribution of the bandpass filter");
  g_amp_bp->GetXaxis()->SetTitle("f / MHz");
  g_amp_bp->GetYaxis()->SetTitle("gain / dB");



  // LNA
  ifstream input_amp_lna;
  input_amp_lna.open("LNA/A_LNA.TXT");
  
  while(1){
    input_amp_lna >> temp;
    if( strcmp(temp,"Imag\"")==0 ) break;
  }
  
  float freq_amp_lna[entries], amp_lna[entries];
  int count_amp_lna = 0;

  while(1){
    input_amp_lna >> temp1 >> temp2 >> temp;
    if(!input_amp_lna.good()) break;
    freq_amp_lna[count_amp_lna] = temp1 / (1.0e6);
    amp_lna[count_amp_lna] = temp2;
    count_amp_lna++;
  }

  TGraph * g_amp_lna = new TGraph(count_amp_lna, freq_amp_lna, amp_lna);
  g_amp_lna->SetLineColor(8);
  g_amp_lna->SetLineWidth(line_width);
  g_amp_lna->SetTitle("amplitude distribution of the low noise amplifier");
  g_amp_lna->GetXaxis()->SetTitle("f / MHz");
  g_amp_lna->GetYaxis()->SetTitle("gain / dB");
  
  
  
  //reflection coefficient
  ifstream input_amp_refcoef;
  input_amp_refcoef.open("RefCoef/S11_TypAachen.TXT");

  while(1){
    input_amp_refcoef >> temp;
    if( strcmp(temp,"Imag\"")==0 ) break;
  }
  
  float freq_amp_refcoef[entries], amp_refcoef[entries];
  int count_amp_refcoef = 0;

  while(1){
    input_amp_refcoef >> temp1 >> temp2 >> temp >> temp >> temp;
    if(!input_amp_refcoef.good()) break;
    freq_amp_refcoef[count_amp_refcoef] = temp1 / (1.0e6);
    amp_refcoef[count_amp_refcoef] = 10*TMath::Log10(1-TMath::Power(TMath::Abs(TMath::Power(10,temp2/20.0)),2));
    count_amp_refcoef++;
  }

  TGraph * g_amp_refcoef = new TGraph(count_amp_refcoef, freq_amp_refcoef, amp_refcoef);
  g_amp_refcoef->SetLineColor(6);
  g_amp_refcoef->SetLineWidth(line_width);
  g_amp_refcoef->SetTitle("insertion loss of the LPDA");
  g_amp_refcoef->GetXaxis()->SetTitle("f / MHz");
  g_amp_refcoef->GetYaxis()->SetTitle("gain / dB");

  
  //Sum of all amplitude dependencies
  int sum_entries = 131*5;
  float freq_amp_sum[sum_entries], amp_sum[sum_entries];
  for(int i=0; i<sum_entries; i++) {
    freq_amp_sum[i] = 0;
    amp_sum[i] = 0;
  }
  // interpolation from 15 up to 145
  float inter_bp, inter_lna, inter_refcoef;
  float tempfreq[sum_entries];
  for(int i=0; i<sum_entries; i++){
    
    freq_amp_sum[i] = (float)(131.0 / (float)sum_entries * (float)i) + 15;
    
    //interpolate bandpass
    for(int j=0; j<count_amp_bp; j++){
       if(freq_amp_sum[i]<freq_amp_bp[j]){
        amp_sum[i] += (( (amp_bp[j-1]-amp_bp[j]) / (freq_amp_bp[j-1]-freq_amp_bp[j]))*(freq_amp_sum[i]-freq_amp_bp[j])+amp_bp[j]);
        break;
      }
    }
    
    //interpolate LNA
    for(int j=0; j<count_amp_lna; j++){
       if(freq_amp_sum[i]<freq_amp_lna[j]){
        amp_sum[i] += (( (amp_lna[j-1]-amp_lna[j]) / (freq_amp_lna[j-1]-freq_amp_lna[j]))*(freq_amp_sum[i]-freq_amp_lna[j])+amp_lna[j]);
        break;
      }
    }
    
    //interpolate insertion loss
    for(int j=0; j<count_amp_refcoef; j++){
       if(freq_amp_sum[i]<freq_amp_refcoef[j]){
        amp_sum[i] += (( (amp_refcoef[j-1]-amp_refcoef[j]) / (freq_amp_refcoef[j-1]-freq_amp_refcoef[j]))*(freq_amp_sum[i]-freq_amp_refcoef[j])+amp_refcoef[j]);
        break;
      }
    }

  }
  
  TGraph * g_amp_sum = new TGraph(sum_entries, freq_amp_sum, amp_sum);
  g_amp_sum->SetLineColor(1);
  g_amp_sum->SetLineWidth(line_width);
  

  TLegend *leg = new TLegend(0.762579, 0.767391, 0.98978, 0.990217);
  leg->AddEntry(g_amp_bp,"bandpass filter","L");
  leg->AddEntry(g_amp_lna,"low noise amplifier","L");
  leg->AddEntry(g_amp_refcoef,"insertion loss","L");
  if(draw_sum) leg->AddEntry(g_amp_sum,"sum of all","L");
  
  TMultiGraph *mg = new TMultiGraph();
  mg->Add(g_amp_bp);
  mg->Add(g_amp_lna);
  mg->Add(g_amp_refcoef);
  if(draw_sum) mg->Add(g_amp_sum);
  mg->SetTitle("dependecies of electronics and insertion loss of LPDA -- Calibration");

  TCanvas *can = new TCanvas("can");
  mg->Draw("AL");
  mg->GetXaxis()->SetTitle("f / MHz");
  mg->GetXaxis()->SetRangeUser(20,100);
  mg->GetYaxis()->SetTitle("gain / dB");
  leg->Draw("SAME");
  
  
  if(code_output_calibration_cpp){
    printf("\n  int entries = %i;\n",sum_entries);
    printf("  float *freq = new float [entries]; \nfloat *gain_el_fs = new float [entries];\n");
    for(int i=0; i<sum_entries; i++){
      printf("  freq[%3i] = %4.2f; \t gain_el_fs[%3i] = %3.5f;\n",i, freq_amp_sum[i], i, pow(10, amp_sum[i]/20));
    }
    printf("\n");
  }
  
  if(code_output_calib_compare){
    printf("\n  float *freq_ele = new float [%i];\nfloat *corr_ele = new float [%i];\n",sum_entries, sum_entries);
    for(int i=0; i<sum_entries; i++){
      printf("  freq_ele[%3i] = %4.2f; \t corr_ele[%3i] = %3.5f;\n", i, freq_amp_sum[i], i, pow(10, amp_sum[i]/20));
    }
  }
  
  
//Phase Part
//**********

  if(code_output_calibration_cpp){
    cout << "(phase) code output not implemented" << endl;
  }
  
}
