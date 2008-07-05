
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

static int minus_UxU_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    UInt64     *tout0 =  (UInt64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = -*tin0;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor minus_UxU_vxv_descr =
{ "minus_UxU_vxv", (void *) minus_UxU_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt64), sizeof(UInt64) }, { 0, 0, 0 } };
/*********************  add  *********************/

static int add_UUxU_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    UInt64      tin1 = *(UInt64 *) buffers[1];
    UInt64     *tout0 =  (UInt64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 + tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor add_UUxU_vsxv_descr =
{ "add_UUxU_vsxv", (void *) add_UUxU_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(UInt64) }, { 0, 1, 0, 0 } };

static int add_UUxU_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    UInt64     *tin1 =  (UInt64 *) buffers[1];
    UInt64     *tout0 =  (UInt64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 + *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor add_UUxU_vvxv_descr =
{ "add_UUxU_vvxv", (void *) add_UUxU_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(UInt64) }, { 0, 0, 0, 0 } };

static void _add_UxU_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt64  *tin0   = (UInt64 *) ((char *) input  + inboffset);
    UInt64 *tout0  = (UInt64 *) ((char *) output + outboffset);
    UInt64 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (UInt64 *) ((char *) tin0 + inbstrides[dim]);
            net    =     net + *tin0;
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _add_UxU_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int add_UxU_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _add_UxU_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(add_UxU_R, CHECK_ALIGN, sizeof(UInt64), sizeof(UInt64));

static void _add_UxU_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt64 *tin0   = (UInt64 *) ((char *) input  + inboffset);
    UInt64 *tout0 = (UInt64 *) ((char *) output + outboffset);
    UInt64 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (UInt64 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (UInt64 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = lastval + *tin0;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _add_UxU_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  add_UxU_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _add_UxU_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(add_UxU_A, CHECK_ALIGN, sizeof(UInt64), sizeof(UInt64));

static int add_UUxU_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64      tin0 = *(UInt64 *) buffers[0];
    UInt64     *tin1 =  (UInt64 *) buffers[1];
    UInt64     *tout0 =  (UInt64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 + *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor add_UUxU_svxv_descr =
{ "add_UUxU_svxv", (void *) add_UUxU_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(UInt64) }, { 1, 0, 0, 0 } };
/*********************  subtract  *********************/

static int subtract_UUxU_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    UInt64      tin1 = *(UInt64 *) buffers[1];
    UInt64     *tout0 =  (UInt64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 - tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor subtract_UUxU_vsxv_descr =
{ "subtract_UUxU_vsxv", (void *) subtract_UUxU_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(UInt64) }, { 0, 1, 0, 0 } };

static int subtract_UUxU_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    UInt64     *tin1 =  (UInt64 *) buffers[1];
    UInt64     *tout0 =  (UInt64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 - *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor subtract_UUxU_vvxv_descr =
{ "subtract_UUxU_vvxv", (void *) subtract_UUxU_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(UInt64) }, { 0, 0, 0, 0 } };

static void _subtract_UxU_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt64  *tin0   = (UInt64 *) ((char *) input  + inboffset);
    UInt64 *tout0  = (UInt64 *) ((char *) output + outboffset);
    UInt64 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (UInt64 *) ((char *) tin0 + inbstrides[dim]);
            net    =     net - *tin0;
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _subtract_UxU_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int subtract_UxU_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _subtract_UxU_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(subtract_UxU_R, CHECK_ALIGN, sizeof(UInt64), sizeof(UInt64));

static void _subtract_UxU_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt64 *tin0   = (UInt64 *) ((char *) input  + inboffset);
    UInt64 *tout0 = (UInt64 *) ((char *) output + outboffset);
    UInt64 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (UInt64 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (UInt64 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = lastval - *tin0;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _subtract_UxU_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  subtract_UxU_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _subtract_UxU_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(subtract_UxU_A, CHECK_ALIGN, sizeof(UInt64), sizeof(UInt64));

static int subtract_UUxU_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64      tin0 = *(UInt64 *) buffers[0];
    UInt64     *tin1 =  (UInt64 *) buffers[1];
    UInt64     *tout0 =  (UInt64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 - *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor subtract_UUxU_svxv_descr =
{ "subtract_UUxU_svxv", (void *) subtract_UUxU_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(UInt64) }, { 1, 0, 0, 0 } };
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/

static int multiply_UUxU_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    UInt64      tin1 = *(UInt64 *) buffers[1];
    UInt64     *tout0 =  (UInt64 *) buffers[2];
Int64 temp;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        temp = ((UInt64) *tin0) * ((UInt64) tin1);
    if (umult64_overflow(*tin0, tin1)) temp = (Float64) int_overflow_error(9223372036854775807.);
    *tout0 = (UInt64) temp;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor multiply_UUxU_vsxv_descr =
{ "multiply_UUxU_vsxv", (void *) multiply_UUxU_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(UInt64) }, { 0, 1, 0, 0 } };

static int multiply_UUxU_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    UInt64     *tin1 =  (UInt64 *) buffers[1];
    UInt64     *tout0 =  (UInt64 *) buffers[2];
Int64 temp;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        temp = ((UInt64) *tin0) * ((UInt64) *tin1);
    if (umult64_overflow(*tin0, *tin1)) temp = (Float64) int_overflow_error(9223372036854775807.);
    *tout0 = (UInt64) temp;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor multiply_UUxU_vvxv_descr =
{ "multiply_UUxU_vvxv", (void *) multiply_UUxU_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(UInt64) }, { 0, 0, 0, 0 } };

static void _multiply_UxU_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt64  *tin0   = (UInt64 *) ((char *) input  + inboffset);
    UInt64 *tout0  = (UInt64 *) ((char *) output + outboffset);
    UInt64 net;
    Int64 temp;
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (UInt64 *) ((char *) tin0 + inbstrides[dim]);
            temp = ((UInt64) net) * ((UInt64) *tin0);
    if (umult64_overflow(net, *tin0)) temp = (Float64) int_overflow_error(9223372036854775807.);
    net = (UInt64) temp;
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _multiply_UxU_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int multiply_UxU_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _multiply_UxU_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(multiply_UxU_R, CHECK_ALIGN, sizeof(UInt64), sizeof(UInt64));

static void _multiply_UxU_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt64 *tin0   = (UInt64 *) ((char *) input  + inboffset);
    UInt64 *tout0 = (UInt64 *) ((char *) output + outboffset);
    UInt64 lastval;
    Int64 temp;
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (UInt64 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (UInt64 *) ((char *) tout0 + outbstrides[dim]);
            temp = ((UInt64) lastval) * ((UInt64) *tin0);
    if (umult64_overflow(lastval, *tin0)) temp = (Float64) int_overflow_error(9223372036854775807.);
    *tout0 = (UInt64) temp;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _multiply_UxU_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  multiply_UxU_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _multiply_UxU_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(multiply_UxU_A, CHECK_ALIGN, sizeof(UInt64), sizeof(UInt64));

static int multiply_UUxU_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64      tin0 = *(UInt64 *) buffers[0];
    UInt64     *tin1 =  (UInt64 *) buffers[1];
    UInt64     *tout0 =  (UInt64 *) buffers[2];
Int64 temp;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        temp = ((UInt64) tin0) * ((UInt64) *tin1);
    if (umult64_overflow(tin0, *tin1)) temp = (Float64) int_overflow_error(9223372036854775807.);
    *tout0 = (UInt64) temp;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor multiply_UUxU_svxv_descr =
{ "multiply_UUxU_svxv", (void *) multiply_UUxU_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(UInt64) }, { 1, 0, 0, 0 } };
/*********************  multiply  *********************/
/*********************  divide  *********************/

static int divide_UUxU_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    UInt64      tin1 = *(UInt64 *) buffers[1];
    UInt64     *tout0 =  (UInt64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((tin1==0) ? int_dividebyzero_error(tin1, *tin0) : *tin0 / tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor divide_UUxU_vsxv_descr =
{ "divide_UUxU_vsxv", (void *) divide_UUxU_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(UInt64) }, { 0, 1, 0, 0 } };

static int divide_UUxU_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    UInt64     *tin1 =  (UInt64 *) buffers[1];
    UInt64     *tout0 =  (UInt64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, *tin0) : *tin0 / *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor divide_UUxU_vvxv_descr =
{ "divide_UUxU_vvxv", (void *) divide_UUxU_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(UInt64) }, { 0, 0, 0, 0 } };

static void _divide_UxU_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt64  *tin0   = (UInt64 *) ((char *) input  + inboffset);
    UInt64 *tout0  = (UInt64 *) ((char *) output + outboffset);
    UInt64 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (UInt64 *) ((char *) tin0 + inbstrides[dim]);
            net = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0) : net / *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _divide_UxU_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int divide_UxU_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _divide_UxU_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(divide_UxU_R, CHECK_ALIGN, sizeof(UInt64), sizeof(UInt64));

static void _divide_UxU_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt64 *tin0   = (UInt64 *) ((char *) input  + inboffset);
    UInt64 *tout0 = (UInt64 *) ((char *) output + outboffset);
    UInt64 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (UInt64 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (UInt64 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0) : lastval / *tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _divide_UxU_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  divide_UxU_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _divide_UxU_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(divide_UxU_A, CHECK_ALIGN, sizeof(UInt64), sizeof(UInt64));

static int divide_UUxU_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64      tin0 = *(UInt64 *) buffers[0];
    UInt64     *tin1 =  (UInt64 *) buffers[1];
    UInt64     *tout0 =  (UInt64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, 0) : tin0 / *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor divide_UUxU_svxv_descr =
{ "divide_UUxU_svxv", (void *) divide_UUxU_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(UInt64) }, { 1, 0, 0, 0 } };
/*********************  divide  *********************/
/*********************  floor_divide  *********************/

static int floor_divide_UUxU_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    UInt64      tin1 = *(UInt64 *) buffers[1];
    UInt64     *tout0 =  (UInt64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = (( tin1==0) ? int_dividebyzero_error( tin1, *tin0) : floor(*tin0   / (double)  tin1));
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor floor_divide_UUxU_vsxv_descr =
{ "floor_divide_UUxU_vsxv", (void *) floor_divide_UUxU_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(UInt64) }, { 0, 1, 0, 0 } };

static int floor_divide_UUxU_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    UInt64     *tin1 =  (UInt64 *) buffers[1];
    UInt64     *tout0 =  (UInt64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, *tin0) : floor(*tin0   / (double) *tin1));
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor floor_divide_UUxU_vvxv_descr =
{ "floor_divide_UUxU_vvxv", (void *) floor_divide_UUxU_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(UInt64) }, { 0, 0, 0, 0 } };

