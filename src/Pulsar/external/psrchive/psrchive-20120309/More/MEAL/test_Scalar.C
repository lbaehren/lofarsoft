/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/ScalarMath.h"
#include "MEAL/ScalarParameter.h"

using namespace std;

#include "complex_public.h"

int main (int argc, char** argv) try {

  std::complex<MEAL::ScalarMath> w (1.2, 3.4);
  std::complex<MEAL::ScalarMath> z (-5.6, 7.8);

  std::complex<MEAL::ScalarMath> product = w * z;

  unsigned nparam_real = pub(product).real.get_expression()->get_nparam();
  unsigned nparam_imag = pub(product).imag.get_expression()->get_nparam();

  if (nparam_real != 0) {
    cerr << "real nparam = " << nparam_real << " != 0" << endl;
    return -1;
  }
  if (nparam_imag != 0) {
    cerr << "imag nparam = " << nparam_imag << " != 0" << endl;
    return -1;
  }

  Estimate<double> x (4.6, 0.09);
  Estimate<double> y (-0.5, 0.01);
  Estimate<double> z1, z2;

  MEAL::ScalarMath xp = *(new MEAL::ScalarParameter (x));
  MEAL::ScalarMath yp = *(new MEAL::ScalarParameter (y));
  MEAL::ScalarMath result;

  cerr << "\nTesting with:" << endl;
  cerr << "x=" << x << " y=" << y << endl;

  cerr << "\nTesting ScalarSum" << endl;

  result = xp + yp;
  z1 = x + y;
  z2 = result.get_Estimate();

  cerr << "z1=x+y=" << z1 << endl;
  cerr << "z2=x+y=" << z2 << endl;

  if (z1.val != z2.val || z1.var != z2.var)
    throw Error (InvalidState, "ScalarSum not working");
  else
    cerr << "ScalarSum passes test" << endl;



  cerr << "\nTesting ScalarNegation" << endl;

  result = xp - yp;
  z1 = x - y;
  z2 = result.get_Estimate();

  cerr << "z1=x-y=" << z1 << endl;
  cerr << "z2=x-y=" << z2 << endl;

  if (z1.val != z2.val || z1.var != z2.var)
    throw Error (InvalidState, "ScalarNegation not working");
  else
    cerr << "ScalarNegation passes test" << endl;



  cerr << "\nTesting ScalarProduct" << endl;

  result = xp * yp;
  z1 = x * y;
  z2 = result.get_Estimate();

  cerr << "z1=x*y=" << z1 << endl;
  cerr << "z2=x*y=" << z2 << endl;

  if (z1.val != z2.val || z1.var != z2.var)
    throw Error (InvalidState, "ScalarProduct not working");
  else
    cerr << "ScalarProduct passes test" << endl;

  cerr << "\nTesting Scalar squared" << endl;

  double expected = 4.0 * x.val * x.val * x.var;

  cerr << "\nThis test illustrates the limits of the Estimate template"
    "\nThe variances should be unequal, with z2.var=" << expected 
       << " (sigma=" << sqrt(expected) << ")" << endl;

  result = xp * xp;
  z1 = x * x;
  z2 = result.get_Estimate();

  cerr << "z1=xx=" << z1 << endl;
  cerr << "z2=xx=" << z2 << endl;

  if (z1.val != z2.val || fabs (z2.var - expected) > 1e-10)
    throw Error (InvalidState, "Scalar squared not working", 
		 "expected var=%f got var=%f", expected, z2.var);
  else
    cerr << "Scalar squared passes test" << endl;



  cerr << "\nTesting ScalarInverse" << endl;

  result = xp / yp;
  z1 = x / y;
  z2 = result.get_Estimate();

  cerr << "z1=x/y=" << z1 << endl;
  cerr << "z2=x/y=" << z2 << endl;

  if (z1.val != z2.val || z1.var != z2.var)
    throw Error (InvalidState, "ScalarInverse not working");
  else
    cerr << "ScalarInverse passes test" << endl;



  cerr << "\nTesting ScalarPower" << endl;

  result = sqrt(xp);
  z1 = sqrt(x);
  z2 = result.get_Estimate();

  cerr << "z1=sqrt(x)=" << z1 << endl;
  cerr << "z2=sqrt(x)=" << z2 << endl;

  if (z1.val != z2.val || z1.var != z2.var)
    throw Error (InvalidState, "ScalarPower not working");
  else
    cerr << "ScalarPower passes test" << endl;


  cerr << "\nTesting Scalar invariant interval" << endl;

  Estimate<double> I (0.9587, 0.00058);
  Estimate<double> Q (-0.16, 0.00063);
  Estimate<double> U (0.34, 0.00074);
  Estimate<double> V (-0.05, 0.00070);
  Estimate<double> inv;

  MEAL::ScalarMath SI = I;
  MEAL::ScalarMath SQ = Q;
  MEAL::ScalarMath SU = U;
  MEAL::ScalarMath SV = V;

  MEAL::ScalarMath Sinv = sqrt (SI*SI -SQ*SQ -SU*SU -SV*SV);

  inv = Sinv.get_Estimate();

  cerr << "\n\nI=" << I << "\nQ=" << Q << "\nU=" << U << "\nV=" << V 
       << "\ninv=" << inv << endl;

  double p = sqrt (Q*Q + U*U + V*V).val;
  double p_var = (Q.val*Q.val*Q.var + U.val*U.val*U.var + V.val*V.val*V.var)
                 / (p*p);

  double invariant = sqrt(I.val*I.val - p*p);
  double inv_var = (I.val*I.val*I.var + p*p*p_var) / (invariant*invariant);

  if (fabs(inv.val-invariant) > 1e-10)
    throw Error (InvalidState, "Scalar invariant interval not working",
		 "inv.val=%lf != invariant=%lf", inv.val, invariant);

  // where z = I/inv
  // dz/dI = -p^2 (I^2-p^2)^{-3/2}
  // dz/dp = Ip (I^2-p^2)^{-3/2}

  double common = 1.0 / (inv.val*inv.val*inv.val);
  double dzdI = - p*p * common;
  double dzdp = I.val*p * common;

  double var_I = dzdI*dzdI*I.var + dzdp*dzdp*p_var;

  cerr << "expected sigma of normalized intensity = " << sqrt(var_I) << endl;

  SI = SI/Sinv;
  inv = SI.get_Estimate();
  cerr << "I/inv=" << inv << endl;

  if (fabs(inv.val - I.val/invariant) > 1e-10)
    throw Error (InvalidState, "Scalar invariant interval not working",
		 "(I/inv).val=%lf != I/inv=%lf", inv.val, I.val/invariant);

  if (fabs(inv.var - var_I) > 1e-10)
    throw Error (InvalidState, "Scalar invariant interval not working",
		 "(I/inv).var=%lf != expected=%lf", inv.var, var_I);

  double invsq = invariant*invariant;
  double Usq = U.val*U.val;
  double var_U = ( (invsq + 2.0*Usq)*U.var + Usq*inv_var ) / (invsq*invsq);

  cerr << "expected sigma of normalized Stokes U = " << sqrt(var_U) << endl;

  SU = SU/Sinv;
  inv = SU.get_Estimate();
  cerr << "U/inv=" << inv << endl;

  if (fabs(inv.val - U.val/invariant) > 1e-10)
    throw Error (InvalidState, "Scalar invariant interval not working",
                 "(U/inv).val=%lf != U/inv=%lf", inv.val, U.val/invariant);

  if (fabs(inv.var - var_U) > 1e-10)
    throw Error (InvalidState, "Scalar invariant interval not working",
                 "(U/inv).var=%lf != expected=%lf", inv.var, var_U);

  cerr << 
    "Invariant interval computation passes test\n\n"
    "All tests passed.\n" 
       << endl; 

  return 0;

} catch (Error& error) {
  cerr << error << endl;
  return -1;
}

