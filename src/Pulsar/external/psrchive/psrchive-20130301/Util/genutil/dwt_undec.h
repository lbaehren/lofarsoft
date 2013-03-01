/* dwt_undec.h
 * Paul Demorest, 2007/10
 */
#ifndef _DWT_UNDEC_H_
#define _DWT_UNDEC_H_
#include <gsl/gsl_wavelet.h>
#ifdef __cplusplus
extern "C" {
#endif
int dwt_undec_transform(double *in, double *out, size_t n, const gsl_wavelet *w);
int dwt_undec_inverse(double *in, double *out, size_t n, const gsl_wavelet *w);
#ifdef __cplusplus
}
#endif
#endif