static void _floor_divide_UxU_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt64  *tin0   = (UInt64 *) ((char *) input  + inboffset);
    UInt64 *tout0  = (UInt64 *) ((char *) output + outboffset);
    UInt64 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (UInt64 *) ((char *) tin0 + inbstrides[dim]);
            net    = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0)     : floor(net     / (double) *tin0));
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _floor_divide_UxU_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int floor_divide_UxU_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _floor_divide_UxU_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(floor_divide_UxU_R, CHECK_ALIGN, sizeof(UInt64), sizeof(UInt64));

static void _floor_divide_UxU_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt64 *tin0   = (UInt64 *) ((char *) input  + inboffset);
    UInt64 *tout0 = (UInt64 *) ((char *) output + outboffset);
    UInt64 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (UInt64 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (UInt64 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0)     : floor(lastval / (double) *tin0));
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _floor_divide_UxU_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  floor_divide_UxU_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _floor_divide_UxU_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(floor_divide_UxU_A, CHECK_ALIGN, sizeof(UInt64), sizeof(UInt64));

static int floor_divide_UUxU_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64      tin0 = *(UInt64 *) buffers[0];
    UInt64     *tin1 =  (UInt64 *) buffers[1];
    UInt64     *tout0 =  (UInt64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, 0)     : floor(tin0    / (double) *tin1));
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor floor_divide_UUxU_svxv_descr =
{ "floor_divide_UUxU_svxv", (void *) floor_divide_UUxU_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(UInt64) }, { 1, 0, 0, 0 } };
/*********************  floor_divide  *********************/
/*********************  true_divide  *********************/

static int true_divide_UUxd_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    UInt64      tin1 = *(UInt64 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = (( tin1==0) ? int_dividebyzero_error( tin1, *tin0) : *tin0         / (double) tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor true_divide_UUxd_vsxv_descr =
{ "true_divide_UUxd_vsxv", (void *) true_divide_UUxd_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(Float64) }, { 0, 1, 0, 0 } };

static int true_divide_UUxd_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    UInt64     *tin1 =  (UInt64 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, *tin0) : *tin0         / (double) *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor true_divide_UUxd_vvxv_descr =
{ "true_divide_UUxd_vvxv", (void *) true_divide_UUxd_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(Float64) }, { 0, 0, 0, 0 } };

static void _true_divide_Uxd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt64  *tin0   = (UInt64 *) ((char *) input  + inboffset);
    Float64 *tout0  = (Float64 *) ((char *) output + outboffset);
    Float64 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (UInt64 *) ((char *) tin0 + inbstrides[dim]);
            net    = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0)     : net           / (double) *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _true_divide_Uxd_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int true_divide_Uxd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _true_divide_Uxd_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(true_divide_Uxd_R, CHECK_ALIGN, sizeof(UInt64), sizeof(Float64));

static void _true_divide_Uxd_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt64 *tin0   = (UInt64 *) ((char *) input  + inboffset);
    Float64 *tout0 = (Float64 *) ((char *) output + outboffset);
    Float64 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (UInt64 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Float64 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0)     : lastval       / (double) *tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _true_divide_Uxd_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  true_divide_Uxd_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _true_divide_Uxd_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(true_divide_Uxd_A, CHECK_ALIGN, sizeof(UInt64), sizeof(Float64));

static int true_divide_UUxd_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64      tin0 = *(UInt64 *) buffers[0];
    UInt64     *tin1 =  (UInt64 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, 0)     : tin0          / (double) *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor true_divide_UUxd_svxv_descr =
{ "true_divide_UUxd_svxv", (void *) true_divide_UUxd_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(Float64) }, { 1, 0, 0, 0 } };
/*********************  true_divide  *********************/
/*********************  remainder  *********************/

static int remainder_UUxU_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    UInt64      tin1 = *(UInt64 *) buffers[1];
    UInt64     *tout0 =  (UInt64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((tin1==0) ? int_dividebyzero_error(tin1, *tin0) : *tin0  %  tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor remainder_UUxU_vsxv_descr =
{ "remainder_UUxU_vsxv", (void *) remainder_UUxU_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(UInt64) }, { 0, 1, 0, 0 } };

static int remainder_UUxU_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    UInt64     *tin1 =  (UInt64 *) buffers[1];
    UInt64     *tout0 =  (UInt64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, *tin0) : *tin0  %  *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor remainder_UUxU_vvxv_descr =
{ "remainder_UUxU_vvxv", (void *) remainder_UUxU_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(UInt64) }, { 0, 0, 0, 0 } };

