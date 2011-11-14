{
gROOT->Reset();

char *data_name="characteristic_curve_filter_data.txt";
ifstream data;
data.open(data_name);

char ctmp[1024];
int entries = 21;
float puls_height[entries];
float hf_burst[entries];
float neg_env_passive[entries];
float neg_env_active[entries];
 
 data >> ctmp;
 for (int i=0; i<entries; i++) data >> puls_height[i];

 data >> ctmp;
 for (int i=0; i<entries; i++) data >> hf_burst[i];

 data >> ctmp;
 for (int i=0; i<entries; i++) data >> neg_env_passive[i];
 
 data >> ctmp;
 for (int i=0; i<entries; i++) data >> neg_env_active[i];
 
 data.close();

int width = 2 ;
TLegend *leg = new TLegend(0.133621, 0.699153, 0.613506, 0.891949);

TGraph *NEG_ENV_ACTIVE = new TGraph (entries, puls_height, neg_env_active);
NEG_ENV_ACTIVE->SetLineWidth(width);
NEG_ENV_ACTIVE->SetLineColor(3);
NEG_ENV_ACTIVE->SetTitle("characteristic curves of the trigger filters");
NEG_ENV_ACTIVE->GetXaxis()->SetTitle("puls height (LNA) / #muV");
NEG_ENV_ACTIVE->GetYaxis()->SetTitle("peak height / mV");

TGraph *HF_BURST = new TGraph (entries, puls_height, hf_burst);
HF_BURST->SetLineWidth(width);
HF_BURST->SetLineColor(1);

TGraph *NEG_ENV_PASSIVE = new TGraph (entries, puls_height, neg_env_passive);
NEG_ENV_PASSIVE->SetLineWidth(width);
NEG_ENV_PASSIVE->SetLineColor(4);


float e_field[entries];
for(int i=0; i< entries; i++) e_field[i] = puls_height[i]*1.04059/40*26.66667; //muV/m/MHz
//26.6 ->Scale factor

TGraph *E_FIELD = new TGraph (entries, puls_height, e_field);
E_FIELD->SetLineWidth(width);
E_FIELD->SetLineColor(2);

TGaxis *axis = new TGaxis(1090,0, 1090, 800,0,30,510,"+L");
axis->SetTitle("#epsilon_{f} / (#muV/( m #upoint MHz ))");
axis->SetLineColor(2);
axis->SetLabelColor(2);




leg->AddEntry(NEG_ENV_ACTIVE,"negative envelope - active filter","l");
leg->AddEntry(HF_BURST,"HF burst","l");
leg->AddEntry(NEG_ENV_PASSIVE,"negative envelope - passive filter","l");
//leg->AddEntry(E_FIELD,"#epsilon_{f}","l");



NEG_ENV_ACTIVE->Draw("AL");
HF_BURST->Draw("L");
NEG_ENV_PASSIVE->Draw("L");
//E_FIELD->Draw("L");
//axis->Draw();
leg->Draw();

}
