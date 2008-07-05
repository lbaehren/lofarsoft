
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

static int minus_NxN_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Int64      *tout0 =  (Int64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = -*tin0;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor minus_NxN_vxv_descr =
{ "minus_NxN_vxv", (void *) minus_NxN_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int64), sizeof(Int64) }, { 0, 0, 0 } };
/*********************  add  *********************/

static int add_NNxN_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Int64       tin1 = *(Int64 *) buffers[1];
    Int64      *tout0 =  (Int64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 + tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor add_NNxN_vsxv_descr =
{ "add_NNxN_vsxv", (void *) add_NNxN_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Int64) }, { 0, 1, 0, 0 } };

static int add_NNxN_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Int64      *tin1 =  (Int64 *) buffers[1];
    Int64      *tout0 =  (Int64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 + *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor add_NNxN_vvxv_descr =
{ "add_NNxN_vvxv", (void *) add_NNxN_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Int64) }, { 0, 0, 0, 0 } };

static void _add_NxN_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int64  *tin0   = (Int64 *) ((char *) input  + inboffset);
    Int64 *tout0  = (Int64 *) ((char *) output + outboffset);
    Int64 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Int64 *) ((char *) tin0 + inbstrides[dim]);
            net    =     net + *tin0;
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _add_NxN_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int add_NxN_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _add_NxN_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(add_NxN_R, CHECK_ALIGN, sizeof(Int64), sizeof(Int64));

static void _add_NxN_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int64 *tin0   = (Int64 *) ((char *) input  + inboffset);
    Int64 *tout0 = (Int64 *) ((char *) output + outboffset);
    Int64 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Int64 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Int64 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = lastval + *tin0;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _add_NxN_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  add_NxN_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _add_NxN_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(add_NxN_A, CHECK_ALIGN, sizeof(Int64), sizeof(Int64));

static int add_NNxN_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64       tin0 = *(Int64 *) buffers[0];
    Int64      *tin1 =  (Int64 *) buffers[1];
    Int64      *tout0 =  (Int64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 + *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor add_NNxN_svxv_descr =
{ "add_NNxN_svxv", (void *) add_NNxN_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Int64) }, { 1, 0, 0, 0 } };
/*********************  subtract  *********************/

static int subtract_NNxN_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Int64       tin1 = *(Int64 *) buffers[1];
    Int64      *tout0 =  (Int64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 - tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor subtract_NNxN_vsxv_descr =
{ "subtract_NNxN_vsxv", (void *) subtract_NNxN_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Int64) }, { 0, 1, 0, 0 } };

static int subtract_NNxN_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Int64      *tin1 =  (Int64 *) buffers[1];
    Int64      *tout0 =  (Int64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 - *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor subtract_NNxN_vvxv_descr =
{ "subtract_NNxN_vvxv", (void *) subtract_NNxN_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Int64) }, { 0, 0, 0, 0 } };

static void _subtract_NxN_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int64  *tin0   = (Int64 *) ((char *) input  + inboffset);
    Int64 *tout0  = (Int64 *) ((char *) output + outboffset);
    Int64 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Int64 *) ((char *) tin0 + inbstrides[dim]);
            net    =     net - *tin0;
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _subtract_NxN_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int subtract_NxN_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _subtract_NxN_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(subtract_NxN_R, CHECK_ALIGN, sizeof(Int64), sizeof(Int64));

static void _subtract_NxN_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int64 *tin0   = (Int64 *) ((char *) input  + inboffset);
    Int64 *tout0 = (Int64 *) ((char *) output + outboffset);
    Int64 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Int64 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Int64 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = lastval - *tin0;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _subtract_NxN_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  subtract_NxN_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _subtract_NxN_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(subtract_NxN_A, CHECK_ALIGN, sizeof(Int64), sizeof(Int64));

static int subtract_NNxN_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64       tin0 = *(Int64 *) buffers[0];
    Int64      *tin1 =  (Int64 *) buffers[1];
    Int64      *tout0 =  (Int64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 - *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor subtract_NNxN_svxv_descr =
{ "subtract_NNxN_svxv", (void *) subtract_NNxN_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Int64) }, { 1, 0, 0, 0 } };
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/

static int multiply_NNxN_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Int64       tin1 = *(Int64 *) buffers[1];
    Int64      *tout0 =  (Int64 *) buffers[2];
Int64 temp;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        temp = ((Int64) *tin0) * ((Int64) tin1);
    if (smult64_overflow(*tin0, tin1)) temp = (Float64) int_overflow_error(4611686018427387903.);
    *tout0 = (Int64) temp;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor multiply_NNxN_vsxv_descr =
{ "multiply_NNxN_vsxv", (void *) multiply_NNxN_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Int64) }, { 0, 1, 0, 0 } };

static int multiply_NNxN_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Int64      *tin1 =  (Int64 *) buffers[1];
    Int64      *tout0 =  (Int64 *) buffers[2];
Int64 temp;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        temp = ((Int64) *tin0) * ((Int64) *tin1);
    if (smult64_overflow(*tin0, *tin1)) temp = (Float64) int_overflow_error(4611686018427387903.);
    *tout0 = (Int64) temp;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor multiply_NNxN_vvxv_descr =
{ "multiply_NNxN_vvxv", (void *) multiply_NNxN_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Int64) }, { 0, 0, 0, 0 } };

static void _multiply_NxN_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int64  *tin0   = (Int64 *) ((char *) input  + inboffset);
    Int64 *tout0  = (Int64 *) ((char *) output + outboffset);
    Int64 net;
    Int64 temp;
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Int64 *) ((char *) tin0 + inbstrides[dim]);
            temp = ((Int64) net) * ((Int64) *tin0);
    if (smult64_overflow(net, *tin0)) temp = (Float64) int_overflow_error(4611686018427387903.);
    net = (Int64) temp;
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _multiply_NxN_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int multiply_NxN_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _multiply_NxN_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(multiply_NxN_R, CHECK_ALIGN, sizeof(Int64), sizeof(Int64));

static void _multiply_NxN_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int64 *tin0   = (Int64 *) ((char *) input  + inboffset);
    Int64 *tout0 = (Int64 *) ((char *) output + outboffset);
    Int64 lastval;
    Int64 temp;
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Int64 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Int64 *) ((char *) tout0 + outbstrides[dim]);
            temp = ((Int64) lastval) * ((Int64) *tin0);
    if (smult64_overflow(lastval, *tin0)) temp = (Float64) int_overflow_error(4611686018427387903.);
    *tout0 = (Int64) temp;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _multiply_NxN_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  multiply_NxN_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _multiply_NxN_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(multiply_NxN_A, CHECK_ALIGN, sizeof(Int64), sizeof(Int64));

static int multiply_NNxN_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64       tin0 = *(Int64 *) buffers[0];
    Int64      *tin1 =  (Int64 *) buffers[1];
    Int64      *tout0 =  (Int64 *) buffers[2];
Int64 temp;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        temp = ((Int64) tin0) * ((Int64) *tin1);
    if (smult64_overflow(tin0, *tin1)) temp = (Float64) int_overflow_error(4611686018427387903.);
    *tout0 = (Int64) temp;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor multiply_NNxN_svxv_descr =
{ "multiply_NNxN_svxv", (void *) multiply_NNxN_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Int64) }, { 1, 0, 0, 0 } };
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  divide  *********************/