static void _remainder_UxU_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt64  *tin0   = (UInt64 *) ((char *) input  + inboffset);
    UInt64 *tout0  = (UInt64 *) ((char *) output + outboffset);
    UInt64 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (UInt64 *) ((char *) tin0 + inbstrides[dim]);
            net = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0) : net  %  *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _remainder_UxU_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int remainder_UxU_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _remainder_UxU_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(remainder_UxU_R, CHECK_ALIGN, sizeof(UInt64), sizeof(UInt64));

static void _remainder_UxU_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt64 *tin0   = (UInt64 *) ((char *) input  + inboffset);
    UInt64 *tout0 = (UInt64 *) ((char *) output + outboffset);
    UInt64 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (UInt64 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (UInt64 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0) : lastval  %  *tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _remainder_UxU_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  remainder_UxU_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _remainder_UxU_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(remainder_UxU_A, CHECK_ALIGN, sizeof(UInt64), sizeof(UInt64));

static int remainder_UUxU_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64      tin0 = *(UInt64 *) buffers[0];
    UInt64     *tin1 =  (UInt64 *) buffers[1];
    UInt64     *tout0 =  (UInt64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, 0) : tin0  %  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor remainder_UUxU_svxv_descr =
{ "remainder_UUxU_svxv", (void *) remainder_UUxU_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(UInt64) }, { 1, 0, 0, 0 } };
/*********************  remainder  *********************/
/*********************  power  *********************/

