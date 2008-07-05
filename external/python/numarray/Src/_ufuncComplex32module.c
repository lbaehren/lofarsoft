
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

static int isnan_FxB_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32  *tin0 =  (Complex32 *) buffers[0];
    Bool       *tout0 =  (Bool *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NA_isnanC32(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor isnan_FxB_vxf_descr =
{ "isnan_FxB_vxf", (void *) isnan_FxB_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Complex32), sizeof(Bool) }, { 0, 0, 0 } };
/*********************  isnan  *********************/
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

static int minus_FxF_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32  *tin0 =  (Complex32 *) buffers[0];
    Complex32  *tout0 =  (Complex32 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CMINUS(*tin0, *tout0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor minus_FxF_vxv_descr =
{ "minus_FxF_vxv", (void *) minus_FxF_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Complex32), sizeof(Complex32) }, { 0, 0, 0 } };
/*********************  add  *********************/

static int add_FFxF_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32  *tin0 =  (Complex32 *) buffers[0];
    Complex32   tin1 = *(Complex32 *) buffers[1];
    Complex32  *tout0 =  (Complex32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CADD(*tin0, tin1, *tout0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor add_FFxF_vsxv_descr =
{ "add_FFxF_vsxv", (void *) add_FFxF_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex32), sizeof(Complex32), sizeof(Complex32) }, { 0, 1, 0, 0 } };

static int add_FFxF_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32  *tin0 =  (Complex32 *) buffers[0];
    Complex32  *tin1 =  (Complex32 *) buffers[1];
    Complex32  *tout0 =  (Complex32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CADD(*tin0, *tin1, *tout0);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor add_FFxF_vvxv_descr =
{ "add_FFxF_vvxv", (void *) add_FFxF_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex32), sizeof(Complex32), sizeof(Complex32) }, { 0, 0, 0, 0 } };

static void _add_FxF_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Complex32  *tin0   = (Complex32 *) ((char *) input  + inboffset);
    Complex32 *tout0  = (Complex32 *) ((char *) output + outboffset);
    Complex32 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Complex32 *) ((char *) tin0 + inbstrides[dim]);
            NUM_CADD(net, *tin0, net);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _add_FxF_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int add_FxF_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _add_FxF_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(add_FxF_R, CHECK_ALIGN, sizeof(Complex32), sizeof(Complex32));

static void _add_FxF_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Complex32 *tin0   = (Complex32 *) ((char *) input  + inboffset);
    Complex32 *tout0 = (Complex32 *) ((char *) output + outboffset);
    Complex32 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Complex32 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Complex32 *) ((char *) tout0 + outbstrides[dim]);
            NUM_CADD(lastval ,*tin0, *tout0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _add_FxF_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  add_FxF_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _add_FxF_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(add_FxF_A, CHECK_ALIGN, sizeof(Complex32), sizeof(Complex32));

static int add_FFxF_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32   tin0 = *(Complex32 *) buffers[0];
    Complex32  *tin1 =  (Complex32 *) buffers[1];
    Complex32  *tout0 =  (Complex32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CADD(tin0, *tin1, *tout0);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor add_FFxF_svxv_descr =
{ "add_FFxF_svxv", (void *) add_FFxF_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex32), sizeof(Complex32), sizeof(Complex32) }, { 1, 0, 0, 0 } };
/*********************  subtract  *********************/

static int subtract_FFxF_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32  *tin0 =  (Complex32 *) buffers[0];
    Complex32   tin1 = *(Complex32 *) buffers[1];
    Complex32  *tout0 =  (Complex32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CSUB(*tin0, tin1, *tout0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor subtract_FFxF_vsxv_descr =
{ "subtract_FFxF_vsxv", (void *) subtract_FFxF_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex32), sizeof(Complex32), sizeof(Complex32) }, { 0, 1, 0, 0 } };

static int subtract_FFxF_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32  *tin0 =  (Complex32 *) buffers[0];
    Complex32  *tin1 =  (Complex32 *) buffers[1];
    Complex32  *tout0 =  (Complex32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CSUB(*tin0, *tin1, *tout0);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor subtract_FFxF_vvxv_descr =
{ "subtract_FFxF_vvxv", (void *) subtract_FFxF_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex32), sizeof(Complex32), sizeof(Complex32) }, { 0, 0, 0, 0 } };

static void _subtract_FxF_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Complex32  *tin0   = (Complex32 *) ((char *) input  + inboffset);
    Complex32 *tout0  = (Complex32 *) ((char *) output + outboffset);
    Complex32 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Complex32 *) ((char *) tin0 + inbstrides[dim]);
            NUM_CSUB(net, *tin0, net);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _subtract_FxF_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int subtract_FxF_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _subtract_FxF_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(subtract_FxF_R, CHECK_ALIGN, sizeof(Complex32), sizeof(Complex32));

static void _subtract_FxF_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Complex32 *tin0   = (Complex32 *) ((char *) input  + inboffset);
    Complex32 *tout0 = (Complex32 *) ((char *) output + outboffset);
    Complex32 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Complex32 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Complex32 *) ((char *) tout0 + outbstrides[dim]);
            NUM_CSUB(lastval ,*tin0, *tout0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _subtract_FxF_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  subtract_FxF_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _subtract_FxF_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(subtract_FxF_A, CHECK_ALIGN, sizeof(Complex32), sizeof(Complex32));

static int subtract_FFxF_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32   tin0 = *(Complex32 *) buffers[0];
    Complex32  *tin1 =  (Complex32 *) buffers[1];
    Complex32  *tout0 =  (Complex32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CSUB(tin0, *tin1, *tout0);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor subtract_FFxF_svxv_descr =
{ "subtract_FFxF_svxv", (void *) subtract_FFxF_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex32), sizeof(Complex32), sizeof(Complex32) }, { 1, 0, 0, 0 } };
/*********************  multiply  *********************/

static int multiply_FFxF_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32  *tin0 =  (Complex32 *) buffers[0];
    Complex32   tin1 = *(Complex32 *) buffers[1];
    Complex32  *tout0 =  (Complex32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CMUL(*tin0, tin1, *tout0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor multiply_FFxF_vsxv_descr =
{ "multiply_FFxF_vsxv", (void *) multiply_FFxF_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex32), sizeof(Complex32), sizeof(Complex32) }, { 0, 1, 0, 0 } };

