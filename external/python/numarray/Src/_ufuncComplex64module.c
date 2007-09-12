
#include <Python.h>
#include <stdio.h>
#include <math.h>
#include "libnumarray.h"

#ifdef MS_WIN32
#pragma warning(once : 4244)
#endif

#define logical_and(arg1, arg2) (arg1 != 0) & (arg2 != 0)
#define logical_or(arg1, arg2)  (arg1 != 0) | (arg2 != 0)
#define logical_xor(arg1, arg2) ((arg1 != 0) ^ (arg2 != 0)) & 1
#define ufmaximum(arg1, arg2) (((temp1=arg1) > (temp2=arg2)) ? temp1 : temp2)
#define ufminimum(arg1, arg2) (((temp1=arg1) < (temp2=arg2)) ? temp1 : temp2)

#define distance3d(x,y,z) sqrt(x*x + y*y + z*z)

/*********************  _distance3d  *********************/
/*********************  minus  *********************/
/*********************  add  *********************/
/*********************  subtract  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  divide  *********************/
/*********************  divide  *********************/
/*********************  floor_divide  *********************/
/*********************  floor_divide  *********************/
/*********************  true_divide  *********************/
/*********************  true_divide  *********************/
/*********************  remainder  *********************/
/*********************  remainder  *********************/
/*********************  power  *********************/
/*********************  abs  *********************/
/*********************  sin  *********************/
/*********************  cos  *********************/
/*********************  tan  *********************/
/*********************  arcsin  *********************/
/*********************  arccos  *********************/
/*********************  arctan  *********************/
/*********************  arctan2  *********************/
/*********************  log  *********************/
/*********************  log10  *********************/
/*********************  exp  *********************/
/*********************  sinh  *********************/
/*********************  cosh  *********************/
/*********************  tanh  *********************/
/*********************  arcsinh  *********************/
/*********************  arccosh  *********************/
/*********************  arctanh  *********************/
/*********************  ieeemask  *********************/
/*********************  ieeemask  *********************/
/*********************  isnan  *********************/
/*********************  isnan  *********************/
/*********************  isnan  *********************/
/*********************  isnan  *********************/

static int isnan_DxB_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64  *tin0 =  (Complex64 *) buffers[0];
    Bool       *tout0 =  (Bool *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NA_isnanC64(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor isnan_DxB_vxf_descr =
{ "isnan_DxB_vxf", (void *) isnan_DxB_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Complex64), sizeof(Bool) }, { 0, 0, 0 } };
/*********************  sqrt  *********************/
/*********************  equal  *********************/
/*********************  not_equal  *********************/
/*********************  greater  *********************/
/*********************  greater_equal  *********************/
/*********************  less  *********************/
/*********************  less_equal  *********************/
/*********************  logical_and  *********************/
/*********************  logical_or  *********************/
/*********************  logical_xor  *********************/
/*********************  logical_and  *********************/
/*********************  logical_or  *********************/
/*********************  logical_xor  *********************/
/*********************  logical_not  *********************/
/*********************  bitwise_and  *********************/
/*********************  bitwise_or  *********************/
/*********************  bitwise_xor  *********************/
/*********************  bitwise_not  *********************/
/*********************  bitwise_not  *********************/
/*********************  rshift  *********************/
/*********************  lshift  *********************/
/*********************  floor  *********************/
/*********************  floor  *********************/
/*********************  ceil  *********************/
/*********************  ceil  *********************/
/*********************  maximum  *********************/
/*********************  minimum  *********************/
/*********************  fabs  *********************/
/*********************  _round  *********************/
/*********************  hypot  *********************/
/*********************  minus  *********************/

static int minus_DxD_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64  *tin0 =  (Complex64 *) buffers[0];
    Complex64  *tout0 =  (Complex64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CMINUS(*tin0, *tout0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor minus_DxD_vxv_descr =
{ "minus_DxD_vxv", (void *) minus_DxD_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Complex64), sizeof(Complex64) }, { 0, 0, 0 } };
/*********************  add  *********************/

static int add_DDxD_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64  *tin0 =  (Complex64 *) buffers[0];
    Complex64   tin1 = *(Complex64 *) buffers[1];
    Complex64  *tout0 =  (Complex64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CADD(*tin0, tin1, *tout0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor add_DDxD_vsxv_descr =
{ "add_DDxD_vsxv", (void *) add_DDxD_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex64), sizeof(Complex64), sizeof(Complex64) }, { 0, 1, 0, 0 } };

static int add_DDxD_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64  *tin0 =  (Complex64 *) buffers[0];
    Complex64  *tin1 =  (Complex64 *) buffers[1];
    Complex64  *tout0 =  (Complex64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CADD(*tin0, *tin1, *tout0);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor add_DDxD_vvxv_descr =
{ "add_DDxD_vvxv", (void *) add_DDxD_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex64), sizeof(Complex64), sizeof(Complex64) }, { 0, 0, 0, 0 } };

static void _add_DxD_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Complex64  *tin0   = (Complex64 *) ((char *) input  + inboffset);
    Complex64 *tout0  = (Complex64 *) ((char *) output + outboffset);
    Complex64 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Complex64 *) ((char *) tin0 + inbstrides[dim]);
            NUM_CADD(net, *tin0, net);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _add_DxD_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int add_DxD_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _add_DxD_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(add_DxD_R, CHECK_ALIGN, sizeof(Complex64), sizeof(Complex64));

static void _add_DxD_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Complex64 *tin0   = (Complex64 *) ((char *) input  + inboffset);
    Complex64 *tout0 = (Complex64 *) ((char *) output + outboffset);
    Complex64 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Complex64 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Complex64 *) ((char *) tout0 + outbstrides[dim]);
            NUM_CADD(lastval ,*tin0, *tout0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _add_DxD_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  add_DxD_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _add_DxD_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(add_DxD_A, CHECK_ALIGN, sizeof(Complex64), sizeof(Complex64));

static int add_DDxD_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64   tin0 = *(Complex64 *) buffers[0];
    Complex64  *tin1 =  (Complex64 *) buffers[1];
    Complex64  *tout0 =  (Complex64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CADD(tin0, *tin1, *tout0);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor add_DDxD_svxv_descr =
{ "add_DDxD_svxv", (void *) add_DDxD_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex64), sizeof(Complex64), sizeof(Complex64) }, { 1, 0, 0, 0 } };
/*********************  subtract  *********************/

static int subtract_DDxD_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64  *tin0 =  (Complex64 *) buffers[0];
    Complex64   tin1 = *(Complex64 *) buffers[1];
    Complex64  *tout0 =  (Complex64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CSUB(*tin0, tin1, *tout0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor subtract_DDxD_vsxv_descr =
{ "subtract_DDxD_vsxv", (void *) subtract_DDxD_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex64), sizeof(Complex64), sizeof(Complex64) }, { 0, 1, 0, 0 } };

