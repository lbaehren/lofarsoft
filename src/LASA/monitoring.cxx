#include <math.h>
#include <TApplication.h> 
#include <TGClient.h> 
#include <TSystem.h> 
#include <TCanvas.h> 
#include <TFrame.h> 
#include <TStyle.h> 
#include <TGraph.h>  
#include <TPad.h> 
#include <TH1.h> 
#include <TRandom.h> 
#include <TGButton.h> 
#include <TGButtonGroup.h> 
#include <TRootEmbeddedCanvas.h> 
#include "monitoring.h"
 
MONITORING::MONITORING(const TGWindow *p,UInt_t w,UInt_t h)
{ 
	fMain = new TGMainFrame(p,w,h) ;				// Create a main frame 
	fEcanvas = new TRootEmbeddedCanvas("Ecanvas",fMain,200,200) ;	// Create 1st canvas widget 
	fMain->AddFrame(fEcanvas, new TGLayoutHints(kLHintsExpandX| kLHintsExpandY,10,10,10,1)) ;
	fEcanvas2 = new TRootEmbeddedCanvas("Ecanvas2",fMain,200,200) ;	// Create 2nd canvas widget 
	fMain->AddFrame(fEcanvas2, new TGLayoutHints(kLHintsExpandX| kLHintsExpandY,10,10,10,1)) ;
 
	// Create a horizontal frame widget with buttons  
	TGHorizontalFrame *hframe = new TGHorizontalFrame(fMain,200,40) ;

	//-------------------------------
	TGButtonGroup *fgroup = new TGButtonGroup(hframe,"Select LASAs",kVerticalFrame) ;
	TGCheckButton *fR[NO_LASAs] ;
	char name[20] ;
	for(int i=0;i<NO_LASAs;i++)
	{
		sprintf(name,"LASA %d",i+1) ;
		fR[i] = new TGCheckButton(fgroup,name) ;
		//fR[i]->SetState(kButtonUp) ;
		strcpy(name,"") ;
	}
	fR[0]->Connect("Clicked()","MONITORING",this,"Select_Lasa1()") ; 
	fR[1]->Connect("Clicked()","MONITORING",this,"Select_Lasa2()") ; 
	fR[2]->Connect("Clicked()","MONITORING",this,"Select_Lasa3()") ; 
	fR[3]->Connect("Clicked()","MONITORING",this,"Select_Lasa4()") ; 
	fR[4]->Connect("Clicked()","MONITORING",this,"Select_Lasa5()") ; 
	hframe->AddFrame(fgroup, new TGLayoutHints(kLHintsCenterX,5,5,3,4)) ;

	TGCheckButton *fcheck = new TGCheckButton(hframe,"Done") ;
	hframe->AddFrame(fcheck,new TGLayoutHints(kLHintsTop | kLHintsLeft,5,5,3,4)) ;
	fcheck->Connect("Clicked()","MONITORING",this,"Select_Done()") ; 
	//-------------------------------

	char tmp3[20],tmp4[20] ;
	TGComboBox *fCombo = new TGComboBox(hframe,100) ;
	fCombo->AddEntry("StartRun: Select",0) ;
	strcpy(tmp3,"Run: Normal") ;
	strcpy(tmp4,"Run: Calib") ;
	fCombo->AddEntry(tmp3,1) ;
	fCombo->AddEntry(tmp4,2) ;
	fCombo->Resize(120,20) ;
	fCombo->Select(0) ;
	fCombo->Connect("Selected(Int_t)","MONITORING",this,"Start(int)") ; 
	hframe->AddFrame(fCombo,new TGLayoutHints(kLHintsTop | kLHintsLeft,5,5,3,4)) ;

	//------------------------
	TGButtonGroup *fgroup2 = new TGButtonGroup(hframe,"Monitor Panel",kVerticalFrame) ;
	TGRadioButton *draw1 = new TGRadioButton(fgroup2,"Panel 1") ;
	draw1->Connect("Clicked()","MONITORING",this,"Click1()") ;
	TGRadioButton *draw2 = new TGRadioButton(fgroup2,"Panel 2"); 
	draw2->Connect("Clicked()","MONITORING",this,"Click2()"); 
	hframe->AddFrame(fgroup2, new TGLayoutHints(kLHintsCenterX,5,5,3,4)) ;

	TGButtonGroup *fgroup3 = new TGButtonGroup(hframe,"Event Display",kVerticalFrame) ;
	TGRadioButton *fR1 = new TGRadioButton(fgroup3,"1") ;
	TGRadioButton *fR2 = new TGRadioButton(fgroup3,"3") ;
	TGRadioButton *fR3 = new TGRadioButton(fgroup3,"5") ;
	TGRadioButton *fR4 = new TGRadioButton(fgroup3,"10") ;
	TGRadioButton *fR5 = new TGRadioButton(fgroup3,"50") ;
	fR1->Connect("Clicked()","MONITORING",this,"Event_Display_interval1()") ;
	fR2->Connect("Clicked()","MONITORING",this,"Event_Display_interval2()") ;
	fR3->Connect("Clicked()","MONITORING",this,"Event_Display_interval3()") ;
	fR4->Connect("Clicked()","MONITORING",this,"Event_Display_interval4()") ;
	fR5->Connect("Clicked()","MONITORING",this,"Event_Display_interval5()") ;
	hframe->AddFrame(fgroup3, new TGLayoutHints(kLHintsCenterX,5,5,3,4)) ;
	//----------------------- 
	
	char tmp1[20],tmp2[20] ;
	TGComboBox *fCombo1 = new TGComboBox(hframe,100) ;
	TGComboBox *fCombo2 = new TGComboBox(hframe,100) ;
	fCombo1->AddEntry("Traces: Select",0) ;
	fCombo2->AddEntry("Landau: Select",0) ;
	for(int i=1;i<=NO_LASAs*4;i++)
	{
		sprintf(tmp1,"Traces: Det%d",i) ;
		sprintf(tmp2,"Landau: Det%d",i) ;
		fCombo1->AddEntry(tmp1,i) ;
		fCombo2->AddEntry(tmp2,i) ;
	}
	fCombo1->Resize(120,20) ;
	fCombo2->Resize(120,20) ;
	fCombo1->Select(0) ;
	fCombo2->Select(0) ;
	fCombo1->Connect("Selected(Int_t)","MONITORING",this,"Traces(int)"); 
	fCombo2->Connect("Selected(Int_t)","MONITORING",this,"Landau(int)"); 
	hframe->AddFrame(fCombo1,new TGLayoutHints(kLHintsTop | kLHintsLeft,5,5,3,4)) ;
	hframe->AddFrame(fCombo2,new TGLayoutHints(kLHintsTop | kLHintsLeft,5,5,3,4)) ;

	TGTextButton *stop = new TGTextButton(hframe,"&StopPlot");
	stop->Connect("Clicked()", "MONITORING", this, "Stop()"); 
	hframe->AddFrame(stop, new TGLayoutHints(kLHintsCenterX,5,5,3,4));

	TGTextButton *exit = new TGTextButton(hframe,"&ExitRun");
	exit->Connect("Clicked()", "MONITORING", this, "Exit()"); 
	hframe->AddFrame(exit, new TGLayoutHints(kLHintsCenterX,5,5,3,4)) ;

	//hframe->ChangeOptions((hframe->GetOptions()& ~kHorizontalFrame)| kVerticalFrame) ; 
	fMain->AddFrame(hframe, new TGLayoutHints(kLHintsCenterX,2,2,2,2)) ; 
	fMain->SetWindowName("DWINGELOO") ;		// Set a name to the main frame 
	fMain->MapSubwindows() ; 		// Map all subwindows of main frame
	fMain->Resize(fMain->GetDefaultSize()) ; // Initialize the layout algorithm
 	fMain->MapWindow() ;			// Map main frame
}
 