static int divide_NNxN_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Int64       tin1 = *(Int64 *) buffers[1];
    Int64      *tout0 =  (Int64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((tin1==0) ? int_dividebyzero_error(tin1, *tin0) : *tin0 / tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor divide_NNxN_vsxv_descr =
{ "divide_NNxN_vsxv", (void *) divide_NNxN_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Int64) }, { 0, 1, 0, 0 } };

static int divide_NNxN_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Int64      *tin1 =  (Int64 *) buffers[1];
    Int64      *tout0 =  (Int64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, *tin0) : *tin0 / *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor divide_NNxN_vvxv_descr =
{ "divide_NNxN_vvxv", (void *) divide_NNxN_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Int64) }, { 0, 0, 0, 0 } };

static void _divide_NxN_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int64  *tin0   = (Int64 *) ((char *) input  + inboffset);
    Int64 *tout0  = (Int64 *) ((char *) output + outboffset);
    Int64 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Int64 *) ((char *) tin0 + inbstrides[dim]);
            net = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0) : net / *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _divide_NxN_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int divide_NxN_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _divide_NxN_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(divide_NxN_R, CHECK_ALIGN, sizeof(Int64), sizeof(Int64));

static void _divide_NxN_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int64 *tin0   = (Int64 *) ((char *) input  + inboffset);
    Int64 *tout0 = (Int64 *) ((char *) output + outboffset);
    Int64 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Int64 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Int64 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0) : lastval / *tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _divide_NxN_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  divide_NxN_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _divide_NxN_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(divide_NxN_A, CHECK_ALIGN, sizeof(Int64), sizeof(Int64));

static int divide_NNxN_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64       tin0 = *(Int64 *) buffers[0];
    Int64      *tin1 =  (Int64 *) buffers[1];
    Int64      *tout0 =  (Int64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, 0) : tin0 / *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor divide_NNxN_svxv_descr =
{ "divide_NNxN_svxv", (void *) divide_NNxN_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Int64) }, { 1, 0, 0, 0 } };
/*********************  divide  *********************/
/*********************  floor_divide  *********************/

static int floor_divide_NNxN_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Int64       tin1 = *(Int64 *) buffers[1];
    Int64      *tout0 =  (Int64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = (( tin1==0) ? int_dividebyzero_error( tin1, *tin0) : floor(*tin0   / (double)  tin1));
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor floor_divide_NNxN_vsxv_descr =
{ "floor_divide_NNxN_vsxv", (void *) floor_divide_NNxN_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Int64) }, { 0, 1, 0, 0 } };

static int floor_divide_NNxN_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Int64      *tin1 =  (Int64 *) buffers[1];
    Int64      *tout0 =  (Int64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, *tin0) : floor(*tin0   / (double) *tin1));
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor floor_divide_NNxN_vvxv_descr =
{ "floor_divide_NNxN_vvxv", (void *) floor_divide_NNxN_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Int64) }, { 0, 0, 0, 0 } };

static void _floor_divide_NxN_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int64  *tin0   = (Int64 *) ((char *) input  + inboffset);
    Int64 *tout0  = (Int64 *) ((char *) output + outboffset);
    Int64 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Int64 *) ((char *) tin0 + inbstrides[dim]);
            net    = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0)     : floor(net     / (double) *tin0));
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _floor_divide_NxN_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int floor_divide_NxN_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _floor_divide_NxN_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(floor_divide_NxN_R, CHECK_ALIGN, sizeof(Int64), sizeof(Int64));

static void _floor_divide_NxN_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int64 *tin0   = (Int64 *) ((char *) input  + inboffset);
    Int64 *tout0 = (Int64 *) ((char *) output + outboffset);
    Int64 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Int64 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Int64 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0)     : floor(lastval / (double) *tin0));
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _floor_divide_NxN_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  floor_divide_NxN_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _floor_divide_NxN_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(floor_divide_NxN_A, CHECK_ALIGN, sizeof(Int64), sizeof(Int64));

static int floor_divide_NNxN_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64       tin0 = *(Int64 *) buffers[0];
    Int64      *tin1 =  (Int64 *) buffers[1];
    Int64      *tout0 =  (Int64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, 0)     : floor(tin0    / (double) *tin1));
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor floor_divide_NNxN_svxv_descr =
{ "floor_divide_NNxN_svxv", (void *) floor_divide_NNxN_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Int64) }, { 1, 0, 0, 0 } };
/*********************  floor_divide  *********************/
/*********************  true_divide  *********************/

static int true_divide_NNxd_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Int64       tin1 = *(Int64 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = (( tin1==0) ? int_dividebyzero_error( tin1, *tin0) : *tin0         / (double) tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor true_divide_NNxd_vsxv_descr =
{ "true_divide_NNxd_vsxv", (void *) true_divide_NNxd_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Float64) }, { 0, 1, 0, 0 } };

static int true_divide_NNxd_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Int64      *tin1 =  (Int64 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, *tin0) : *tin0         / (double) *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor true_divide_NNxd_vvxv_descr =
{ "true_divide_NNxd_vvxv", (void *) true_divide_NNxd_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Float64) }, { 0, 0, 0, 0 } };

static void _true_divide_Nxd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int64  *tin0   = (Int64 *) ((char *) input  + inboffset);
    Float64 *tout0  = (Float64 *) ((char *) output + outboffset);
    Float64 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Int64 *) ((char *) tin0 + inbstrides[dim]);
            net    = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0)     : net           / (double) *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _true_divide_Nxd_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int true_divide_Nxd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _true_divide_Nxd_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(true_divide_Nxd_R, CHECK_ALIGN, sizeof(Int64), sizeof(Float64));

static void _true_divide_Nxd_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int64 *tin0   = (Int64 *) ((char *) input  + inboffset);
    Float64 *tout0 = (Float64 *) ((char *) output + outboffset);
    Float64 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Int64 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Float64 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0)     : lastval       / (double) *tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _true_divide_Nxd_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  true_divide_Nxd_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _true_divide_Nxd_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(true_divide_Nxd_A, CHECK_ALIGN, sizeof(Int64), sizeof(Float64));

static int true_divide_NNxd_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64       tin0 = *(Int64 *) buffers[0];
    Int64      *tin1 =  (Int64 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, 0)     : tin0          / (double) *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor true_divide_NNxd_svxv_descr =
{ "true_divide_NNxd_svxv", (void *) true_divide_NNxd_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Float64) }, { 1, 0, 0, 0 } };
/*********************  true_divide  *********************/
/*********************  remainder  *********************/

static int remainder_NNxN_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Int64       tin1 = *(Int64 *) buffers[1];
    Int64      *tout0 =  (Int64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((tin1==0) ? int_dividebyzero_error(tin1, *tin0) : *tin0  %  tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor remainder_NNxN_vsxv_descr =
{ "remainder_NNxN_vsxv", (void *) remainder_NNxN_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Int64) }, { 0, 1, 0, 0 } };

static int remainder_NNxN_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Int64      *tin1 =  (Int64 *) buffers[1];
    Int64      *tout0 =  (Int64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, *tin0) : *tin0  %  *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor remainder_NNxN_vvxv_descr =
{ "remainder_NNxN_vvxv", (void *) remainder_NNxN_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Int64) }, { 0, 0, 0, 0 } };