static int subtract_DDxD_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64  *tin0 =  (Complex64 *) buffers[0];
    Complex64  *tin1 =  (Complex64 *) buffers[1];
    Complex64  *tout0 =  (Complex64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CSUB(*tin0, *tin1, *tout0);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor subtract_DDxD_vvxv_descr =
{ "subtract_DDxD_vvxv", (void *) subtract_DDxD_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex64), sizeof(Complex64), sizeof(Complex64) }, { 0, 0, 0, 0 } };

static void _subtract_DxD_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Complex64  *tin0   = (Complex64 *) ((char *) input  + inboffset);
    Complex64 *tout0  = (Complex64 *) ((char *) output + outboffset);
    Complex64 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Complex64 *) ((char *) tin0 + inbstrides[dim]);
            NUM_CSUB(net, *tin0, net);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _subtract_DxD_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int subtract_DxD_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _subtract_DxD_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(subtract_DxD_R, CHECK_ALIGN, sizeof(Complex64), sizeof(Complex64));

static void _subtract_DxD_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Complex64 *tin0   = (Complex64 *) ((char *) input  + inboffset);
    Complex64 *tout0 = (Complex64 *) ((char *) output + outboffset);
    Complex64 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Complex64 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Complex64 *) ((char *) tout0 + outbstrides[dim]);
            NUM_CSUB(lastval ,*tin0, *tout0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _subtract_DxD_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  subtract_DxD_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _subtract_DxD_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(subtract_DxD_A, CHECK_ALIGN, sizeof(Complex64), sizeof(Complex64));

static int subtract_DDxD_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64   tin0 = *(Complex64 *) buffers[0];
    Complex64  *tin1 =  (Complex64 *) buffers[1];
    Complex64  *tout0 =  (Complex64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CSUB(tin0, *tin1, *tout0);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor subtract_DDxD_svxv_descr =
{ "subtract_DDxD_svxv", (void *) subtract_DDxD_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex64), sizeof(Complex64), sizeof(Complex64) }, { 1, 0, 0, 0 } };
/*********************  multiply  *********************/

static int multiply_DDxD_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64  *tin0 =  (Complex64 *) buffers[0];
    Complex64   tin1 = *(Complex64 *) buffers[1];
    Complex64  *tout0 =  (Complex64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CMUL(*tin0, tin1, *tout0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor multiply_DDxD_vsxv_descr =
{ "multiply_DDxD_vsxv", (void *) multiply_DDxD_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex64), sizeof(Complex64), sizeof(Complex64) }, { 0, 1, 0, 0 } };

static int multiply_DDxD_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64  *tin0 =  (Complex64 *) buffers[0];
    Complex64  *tin1 =  (Complex64 *) buffers[1];
    Complex64  *tout0 =  (Complex64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CMUL(*tin0, *tin1, *tout0);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor multiply_DDxD_vvxv_descr =
{ "multiply_DDxD_vvxv", (void *) multiply_DDxD_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex64), sizeof(Complex64), sizeof(Complex64) }, { 0, 0, 0, 0 } };

static void _multiply_DxD_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Complex64  *tin0   = (Complex64 *) ((char *) input  + inboffset);
    Complex64 *tout0  = (Complex64 *) ((char *) output + outboffset);
    Complex64 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Complex64 *) ((char *) tin0 + inbstrides[dim]);
            NUM_CMUL(net, *tin0, net);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _multiply_DxD_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int multiply_DxD_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _multiply_DxD_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(multiply_DxD_R, CHECK_ALIGN, sizeof(Complex64), sizeof(Complex64));

static void _multiply_DxD_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Complex64 *tin0   = (Complex64 *) ((char *) input  + inboffset);
    Complex64 *tout0 = (Complex64 *) ((char *) output + outboffset);
    Complex64 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Complex64 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Complex64 *) ((char *) tout0 + outbstrides[dim]);
            NUM_CMUL(lastval ,*tin0, *tout0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _multiply_DxD_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  multiply_DxD_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _multiply_DxD_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(multiply_DxD_A, CHECK_ALIGN, sizeof(Complex64), sizeof(Complex64));

static int multiply_DDxD_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64   tin0 = *(Complex64 *) buffers[0];
    Complex64  *tin1 =  (Complex64 *) buffers[1];
    Complex64  *tout0 =  (Complex64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CMUL(tin0, *tin1, *tout0);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor multiply_DDxD_svxv_descr =
{ "multiply_DDxD_svxv", (void *) multiply_DDxD_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex64), sizeof(Complex64), sizeof(Complex64) }, { 1, 0, 0, 0 } };
/*********************  divide  *********************/

static int divide_DDxD_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64  *tin0 =  (Complex64 *) buffers[0];
    Complex64   tin1 = *(Complex64 *) buffers[1];
    Complex64  *tout0 =  (Complex64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CDIV(*tin0, tin1, *tout0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor divide_DDxD_vsxv_descr =
{ "divide_DDxD_vsxv", (void *) divide_DDxD_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex64), sizeof(Complex64), sizeof(Complex64) }, { 0, 1, 0, 0 } };

static int divide_DDxD_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64  *tin0 =  (Complex64 *) buffers[0];
    Complex64  *tin1 =  (Complex64 *) buffers[1];
    Complex64  *tout0 =  (Complex64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CDIV(*tin0, *tin1, *tout0);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor divide_DDxD_vvxv_descr =
{ "divide_DDxD_vvxv", (void *) divide_DDxD_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex64), sizeof(Complex64), sizeof(Complex64) }, { 0, 0, 0, 0 } };

static void _divide_DxD_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Complex64  *tin0   = (Complex64 *) ((char *) input  + inboffset);
    Complex64 *tout0  = (Complex64 *) ((char *) output + outboffset);
    Complex64 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Complex64 *) ((char *) tin0 + inbstrides[dim]);
            NUM_CDIV(net, *tin0, net);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _divide_DxD_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int divide_DxD_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _divide_DxD_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(divide_DxD_R, CHECK_ALIGN, sizeof(Complex64), sizeof(Complex64));

static void _divide_DxD_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Complex64 *tin0   = (Complex64 *) ((char *) input  + inboffset);
    Complex64 *tout0 = (Complex64 *) ((char *) output + outboffset);
    Complex64 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Complex64 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Complex64 *) ((char *) tout0 + outbstrides[dim]);
            NUM_CDIV(lastval ,*tin0, *tout0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _divide_DxD_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  divide_DxD_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _divide_DxD_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(divide_DxD_A, CHECK_ALIGN, sizeof(Complex64), sizeof(Complex64));

static int divide_DDxD_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64   tin0 = *(Complex64 *) buffers[0];
    Complex64  *tin1 =  (Complex64 *) buffers[1];
    Complex64  *tout0 =  (Complex64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CDIV(tin0, *tin1, *tout0);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor divide_DDxD_svxv_descr =
{ "divide_DDxD_svxv", (void *) divide_DDxD_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex64), sizeof(Complex64), sizeof(Complex64) }, { 1, 0, 0, 0 } };
/*********************  true_divide  *********************/