static int multiply_FFxF_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32  *tin0 =  (Complex32 *) buffers[0];
    Complex32  *tin1 =  (Complex32 *) buffers[1];
    Complex32  *tout0 =  (Complex32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CMUL(*tin0, *tin1, *tout0);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor multiply_FFxF_vvxv_descr =
{ "multiply_FFxF_vvxv", (void *) multiply_FFxF_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex32), sizeof(Complex32), sizeof(Complex32) }, { 0, 0, 0, 0 } };

static void _multiply_FxF_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Complex32  *tin0   = (Complex32 *) ((char *) input  + inboffset);
    Complex32 *tout0  = (Complex32 *) ((char *) output + outboffset);
    Complex32 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Complex32 *) ((char *) tin0 + inbstrides[dim]);
            NUM_CMUL(net, *tin0, net);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _multiply_FxF_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int multiply_FxF_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _multiply_FxF_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(multiply_FxF_R, CHECK_ALIGN, sizeof(Complex32), sizeof(Complex32));

static void _multiply_FxF_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Complex32 *tin0   = (Complex32 *) ((char *) input  + inboffset);
    Complex32 *tout0 = (Complex32 *) ((char *) output + outboffset);
    Complex32 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Complex32 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Complex32 *) ((char *) tout0 + outbstrides[dim]);
            NUM_CMUL(lastval ,*tin0, *tout0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _multiply_FxF_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  multiply_FxF_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _multiply_FxF_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(multiply_FxF_A, CHECK_ALIGN, sizeof(Complex32), sizeof(Complex32));

static int multiply_FFxF_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32   tin0 = *(Complex32 *) buffers[0];
    Complex32  *tin1 =  (Complex32 *) buffers[1];
    Complex32  *tout0 =  (Complex32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CMUL(tin0, *tin1, *tout0);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor multiply_FFxF_svxv_descr =
{ "multiply_FFxF_svxv", (void *) multiply_FFxF_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex32), sizeof(Complex32), sizeof(Complex32) }, { 1, 0, 0, 0 } };
/*********************  divide  *********************/

static int divide_FFxF_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32  *tin0 =  (Complex32 *) buffers[0];
    Complex32   tin1 = *(Complex32 *) buffers[1];
    Complex32  *tout0 =  (Complex32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CDIV(*tin0, tin1, *tout0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor divide_FFxF_vsxv_descr =
{ "divide_FFxF_vsxv", (void *) divide_FFxF_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex32), sizeof(Complex32), sizeof(Complex32) }, { 0, 1, 0, 0 } };

static int divide_FFxF_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32  *tin0 =  (Complex32 *) buffers[0];
    Complex32  *tin1 =  (Complex32 *) buffers[1];
    Complex32  *tout0 =  (Complex32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CDIV(*tin0, *tin1, *tout0);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor divide_FFxF_vvxv_descr =
{ "divide_FFxF_vvxv", (void *) divide_FFxF_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex32), sizeof(Complex32), sizeof(Complex32) }, { 0, 0, 0, 0 } };

static void _divide_FxF_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Complex32  *tin0   = (Complex32 *) ((char *) input  + inboffset);
    Complex32 *tout0  = (Complex32 *) ((char *) output + outboffset);
    Complex32 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Complex32 *) ((char *) tin0 + inbstrides[dim]);
            NUM_CDIV(net, *tin0, net);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _divide_FxF_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int divide_FxF_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _divide_FxF_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(divide_FxF_R, CHECK_ALIGN, sizeof(Complex32), sizeof(Complex32));

static void _divide_FxF_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Complex32 *tin0   = (Complex32 *) ((char *) input  + inboffset);
    Complex32 *tout0 = (Complex32 *) ((char *) output + outboffset);
    Complex32 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Complex32 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Complex32 *) ((char *) tout0 + outbstrides[dim]);
            NUM_CDIV(lastval ,*tin0, *tout0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _divide_FxF_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  divide_FxF_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _divide_FxF_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(divide_FxF_A, CHECK_ALIGN, sizeof(Complex32), sizeof(Complex32));

static int divide_FFxF_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32   tin0 = *(Complex32 *) buffers[0];
    Complex32  *tin1 =  (Complex32 *) buffers[1];
    Complex32  *tout0 =  (Complex32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CDIV(tin0, *tin1, *tout0);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor divide_FFxF_svxv_descr =
{ "divide_FFxF_svxv", (void *) divide_FFxF_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex32), sizeof(Complex32), sizeof(Complex32) }, { 1, 0, 0, 0 } };
/*********************  true_divide  *********************/

static int true_divide_FFxF_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32  *tin0 =  (Complex32 *) buffers[0];
    Complex32   tin1 = *(Complex32 *) buffers[1];
    Complex32  *tout0 =  (Complex32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CDIV(*tin0, tin1, *tout0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor true_divide_FFxF_vsxv_descr =
{ "true_divide_FFxF_vsxv", (void *) true_divide_FFxF_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex32), sizeof(Complex32), sizeof(Complex32) }, { 0, 1, 0, 0 } };

static int true_divide_FFxF_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32  *tin0 =  (Complex32 *) buffers[0];
    Complex32  *tin1 =  (Complex32 *) buffers[1];
    Complex32  *tout0 =  (Complex32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CDIV(*tin0, *tin1, *tout0);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor true_divide_FFxF_vvxv_descr =
{ "true_divide_FFxF_vvxv", (void *) true_divide_FFxF_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex32), sizeof(Complex32), sizeof(Complex32) }, { 0, 0, 0, 0 } };

static void _true_divide_FxF_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Complex32  *tin0   = (Complex32 *) ((char *) input  + inboffset);
    Complex32 *tout0  = (Complex32 *) ((char *) output + outboffset);
    Complex32 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Complex32 *) ((char *) tin0 + inbstrides[dim]);
            NUM_CDIV(net, *tin0, net);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _true_divide_FxF_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int true_divide_FxF_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _true_divide_FxF_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(true_divide_FxF_R, CHECK_ALIGN, sizeof(Complex32), sizeof(Complex32));

static void _true_divide_FxF_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Complex32 *tin0   = (Complex32 *) ((char *) input  + inboffset);
    Complex32 *tout0 = (Complex32 *) ((char *) output + outboffset);
    Complex32 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Complex32 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Complex32 *) ((char *) tout0 + outbstrides[dim]);
            NUM_CDIV(lastval ,*tin0, *tout0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _true_divide_FxF_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  true_divide_FxF_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _true_divide_FxF_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(true_divide_FxF_A, CHECK_ALIGN, sizeof(Complex32), sizeof(Complex32));

static int true_divide_FFxF_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32   tin0 = *(Complex32 *) buffers[0];
    Complex32  *tin1 =  (Complex32 *) buffers[1];
    Complex32  *tout0 =  (Complex32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CDIV(tin0, *tin1, *tout0);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor true_divide_FFxF_svxv_descr =
{ "true_divide_FFxF_svxv", (void *) true_divide_FFxF_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex32), sizeof(Complex32), sizeof(Complex32) }, { 1, 0, 0, 0 } };
/*********************  remainder  *********************/