static int power_UUxU_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    UInt64      tin1 = *(UInt64 *) buffers[1];
    UInt64     *tout0 =  (UInt64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_pow(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor power_UUxU_vsxf_descr =
{ "power_UUxU_vsxf", (void *) power_UUxU_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(UInt64) }, { 0, 1, 0, 0 } };

static int power_UUxU_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    UInt64     *tin1 =  (UInt64 *) buffers[1];
    UInt64     *tout0 =  (UInt64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_pow(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor power_UUxU_vvxf_descr =
{ "power_UUxU_vvxf", (void *) power_UUxU_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(UInt64) }, { 0, 0, 0, 0 } };

static void _power_UxU_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt64  *tin0   = (UInt64 *) ((char *) input  + inboffset);
    UInt64 *tout0  = (UInt64 *) ((char *) output + outboffset);
    UInt64 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (UInt64 *) ((char *) tin0 + inbstrides[dim]);
            net   = num_pow(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _power_UxU_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int power_UxU_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _power_UxU_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(power_UxU_R, CHECK_ALIGN, sizeof(UInt64), sizeof(UInt64));

static void _power_UxU_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt64 *tin0   = (UInt64 *) ((char *) input  + inboffset);
    UInt64 *tout0 = (UInt64 *) ((char *) output + outboffset);
    UInt64 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (UInt64 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (UInt64 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = num_pow(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _power_UxU_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  power_UxU_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _power_UxU_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(power_UxU_A, CHECK_ALIGN, sizeof(UInt64), sizeof(UInt64));

static int power_UUxU_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64      tin0 = *(UInt64 *) buffers[0];
    UInt64     *tin1 =  (UInt64 *) buffers[1];
    UInt64     *tout0 =  (UInt64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_pow(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor power_UUxU_svxf_descr =
{ "power_UUxU_svxf", (void *) power_UUxU_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(UInt64) }, { 1, 0, 0, 0 } };
/*********************  abs  *********************/

static int abs_UxU_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    UInt64     *tout0 =  (UInt64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = fabs(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor abs_UxU_vxf_descr =
{ "abs_UxU_vxf", (void *) abs_UxU_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt64), sizeof(UInt64) }, { 0, 0, 0 } };
/*********************  sin  *********************/

static int sin_Uxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = sin(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor sin_Uxd_vxf_descr =
{ "sin_Uxd_vxf", (void *) sin_Uxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt64), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  cos  *********************/

static int cos_Uxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = cos(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor cos_Uxd_vxf_descr =
{ "cos_Uxd_vxf", (void *) cos_Uxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt64), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  tan  *********************/

static int tan_Uxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = tan(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor tan_Uxd_vxf_descr =
{ "tan_Uxd_vxf", (void *) tan_Uxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt64), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arcsin  *********************/

static int arcsin_Uxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = asin(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arcsin_Uxd_vxf_descr =
{ "arcsin_Uxd_vxf", (void *) arcsin_Uxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt64), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arccos  *********************/

static int arccos_Uxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = acos(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arccos_Uxd_vxf_descr =
{ "arccos_Uxd_vxf", (void *) arccos_Uxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt64), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arctan  *********************/

static int arctan_Uxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = atan(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arctan_Uxd_vxf_descr =
{ "arctan_Uxd_vxf", (void *) arctan_Uxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt64), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arctan2  *********************/

static int arctan2_UUxd_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    UInt64      tin1 = *(UInt64 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = atan2(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arctan2_UUxd_vsxf_descr =
{ "arctan2_UUxd_vsxf", (void *) arctan2_UUxd_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(Float64) }, { 0, 1, 0, 0 } };

static int arctan2_UUxd_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    UInt64     *tin1 =  (UInt64 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = atan2(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arctan2_UUxd_vvxf_descr =
{ "arctan2_UUxd_vvxf", (void *) arctan2_UUxd_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(Float64) }, { 0, 0, 0, 0 } };

static void _arctan2_Uxd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt64  *tin0   = (UInt64 *) ((char *) input  + inboffset);
    Float64 *tout0  = (Float64 *) ((char *) output + outboffset);
    Float64 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (UInt64 *) ((char *) tin0 + inbstrides[dim]);
            net   = atan2(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _arctan2_Uxd_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int arctan2_Uxd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _arctan2_Uxd_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(arctan2_Uxd_R, CHECK_ALIGN, sizeof(UInt64), sizeof(Float64));

static void _arctan2_Uxd_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt64 *tin0   = (UInt64 *) ((char *) input  + inboffset);
    Float64 *tout0 = (Float64 *) ((char *) output + outboffset);
    Float64 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (UInt64 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Float64 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = atan2(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _arctan2_Uxd_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  arctan2_Uxd_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _arctan2_Uxd_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(arctan2_Uxd_A, CHECK_ALIGN, sizeof(UInt64), sizeof(Float64));

static int arctan2_UUxd_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64      tin0 = *(UInt64 *) buffers[0];
    UInt64     *tin1 =  (UInt64 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = atan2(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arctan2_UUxd_svxf_descr =
{ "arctan2_UUxd_svxf", (void *) arctan2_UUxd_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(Float64) }, { 1, 0, 0, 0 } };
/*********************  log  *********************/

static int log_Uxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_log(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor log_Uxd_vxf_descr =
{ "log_Uxd_vxf", (void *) log_Uxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt64), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  log10  *********************/

static int log10_Uxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_log10(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor log10_Uxd_vxf_descr =
{ "log10_Uxd_vxf", (void *) log10_Uxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt64), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  exp  *********************/

static int exp_Uxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = exp(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor exp_Uxd_vxf_descr =
{ "exp_Uxd_vxf", (void *) exp_Uxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt64), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  sinh  *********************/

static int sinh_Uxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = sinh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor sinh_Uxd_vxf_descr =
{ "sinh_Uxd_vxf", (void *) sinh_Uxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt64), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  cosh  *********************/

static int cosh_Uxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = cosh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor cosh_Uxd_vxf_descr =
{ "cosh_Uxd_vxf", (void *) cosh_Uxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt64), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  tanh  *********************/

static int tanh_Uxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = tanh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor tanh_Uxd_vxf_descr =
{ "tanh_Uxd_vxf", (void *) tanh_Uxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt64), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arcsinh  *********************/

static int arcsinh_Uxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_asinh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arcsinh_Uxd_vxf_descr =
{ "arcsinh_Uxd_vxf", (void *) arcsinh_Uxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt64), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arccosh  *********************/

static int arccosh_Uxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_acosh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arccosh_Uxd_vxf_descr =
{ "arccosh_Uxd_vxf", (void *) arccosh_Uxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt64), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arctanh  *********************/

static int arctanh_Uxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_atanh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arctanh_Uxd_vxf_descr =
{ "arctanh_Uxd_vxf", (void *) arctanh_Uxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt64), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  ieeemask  *********************/
/*********************  ieeemask  *********************/
/*********************  isnan  *********************/
/*********************  isnan  *********************/
/*********************  isnan  *********************/
/*********************  isnan  *********************/
/*********************  sqrt  *********************/

static int sqrt_Uxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = sqrt(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor sqrt_Uxd_vxf_descr =
{ "sqrt_Uxd_vxf", (void *) sqrt_Uxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt64), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  equal  *********************/

static int equal_UUxB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    UInt64      tin1 = *(UInt64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 == tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor equal_UUxB_vsxv_descr =
{ "equal_UUxB_vsxv", (void *) equal_UUxB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int equal_UUxB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    UInt64     *tin1 =  (UInt64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 == *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor equal_UUxB_vvxv_descr =
{ "equal_UUxB_vvxv", (void *) equal_UUxB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int equal_UUxB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64      tin0 = *(UInt64 *) buffers[0];
    UInt64     *tin1 =  (UInt64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 == *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor equal_UUxB_svxv_descr =
{ "equal_UUxB_svxv", (void *) equal_UUxB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  not_equal  *********************/

static int not_equal_UUxB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    UInt64      tin1 = *(UInt64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 != tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor not_equal_UUxB_vsxv_descr =
{ "not_equal_UUxB_vsxv", (void *) not_equal_UUxB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int not_equal_UUxB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    UInt64     *tin1 =  (UInt64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 != *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor not_equal_UUxB_vvxv_descr =
{ "not_equal_UUxB_vvxv", (void *) not_equal_UUxB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int not_equal_UUxB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64      tin0 = *(UInt64 *) buffers[0];
    UInt64     *tin1 =  (UInt64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 != *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor not_equal_UUxB_svxv_descr =
{ "not_equal_UUxB_svxv", (void *) not_equal_UUxB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  greater  *********************/

static int greater_UUxB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    UInt64      tin1 = *(UInt64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 > tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_UUxB_vsxv_descr =
{ "greater_UUxB_vsxv", (void *) greater_UUxB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int greater_UUxB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    UInt64     *tin1 =  (UInt64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 > *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_UUxB_vvxv_descr =
{ "greater_UUxB_vvxv", (void *) greater_UUxB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int greater_UUxB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64      tin0 = *(UInt64 *) buffers[0];
    UInt64     *tin1 =  (UInt64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 > *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_UUxB_svxv_descr =
{ "greater_UUxB_svxv", (void *) greater_UUxB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  greater_equal  *********************/

static int greater_equal_UUxB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    UInt64      tin1 = *(UInt64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 >= tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_equal_UUxB_vsxv_descr =
{ "greater_equal_UUxB_vsxv", (void *) greater_equal_UUxB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int greater_equal_UUxB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    UInt64     *tin1 =  (UInt64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 >= *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_equal_UUxB_vvxv_descr =
{ "greater_equal_UUxB_vvxv", (void *) greater_equal_UUxB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int greater_equal_UUxB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64      tin0 = *(UInt64 *) buffers[0];
    UInt64     *tin1 =  (UInt64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 >= *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_equal_UUxB_svxv_descr =
{ "greater_equal_UUxB_svxv", (void *) greater_equal_UUxB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  less  *********************/

static int less_UUxB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    UInt64      tin1 = *(UInt64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 < tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_UUxB_vsxv_descr =
{ "less_UUxB_vsxv", (void *) less_UUxB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int less_UUxB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    UInt64     *tin1 =  (UInt64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 < *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_UUxB_vvxv_descr =
{ "less_UUxB_vvxv", (void *) less_UUxB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int less_UUxB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64      tin0 = *(UInt64 *) buffers[0];
    UInt64     *tin1 =  (UInt64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 < *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_UUxB_svxv_descr =
{ "less_UUxB_svxv", (void *) less_UUxB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  less_equal  *********************/

static int less_equal_UUxB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    UInt64      tin1 = *(UInt64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 <= tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_equal_UUxB_vsxv_descr =
{ "less_equal_UUxB_vsxv", (void *) less_equal_UUxB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int less_equal_UUxB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    UInt64     *tin1 =  (UInt64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 <= *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_equal_UUxB_vvxv_descr =
{ "less_equal_UUxB_vvxv", (void *) less_equal_UUxB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int less_equal_UUxB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64      tin0 = *(UInt64 *) buffers[0];
    UInt64     *tin1 =  (UInt64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 <= *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_equal_UUxB_svxv_descr =
{ "less_equal_UUxB_svxv", (void *) less_equal_UUxB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  logical_and  *********************/

static int logical_and_UUxB_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    UInt64      tin1 = *(UInt64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_and(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_and_UUxB_vsxf_descr =
{ "logical_and_UUxB_vsxf", (void *) logical_and_UUxB_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int logical_and_UUxB_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    UInt64     *tin1 =  (UInt64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_and(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_and_UUxB_vvxf_descr =
{ "logical_and_UUxB_vvxf", (void *) logical_and_UUxB_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int logical_and_UUxB_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64      tin0 = *(UInt64 *) buffers[0];
    UInt64     *tin1 =  (UInt64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_and(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_and_UUxB_svxf_descr =
{ "logical_and_UUxB_svxf", (void *) logical_and_UUxB_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  logical_or  *********************/

static int logical_or_UUxB_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    UInt64      tin1 = *(UInt64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_or(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_or_UUxB_vsxf_descr =
{ "logical_or_UUxB_vsxf", (void *) logical_or_UUxB_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int logical_or_UUxB_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    UInt64     *tin1 =  (UInt64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_or(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_or_UUxB_vvxf_descr =
{ "logical_or_UUxB_vvxf", (void *) logical_or_UUxB_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int logical_or_UUxB_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64      tin0 = *(UInt64 *) buffers[0];
    UInt64     *tin1 =  (UInt64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_or(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_or_UUxB_svxf_descr =
{ "logical_or_UUxB_svxf", (void *) logical_or_UUxB_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  logical_xor  *********************/

static int logical_xor_UUxB_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    UInt64      tin1 = *(UInt64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_xor(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_xor_UUxB_vsxf_descr =
{ "logical_xor_UUxB_vsxf", (void *) logical_xor_UUxB_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int logical_xor_UUxB_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    UInt64     *tin1 =  (UInt64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_xor(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_xor_UUxB_vvxf_descr =
{ "logical_xor_UUxB_vvxf", (void *) logical_xor_UUxB_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int logical_xor_UUxB_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64      tin0 = *(UInt64 *) buffers[0];
    UInt64     *tin1 =  (UInt64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_xor(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_xor_UUxB_svxf_descr =
{ "logical_xor_UUxB_svxf", (void *) logical_xor_UUxB_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  logical_and  *********************/
/*********************  logical_or  *********************/
/*********************  logical_xor  *********************/
/*********************  logical_not  *********************/

static int logical_not_UxB_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    Bool       *tout0 =  (Bool *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = !*tin0;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_not_UxB_vxv_descr =
{ "logical_not_UxB_vxv", (void *) logical_not_UxB_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt64), sizeof(Bool) }, { 0, 0, 0 } };
/*********************  bitwise_and  *********************/

static int bitwise_and_UUxU_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    UInt64      tin1 = *(UInt64 *) buffers[1];
    UInt64     *tout0 =  (UInt64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 & tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_and_UUxU_vsxv_descr =
{ "bitwise_and_UUxU_vsxv", (void *) bitwise_and_UUxU_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(UInt64) }, { 0, 1, 0, 0 } };

static int bitwise_and_UUxU_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    UInt64     *tin1 =  (UInt64 *) buffers[1];
    UInt64     *tout0 =  (UInt64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 & *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_and_UUxU_vvxv_descr =
{ "bitwise_and_UUxU_vvxv", (void *) bitwise_and_UUxU_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(UInt64) }, { 0, 0, 0, 0 } };

static void _bitwise_and_UxU_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt64  *tin0   = (UInt64 *) ((char *) input  + inboffset);
    UInt64 *tout0  = (UInt64 *) ((char *) output + outboffset);
    UInt64 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (UInt64 *) ((char *) tin0 + inbstrides[dim]);
            net    =     net & *tin0;
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _bitwise_and_UxU_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int bitwise_and_UxU_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _bitwise_and_UxU_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(bitwise_and_UxU_R, CHECK_ALIGN, sizeof(UInt64), sizeof(UInt64));

static void _bitwise_and_UxU_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt64 *tin0   = (UInt64 *) ((char *) input  + inboffset);
    UInt64 *tout0 = (UInt64 *) ((char *) output + outboffset);
    UInt64 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (UInt64 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (UInt64 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = lastval & *tin0;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _bitwise_and_UxU_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  bitwise_and_UxU_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _bitwise_and_UxU_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(bitwise_and_UxU_A, CHECK_ALIGN, sizeof(UInt64), sizeof(UInt64));

static int bitwise_and_UUxU_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64      tin0 = *(UInt64 *) buffers[0];
    UInt64     *tin1 =  (UInt64 *) buffers[1];
    UInt64     *tout0 =  (UInt64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 & *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_and_UUxU_svxv_descr =
{ "bitwise_and_UUxU_svxv", (void *) bitwise_and_UUxU_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(UInt64) }, { 1, 0, 0, 0 } };
/*********************  bitwise_or  *********************/

static int bitwise_or_UUxU_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    UInt64      tin1 = *(UInt64 *) buffers[1];
    UInt64     *tout0 =  (UInt64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 | tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_or_UUxU_vsxv_descr =
{ "bitwise_or_UUxU_vsxv", (void *) bitwise_or_UUxU_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(UInt64) }, { 0, 1, 0, 0 } };

static int bitwise_or_UUxU_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    UInt64     *tin1 =  (UInt64 *) buffers[1];
    UInt64     *tout0 =  (UInt64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 | *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_or_UUxU_vvxv_descr =
{ "bitwise_or_UUxU_vvxv", (void *) bitwise_or_UUxU_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(UInt64) }, { 0, 0, 0, 0 } };

static void _bitwise_or_UxU_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt64  *tin0   = (UInt64 *) ((char *) input  + inboffset);
    UInt64 *tout0  = (UInt64 *) ((char *) output + outboffset);
    UInt64 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (UInt64 *) ((char *) tin0 + inbstrides[dim]);
            net    =     net | *tin0;
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _bitwise_or_UxU_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int bitwise_or_UxU_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _bitwise_or_UxU_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(bitwise_or_UxU_R, CHECK_ALIGN, sizeof(UInt64), sizeof(UInt64));

static void _bitwise_or_UxU_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt64 *tin0   = (UInt64 *) ((char *) input  + inboffset);
    UInt64 *tout0 = (UInt64 *) ((char *) output + outboffset);
    UInt64 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (UInt64 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (UInt64 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = lastval | *tin0;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _bitwise_or_UxU_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  bitwise_or_UxU_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _bitwise_or_UxU_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(bitwise_or_UxU_A, CHECK_ALIGN, sizeof(UInt64), sizeof(UInt64));

static int bitwise_or_UUxU_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64      tin0 = *(UInt64 *) buffers[0];
    UInt64     *tin1 =  (UInt64 *) buffers[1];
    UInt64     *tout0 =  (UInt64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 | *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_or_UUxU_svxv_descr =
{ "bitwise_or_UUxU_svxv", (void *) bitwise_or_UUxU_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(UInt64) }, { 1, 0, 0, 0 } };
/*********************  bitwise_xor  *********************/

static int bitwise_xor_UUxU_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    UInt64      tin1 = *(UInt64 *) buffers[1];
    UInt64     *tout0 =  (UInt64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 ^ tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_xor_UUxU_vsxv_descr =
{ "bitwise_xor_UUxU_vsxv", (void *) bitwise_xor_UUxU_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(UInt64) }, { 0, 1, 0, 0 } };

static int bitwise_xor_UUxU_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    UInt64     *tin1 =  (UInt64 *) buffers[1];
    UInt64     *tout0 =  (UInt64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 ^ *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_xor_UUxU_vvxv_descr =
{ "bitwise_xor_UUxU_vvxv", (void *) bitwise_xor_UUxU_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(UInt64) }, { 0, 0, 0, 0 } };

static void _bitwise_xor_UxU_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt64  *tin0   = (UInt64 *) ((char *) input  + inboffset);
    UInt64 *tout0  = (UInt64 *) ((char *) output + outboffset);
    UInt64 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (UInt64 *) ((char *) tin0 + inbstrides[dim]);
            net    =     net ^ *tin0;
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _bitwise_xor_UxU_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int bitwise_xor_UxU_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _bitwise_xor_UxU_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(bitwise_xor_UxU_R, CHECK_ALIGN, sizeof(UInt64), sizeof(UInt64));

static void _bitwise_xor_UxU_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt64 *tin0   = (UInt64 *) ((char *) input  + inboffset);
    UInt64 *tout0 = (UInt64 *) ((char *) output + outboffset);
    UInt64 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (UInt64 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (UInt64 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = lastval ^ *tin0;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _bitwise_xor_UxU_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  bitwise_xor_UxU_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _bitwise_xor_UxU_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(bitwise_xor_UxU_A, CHECK_ALIGN, sizeof(UInt64), sizeof(UInt64));

static int bitwise_xor_UUxU_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64      tin0 = *(UInt64 *) buffers[0];
    UInt64     *tin1 =  (UInt64 *) buffers[1];
    UInt64     *tout0 =  (UInt64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 ^ *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_xor_UUxU_svxv_descr =
{ "bitwise_xor_UUxU_svxv", (void *) bitwise_xor_UUxU_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(UInt64) }, { 1, 0, 0, 0 } };
/*********************  bitwise_not  *********************/

static int bitwise_not_UxU_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    UInt64     *tout0 =  (UInt64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ~*tin0;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_not_UxU_vxv_descr =
{ "bitwise_not_UxU_vxv", (void *) bitwise_not_UxU_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt64), sizeof(UInt64) }, { 0, 0, 0 } };
/*********************  bitwise_not  *********************/
/*********************  rshift  *********************/

static int rshift_UUxU_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    UInt64      tin1 = *(UInt64 *) buffers[1];
    UInt64     *tout0 =  (UInt64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 >> tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor rshift_UUxU_vsxv_descr =
{ "rshift_UUxU_vsxv", (void *) rshift_UUxU_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(UInt64) }, { 0, 1, 0, 0 } };

static int rshift_UUxU_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    UInt64     *tin1 =  (UInt64 *) buffers[1];
    UInt64     *tout0 =  (UInt64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 >> *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor rshift_UUxU_vvxv_descr =
{ "rshift_UUxU_vvxv", (void *) rshift_UUxU_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(UInt64) }, { 0, 0, 0, 0 } };

static int rshift_UUxU_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64      tin0 = *(UInt64 *) buffers[0];
    UInt64     *tin1 =  (UInt64 *) buffers[1];
    UInt64     *tout0 =  (UInt64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 >> *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor rshift_UUxU_svxv_descr =
{ "rshift_UUxU_svxv", (void *) rshift_UUxU_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(UInt64) }, { 1, 0, 0, 0 } };
/*********************  lshift  *********************/

static int lshift_UUxU_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    UInt64      tin1 = *(UInt64 *) buffers[1];
    UInt64     *tout0 =  (UInt64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 << tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor lshift_UUxU_vsxv_descr =
{ "lshift_UUxU_vsxv", (void *) lshift_UUxU_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(UInt64) }, { 0, 1, 0, 0 } };

static int lshift_UUxU_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    UInt64     *tin1 =  (UInt64 *) buffers[1];
    UInt64     *tout0 =  (UInt64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 << *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor lshift_UUxU_vvxv_descr =
{ "lshift_UUxU_vvxv", (void *) lshift_UUxU_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(UInt64) }, { 0, 0, 0, 0 } };

static int lshift_UUxU_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64      tin0 = *(UInt64 *) buffers[0];
    UInt64     *tin1 =  (UInt64 *) buffers[1];
    UInt64     *tout0 =  (UInt64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 << *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor lshift_UUxU_svxv_descr =
{ "lshift_UUxU_svxv", (void *) lshift_UUxU_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(UInt64) }, { 1, 0, 0, 0 } };
/*********************  floor  *********************/

static int floor_UxU_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    UInt64     *tout0 =  (UInt64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = (*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor floor_UxU_vxf_descr =
{ "floor_UxU_vxf", (void *) floor_UxU_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt64), sizeof(UInt64) }, { 0, 0, 0 } };
/*********************  floor  *********************/
/*********************  ceil  *********************/

static int ceil_UxU_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    UInt64     *tout0 =  (UInt64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = (*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor ceil_UxU_vxf_descr =
{ "ceil_UxU_vxf", (void *) ceil_UxU_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt64), sizeof(UInt64) }, { 0, 0, 0 } };
/*********************  ceil  *********************/
/*********************  maximum  *********************/

static int maximum_UUxU_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    UInt64      tin1 = *(UInt64 *) buffers[1];
    UInt64     *tout0 =  (UInt64 *) buffers[2];
UInt64 temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufmaximum(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor maximum_UUxU_vsxf_descr =
{ "maximum_UUxU_vsxf", (void *) maximum_UUxU_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(UInt64) }, { 0, 1, 0, 0 } };

static int maximum_UUxU_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    UInt64     *tin1 =  (UInt64 *) buffers[1];
    UInt64     *tout0 =  (UInt64 *) buffers[2];
UInt64 temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufmaximum(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor maximum_UUxU_vvxf_descr =
{ "maximum_UUxU_vvxf", (void *) maximum_UUxU_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(UInt64) }, { 0, 0, 0, 0 } };

static void _maximum_UxU_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt64  *tin0   = (UInt64 *) ((char *) input  + inboffset);
    UInt64 *tout0  = (UInt64 *) ((char *) output + outboffset);
    UInt64 net;
    UInt64 temp1, temp2;
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (UInt64 *) ((char *) tin0 + inbstrides[dim]);
            net   = ufmaximum(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _maximum_UxU_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int maximum_UxU_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _maximum_UxU_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(maximum_UxU_R, CHECK_ALIGN, sizeof(UInt64), sizeof(UInt64));

static void _maximum_UxU_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt64 *tin0   = (UInt64 *) ((char *) input  + inboffset);
    UInt64 *tout0 = (UInt64 *) ((char *) output + outboffset);
    UInt64 lastval;
    UInt64 temp1, temp2;
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (UInt64 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (UInt64 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = ufmaximum(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _maximum_UxU_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  maximum_UxU_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _maximum_UxU_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(maximum_UxU_A, CHECK_ALIGN, sizeof(UInt64), sizeof(UInt64));

static int maximum_UUxU_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64      tin0 = *(UInt64 *) buffers[0];
    UInt64     *tin1 =  (UInt64 *) buffers[1];
    UInt64     *tout0 =  (UInt64 *) buffers[2];
UInt64 temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufmaximum(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor maximum_UUxU_svxf_descr =
{ "maximum_UUxU_svxf", (void *) maximum_UUxU_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(UInt64) }, { 1, 0, 0, 0 } };
/*********************  minimum  *********************/

static int minimum_UUxU_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    UInt64      tin1 = *(UInt64 *) buffers[1];
    UInt64     *tout0 =  (UInt64 *) buffers[2];
UInt64 temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufminimum(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor minimum_UUxU_vsxf_descr =
{ "minimum_UUxU_vsxf", (void *) minimum_UUxU_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(UInt64) }, { 0, 1, 0, 0 } };

static int minimum_UUxU_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    UInt64     *tin1 =  (UInt64 *) buffers[1];
    UInt64     *tout0 =  (UInt64 *) buffers[2];
UInt64 temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufminimum(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor minimum_UUxU_vvxf_descr =
{ "minimum_UUxU_vvxf", (void *) minimum_UUxU_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(UInt64) }, { 0, 0, 0, 0 } };

static void _minimum_UxU_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt64  *tin0   = (UInt64 *) ((char *) input  + inboffset);
    UInt64 *tout0  = (UInt64 *) ((char *) output + outboffset);
    UInt64 net;
    UInt64 temp1, temp2;
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (UInt64 *) ((char *) tin0 + inbstrides[dim]);
            net   = ufminimum(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _minimum_UxU_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int minimum_UxU_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _minimum_UxU_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(minimum_UxU_R, CHECK_ALIGN, sizeof(UInt64), sizeof(UInt64));

static void _minimum_UxU_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt64 *tin0   = (UInt64 *) ((char *) input  + inboffset);
    UInt64 *tout0 = (UInt64 *) ((char *) output + outboffset);
    UInt64 lastval;
    UInt64 temp1, temp2;
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (UInt64 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (UInt64 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = ufminimum(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _minimum_UxU_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  minimum_UxU_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _minimum_UxU_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(minimum_UxU_A, CHECK_ALIGN, sizeof(UInt64), sizeof(UInt64));

static int minimum_UUxU_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64      tin0 = *(UInt64 *) buffers[0];
    UInt64     *tin1 =  (UInt64 *) buffers[1];
    UInt64     *tout0 =  (UInt64 *) buffers[2];
UInt64 temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufminimum(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor minimum_UUxU_svxf_descr =
{ "minimum_UUxU_svxf", (void *) minimum_UUxU_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(UInt64) }, { 1, 0, 0, 0 } };
/*********************  fabs  *********************/

static int fabs_Uxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = fabs(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor fabs_Uxd_vxf_descr =
{ "fabs_Uxd_vxf", (void *) fabs_Uxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt64), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  _round  *********************/

static int _round_Uxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_round(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor _round_Uxd_vxf_descr =
{ "_round_Uxd_vxf", (void *) _round_Uxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt64), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  hypot  *********************/

static int hypot_UUxd_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    UInt64      tin1 = *(UInt64 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = hypot(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor hypot_UUxd_vsxf_descr =
{ "hypot_UUxd_vsxf", (void *) hypot_UUxd_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(Float64) }, { 0, 1, 0, 0 } };

static int hypot_UUxd_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64     *tin0 =  (UInt64 *) buffers[0];
    UInt64     *tin1 =  (UInt64 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = hypot(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor hypot_UUxd_vvxf_descr =
{ "hypot_UUxd_vvxf", (void *) hypot_UUxd_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(Float64) }, { 0, 0, 0, 0 } };

static void _hypot_Uxd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt64  *tin0   = (UInt64 *) ((char *) input  + inboffset);
    Float64 *tout0  = (Float64 *) ((char *) output + outboffset);
    Float64 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (UInt64 *) ((char *) tin0 + inbstrides[dim]);
            net   = hypot(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _hypot_Uxd_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int hypot_Uxd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _hypot_Uxd_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(hypot_Uxd_R, CHECK_ALIGN, sizeof(UInt64), sizeof(Float64));

static void _hypot_Uxd_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt64 *tin0   = (UInt64 *) ((char *) input  + inboffset);
    Float64 *tout0 = (Float64 *) ((char *) output + outboffset);
    Float64 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (UInt64 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Float64 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = hypot(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _hypot_Uxd_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  hypot_Uxd_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _hypot_Uxd_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(hypot_Uxd_A, CHECK_ALIGN, sizeof(UInt64), sizeof(Float64));

static int hypot_UUxd_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt64      tin0 = *(UInt64 *) buffers[0];
    UInt64     *tin1 =  (UInt64 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = hypot(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor hypot_UUxd_svxf_descr =
{ "hypot_UUxd_svxf", (void *) hypot_UUxd_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt64), sizeof(UInt64), sizeof(Float64) }, { 1, 0, 0, 0 } };
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

static PyMethodDef _ufuncUInt64Methods[] = {

	{NULL,      NULL}        /* Sentinel */
};

static PyObject *init_funcDict(void) {
    PyObject *dict, *keytuple, *cfunc;
    dict = PyDict_New();
    /* minus_UxU_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","minus","v","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minus_UxU_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_UUxU_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","add","vs","UInt64","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_UUxU_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_UUxU_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","add","vv","UInt64","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_UUxU_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_UxU_R */
    keytuple=Py_BuildValue("ss((s)(s))","add","R","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_UxU_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_UxU_A */
    keytuple=Py_BuildValue("ss((s)(s))","add","A","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_UxU_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_UUxU_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","add","sv","UInt64","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_UUxU_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_UUxU_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","subtract","vs","UInt64","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_UUxU_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_UUxU_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","subtract","vv","UInt64","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_UUxU_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_UxU_R */
    keytuple=Py_BuildValue("ss((s)(s))","subtract","R","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_UxU_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_UxU_A */
    keytuple=Py_BuildValue("ss((s)(s))","subtract","A","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_UxU_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_UUxU_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","subtract","sv","UInt64","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_UUxU_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* multiply_UUxU_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","multiply","vs","UInt64","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&multiply_UUxU_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* multiply_UUxU_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","multiply","vv","UInt64","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&multiply_UUxU_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* multiply_UxU_R */
    keytuple=Py_BuildValue("ss((s)(s))","multiply","R","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&multiply_UxU_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* multiply_UxU_A */
    keytuple=Py_BuildValue("ss((s)(s))","multiply","A","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&multiply_UxU_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* multiply_UUxU_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","multiply","sv","UInt64","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&multiply_UUxU_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_UUxU_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","divide","vs","UInt64","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_UUxU_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_UUxU_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","divide","vv","UInt64","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_UUxU_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_UxU_R */
    keytuple=Py_BuildValue("ss((s)(s))","divide","R","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_UxU_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_UxU_A */
    keytuple=Py_BuildValue("ss((s)(s))","divide","A","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_UxU_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_UUxU_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","divide","sv","UInt64","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_UUxU_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_divide_UUxU_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","floor_divide","vs","UInt64","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_divide_UUxU_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_divide_UUxU_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","floor_divide","vv","UInt64","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_divide_UUxU_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_divide_UxU_R */
    keytuple=Py_BuildValue("ss((s)(s))","floor_divide","R","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_divide_UxU_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_divide_UxU_A */
    keytuple=Py_BuildValue("ss((s)(s))","floor_divide","A","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_divide_UxU_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_divide_UUxU_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","floor_divide","sv","UInt64","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_divide_UUxU_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_UUxd_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","true_divide","vs","UInt64","UInt64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_UUxd_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_UUxd_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","true_divide","vv","UInt64","UInt64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_UUxd_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_Uxd_R */
    keytuple=Py_BuildValue("ss((s)(s))","true_divide","R","UInt64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_Uxd_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_Uxd_A */
    keytuple=Py_BuildValue("ss((s)(s))","true_divide","A","UInt64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_Uxd_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_UUxd_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","true_divide","sv","UInt64","UInt64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_UUxd_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_UUxU_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","remainder","vs","UInt64","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_UUxU_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_UUxU_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","remainder","vv","UInt64","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_UUxU_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_UxU_R */
    keytuple=Py_BuildValue("ss((s)(s))","remainder","R","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_UxU_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_UxU_A */
    keytuple=Py_BuildValue("ss((s)(s))","remainder","A","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_UxU_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_UUxU_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","remainder","sv","UInt64","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_UUxU_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_UUxU_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","power","vs","UInt64","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_UUxU_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_UUxU_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","power","vv","UInt64","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_UUxU_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_UxU_R */
    keytuple=Py_BuildValue("ss((s)(s))","power","R","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_UxU_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_UxU_A */
    keytuple=Py_BuildValue("ss((s)(s))","power","A","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_UxU_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_UUxU_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","power","sv","UInt64","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_UUxU_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* abs_UxU_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","abs","v","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&abs_UxU_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* sin_Uxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","sin","v","UInt64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&sin_Uxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* cos_Uxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","cos","v","UInt64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&cos_Uxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* tan_Uxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","tan","v","UInt64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&tan_Uxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arcsin_Uxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arcsin","v","UInt64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arcsin_Uxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arccos_Uxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arccos","v","UInt64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arccos_Uxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan_Uxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arctan","v","UInt64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan_Uxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan2_UUxd_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","arctan2","vs","UInt64","UInt64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan2_UUxd_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan2_UUxd_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","arctan2","vv","UInt64","UInt64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan2_UUxd_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan2_Uxd_R */
    keytuple=Py_BuildValue("ss((s)(s))","arctan2","R","UInt64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan2_Uxd_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan2_Uxd_A */
    keytuple=Py_BuildValue("ss((s)(s))","arctan2","A","UInt64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan2_Uxd_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan2_UUxd_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","arctan2","sv","UInt64","UInt64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan2_UUxd_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* log_Uxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","log","v","UInt64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&log_Uxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* log10_Uxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","log10","v","UInt64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&log10_Uxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* exp_Uxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","exp","v","UInt64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&exp_Uxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* sinh_Uxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","sinh","v","UInt64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&sinh_Uxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* cosh_Uxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","cosh","v","UInt64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&cosh_Uxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* tanh_Uxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","tanh","v","UInt64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&tanh_Uxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arcsinh_Uxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arcsinh","v","UInt64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arcsinh_Uxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arccosh_Uxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arccosh","v","UInt64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arccosh_Uxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctanh_Uxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arctanh","v","UInt64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctanh_Uxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* sqrt_Uxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","sqrt","v","UInt64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&sqrt_Uxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* equal_UUxB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","equal","vs","UInt64","UInt64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&equal_UUxB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* equal_UUxB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","equal","vv","UInt64","UInt64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&equal_UUxB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* equal_UUxB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","equal","sv","UInt64","UInt64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&equal_UUxB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* not_equal_UUxB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","not_equal","vs","UInt64","UInt64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&not_equal_UUxB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* not_equal_UUxB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","not_equal","vv","UInt64","UInt64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&not_equal_UUxB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* not_equal_UUxB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","not_equal","sv","UInt64","UInt64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&not_equal_UUxB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_UUxB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater","vs","UInt64","UInt64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_UUxB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_UUxB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater","vv","UInt64","UInt64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_UUxB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_UUxB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater","sv","UInt64","UInt64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_UUxB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_equal_UUxB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater_equal","vs","UInt64","UInt64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_equal_UUxB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_equal_UUxB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater_equal","vv","UInt64","UInt64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_equal_UUxB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_equal_UUxB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater_equal","sv","UInt64","UInt64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_equal_UUxB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_UUxB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less","vs","UInt64","UInt64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_UUxB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_UUxB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less","vv","UInt64","UInt64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_UUxB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_UUxB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less","sv","UInt64","UInt64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_UUxB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_equal_UUxB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less_equal","vs","UInt64","UInt64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_equal_UUxB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_equal_UUxB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less_equal","vv","UInt64","UInt64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_equal_UUxB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_equal_UUxB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less_equal","sv","UInt64","UInt64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_equal_UUxB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_and_UUxB_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_and","vs","UInt64","UInt64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_and_UUxB_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_and_UUxB_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_and","vv","UInt64","UInt64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_and_UUxB_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_and_UUxB_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_and","sv","UInt64","UInt64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_and_UUxB_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_or_UUxB_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_or","vs","UInt64","UInt64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_or_UUxB_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_or_UUxB_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_or","vv","UInt64","UInt64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_or_UUxB_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_or_UUxB_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_or","sv","UInt64","UInt64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_or_UUxB_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_xor_UUxB_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_xor","vs","UInt64","UInt64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_xor_UUxB_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_xor_UUxB_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_xor","vv","UInt64","UInt64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_xor_UUxB_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_xor_UUxB_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_xor","sv","UInt64","UInt64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_xor_UUxB_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_not_UxB_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","logical_not","v","UInt64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_not_UxB_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_and_UUxU_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_and","vs","UInt64","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_and_UUxU_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_and_UUxU_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_and","vv","UInt64","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_and_UUxU_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_and_UxU_R */
    keytuple=Py_BuildValue("ss((s)(s))","bitwise_and","R","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_and_UxU_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_and_UxU_A */
    keytuple=Py_BuildValue("ss((s)(s))","bitwise_and","A","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_and_UxU_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_and_UUxU_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_and","sv","UInt64","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_and_UUxU_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_or_UUxU_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_or","vs","UInt64","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_or_UUxU_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_or_UUxU_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_or","vv","UInt64","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_or_UUxU_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_or_UxU_R */
    keytuple=Py_BuildValue("ss((s)(s))","bitwise_or","R","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_or_UxU_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_or_UxU_A */
    keytuple=Py_BuildValue("ss((s)(s))","bitwise_or","A","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_or_UxU_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_or_UUxU_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_or","sv","UInt64","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_or_UUxU_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_xor_UUxU_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_xor","vs","UInt64","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_xor_UUxU_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_xor_UUxU_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_xor","vv","UInt64","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_xor_UUxU_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_xor_UxU_R */
    keytuple=Py_BuildValue("ss((s)(s))","bitwise_xor","R","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_xor_UxU_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_xor_UxU_A */
    keytuple=Py_BuildValue("ss((s)(s))","bitwise_xor","A","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_xor_UxU_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_xor_UUxU_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_xor","sv","UInt64","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_xor_UUxU_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_not_UxU_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","bitwise_not","v","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_not_UxU_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* rshift_UUxU_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","rshift","vs","UInt64","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&rshift_UUxU_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* rshift_UUxU_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","rshift","vv","UInt64","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&rshift_UUxU_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* rshift_UUxU_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","rshift","sv","UInt64","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&rshift_UUxU_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* lshift_UUxU_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","lshift","vs","UInt64","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&lshift_UUxU_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* lshift_UUxU_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","lshift","vv","UInt64","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&lshift_UUxU_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* lshift_UUxU_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","lshift","sv","UInt64","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&lshift_UUxU_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_UxU_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","floor","v","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_UxU_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* ceil_UxU_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","ceil","v","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&ceil_UxU_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_UUxU_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","maximum","vs","UInt64","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_UUxU_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_UUxU_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","maximum","vv","UInt64","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_UUxU_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_UxU_R */
    keytuple=Py_BuildValue("ss((s)(s))","maximum","R","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_UxU_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_UxU_A */
    keytuple=Py_BuildValue("ss((s)(s))","maximum","A","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_UxU_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_UUxU_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","maximum","sv","UInt64","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_UUxU_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_UUxU_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","minimum","vs","UInt64","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_UUxU_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_UUxU_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","minimum","vv","UInt64","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_UUxU_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_UxU_R */
    keytuple=Py_BuildValue("ss((s)(s))","minimum","R","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_UxU_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_UxU_A */
    keytuple=Py_BuildValue("ss((s)(s))","minimum","A","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_UxU_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_UUxU_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","minimum","sv","UInt64","UInt64","UInt64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_UUxU_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* fabs_Uxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","fabs","v","UInt64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&fabs_Uxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* _round_Uxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","_round","v","UInt64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&_round_Uxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_UUxd_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","hypot","vs","UInt64","UInt64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_UUxd_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_UUxd_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","hypot","vv","UInt64","UInt64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_UUxd_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_Uxd_R */
    keytuple=Py_BuildValue("ss((s)(s))","hypot","R","UInt64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_Uxd_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_Uxd_A */
    keytuple=Py_BuildValue("ss((s)(s))","hypot","A","UInt64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_Uxd_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_UUxd_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","hypot","sv","UInt64","UInt64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_UUxd_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    return dict;
}

/* platform independent*/
#ifdef MS_WIN32
__declspec(dllexport)
#endif
void init_ufuncUInt64(void) {
    PyObject *m, *d, *functions;
    m = Py_InitModule("_ufuncUInt64", _ufuncUInt64Methods);
    d = PyModule_GetDict(m);
    import_libnumarray();
    functions = init_funcDict();
    PyDict_SetItemString(d, "functionDict", functions);
    Py_DECREF(functions);
    ADD_VERSION(m);
}