static int true_divide_DDxD_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64  *tin0 =  (Complex64 *) buffers[0];
    Complex64   tin1 = *(Complex64 *) buffers[1];
    Complex64  *tout0 =  (Complex64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CDIV(*tin0, tin1, *tout0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor true_divide_DDxD_vsxv_descr =
{ "true_divide_DDxD_vsxv", (void *) true_divide_DDxD_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex64), sizeof(Complex64), sizeof(Complex64) }, { 0, 1, 0, 0 } };

static int true_divide_DDxD_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64  *tin0 =  (Complex64 *) buffers[0];
    Complex64  *tin1 =  (Complex64 *) buffers[1];
    Complex64  *tout0 =  (Complex64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CDIV(*tin0, *tin1, *tout0);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor true_divide_DDxD_vvxv_descr =
{ "true_divide_DDxD_vvxv", (void *) true_divide_DDxD_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex64), sizeof(Complex64), sizeof(Complex64) }, { 0, 0, 0, 0 } };

static void _true_divide_DxD_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Complex64  *tin0   = (Complex64 *) ((char *) input  + inboffset);
    Complex64 *tout0  = (Complex64 *) ((char *) output + outboffset);
    Complex64 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Complex64 *) ((char *) tin0 + inbstrides[dim]);
            NUM_CDIV(net, *tin0, net);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _true_divide_DxD_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int true_divide_DxD_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _true_divide_DxD_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(true_divide_DxD_R, CHECK_ALIGN, sizeof(Complex64), sizeof(Complex64));

static void _true_divide_DxD_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Complex64 *tin0   = (Complex64 *) ((char *) input  + inboffset);
    Complex64 *tout0 = (Complex64 *) ((char *) output + outboffset);
    Complex64 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Complex64 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Complex64 *) ((char *) tout0 + outbstrides[dim]);
            NUM_CDIV(lastval ,*tin0, *tout0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _true_divide_DxD_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  true_divide_DxD_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _true_divide_DxD_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(true_divide_DxD_A, CHECK_ALIGN, sizeof(Complex64), sizeof(Complex64));

static int true_divide_DDxD_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64   tin0 = *(Complex64 *) buffers[0];
    Complex64  *tin1 =  (Complex64 *) buffers[1];
    Complex64  *tout0 =  (Complex64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CDIV(tin0, *tin1, *tout0);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor true_divide_DDxD_svxv_descr =
{ "true_divide_DDxD_svxv", (void *) true_divide_DDxD_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex64), sizeof(Complex64), sizeof(Complex64) }, { 1, 0, 0, 0 } };
/*********************  remainder  *********************/

static int remainder_DDxD_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64  *tin0 =  (Complex64 *) buffers[0];
    Complex64   tin1 = *(Complex64 *) buffers[1];
    Complex64  *tout0 =  (Complex64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CREM(*tin0, tin1, *tout0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor remainder_DDxD_vsxv_descr =
{ "remainder_DDxD_vsxv", (void *) remainder_DDxD_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex64), sizeof(Complex64), sizeof(Complex64) }, { 0, 1, 0, 0 } };

static int remainder_DDxD_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64  *tin0 =  (Complex64 *) buffers[0];
    Complex64  *tin1 =  (Complex64 *) buffers[1];
    Complex64  *tout0 =  (Complex64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CREM(*tin0, *tin1, *tout0);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor remainder_DDxD_vvxv_descr =
{ "remainder_DDxD_vvxv", (void *) remainder_DDxD_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex64), sizeof(Complex64), sizeof(Complex64) }, { 0, 0, 0, 0 } };

static void _remainder_DxD_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Complex64  *tin0   = (Complex64 *) ((char *) input  + inboffset);
    Complex64 *tout0  = (Complex64 *) ((char *) output + outboffset);
    Complex64 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Complex64 *) ((char *) tin0 + inbstrides[dim]);
            NUM_CREM(net, *tin0, net);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _remainder_DxD_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int remainder_DxD_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _remainder_DxD_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(remainder_DxD_R, CHECK_ALIGN, sizeof(Complex64), sizeof(Complex64));

static void _remainder_DxD_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Complex64 *tin0   = (Complex64 *) ((char *) input  + inboffset);
    Complex64 *tout0 = (Complex64 *) ((char *) output + outboffset);
    Complex64 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Complex64 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Complex64 *) ((char *) tout0 + outbstrides[dim]);
            NUM_CREM(lastval ,*tin0, *tout0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _remainder_DxD_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  remainder_DxD_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _remainder_DxD_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(remainder_DxD_A, CHECK_ALIGN, sizeof(Complex64), sizeof(Complex64));

static int remainder_DDxD_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64   tin0 = *(Complex64 *) buffers[0];
    Complex64  *tin1 =  (Complex64 *) buffers[1];
    Complex64  *tout0 =  (Complex64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CREM(tin0, *tin1, *tout0);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor remainder_DDxD_svxv_descr =
{ "remainder_DDxD_svxv", (void *) remainder_DDxD_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex64), sizeof(Complex64), sizeof(Complex64) }, { 1, 0, 0, 0 } };
/*********************  power  *********************/

static int power_DDxD_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64  *tin0 =  (Complex64 *) buffers[0];
    Complex64   tin1 = *(Complex64 *) buffers[1];
    Complex64  *tout0 =  (Complex64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CPOW(*tin0, tin1, *tout0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor power_DDxD_vsxv_descr =
{ "power_DDxD_vsxv", (void *) power_DDxD_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex64), sizeof(Complex64), sizeof(Complex64) }, { 0, 1, 0, 0 } };

static int power_DDxD_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64  *tin0 =  (Complex64 *) buffers[0];
    Complex64  *tin1 =  (Complex64 *) buffers[1];
    Complex64  *tout0 =  (Complex64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CPOW(*tin0, *tin1, *tout0);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor power_DDxD_vvxv_descr =
{ "power_DDxD_vvxv", (void *) power_DDxD_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex64), sizeof(Complex64), sizeof(Complex64) }, { 0, 0, 0, 0 } };

static void _power_DxD_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Complex64  *tin0   = (Complex64 *) ((char *) input  + inboffset);
    Complex64 *tout0  = (Complex64 *) ((char *) output + outboffset);
    Complex64 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Complex64 *) ((char *) tin0 + inbstrides[dim]);
            NUM_CPOW(net, *tin0, net);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _power_DxD_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int power_DxD_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _power_DxD_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(power_DxD_R, CHECK_ALIGN, sizeof(Complex64), sizeof(Complex64));