static void _remainder_NxN_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int64  *tin0   = (Int64 *) ((char *) input  + inboffset);
    Int64 *tout0  = (Int64 *) ((char *) output + outboffset);
    Int64 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Int64 *) ((char *) tin0 + inbstrides[dim]);
            net = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0) : net  %  *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _remainder_NxN_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int remainder_NxN_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _remainder_NxN_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(remainder_NxN_R, CHECK_ALIGN, sizeof(Int64), sizeof(Int64));

static void _remainder_NxN_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int64 *tin0   = (Int64 *) ((char *) input  + inboffset);
    Int64 *tout0 = (Int64 *) ((char *) output + outboffset);
    Int64 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Int64 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Int64 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0) : lastval  %  *tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _remainder_NxN_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  remainder_NxN_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _remainder_NxN_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(remainder_NxN_A, CHECK_ALIGN, sizeof(Int64), sizeof(Int64));

static int remainder_NNxN_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64       tin0 = *(Int64 *) buffers[0];
    Int64      *tin1 =  (Int64 *) buffers[1];
    Int64      *tout0 =  (Int64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, 0) : tin0  %  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor remainder_NNxN_svxv_descr =
{ "remainder_NNxN_svxv", (void *) remainder_NNxN_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Int64) }, { 1, 0, 0, 0 } };
/*********************  remainder  *********************/
/*********************  power  *********************/

static int power_NNxN_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Int64       tin1 = *(Int64 *) buffers[1];
    Int64      *tout0 =  (Int64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_pow(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor power_NNxN_vsxf_descr =
{ "power_NNxN_vsxf", (void *) power_NNxN_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Int64) }, { 0, 1, 0, 0 } };

static int power_NNxN_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Int64      *tin1 =  (Int64 *) buffers[1];
    Int64      *tout0 =  (Int64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_pow(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor power_NNxN_vvxf_descr =
{ "power_NNxN_vvxf", (void *) power_NNxN_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Int64) }, { 0, 0, 0, 0 } };

