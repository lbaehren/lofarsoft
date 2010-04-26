#include <strstream>
#include <TQObject.h>
#include <TGFrame.h>
#include <RQ_OBJECT.h>
#include <TCanvas.h>
#include <TGComboBox.h>
#include <TGraph.h>
#include <TGraph2D.h>
#include <TLegend.h>
#include <TGaxis.h>
#include <TH2F.h>
#include <TF1.h>
#include "Constants.h"

#define	Bin_Size_X	5	//Bin size for the 2D event display
#define	Bin_Size_Y	5	//Bin size for the 2D event display
	
class TGWindow;
class TGMainFrame;
class TRootEmbeddedCanvas;

/*!
  \class MONITORING
  \ingroup LASA
  \brief Monitoring of the air-shower array
*/
class MONITORING
{ 
  RQ_OBJECT("MONITORING") 
    private:
  //FILE *fp ; 
  TGMainFrame *fMain ; 
  TRootEmbeddedCanvas *fEcanvas,*fEcanvas2 ;
  TCanvas *c,*c2 ;
  TH1F *h_hits_total,*h_hits_recent,*hpz ;
  TH1F *h_rate,*h_dtime ;
  TH2F *h_XY ;
  TH1F *h_size ;
  TH2F *h_core ;
  TPad *pad1,*pad2,*pad3,*pad4,*pad5,*pad6 ;
  TPad *pad7,*pad8,*pad9 ;
  TGraph *g_MPV,*g_sigma,*g_HV,*gl,*gm,*gn ;
  int start,flag1,flag2,flag_traces,flag_landau,stop,EXIT ;
  int select_lasa1,select_lasa2,select_lasa3,select_lasa4,select_lasa5,select_done ;
  int event_display_interval ;
  float Det[NO_LASAs*4],MPV[NO_LASAs*4],Sigma[NO_LASAs*4],HV[NO_LASAs*4] ;
  float Det_X[NO_LASAs*4],Det_Y[NO_LASAs*4],Det_Z[NO_LASAs*4] ;	//X,Y,Z coordinates of the detectors
  double Xmax,Xmin,Ymax,Ymin ;	//Boundary coords. for 2D event display
  //float Bin_Size_X,Bin_Size_Y ;	//Bin size for the 2D event display	
 public: 
  //! Argumented constructor
  MONITORING(const TGWindow *p,UInt_t w,UInt_t h) ; 
  virtual ~MONITORING() ;
  //! Read the detector coordinates
  void Read_Detector_Cord() ;
  //! Initialize the shower detector array
  void Initialisation() ; 
  void Select_Lasa1() ;
  int Select_Lasa1_return() ;
  void Select_Lasa2() ;
  int Select_Lasa2_return() ;
  void Select_Lasa3() ;
  int Select_Lasa3_return() ;
  void Select_Lasa4() ;
  int Select_Lasa4_return() ;
  void Select_Lasa5() ;
  int Select_Lasa5_return() ;
  void Select_Done() ;
  int Select_Done_return() ;
  void Start(int) ;
  int Start_return() ;
  
  void Event_Display_interval1() ;	
  void Event_Display_interval2() ;	
  void Event_Display_interval3() ;	
  void Event_Display_interval4() ;	
  void Event_Display_interval5() ;	
  int Event_Display_interval_return() ;	
  
  void Traces(int) ;
  void Landau(int) ;
  void Stop() ;
  int Stop_return() ;
  void Exit() ;
  int Exit_return() ;
  void DEFINE_Histo() ;
  void DRAW_canvas() ;
  
  void Click1() ;
  int Click1_return() ;
  void Fill_MPV_Sigma(unsigned int,float,float) ;
  void Draw_MPV_Sigma() ;
  void FILL_Hits(unsigned int) ;
  void DRAW_Hits(int) ;
  void DRAW_HV() ;
  int Click_Traces_return() ;
  int Click_Landau_return() ;
  //! Draw iD plot of the traces
  void DRAW_Traces(TH1F*,int) ;
  //! Draw 1D plot of the Landau distribution
  void DRAW_Landau(TH1F*,int,float,float,float) ;
  
  void Click2() ;
  int Click2_return() ;
  void Fill_XY_display(unsigned int,unsigned int) ;
  void DRAW_XY_display() ;
  void DRAW_Event_Rate(double) ;
  void Fill_Event_Size(unsigned int) ;
  void DRAW_Event_Size() ;
  void Fill_Event_Core(float,float) ;
  void DRAW_Event_Core() ;
  void Fill_Time_Diff(float) ;
  void DRAW_Time_Diff() ;
} ; 