MONITORING::~MONITORING()
{ 
	fMain->Cleanup() ; // Clean up used widgets: frames, buttons, layouthints
	delete fMain ; 
} 

void MONITORING::Read_Detector_Cord()
{
	char content[200] ;
	char det[50],x[50],y[50],z[50] ;
	float X,Y,Z ;
	int i=0 ;
	FILE *fp ;
	fp=fopen("Detector_Cord.dat","r") ;
	while(fgets(content,200,fp)!=NULL)
	{
		if(strncmp(content,"//",strlen("//"))==0) continue ;
		std::istrstream M(content) ;
		M>>det>>x>>y>>z ;
		std::istrstream XX(x) ;
		std::istrstream YY(y) ;
		std::istrstream ZZ(z) ;
		XX>>X ;
		YY>>Y ;
		ZZ>>Z ;
		Det_X[i]=X ; 
		Det_Y[i]=Y ;
		Det_Z[i]=Z ;
		if(Xmin==0) Xmin=Det_X[i] ;
		if(Xmax==0) Xmax=Det_X[i] ;
		if(Ymin==0) Ymin=Det_Y[i] ;
		if(Ymax==0) Ymax=Det_Y[i] ;
		if(Det_X[i]<Xmin) Xmin=Det_X[i] ;  
		if(Det_X[i]>Xmax) Xmax=Det_X[i] ;  
		if(Det_Y[i]<Ymin) Ymin=Det_Y[i] ;  
		if(Det_Y[i]>Ymax) Ymax=Det_Y[i] ;  
		printf("%d\t%f\t%f\t%f\n",i,Det_X[i],Det_Y[i],Det_Z[i]) ;
		i++ ;
		if(i==NO_LASAs*4) break ;
	}
	printf("Xmin=%lf\tXmax=%lf\tYmin=%lf\tYmax=%lf\n",Xmin,Xmax,Ymin,Ymax) ;
	fclose(fp) ;
}

