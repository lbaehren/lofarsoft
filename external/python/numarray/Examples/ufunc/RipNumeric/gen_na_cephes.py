### This script is used to "manually" generate _na_cephesmodule.c

import sys
from numarray.codegenerator import UfuncModule, make_stub 
from numarray.numarrayext import NumarrayExtension

from na_cephes_table import func_tab

python_code = '''

_scipy_special_errprint = 0

def errprint(val=None):
    global _scipy_special_errprint
    old_val = _scipy_special_errprint
    if val is not None:
        _scipy_special_errprint = (val != 0)
    return old_val
'''

# Added "fake" headers due to naming conflicts between Py_complex
# (really used by cephes wrappers) and Complex{32,64} (used by numarray).

fake_header_code = '''#include "cephes.h"

double besselpoly(double a, double lambda, double nu);

Complex64 cbesi_wrap( double v, Complex64 z);
Complex64 cbesi_wrap_e( double v, Complex64 z);
Complex64 cbesj_wrap( double v, Complex64 z);
Complex64 cbesj_wrap_e( double v, Complex64 z);
Complex64 cbesy_wrap( double v, Complex64 z);
Complex64 cbesy_wrap_e( double v, Complex64 z);
Complex64 cbesk_wrap( double v, Complex64 z);
Complex64 cbesk_wrap_e( double v, Complex64 z);  
Complex64 cbesh_wrap1( double v, Complex64 z);
Complex64 cbesh_wrap1_e( double v, Complex64 z);  
Complex64 cbesh_wrap2( double v, Complex64 z);
Complex64 cbesh_wrap2_e( double v, Complex64 z);

extern double cdfbet3_wrap(double p, double x, double b);
extern double cdfbet4_wrap(double p, double x, double a);

extern double cdfbin2_wrap(double p, double xn, double pr);
extern double cdfbin3_wrap(double p, double s, double pr);

extern double cdfchi3_wrap(double p, double x);

extern double cdfchn1_wrap(double x, double df, double nc);
extern double cdfchn2_wrap(double p, double df, double nc);
extern double cdfchn3_wrap(double p, double x, double nc);
extern double cdfchn4_wrap(double p, double x, double df);

extern double cdff3_wrap(double p, double f, double dfd);
extern double cdff4_wrap(double p, double f, double dfn);

extern double cdffnc1_wrap(double f, double dfn, double dfd, double nc);
extern double cdffnc2_wrap(double p, double dfn, double dfd, double nc);
extern double cdffnc3_wrap(double p, double f, double dfd, double nc);
extern double cdffnc4_wrap(double p, double f, double dfn, double nc);
extern double cdffnc5_wrap(double p, double f, double dfn, double dfd);

extern double cdfgam1_wrap(double p, double x, double scl);
extern double cdfgam2_wrap(double p, double x, double shp);
extern double cdfgam3_wrap(double p, double x, double scl);
extern double cdfgam4_wrap(double p, double x, double shp);

extern double cdfnbn2_wrap(double p, double xn, double pr);
extern double cdfnbn3_wrap(double p, double s, double pr);

extern double cdfnor3_wrap(double p, double x, double std);
extern double cdfnor4_wrap(double p, double x, double mn);

extern double cdfpoi2_wrap(double p, double xlam);

extern double cdft1_wrap(double p, double t);
extern double cdft2_wrap(double p, double t);
extern double cdft3_wrap(double p, double t);

extern double cdftnc1_wrap(double df, double nc, double t);
extern double cdftnc2_wrap(double df, double nc, double p);
extern double cdftnc3_wrap(double p, double nc, double t);
extern double cdftnc4_wrap(double df, double p, double t);

extern double tukeylambdacdf(double x, double lambda);

Complex64 cgamma_wrap( Complex64 z);
Complex64 clngamma_wrap( Complex64 z);
Complex64 cpsi_wrap( Complex64 z);
Complex64 crgamma_wrap( Complex64 z);
Complex64 chyp2f1_wrap( double a, double b, double c, Complex64 z);
Complex64 chyp1f1_wrap( double a, double b, Complex64 z);
double hypU_wrap(double a, double b, double x);
double exp1_wrap(double x);
double expi_wrap(double x);
Complex64 cexp1_wrap( Complex64 z);
Complex64 cerf_wrap( Complex64 z);
int itairy_wrap(double x, double *apt, double *bpt, double *ant, double *bnt);

double struve_wrap(double v, double x);
double itstruve0_wrap(double x);
double it2struve0_wrap(double x);

double modstruve_wrap(double v, double x);
double itmodstruve0_wrap(double x);

double ber_wrap(double x);
double bei_wrap(double x);
double ker_wrap(double x);
double kei_wrap(double x);
double berp_wrap(double x);
double beip_wrap(double x);
double kerp_wrap(double x);
double keip_wrap(double x);

int kelvin_wrap(double x, Complex64 *Be, Complex64 *Ke, Complex64 *Bep, Complex64 *Kep);

int it1j0y0_wrap(double x, double *, double *);
int it2j0y0_wrap(double x, double *, double *);
int it1i0k0_wrap(double x, double *, double *);
int it2i0k0_wrap(double x, double *, double *);

int cfresnl_wrap(Complex64 x, Complex64 *sf, Complex64 *cf);
double cem_cva_wrap(double m, double q);
double sem_cva_wrap(double m, double q);
int cem_wrap(double m, double q, double x, double *csf, double *csd);
int sem_wrap(double m, double q, double x, double *csf, double *csd);
int mcm1_wrap(double m, double q, double x, double *f1r, double *d1r);
int msm1_wrap(double m, double q, double x, double *f1r, double *d1r);
int mcm2_wrap(double m, double q, double x, double *f2r, double *d2r);
int msm2_wrap(double m, double q, double x, double *f2r, double *d2r);
double pmv_wrap(double, double, double);
int pbwa_wrap(double, double, double *, double *);
int pbdv_wrap(double, double, double *, double *);
int pbvv_wrap(double, double, double *, double *);

int prolate_aswfa_wrap(double, double, double, double, double, double *, double *);
int prolate_radial1_wrap(double, double, double, double, double, double *, double *);
int prolate_radial2_wrap(double, double, double, double, double, double *, double *);

/*
int oblate_aswfa_wrap(double, double, double, double, double, double *, double *);
int oblate_radial1_wrap(double, double, double, double, double, double *, double *);
int oblate_radial2_wrap(double, double, double, double, double, double *, double *);
double prolate_aswfa_nocv_wrap(double, double, double, double, double *);
double prolate_radial1_nocv_wrap(double, double, double, double, double *);
double prolate_radial2_nocv_wrap(double, double, double, double, double *);
double oblate_aswfa_nocv_wrap(double, double, double, double, double *);
double oblate_radial1_nocv_wrap(double, double, double, double, double *);
double oblate_radial2_nocv_wrap(double, double, double, double, double *);
*/

double prolate_segv_wrap(double, double, double);
double oblate_segv_wrap(double, double, double);


int modified_fresnel_plus_wrap(double x, Complex64 *F, Complex64 *K);
int modified_fresnel_minus_wrap(double x, Complex64 *F, Complex64 *K);

extern Complex64 cwofz_wrap(Complex64 z);

'''


def gencode():
    m = UfuncModule('_na_cephes')

    m.add_code( fake_header_code ) 
    
    for r in func_tab:
        ufunc, c_func, c_sig, forms, ufsigs = r
        print "adding",ufunc
        m.add_nary_ufunc( ufunc_name=ufunc,
                          c_function=c_func,
                          forms=forms,
                          signatures=ufsigs,
                          c_signature=c_sig)
        
    m.generate('_na_cephesmodule.c')
    make_stub('na_cephes', '_numarray._na_cephes',
              add_code = python_code)  

if __name__ == "__main__":
    gencode()