static void _power_DxD_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Complex64 *tin0   = (Complex64 *) ((char *) input  + inboffset);
    Complex64 *tout0 = (Complex64 *) ((char *) output + outboffset);
    Complex64 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Complex64 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Complex64 *) ((char *) tout0 + outbstrides[dim]);
            NUM_CPOW(lastval ,*tin0, *tout0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _power_DxD_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  power_DxD_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _power_DxD_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(power_DxD_A, CHECK_ALIGN, sizeof(Complex64), sizeof(Complex64));

static int power_DDxD_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64   tin0 = *(Complex64 *) buffers[0];
    Complex64  *tin1 =  (Complex64 *) buffers[1];
    Complex64  *tout0 =  (Complex64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CPOW(tin0, *tin1, *tout0);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor power_DDxD_svxv_descr =
{ "power_DDxD_svxv", (void *) power_DDxD_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex64), sizeof(Complex64), sizeof(Complex64) }, { 1, 0, 0, 0 } };
/*********************  abs  *********************/

static int abs_Dxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64  *tin0 =  (Complex64 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CABS(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor abs_Dxd_vxf_descr =
{ "abs_Dxd_vxf", (void *) abs_Dxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Complex64), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  sin  *********************/

static int sin_DxD_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64  *tin0 =  (Complex64 *) buffers[0];
    Complex64  *tout0 =  (Complex64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CSIN(*tin0, *tout0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor sin_DxD_vxv_descr =
{ "sin_DxD_vxv", (void *) sin_DxD_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Complex64), sizeof(Complex64) }, { 0, 0, 0 } };
/*********************  cos  *********************/

static int cos_DxD_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64  *tin0 =  (Complex64 *) buffers[0];
    Complex64  *tout0 =  (Complex64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CCOS(*tin0, *tout0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor cos_DxD_vxv_descr =
{ "cos_DxD_vxv", (void *) cos_DxD_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Complex64), sizeof(Complex64) }, { 0, 0, 0 } };
/*********************  tan  *********************/

static int tan_DxD_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64  *tin0 =  (Complex64 *) buffers[0];
    Complex64  *tout0 =  (Complex64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CTAN(*tin0, *tout0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor tan_DxD_vxv_descr =
{ "tan_DxD_vxv", (void *) tan_DxD_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Complex64), sizeof(Complex64) }, { 0, 0, 0 } };
/*********************  arcsin  *********************/

static int arcsin_DxD_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64  *tin0 =  (Complex64 *) buffers[0];
    Complex64  *tout0 =  (Complex64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CASIN(*tin0, *tout0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arcsin_DxD_vxv_descr =
{ "arcsin_DxD_vxv", (void *) arcsin_DxD_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Complex64), sizeof(Complex64) }, { 0, 0, 0 } };
/*********************  arccos  *********************/

static int arccos_DxD_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64  *tin0 =  (Complex64 *) buffers[0];
    Complex64  *tout0 =  (Complex64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CACOS(*tin0, *tout0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arccos_DxD_vxv_descr =
{ "arccos_DxD_vxv", (void *) arccos_DxD_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Complex64), sizeof(Complex64) }, { 0, 0, 0 } };
/*********************  arctan  *********************/

static int arctan_DxD_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64  *tin0 =  (Complex64 *) buffers[0];
    Complex64  *tout0 =  (Complex64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CATAN(*tin0, *tout0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arctan_DxD_vxv_descr =
{ "arctan_DxD_vxv", (void *) arctan_DxD_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Complex64), sizeof(Complex64) }, { 0, 0, 0 } };
/*********************  arcsinh  *********************/

static int arcsinh_DxD_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64  *tin0 =  (Complex64 *) buffers[0];
    Complex64  *tout0 =  (Complex64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CASINH(*tin0, *tout0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arcsinh_DxD_vxv_descr =
{ "arcsinh_DxD_vxv", (void *) arcsinh_DxD_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Complex64), sizeof(Complex64) }, { 0, 0, 0 } };
/*********************  arccosh  *********************/

static int arccosh_DxD_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64  *tin0 =  (Complex64 *) buffers[0];
    Complex64  *tout0 =  (Complex64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CACOSH(*tin0, *tout0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arccosh_DxD_vxv_descr =
{ "arccosh_DxD_vxv", (void *) arccosh_DxD_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Complex64), sizeof(Complex64) }, { 0, 0, 0 } };
/*********************  arctanh  *********************/

static int arctanh_DxD_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64  *tin0 =  (Complex64 *) buffers[0];
    Complex64  *tout0 =  (Complex64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CATANH(*tin0, *tout0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arctanh_DxD_vxv_descr =
{ "arctanh_DxD_vxv", (void *) arctanh_DxD_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Complex64), sizeof(Complex64) }, { 0, 0, 0 } };
/*********************  log  *********************/

static int log_DxD_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64  *tin0 =  (Complex64 *) buffers[0];
    Complex64  *tout0 =  (Complex64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CLOG(*tin0, *tout0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor log_DxD_vxv_descr =
{ "log_DxD_vxv", (void *) log_DxD_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Complex64), sizeof(Complex64) }, { 0, 0, 0 } };
/*********************  log10  *********************/

static int log10_DxD_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64  *tin0 =  (Complex64 *) buffers[0];
    Complex64  *tout0 =  (Complex64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CLOG10(*tin0, *tout0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor log10_DxD_vxv_descr =
{ "log10_DxD_vxv", (void *) log10_DxD_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Complex64), sizeof(Complex64) }, { 0, 0, 0 } };
/*********************  exp  *********************/

static int exp_DxD_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64  *tin0 =  (Complex64 *) buffers[0];
    Complex64  *tout0 =  (Complex64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CEXP(*tin0, *tout0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor exp_DxD_vxv_descr =
{ "exp_DxD_vxv", (void *) exp_DxD_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Complex64), sizeof(Complex64) }, { 0, 0, 0 } };
/*********************  sinh  *********************/

static int sinh_DxD_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64  *tin0 =  (Complex64 *) buffers[0];
    Complex64  *tout0 =  (Complex64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CSINH(*tin0, *tout0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor sinh_DxD_vxv_descr =
{ "sinh_DxD_vxv", (void *) sinh_DxD_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Complex64), sizeof(Complex64) }, { 0, 0, 0 } };
/*********************  cosh  *********************/

static int cosh_DxD_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64  *tin0 =  (Complex64 *) buffers[0];
    Complex64  *tout0 =  (Complex64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CCOSH(*tin0, *tout0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor cosh_DxD_vxv_descr =
{ "cosh_DxD_vxv", (void *) cosh_DxD_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Complex64), sizeof(Complex64) }, { 0, 0, 0 } };
/*********************  tanh  *********************/

static int tanh_DxD_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64  *tin0 =  (Complex64 *) buffers[0];
    Complex64  *tout0 =  (Complex64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CTANH(*tin0, *tout0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor tanh_DxD_vxv_descr =
{ "tanh_DxD_vxv", (void *) tanh_DxD_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Complex64), sizeof(Complex64) }, { 0, 0, 0 } };
/*********************  sqrt  *********************/

static int sqrt_DxD_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64  *tin0 =  (Complex64 *) buffers[0];
    Complex64  *tout0 =  (Complex64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CSQRT(*tin0, *tout0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor sqrt_DxD_vxv_descr =
{ "sqrt_DxD_vxv", (void *) sqrt_DxD_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Complex64), sizeof(Complex64) }, { 0, 0, 0 } };
/*********************  equal  *********************/

static int equal_DDxB_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64  *tin0 =  (Complex64 *) buffers[0];
    Complex64   tin1 = *(Complex64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CEQ(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor equal_DDxB_vsxf_descr =
{ "equal_DDxB_vsxf", (void *) equal_DDxB_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex64), sizeof(Complex64), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int equal_DDxB_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64  *tin0 =  (Complex64 *) buffers[0];
    Complex64  *tin1 =  (Complex64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CEQ(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor equal_DDxB_vvxf_descr =
{ "equal_DDxB_vvxf", (void *) equal_DDxB_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex64), sizeof(Complex64), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int equal_DDxB_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64   tin0 = *(Complex64 *) buffers[0];
    Complex64  *tin1 =  (Complex64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CEQ(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor equal_DDxB_svxf_descr =
{ "equal_DDxB_svxf", (void *) equal_DDxB_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex64), sizeof(Complex64), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  not_equal  *********************/

static int not_equal_DDxB_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64  *tin0 =  (Complex64 *) buffers[0];
    Complex64   tin1 = *(Complex64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CNE(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor not_equal_DDxB_vsxf_descr =
{ "not_equal_DDxB_vsxf", (void *) not_equal_DDxB_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex64), sizeof(Complex64), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int not_equal_DDxB_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64  *tin0 =  (Complex64 *) buffers[0];
    Complex64  *tin1 =  (Complex64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CNE(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor not_equal_DDxB_vvxf_descr =
{ "not_equal_DDxB_vvxf", (void *) not_equal_DDxB_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex64), sizeof(Complex64), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int not_equal_DDxB_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64   tin0 = *(Complex64 *) buffers[0];
    Complex64  *tin1 =  (Complex64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CNE(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor not_equal_DDxB_svxf_descr =
{ "not_equal_DDxB_svxf", (void *) not_equal_DDxB_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex64), sizeof(Complex64), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  greater  *********************/

static int greater_DDxB_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64  *tin0 =  (Complex64 *) buffers[0];
    Complex64   tin1 = *(Complex64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CGT(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_DDxB_vsxf_descr =
{ "greater_DDxB_vsxf", (void *) greater_DDxB_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex64), sizeof(Complex64), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int greater_DDxB_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64  *tin0 =  (Complex64 *) buffers[0];
    Complex64  *tin1 =  (Complex64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CGT(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_DDxB_vvxf_descr =
{ "greater_DDxB_vvxf", (void *) greater_DDxB_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex64), sizeof(Complex64), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int greater_DDxB_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64   tin0 = *(Complex64 *) buffers[0];
    Complex64  *tin1 =  (Complex64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CGT(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_DDxB_svxf_descr =
{ "greater_DDxB_svxf", (void *) greater_DDxB_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex64), sizeof(Complex64), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  greater_equal  *********************/

static int greater_equal_DDxB_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64  *tin0 =  (Complex64 *) buffers[0];
    Complex64   tin1 = *(Complex64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CGE(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_equal_DDxB_vsxf_descr =
{ "greater_equal_DDxB_vsxf", (void *) greater_equal_DDxB_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex64), sizeof(Complex64), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int greater_equal_DDxB_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64  *tin0 =  (Complex64 *) buffers[0];
    Complex64  *tin1 =  (Complex64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CGE(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_equal_DDxB_vvxf_descr =
{ "greater_equal_DDxB_vvxf", (void *) greater_equal_DDxB_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex64), sizeof(Complex64), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int greater_equal_DDxB_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64   tin0 = *(Complex64 *) buffers[0];
    Complex64  *tin1 =  (Complex64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CGE(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_equal_DDxB_svxf_descr =
{ "greater_equal_DDxB_svxf", (void *) greater_equal_DDxB_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex64), sizeof(Complex64), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  less  *********************/

static int less_DDxB_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64  *tin0 =  (Complex64 *) buffers[0];
    Complex64   tin1 = *(Complex64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CLT(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_DDxB_vsxf_descr =
{ "less_DDxB_vsxf", (void *) less_DDxB_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex64), sizeof(Complex64), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int less_DDxB_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64  *tin0 =  (Complex64 *) buffers[0];
    Complex64  *tin1 =  (Complex64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CLT(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_DDxB_vvxf_descr =
{ "less_DDxB_vvxf", (void *) less_DDxB_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex64), sizeof(Complex64), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int less_DDxB_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64   tin0 = *(Complex64 *) buffers[0];
    Complex64  *tin1 =  (Complex64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CLT(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_DDxB_svxf_descr =
{ "less_DDxB_svxf", (void *) less_DDxB_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex64), sizeof(Complex64), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  less_equal  *********************/

static int less_equal_DDxB_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64  *tin0 =  (Complex64 *) buffers[0];
    Complex64   tin1 = *(Complex64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CLE(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_equal_DDxB_vsxf_descr =
{ "less_equal_DDxB_vsxf", (void *) less_equal_DDxB_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex64), sizeof(Complex64), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int less_equal_DDxB_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64  *tin0 =  (Complex64 *) buffers[0];
    Complex64  *tin1 =  (Complex64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CLE(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_equal_DDxB_vvxf_descr =
{ "less_equal_DDxB_vvxf", (void *) less_equal_DDxB_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex64), sizeof(Complex64), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int less_equal_DDxB_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64   tin0 = *(Complex64 *) buffers[0];
    Complex64  *tin1 =  (Complex64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CLE(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_equal_DDxB_svxf_descr =
{ "less_equal_DDxB_svxf", (void *) less_equal_DDxB_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex64), sizeof(Complex64), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  logical_and  *********************/

static int logical_and_DDxB_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64  *tin0 =  (Complex64 *) buffers[0];
    Complex64   tin1 = *(Complex64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CLAND(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_and_DDxB_vsxf_descr =
{ "logical_and_DDxB_vsxf", (void *) logical_and_DDxB_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex64), sizeof(Complex64), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int logical_and_DDxB_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64  *tin0 =  (Complex64 *) buffers[0];
    Complex64  *tin1 =  (Complex64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CLAND(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_and_DDxB_vvxf_descr =
{ "logical_and_DDxB_vvxf", (void *) logical_and_DDxB_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex64), sizeof(Complex64), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int logical_and_DDxB_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64   tin0 = *(Complex64 *) buffers[0];
    Complex64  *tin1 =  (Complex64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CLAND(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_and_DDxB_svxf_descr =
{ "logical_and_DDxB_svxf", (void *) logical_and_DDxB_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex64), sizeof(Complex64), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  logical_or  *********************/

static int logical_or_DDxB_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64  *tin0 =  (Complex64 *) buffers[0];
    Complex64   tin1 = *(Complex64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CLOR(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_or_DDxB_vsxf_descr =
{ "logical_or_DDxB_vsxf", (void *) logical_or_DDxB_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex64), sizeof(Complex64), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int logical_or_DDxB_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64  *tin0 =  (Complex64 *) buffers[0];
    Complex64  *tin1 =  (Complex64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CLOR(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_or_DDxB_vvxf_descr =
{ "logical_or_DDxB_vvxf", (void *) logical_or_DDxB_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex64), sizeof(Complex64), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int logical_or_DDxB_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64   tin0 = *(Complex64 *) buffers[0];
    Complex64  *tin1 =  (Complex64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CLOR(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_or_DDxB_svxf_descr =
{ "logical_or_DDxB_svxf", (void *) logical_or_DDxB_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex64), sizeof(Complex64), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  logical_xor  *********************/

static int logical_xor_DDxB_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64  *tin0 =  (Complex64 *) buffers[0];
    Complex64   tin1 = *(Complex64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CLXOR(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_xor_DDxB_vsxf_descr =
{ "logical_xor_DDxB_vsxf", (void *) logical_xor_DDxB_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex64), sizeof(Complex64), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int logical_xor_DDxB_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64  *tin0 =  (Complex64 *) buffers[0];
    Complex64  *tin1 =  (Complex64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CLXOR(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_xor_DDxB_vvxf_descr =
{ "logical_xor_DDxB_vvxf", (void *) logical_xor_DDxB_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex64), sizeof(Complex64), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int logical_xor_DDxB_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64   tin0 = *(Complex64 *) buffers[0];
    Complex64  *tin1 =  (Complex64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CLXOR(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_xor_DDxB_svxf_descr =
{ "logical_xor_DDxB_svxf", (void *) logical_xor_DDxB_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex64), sizeof(Complex64), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  logical_not  *********************/

static int logical_not_DxB_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64  *tin0 =  (Complex64 *) buffers[0];
    Bool       *tout0 =  (Bool *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CLNOT(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_not_DxB_vxf_descr =
{ "logical_not_DxB_vxf", (void *) logical_not_DxB_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Complex64), sizeof(Bool) }, { 0, 0, 0 } };
/*********************  floor  *********************/

static int floor_DxD_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64  *tin0 =  (Complex64 *) buffers[0];
    Complex64  *tout0 =  (Complex64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CFLOOR(*tin0, *tout0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor floor_DxD_vxv_descr =
{ "floor_DxD_vxv", (void *) floor_DxD_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Complex64), sizeof(Complex64) }, { 0, 0, 0 } };
/*********************  ceil  *********************/

static int ceil_DxD_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64  *tin0 =  (Complex64 *) buffers[0];
    Complex64  *tout0 =  (Complex64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CCEIL(*tin0, *tout0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor ceil_DxD_vxv_descr =
{ "ceil_DxD_vxv", (void *) ceil_DxD_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Complex64), sizeof(Complex64) }, { 0, 0, 0 } };
/*********************  maximum  *********************/

static int maximum_DDxD_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64  *tin0 =  (Complex64 *) buffers[0];
    Complex64   tin1 = *(Complex64 *) buffers[1];
    Complex64  *tout0 =  (Complex64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CMAX(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor maximum_DDxD_vsxf_descr =
{ "maximum_DDxD_vsxf", (void *) maximum_DDxD_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex64), sizeof(Complex64), sizeof(Complex64) }, { 0, 1, 0, 0 } };

static int maximum_DDxD_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64  *tin0 =  (Complex64 *) buffers[0];
    Complex64  *tin1 =  (Complex64 *) buffers[1];
    Complex64  *tout0 =  (Complex64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CMAX(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor maximum_DDxD_vvxf_descr =
{ "maximum_DDxD_vvxf", (void *) maximum_DDxD_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex64), sizeof(Complex64), sizeof(Complex64) }, { 0, 0, 0, 0 } };

static void _maximum_DxD_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Complex64  *tin0   = (Complex64 *) ((char *) input  + inboffset);
    Complex64 *tout0  = (Complex64 *) ((char *) output + outboffset);
    Complex64 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Complex64 *) ((char *) tin0 + inbstrides[dim]);
            net   = NUM_CMAX(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _maximum_DxD_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int maximum_DxD_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _maximum_DxD_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(maximum_DxD_R, CHECK_ALIGN, sizeof(Complex64), sizeof(Complex64));

static void _maximum_DxD_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Complex64 *tin0   = (Complex64 *) ((char *) input  + inboffset);
    Complex64 *tout0 = (Complex64 *) ((char *) output + outboffset);
    Complex64 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Complex64 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Complex64 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = NUM_CMAX(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _maximum_DxD_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  maximum_DxD_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _maximum_DxD_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(maximum_DxD_A, CHECK_ALIGN, sizeof(Complex64), sizeof(Complex64));

static int maximum_DDxD_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64   tin0 = *(Complex64 *) buffers[0];
    Complex64  *tin1 =  (Complex64 *) buffers[1];
    Complex64  *tout0 =  (Complex64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CMAX(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor maximum_DDxD_svxf_descr =
{ "maximum_DDxD_svxf", (void *) maximum_DDxD_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex64), sizeof(Complex64), sizeof(Complex64) }, { 1, 0, 0, 0 } };
/*********************  minimum  *********************/

static int minimum_DDxD_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64  *tin0 =  (Complex64 *) buffers[0];
    Complex64   tin1 = *(Complex64 *) buffers[1];
    Complex64  *tout0 =  (Complex64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CMIN(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor minimum_DDxD_vsxf_descr =
{ "minimum_DDxD_vsxf", (void *) minimum_DDxD_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex64), sizeof(Complex64), sizeof(Complex64) }, { 0, 1, 0, 0 } };

static int minimum_DDxD_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64  *tin0 =  (Complex64 *) buffers[0];
    Complex64  *tin1 =  (Complex64 *) buffers[1];
    Complex64  *tout0 =  (Complex64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CMIN(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor minimum_DDxD_vvxf_descr =
{ "minimum_DDxD_vvxf", (void *) minimum_DDxD_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex64), sizeof(Complex64), sizeof(Complex64) }, { 0, 0, 0, 0 } };

static void _minimum_DxD_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Complex64  *tin0   = (Complex64 *) ((char *) input  + inboffset);
    Complex64 *tout0  = (Complex64 *) ((char *) output + outboffset);
    Complex64 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Complex64 *) ((char *) tin0 + inbstrides[dim]);
            net   = NUM_CMIN(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _minimum_DxD_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int minimum_DxD_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _minimum_DxD_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(minimum_DxD_R, CHECK_ALIGN, sizeof(Complex64), sizeof(Complex64));

static void _minimum_DxD_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Complex64 *tin0   = (Complex64 *) ((char *) input  + inboffset);
    Complex64 *tout0 = (Complex64 *) ((char *) output + outboffset);
    Complex64 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Complex64 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Complex64 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = NUM_CMIN(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _minimum_DxD_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  minimum_DxD_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _minimum_DxD_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(minimum_DxD_A, CHECK_ALIGN, sizeof(Complex64), sizeof(Complex64));

static int minimum_DDxD_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64   tin0 = *(Complex64 *) buffers[0];
    Complex64  *tin1 =  (Complex64 *) buffers[1];
    Complex64  *tout0 =  (Complex64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CMIN(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor minimum_DDxD_svxf_descr =
{ "minimum_DDxD_svxf", (void *) minimum_DDxD_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex64), sizeof(Complex64), sizeof(Complex64) }, { 1, 0, 0, 0 } };
/*********************  fabs  *********************/

static int fabs_DxD_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64  *tin0 =  (Complex64 *) buffers[0];
    Complex64  *tout0 =  (Complex64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CFABS(*tin0, *tout0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor fabs_DxD_vxv_descr =
{ "fabs_DxD_vxv", (void *) fabs_DxD_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Complex64), sizeof(Complex64) }, { 0, 0, 0 } };
/*********************  _round  *********************/

static int _round_DxD_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64  *tin0 =  (Complex64 *) buffers[0];
    Complex64  *tout0 =  (Complex64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CROUND(*tin0, *tout0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor _round_DxD_vxv_descr =
{ "_round_DxD_vxv", (void *) _round_DxD_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Complex64), sizeof(Complex64) }, { 0, 0, 0 } };
/*********************  hypot  *********************/

static int hypot_DDxD_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64  *tin0 =  (Complex64 *) buffers[0];
    Complex64   tin1 = *(Complex64 *) buffers[1];
    Complex64  *tout0 =  (Complex64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CHYPOT(*tin0, tin1, *tout0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor hypot_DDxD_vsxv_descr =
{ "hypot_DDxD_vsxv", (void *) hypot_DDxD_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex64), sizeof(Complex64), sizeof(Complex64) }, { 0, 1, 0, 0 } };

static int hypot_DDxD_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64  *tin0 =  (Complex64 *) buffers[0];
    Complex64  *tin1 =  (Complex64 *) buffers[1];
    Complex64  *tout0 =  (Complex64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CHYPOT(*tin0, *tin1, *tout0);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor hypot_DDxD_vvxv_descr =
{ "hypot_DDxD_vvxv", (void *) hypot_DDxD_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex64), sizeof(Complex64), sizeof(Complex64) }, { 0, 0, 0, 0 } };

static void _hypot_DxD_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Complex64  *tin0   = (Complex64 *) ((char *) input  + inboffset);
    Complex64 *tout0  = (Complex64 *) ((char *) output + outboffset);
    Complex64 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Complex64 *) ((char *) tin0 + inbstrides[dim]);
            NUM_CHYPOT(net, *tin0, net);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _hypot_DxD_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int hypot_DxD_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _hypot_DxD_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(hypot_DxD_R, CHECK_ALIGN, sizeof(Complex64), sizeof(Complex64));

static void _hypot_DxD_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Complex64 *tin0   = (Complex64 *) ((char *) input  + inboffset);
    Complex64 *tout0 = (Complex64 *) ((char *) output + outboffset);
    Complex64 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Complex64 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Complex64 *) ((char *) tout0 + outbstrides[dim]);
            NUM_CHYPOT(lastval ,*tin0, *tout0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _hypot_DxD_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  hypot_DxD_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _hypot_DxD_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(hypot_DxD_A, CHECK_ALIGN, sizeof(Complex64), sizeof(Complex64));

static int hypot_DDxD_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64   tin0 = *(Complex64 *) buffers[0];
    Complex64  *tin1 =  (Complex64 *) buffers[1];
    Complex64  *tout0 =  (Complex64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CHYPOT(tin0, *tin1, *tout0);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor hypot_DDxD_svxv_descr =
{ "hypot_DDxD_svxv", (void *) hypot_DDxD_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex64), sizeof(Complex64), sizeof(Complex64) }, { 1, 0, 0, 0 } };

static PyMethodDef _ufuncComplex64Methods[] = {

	{NULL,      NULL}        /* Sentinel */
};

static PyObject *init_funcDict(void) {
    PyObject *dict, *keytuple, *cfunc;
    dict = PyDict_New();
    /* isnan_DxB_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","isnan","v","Complex64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&isnan_DxB_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minus_DxD_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","minus","v","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minus_DxD_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_DDxD_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","add","vs","Complex64","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_DDxD_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_DDxD_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","add","vv","Complex64","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_DDxD_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_DxD_R */
    keytuple=Py_BuildValue("ss((s)(s))","add","R","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_DxD_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_DxD_A */
    keytuple=Py_BuildValue("ss((s)(s))","add","A","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_DxD_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_DDxD_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","add","sv","Complex64","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_DDxD_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_DDxD_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","subtract","vs","Complex64","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_DDxD_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_DDxD_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","subtract","vv","Complex64","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_DDxD_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_DxD_R */
    keytuple=Py_BuildValue("ss((s)(s))","subtract","R","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_DxD_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_DxD_A */
    keytuple=Py_BuildValue("ss((s)(s))","subtract","A","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_DxD_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_DDxD_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","subtract","sv","Complex64","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_DDxD_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* multiply_DDxD_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","multiply","vs","Complex64","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&multiply_DDxD_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* multiply_DDxD_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","multiply","vv","Complex64","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&multiply_DDxD_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* multiply_DxD_R */
    keytuple=Py_BuildValue("ss((s)(s))","multiply","R","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&multiply_DxD_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* multiply_DxD_A */
    keytuple=Py_BuildValue("ss((s)(s))","multiply","A","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&multiply_DxD_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* multiply_DDxD_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","multiply","sv","Complex64","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&multiply_DDxD_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_DDxD_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","divide","vs","Complex64","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_DDxD_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_DDxD_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","divide","vv","Complex64","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_DDxD_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_DxD_R */
    keytuple=Py_BuildValue("ss((s)(s))","divide","R","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_DxD_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_DxD_A */
    keytuple=Py_BuildValue("ss((s)(s))","divide","A","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_DxD_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_DDxD_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","divide","sv","Complex64","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_DDxD_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_DDxD_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","true_divide","vs","Complex64","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_DDxD_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_DDxD_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","true_divide","vv","Complex64","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_DDxD_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_DxD_R */
    keytuple=Py_BuildValue("ss((s)(s))","true_divide","R","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_DxD_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_DxD_A */
    keytuple=Py_BuildValue("ss((s)(s))","true_divide","A","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_DxD_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_DDxD_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","true_divide","sv","Complex64","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_DDxD_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_DDxD_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","remainder","vs","Complex64","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_DDxD_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_DDxD_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","remainder","vv","Complex64","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_DDxD_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_DxD_R */
    keytuple=Py_BuildValue("ss((s)(s))","remainder","R","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_DxD_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_DxD_A */
    keytuple=Py_BuildValue("ss((s)(s))","remainder","A","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_DxD_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_DDxD_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","remainder","sv","Complex64","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_DDxD_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_DDxD_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","power","vs","Complex64","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_DDxD_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_DDxD_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","power","vv","Complex64","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_DDxD_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_DxD_R */
    keytuple=Py_BuildValue("ss((s)(s))","power","R","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_DxD_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_DxD_A */
    keytuple=Py_BuildValue("ss((s)(s))","power","A","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_DxD_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_DDxD_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","power","sv","Complex64","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_DDxD_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* abs_Dxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","abs","v","Complex64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&abs_Dxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* sin_DxD_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","sin","v","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&sin_DxD_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* cos_DxD_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","cos","v","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&cos_DxD_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* tan_DxD_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","tan","v","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&tan_DxD_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arcsin_DxD_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","arcsin","v","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arcsin_DxD_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arccos_DxD_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","arccos","v","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arccos_DxD_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan_DxD_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","arctan","v","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan_DxD_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arcsinh_DxD_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","arcsinh","v","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arcsinh_DxD_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arccosh_DxD_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","arccosh","v","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arccosh_DxD_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctanh_DxD_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","arctanh","v","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctanh_DxD_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* log_DxD_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","log","v","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&log_DxD_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* log10_DxD_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","log10","v","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&log10_DxD_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* exp_DxD_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","exp","v","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&exp_DxD_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* sinh_DxD_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","sinh","v","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&sinh_DxD_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* cosh_DxD_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","cosh","v","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&cosh_DxD_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* tanh_DxD_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","tanh","v","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&tanh_DxD_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* sqrt_DxD_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","sqrt","v","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&sqrt_DxD_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* equal_DDxB_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","equal","vs","Complex64","Complex64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&equal_DDxB_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* equal_DDxB_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","equal","vv","Complex64","Complex64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&equal_DDxB_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* equal_DDxB_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","equal","sv","Complex64","Complex64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&equal_DDxB_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* not_equal_DDxB_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","not_equal","vs","Complex64","Complex64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&not_equal_DDxB_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* not_equal_DDxB_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","not_equal","vv","Complex64","Complex64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&not_equal_DDxB_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* not_equal_DDxB_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","not_equal","sv","Complex64","Complex64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&not_equal_DDxB_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_DDxB_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","greater","vs","Complex64","Complex64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_DDxB_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_DDxB_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","greater","vv","Complex64","Complex64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_DDxB_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_DDxB_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","greater","sv","Complex64","Complex64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_DDxB_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_equal_DDxB_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","greater_equal","vs","Complex64","Complex64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_equal_DDxB_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_equal_DDxB_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","greater_equal","vv","Complex64","Complex64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_equal_DDxB_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_equal_DDxB_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","greater_equal","sv","Complex64","Complex64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_equal_DDxB_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_DDxB_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","less","vs","Complex64","Complex64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_DDxB_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_DDxB_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","less","vv","Complex64","Complex64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_DDxB_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_DDxB_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","less","sv","Complex64","Complex64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_DDxB_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_equal_DDxB_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","less_equal","vs","Complex64","Complex64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_equal_DDxB_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_equal_DDxB_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","less_equal","vv","Complex64","Complex64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_equal_DDxB_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_equal_DDxB_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","less_equal","sv","Complex64","Complex64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_equal_DDxB_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_and_DDxB_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_and","vs","Complex64","Complex64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_and_DDxB_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_and_DDxB_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_and","vv","Complex64","Complex64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_and_DDxB_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_and_DDxB_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_and","sv","Complex64","Complex64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_and_DDxB_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_or_DDxB_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_or","vs","Complex64","Complex64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_or_DDxB_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_or_DDxB_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_or","vv","Complex64","Complex64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_or_DDxB_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_or_DDxB_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_or","sv","Complex64","Complex64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_or_DDxB_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_xor_DDxB_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_xor","vs","Complex64","Complex64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_xor_DDxB_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_xor_DDxB_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_xor","vv","Complex64","Complex64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_xor_DDxB_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_xor_DDxB_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_xor","sv","Complex64","Complex64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_xor_DDxB_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_not_DxB_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","logical_not","v","Complex64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_not_DxB_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_DxD_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","floor","v","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_DxD_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* ceil_DxD_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","ceil","v","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&ceil_DxD_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_DDxD_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","maximum","vs","Complex64","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_DDxD_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_DDxD_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","maximum","vv","Complex64","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_DDxD_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_DxD_R */
    keytuple=Py_BuildValue("ss((s)(s))","maximum","R","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_DxD_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_DxD_A */
    keytuple=Py_BuildValue("ss((s)(s))","maximum","A","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_DxD_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_DDxD_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","maximum","sv","Complex64","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_DDxD_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_DDxD_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","minimum","vs","Complex64","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_DDxD_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_DDxD_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","minimum","vv","Complex64","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_DDxD_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_DxD_R */
    keytuple=Py_BuildValue("ss((s)(s))","minimum","R","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_DxD_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_DxD_A */
    keytuple=Py_BuildValue("ss((s)(s))","minimum","A","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_DxD_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_DDxD_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","minimum","sv","Complex64","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_DDxD_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* fabs_DxD_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","fabs","v","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&fabs_DxD_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* _round_DxD_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","_round","v","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&_round_DxD_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_DDxD_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","hypot","vs","Complex64","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_DDxD_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_DDxD_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","hypot","vv","Complex64","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_DDxD_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_DxD_R */
    keytuple=Py_BuildValue("ss((s)(s))","hypot","R","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_DxD_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_DxD_A */
    keytuple=Py_BuildValue("ss((s)(s))","hypot","A","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_DxD_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_DDxD_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","hypot","sv","Complex64","Complex64","Complex64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_DDxD_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    return dict;
}

/* platform independent*/
#ifdef MS_WIN32
__declspec(dllexport)
#endif
void init_ufuncComplex64(void) {
    PyObject *m, *d, *functions;
    m = Py_InitModule("_ufuncComplex64", _ufuncComplex64Methods);
    d = PyModule_GetDict(m);
    import_libnumarray();
    functions = init_funcDict();
    PyDict_SetItemString(d, "functionDict", functions);
    Py_DECREF(functions);
    ADD_VERSION(m);
}
