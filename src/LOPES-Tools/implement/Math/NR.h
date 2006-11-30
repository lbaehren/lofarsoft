
#define TINY 1.0e-20
#define MAXIT 100
#define EPS 3.0e-7
#define FPMIN 1.0e-30

namespace LOPES {  // NAMESPACE LOPES -- BEGIN
  
  //! Complementary error function
  double NRerfcc (double x);
  
  //! Logarithm of the Gamma function
  double NRgammln (double xx);
  
  //! Continued fraction for the incomplete betta function
  double NRbetacf (double a,
		   double b,
		   double x);
  
  //! Incomplete beta function
  double NRbetai (double a,
		  double b,
		  double x);
  
}  // NAMESPACE LOPES -- END