void MONITORING::Initialisation()
{
	select_lasa1=0 ;
	select_lasa2=0 ;
	select_lasa3=0 ;
	select_lasa4=0 ;
	select_lasa5=0 ;
	select_done=0 ;
	start=0 ;
	event_display_interval=1 ;
	flag1=0 ;
	flag2=0 ;
	stop=0 ;
	EXIT=0 ;
	for(int k=0;k<(NO_LASAs*4);k++)
		Det[k]=k+1 ;
	
	Xmin=Xmax=Ymin=Ymax=0 ;
	Read_Detector_Cord() ;
}

void MONITORING::Select_Lasa1()
{
	if(select_lasa1==0) select_lasa1=1 ;
	else select_lasa1=0 ;
}

int MONITORING::Select_Lasa1_return()
{
	return select_lasa1 ;	
}

void MONITORING::Select_Lasa2()
{
	if(select_lasa2==0) select_lasa2=1 ;
	else select_lasa2=0 ;
}

int MONITORING::Select_Lasa2_return()
{
	return select_lasa2 ;	
}

void MONITORING::Select_Lasa3()
{
	if(select_lasa3==0) select_lasa3=1 ;
	else select_lasa3=0 ;
}

int MONITORING::Select_Lasa3_return()
{
	return select_lasa3 ;	
}

void MONITORING::Select_Lasa4()
{
	if(select_lasa4==0) select_lasa4=1 ;
	else select_lasa4=0 ;
}

int MONITORING::Select_Lasa4_return()
{
	return select_lasa4 ;	
}

void MONITORING::Select_Lasa5()
{
	if(select_lasa5==0) select_lasa5=1 ;
	else select_lasa5=0 ;
}

int MONITORING::Select_Lasa5_return()
{
	return select_lasa5 ;	
}

