/* dwt_undec.c
 * Paul Demorest, 2007/10
 *
 * Undecimated wavelet transform and inverse.  These
 * do the usual wavelet filter tree, but don't decimate
 * by 2 after each step.  This makes the transform 
 * translation-invariant (to translations which are 
 * integer numbers of samples anyways).
 *
 * Ends up with n*log2(n) sized array, from size n input.
 *
 * Wavelet coeffs g and h need to be externally supplied.
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <gsl/gsl_wavelet.h>

/* Do one UDWT step on input data, out-of-place (original 
 * data array is unaltered).
 * Vars:
 *   in  pointer to input data (n doubles)
 *   outl pointer to low-pass output data (n doubles)
 *   outh pointer to high-pass output data (n doubles)
 *   n   size of in/out
 *   h   Wavelet coeffs, low-pass (nc doubles)
 *   g   Wavelet coeffs, high-pass (nc doubles)
 *   nc  size of h/g
 *   s   scale (filters expand by 2^s)
 */
void udwt_forward_step(double *in, double *outl, double *outh, size_t n, 
        const double h[], const double g[], size_t nc, int s) {

    int i,j,idx;

    /* Zero output array */
    for (i=0; i<n; i++) { outl[i]=0.0; outh[i]=0.0; }

    /* Filter */
    for (i=0; i<n; i++) {
        for (j=0; j<nc; j++) {
            idx = (i + (1<<s)*j) % n;
            outl[i] += h[j] * in[idx];
            outh[i] += g[j] * in[idx];
        }
    }
}

/* Inverse UDWT step similar to above.  Remember to stay
 * consistent about which half contains h and which
 * contains g chunks!
 */
void udwt_inverse_step(double *inl, double *inh, double *out, size_t n,
        const double h[], const double g[], size_t nc, int s) {

    int i,j,idx;

    /* Zero output */
    for (i=0; i<n; i++) { out[i]=0.0; }

    /* filter, add */
    for (i=0; i<n; i++) {
        for (j=0; j<nc; j++) {
            idx = (i - (1<<s)*j) % n;
            out[i] += h[j] * inl[idx];
            out[i] += g[j] * inh[idx];
        }
    }

    /* Inverse needs a rescaling */
    for (i=0; i<n; i++) { out[i] *= 0.5; }
}

/* Return integer log 2 of input.
 * Error if not a power of 2?
 */
static int log2i(int n) { 
    int rv=0;
    while ((n>>rv)>1) { rv++; }
    return(rv);
}

/* Do a undec wavelet transform on input data n.
 * Vars:
 *   in  pointer to input data (n doubles)
 *   out pointer to output data (n*log2(n) doubles)
 *   n   size of in
 *   h   Wavelet coeffs, high-pass (nc doubles)
 *   g   Wavelet coeffs, low-pass (nc doubles)
 *   nc  size of h/g
 *
 * This can be called using a GSL wavelet definition struct like:
 * gsl_wavelet *w;
 * (etc, init wavelet...)
 * dwt_undec_transform(in, &out, n, w->h1, w->g1, w->nc);
 */
int _dwt_undec_transform(double *in, double *out, size_t n, 
        const double h[], const double g[], size_t nc) {

    /* Determine number of levels of decomp */
    int nl=0;
    nl = log2i(n);

    /* maybe do some value checks on n, nl .. */

    /* Temporary low-pass outputs */
    double *tmp1 = (double *)malloc(sizeof(double)*n);
    double *tmp2 = (double *)malloc(sizeof(double)*n);

    /* Loop to do dwts */
    int i;
    double *curi, *curol, *curoh, *swp;
    curi = in;
    curol = tmp1;
    curoh = out;
    for (i=0; i<nl; i++) {
        udwt_forward_step(curi, curol, curoh, n, h, g, nc, i);
        if (i==0) { curi=tmp2; }
        swp = curi;
        curi = curol;
        curol = swp;
        curoh = &(out[(i+1)*n]);
    }

    free(tmp1);
    free(tmp2);
    return(nl);
}

/* Inverse, similar to above, but switch in/out
 * This can be called using a GSL wavelet definition struct like:
 * gsl_wavelet *w;
 * (etc, init wavelet...)
 * dwt_undec_inverse(in, out, n, w->h2, w->g2, w->nc);
 */
int _dwt_undec_inverse(double *in, double *out, size_t n, 
        const double h[], const double g[], size_t nc) {

    /* Determine number of levels of decomp */
    int nl=0;
    nl = log2i(n);

    /* maybe do some value checks on n, nl .. */

    /* Temporary low-pass outputs */
    int i;
    double *tmp1 = (double *)malloc(sizeof(double)*n);
    double *tmp2 = (double *)malloc(sizeof(double)*n);
    for (i=0; i<n; i++) { tmp1[i]=0.0; }

    /* Loop to do dwts */
    double *curil, *curih, *curo, *swp;
    curil = tmp1;
    curih = &in[(nl-1)*n];
    curo = tmp2;
    for (i=nl-1; i>=0; i--) {
        udwt_inverse_step(curil, curih, curo, n, h, g, nc, i);
        swp = curil;
        curil = curo;
        curo = swp;
        curih = &in[(i-1)*n];
    }

    /* We end up with final output in curil */
    for (i=0; i<n; i++) { out[i] = curil[i]; }

    free(tmp1);
    free(tmp2);
    return(nl);
}

/* Wrappers using GSL wavelet definitions */
int dwt_undec_transform(double *in, double *out, size_t n, const gsl_wavelet *w) {
    return _dwt_undec_transform(in, out, n, w->h1, w->g1, w->nc);
}
int dwt_undec_inverse(double *in, double *out, size_t n, const gsl_wavelet *w) {
    return _dwt_undec_inverse(in, out, n, w->h2, w->g2, w->nc);
}