static int remainder_FFxF_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32  *tin0 =  (Complex32 *) buffers[0];
    Complex32   tin1 = *(Complex32 *) buffers[1];
    Complex32  *tout0 =  (Complex32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CREM(*tin0, tin1, *tout0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor remainder_FFxF_vsxv_descr =
{ "remainder_FFxF_vsxv", (void *) remainder_FFxF_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex32), sizeof(Complex32), sizeof(Complex32) }, { 0, 1, 0, 0 } };

static int remainder_FFxF_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32  *tin0 =  (Complex32 *) buffers[0];
    Complex32  *tin1 =  (Complex32 *) buffers[1];
    Complex32  *tout0 =  (Complex32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CREM(*tin0, *tin1, *tout0);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor remainder_FFxF_vvxv_descr =
{ "remainder_FFxF_vvxv", (void *) remainder_FFxF_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex32), sizeof(Complex32), sizeof(Complex32) }, { 0, 0, 0, 0 } };

static void _remainder_FxF_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Complex32  *tin0   = (Complex32 *) ((char *) input  + inboffset);
    Complex32 *tout0  = (Complex32 *) ((char *) output + outboffset);
    Complex32 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Complex32 *) ((char *) tin0 + inbstrides[dim]);
            NUM_CREM(net, *tin0, net);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _remainder_FxF_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int remainder_FxF_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _remainder_FxF_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(remainder_FxF_R, CHECK_ALIGN, sizeof(Complex32), sizeof(Complex32));

static void _remainder_FxF_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Complex32 *tin0   = (Complex32 *) ((char *) input  + inboffset);
    Complex32 *tout0 = (Complex32 *) ((char *) output + outboffset);
    Complex32 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Complex32 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Complex32 *) ((char *) tout0 + outbstrides[dim]);
            NUM_CREM(lastval ,*tin0, *tout0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _remainder_FxF_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  remainder_FxF_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _remainder_FxF_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(remainder_FxF_A, CHECK_ALIGN, sizeof(Complex32), sizeof(Complex32));

static int remainder_FFxF_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32   tin0 = *(Complex32 *) buffers[0];
    Complex32  *tin1 =  (Complex32 *) buffers[1];
    Complex32  *tout0 =  (Complex32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CREM(tin0, *tin1, *tout0);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor remainder_FFxF_svxv_descr =
{ "remainder_FFxF_svxv", (void *) remainder_FFxF_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex32), sizeof(Complex32), sizeof(Complex32) }, { 1, 0, 0, 0 } };
/*********************  power  *********************/

static int power_FFxF_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32  *tin0 =  (Complex32 *) buffers[0];
    Complex32   tin1 = *(Complex32 *) buffers[1];
    Complex32  *tout0 =  (Complex32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CPOW(*tin0, tin1, *tout0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor power_FFxF_vsxv_descr =
{ "power_FFxF_vsxv", (void *) power_FFxF_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex32), sizeof(Complex32), sizeof(Complex32) }, { 0, 1, 0, 0 } };

static int power_FFxF_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32  *tin0 =  (Complex32 *) buffers[0];
    Complex32  *tin1 =  (Complex32 *) buffers[1];
    Complex32  *tout0 =  (Complex32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CPOW(*tin0, *tin1, *tout0);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor power_FFxF_vvxv_descr =
{ "power_FFxF_vvxv", (void *) power_FFxF_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex32), sizeof(Complex32), sizeof(Complex32) }, { 0, 0, 0, 0 } };

static void _power_FxF_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Complex32  *tin0   = (Complex32 *) ((char *) input  + inboffset);
    Complex32 *tout0  = (Complex32 *) ((char *) output + outboffset);
    Complex32 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Complex32 *) ((char *) tin0 + inbstrides[dim]);
            NUM_CPOW(net, *tin0, net);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _power_FxF_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int power_FxF_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _power_FxF_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(power_FxF_R, CHECK_ALIGN, sizeof(Complex32), sizeof(Complex32));

static void _power_FxF_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Complex32 *tin0   = (Complex32 *) ((char *) input  + inboffset);
    Complex32 *tout0 = (Complex32 *) ((char *) output + outboffset);
    Complex32 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Complex32 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Complex32 *) ((char *) tout0 + outbstrides[dim]);
            NUM_CPOW(lastval ,*tin0, *tout0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _power_FxF_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  power_FxF_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _power_FxF_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(power_FxF_A, CHECK_ALIGN, sizeof(Complex32), sizeof(Complex32));

static int power_FFxF_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32   tin0 = *(Complex32 *) buffers[0];
    Complex32  *tin1 =  (Complex32 *) buffers[1];
    Complex32  *tout0 =  (Complex32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CPOW(tin0, *tin1, *tout0);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor power_FFxF_svxv_descr =
{ "power_FFxF_svxv", (void *) power_FFxF_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex32), sizeof(Complex32), sizeof(Complex32) }, { 1, 0, 0, 0 } };
/*********************  abs  *********************/

static int abs_Fxf_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32  *tin0 =  (Complex32 *) buffers[0];
    Float32    *tout0 =  (Float32 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CABS(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor abs_Fxf_vxf_descr =
{ "abs_Fxf_vxf", (void *) abs_Fxf_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Complex32), sizeof(Float32) }, { 0, 0, 0 } };
/*********************  sin  *********************/

static int sin_FxF_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32  *tin0 =  (Complex32 *) buffers[0];
    Complex32  *tout0 =  (Complex32 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CSIN(*tin0, *tout0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor sin_FxF_vxv_descr =
{ "sin_FxF_vxv", (void *) sin_FxF_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Complex32), sizeof(Complex32) }, { 0, 0, 0 } };
/*********************  cos  *********************/

static int cos_FxF_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32  *tin0 =  (Complex32 *) buffers[0];
    Complex32  *tout0 =  (Complex32 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CCOS(*tin0, *tout0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor cos_FxF_vxv_descr =
{ "cos_FxF_vxv", (void *) cos_FxF_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Complex32), sizeof(Complex32) }, { 0, 0, 0 } };
/*********************  tan  *********************/