void MONITORING::Select_Done()
{
	select_done=1 ;
}

int MONITORING::Select_Done_return()
{
	return select_done ;	
}

void MONITORING::Start(int i)
{
	start=i ;
}

int MONITORING::Start_return()
{
	return start ;
}

void MONITORING::Event_Display_interval1()
{
	event_display_interval=1 ;
}

void MONITORING::Event_Display_interval2()
{
	event_display_interval=3 ;
}

void MONITORING::Event_Display_interval3()
{
	event_display_interval=5 ;
}

void MONITORING::Event_Display_interval4()
{
	event_display_interval=10 ;
}

void MONITORING::Event_Display_interval5()
{
	event_display_interval=50 ;
}

int MONITORING::Event_Display_interval_return()
{
	return event_display_interval ;	
}


void MONITORING::Click1()
{
	stop=0 ;
	flag1=1 ;
	flag2=0 ;
}

int MONITORING::Click1_return()
{
	return flag1 ;
}

void MONITORING::Click2()
{
	stop=0 ;
	flag1=0 ;
	flag2=1 ;
}

int MONITORING::Click2_return()
{
	return flag2 ;
}

void MONITORING::Traces(int i)
{
	flag_traces=i ;
}

void MONITORING::Landau(int i)
{
	flag_landau=i ;
}

int MONITORING::Click_Traces_return()
{
	return flag_traces ;
}

int MONITORING::Click_Landau_return()
{
	return flag_landau ;
}


void MONITORING::Stop()
{
	flag1=0 ;
	flag2=0 ;
	stop=1 ;
	printf("Stop\n") ;
}

int MONITORING::Stop_return()
{
	return stop ;
}

void MONITORING::Exit()
{
	EXIT=1 ;
}

int MONITORING::Exit_return()
{
	return EXIT ;
}

void MONITORING::Fill_MPV_Sigma(unsigned int n,float a,float b)
{
	MPV[n-1]=a ;
	Sigma[n-1]=b ;
}

void MONITORING::Draw_MPV_Sigma()
{
	c->Update() ;
	c->cd() ;
	gSystem->ProcessEvents() ;
	g_MPV = new TGraph(NO_LASAs*4,Det,MPV) ;
	g_sigma = new TGraph(NO_LASAs*4,Det,Sigma) ;
	
	pad1->cd() ;
	pad1->SetGrid() ;
	g_MPV->SetMarkerColor(2);
       	g_MPV->SetMarkerStyle(21);
       	g_MPV->SetMarkerSize(1) ;
	g_MPV->SetMinimum(0) ;
	g_MPV->GetHistogram()->SetAxisRange(0,NO_LASAs*4+1,"X") ;
	g_MPV->GetHistogram()->SetNdivisions(NO_LASAs*4+2,"X") ;	
	g_MPV->SetTitle("MPV") ;
	g_MPV->GetHistogram()->SetXTitle("Detector no") ;	
	g_MPV->GetHistogram()->GetXaxis()->CenterTitle() ;
	//g_MPV->GetHistogram()->SetYTitle("Value") ;	
	g_MPV->Draw("AP");

	pad2->cd();
	pad2->SetGrid() ;
	g_sigma->SetMarkerColor(2);
       	g_sigma->SetMarkerStyle(21);
       	g_sigma->SetMarkerSize(1) ;
	g_sigma->SetMinimum(0) ;
	g_sigma->GetHistogram()->SetAxisRange(0,NO_LASAs*4+1,"X") ;
	g_sigma->GetHistogram()->SetNdivisions(NO_LASAs*4+2,"X") ;
	g_sigma->SetTitle("Sigma") ;
	g_sigma->GetHistogram()->SetXTitle("Detector no") ;	
	g_sigma->GetHistogram()->GetXaxis()->CenterTitle() ;
	//g_sigma->GetHistogram()->SetYTitle("Value") ;
	g_sigma->Draw("AP");

	c->cd() ;
	c->Update();
}