static void _power_NxN_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int64  *tin0   = (Int64 *) ((char *) input  + inboffset);
    Int64 *tout0  = (Int64 *) ((char *) output + outboffset);
    Int64 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Int64 *) ((char *) tin0 + inbstrides[dim]);
            net   = num_pow(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _power_NxN_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int power_NxN_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _power_NxN_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(power_NxN_R, CHECK_ALIGN, sizeof(Int64), sizeof(Int64));

static void _power_NxN_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int64 *tin0   = (Int64 *) ((char *) input  + inboffset);
    Int64 *tout0 = (Int64 *) ((char *) output + outboffset);
    Int64 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Int64 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Int64 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = num_pow(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _power_NxN_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  power_NxN_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _power_NxN_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(power_NxN_A, CHECK_ALIGN, sizeof(Int64), sizeof(Int64));

static int power_NNxN_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64       tin0 = *(Int64 *) buffers[0];
    Int64      *tin1 =  (Int64 *) buffers[1];
    Int64      *tout0 =  (Int64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_pow(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor power_NNxN_svxf_descr =
{ "power_NNxN_svxf", (void *) power_NNxN_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Int64) }, { 1, 0, 0, 0 } };
/*********************  abs  *********************/

static int abs_NxN_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Int64      *tout0 =  (Int64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = fabs(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor abs_NxN_vxf_descr =
{ "abs_NxN_vxf", (void *) abs_NxN_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int64), sizeof(Int64) }, { 0, 0, 0 } };
/*********************  sin  *********************/

static int sin_Nxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = sin(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor sin_Nxd_vxf_descr =
{ "sin_Nxd_vxf", (void *) sin_Nxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int64), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  cos  *********************/

static int cos_Nxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = cos(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor cos_Nxd_vxf_descr =
{ "cos_Nxd_vxf", (void *) cos_Nxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int64), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  tan  *********************/

static int tan_Nxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = tan(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor tan_Nxd_vxf_descr =
{ "tan_Nxd_vxf", (void *) tan_Nxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int64), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arcsin  *********************/

static int arcsin_Nxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = asin(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arcsin_Nxd_vxf_descr =
{ "arcsin_Nxd_vxf", (void *) arcsin_Nxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int64), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arccos  *********************/

static int arccos_Nxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = acos(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arccos_Nxd_vxf_descr =
{ "arccos_Nxd_vxf", (void *) arccos_Nxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int64), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arctan  *********************/

static int arctan_Nxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = atan(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arctan_Nxd_vxf_descr =
{ "arctan_Nxd_vxf", (void *) arctan_Nxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int64), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arctan2  *********************/

static int arctan2_NNxd_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Int64       tin1 = *(Int64 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = atan2(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arctan2_NNxd_vsxf_descr =
{ "arctan2_NNxd_vsxf", (void *) arctan2_NNxd_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Float64) }, { 0, 1, 0, 0 } };

static int arctan2_NNxd_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Int64      *tin1 =  (Int64 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = atan2(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arctan2_NNxd_vvxf_descr =
{ "arctan2_NNxd_vvxf", (void *) arctan2_NNxd_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Float64) }, { 0, 0, 0, 0 } };

static void _arctan2_Nxd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int64  *tin0   = (Int64 *) ((char *) input  + inboffset);
    Float64 *tout0  = (Float64 *) ((char *) output + outboffset);
    Float64 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Int64 *) ((char *) tin0 + inbstrides[dim]);
            net   = atan2(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _arctan2_Nxd_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int arctan2_Nxd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _arctan2_Nxd_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(arctan2_Nxd_R, CHECK_ALIGN, sizeof(Int64), sizeof(Float64));

static void _arctan2_Nxd_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int64 *tin0   = (Int64 *) ((char *) input  + inboffset);
    Float64 *tout0 = (Float64 *) ((char *) output + outboffset);
    Float64 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Int64 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Float64 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = atan2(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _arctan2_Nxd_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  arctan2_Nxd_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _arctan2_Nxd_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(arctan2_Nxd_A, CHECK_ALIGN, sizeof(Int64), sizeof(Float64));

static int arctan2_NNxd_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64       tin0 = *(Int64 *) buffers[0];
    Int64      *tin1 =  (Int64 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = atan2(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arctan2_NNxd_svxf_descr =
{ "arctan2_NNxd_svxf", (void *) arctan2_NNxd_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Float64) }, { 1, 0, 0, 0 } };
/*********************  log  *********************/

static int log_Nxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_log(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor log_Nxd_vxf_descr =
{ "log_Nxd_vxf", (void *) log_Nxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int64), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  log10  *********************/

static int log10_Nxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_log10(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor log10_Nxd_vxf_descr =
{ "log10_Nxd_vxf", (void *) log10_Nxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int64), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  exp  *********************/

static int exp_Nxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = exp(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor exp_Nxd_vxf_descr =
{ "exp_Nxd_vxf", (void *) exp_Nxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int64), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  sinh  *********************/

static int sinh_Nxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = sinh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor sinh_Nxd_vxf_descr =
{ "sinh_Nxd_vxf", (void *) sinh_Nxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int64), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  cosh  *********************/

static int cosh_Nxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = cosh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor cosh_Nxd_vxf_descr =
{ "cosh_Nxd_vxf", (void *) cosh_Nxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int64), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  tanh  *********************/

static int tanh_Nxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = tanh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor tanh_Nxd_vxf_descr =
{ "tanh_Nxd_vxf", (void *) tanh_Nxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int64), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arcsinh  *********************/

static int arcsinh_Nxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_asinh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arcsinh_Nxd_vxf_descr =
{ "arcsinh_Nxd_vxf", (void *) arcsinh_Nxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int64), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arccosh  *********************/

static int arccosh_Nxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_acosh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arccosh_Nxd_vxf_descr =
{ "arccosh_Nxd_vxf", (void *) arccosh_Nxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int64), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arctanh  *********************/

static int arctanh_Nxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_atanh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arctanh_Nxd_vxf_descr =
{ "arctanh_Nxd_vxf", (void *) arctanh_Nxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int64), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  ieeemask  *********************/
/*********************  ieeemask  *********************/
/*********************  isnan  *********************/
/*********************  isnan  *********************/
/*********************  isnan  *********************/
/*********************  isnan  *********************/
/*********************  sqrt  *********************/

static int sqrt_Nxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = sqrt(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor sqrt_Nxd_vxf_descr =
{ "sqrt_Nxd_vxf", (void *) sqrt_Nxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int64), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  equal  *********************/

static int equal_NNxB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Int64       tin1 = *(Int64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 == tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor equal_NNxB_vsxv_descr =
{ "equal_NNxB_vsxv", (void *) equal_NNxB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int equal_NNxB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Int64      *tin1 =  (Int64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 == *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor equal_NNxB_vvxv_descr =
{ "equal_NNxB_vvxv", (void *) equal_NNxB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int equal_NNxB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64       tin0 = *(Int64 *) buffers[0];
    Int64      *tin1 =  (Int64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 == *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor equal_NNxB_svxv_descr =
{ "equal_NNxB_svxv", (void *) equal_NNxB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  not_equal  *********************/

static int not_equal_NNxB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Int64       tin1 = *(Int64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 != tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor not_equal_NNxB_vsxv_descr =
{ "not_equal_NNxB_vsxv", (void *) not_equal_NNxB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int not_equal_NNxB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Int64      *tin1 =  (Int64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 != *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor not_equal_NNxB_vvxv_descr =
{ "not_equal_NNxB_vvxv", (void *) not_equal_NNxB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int not_equal_NNxB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64       tin0 = *(Int64 *) buffers[0];
    Int64      *tin1 =  (Int64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 != *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor not_equal_NNxB_svxv_descr =
{ "not_equal_NNxB_svxv", (void *) not_equal_NNxB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  greater  *********************/

static int greater_NNxB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Int64       tin1 = *(Int64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 > tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_NNxB_vsxv_descr =
{ "greater_NNxB_vsxv", (void *) greater_NNxB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int greater_NNxB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Int64      *tin1 =  (Int64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 > *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_NNxB_vvxv_descr =
{ "greater_NNxB_vvxv", (void *) greater_NNxB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int greater_NNxB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64       tin0 = *(Int64 *) buffers[0];
    Int64      *tin1 =  (Int64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 > *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_NNxB_svxv_descr =
{ "greater_NNxB_svxv", (void *) greater_NNxB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  greater_equal  *********************/

static int greater_equal_NNxB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Int64       tin1 = *(Int64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 >= tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_equal_NNxB_vsxv_descr =
{ "greater_equal_NNxB_vsxv", (void *) greater_equal_NNxB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int greater_equal_NNxB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Int64      *tin1 =  (Int64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 >= *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_equal_NNxB_vvxv_descr =
{ "greater_equal_NNxB_vvxv", (void *) greater_equal_NNxB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int greater_equal_NNxB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64       tin0 = *(Int64 *) buffers[0];
    Int64      *tin1 =  (Int64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 >= *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_equal_NNxB_svxv_descr =
{ "greater_equal_NNxB_svxv", (void *) greater_equal_NNxB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  less  *********************/

static int less_NNxB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Int64       tin1 = *(Int64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 < tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_NNxB_vsxv_descr =
{ "less_NNxB_vsxv", (void *) less_NNxB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int less_NNxB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Int64      *tin1 =  (Int64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 < *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_NNxB_vvxv_descr =
{ "less_NNxB_vvxv", (void *) less_NNxB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int less_NNxB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64       tin0 = *(Int64 *) buffers[0];
    Int64      *tin1 =  (Int64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 < *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_NNxB_svxv_descr =
{ "less_NNxB_svxv", (void *) less_NNxB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  less_equal  *********************/

static int less_equal_NNxB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Int64       tin1 = *(Int64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 <= tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_equal_NNxB_vsxv_descr =
{ "less_equal_NNxB_vsxv", (void *) less_equal_NNxB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int less_equal_NNxB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Int64      *tin1 =  (Int64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 <= *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_equal_NNxB_vvxv_descr =
{ "less_equal_NNxB_vvxv", (void *) less_equal_NNxB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int less_equal_NNxB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64       tin0 = *(Int64 *) buffers[0];
    Int64      *tin1 =  (Int64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 <= *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_equal_NNxB_svxv_descr =
{ "less_equal_NNxB_svxv", (void *) less_equal_NNxB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  logical_and  *********************/

static int logical_and_NNxB_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Int64       tin1 = *(Int64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_and(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_and_NNxB_vsxf_descr =
{ "logical_and_NNxB_vsxf", (void *) logical_and_NNxB_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int logical_and_NNxB_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Int64      *tin1 =  (Int64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_and(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_and_NNxB_vvxf_descr =
{ "logical_and_NNxB_vvxf", (void *) logical_and_NNxB_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int logical_and_NNxB_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64       tin0 = *(Int64 *) buffers[0];
    Int64      *tin1 =  (Int64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_and(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_and_NNxB_svxf_descr =
{ "logical_and_NNxB_svxf", (void *) logical_and_NNxB_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  logical_or  *********************/

static int logical_or_NNxB_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Int64       tin1 = *(Int64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_or(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_or_NNxB_vsxf_descr =
{ "logical_or_NNxB_vsxf", (void *) logical_or_NNxB_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int logical_or_NNxB_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Int64      *tin1 =  (Int64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_or(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_or_NNxB_vvxf_descr =
{ "logical_or_NNxB_vvxf", (void *) logical_or_NNxB_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int logical_or_NNxB_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64       tin0 = *(Int64 *) buffers[0];
    Int64      *tin1 =  (Int64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_or(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_or_NNxB_svxf_descr =
{ "logical_or_NNxB_svxf", (void *) logical_or_NNxB_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  logical_xor  *********************/

static int logical_xor_NNxB_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Int64       tin1 = *(Int64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_xor(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_xor_NNxB_vsxf_descr =
{ "logical_xor_NNxB_vsxf", (void *) logical_xor_NNxB_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int logical_xor_NNxB_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Int64      *tin1 =  (Int64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_xor(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_xor_NNxB_vvxf_descr =
{ "logical_xor_NNxB_vvxf", (void *) logical_xor_NNxB_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int logical_xor_NNxB_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64       tin0 = *(Int64 *) buffers[0];
    Int64      *tin1 =  (Int64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_xor(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_xor_NNxB_svxf_descr =
{ "logical_xor_NNxB_svxf", (void *) logical_xor_NNxB_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  logical_and  *********************/
/*********************  logical_or  *********************/
/*********************  logical_xor  *********************/
/*********************  logical_not  *********************/

static int logical_not_NxB_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Bool       *tout0 =  (Bool *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = !*tin0;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_not_NxB_vxv_descr =
{ "logical_not_NxB_vxv", (void *) logical_not_NxB_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int64), sizeof(Bool) }, { 0, 0, 0 } };
/*********************  bitwise_and  *********************/

static int bitwise_and_NNxN_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Int64       tin1 = *(Int64 *) buffers[1];
    Int64      *tout0 =  (Int64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 & tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_and_NNxN_vsxv_descr =
{ "bitwise_and_NNxN_vsxv", (void *) bitwise_and_NNxN_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Int64) }, { 0, 1, 0, 0 } };

static int bitwise_and_NNxN_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Int64      *tin1 =  (Int64 *) buffers[1];
    Int64      *tout0 =  (Int64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 & *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_and_NNxN_vvxv_descr =
{ "bitwise_and_NNxN_vvxv", (void *) bitwise_and_NNxN_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Int64) }, { 0, 0, 0, 0 } };

static void _bitwise_and_NxN_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int64  *tin0   = (Int64 *) ((char *) input  + inboffset);
    Int64 *tout0  = (Int64 *) ((char *) output + outboffset);
    Int64 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Int64 *) ((char *) tin0 + inbstrides[dim]);
            net    =     net & *tin0;
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _bitwise_and_NxN_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int bitwise_and_NxN_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _bitwise_and_NxN_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(bitwise_and_NxN_R, CHECK_ALIGN, sizeof(Int64), sizeof(Int64));

static void _bitwise_and_NxN_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int64 *tin0   = (Int64 *) ((char *) input  + inboffset);
    Int64 *tout0 = (Int64 *) ((char *) output + outboffset);
    Int64 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Int64 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Int64 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = lastval & *tin0;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _bitwise_and_NxN_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  bitwise_and_NxN_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _bitwise_and_NxN_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(bitwise_and_NxN_A, CHECK_ALIGN, sizeof(Int64), sizeof(Int64));

static int bitwise_and_NNxN_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64       tin0 = *(Int64 *) buffers[0];
    Int64      *tin1 =  (Int64 *) buffers[1];
    Int64      *tout0 =  (Int64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 & *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_and_NNxN_svxv_descr =
{ "bitwise_and_NNxN_svxv", (void *) bitwise_and_NNxN_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Int64) }, { 1, 0, 0, 0 } };
/*********************  bitwise_or  *********************/

static int bitwise_or_NNxN_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Int64       tin1 = *(Int64 *) buffers[1];
    Int64      *tout0 =  (Int64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 | tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_or_NNxN_vsxv_descr =
{ "bitwise_or_NNxN_vsxv", (void *) bitwise_or_NNxN_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Int64) }, { 0, 1, 0, 0 } };

static int bitwise_or_NNxN_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Int64      *tin1 =  (Int64 *) buffers[1];
    Int64      *tout0 =  (Int64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 | *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_or_NNxN_vvxv_descr =
{ "bitwise_or_NNxN_vvxv", (void *) bitwise_or_NNxN_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Int64) }, { 0, 0, 0, 0 } };

static void _bitwise_or_NxN_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int64  *tin0   = (Int64 *) ((char *) input  + inboffset);
    Int64 *tout0  = (Int64 *) ((char *) output + outboffset);
    Int64 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Int64 *) ((char *) tin0 + inbstrides[dim]);
            net    =     net | *tin0;
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _bitwise_or_NxN_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int bitwise_or_NxN_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _bitwise_or_NxN_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(bitwise_or_NxN_R, CHECK_ALIGN, sizeof(Int64), sizeof(Int64));

static void _bitwise_or_NxN_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int64 *tin0   = (Int64 *) ((char *) input  + inboffset);
    Int64 *tout0 = (Int64 *) ((char *) output + outboffset);
    Int64 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Int64 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Int64 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = lastval | *tin0;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _bitwise_or_NxN_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  bitwise_or_NxN_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _bitwise_or_NxN_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(bitwise_or_NxN_A, CHECK_ALIGN, sizeof(Int64), sizeof(Int64));

static int bitwise_or_NNxN_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64       tin0 = *(Int64 *) buffers[0];
    Int64      *tin1 =  (Int64 *) buffers[1];
    Int64      *tout0 =  (Int64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 | *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_or_NNxN_svxv_descr =
{ "bitwise_or_NNxN_svxv", (void *) bitwise_or_NNxN_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Int64) }, { 1, 0, 0, 0 } };
/*********************  bitwise_xor  *********************/

static int bitwise_xor_NNxN_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Int64       tin1 = *(Int64 *) buffers[1];
    Int64      *tout0 =  (Int64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 ^ tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_xor_NNxN_vsxv_descr =
{ "bitwise_xor_NNxN_vsxv", (void *) bitwise_xor_NNxN_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Int64) }, { 0, 1, 0, 0 } };

static int bitwise_xor_NNxN_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Int64      *tin1 =  (Int64 *) buffers[1];
    Int64      *tout0 =  (Int64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 ^ *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_xor_NNxN_vvxv_descr =
{ "bitwise_xor_NNxN_vvxv", (void *) bitwise_xor_NNxN_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Int64) }, { 0, 0, 0, 0 } };

static void _bitwise_xor_NxN_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int64  *tin0   = (Int64 *) ((char *) input  + inboffset);
    Int64 *tout0  = (Int64 *) ((char *) output + outboffset);
    Int64 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Int64 *) ((char *) tin0 + inbstrides[dim]);
            net    =     net ^ *tin0;
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _bitwise_xor_NxN_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int bitwise_xor_NxN_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _bitwise_xor_NxN_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(bitwise_xor_NxN_R, CHECK_ALIGN, sizeof(Int64), sizeof(Int64));

static void _bitwise_xor_NxN_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int64 *tin0   = (Int64 *) ((char *) input  + inboffset);
    Int64 *tout0 = (Int64 *) ((char *) output + outboffset);
    Int64 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Int64 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Int64 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = lastval ^ *tin0;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _bitwise_xor_NxN_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  bitwise_xor_NxN_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _bitwise_xor_NxN_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(bitwise_xor_NxN_A, CHECK_ALIGN, sizeof(Int64), sizeof(Int64));

static int bitwise_xor_NNxN_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64       tin0 = *(Int64 *) buffers[0];
    Int64      *tin1 =  (Int64 *) buffers[1];
    Int64      *tout0 =  (Int64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 ^ *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_xor_NNxN_svxv_descr =
{ "bitwise_xor_NNxN_svxv", (void *) bitwise_xor_NNxN_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Int64) }, { 1, 0, 0, 0 } };
/*********************  bitwise_not  *********************/

static int bitwise_not_NxN_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Int64      *tout0 =  (Int64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ~*tin0;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_not_NxN_vxv_descr =
{ "bitwise_not_NxN_vxv", (void *) bitwise_not_NxN_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int64), sizeof(Int64) }, { 0, 0, 0 } };
/*********************  bitwise_not  *********************/
/*********************  rshift  *********************/

static int rshift_NNxN_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Int64       tin1 = *(Int64 *) buffers[1];
    Int64      *tout0 =  (Int64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 >> tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor rshift_NNxN_vsxv_descr =
{ "rshift_NNxN_vsxv", (void *) rshift_NNxN_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Int64) }, { 0, 1, 0, 0 } };

static int rshift_NNxN_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Int64      *tin1 =  (Int64 *) buffers[1];
    Int64      *tout0 =  (Int64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 >> *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor rshift_NNxN_vvxv_descr =
{ "rshift_NNxN_vvxv", (void *) rshift_NNxN_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Int64) }, { 0, 0, 0, 0 } };

static int rshift_NNxN_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64       tin0 = *(Int64 *) buffers[0];
    Int64      *tin1 =  (Int64 *) buffers[1];
    Int64      *tout0 =  (Int64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 >> *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor rshift_NNxN_svxv_descr =
{ "rshift_NNxN_svxv", (void *) rshift_NNxN_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Int64) }, { 1, 0, 0, 0 } };
/*********************  lshift  *********************/

static int lshift_NNxN_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Int64       tin1 = *(Int64 *) buffers[1];
    Int64      *tout0 =  (Int64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 << tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor lshift_NNxN_vsxv_descr =
{ "lshift_NNxN_vsxv", (void *) lshift_NNxN_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Int64) }, { 0, 1, 0, 0 } };

static int lshift_NNxN_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Int64      *tin1 =  (Int64 *) buffers[1];
    Int64      *tout0 =  (Int64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 << *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor lshift_NNxN_vvxv_descr =
{ "lshift_NNxN_vvxv", (void *) lshift_NNxN_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Int64) }, { 0, 0, 0, 0 } };

static int lshift_NNxN_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64       tin0 = *(Int64 *) buffers[0];
    Int64      *tin1 =  (Int64 *) buffers[1];
    Int64      *tout0 =  (Int64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 << *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor lshift_NNxN_svxv_descr =
{ "lshift_NNxN_svxv", (void *) lshift_NNxN_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Int64) }, { 1, 0, 0, 0 } };
/*********************  floor  *********************/

static int floor_NxN_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Int64      *tout0 =  (Int64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = (*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor floor_NxN_vxf_descr =
{ "floor_NxN_vxf", (void *) floor_NxN_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int64), sizeof(Int64) }, { 0, 0, 0 } };
/*********************  floor  *********************/
/*********************  ceil  *********************/

static int ceil_NxN_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Int64      *tout0 =  (Int64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = (*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor ceil_NxN_vxf_descr =
{ "ceil_NxN_vxf", (void *) ceil_NxN_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int64), sizeof(Int64) }, { 0, 0, 0 } };
/*********************  ceil  *********************/
/*********************  maximum  *********************/

static int maximum_NNxN_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Int64       tin1 = *(Int64 *) buffers[1];
    Int64      *tout0 =  (Int64 *) buffers[2];
Int64 temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufmaximum(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor maximum_NNxN_vsxf_descr =
{ "maximum_NNxN_vsxf", (void *) maximum_NNxN_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Int64) }, { 0, 1, 0, 0 } };

static int maximum_NNxN_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Int64      *tin1 =  (Int64 *) buffers[1];
    Int64      *tout0 =  (Int64 *) buffers[2];
Int64 temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufmaximum(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor maximum_NNxN_vvxf_descr =
{ "maximum_NNxN_vvxf", (void *) maximum_NNxN_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Int64) }, { 0, 0, 0, 0 } };

static void _maximum_NxN_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int64  *tin0   = (Int64 *) ((char *) input  + inboffset);
    Int64 *tout0  = (Int64 *) ((char *) output + outboffset);
    Int64 net;
    Int64 temp1, temp2;
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Int64 *) ((char *) tin0 + inbstrides[dim]);
            net   = ufmaximum(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _maximum_NxN_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int maximum_NxN_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _maximum_NxN_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(maximum_NxN_R, CHECK_ALIGN, sizeof(Int64), sizeof(Int64));

static void _maximum_NxN_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int64 *tin0   = (Int64 *) ((char *) input  + inboffset);
    Int64 *tout0 = (Int64 *) ((char *) output + outboffset);
    Int64 lastval;
    Int64 temp1, temp2;
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Int64 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Int64 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = ufmaximum(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _maximum_NxN_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  maximum_NxN_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _maximum_NxN_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(maximum_NxN_A, CHECK_ALIGN, sizeof(Int64), sizeof(Int64));

static int maximum_NNxN_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64       tin0 = *(Int64 *) buffers[0];
    Int64      *tin1 =  (Int64 *) buffers[1];
    Int64      *tout0 =  (Int64 *) buffers[2];
Int64 temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufmaximum(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor maximum_NNxN_svxf_descr =
{ "maximum_NNxN_svxf", (void *) maximum_NNxN_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Int64) }, { 1, 0, 0, 0 } };
/*********************  minimum  *********************/

static int minimum_NNxN_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Int64       tin1 = *(Int64 *) buffers[1];
    Int64      *tout0 =  (Int64 *) buffers[2];
Int64 temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufminimum(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor minimum_NNxN_vsxf_descr =
{ "minimum_NNxN_vsxf", (void *) minimum_NNxN_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Int64) }, { 0, 1, 0, 0 } };

static int minimum_NNxN_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Int64      *tin1 =  (Int64 *) buffers[1];
    Int64      *tout0 =  (Int64 *) buffers[2];
Int64 temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufminimum(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor minimum_NNxN_vvxf_descr =
{ "minimum_NNxN_vvxf", (void *) minimum_NNxN_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Int64) }, { 0, 0, 0, 0 } };

static void _minimum_NxN_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int64  *tin0   = (Int64 *) ((char *) input  + inboffset);
    Int64 *tout0  = (Int64 *) ((char *) output + outboffset);
    Int64 net;
    Int64 temp1, temp2;
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Int64 *) ((char *) tin0 + inbstrides[dim]);
            net   = ufminimum(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _minimum_NxN_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int minimum_NxN_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _minimum_NxN_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(minimum_NxN_R, CHECK_ALIGN, sizeof(Int64), sizeof(Int64));

static void _minimum_NxN_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int64 *tin0   = (Int64 *) ((char *) input  + inboffset);
    Int64 *tout0 = (Int64 *) ((char *) output + outboffset);
    Int64 lastval;
    Int64 temp1, temp2;
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Int64 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Int64 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = ufminimum(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _minimum_NxN_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  minimum_NxN_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _minimum_NxN_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(minimum_NxN_A, CHECK_ALIGN, sizeof(Int64), sizeof(Int64));

static int minimum_NNxN_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64       tin0 = *(Int64 *) buffers[0];
    Int64      *tin1 =  (Int64 *) buffers[1];
    Int64      *tout0 =  (Int64 *) buffers[2];
Int64 temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufminimum(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor minimum_NNxN_svxf_descr =
{ "minimum_NNxN_svxf", (void *) minimum_NNxN_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Int64) }, { 1, 0, 0, 0 } };
/*********************  fabs  *********************/

static int fabs_Nxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = fabs(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor fabs_Nxd_vxf_descr =
{ "fabs_Nxd_vxf", (void *) fabs_Nxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int64), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  _round  *********************/

static int _round_Nxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_round(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor _round_Nxd_vxf_descr =
{ "_round_Nxd_vxf", (void *) _round_Nxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int64), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  hypot  *********************/

static int hypot_NNxd_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Int64       tin1 = *(Int64 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = hypot(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor hypot_NNxd_vsxf_descr =
{ "hypot_NNxd_vsxf", (void *) hypot_NNxd_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Float64) }, { 0, 1, 0, 0 } };

static int hypot_NNxd_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64      *tin0 =  (Int64 *) buffers[0];
    Int64      *tin1 =  (Int64 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = hypot(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor hypot_NNxd_vvxf_descr =
{ "hypot_NNxd_vvxf", (void *) hypot_NNxd_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Float64) }, { 0, 0, 0, 0 } };

static void _hypot_Nxd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int64  *tin0   = (Int64 *) ((char *) input  + inboffset);
    Float64 *tout0  = (Float64 *) ((char *) output + outboffset);
    Float64 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Int64 *) ((char *) tin0 + inbstrides[dim]);
            net   = hypot(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _hypot_Nxd_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int hypot_Nxd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _hypot_Nxd_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(hypot_Nxd_R, CHECK_ALIGN, sizeof(Int64), sizeof(Float64));

static void _hypot_Nxd_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int64 *tin0   = (Int64 *) ((char *) input  + inboffset);
    Float64 *tout0 = (Float64 *) ((char *) output + outboffset);
    Float64 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Int64 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Float64 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = hypot(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _hypot_Nxd_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  hypot_Nxd_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _hypot_Nxd_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(hypot_Nxd_A, CHECK_ALIGN, sizeof(Int64), sizeof(Float64));

static int hypot_NNxd_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int64       tin0 = *(Int64 *) buffers[0];
    Int64      *tin1 =  (Int64 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = hypot(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor hypot_NNxd_svxf_descr =
{ "hypot_NNxd_svxf", (void *) hypot_NNxd_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int64), sizeof(Int64), sizeof(Float64) }, { 1, 0, 0, 0 } };
/*********************  minus  *********************/
/*********************  add  *********************/
/*********************  subtract  *********************/
/*********************  multiply  *********************/
/*********************  divide  *********************/
/*********************  true_divide  *********************/
/*********************  remainder  *********************/
/*********************  power  *********************/
/*********************  abs  *********************/
/*********************  sin  *********************/
/*********************  cos  *********************/
/*********************  tan  *********************/
/*********************  arcsin  *********************/
/*********************  arccos  *********************/
/*********************  arctan  *********************/
/*********************  arcsinh  *********************/
/*********************  arccosh  *********************/
/*********************  arctanh  *********************/
/*********************  log  *********************/
/*********************  log10  *********************/
/*********************  exp  *********************/
/*********************  sinh  *********************/
/*********************  cosh  *********************/
/*********************  tanh  *********************/
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
/*********************  logical_not  *********************/
/*********************  floor  *********************/
/*********************  ceil  *********************/
/*********************  maximum  *********************/
/*********************  minimum  *********************/
/*********************  fabs  *********************/
/*********************  _round  *********************/
/*********************  hypot  *********************/

static PyMethodDef _ufuncInt64Methods[] = {

	{NULL,      NULL}        /* Sentinel */
};

static PyObject *init_funcDict(void) {
    PyObject *dict, *keytuple, *cfunc;
    dict = PyDict_New();
    /* minus_NxN_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","minus","v","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minus_NxN_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_NNxN_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","add","vs","Int64","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_NNxN_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_NNxN_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","add","vv","Int64","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_NNxN_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_NxN_R */
    keytuple=Py_BuildValue("ss((s)(s))","add","R","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_NxN_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_NxN_A */
    keytuple=Py_BuildValue("ss((s)(s))","add","A","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_NxN_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_NNxN_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","add","sv","Int64","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_NNxN_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_NNxN_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","subtract","vs","Int64","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_NNxN_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_NNxN_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","subtract","vv","Int64","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_NNxN_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_NxN_R */
    keytuple=Py_BuildValue("ss((s)(s))","subtract","R","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_NxN_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_NxN_A */
    keytuple=Py_BuildValue("ss((s)(s))","subtract","A","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_NxN_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_NNxN_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","subtract","sv","Int64","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_NNxN_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* multiply_NNxN_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","multiply","vs","Int64","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&multiply_NNxN_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* multiply_NNxN_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","multiply","vv","Int64","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&multiply_NNxN_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* multiply_NxN_R */
    keytuple=Py_BuildValue("ss((s)(s))","multiply","R","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&multiply_NxN_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* multiply_NxN_A */
    keytuple=Py_BuildValue("ss((s)(s))","multiply","A","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&multiply_NxN_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* multiply_NNxN_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","multiply","sv","Int64","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&multiply_NNxN_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_NNxN_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","divide","vs","Int64","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_NNxN_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_NNxN_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","divide","vv","Int64","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_NNxN_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_NxN_R */
    keytuple=Py_BuildValue("ss((s)(s))","divide","R","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_NxN_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_NxN_A */
    keytuple=Py_BuildValue("ss((s)(s))","divide","A","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_NxN_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_NNxN_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","divide","sv","Int64","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_NNxN_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_divide_NNxN_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","floor_divide","vs","Int64","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_divide_NNxN_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_divide_NNxN_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","floor_divide","vv","Int64","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_divide_NNxN_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_divide_NxN_R */
    keytuple=Py_BuildValue("ss((s)(s))","floor_divide","R","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_divide_NxN_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_divide_NxN_A */
    keytuple=Py_BuildValue("ss((s)(s))","floor_divide","A","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_divide_NxN_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_divide_NNxN_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","floor_divide","sv","Int64","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_divide_NNxN_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_NNxd_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","true_divide","vs","Int64","Int64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_NNxd_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_NNxd_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","true_divide","vv","Int64","Int64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_NNxd_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_Nxd_R */
    keytuple=Py_BuildValue("ss((s)(s))","true_divide","R","Int64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_Nxd_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_Nxd_A */
    keytuple=Py_BuildValue("ss((s)(s))","true_divide","A","Int64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_Nxd_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_NNxd_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","true_divide","sv","Int64","Int64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_NNxd_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_NNxN_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","remainder","vs","Int64","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_NNxN_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_NNxN_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","remainder","vv","Int64","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_NNxN_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_NxN_R */
    keytuple=Py_BuildValue("ss((s)(s))","remainder","R","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_NxN_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_NxN_A */
    keytuple=Py_BuildValue("ss((s)(s))","remainder","A","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_NxN_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_NNxN_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","remainder","sv","Int64","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_NNxN_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_NNxN_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","power","vs","Int64","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_NNxN_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_NNxN_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","power","vv","Int64","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_NNxN_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_NxN_R */
    keytuple=Py_BuildValue("ss((s)(s))","power","R","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_NxN_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_NxN_A */
    keytuple=Py_BuildValue("ss((s)(s))","power","A","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_NxN_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_NNxN_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","power","sv","Int64","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_NNxN_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* abs_NxN_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","abs","v","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&abs_NxN_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* sin_Nxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","sin","v","Int64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&sin_Nxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* cos_Nxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","cos","v","Int64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&cos_Nxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* tan_Nxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","tan","v","Int64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&tan_Nxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arcsin_Nxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arcsin","v","Int64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arcsin_Nxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arccos_Nxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arccos","v","Int64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arccos_Nxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan_Nxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arctan","v","Int64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan_Nxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan2_NNxd_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","arctan2","vs","Int64","Int64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan2_NNxd_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan2_NNxd_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","arctan2","vv","Int64","Int64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan2_NNxd_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan2_Nxd_R */
    keytuple=Py_BuildValue("ss((s)(s))","arctan2","R","Int64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan2_Nxd_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan2_Nxd_A */
    keytuple=Py_BuildValue("ss((s)(s))","arctan2","A","Int64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan2_Nxd_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan2_NNxd_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","arctan2","sv","Int64","Int64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan2_NNxd_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* log_Nxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","log","v","Int64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&log_Nxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* log10_Nxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","log10","v","Int64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&log10_Nxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* exp_Nxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","exp","v","Int64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&exp_Nxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* sinh_Nxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","sinh","v","Int64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&sinh_Nxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* cosh_Nxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","cosh","v","Int64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&cosh_Nxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* tanh_Nxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","tanh","v","Int64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&tanh_Nxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arcsinh_Nxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arcsinh","v","Int64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arcsinh_Nxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arccosh_Nxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arccosh","v","Int64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arccosh_Nxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctanh_Nxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arctanh","v","Int64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctanh_Nxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* sqrt_Nxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","sqrt","v","Int64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&sqrt_Nxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* equal_NNxB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","equal","vs","Int64","Int64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&equal_NNxB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* equal_NNxB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","equal","vv","Int64","Int64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&equal_NNxB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* equal_NNxB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","equal","sv","Int64","Int64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&equal_NNxB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* not_equal_NNxB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","not_equal","vs","Int64","Int64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&not_equal_NNxB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* not_equal_NNxB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","not_equal","vv","Int64","Int64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&not_equal_NNxB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* not_equal_NNxB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","not_equal","sv","Int64","Int64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&not_equal_NNxB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_NNxB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater","vs","Int64","Int64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_NNxB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_NNxB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater","vv","Int64","Int64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_NNxB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_NNxB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater","sv","Int64","Int64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_NNxB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_equal_NNxB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater_equal","vs","Int64","Int64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_equal_NNxB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_equal_NNxB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater_equal","vv","Int64","Int64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_equal_NNxB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_equal_NNxB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater_equal","sv","Int64","Int64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_equal_NNxB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_NNxB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less","vs","Int64","Int64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_NNxB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_NNxB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less","vv","Int64","Int64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_NNxB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_NNxB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less","sv","Int64","Int64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_NNxB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_equal_NNxB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less_equal","vs","Int64","Int64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_equal_NNxB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_equal_NNxB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less_equal","vv","Int64","Int64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_equal_NNxB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_equal_NNxB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less_equal","sv","Int64","Int64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_equal_NNxB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_and_NNxB_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_and","vs","Int64","Int64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_and_NNxB_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_and_NNxB_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_and","vv","Int64","Int64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_and_NNxB_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_and_NNxB_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_and","sv","Int64","Int64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_and_NNxB_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_or_NNxB_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_or","vs","Int64","Int64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_or_NNxB_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_or_NNxB_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_or","vv","Int64","Int64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_or_NNxB_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_or_NNxB_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_or","sv","Int64","Int64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_or_NNxB_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_xor_NNxB_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_xor","vs","Int64","Int64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_xor_NNxB_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_xor_NNxB_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_xor","vv","Int64","Int64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_xor_NNxB_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_xor_NNxB_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_xor","sv","Int64","Int64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_xor_NNxB_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_not_NxB_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","logical_not","v","Int64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_not_NxB_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_and_NNxN_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_and","vs","Int64","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_and_NNxN_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_and_NNxN_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_and","vv","Int64","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_and_NNxN_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_and_NxN_R */
    keytuple=Py_BuildValue("ss((s)(s))","bitwise_and","R","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_and_NxN_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_and_NxN_A */
    keytuple=Py_BuildValue("ss((s)(s))","bitwise_and","A","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_and_NxN_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_and_NNxN_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_and","sv","Int64","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_and_NNxN_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_or_NNxN_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_or","vs","Int64","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_or_NNxN_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_or_NNxN_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_or","vv","Int64","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_or_NNxN_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_or_NxN_R */
    keytuple=Py_BuildValue("ss((s)(s))","bitwise_or","R","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_or_NxN_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_or_NxN_A */
    keytuple=Py_BuildValue("ss((s)(s))","bitwise_or","A","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_or_NxN_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_or_NNxN_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_or","sv","Int64","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_or_NNxN_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_xor_NNxN_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_xor","vs","Int64","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_xor_NNxN_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_xor_NNxN_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_xor","vv","Int64","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_xor_NNxN_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_xor_NxN_R */
    keytuple=Py_BuildValue("ss((s)(s))","bitwise_xor","R","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_xor_NxN_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_xor_NxN_A */
    keytuple=Py_BuildValue("ss((s)(s))","bitwise_xor","A","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_xor_NxN_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_xor_NNxN_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_xor","sv","Int64","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_xor_NNxN_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_not_NxN_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","bitwise_not","v","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_not_NxN_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* rshift_NNxN_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","rshift","vs","Int64","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&rshift_NNxN_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* rshift_NNxN_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","rshift","vv","Int64","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&rshift_NNxN_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* rshift_NNxN_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","rshift","sv","Int64","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&rshift_NNxN_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* lshift_NNxN_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","lshift","vs","Int64","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&lshift_NNxN_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* lshift_NNxN_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","lshift","vv","Int64","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&lshift_NNxN_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* lshift_NNxN_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","lshift","sv","Int64","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&lshift_NNxN_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_NxN_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","floor","v","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_NxN_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* ceil_NxN_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","ceil","v","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&ceil_NxN_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_NNxN_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","maximum","vs","Int64","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_NNxN_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_NNxN_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","maximum","vv","Int64","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_NNxN_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_NxN_R */
    keytuple=Py_BuildValue("ss((s)(s))","maximum","R","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_NxN_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_NxN_A */
    keytuple=Py_BuildValue("ss((s)(s))","maximum","A","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_NxN_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_NNxN_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","maximum","sv","Int64","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_NNxN_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_NNxN_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","minimum","vs","Int64","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_NNxN_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_NNxN_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","minimum","vv","Int64","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_NNxN_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_NxN_R */
    keytuple=Py_BuildValue("ss((s)(s))","minimum","R","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_NxN_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_NxN_A */
    keytuple=Py_BuildValue("ss((s)(s))","minimum","A","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_NxN_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_NNxN_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","minimum","sv","Int64","Int64","Int64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_NNxN_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* fabs_Nxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","fabs","v","Int64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&fabs_Nxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* _round_Nxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","_round","v","Int64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&_round_Nxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_NNxd_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","hypot","vs","Int64","Int64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_NNxd_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_NNxd_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","hypot","vv","Int64","Int64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_NNxd_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_Nxd_R */
    keytuple=Py_BuildValue("ss((s)(s))","hypot","R","Int64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_Nxd_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_Nxd_A */
    keytuple=Py_BuildValue("ss((s)(s))","hypot","A","Int64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_Nxd_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_NNxd_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","hypot","sv","Int64","Int64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_NNxd_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    return dict;
}

/* platform independent*/
#ifdef MS_WIN32
__declspec(dllexport)
#endif
void init_ufuncInt64(void) {
    PyObject *m, *d, *functions;
    m = Py_InitModule("_ufuncInt64", _ufuncInt64Methods);
    d = PyModule_GetDict(m);
    import_libnumarray();
    functions = init_funcDict();
    PyDict_SetItemString(d, "functionDict", functions);
    Py_DECREF(functions);
    ADD_VERSION(m);
}