static int tan_FxF_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32  *tin0 =  (Complex32 *) buffers[0];
    Complex32  *tout0 =  (Complex32 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CTAN(*tin0, *tout0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor tan_FxF_vxv_descr =
{ "tan_FxF_vxv", (void *) tan_FxF_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Complex32), sizeof(Complex32) }, { 0, 0, 0 } };
/*********************  arcsin  *********************/

static int arcsin_FxF_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32  *tin0 =  (Complex32 *) buffers[0];
    Complex32  *tout0 =  (Complex32 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CASIN(*tin0, *tout0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arcsin_FxF_vxv_descr =
{ "arcsin_FxF_vxv", (void *) arcsin_FxF_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Complex32), sizeof(Complex32) }, { 0, 0, 0 } };
/*********************  arccos  *********************/

static int arccos_FxF_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32  *tin0 =  (Complex32 *) buffers[0];
    Complex32  *tout0 =  (Complex32 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CACOS(*tin0, *tout0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arccos_FxF_vxv_descr =
{ "arccos_FxF_vxv", (void *) arccos_FxF_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Complex32), sizeof(Complex32) }, { 0, 0, 0 } };
/*********************  arctan  *********************/

static int arctan_FxF_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32  *tin0 =  (Complex32 *) buffers[0];
    Complex32  *tout0 =  (Complex32 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CATAN(*tin0, *tout0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arctan_FxF_vxv_descr =
{ "arctan_FxF_vxv", (void *) arctan_FxF_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Complex32), sizeof(Complex32) }, { 0, 0, 0 } };
/*********************  arcsinh  *********************/

static int arcsinh_FxF_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32  *tin0 =  (Complex32 *) buffers[0];
    Complex32  *tout0 =  (Complex32 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CASINH(*tin0, *tout0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arcsinh_FxF_vxv_descr =
{ "arcsinh_FxF_vxv", (void *) arcsinh_FxF_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Complex32), sizeof(Complex32) }, { 0, 0, 0 } };
/*********************  arccosh  *********************/

static int arccosh_FxF_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32  *tin0 =  (Complex32 *) buffers[0];
    Complex32  *tout0 =  (Complex32 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CACOSH(*tin0, *tout0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arccosh_FxF_vxv_descr =
{ "arccosh_FxF_vxv", (void *) arccosh_FxF_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Complex32), sizeof(Complex32) }, { 0, 0, 0 } };
/*********************  arctanh  *********************/

static int arctanh_FxF_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32  *tin0 =  (Complex32 *) buffers[0];
    Complex32  *tout0 =  (Complex32 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CATANH(*tin0, *tout0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arctanh_FxF_vxv_descr =
{ "arctanh_FxF_vxv", (void *) arctanh_FxF_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Complex32), sizeof(Complex32) }, { 0, 0, 0 } };
/*********************  log  *********************/

static int log_FxF_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32  *tin0 =  (Complex32 *) buffers[0];
    Complex32  *tout0 =  (Complex32 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CLOG(*tin0, *tout0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor log_FxF_vxv_descr =
{ "log_FxF_vxv", (void *) log_FxF_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Complex32), sizeof(Complex32) }, { 0, 0, 0 } };
/*********************  log10  *********************/

static int log10_FxF_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32  *tin0 =  (Complex32 *) buffers[0];
    Complex32  *tout0 =  (Complex32 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CLOG10(*tin0, *tout0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor log10_FxF_vxv_descr =
{ "log10_FxF_vxv", (void *) log10_FxF_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Complex32), sizeof(Complex32) }, { 0, 0, 0 } };
/*********************  exp  *********************/

static int exp_FxF_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32  *tin0 =  (Complex32 *) buffers[0];
    Complex32  *tout0 =  (Complex32 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CEXP(*tin0, *tout0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor exp_FxF_vxv_descr =
{ "exp_FxF_vxv", (void *) exp_FxF_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Complex32), sizeof(Complex32) }, { 0, 0, 0 } };
/*********************  sinh  *********************/

static int sinh_FxF_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32  *tin0 =  (Complex32 *) buffers[0];
    Complex32  *tout0 =  (Complex32 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CSINH(*tin0, *tout0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor sinh_FxF_vxv_descr =
{ "sinh_FxF_vxv", (void *) sinh_FxF_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Complex32), sizeof(Complex32) }, { 0, 0, 0 } };
/*********************  cosh  *********************/

static int cosh_FxF_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32  *tin0 =  (Complex32 *) buffers[0];
    Complex32  *tout0 =  (Complex32 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CCOSH(*tin0, *tout0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor cosh_FxF_vxv_descr =
{ "cosh_FxF_vxv", (void *) cosh_FxF_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Complex32), sizeof(Complex32) }, { 0, 0, 0 } };
/*********************  tanh  *********************/

static int tanh_FxF_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32  *tin0 =  (Complex32 *) buffers[0];
    Complex32  *tout0 =  (Complex32 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CTANH(*tin0, *tout0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor tanh_FxF_vxv_descr =
{ "tanh_FxF_vxv", (void *) tanh_FxF_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Complex32), sizeof(Complex32) }, { 0, 0, 0 } };
/*********************  sqrt  *********************/

static int sqrt_FxF_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32  *tin0 =  (Complex32 *) buffers[0];
    Complex32  *tout0 =  (Complex32 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CSQRT(*tin0, *tout0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor sqrt_FxF_vxv_descr =
{ "sqrt_FxF_vxv", (void *) sqrt_FxF_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Complex32), sizeof(Complex32) }, { 0, 0, 0 } };
/*********************  equal  *********************/