void MONITORING::DRAW_HV()
{
	c2->Update() ;
	c2->cd() ;
	gSystem->ProcessEvents() ;
	g_HV = new TGraph(NO_LASAs*4,Det,HV) ;
	
	pad7->cd() ;
	pad7->SetGrid() ;
	pad7->SetLogy(0) ;
	g_HV->SetMarkerColor(2);
       	g_HV->SetMarkerStyle(21);
       	g_HV->SetMarkerSize(1) ;
	g_HV->SetMinimum(0) ;
	g_HV->GetHistogram()->SetAxisRange(0,NO_LASAs*4+1,"X") ;
	g_HV->GetHistogram()->SetNdivisions(NO_LASAs*4+2,"X") ;	
	g_HV->SetTitle("HV") ;
	g_HV->GetHistogram()->SetXTitle("Detector no") ;	
	g_HV->GetHistogram()->GetXaxis()->CenterTitle() ;
	//g_HV->GetHistogram()->SetYTitle("Value") ;	
	g_HV->Draw("AP");

	c2->cd() ;
	c2->Update();
}

void MONITORING::DRAW_Traces(TH1F *traces,int n)
{
	char temp[20] ;
	sprintf(temp,"Traces Det%d",n) ;
	TH1F *h_traces=(TH1F*)traces->Clone() ;
	c->Update() ;
	c->cd() ;
	gSystem->ProcessEvents() ;

	pad8->cd() ;
	pad8->SetGrid() ;
	gStyle->SetOptStat(10) ;
	h_traces->SetLineColor(kRed);
       	h_traces->SetLineWidth(2) ;
	h_traces->SetTitle(temp) ;
	h_traces->SetXTitle("Time (nsecs)") ;	
	h_traces->GetXaxis()->CenterTitle() ;
	h_traces->Draw("C");

	c->cd() ;
	c->Update() ;
}

void MONITORING::DRAW_Landau(TH1F *landau,int n,float a,float b,float d)
{
	char temp[20] ;
	sprintf(temp,"Landau Det%d",n) ;
	TH1F *h_landau=(TH1F*)landau->Clone() ;
	TF1 *fit=new TF1("fit","landau",0,15000) ;
	fit->SetParameters(a,b,d) ;
	c->Update() ;
	c->cd() ;
	gSystem->ProcessEvents() ;

	pad9->cd() ;
	pad9->SetGrid() ;
	gStyle->SetOptStat(10) ;
	h_landau->SetLineColor(kRed) ;
       	h_landau->SetLineWidth(2) ;
	h_landau->SetTitle(temp) ;
	h_landau->SetXTitle("Counts") ;	
	h_landau->GetXaxis()->CenterTitle() ;
	h_landau->Draw("HIST") ;
	gStyle->SetOptFit(1011) ;
	fit->Draw("SAME") ;

	c->cd() ;
	c->Update() ;
}

void MONITORING::DRAW_canvas()
{
	c = fEcanvas->GetCanvas();
	c->cd() ;
	c->Update() ;
	c->GetFrame()->SetFillColor(10);
   	c->GetFrame()->SetBorderSize(2);
   	c->SetBorderMode(1);
   	c->SetFillColor(4);
	gStyle->SetTitleX(0.4) ;
	gStyle->SetTitleW(0.2) ;

	pad1 = new TPad("pad1","This is pad1",0.01,0.01,0.336,0.99,3) ;
   	pad2 = new TPad("pad2","This is pad2",0.336,0.01,0.663,0.99,5) ;
   	pad3 = new TPad("pad3","This is pad3",0.663,0.01,0.99,0.99,7) ;
	pad1->Draw() ;
   	pad2->Draw() ;
   	pad3->Draw() ;
	
	c2 = fEcanvas2->GetCanvas() ;
	c2->cd() ;
	c2->Update() ;
	c2->GetFrame()->SetFillColor(10) ;
   	c2->GetFrame()->SetBorderSize(2) ;
   	c2->SetBorderMode(1) ;
   	c2->SetFillColor(4) ;
	gStyle->SetTitleX(0.4) ;
	gStyle->SetTitleW(0.2) ;
	pad7 = new TPad("pad1","This is pad1",0.01,0.01,0.336,0.99,3) ;
   	pad8 = new TPad("pad2","This is pad2",0.336,0.01,0.663,0.99,5) ;
   	pad9 = new TPad("pad3","This is pad3",0.663,0.01,0.99,0.99,7) ;
	pad7->Draw() ;
   	pad8->Draw() ;
   	pad9->Draw() ;
}

