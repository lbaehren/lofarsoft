
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

static int minus_uxu_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    UInt32     *tout0 =  (UInt32 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = -*tin0;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor minus_uxu_vxv_descr =
{ "minus_uxu_vxv", (void *) minus_uxu_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt32), sizeof(UInt32) }, { 0, 0, 0 } };
/*********************  add  *********************/

static int add_uuxu_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    UInt32      tin1 = *(UInt32 *) buffers[1];
    UInt32     *tout0 =  (UInt32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 + tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor add_uuxu_vsxv_descr =
{ "add_uuxu_vsxv", (void *) add_uuxu_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(UInt32) }, { 0, 1, 0, 0 } };

static int add_uuxu_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    UInt32     *tin1 =  (UInt32 *) buffers[1];
    UInt32     *tout0 =  (UInt32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 + *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor add_uuxu_vvxv_descr =
{ "add_uuxu_vvxv", (void *) add_uuxu_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(UInt32) }, { 0, 0, 0, 0 } };

static void _add_uxu_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt32  *tin0   = (UInt32 *) ((char *) input  + inboffset);
    UInt32 *tout0  = (UInt32 *) ((char *) output + outboffset);
    UInt32 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (UInt32 *) ((char *) tin0 + inbstrides[dim]);
            net    =     net + *tin0;
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _add_uxu_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int add_uxu_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _add_uxu_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(add_uxu_R, CHECK_ALIGN, sizeof(UInt32), sizeof(UInt32));

static void _add_uxu_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt32 *tin0   = (UInt32 *) ((char *) input  + inboffset);
    UInt32 *tout0 = (UInt32 *) ((char *) output + outboffset);
    UInt32 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (UInt32 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (UInt32 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = lastval + *tin0;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _add_uxu_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  add_uxu_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _add_uxu_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(add_uxu_A, CHECK_ALIGN, sizeof(UInt32), sizeof(UInt32));

static int add_uuxu_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32      tin0 = *(UInt32 *) buffers[0];
    UInt32     *tin1 =  (UInt32 *) buffers[1];
    UInt32     *tout0 =  (UInt32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 + *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor add_uuxu_svxv_descr =
{ "add_uuxu_svxv", (void *) add_uuxu_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(UInt32) }, { 1, 0, 0, 0 } };
/*********************  subtract  *********************/

static int subtract_uuxu_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    UInt32      tin1 = *(UInt32 *) buffers[1];
    UInt32     *tout0 =  (UInt32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 - tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor subtract_uuxu_vsxv_descr =
{ "subtract_uuxu_vsxv", (void *) subtract_uuxu_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(UInt32) }, { 0, 1, 0, 0 } };

static int subtract_uuxu_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    UInt32     *tin1 =  (UInt32 *) buffers[1];
    UInt32     *tout0 =  (UInt32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 - *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor subtract_uuxu_vvxv_descr =
{ "subtract_uuxu_vvxv", (void *) subtract_uuxu_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(UInt32) }, { 0, 0, 0, 0 } };

static void _subtract_uxu_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt32  *tin0   = (UInt32 *) ((char *) input  + inboffset);
    UInt32 *tout0  = (UInt32 *) ((char *) output + outboffset);
    UInt32 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (UInt32 *) ((char *) tin0 + inbstrides[dim]);
            net    =     net - *tin0;
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _subtract_uxu_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int subtract_uxu_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _subtract_uxu_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(subtract_uxu_R, CHECK_ALIGN, sizeof(UInt32), sizeof(UInt32));

static void _subtract_uxu_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt32 *tin0   = (UInt32 *) ((char *) input  + inboffset);
    UInt32 *tout0 = (UInt32 *) ((char *) output + outboffset);
    UInt32 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (UInt32 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (UInt32 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = lastval - *tin0;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _subtract_uxu_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  subtract_uxu_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _subtract_uxu_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(subtract_uxu_A, CHECK_ALIGN, sizeof(UInt32), sizeof(UInt32));

static int subtract_uuxu_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32      tin0 = *(UInt32 *) buffers[0];
    UInt32     *tin1 =  (UInt32 *) buffers[1];
    UInt32     *tout0 =  (UInt32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 - *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor subtract_uuxu_svxv_descr =
{ "subtract_uuxu_svxv", (void *) subtract_uuxu_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(UInt32) }, { 1, 0, 0, 0 } };
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/

static int multiply_uuxu_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    UInt32      tin1 = *(UInt32 *) buffers[1];
    UInt32     *tout0 =  (UInt32 *) buffers[2];
Float64 temp;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        temp = ((Float64) *tin0) * ((Float64) tin1);
    if (temp > 4294967295U) temp = (Float64) int_overflow_error(4294967295.);
    *tout0 = (UInt32) temp;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor multiply_uuxu_vsxv_descr =
{ "multiply_uuxu_vsxv", (void *) multiply_uuxu_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(UInt32) }, { 0, 1, 0, 0 } };

static int multiply_uuxu_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    UInt32     *tin1 =  (UInt32 *) buffers[1];
    UInt32     *tout0 =  (UInt32 *) buffers[2];
Float64 temp;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        temp = ((Float64) *tin0) * ((Float64) *tin1);
    if (temp > 4294967295U) temp = (Float64) int_overflow_error(4294967295.);
    *tout0 = (UInt32) temp;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor multiply_uuxu_vvxv_descr =
{ "multiply_uuxu_vvxv", (void *) multiply_uuxu_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(UInt32) }, { 0, 0, 0, 0 } };

static void _multiply_uxu_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt32  *tin0   = (UInt32 *) ((char *) input  + inboffset);
    UInt32 *tout0  = (UInt32 *) ((char *) output + outboffset);
    UInt32 net;
    Float64 temp;
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (UInt32 *) ((char *) tin0 + inbstrides[dim]);
            temp = ((Float64) net) * ((Float64) *tin0);
    if (temp > 4294967295U) temp = (Float64) int_overflow_error(4294967295.);
    net = (UInt32) temp;
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _multiply_uxu_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int multiply_uxu_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _multiply_uxu_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(multiply_uxu_R, CHECK_ALIGN, sizeof(UInt32), sizeof(UInt32));

static void _multiply_uxu_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt32 *tin0   = (UInt32 *) ((char *) input  + inboffset);
    UInt32 *tout0 = (UInt32 *) ((char *) output + outboffset);
    UInt32 lastval;
    Float64 temp;
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (UInt32 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (UInt32 *) ((char *) tout0 + outbstrides[dim]);
            temp = ((Float64) lastval) * ((Float64) *tin0);
    if (temp > 4294967295U) temp = (Float64) int_overflow_error(4294967295.);
    *tout0 = (UInt32) temp;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _multiply_uxu_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  multiply_uxu_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _multiply_uxu_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(multiply_uxu_A, CHECK_ALIGN, sizeof(UInt32), sizeof(UInt32));

static int multiply_uuxu_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32      tin0 = *(UInt32 *) buffers[0];
    UInt32     *tin1 =  (UInt32 *) buffers[1];
    UInt32     *tout0 =  (UInt32 *) buffers[2];
Float64 temp;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        temp = ((Float64) tin0) * ((Float64) *tin1);
    if (temp > 4294967295U) temp = (Float64) int_overflow_error(4294967295.);
    *tout0 = (UInt32) temp;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor multiply_uuxu_svxv_descr =
{ "multiply_uuxu_svxv", (void *) multiply_uuxu_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(UInt32) }, { 1, 0, 0, 0 } };
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  divide  *********************/

static int divide_uuxu_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    UInt32      tin1 = *(UInt32 *) buffers[1];
    UInt32     *tout0 =  (UInt32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((tin1==0) ? int_dividebyzero_error(tin1, *tin0) : *tin0 / tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor divide_uuxu_vsxv_descr =
{ "divide_uuxu_vsxv", (void *) divide_uuxu_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(UInt32) }, { 0, 1, 0, 0 } };

static int divide_uuxu_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    UInt32     *tin1 =  (UInt32 *) buffers[1];
    UInt32     *tout0 =  (UInt32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, *tin0) : *tin0 / *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor divide_uuxu_vvxv_descr =
{ "divide_uuxu_vvxv", (void *) divide_uuxu_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(UInt32) }, { 0, 0, 0, 0 } };

static void _divide_uxu_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt32  *tin0   = (UInt32 *) ((char *) input  + inboffset);
    UInt32 *tout0  = (UInt32 *) ((char *) output + outboffset);
    UInt32 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (UInt32 *) ((char *) tin0 + inbstrides[dim]);
            net = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0) : net / *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _divide_uxu_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int divide_uxu_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _divide_uxu_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(divide_uxu_R, CHECK_ALIGN, sizeof(UInt32), sizeof(UInt32));

static void _divide_uxu_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt32 *tin0   = (UInt32 *) ((char *) input  + inboffset);
    UInt32 *tout0 = (UInt32 *) ((char *) output + outboffset);
    UInt32 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (UInt32 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (UInt32 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0) : lastval / *tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _divide_uxu_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  divide_uxu_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _divide_uxu_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(divide_uxu_A, CHECK_ALIGN, sizeof(UInt32), sizeof(UInt32));

static int divide_uuxu_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32      tin0 = *(UInt32 *) buffers[0];
    UInt32     *tin1 =  (UInt32 *) buffers[1];
    UInt32     *tout0 =  (UInt32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, 0) : tin0 / *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor divide_uuxu_svxv_descr =
{ "divide_uuxu_svxv", (void *) divide_uuxu_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(UInt32) }, { 1, 0, 0, 0 } };
/*********************  divide  *********************/
/*********************  floor_divide  *********************/

static int floor_divide_uuxu_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    UInt32      tin1 = *(UInt32 *) buffers[1];
    UInt32     *tout0 =  (UInt32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = (( tin1==0) ? int_dividebyzero_error( tin1, *tin0) : floor(*tin0   / (double)  tin1));
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor floor_divide_uuxu_vsxv_descr =
{ "floor_divide_uuxu_vsxv", (void *) floor_divide_uuxu_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(UInt32) }, { 0, 1, 0, 0 } };

static int floor_divide_uuxu_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    UInt32     *tin1 =  (UInt32 *) buffers[1];
    UInt32     *tout0 =  (UInt32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, *tin0) : floor(*tin0   / (double) *tin1));
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor floor_divide_uuxu_vvxv_descr =
{ "floor_divide_uuxu_vvxv", (void *) floor_divide_uuxu_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(UInt32) }, { 0, 0, 0, 0 } };