static int equal_FFxB_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32  *tin0 =  (Complex32 *) buffers[0];
    Complex32   tin1 = *(Complex32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CEQ(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor equal_FFxB_vsxf_descr =
{ "equal_FFxB_vsxf", (void *) equal_FFxB_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex32), sizeof(Complex32), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int equal_FFxB_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32  *tin0 =  (Complex32 *) buffers[0];
    Complex32  *tin1 =  (Complex32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CEQ(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor equal_FFxB_vvxf_descr =
{ "equal_FFxB_vvxf", (void *) equal_FFxB_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex32), sizeof(Complex32), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int equal_FFxB_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32   tin0 = *(Complex32 *) buffers[0];
    Complex32  *tin1 =  (Complex32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CEQ(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor equal_FFxB_svxf_descr =
{ "equal_FFxB_svxf", (void *) equal_FFxB_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex32), sizeof(Complex32), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  not_equal  *********************/

static int not_equal_FFxB_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32  *tin0 =  (Complex32 *) buffers[0];
    Complex32   tin1 = *(Complex32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CNE(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor not_equal_FFxB_vsxf_descr =
{ "not_equal_FFxB_vsxf", (void *) not_equal_FFxB_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex32), sizeof(Complex32), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int not_equal_FFxB_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32  *tin0 =  (Complex32 *) buffers[0];
    Complex32  *tin1 =  (Complex32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CNE(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor not_equal_FFxB_vvxf_descr =
{ "not_equal_FFxB_vvxf", (void *) not_equal_FFxB_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex32), sizeof(Complex32), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int not_equal_FFxB_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32   tin0 = *(Complex32 *) buffers[0];
    Complex32  *tin1 =  (Complex32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CNE(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor not_equal_FFxB_svxf_descr =
{ "not_equal_FFxB_svxf", (void *) not_equal_FFxB_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex32), sizeof(Complex32), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  greater  *********************/

static int greater_FFxB_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32  *tin0 =  (Complex32 *) buffers[0];
    Complex32   tin1 = *(Complex32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CGT(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_FFxB_vsxf_descr =
{ "greater_FFxB_vsxf", (void *) greater_FFxB_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex32), sizeof(Complex32), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int greater_FFxB_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32  *tin0 =  (Complex32 *) buffers[0];
    Complex32  *tin1 =  (Complex32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CGT(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_FFxB_vvxf_descr =
{ "greater_FFxB_vvxf", (void *) greater_FFxB_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex32), sizeof(Complex32), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int greater_FFxB_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32   tin0 = *(Complex32 *) buffers[0];
    Complex32  *tin1 =  (Complex32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CGT(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_FFxB_svxf_descr =
{ "greater_FFxB_svxf", (void *) greater_FFxB_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex32), sizeof(Complex32), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  greater_equal  *********************/

static int greater_equal_FFxB_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32  *tin0 =  (Complex32 *) buffers[0];
    Complex32   tin1 = *(Complex32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CGE(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_equal_FFxB_vsxf_descr =
{ "greater_equal_FFxB_vsxf", (void *) greater_equal_FFxB_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex32), sizeof(Complex32), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int greater_equal_FFxB_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32  *tin0 =  (Complex32 *) buffers[0];
    Complex32  *tin1 =  (Complex32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CGE(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_equal_FFxB_vvxf_descr =
{ "greater_equal_FFxB_vvxf", (void *) greater_equal_FFxB_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex32), sizeof(Complex32), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int greater_equal_FFxB_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32   tin0 = *(Complex32 *) buffers[0];
    Complex32  *tin1 =  (Complex32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CGE(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_equal_FFxB_svxf_descr =
{ "greater_equal_FFxB_svxf", (void *) greater_equal_FFxB_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex32), sizeof(Complex32), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  less  *********************/

static int less_FFxB_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32  *tin0 =  (Complex32 *) buffers[0];
    Complex32   tin1 = *(Complex32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CLT(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_FFxB_vsxf_descr =
{ "less_FFxB_vsxf", (void *) less_FFxB_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex32), sizeof(Complex32), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int less_FFxB_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32  *tin0 =  (Complex32 *) buffers[0];
    Complex32  *tin1 =  (Complex32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CLT(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_FFxB_vvxf_descr =
{ "less_FFxB_vvxf", (void *) less_FFxB_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex32), sizeof(Complex32), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int less_FFxB_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32   tin0 = *(Complex32 *) buffers[0];
    Complex32  *tin1 =  (Complex32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CLT(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_FFxB_svxf_descr =
{ "less_FFxB_svxf", (void *) less_FFxB_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex32), sizeof(Complex32), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  less_equal  *********************/

static int less_equal_FFxB_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32  *tin0 =  (Complex32 *) buffers[0];
    Complex32   tin1 = *(Complex32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CLE(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_equal_FFxB_vsxf_descr =
{ "less_equal_FFxB_vsxf", (void *) less_equal_FFxB_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex32), sizeof(Complex32), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int less_equal_FFxB_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32  *tin0 =  (Complex32 *) buffers[0];
    Complex32  *tin1 =  (Complex32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CLE(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_equal_FFxB_vvxf_descr =
{ "less_equal_FFxB_vvxf", (void *) less_equal_FFxB_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex32), sizeof(Complex32), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int less_equal_FFxB_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32   tin0 = *(Complex32 *) buffers[0];
    Complex32  *tin1 =  (Complex32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CLE(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_equal_FFxB_svxf_descr =
{ "less_equal_FFxB_svxf", (void *) less_equal_FFxB_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex32), sizeof(Complex32), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  logical_and  *********************/

static int logical_and_FFxB_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32  *tin0 =  (Complex32 *) buffers[0];
    Complex32   tin1 = *(Complex32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CLAND(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_and_FFxB_vsxf_descr =
{ "logical_and_FFxB_vsxf", (void *) logical_and_FFxB_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex32), sizeof(Complex32), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int logical_and_FFxB_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32  *tin0 =  (Complex32 *) buffers[0];
    Complex32  *tin1 =  (Complex32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CLAND(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_and_FFxB_vvxf_descr =
{ "logical_and_FFxB_vvxf", (void *) logical_and_FFxB_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex32), sizeof(Complex32), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int logical_and_FFxB_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32   tin0 = *(Complex32 *) buffers[0];
    Complex32  *tin1 =  (Complex32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CLAND(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_and_FFxB_svxf_descr =
{ "logical_and_FFxB_svxf", (void *) logical_and_FFxB_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex32), sizeof(Complex32), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  logical_or  *********************/

static int logical_or_FFxB_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32  *tin0 =  (Complex32 *) buffers[0];
    Complex32   tin1 = *(Complex32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CLOR(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_or_FFxB_vsxf_descr =
{ "logical_or_FFxB_vsxf", (void *) logical_or_FFxB_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex32), sizeof(Complex32), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int logical_or_FFxB_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32  *tin0 =  (Complex32 *) buffers[0];
    Complex32  *tin1 =  (Complex32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CLOR(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_or_FFxB_vvxf_descr =
{ "logical_or_FFxB_vvxf", (void *) logical_or_FFxB_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex32), sizeof(Complex32), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int logical_or_FFxB_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32   tin0 = *(Complex32 *) buffers[0];
    Complex32  *tin1 =  (Complex32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CLOR(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_or_FFxB_svxf_descr =
{ "logical_or_FFxB_svxf", (void *) logical_or_FFxB_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex32), sizeof(Complex32), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  logical_xor  *********************/

static int logical_xor_FFxB_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32  *tin0 =  (Complex32 *) buffers[0];
    Complex32   tin1 = *(Complex32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CLXOR(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_xor_FFxB_vsxf_descr =
{ "logical_xor_FFxB_vsxf", (void *) logical_xor_FFxB_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex32), sizeof(Complex32), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int logical_xor_FFxB_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32  *tin0 =  (Complex32 *) buffers[0];
    Complex32  *tin1 =  (Complex32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CLXOR(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_xor_FFxB_vvxf_descr =
{ "logical_xor_FFxB_vvxf", (void *) logical_xor_FFxB_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex32), sizeof(Complex32), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int logical_xor_FFxB_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32   tin0 = *(Complex32 *) buffers[0];
    Complex32  *tin1 =  (Complex32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CLXOR(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_xor_FFxB_svxf_descr =
{ "logical_xor_FFxB_svxf", (void *) logical_xor_FFxB_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex32), sizeof(Complex32), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  logical_not  *********************/

static int logical_not_FxB_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32  *tin0 =  (Complex32 *) buffers[0];
    Bool       *tout0 =  (Bool *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CLNOT(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_not_FxB_vxf_descr =
{ "logical_not_FxB_vxf", (void *) logical_not_FxB_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Complex32), sizeof(Bool) }, { 0, 0, 0 } };
/*********************  floor  *********************/

static int floor_FxF_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32  *tin0 =  (Complex32 *) buffers[0];
    Complex32  *tout0 =  (Complex32 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CFLOOR(*tin0, *tout0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor floor_FxF_vxv_descr =
{ "floor_FxF_vxv", (void *) floor_FxF_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Complex32), sizeof(Complex32) }, { 0, 0, 0 } };
/*********************  ceil  *********************/

static int ceil_FxF_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32  *tin0 =  (Complex32 *) buffers[0];
    Complex32  *tout0 =  (Complex32 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CCEIL(*tin0, *tout0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor ceil_FxF_vxv_descr =
{ "ceil_FxF_vxv", (void *) ceil_FxF_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Complex32), sizeof(Complex32) }, { 0, 0, 0 } };
/*********************  maximum  *********************/

static int maximum_FFxF_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32  *tin0 =  (Complex32 *) buffers[0];
    Complex32   tin1 = *(Complex32 *) buffers[1];
    Complex32  *tout0 =  (Complex32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CMAX(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor maximum_FFxF_vsxf_descr =
{ "maximum_FFxF_vsxf", (void *) maximum_FFxF_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex32), sizeof(Complex32), sizeof(Complex32) }, { 0, 1, 0, 0 } };

static int maximum_FFxF_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32  *tin0 =  (Complex32 *) buffers[0];
    Complex32  *tin1 =  (Complex32 *) buffers[1];
    Complex32  *tout0 =  (Complex32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CMAX(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor maximum_FFxF_vvxf_descr =
{ "maximum_FFxF_vvxf", (void *) maximum_FFxF_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex32), sizeof(Complex32), sizeof(Complex32) }, { 0, 0, 0, 0 } };

static void _maximum_FxF_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Complex32  *tin0   = (Complex32 *) ((char *) input  + inboffset);
    Complex32 *tout0  = (Complex32 *) ((char *) output + outboffset);
    Complex32 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Complex32 *) ((char *) tin0 + inbstrides[dim]);
            net   = NUM_CMAX(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _maximum_FxF_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int maximum_FxF_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _maximum_FxF_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(maximum_FxF_R, CHECK_ALIGN, sizeof(Complex32), sizeof(Complex32));

static void _maximum_FxF_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Complex32 *tin0   = (Complex32 *) ((char *) input  + inboffset);
    Complex32 *tout0 = (Complex32 *) ((char *) output + outboffset);
    Complex32 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Complex32 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Complex32 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = NUM_CMAX(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _maximum_FxF_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  maximum_FxF_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _maximum_FxF_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(maximum_FxF_A, CHECK_ALIGN, sizeof(Complex32), sizeof(Complex32));

static int maximum_FFxF_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32   tin0 = *(Complex32 *) buffers[0];
    Complex32  *tin1 =  (Complex32 *) buffers[1];
    Complex32  *tout0 =  (Complex32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CMAX(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor maximum_FFxF_svxf_descr =
{ "maximum_FFxF_svxf", (void *) maximum_FFxF_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex32), sizeof(Complex32), sizeof(Complex32) }, { 1, 0, 0, 0 } };
/*********************  minimum  *********************/

static int minimum_FFxF_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32  *tin0 =  (Complex32 *) buffers[0];
    Complex32   tin1 = *(Complex32 *) buffers[1];
    Complex32  *tout0 =  (Complex32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CMIN(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor minimum_FFxF_vsxf_descr =
{ "minimum_FFxF_vsxf", (void *) minimum_FFxF_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex32), sizeof(Complex32), sizeof(Complex32) }, { 0, 1, 0, 0 } };

static int minimum_FFxF_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32  *tin0 =  (Complex32 *) buffers[0];
    Complex32  *tin1 =  (Complex32 *) buffers[1];
    Complex32  *tout0 =  (Complex32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CMIN(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor minimum_FFxF_vvxf_descr =
{ "minimum_FFxF_vvxf", (void *) minimum_FFxF_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex32), sizeof(Complex32), sizeof(Complex32) }, { 0, 0, 0, 0 } };

static void _minimum_FxF_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Complex32  *tin0   = (Complex32 *) ((char *) input  + inboffset);
    Complex32 *tout0  = (Complex32 *) ((char *) output + outboffset);
    Complex32 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Complex32 *) ((char *) tin0 + inbstrides[dim]);
            net   = NUM_CMIN(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _minimum_FxF_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int minimum_FxF_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _minimum_FxF_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(minimum_FxF_R, CHECK_ALIGN, sizeof(Complex32), sizeof(Complex32));

static void _minimum_FxF_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Complex32 *tin0   = (Complex32 *) ((char *) input  + inboffset);
    Complex32 *tout0 = (Complex32 *) ((char *) output + outboffset);
    Complex32 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Complex32 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Complex32 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = NUM_CMIN(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _minimum_FxF_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  minimum_FxF_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _minimum_FxF_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(minimum_FxF_A, CHECK_ALIGN, sizeof(Complex32), sizeof(Complex32));

static int minimum_FFxF_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32   tin0 = *(Complex32 *) buffers[0];
    Complex32  *tin1 =  (Complex32 *) buffers[1];
    Complex32  *tout0 =  (Complex32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NUM_CMIN(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor minimum_FFxF_svxf_descr =
{ "minimum_FFxF_svxf", (void *) minimum_FFxF_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex32), sizeof(Complex32), sizeof(Complex32) }, { 1, 0, 0, 0 } };
/*********************  fabs  *********************/

static int fabs_FxF_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32  *tin0 =  (Complex32 *) buffers[0];
    Complex32  *tout0 =  (Complex32 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CFABS(*tin0, *tout0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor fabs_FxF_vxv_descr =
{ "fabs_FxF_vxv", (void *) fabs_FxF_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Complex32), sizeof(Complex32) }, { 0, 0, 0 } };
/*********************  _round  *********************/

static int _round_FxF_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32  *tin0 =  (Complex32 *) buffers[0];
    Complex32  *tout0 =  (Complex32 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CROUND(*tin0, *tout0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor _round_FxF_vxv_descr =
{ "_round_FxF_vxv", (void *) _round_FxF_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Complex32), sizeof(Complex32) }, { 0, 0, 0 } };
/*********************  hypot  *********************/

static int hypot_FFxF_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32  *tin0 =  (Complex32 *) buffers[0];
    Complex32   tin1 = *(Complex32 *) buffers[1];
    Complex32  *tout0 =  (Complex32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CHYPOT(*tin0, tin1, *tout0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor hypot_FFxF_vsxv_descr =
{ "hypot_FFxF_vsxv", (void *) hypot_FFxF_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex32), sizeof(Complex32), sizeof(Complex32) }, { 0, 1, 0, 0 } };

static int hypot_FFxF_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32  *tin0 =  (Complex32 *) buffers[0];
    Complex32  *tin1 =  (Complex32 *) buffers[1];
    Complex32  *tout0 =  (Complex32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CHYPOT(*tin0, *tin1, *tout0);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor hypot_FFxF_vvxv_descr =
{ "hypot_FFxF_vvxv", (void *) hypot_FFxF_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex32), sizeof(Complex32), sizeof(Complex32) }, { 0, 0, 0, 0 } };

static void _hypot_FxF_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Complex32  *tin0   = (Complex32 *) ((char *) input  + inboffset);
    Complex32 *tout0  = (Complex32 *) ((char *) output + outboffset);
    Complex32 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Complex32 *) ((char *) tin0 + inbstrides[dim]);
            NUM_CHYPOT(net, *tin0, net);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _hypot_FxF_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int hypot_FxF_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _hypot_FxF_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(hypot_FxF_R, CHECK_ALIGN, sizeof(Complex32), sizeof(Complex32));

static void _hypot_FxF_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Complex32 *tin0   = (Complex32 *) ((char *) input  + inboffset);
    Complex32 *tout0 = (Complex32 *) ((char *) output + outboffset);
    Complex32 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Complex32 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Complex32 *) ((char *) tout0 + outbstrides[dim]);
            NUM_CHYPOT(lastval ,*tin0, *tout0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _hypot_FxF_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  hypot_FxF_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _hypot_FxF_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(hypot_FxF_A, CHECK_ALIGN, sizeof(Complex32), sizeof(Complex32));

static int hypot_FFxF_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32   tin0 = *(Complex32 *) buffers[0];
    Complex32  *tin1 =  (Complex32 *) buffers[1];
    Complex32  *tout0 =  (Complex32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_CHYPOT(tin0, *tin1, *tout0);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor hypot_FFxF_svxv_descr =
{ "hypot_FFxF_svxv", (void *) hypot_FFxF_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Complex32), sizeof(Complex32), sizeof(Complex32) }, { 1, 0, 0, 0 } };

static PyMethodDef _ufuncComplex32Methods[] = {

	{NULL,      NULL}        /* Sentinel */
};

static PyObject *init_funcDict(void) {
    PyObject *dict, *keytuple, *cfunc;
    dict = PyDict_New();
    /* isnan_FxB_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","isnan","v","Complex32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&isnan_FxB_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minus_FxF_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","minus","v","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minus_FxF_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_FFxF_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","add","vs","Complex32","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_FFxF_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_FFxF_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","add","vv","Complex32","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_FFxF_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_FxF_R */
    keytuple=Py_BuildValue("ss((s)(s))","add","R","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_FxF_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_FxF_A */
    keytuple=Py_BuildValue("ss((s)(s))","add","A","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_FxF_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_FFxF_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","add","sv","Complex32","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_FFxF_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_FFxF_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","subtract","vs","Complex32","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_FFxF_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_FFxF_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","subtract","vv","Complex32","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_FFxF_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_FxF_R */
    keytuple=Py_BuildValue("ss((s)(s))","subtract","R","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_FxF_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_FxF_A */
    keytuple=Py_BuildValue("ss((s)(s))","subtract","A","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_FxF_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_FFxF_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","subtract","sv","Complex32","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_FFxF_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* multiply_FFxF_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","multiply","vs","Complex32","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&multiply_FFxF_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* multiply_FFxF_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","multiply","vv","Complex32","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&multiply_FFxF_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* multiply_FxF_R */
    keytuple=Py_BuildValue("ss((s)(s))","multiply","R","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&multiply_FxF_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* multiply_FxF_A */
    keytuple=Py_BuildValue("ss((s)(s))","multiply","A","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&multiply_FxF_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* multiply_FFxF_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","multiply","sv","Complex32","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&multiply_FFxF_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_FFxF_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","divide","vs","Complex32","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_FFxF_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_FFxF_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","divide","vv","Complex32","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_FFxF_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_FxF_R */
    keytuple=Py_BuildValue("ss((s)(s))","divide","R","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_FxF_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_FxF_A */
    keytuple=Py_BuildValue("ss((s)(s))","divide","A","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_FxF_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_FFxF_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","divide","sv","Complex32","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_FFxF_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_FFxF_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","true_divide","vs","Complex32","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_FFxF_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_FFxF_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","true_divide","vv","Complex32","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_FFxF_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_FxF_R */
    keytuple=Py_BuildValue("ss((s)(s))","true_divide","R","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_FxF_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_FxF_A */
    keytuple=Py_BuildValue("ss((s)(s))","true_divide","A","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_FxF_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_FFxF_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","true_divide","sv","Complex32","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_FFxF_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_FFxF_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","remainder","vs","Complex32","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_FFxF_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_FFxF_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","remainder","vv","Complex32","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_FFxF_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_FxF_R */
    keytuple=Py_BuildValue("ss((s)(s))","remainder","R","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_FxF_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_FxF_A */
    keytuple=Py_BuildValue("ss((s)(s))","remainder","A","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_FxF_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_FFxF_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","remainder","sv","Complex32","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_FFxF_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_FFxF_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","power","vs","Complex32","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_FFxF_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_FFxF_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","power","vv","Complex32","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_FFxF_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_FxF_R */
    keytuple=Py_BuildValue("ss((s)(s))","power","R","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_FxF_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_FxF_A */
    keytuple=Py_BuildValue("ss((s)(s))","power","A","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_FxF_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_FFxF_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","power","sv","Complex32","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_FFxF_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* abs_Fxf_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","abs","v","Complex32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&abs_Fxf_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* sin_FxF_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","sin","v","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&sin_FxF_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* cos_FxF_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","cos","v","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&cos_FxF_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* tan_FxF_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","tan","v","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&tan_FxF_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arcsin_FxF_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","arcsin","v","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arcsin_FxF_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arccos_FxF_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","arccos","v","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arccos_FxF_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan_FxF_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","arctan","v","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan_FxF_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arcsinh_FxF_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","arcsinh","v","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arcsinh_FxF_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arccosh_FxF_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","arccosh","v","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arccosh_FxF_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctanh_FxF_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","arctanh","v","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctanh_FxF_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* log_FxF_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","log","v","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&log_FxF_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* log10_FxF_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","log10","v","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&log10_FxF_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* exp_FxF_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","exp","v","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&exp_FxF_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* sinh_FxF_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","sinh","v","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&sinh_FxF_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* cosh_FxF_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","cosh","v","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&cosh_FxF_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* tanh_FxF_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","tanh","v","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&tanh_FxF_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* sqrt_FxF_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","sqrt","v","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&sqrt_FxF_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* equal_FFxB_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","equal","vs","Complex32","Complex32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&equal_FFxB_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* equal_FFxB_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","equal","vv","Complex32","Complex32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&equal_FFxB_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* equal_FFxB_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","equal","sv","Complex32","Complex32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&equal_FFxB_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* not_equal_FFxB_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","not_equal","vs","Complex32","Complex32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&not_equal_FFxB_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* not_equal_FFxB_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","not_equal","vv","Complex32","Complex32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&not_equal_FFxB_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* not_equal_FFxB_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","not_equal","sv","Complex32","Complex32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&not_equal_FFxB_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_FFxB_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","greater","vs","Complex32","Complex32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_FFxB_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_FFxB_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","greater","vv","Complex32","Complex32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_FFxB_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_FFxB_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","greater","sv","Complex32","Complex32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_FFxB_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_equal_FFxB_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","greater_equal","vs","Complex32","Complex32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_equal_FFxB_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_equal_FFxB_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","greater_equal","vv","Complex32","Complex32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_equal_FFxB_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_equal_FFxB_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","greater_equal","sv","Complex32","Complex32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_equal_FFxB_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_FFxB_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","less","vs","Complex32","Complex32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_FFxB_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_FFxB_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","less","vv","Complex32","Complex32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_FFxB_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_FFxB_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","less","sv","Complex32","Complex32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_FFxB_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_equal_FFxB_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","less_equal","vs","Complex32","Complex32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_equal_FFxB_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_equal_FFxB_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","less_equal","vv","Complex32","Complex32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_equal_FFxB_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_equal_FFxB_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","less_equal","sv","Complex32","Complex32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_equal_FFxB_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_and_FFxB_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_and","vs","Complex32","Complex32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_and_FFxB_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_and_FFxB_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_and","vv","Complex32","Complex32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_and_FFxB_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_and_FFxB_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_and","sv","Complex32","Complex32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_and_FFxB_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_or_FFxB_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_or","vs","Complex32","Complex32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_or_FFxB_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_or_FFxB_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_or","vv","Complex32","Complex32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_or_FFxB_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_or_FFxB_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_or","sv","Complex32","Complex32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_or_FFxB_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_xor_FFxB_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_xor","vs","Complex32","Complex32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_xor_FFxB_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_xor_FFxB_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_xor","vv","Complex32","Complex32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_xor_FFxB_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_xor_FFxB_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_xor","sv","Complex32","Complex32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_xor_FFxB_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_not_FxB_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","logical_not","v","Complex32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_not_FxB_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_FxF_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","floor","v","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_FxF_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* ceil_FxF_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","ceil","v","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&ceil_FxF_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_FFxF_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","maximum","vs","Complex32","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_FFxF_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_FFxF_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","maximum","vv","Complex32","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_FFxF_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_FxF_R */
    keytuple=Py_BuildValue("ss((s)(s))","maximum","R","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_FxF_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_FxF_A */
    keytuple=Py_BuildValue("ss((s)(s))","maximum","A","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_FxF_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_FFxF_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","maximum","sv","Complex32","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_FFxF_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_FFxF_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","minimum","vs","Complex32","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_FFxF_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_FFxF_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","minimum","vv","Complex32","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_FFxF_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_FxF_R */
    keytuple=Py_BuildValue("ss((s)(s))","minimum","R","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_FxF_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_FxF_A */
    keytuple=Py_BuildValue("ss((s)(s))","minimum","A","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_FxF_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_FFxF_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","minimum","sv","Complex32","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_FFxF_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* fabs_FxF_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","fabs","v","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&fabs_FxF_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* _round_FxF_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","_round","v","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&_round_FxF_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_FFxF_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","hypot","vs","Complex32","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_FFxF_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_FFxF_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","hypot","vv","Complex32","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_FFxF_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_FxF_R */
    keytuple=Py_BuildValue("ss((s)(s))","hypot","R","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_FxF_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_FxF_A */
    keytuple=Py_BuildValue("ss((s)(s))","hypot","A","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_FxF_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_FFxF_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","hypot","sv","Complex32","Complex32","Complex32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_FFxF_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    return dict;
}

/* platform independent*/
#ifdef MS_WIN32
__declspec(dllexport)
#endif
void init_ufuncComplex32(void) {
    PyObject *m, *d, *functions;
    m = Py_InitModule("_ufuncComplex32", _ufuncComplex32Methods);
    d = PyModule_GetDict(m);
    import_libnumarray();
    functions = init_funcDict();
    PyDict_SetItemString(d, "functionDict", functions);
    Py_DECREF(functions);
    ADD_VERSION(m);
}