void MONITORING::DEFINE_Histo()
{
	h_hits_total = new TH1F("Hits_Total","Total No. of hits",NO_LASAs*4+1,0,NO_LASAs*4+1) ;
	h_hits_recent = new TH1F("Hits_Recent","Recent No. of hits",NO_LASAs*4+1,0,NO_LASAs*4+1) ;
	
	h_XY = new TH2F("h_XY","Event Display",(int)(((Xmax+10)-(Xmin-10))/Bin_Size_X),Xmin-10.0,Xmax+10.0,(int)(((Ymax+10)-(Ymin-10))/Bin_Size_Y),Ymin-10.0,Ymax+10.0) ;
	h_rate = new TH1F("Rates","Event Rate",40,0,40) ;
	h_size = new TH1F("Size","Event Size",50,1,7) ;
	
	h_core = new TH2F("h_core","Event Core",(int)(((Xmax+10)-(Xmin-10))/Bin_Size_X),Xmin-10.0,Xmax+10.0,(int)(((Ymax+10)-(Ymin-10))/Bin_Size_Y),Ymin-10.0,Ymax+10.0) ;
	
	h_dtime = new TH1F("Dtime","Event Time Diff",400,0,100000) ;
}

void MONITORING::FILL_Hits(unsigned int i)
{
	h_hits_total->SetBinContent(i,h_hits_total->GetBinContent(i)+1) ;
	h_hits_recent->SetBinContent(i,h_hits_recent->GetBinContent(i)+1) ;
}
void MONITORING::DRAW_Hits(int i)
{
	c->Update() ;
	c->cd() ;
	gSystem->ProcessEvents() ;

	pad3->cd() ;
	pad3->SetGrid() ;
	pad3->SetLogy(1) ;
	gStyle->SetOptStat(0) ;
	h_hits_total->SetNdivisions(NO_LASAs*4+2,"X") ;	
	h_hits_total->SetXTitle("Detector no") ;	
	h_hits_total->GetXaxis()->CenterTitle() ;
	h_hits_total->SetFillColor(kBlack) ;
	h_hits_total->SetBarOffset(0.5) ;
	h_hits_total->SetBarWidth(0.3) ;
	h_hits_total->SetMinimum(1) ;
	h_hits_total->Draw("bar4") ;

	h_hits_recent->SetFillColor(kRed) ;
	h_hits_recent->SetBarOffset(0.7) ;
	h_hits_recent->SetBarWidth(0.3) ;
	h_hits_recent->Draw("bar4,same") ;
	TLegend *legend = new TLegend(0.68,0.89,0.94,0.99) ;
	char temp[30] ;
	//sprintf(temp,"Total %llu",(long long unsigned)h_hits_total->GetEntries()) ;
	strcpy(temp,"Total") ;
	legend->AddEntry(h_hits_total,temp,"f");
	//sprintf(temp,"Recent %llu",(long long unsigned)h_hits_recent->GetEntries()) ;
	strcpy(temp,"Recent") ;
	legend->AddEntry(h_hits_recent,temp,"f");
	legend->Draw();

	pad3->cd() ;
	if(i==1) h_hits_recent->Reset() ;	
			
	c->cd();
	c->Update();
}