static void _floor_divide_uxu_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt32  *tin0   = (UInt32 *) ((char *) input  + inboffset);
    UInt32 *tout0  = (UInt32 *) ((char *) output + outboffset);
    UInt32 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (UInt32 *) ((char *) tin0 + inbstrides[dim]);
            net    = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0)     : floor(net     / (double) *tin0));
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _floor_divide_uxu_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int floor_divide_uxu_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _floor_divide_uxu_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(floor_divide_uxu_R, CHECK_ALIGN, sizeof(UInt32), sizeof(UInt32));

static void _floor_divide_uxu_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt32 *tin0   = (UInt32 *) ((char *) input  + inboffset);
    UInt32 *tout0 = (UInt32 *) ((char *) output + outboffset);
    UInt32 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (UInt32 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (UInt32 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0)     : floor(lastval / (double) *tin0));
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _floor_divide_uxu_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  floor_divide_uxu_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _floor_divide_uxu_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(floor_divide_uxu_A, CHECK_ALIGN, sizeof(UInt32), sizeof(UInt32));

static int floor_divide_uuxu_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32      tin0 = *(UInt32 *) buffers[0];
    UInt32     *tin1 =  (UInt32 *) buffers[1];
    UInt32     *tout0 =  (UInt32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, 0)     : floor(tin0    / (double) *tin1));
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor floor_divide_uuxu_svxv_descr =
{ "floor_divide_uuxu_svxv", (void *) floor_divide_uuxu_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(UInt32) }, { 1, 0, 0, 0 } };
/*********************  floor_divide  *********************/
/*********************  true_divide  *********************/

static int true_divide_uuxf_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    UInt32      tin1 = *(UInt32 *) buffers[1];
    Float32    *tout0 =  (Float32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = (( tin1==0) ? int_dividebyzero_error( tin1, *tin0) : *tin0         / (double) tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor true_divide_uuxf_vsxv_descr =
{ "true_divide_uuxf_vsxv", (void *) true_divide_uuxf_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(Float32) }, { 0, 1, 0, 0 } };

static int true_divide_uuxf_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    UInt32     *tin1 =  (UInt32 *) buffers[1];
    Float32    *tout0 =  (Float32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, *tin0) : *tin0         / (double) *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor true_divide_uuxf_vvxv_descr =
{ "true_divide_uuxf_vvxv", (void *) true_divide_uuxf_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(Float32) }, { 0, 0, 0, 0 } };

static void _true_divide_uxf_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt32  *tin0   = (UInt32 *) ((char *) input  + inboffset);
    Float32 *tout0  = (Float32 *) ((char *) output + outboffset);
    Float32 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (UInt32 *) ((char *) tin0 + inbstrides[dim]);
            net    = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0)     : net           / (double) *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _true_divide_uxf_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int true_divide_uxf_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _true_divide_uxf_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(true_divide_uxf_R, CHECK_ALIGN, sizeof(UInt32), sizeof(Float32));

static void _true_divide_uxf_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt32 *tin0   = (UInt32 *) ((char *) input  + inboffset);
    Float32 *tout0 = (Float32 *) ((char *) output + outboffset);
    Float32 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (UInt32 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Float32 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0)     : lastval       / (double) *tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _true_divide_uxf_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  true_divide_uxf_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _true_divide_uxf_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(true_divide_uxf_A, CHECK_ALIGN, sizeof(UInt32), sizeof(Float32));

static int true_divide_uuxf_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32      tin0 = *(UInt32 *) buffers[0];
    UInt32     *tin1 =  (UInt32 *) buffers[1];
    Float32    *tout0 =  (Float32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, 0)     : tin0          / (double) *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor true_divide_uuxf_svxv_descr =
{ "true_divide_uuxf_svxv", (void *) true_divide_uuxf_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(Float32) }, { 1, 0, 0, 0 } };
/*********************  true_divide  *********************/
/*********************  remainder  *********************/

static int remainder_uuxu_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    UInt32      tin1 = *(UInt32 *) buffers[1];
    UInt32     *tout0 =  (UInt32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((tin1==0) ? int_dividebyzero_error(tin1, *tin0) : *tin0  %  tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor remainder_uuxu_vsxv_descr =
{ "remainder_uuxu_vsxv", (void *) remainder_uuxu_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(UInt32) }, { 0, 1, 0, 0 } };

static int remainder_uuxu_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    UInt32     *tin1 =  (UInt32 *) buffers[1];
    UInt32     *tout0 =  (UInt32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, *tin0) : *tin0  %  *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor remainder_uuxu_vvxv_descr =
{ "remainder_uuxu_vvxv", (void *) remainder_uuxu_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(UInt32) }, { 0, 0, 0, 0 } };

