
// Standard header files
#include <stdlib.h>
#include <iostream>
#include <strstream>
// ROOT header files
#include <TMath.h>
#include <TF2.h>
#include <TH2F.h>
#include <TROOT.h>
#include <TMinuit.h>
// LASA header files
#include "Constants.h"

/*!
  \class ARRIVAL_DIRECTION
  \ingroup LASA
  \brief Reconstruction of arrival direction
*/
class ARRIVAL_DIRECTION
{

  //! Histogram storing direction of arrival
  TH2F *h_theta_phi ;
  //! Name of the histogram
  std::string histogram_p;
  //! Title of the histogram
  std::string title_p;
  //! For arrival direction calculation 
  struct AD
  {
    //! Detector having the Maximum count in an event
    unsigned int Max_Count ;
    float cdt[NO_LASAs*4] ;
    float weight[NO_LASAs*4] ;
  } ad;
  
  
 public:
  
  // === Construction ===========================================================
  
  //! Default constructor
  ARRIVAL_DIRECTION();

  //! Default constructor
  ARRIVAL_DIRECTION (std::string const &histogram,
		     std::string const &title);

  // === Parameter access =======================================================

  //! Get the name of the histogram
  inline std::string histogram () const {
    return histogram_p;
  }

  //! Get the title of the histogram
  inline std::string title () const {
    return title_p;
  }

  // === Methods ================================================================

  //! Get detector coordinates
  void Read_Detector_Cord() ;
  //! Get point of origin
  void Get_Origin(int,float *,float *) ;
  //! Perform fit to the data
  TH2F* Do_Fit() ;

 private:

  //! Initialize the object's internal data
  void init (std::string const &histogram="h_theta_phi",
	     std::string const &title="Arrival Direction");
  
};