void MONITORING::Fill_XY_display(unsigned int n,unsigned int count)
{
	h_XY->SetBinContent(h_XY->GetXaxis()->FindBin(Det_X[n-1]),h_XY->GetYaxis()->FindBin(Det_Y[n-1]),(int)count) ;
}

void MONITORING::DRAW_XY_display()
{
	c->Update() ;
	c->cd() ;
	gSystem->ProcessEvents() ;
	
	pad1->cd() ;
	pad1->SetGrid() ;
	gStyle->SetOptStat(0) ;
	h_XY->SetMinimum(0) ;
	h_XY->SetXTitle("X") ;
	h_XY->GetXaxis()->CenterTitle() ;
	h_XY->SetYTitle("Y") ;
	h_XY->GetYaxis()->CenterTitle() ;
	h_XY->Draw("LEGO2Z") ;

	c->cd() ;
	c->Update();
}

void MONITORING::DRAW_Event_Rate(double event_rate)
{
	for(int i=40;i>1;i--)
		h_rate->SetBinContent(i,h_rate->GetBinContent(i-1)) ;
	h_rate->SetBinContent(1,event_rate) ;

	c->Update() ;
	c->cd() ;
	gSystem->ProcessEvents() ;
	
	pad2->cd() ;
	pad2->SetGrid() ;
	gStyle->SetOptStat(0) ;
	h_rate->SetXTitle("Time (arb. units)") ;
	h_rate->GetXaxis()->CenterTitle() ;
	h_rate->SetYTitle("Rate (Hz)") ;
	h_rate->GetYaxis()->CenterTitle() ;
	h_rate->SetFillColor(kGreen) ;
	h_rate->Draw("HIST") ;
	
	c->cd() ;
	c->Update() ;
}

void MONITORING::Fill_Event_Size(unsigned int event_size)
{
	h_size->Fill(log10(event_size)) ;
}

void MONITORING::DRAW_Event_Size()
{
	c->Update() ;
	c->cd() ;
	gSystem->ProcessEvents() ;
	
	pad3->cd() ;
	pad3->SetGrid() ;
	pad3->SetLogy() ;
	gStyle->SetOptStat(10) ;
	h_size->SetXTitle("log_{10}Size") ;
	h_size->GetXaxis()->CenterTitle() ;
	h_size->SetFillColor(kGreen) ;
	h_size->Draw("HIST") ;
	
	c->cd() ;
	c->Update() ;
}

void MONITORING::Fill_Event_Core(float x,float y)
{
	h_core->Fill(x,y) ;
}

void MONITORING::DRAW_Event_Core()
{
	c2->Update() ;
	c2->cd() ;
	gSystem->ProcessEvents() ;
	
	pad7->cd() ;
	pad7->SetGrid() ;
	gStyle->SetOptStat(10) ;
	h_core->SetXTitle("X") ;
	h_core->GetXaxis()->CenterTitle() ;
	h_core->SetYTitle("Y") ;
	h_core->GetYaxis()->CenterTitle() ;
	h_core->Draw("LEGO2Z") ;
	
	c2->cd() ;
	c2->Update() ;
}

void MONITORING::Fill_Time_Diff(float dtime)
{
	h_dtime->Fill(dtime) ;
}

void MONITORING::DRAW_Time_Diff()
{
	c2->Update() ;
	c2->cd() ;
	gSystem->ProcessEvents() ;
	
	pad8->cd() ;
	pad8->SetGrid() ;
	gStyle->SetOptStat(0) ;
	h_dtime->SetXTitle("Event Time diff (micro sec)") ;
	h_dtime->GetXaxis()->CenterTitle() ;
	h_dtime->SetFillColor(kGreen) ;
	h_dtime->Draw("HIST") ;
	
	c2->cd() ;
	c2->Update() ;
}