static void _remainder_uxu_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt32  *tin0   = (UInt32 *) ((char *) input  + inboffset);
    UInt32 *tout0  = (UInt32 *) ((char *) output + outboffset);
    UInt32 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (UInt32 *) ((char *) tin0 + inbstrides[dim]);
            net = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0) : net  %  *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _remainder_uxu_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int remainder_uxu_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _remainder_uxu_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(remainder_uxu_R, CHECK_ALIGN, sizeof(UInt32), sizeof(UInt32));

static void _remainder_uxu_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt32 *tin0   = (UInt32 *) ((char *) input  + inboffset);
    UInt32 *tout0 = (UInt32 *) ((char *) output + outboffset);
    UInt32 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (UInt32 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (UInt32 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0) : lastval  %  *tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _remainder_uxu_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  remainder_uxu_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _remainder_uxu_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(remainder_uxu_A, CHECK_ALIGN, sizeof(UInt32), sizeof(UInt32));

static int remainder_uuxu_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32      tin0 = *(UInt32 *) buffers[0];
    UInt32     *tin1 =  (UInt32 *) buffers[1];
    UInt32     *tout0 =  (UInt32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, 0) : tin0  %  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor remainder_uuxu_svxv_descr =
{ "remainder_uuxu_svxv", (void *) remainder_uuxu_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(UInt32) }, { 1, 0, 0, 0 } };
/*********************  remainder  *********************/
/*********************  power  *********************/

static int power_uuxu_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    UInt32      tin1 = *(UInt32 *) buffers[1];
    UInt32     *tout0 =  (UInt32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_pow(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor power_uuxu_vsxf_descr =
{ "power_uuxu_vsxf", (void *) power_uuxu_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(UInt32) }, { 0, 1, 0, 0 } };

static int power_uuxu_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    UInt32     *tin1 =  (UInt32 *) buffers[1];
    UInt32     *tout0 =  (UInt32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_pow(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor power_uuxu_vvxf_descr =
{ "power_uuxu_vvxf", (void *) power_uuxu_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(UInt32) }, { 0, 0, 0, 0 } };

static void _power_uxu_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt32  *tin0   = (UInt32 *) ((char *) input  + inboffset);
    UInt32 *tout0  = (UInt32 *) ((char *) output + outboffset);
    UInt32 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (UInt32 *) ((char *) tin0 + inbstrides[dim]);
            net   = num_pow(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _power_uxu_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int power_uxu_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _power_uxu_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(power_uxu_R, CHECK_ALIGN, sizeof(UInt32), sizeof(UInt32));

static void _power_uxu_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt32 *tin0   = (UInt32 *) ((char *) input  + inboffset);
    UInt32 *tout0 = (UInt32 *) ((char *) output + outboffset);
    UInt32 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (UInt32 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (UInt32 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = num_pow(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _power_uxu_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  power_uxu_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _power_uxu_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(power_uxu_A, CHECK_ALIGN, sizeof(UInt32), sizeof(UInt32));

static int power_uuxu_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32      tin0 = *(UInt32 *) buffers[0];
    UInt32     *tin1 =  (UInt32 *) buffers[1];
    UInt32     *tout0 =  (UInt32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_pow(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor power_uuxu_svxf_descr =
{ "power_uuxu_svxf", (void *) power_uuxu_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(UInt32) }, { 1, 0, 0, 0 } };
/*********************  abs  *********************/

static int abs_uxu_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    UInt32     *tout0 =  (UInt32 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = fabs(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor abs_uxu_vxf_descr =
{ "abs_uxu_vxf", (void *) abs_uxu_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt32), sizeof(UInt32) }, { 0, 0, 0 } };
/*********************  sin  *********************/

static int sin_uxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = sin(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor sin_uxd_vxf_descr =
{ "sin_uxd_vxf", (void *) sin_uxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt32), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  cos  *********************/

static int cos_uxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = cos(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor cos_uxd_vxf_descr =
{ "cos_uxd_vxf", (void *) cos_uxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt32), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  tan  *********************/

static int tan_uxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = tan(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor tan_uxd_vxf_descr =
{ "tan_uxd_vxf", (void *) tan_uxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt32), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arcsin  *********************/

static int arcsin_uxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = asin(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arcsin_uxd_vxf_descr =
{ "arcsin_uxd_vxf", (void *) arcsin_uxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt32), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arccos  *********************/

static int arccos_uxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = acos(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arccos_uxd_vxf_descr =
{ "arccos_uxd_vxf", (void *) arccos_uxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt32), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arctan  *********************/

static int arctan_uxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = atan(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arctan_uxd_vxf_descr =
{ "arctan_uxd_vxf", (void *) arctan_uxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt32), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arctan2  *********************/

static int arctan2_uuxd_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    UInt32      tin1 = *(UInt32 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = atan2(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arctan2_uuxd_vsxf_descr =
{ "arctan2_uuxd_vsxf", (void *) arctan2_uuxd_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(Float64) }, { 0, 1, 0, 0 } };

static int arctan2_uuxd_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    UInt32     *tin1 =  (UInt32 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = atan2(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arctan2_uuxd_vvxf_descr =
{ "arctan2_uuxd_vvxf", (void *) arctan2_uuxd_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(Float64) }, { 0, 0, 0, 0 } };

static void _arctan2_uxd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt32  *tin0   = (UInt32 *) ((char *) input  + inboffset);
    Float64 *tout0  = (Float64 *) ((char *) output + outboffset);
    Float64 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (UInt32 *) ((char *) tin0 + inbstrides[dim]);
            net   = atan2(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _arctan2_uxd_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int arctan2_uxd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _arctan2_uxd_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(arctan2_uxd_R, CHECK_ALIGN, sizeof(UInt32), sizeof(Float64));

static void _arctan2_uxd_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt32 *tin0   = (UInt32 *) ((char *) input  + inboffset);
    Float64 *tout0 = (Float64 *) ((char *) output + outboffset);
    Float64 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (UInt32 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Float64 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = atan2(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _arctan2_uxd_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  arctan2_uxd_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _arctan2_uxd_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(arctan2_uxd_A, CHECK_ALIGN, sizeof(UInt32), sizeof(Float64));

static int arctan2_uuxd_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32      tin0 = *(UInt32 *) buffers[0];
    UInt32     *tin1 =  (UInt32 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = atan2(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arctan2_uuxd_svxf_descr =
{ "arctan2_uuxd_svxf", (void *) arctan2_uuxd_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(Float64) }, { 1, 0, 0, 0 } };
/*********************  log  *********************/

static int log_uxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_log(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor log_uxd_vxf_descr =
{ "log_uxd_vxf", (void *) log_uxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt32), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  log10  *********************/

static int log10_uxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_log10(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor log10_uxd_vxf_descr =
{ "log10_uxd_vxf", (void *) log10_uxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt32), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  exp  *********************/

static int exp_uxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = exp(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor exp_uxd_vxf_descr =
{ "exp_uxd_vxf", (void *) exp_uxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt32), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  sinh  *********************/

static int sinh_uxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = sinh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor sinh_uxd_vxf_descr =
{ "sinh_uxd_vxf", (void *) sinh_uxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt32), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  cosh  *********************/

static int cosh_uxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = cosh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor cosh_uxd_vxf_descr =
{ "cosh_uxd_vxf", (void *) cosh_uxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt32), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  tanh  *********************/

static int tanh_uxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = tanh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor tanh_uxd_vxf_descr =
{ "tanh_uxd_vxf", (void *) tanh_uxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt32), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arcsinh  *********************/

static int arcsinh_uxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_asinh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arcsinh_uxd_vxf_descr =
{ "arcsinh_uxd_vxf", (void *) arcsinh_uxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt32), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arccosh  *********************/

static int arccosh_uxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_acosh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arccosh_uxd_vxf_descr =
{ "arccosh_uxd_vxf", (void *) arccosh_uxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt32), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arctanh  *********************/

static int arctanh_uxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_atanh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arctanh_uxd_vxf_descr =
{ "arctanh_uxd_vxf", (void *) arctanh_uxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt32), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  ieeemask  *********************/
/*********************  ieeemask  *********************/
/*********************  isnan  *********************/
/*********************  isnan  *********************/
/*********************  isnan  *********************/
/*********************  isnan  *********************/
/*********************  sqrt  *********************/

static int sqrt_uxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = sqrt(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor sqrt_uxd_vxf_descr =
{ "sqrt_uxd_vxf", (void *) sqrt_uxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt32), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  equal  *********************/

static int equal_uuxB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    UInt32      tin1 = *(UInt32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 == tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor equal_uuxB_vsxv_descr =
{ "equal_uuxB_vsxv", (void *) equal_uuxB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int equal_uuxB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    UInt32     *tin1 =  (UInt32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 == *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor equal_uuxB_vvxv_descr =
{ "equal_uuxB_vvxv", (void *) equal_uuxB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int equal_uuxB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32      tin0 = *(UInt32 *) buffers[0];
    UInt32     *tin1 =  (UInt32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 == *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor equal_uuxB_svxv_descr =
{ "equal_uuxB_svxv", (void *) equal_uuxB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  not_equal  *********************/

static int not_equal_uuxB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    UInt32      tin1 = *(UInt32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 != tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor not_equal_uuxB_vsxv_descr =
{ "not_equal_uuxB_vsxv", (void *) not_equal_uuxB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int not_equal_uuxB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    UInt32     *tin1 =  (UInt32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 != *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor not_equal_uuxB_vvxv_descr =
{ "not_equal_uuxB_vvxv", (void *) not_equal_uuxB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int not_equal_uuxB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32      tin0 = *(UInt32 *) buffers[0];
    UInt32     *tin1 =  (UInt32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 != *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor not_equal_uuxB_svxv_descr =
{ "not_equal_uuxB_svxv", (void *) not_equal_uuxB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  greater  *********************/

static int greater_uuxB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    UInt32      tin1 = *(UInt32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 > tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_uuxB_vsxv_descr =
{ "greater_uuxB_vsxv", (void *) greater_uuxB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int greater_uuxB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    UInt32     *tin1 =  (UInt32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 > *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_uuxB_vvxv_descr =
{ "greater_uuxB_vvxv", (void *) greater_uuxB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int greater_uuxB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32      tin0 = *(UInt32 *) buffers[0];
    UInt32     *tin1 =  (UInt32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 > *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_uuxB_svxv_descr =
{ "greater_uuxB_svxv", (void *) greater_uuxB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  greater_equal  *********************/

static int greater_equal_uuxB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    UInt32      tin1 = *(UInt32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 >= tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_equal_uuxB_vsxv_descr =
{ "greater_equal_uuxB_vsxv", (void *) greater_equal_uuxB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int greater_equal_uuxB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    UInt32     *tin1 =  (UInt32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 >= *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_equal_uuxB_vvxv_descr =
{ "greater_equal_uuxB_vvxv", (void *) greater_equal_uuxB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int greater_equal_uuxB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32      tin0 = *(UInt32 *) buffers[0];
    UInt32     *tin1 =  (UInt32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 >= *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_equal_uuxB_svxv_descr =
{ "greater_equal_uuxB_svxv", (void *) greater_equal_uuxB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  less  *********************/

static int less_uuxB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    UInt32      tin1 = *(UInt32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 < tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_uuxB_vsxv_descr =
{ "less_uuxB_vsxv", (void *) less_uuxB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int less_uuxB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    UInt32     *tin1 =  (UInt32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 < *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_uuxB_vvxv_descr =
{ "less_uuxB_vvxv", (void *) less_uuxB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int less_uuxB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32      tin0 = *(UInt32 *) buffers[0];
    UInt32     *tin1 =  (UInt32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 < *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_uuxB_svxv_descr =
{ "less_uuxB_svxv", (void *) less_uuxB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  less_equal  *********************/

static int less_equal_uuxB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    UInt32      tin1 = *(UInt32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 <= tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_equal_uuxB_vsxv_descr =
{ "less_equal_uuxB_vsxv", (void *) less_equal_uuxB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int less_equal_uuxB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    UInt32     *tin1 =  (UInt32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 <= *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_equal_uuxB_vvxv_descr =
{ "less_equal_uuxB_vvxv", (void *) less_equal_uuxB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int less_equal_uuxB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32      tin0 = *(UInt32 *) buffers[0];
    UInt32     *tin1 =  (UInt32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 <= *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_equal_uuxB_svxv_descr =
{ "less_equal_uuxB_svxv", (void *) less_equal_uuxB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  logical_and  *********************/

static int logical_and_uuxB_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    UInt32      tin1 = *(UInt32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_and(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_and_uuxB_vsxf_descr =
{ "logical_and_uuxB_vsxf", (void *) logical_and_uuxB_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int logical_and_uuxB_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    UInt32     *tin1 =  (UInt32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_and(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_and_uuxB_vvxf_descr =
{ "logical_and_uuxB_vvxf", (void *) logical_and_uuxB_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int logical_and_uuxB_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32      tin0 = *(UInt32 *) buffers[0];
    UInt32     *tin1 =  (UInt32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_and(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_and_uuxB_svxf_descr =
{ "logical_and_uuxB_svxf", (void *) logical_and_uuxB_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  logical_or  *********************/

static int logical_or_uuxB_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    UInt32      tin1 = *(UInt32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_or(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_or_uuxB_vsxf_descr =
{ "logical_or_uuxB_vsxf", (void *) logical_or_uuxB_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int logical_or_uuxB_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    UInt32     *tin1 =  (UInt32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_or(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_or_uuxB_vvxf_descr =
{ "logical_or_uuxB_vvxf", (void *) logical_or_uuxB_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int logical_or_uuxB_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32      tin0 = *(UInt32 *) buffers[0];
    UInt32     *tin1 =  (UInt32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_or(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_or_uuxB_svxf_descr =
{ "logical_or_uuxB_svxf", (void *) logical_or_uuxB_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  logical_xor  *********************/

static int logical_xor_uuxB_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    UInt32      tin1 = *(UInt32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_xor(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_xor_uuxB_vsxf_descr =
{ "logical_xor_uuxB_vsxf", (void *) logical_xor_uuxB_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int logical_xor_uuxB_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    UInt32     *tin1 =  (UInt32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_xor(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_xor_uuxB_vvxf_descr =
{ "logical_xor_uuxB_vvxf", (void *) logical_xor_uuxB_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int logical_xor_uuxB_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32      tin0 = *(UInt32 *) buffers[0];
    UInt32     *tin1 =  (UInt32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_xor(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_xor_uuxB_svxf_descr =
{ "logical_xor_uuxB_svxf", (void *) logical_xor_uuxB_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  logical_and  *********************/
/*********************  logical_or  *********************/
/*********************  logical_xor  *********************/
/*********************  logical_not  *********************/

static int logical_not_uxB_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    Bool       *tout0 =  (Bool *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = !*tin0;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_not_uxB_vxv_descr =
{ "logical_not_uxB_vxv", (void *) logical_not_uxB_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt32), sizeof(Bool) }, { 0, 0, 0 } };
/*********************  bitwise_and  *********************/

static int bitwise_and_uuxu_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    UInt32      tin1 = *(UInt32 *) buffers[1];
    UInt32     *tout0 =  (UInt32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 & tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_and_uuxu_vsxv_descr =
{ "bitwise_and_uuxu_vsxv", (void *) bitwise_and_uuxu_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(UInt32) }, { 0, 1, 0, 0 } };

static int bitwise_and_uuxu_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    UInt32     *tin1 =  (UInt32 *) buffers[1];
    UInt32     *tout0 =  (UInt32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 & *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_and_uuxu_vvxv_descr =
{ "bitwise_and_uuxu_vvxv", (void *) bitwise_and_uuxu_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(UInt32) }, { 0, 0, 0, 0 } };

static void _bitwise_and_uxu_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt32  *tin0   = (UInt32 *) ((char *) input  + inboffset);
    UInt32 *tout0  = (UInt32 *) ((char *) output + outboffset);
    UInt32 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (UInt32 *) ((char *) tin0 + inbstrides[dim]);
            net    =     net & *tin0;
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _bitwise_and_uxu_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int bitwise_and_uxu_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _bitwise_and_uxu_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(bitwise_and_uxu_R, CHECK_ALIGN, sizeof(UInt32), sizeof(UInt32));

static void _bitwise_and_uxu_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt32 *tin0   = (UInt32 *) ((char *) input  + inboffset);
    UInt32 *tout0 = (UInt32 *) ((char *) output + outboffset);
    UInt32 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (UInt32 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (UInt32 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = lastval & *tin0;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _bitwise_and_uxu_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  bitwise_and_uxu_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _bitwise_and_uxu_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(bitwise_and_uxu_A, CHECK_ALIGN, sizeof(UInt32), sizeof(UInt32));

static int bitwise_and_uuxu_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32      tin0 = *(UInt32 *) buffers[0];
    UInt32     *tin1 =  (UInt32 *) buffers[1];
    UInt32     *tout0 =  (UInt32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 & *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_and_uuxu_svxv_descr =
{ "bitwise_and_uuxu_svxv", (void *) bitwise_and_uuxu_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(UInt32) }, { 1, 0, 0, 0 } };
/*********************  bitwise_or  *********************/

static int bitwise_or_uuxu_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    UInt32      tin1 = *(UInt32 *) buffers[1];
    UInt32     *tout0 =  (UInt32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 | tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_or_uuxu_vsxv_descr =
{ "bitwise_or_uuxu_vsxv", (void *) bitwise_or_uuxu_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(UInt32) }, { 0, 1, 0, 0 } };

static int bitwise_or_uuxu_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    UInt32     *tin1 =  (UInt32 *) buffers[1];
    UInt32     *tout0 =  (UInt32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 | *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_or_uuxu_vvxv_descr =
{ "bitwise_or_uuxu_vvxv", (void *) bitwise_or_uuxu_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(UInt32) }, { 0, 0, 0, 0 } };

static void _bitwise_or_uxu_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt32  *tin0   = (UInt32 *) ((char *) input  + inboffset);
    UInt32 *tout0  = (UInt32 *) ((char *) output + outboffset);
    UInt32 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (UInt32 *) ((char *) tin0 + inbstrides[dim]);
            net    =     net | *tin0;
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _bitwise_or_uxu_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int bitwise_or_uxu_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _bitwise_or_uxu_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(bitwise_or_uxu_R, CHECK_ALIGN, sizeof(UInt32), sizeof(UInt32));

static void _bitwise_or_uxu_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt32 *tin0   = (UInt32 *) ((char *) input  + inboffset);
    UInt32 *tout0 = (UInt32 *) ((char *) output + outboffset);
    UInt32 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (UInt32 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (UInt32 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = lastval | *tin0;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _bitwise_or_uxu_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  bitwise_or_uxu_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _bitwise_or_uxu_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(bitwise_or_uxu_A, CHECK_ALIGN, sizeof(UInt32), sizeof(UInt32));

static int bitwise_or_uuxu_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32      tin0 = *(UInt32 *) buffers[0];
    UInt32     *tin1 =  (UInt32 *) buffers[1];
    UInt32     *tout0 =  (UInt32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 | *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_or_uuxu_svxv_descr =
{ "bitwise_or_uuxu_svxv", (void *) bitwise_or_uuxu_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(UInt32) }, { 1, 0, 0, 0 } };
/*********************  bitwise_xor  *********************/

static int bitwise_xor_uuxu_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    UInt32      tin1 = *(UInt32 *) buffers[1];
    UInt32     *tout0 =  (UInt32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 ^ tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_xor_uuxu_vsxv_descr =
{ "bitwise_xor_uuxu_vsxv", (void *) bitwise_xor_uuxu_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(UInt32) }, { 0, 1, 0, 0 } };

static int bitwise_xor_uuxu_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    UInt32     *tin1 =  (UInt32 *) buffers[1];
    UInt32     *tout0 =  (UInt32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 ^ *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_xor_uuxu_vvxv_descr =
{ "bitwise_xor_uuxu_vvxv", (void *) bitwise_xor_uuxu_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(UInt32) }, { 0, 0, 0, 0 } };

static void _bitwise_xor_uxu_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt32  *tin0   = (UInt32 *) ((char *) input  + inboffset);
    UInt32 *tout0  = (UInt32 *) ((char *) output + outboffset);
    UInt32 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (UInt32 *) ((char *) tin0 + inbstrides[dim]);
            net    =     net ^ *tin0;
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _bitwise_xor_uxu_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int bitwise_xor_uxu_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _bitwise_xor_uxu_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(bitwise_xor_uxu_R, CHECK_ALIGN, sizeof(UInt32), sizeof(UInt32));

static void _bitwise_xor_uxu_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt32 *tin0   = (UInt32 *) ((char *) input  + inboffset);
    UInt32 *tout0 = (UInt32 *) ((char *) output + outboffset);
    UInt32 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (UInt32 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (UInt32 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = lastval ^ *tin0;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _bitwise_xor_uxu_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  bitwise_xor_uxu_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _bitwise_xor_uxu_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(bitwise_xor_uxu_A, CHECK_ALIGN, sizeof(UInt32), sizeof(UInt32));

static int bitwise_xor_uuxu_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32      tin0 = *(UInt32 *) buffers[0];
    UInt32     *tin1 =  (UInt32 *) buffers[1];
    UInt32     *tout0 =  (UInt32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 ^ *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_xor_uuxu_svxv_descr =
{ "bitwise_xor_uuxu_svxv", (void *) bitwise_xor_uuxu_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(UInt32) }, { 1, 0, 0, 0 } };
/*********************  bitwise_not  *********************/

static int bitwise_not_uxu_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    UInt32     *tout0 =  (UInt32 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ~*tin0;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_not_uxu_vxv_descr =
{ "bitwise_not_uxu_vxv", (void *) bitwise_not_uxu_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt32), sizeof(UInt32) }, { 0, 0, 0 } };
/*********************  bitwise_not  *********************/
/*********************  rshift  *********************/

static int rshift_uuxu_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    UInt32      tin1 = *(UInt32 *) buffers[1];
    UInt32     *tout0 =  (UInt32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 >> tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor rshift_uuxu_vsxv_descr =
{ "rshift_uuxu_vsxv", (void *) rshift_uuxu_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(UInt32) }, { 0, 1, 0, 0 } };

static int rshift_uuxu_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    UInt32     *tin1 =  (UInt32 *) buffers[1];
    UInt32     *tout0 =  (UInt32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 >> *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor rshift_uuxu_vvxv_descr =
{ "rshift_uuxu_vvxv", (void *) rshift_uuxu_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(UInt32) }, { 0, 0, 0, 0 } };

static int rshift_uuxu_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32      tin0 = *(UInt32 *) buffers[0];
    UInt32     *tin1 =  (UInt32 *) buffers[1];
    UInt32     *tout0 =  (UInt32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 >> *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor rshift_uuxu_svxv_descr =
{ "rshift_uuxu_svxv", (void *) rshift_uuxu_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(UInt32) }, { 1, 0, 0, 0 } };
/*********************  lshift  *********************/

static int lshift_uuxu_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    UInt32      tin1 = *(UInt32 *) buffers[1];
    UInt32     *tout0 =  (UInt32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 << tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor lshift_uuxu_vsxv_descr =
{ "lshift_uuxu_vsxv", (void *) lshift_uuxu_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(UInt32) }, { 0, 1, 0, 0 } };

static int lshift_uuxu_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    UInt32     *tin1 =  (UInt32 *) buffers[1];
    UInt32     *tout0 =  (UInt32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 << *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor lshift_uuxu_vvxv_descr =
{ "lshift_uuxu_vvxv", (void *) lshift_uuxu_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(UInt32) }, { 0, 0, 0, 0 } };

static int lshift_uuxu_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32      tin0 = *(UInt32 *) buffers[0];
    UInt32     *tin1 =  (UInt32 *) buffers[1];
    UInt32     *tout0 =  (UInt32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 << *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor lshift_uuxu_svxv_descr =
{ "lshift_uuxu_svxv", (void *) lshift_uuxu_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(UInt32) }, { 1, 0, 0, 0 } };
/*********************  floor  *********************/

static int floor_uxu_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    UInt32     *tout0 =  (UInt32 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = (*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor floor_uxu_vxf_descr =
{ "floor_uxu_vxf", (void *) floor_uxu_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt32), sizeof(UInt32) }, { 0, 0, 0 } };
/*********************  floor  *********************/
/*********************  ceil  *********************/

static int ceil_uxu_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    UInt32     *tout0 =  (UInt32 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = (*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor ceil_uxu_vxf_descr =
{ "ceil_uxu_vxf", (void *) ceil_uxu_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt32), sizeof(UInt32) }, { 0, 0, 0 } };
/*********************  ceil  *********************/
/*********************  maximum  *********************/

static int maximum_uuxu_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    UInt32      tin1 = *(UInt32 *) buffers[1];
    UInt32     *tout0 =  (UInt32 *) buffers[2];
UInt32 temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufmaximum(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor maximum_uuxu_vsxf_descr =
{ "maximum_uuxu_vsxf", (void *) maximum_uuxu_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(UInt32) }, { 0, 1, 0, 0 } };

static int maximum_uuxu_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    UInt32     *tin1 =  (UInt32 *) buffers[1];
    UInt32     *tout0 =  (UInt32 *) buffers[2];
UInt32 temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufmaximum(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor maximum_uuxu_vvxf_descr =
{ "maximum_uuxu_vvxf", (void *) maximum_uuxu_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(UInt32) }, { 0, 0, 0, 0 } };

static void _maximum_uxu_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt32  *tin0   = (UInt32 *) ((char *) input  + inboffset);
    UInt32 *tout0  = (UInt32 *) ((char *) output + outboffset);
    UInt32 net;
    UInt32 temp1, temp2;
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (UInt32 *) ((char *) tin0 + inbstrides[dim]);
            net   = ufmaximum(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _maximum_uxu_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int maximum_uxu_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _maximum_uxu_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(maximum_uxu_R, CHECK_ALIGN, sizeof(UInt32), sizeof(UInt32));

static void _maximum_uxu_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt32 *tin0   = (UInt32 *) ((char *) input  + inboffset);
    UInt32 *tout0 = (UInt32 *) ((char *) output + outboffset);
    UInt32 lastval;
    UInt32 temp1, temp2;
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (UInt32 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (UInt32 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = ufmaximum(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _maximum_uxu_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  maximum_uxu_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _maximum_uxu_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(maximum_uxu_A, CHECK_ALIGN, sizeof(UInt32), sizeof(UInt32));

static int maximum_uuxu_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32      tin0 = *(UInt32 *) buffers[0];
    UInt32     *tin1 =  (UInt32 *) buffers[1];
    UInt32     *tout0 =  (UInt32 *) buffers[2];
UInt32 temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufmaximum(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor maximum_uuxu_svxf_descr =
{ "maximum_uuxu_svxf", (void *) maximum_uuxu_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(UInt32) }, { 1, 0, 0, 0 } };
/*********************  minimum  *********************/

static int minimum_uuxu_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    UInt32      tin1 = *(UInt32 *) buffers[1];
    UInt32     *tout0 =  (UInt32 *) buffers[2];
UInt32 temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufminimum(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor minimum_uuxu_vsxf_descr =
{ "minimum_uuxu_vsxf", (void *) minimum_uuxu_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(UInt32) }, { 0, 1, 0, 0 } };

static int minimum_uuxu_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    UInt32     *tin1 =  (UInt32 *) buffers[1];
    UInt32     *tout0 =  (UInt32 *) buffers[2];
UInt32 temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufminimum(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor minimum_uuxu_vvxf_descr =
{ "minimum_uuxu_vvxf", (void *) minimum_uuxu_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(UInt32) }, { 0, 0, 0, 0 } };

static void _minimum_uxu_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt32  *tin0   = (UInt32 *) ((char *) input  + inboffset);
    UInt32 *tout0  = (UInt32 *) ((char *) output + outboffset);
    UInt32 net;
    UInt32 temp1, temp2;
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (UInt32 *) ((char *) tin0 + inbstrides[dim]);
            net   = ufminimum(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _minimum_uxu_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int minimum_uxu_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _minimum_uxu_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(minimum_uxu_R, CHECK_ALIGN, sizeof(UInt32), sizeof(UInt32));

static void _minimum_uxu_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt32 *tin0   = (UInt32 *) ((char *) input  + inboffset);
    UInt32 *tout0 = (UInt32 *) ((char *) output + outboffset);
    UInt32 lastval;
    UInt32 temp1, temp2;
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (UInt32 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (UInt32 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = ufminimum(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _minimum_uxu_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  minimum_uxu_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _minimum_uxu_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(minimum_uxu_A, CHECK_ALIGN, sizeof(UInt32), sizeof(UInt32));

static int minimum_uuxu_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32      tin0 = *(UInt32 *) buffers[0];
    UInt32     *tin1 =  (UInt32 *) buffers[1];
    UInt32     *tout0 =  (UInt32 *) buffers[2];
UInt32 temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufminimum(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor minimum_uuxu_svxf_descr =
{ "minimum_uuxu_svxf", (void *) minimum_uuxu_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(UInt32) }, { 1, 0, 0, 0 } };
/*********************  fabs  *********************/

static int fabs_uxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = fabs(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor fabs_uxd_vxf_descr =
{ "fabs_uxd_vxf", (void *) fabs_uxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt32), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  _round  *********************/

static int _round_uxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_round(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor _round_uxd_vxf_descr =
{ "_round_uxd_vxf", (void *) _round_uxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt32), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  hypot  *********************/

static int hypot_uuxd_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    UInt32      tin1 = *(UInt32 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = hypot(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor hypot_uuxd_vsxf_descr =
{ "hypot_uuxd_vsxf", (void *) hypot_uuxd_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(Float64) }, { 0, 1, 0, 0 } };

static int hypot_uuxd_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    UInt32     *tin1 =  (UInt32 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = hypot(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor hypot_uuxd_vvxf_descr =
{ "hypot_uuxd_vvxf", (void *) hypot_uuxd_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(Float64) }, { 0, 0, 0, 0 } };

static void _hypot_uxd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt32  *tin0   = (UInt32 *) ((char *) input  + inboffset);
    Float64 *tout0  = (Float64 *) ((char *) output + outboffset);
    Float64 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (UInt32 *) ((char *) tin0 + inbstrides[dim]);
            net   = hypot(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _hypot_uxd_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int hypot_uxd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _hypot_uxd_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(hypot_uxd_R, CHECK_ALIGN, sizeof(UInt32), sizeof(Float64));

static void _hypot_uxd_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt32 *tin0   = (UInt32 *) ((char *) input  + inboffset);
    Float64 *tout0 = (Float64 *) ((char *) output + outboffset);
    Float64 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (UInt32 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Float64 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = hypot(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _hypot_uxd_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  hypot_uxd_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _hypot_uxd_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(hypot_uxd_A, CHECK_ALIGN, sizeof(UInt32), sizeof(Float64));

static int hypot_uuxd_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32      tin0 = *(UInt32 *) buffers[0];
    UInt32     *tin1 =  (UInt32 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = hypot(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor hypot_uuxd_svxf_descr =
{ "hypot_uuxd_svxf", (void *) hypot_uuxd_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt32), sizeof(UInt32), sizeof(Float64) }, { 1, 0, 0, 0 } };
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

static PyMethodDef _ufuncUInt32Methods[] = {

	{NULL,      NULL}        /* Sentinel */
};

static PyObject *init_funcDict(void) {
    PyObject *dict, *keytuple, *cfunc;
    dict = PyDict_New();
    /* minus_uxu_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","minus","v","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minus_uxu_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_uuxu_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","add","vs","UInt32","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_uuxu_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_uuxu_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","add","vv","UInt32","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_uuxu_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_uxu_R */
    keytuple=Py_BuildValue("ss((s)(s))","add","R","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_uxu_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_uxu_A */
    keytuple=Py_BuildValue("ss((s)(s))","add","A","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_uxu_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_uuxu_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","add","sv","UInt32","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_uuxu_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_uuxu_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","subtract","vs","UInt32","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_uuxu_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_uuxu_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","subtract","vv","UInt32","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_uuxu_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_uxu_R */
    keytuple=Py_BuildValue("ss((s)(s))","subtract","R","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_uxu_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_uxu_A */
    keytuple=Py_BuildValue("ss((s)(s))","subtract","A","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_uxu_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_uuxu_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","subtract","sv","UInt32","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_uuxu_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* multiply_uuxu_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","multiply","vs","UInt32","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&multiply_uuxu_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* multiply_uuxu_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","multiply","vv","UInt32","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&multiply_uuxu_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* multiply_uxu_R */
    keytuple=Py_BuildValue("ss((s)(s))","multiply","R","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&multiply_uxu_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* multiply_uxu_A */
    keytuple=Py_BuildValue("ss((s)(s))","multiply","A","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&multiply_uxu_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* multiply_uuxu_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","multiply","sv","UInt32","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&multiply_uuxu_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_uuxu_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","divide","vs","UInt32","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_uuxu_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_uuxu_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","divide","vv","UInt32","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_uuxu_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_uxu_R */
    keytuple=Py_BuildValue("ss((s)(s))","divide","R","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_uxu_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_uxu_A */
    keytuple=Py_BuildValue("ss((s)(s))","divide","A","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_uxu_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_uuxu_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","divide","sv","UInt32","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_uuxu_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_divide_uuxu_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","floor_divide","vs","UInt32","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_divide_uuxu_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_divide_uuxu_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","floor_divide","vv","UInt32","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_divide_uuxu_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_divide_uxu_R */
    keytuple=Py_BuildValue("ss((s)(s))","floor_divide","R","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_divide_uxu_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_divide_uxu_A */
    keytuple=Py_BuildValue("ss((s)(s))","floor_divide","A","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_divide_uxu_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_divide_uuxu_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","floor_divide","sv","UInt32","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_divide_uuxu_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_uuxf_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","true_divide","vs","UInt32","UInt32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_uuxf_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_uuxf_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","true_divide","vv","UInt32","UInt32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_uuxf_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_uxf_R */
    keytuple=Py_BuildValue("ss((s)(s))","true_divide","R","UInt32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_uxf_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_uxf_A */
    keytuple=Py_BuildValue("ss((s)(s))","true_divide","A","UInt32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_uxf_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_uuxf_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","true_divide","sv","UInt32","UInt32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_uuxf_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_uuxu_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","remainder","vs","UInt32","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_uuxu_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_uuxu_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","remainder","vv","UInt32","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_uuxu_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_uxu_R */
    keytuple=Py_BuildValue("ss((s)(s))","remainder","R","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_uxu_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_uxu_A */
    keytuple=Py_BuildValue("ss((s)(s))","remainder","A","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_uxu_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_uuxu_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","remainder","sv","UInt32","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_uuxu_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_uuxu_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","power","vs","UInt32","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_uuxu_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_uuxu_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","power","vv","UInt32","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_uuxu_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_uxu_R */
    keytuple=Py_BuildValue("ss((s)(s))","power","R","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_uxu_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_uxu_A */
    keytuple=Py_BuildValue("ss((s)(s))","power","A","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_uxu_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_uuxu_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","power","sv","UInt32","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_uuxu_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* abs_uxu_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","abs","v","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&abs_uxu_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* sin_uxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","sin","v","UInt32","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&sin_uxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* cos_uxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","cos","v","UInt32","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&cos_uxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* tan_uxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","tan","v","UInt32","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&tan_uxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arcsin_uxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arcsin","v","UInt32","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arcsin_uxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arccos_uxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arccos","v","UInt32","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arccos_uxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan_uxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arctan","v","UInt32","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan_uxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan2_uuxd_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","arctan2","vs","UInt32","UInt32","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan2_uuxd_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan2_uuxd_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","arctan2","vv","UInt32","UInt32","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan2_uuxd_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan2_uxd_R */
    keytuple=Py_BuildValue("ss((s)(s))","arctan2","R","UInt32","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan2_uxd_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan2_uxd_A */
    keytuple=Py_BuildValue("ss((s)(s))","arctan2","A","UInt32","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan2_uxd_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan2_uuxd_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","arctan2","sv","UInt32","UInt32","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan2_uuxd_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* log_uxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","log","v","UInt32","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&log_uxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* log10_uxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","log10","v","UInt32","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&log10_uxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* exp_uxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","exp","v","UInt32","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&exp_uxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* sinh_uxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","sinh","v","UInt32","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&sinh_uxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* cosh_uxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","cosh","v","UInt32","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&cosh_uxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* tanh_uxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","tanh","v","UInt32","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&tanh_uxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arcsinh_uxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arcsinh","v","UInt32","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arcsinh_uxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arccosh_uxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arccosh","v","UInt32","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arccosh_uxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctanh_uxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arctanh","v","UInt32","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctanh_uxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* sqrt_uxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","sqrt","v","UInt32","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&sqrt_uxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* equal_uuxB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","equal","vs","UInt32","UInt32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&equal_uuxB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* equal_uuxB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","equal","vv","UInt32","UInt32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&equal_uuxB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* equal_uuxB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","equal","sv","UInt32","UInt32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&equal_uuxB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* not_equal_uuxB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","not_equal","vs","UInt32","UInt32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&not_equal_uuxB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* not_equal_uuxB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","not_equal","vv","UInt32","UInt32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&not_equal_uuxB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* not_equal_uuxB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","not_equal","sv","UInt32","UInt32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&not_equal_uuxB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_uuxB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater","vs","UInt32","UInt32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_uuxB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_uuxB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater","vv","UInt32","UInt32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_uuxB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_uuxB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater","sv","UInt32","UInt32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_uuxB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_equal_uuxB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater_equal","vs","UInt32","UInt32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_equal_uuxB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_equal_uuxB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater_equal","vv","UInt32","UInt32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_equal_uuxB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_equal_uuxB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater_equal","sv","UInt32","UInt32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_equal_uuxB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_uuxB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less","vs","UInt32","UInt32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_uuxB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_uuxB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less","vv","UInt32","UInt32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_uuxB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_uuxB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less","sv","UInt32","UInt32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_uuxB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_equal_uuxB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less_equal","vs","UInt32","UInt32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_equal_uuxB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_equal_uuxB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less_equal","vv","UInt32","UInt32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_equal_uuxB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_equal_uuxB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less_equal","sv","UInt32","UInt32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_equal_uuxB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_and_uuxB_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_and","vs","UInt32","UInt32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_and_uuxB_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_and_uuxB_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_and","vv","UInt32","UInt32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_and_uuxB_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_and_uuxB_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_and","sv","UInt32","UInt32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_and_uuxB_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_or_uuxB_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_or","vs","UInt32","UInt32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_or_uuxB_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_or_uuxB_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_or","vv","UInt32","UInt32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_or_uuxB_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_or_uuxB_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_or","sv","UInt32","UInt32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_or_uuxB_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_xor_uuxB_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_xor","vs","UInt32","UInt32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_xor_uuxB_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_xor_uuxB_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_xor","vv","UInt32","UInt32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_xor_uuxB_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_xor_uuxB_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_xor","sv","UInt32","UInt32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_xor_uuxB_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_not_uxB_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","logical_not","v","UInt32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_not_uxB_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_and_uuxu_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_and","vs","UInt32","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_and_uuxu_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_and_uuxu_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_and","vv","UInt32","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_and_uuxu_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_and_uxu_R */
    keytuple=Py_BuildValue("ss((s)(s))","bitwise_and","R","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_and_uxu_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_and_uxu_A */
    keytuple=Py_BuildValue("ss((s)(s))","bitwise_and","A","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_and_uxu_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_and_uuxu_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_and","sv","UInt32","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_and_uuxu_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_or_uuxu_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_or","vs","UInt32","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_or_uuxu_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_or_uuxu_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_or","vv","UInt32","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_or_uuxu_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_or_uxu_R */
    keytuple=Py_BuildValue("ss((s)(s))","bitwise_or","R","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_or_uxu_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_or_uxu_A */
    keytuple=Py_BuildValue("ss((s)(s))","bitwise_or","A","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_or_uxu_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_or_uuxu_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_or","sv","UInt32","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_or_uuxu_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_xor_uuxu_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_xor","vs","UInt32","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_xor_uuxu_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_xor_uuxu_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_xor","vv","UInt32","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_xor_uuxu_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_xor_uxu_R */
    keytuple=Py_BuildValue("ss((s)(s))","bitwise_xor","R","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_xor_uxu_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_xor_uxu_A */
    keytuple=Py_BuildValue("ss((s)(s))","bitwise_xor","A","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_xor_uxu_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_xor_uuxu_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_xor","sv","UInt32","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_xor_uuxu_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_not_uxu_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","bitwise_not","v","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_not_uxu_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* rshift_uuxu_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","rshift","vs","UInt32","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&rshift_uuxu_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* rshift_uuxu_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","rshift","vv","UInt32","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&rshift_uuxu_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* rshift_uuxu_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","rshift","sv","UInt32","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&rshift_uuxu_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* lshift_uuxu_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","lshift","vs","UInt32","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&lshift_uuxu_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* lshift_uuxu_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","lshift","vv","UInt32","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&lshift_uuxu_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* lshift_uuxu_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","lshift","sv","UInt32","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&lshift_uuxu_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_uxu_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","floor","v","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_uxu_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* ceil_uxu_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","ceil","v","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&ceil_uxu_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_uuxu_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","maximum","vs","UInt32","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_uuxu_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_uuxu_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","maximum","vv","UInt32","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_uuxu_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_uxu_R */
    keytuple=Py_BuildValue("ss((s)(s))","maximum","R","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_uxu_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_uxu_A */
    keytuple=Py_BuildValue("ss((s)(s))","maximum","A","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_uxu_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_uuxu_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","maximum","sv","UInt32","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_uuxu_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_uuxu_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","minimum","vs","UInt32","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_uuxu_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_uuxu_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","minimum","vv","UInt32","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_uuxu_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_uxu_R */
    keytuple=Py_BuildValue("ss((s)(s))","minimum","R","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_uxu_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_uxu_A */
    keytuple=Py_BuildValue("ss((s)(s))","minimum","A","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_uxu_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_uuxu_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","minimum","sv","UInt32","UInt32","UInt32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_uuxu_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* fabs_uxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","fabs","v","UInt32","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&fabs_uxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* _round_uxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","_round","v","UInt32","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&_round_uxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_uuxd_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","hypot","vs","UInt32","UInt32","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_uuxd_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_uuxd_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","hypot","vv","UInt32","UInt32","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_uuxd_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_uxd_R */
    keytuple=Py_BuildValue("ss((s)(s))","hypot","R","UInt32","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_uxd_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_uxd_A */
    keytuple=Py_BuildValue("ss((s)(s))","hypot","A","UInt32","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_uxd_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_uuxd_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","hypot","sv","UInt32","UInt32","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_uuxd_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    return dict;
}

/* platform independent*/
#ifdef MS_WIN32
__declspec(dllexport)
#endif
void init_ufuncUInt32(void) {
    PyObject *m, *d, *functions;
    m = Py_InitModule("_ufuncUInt32", _ufuncUInt32Methods);
    d = PyModule_GetDict(m);
    import_libnumarray();
    functions = init_funcDict();
    PyDict_SetItemString(d, "functionDict", functions);
    Py_DECREF(functions);
    ADD_VERSION(m);
}
