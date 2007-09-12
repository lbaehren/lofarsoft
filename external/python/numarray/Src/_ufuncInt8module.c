
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

static int minus_1x1_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Int8       *tout0 =  (Int8 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = -*tin0;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor minus_1x1_vxv_descr =
{ "minus_1x1_vxv", (void *) minus_1x1_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int8), sizeof(Int8) }, { 0, 0, 0 } };
/*********************  add  *********************/

static int add_11x1_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Int8        tin1 = *(Int8 *) buffers[1];
    Int8       *tout0 =  (Int8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 + tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor add_11x1_vsxv_descr =
{ "add_11x1_vsxv", (void *) add_11x1_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Int8) }, { 0, 1, 0, 0 } };

static int add_11x1_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Int8       *tin1 =  (Int8 *) buffers[1];
    Int8       *tout0 =  (Int8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 + *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor add_11x1_vvxv_descr =
{ "add_11x1_vvxv", (void *) add_11x1_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Int8) }, { 0, 0, 0, 0 } };

static void _add_1x1_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int8  *tin0   = (Int8 *) ((char *) input  + inboffset);
    Int8 *tout0  = (Int8 *) ((char *) output + outboffset);
    Int8 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Int8 *) ((char *) tin0 + inbstrides[dim]);
            net    =     net + *tin0;
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _add_1x1_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int add_1x1_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _add_1x1_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(add_1x1_R, CHECK_ALIGN, sizeof(Int8), sizeof(Int8));

static void _add_1x1_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int8 *tin0   = (Int8 *) ((char *) input  + inboffset);
    Int8 *tout0 = (Int8 *) ((char *) output + outboffset);
    Int8 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Int8 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Int8 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = lastval + *tin0;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _add_1x1_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  add_1x1_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _add_1x1_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(add_1x1_A, CHECK_ALIGN, sizeof(Int8), sizeof(Int8));

static int add_11x1_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8        tin0 = *(Int8 *) buffers[0];
    Int8       *tin1 =  (Int8 *) buffers[1];
    Int8       *tout0 =  (Int8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 + *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor add_11x1_svxv_descr =
{ "add_11x1_svxv", (void *) add_11x1_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Int8) }, { 1, 0, 0, 0 } };
/*********************  subtract  *********************/

static int subtract_11x1_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Int8        tin1 = *(Int8 *) buffers[1];
    Int8       *tout0 =  (Int8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 - tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor subtract_11x1_vsxv_descr =
{ "subtract_11x1_vsxv", (void *) subtract_11x1_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Int8) }, { 0, 1, 0, 0 } };

static int subtract_11x1_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Int8       *tin1 =  (Int8 *) buffers[1];
    Int8       *tout0 =  (Int8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 - *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor subtract_11x1_vvxv_descr =
{ "subtract_11x1_vvxv", (void *) subtract_11x1_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Int8) }, { 0, 0, 0, 0 } };

static void _subtract_1x1_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int8  *tin0   = (Int8 *) ((char *) input  + inboffset);
    Int8 *tout0  = (Int8 *) ((char *) output + outboffset);
    Int8 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Int8 *) ((char *) tin0 + inbstrides[dim]);
            net    =     net - *tin0;
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _subtract_1x1_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int subtract_1x1_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _subtract_1x1_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(subtract_1x1_R, CHECK_ALIGN, sizeof(Int8), sizeof(Int8));

static void _subtract_1x1_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int8 *tin0   = (Int8 *) ((char *) input  + inboffset);
    Int8 *tout0 = (Int8 *) ((char *) output + outboffset);
    Int8 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Int8 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Int8 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = lastval - *tin0;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _subtract_1x1_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  subtract_1x1_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _subtract_1x1_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(subtract_1x1_A, CHECK_ALIGN, sizeof(Int8), sizeof(Int8));

static int subtract_11x1_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8        tin0 = *(Int8 *) buffers[0];
    Int8       *tin1 =  (Int8 *) buffers[1];
    Int8       *tout0 =  (Int8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 - *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor subtract_11x1_svxv_descr =
{ "subtract_11x1_svxv", (void *) subtract_11x1_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Int8) }, { 1, 0, 0, 0 } };
/*********************  multiply  *********************/

static int multiply_11x1_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Int8        tin1 = *(Int8 *) buffers[1];
    Int8       *tout0 =  (Int8 *) buffers[2];
Int32 temp;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        temp = ((Int32) *tin0) * ((Int32) tin1);
    if (temp > 127) temp = int_overflow_error(127.);
if (temp < -128) temp = int_overflow_error(-128.);
    *tout0 = (Int8) temp;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor multiply_11x1_vsxv_descr =
{ "multiply_11x1_vsxv", (void *) multiply_11x1_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Int8) }, { 0, 1, 0, 0 } };

static int multiply_11x1_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Int8       *tin1 =  (Int8 *) buffers[1];
    Int8       *tout0 =  (Int8 *) buffers[2];
Int32 temp;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        temp = ((Int32) *tin0) * ((Int32) *tin1);
    if (temp > 127) temp = int_overflow_error(127.);
if (temp < -128) temp = int_overflow_error(-128.);
    *tout0 = (Int8) temp;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor multiply_11x1_vvxv_descr =
{ "multiply_11x1_vvxv", (void *) multiply_11x1_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Int8) }, { 0, 0, 0, 0 } };

static void _multiply_1x1_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int8  *tin0   = (Int8 *) ((char *) input  + inboffset);
    Int8 *tout0  = (Int8 *) ((char *) output + outboffset);
    Int8 net;
    Int32 temp;
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Int8 *) ((char *) tin0 + inbstrides[dim]);
            temp = ((Int32) net) * ((Int32) *tin0);
    if (temp > 127) temp = int_overflow_error(127.);
if (temp < -128) temp = int_overflow_error(-128.);
    net = (Int8) temp;
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _multiply_1x1_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int multiply_1x1_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _multiply_1x1_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(multiply_1x1_R, CHECK_ALIGN, sizeof(Int8), sizeof(Int8));

static void _multiply_1x1_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int8 *tin0   = (Int8 *) ((char *) input  + inboffset);
    Int8 *tout0 = (Int8 *) ((char *) output + outboffset);
    Int8 lastval;
    Int32 temp;
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Int8 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Int8 *) ((char *) tout0 + outbstrides[dim]);
            temp = ((Int32) lastval) * ((Int32) *tin0);
    if (temp > 127) temp = int_overflow_error(127.);
if (temp < -128) temp = int_overflow_error(-128.);
    *tout0 = (Int8) temp;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _multiply_1x1_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  multiply_1x1_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _multiply_1x1_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(multiply_1x1_A, CHECK_ALIGN, sizeof(Int8), sizeof(Int8));

static int multiply_11x1_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8        tin0 = *(Int8 *) buffers[0];
    Int8       *tin1 =  (Int8 *) buffers[1];
    Int8       *tout0 =  (Int8 *) buffers[2];
Int32 temp;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        temp = ((Int32) tin0) * ((Int32) *tin1);
    if (temp > 127) temp = int_overflow_error(127.);
if (temp < -128) temp = int_overflow_error(-128.);
    *tout0 = (Int8) temp;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor multiply_11x1_svxv_descr =
{ "multiply_11x1_svxv", (void *) multiply_11x1_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Int8) }, { 1, 0, 0, 0 } };
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  divide  *********************/

static int divide_11x1_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Int8        tin1 = *(Int8 *) buffers[1];
    Int8       *tout0 =  (Int8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((tin1==0) ? int_dividebyzero_error(tin1, *tin0) : *tin0 / tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor divide_11x1_vsxv_descr =
{ "divide_11x1_vsxv", (void *) divide_11x1_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Int8) }, { 0, 1, 0, 0 } };

static int divide_11x1_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Int8       *tin1 =  (Int8 *) buffers[1];
    Int8       *tout0 =  (Int8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, *tin0) : *tin0 / *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor divide_11x1_vvxv_descr =
{ "divide_11x1_vvxv", (void *) divide_11x1_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Int8) }, { 0, 0, 0, 0 } };

static void _divide_1x1_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int8  *tin0   = (Int8 *) ((char *) input  + inboffset);
    Int8 *tout0  = (Int8 *) ((char *) output + outboffset);
    Int8 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Int8 *) ((char *) tin0 + inbstrides[dim]);
            net = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0) : net / *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _divide_1x1_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int divide_1x1_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _divide_1x1_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(divide_1x1_R, CHECK_ALIGN, sizeof(Int8), sizeof(Int8));

static void _divide_1x1_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int8 *tin0   = (Int8 *) ((char *) input  + inboffset);
    Int8 *tout0 = (Int8 *) ((char *) output + outboffset);
    Int8 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Int8 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Int8 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0) : lastval / *tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _divide_1x1_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  divide_1x1_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _divide_1x1_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(divide_1x1_A, CHECK_ALIGN, sizeof(Int8), sizeof(Int8));

static int divide_11x1_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8        tin0 = *(Int8 *) buffers[0];
    Int8       *tin1 =  (Int8 *) buffers[1];
    Int8       *tout0 =  (Int8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, 0) : tin0 / *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor divide_11x1_svxv_descr =
{ "divide_11x1_svxv", (void *) divide_11x1_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Int8) }, { 1, 0, 0, 0 } };
/*********************  divide  *********************/
/*********************  floor_divide  *********************/

static int floor_divide_11x1_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Int8        tin1 = *(Int8 *) buffers[1];
    Int8       *tout0 =  (Int8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = (( tin1==0) ? int_dividebyzero_error( tin1, *tin0) : floor(*tin0   / (double)  tin1));
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor floor_divide_11x1_vsxv_descr =
{ "floor_divide_11x1_vsxv", (void *) floor_divide_11x1_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Int8) }, { 0, 1, 0, 0 } };

static int floor_divide_11x1_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Int8       *tin1 =  (Int8 *) buffers[1];
    Int8       *tout0 =  (Int8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, *tin0) : floor(*tin0   / (double) *tin1));
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor floor_divide_11x1_vvxv_descr =
{ "floor_divide_11x1_vvxv", (void *) floor_divide_11x1_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Int8) }, { 0, 0, 0, 0 } };

static void _floor_divide_1x1_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int8  *tin0   = (Int8 *) ((char *) input  + inboffset);
    Int8 *tout0  = (Int8 *) ((char *) output + outboffset);
    Int8 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Int8 *) ((char *) tin0 + inbstrides[dim]);
            net    = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0)     : floor(net     / (double) *tin0));
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _floor_divide_1x1_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int floor_divide_1x1_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _floor_divide_1x1_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(floor_divide_1x1_R, CHECK_ALIGN, sizeof(Int8), sizeof(Int8));

static void _floor_divide_1x1_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int8 *tin0   = (Int8 *) ((char *) input  + inboffset);
    Int8 *tout0 = (Int8 *) ((char *) output + outboffset);
    Int8 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Int8 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Int8 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0)     : floor(lastval / (double) *tin0));
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _floor_divide_1x1_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  floor_divide_1x1_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _floor_divide_1x1_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(floor_divide_1x1_A, CHECK_ALIGN, sizeof(Int8), sizeof(Int8));

static int floor_divide_11x1_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8        tin0 = *(Int8 *) buffers[0];
    Int8       *tin1 =  (Int8 *) buffers[1];
    Int8       *tout0 =  (Int8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, 0)     : floor(tin0    / (double) *tin1));
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor floor_divide_11x1_svxv_descr =
{ "floor_divide_11x1_svxv", (void *) floor_divide_11x1_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Int8) }, { 1, 0, 0, 0 } };
/*********************  floor_divide  *********************/
/*********************  true_divide  *********************/

static int true_divide_11xf_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Int8        tin1 = *(Int8 *) buffers[1];
    Float32    *tout0 =  (Float32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = (( tin1==0) ? int_dividebyzero_error( tin1, *tin0) : *tin0         / (double) tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor true_divide_11xf_vsxv_descr =
{ "true_divide_11xf_vsxv", (void *) true_divide_11xf_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Float32) }, { 0, 1, 0, 0 } };

static int true_divide_11xf_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Int8       *tin1 =  (Int8 *) buffers[1];
    Float32    *tout0 =  (Float32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, *tin0) : *tin0         / (double) *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor true_divide_11xf_vvxv_descr =
{ "true_divide_11xf_vvxv", (void *) true_divide_11xf_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Float32) }, { 0, 0, 0, 0 } };

static void _true_divide_1xf_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int8  *tin0   = (Int8 *) ((char *) input  + inboffset);
    Float32 *tout0  = (Float32 *) ((char *) output + outboffset);
    Float32 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Int8 *) ((char *) tin0 + inbstrides[dim]);
            net    = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0)     : net           / (double) *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _true_divide_1xf_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int true_divide_1xf_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _true_divide_1xf_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(true_divide_1xf_R, CHECK_ALIGN, sizeof(Int8), sizeof(Float32));

static void _true_divide_1xf_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int8 *tin0   = (Int8 *) ((char *) input  + inboffset);
    Float32 *tout0 = (Float32 *) ((char *) output + outboffset);
    Float32 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Int8 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Float32 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0)     : lastval       / (double) *tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _true_divide_1xf_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  true_divide_1xf_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _true_divide_1xf_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(true_divide_1xf_A, CHECK_ALIGN, sizeof(Int8), sizeof(Float32));

static int true_divide_11xf_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8        tin0 = *(Int8 *) buffers[0];
    Int8       *tin1 =  (Int8 *) buffers[1];
    Float32    *tout0 =  (Float32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, 0)     : tin0          / (double) *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor true_divide_11xf_svxv_descr =
{ "true_divide_11xf_svxv", (void *) true_divide_11xf_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Float32) }, { 1, 0, 0, 0 } };
/*********************  true_divide  *********************/
/*********************  remainder  *********************/

static int remainder_11x1_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Int8        tin1 = *(Int8 *) buffers[1];
    Int8       *tout0 =  (Int8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((tin1==0) ? int_dividebyzero_error(tin1, *tin0) : *tin0  %  tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor remainder_11x1_vsxv_descr =
{ "remainder_11x1_vsxv", (void *) remainder_11x1_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Int8) }, { 0, 1, 0, 0 } };

static int remainder_11x1_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Int8       *tin1 =  (Int8 *) buffers[1];
    Int8       *tout0 =  (Int8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, *tin0) : *tin0  %  *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor remainder_11x1_vvxv_descr =
{ "remainder_11x1_vvxv", (void *) remainder_11x1_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Int8) }, { 0, 0, 0, 0 } };

static void _remainder_1x1_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int8  *tin0   = (Int8 *) ((char *) input  + inboffset);
    Int8 *tout0  = (Int8 *) ((char *) output + outboffset);
    Int8 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Int8 *) ((char *) tin0 + inbstrides[dim]);
            net = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0) : net  %  *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _remainder_1x1_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int remainder_1x1_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _remainder_1x1_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(remainder_1x1_R, CHECK_ALIGN, sizeof(Int8), sizeof(Int8));

static void _remainder_1x1_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int8 *tin0   = (Int8 *) ((char *) input  + inboffset);
    Int8 *tout0 = (Int8 *) ((char *) output + outboffset);
    Int8 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Int8 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Int8 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0) : lastval  %  *tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _remainder_1x1_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  remainder_1x1_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _remainder_1x1_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(remainder_1x1_A, CHECK_ALIGN, sizeof(Int8), sizeof(Int8));

static int remainder_11x1_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8        tin0 = *(Int8 *) buffers[0];
    Int8       *tin1 =  (Int8 *) buffers[1];
    Int8       *tout0 =  (Int8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, 0) : tin0  %  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor remainder_11x1_svxv_descr =
{ "remainder_11x1_svxv", (void *) remainder_11x1_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Int8) }, { 1, 0, 0, 0 } };
/*********************  remainder  *********************/
/*********************  power  *********************/

static int power_11x1_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Int8        tin1 = *(Int8 *) buffers[1];
    Int8       *tout0 =  (Int8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_pow(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor power_11x1_vsxf_descr =
{ "power_11x1_vsxf", (void *) power_11x1_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Int8) }, { 0, 1, 0, 0 } };

static int power_11x1_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Int8       *tin1 =  (Int8 *) buffers[1];
    Int8       *tout0 =  (Int8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_pow(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor power_11x1_vvxf_descr =
{ "power_11x1_vvxf", (void *) power_11x1_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Int8) }, { 0, 0, 0, 0 } };

static void _power_1x1_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int8  *tin0   = (Int8 *) ((char *) input  + inboffset);
    Int8 *tout0  = (Int8 *) ((char *) output + outboffset);
    Int8 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Int8 *) ((char *) tin0 + inbstrides[dim]);
            net   = num_pow(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _power_1x1_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int power_1x1_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _power_1x1_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(power_1x1_R, CHECK_ALIGN, sizeof(Int8), sizeof(Int8));

static void _power_1x1_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int8 *tin0   = (Int8 *) ((char *) input  + inboffset);
    Int8 *tout0 = (Int8 *) ((char *) output + outboffset);
    Int8 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Int8 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Int8 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = num_pow(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _power_1x1_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  power_1x1_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _power_1x1_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(power_1x1_A, CHECK_ALIGN, sizeof(Int8), sizeof(Int8));

static int power_11x1_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8        tin0 = *(Int8 *) buffers[0];
    Int8       *tin1 =  (Int8 *) buffers[1];
    Int8       *tout0 =  (Int8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_pow(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor power_11x1_svxf_descr =
{ "power_11x1_svxf", (void *) power_11x1_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Int8) }, { 1, 0, 0, 0 } };
/*********************  abs  *********************/

static int abs_1x1_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Int8       *tout0 =  (Int8 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = fabs(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor abs_1x1_vxf_descr =
{ "abs_1x1_vxf", (void *) abs_1x1_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int8), sizeof(Int8) }, { 0, 0, 0 } };
/*********************  sin  *********************/

static int sin_1xd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = sin(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor sin_1xd_vxf_descr =
{ "sin_1xd_vxf", (void *) sin_1xd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int8), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  cos  *********************/

static int cos_1xd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = cos(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor cos_1xd_vxf_descr =
{ "cos_1xd_vxf", (void *) cos_1xd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int8), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  tan  *********************/

static int tan_1xd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = tan(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor tan_1xd_vxf_descr =
{ "tan_1xd_vxf", (void *) tan_1xd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int8), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arcsin  *********************/

static int arcsin_1xd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = asin(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arcsin_1xd_vxf_descr =
{ "arcsin_1xd_vxf", (void *) arcsin_1xd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int8), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arccos  *********************/

static int arccos_1xd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = acos(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arccos_1xd_vxf_descr =
{ "arccos_1xd_vxf", (void *) arccos_1xd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int8), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arctan  *********************/

static int arctan_1xd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = atan(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arctan_1xd_vxf_descr =
{ "arctan_1xd_vxf", (void *) arctan_1xd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int8), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arctan2  *********************/

static int arctan2_11xd_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Int8        tin1 = *(Int8 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = atan2(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arctan2_11xd_vsxf_descr =
{ "arctan2_11xd_vsxf", (void *) arctan2_11xd_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Float64) }, { 0, 1, 0, 0 } };

static int arctan2_11xd_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Int8       *tin1 =  (Int8 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = atan2(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arctan2_11xd_vvxf_descr =
{ "arctan2_11xd_vvxf", (void *) arctan2_11xd_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Float64) }, { 0, 0, 0, 0 } };

static void _arctan2_1xd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int8  *tin0   = (Int8 *) ((char *) input  + inboffset);
    Float64 *tout0  = (Float64 *) ((char *) output + outboffset);
    Float64 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Int8 *) ((char *) tin0 + inbstrides[dim]);
            net   = atan2(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _arctan2_1xd_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int arctan2_1xd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _arctan2_1xd_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(arctan2_1xd_R, CHECK_ALIGN, sizeof(Int8), sizeof(Float64));

static void _arctan2_1xd_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int8 *tin0   = (Int8 *) ((char *) input  + inboffset);
    Float64 *tout0 = (Float64 *) ((char *) output + outboffset);
    Float64 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Int8 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Float64 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = atan2(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _arctan2_1xd_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  arctan2_1xd_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _arctan2_1xd_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(arctan2_1xd_A, CHECK_ALIGN, sizeof(Int8), sizeof(Float64));

static int arctan2_11xd_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8        tin0 = *(Int8 *) buffers[0];
    Int8       *tin1 =  (Int8 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = atan2(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arctan2_11xd_svxf_descr =
{ "arctan2_11xd_svxf", (void *) arctan2_11xd_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Float64) }, { 1, 0, 0, 0 } };
/*********************  log  *********************/

static int log_1xd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_log(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor log_1xd_vxf_descr =
{ "log_1xd_vxf", (void *) log_1xd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int8), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  log10  *********************/

static int log10_1xd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_log10(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor log10_1xd_vxf_descr =
{ "log10_1xd_vxf", (void *) log10_1xd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int8), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  exp  *********************/

static int exp_1xd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = exp(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor exp_1xd_vxf_descr =
{ "exp_1xd_vxf", (void *) exp_1xd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int8), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  sinh  *********************/

static int sinh_1xd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = sinh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor sinh_1xd_vxf_descr =
{ "sinh_1xd_vxf", (void *) sinh_1xd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int8), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  cosh  *********************/

static int cosh_1xd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = cosh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor cosh_1xd_vxf_descr =
{ "cosh_1xd_vxf", (void *) cosh_1xd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int8), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  tanh  *********************/

static int tanh_1xd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = tanh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor tanh_1xd_vxf_descr =
{ "tanh_1xd_vxf", (void *) tanh_1xd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int8), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arcsinh  *********************/

static int arcsinh_1xd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_asinh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arcsinh_1xd_vxf_descr =
{ "arcsinh_1xd_vxf", (void *) arcsinh_1xd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int8), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arccosh  *********************/

static int arccosh_1xd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_acosh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arccosh_1xd_vxf_descr =
{ "arccosh_1xd_vxf", (void *) arccosh_1xd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int8), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arctanh  *********************/

static int arctanh_1xd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_atanh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arctanh_1xd_vxf_descr =
{ "arctanh_1xd_vxf", (void *) arctanh_1xd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int8), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  ieeemask  *********************/
/*********************  ieeemask  *********************/
/*********************  isnan  *********************/
/*********************  isnan  *********************/
/*********************  isnan  *********************/
/*********************  isnan  *********************/
/*********************  sqrt  *********************/

static int sqrt_1xd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = sqrt(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor sqrt_1xd_vxf_descr =
{ "sqrt_1xd_vxf", (void *) sqrt_1xd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int8), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  equal  *********************/

static int equal_11xB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Int8        tin1 = *(Int8 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 == tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor equal_11xB_vsxv_descr =
{ "equal_11xB_vsxv", (void *) equal_11xB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int equal_11xB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Int8       *tin1 =  (Int8 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 == *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor equal_11xB_vvxv_descr =
{ "equal_11xB_vvxv", (void *) equal_11xB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int equal_11xB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8        tin0 = *(Int8 *) buffers[0];
    Int8       *tin1 =  (Int8 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 == *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor equal_11xB_svxv_descr =
{ "equal_11xB_svxv", (void *) equal_11xB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  not_equal  *********************/

static int not_equal_11xB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Int8        tin1 = *(Int8 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 != tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor not_equal_11xB_vsxv_descr =
{ "not_equal_11xB_vsxv", (void *) not_equal_11xB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int not_equal_11xB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Int8       *tin1 =  (Int8 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 != *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor not_equal_11xB_vvxv_descr =
{ "not_equal_11xB_vvxv", (void *) not_equal_11xB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int not_equal_11xB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8        tin0 = *(Int8 *) buffers[0];
    Int8       *tin1 =  (Int8 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 != *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor not_equal_11xB_svxv_descr =
{ "not_equal_11xB_svxv", (void *) not_equal_11xB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  greater  *********************/

static int greater_11xB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Int8        tin1 = *(Int8 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 > tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_11xB_vsxv_descr =
{ "greater_11xB_vsxv", (void *) greater_11xB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int greater_11xB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Int8       *tin1 =  (Int8 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 > *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_11xB_vvxv_descr =
{ "greater_11xB_vvxv", (void *) greater_11xB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int greater_11xB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8        tin0 = *(Int8 *) buffers[0];
    Int8       *tin1 =  (Int8 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 > *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_11xB_svxv_descr =
{ "greater_11xB_svxv", (void *) greater_11xB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  greater_equal  *********************/

static int greater_equal_11xB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Int8        tin1 = *(Int8 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 >= tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_equal_11xB_vsxv_descr =
{ "greater_equal_11xB_vsxv", (void *) greater_equal_11xB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int greater_equal_11xB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Int8       *tin1 =  (Int8 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 >= *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_equal_11xB_vvxv_descr =
{ "greater_equal_11xB_vvxv", (void *) greater_equal_11xB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int greater_equal_11xB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8        tin0 = *(Int8 *) buffers[0];
    Int8       *tin1 =  (Int8 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 >= *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_equal_11xB_svxv_descr =
{ "greater_equal_11xB_svxv", (void *) greater_equal_11xB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  less  *********************/

static int less_11xB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Int8        tin1 = *(Int8 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 < tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_11xB_vsxv_descr =
{ "less_11xB_vsxv", (void *) less_11xB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int less_11xB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Int8       *tin1 =  (Int8 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 < *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_11xB_vvxv_descr =
{ "less_11xB_vvxv", (void *) less_11xB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int less_11xB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8        tin0 = *(Int8 *) buffers[0];
    Int8       *tin1 =  (Int8 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 < *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_11xB_svxv_descr =
{ "less_11xB_svxv", (void *) less_11xB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  less_equal  *********************/

static int less_equal_11xB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Int8        tin1 = *(Int8 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 <= tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_equal_11xB_vsxv_descr =
{ "less_equal_11xB_vsxv", (void *) less_equal_11xB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int less_equal_11xB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Int8       *tin1 =  (Int8 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 <= *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_equal_11xB_vvxv_descr =
{ "less_equal_11xB_vvxv", (void *) less_equal_11xB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int less_equal_11xB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8        tin0 = *(Int8 *) buffers[0];
    Int8       *tin1 =  (Int8 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 <= *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_equal_11xB_svxv_descr =
{ "less_equal_11xB_svxv", (void *) less_equal_11xB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  logical_and  *********************/

static int logical_and_11xB_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Int8        tin1 = *(Int8 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_and(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_and_11xB_vsxf_descr =
{ "logical_and_11xB_vsxf", (void *) logical_and_11xB_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int logical_and_11xB_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Int8       *tin1 =  (Int8 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_and(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_and_11xB_vvxf_descr =
{ "logical_and_11xB_vvxf", (void *) logical_and_11xB_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int logical_and_11xB_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8        tin0 = *(Int8 *) buffers[0];
    Int8       *tin1 =  (Int8 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_and(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_and_11xB_svxf_descr =
{ "logical_and_11xB_svxf", (void *) logical_and_11xB_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  logical_or  *********************/

static int logical_or_11xB_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Int8        tin1 = *(Int8 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_or(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_or_11xB_vsxf_descr =
{ "logical_or_11xB_vsxf", (void *) logical_or_11xB_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int logical_or_11xB_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Int8       *tin1 =  (Int8 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_or(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_or_11xB_vvxf_descr =
{ "logical_or_11xB_vvxf", (void *) logical_or_11xB_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int logical_or_11xB_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8        tin0 = *(Int8 *) buffers[0];
    Int8       *tin1 =  (Int8 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_or(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_or_11xB_svxf_descr =
{ "logical_or_11xB_svxf", (void *) logical_or_11xB_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  logical_xor  *********************/

static int logical_xor_11xB_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Int8        tin1 = *(Int8 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_xor(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_xor_11xB_vsxf_descr =
{ "logical_xor_11xB_vsxf", (void *) logical_xor_11xB_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int logical_xor_11xB_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Int8       *tin1 =  (Int8 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_xor(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_xor_11xB_vvxf_descr =
{ "logical_xor_11xB_vvxf", (void *) logical_xor_11xB_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int logical_xor_11xB_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8        tin0 = *(Int8 *) buffers[0];
    Int8       *tin1 =  (Int8 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_xor(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_xor_11xB_svxf_descr =
{ "logical_xor_11xB_svxf", (void *) logical_xor_11xB_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  logical_and  *********************/
/*********************  logical_or  *********************/
/*********************  logical_xor  *********************/
/*********************  logical_not  *********************/

static int logical_not_1xB_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Bool       *tout0 =  (Bool *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = !*tin0;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_not_1xB_vxv_descr =
{ "logical_not_1xB_vxv", (void *) logical_not_1xB_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int8), sizeof(Bool) }, { 0, 0, 0 } };
/*********************  bitwise_and  *********************/

static int bitwise_and_11x1_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Int8        tin1 = *(Int8 *) buffers[1];
    Int8       *tout0 =  (Int8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 & tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_and_11x1_vsxv_descr =
{ "bitwise_and_11x1_vsxv", (void *) bitwise_and_11x1_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Int8) }, { 0, 1, 0, 0 } };

static int bitwise_and_11x1_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Int8       *tin1 =  (Int8 *) buffers[1];
    Int8       *tout0 =  (Int8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 & *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_and_11x1_vvxv_descr =
{ "bitwise_and_11x1_vvxv", (void *) bitwise_and_11x1_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Int8) }, { 0, 0, 0, 0 } };

static void _bitwise_and_1x1_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int8  *tin0   = (Int8 *) ((char *) input  + inboffset);
    Int8 *tout0  = (Int8 *) ((char *) output + outboffset);
    Int8 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Int8 *) ((char *) tin0 + inbstrides[dim]);
            net    =     net & *tin0;
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _bitwise_and_1x1_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int bitwise_and_1x1_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _bitwise_and_1x1_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(bitwise_and_1x1_R, CHECK_ALIGN, sizeof(Int8), sizeof(Int8));

static void _bitwise_and_1x1_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int8 *tin0   = (Int8 *) ((char *) input  + inboffset);
    Int8 *tout0 = (Int8 *) ((char *) output + outboffset);
    Int8 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Int8 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Int8 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = lastval & *tin0;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _bitwise_and_1x1_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  bitwise_and_1x1_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _bitwise_and_1x1_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(bitwise_and_1x1_A, CHECK_ALIGN, sizeof(Int8), sizeof(Int8));

static int bitwise_and_11x1_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8        tin0 = *(Int8 *) buffers[0];
    Int8       *tin1 =  (Int8 *) buffers[1];
    Int8       *tout0 =  (Int8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 & *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_and_11x1_svxv_descr =
{ "bitwise_and_11x1_svxv", (void *) bitwise_and_11x1_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Int8) }, { 1, 0, 0, 0 } };
/*********************  bitwise_or  *********************/

static int bitwise_or_11x1_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Int8        tin1 = *(Int8 *) buffers[1];
    Int8       *tout0 =  (Int8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 | tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_or_11x1_vsxv_descr =
{ "bitwise_or_11x1_vsxv", (void *) bitwise_or_11x1_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Int8) }, { 0, 1, 0, 0 } };

static int bitwise_or_11x1_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Int8       *tin1 =  (Int8 *) buffers[1];
    Int8       *tout0 =  (Int8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 | *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_or_11x1_vvxv_descr =
{ "bitwise_or_11x1_vvxv", (void *) bitwise_or_11x1_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Int8) }, { 0, 0, 0, 0 } };

static void _bitwise_or_1x1_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int8  *tin0   = (Int8 *) ((char *) input  + inboffset);
    Int8 *tout0  = (Int8 *) ((char *) output + outboffset);
    Int8 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Int8 *) ((char *) tin0 + inbstrides[dim]);
            net    =     net | *tin0;
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _bitwise_or_1x1_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int bitwise_or_1x1_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _bitwise_or_1x1_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(bitwise_or_1x1_R, CHECK_ALIGN, sizeof(Int8), sizeof(Int8));

static void _bitwise_or_1x1_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int8 *tin0   = (Int8 *) ((char *) input  + inboffset);
    Int8 *tout0 = (Int8 *) ((char *) output + outboffset);
    Int8 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Int8 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Int8 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = lastval | *tin0;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _bitwise_or_1x1_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  bitwise_or_1x1_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _bitwise_or_1x1_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(bitwise_or_1x1_A, CHECK_ALIGN, sizeof(Int8), sizeof(Int8));

static int bitwise_or_11x1_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8        tin0 = *(Int8 *) buffers[0];
    Int8       *tin1 =  (Int8 *) buffers[1];
    Int8       *tout0 =  (Int8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 | *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_or_11x1_svxv_descr =
{ "bitwise_or_11x1_svxv", (void *) bitwise_or_11x1_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Int8) }, { 1, 0, 0, 0 } };
/*********************  bitwise_xor  *********************/

static int bitwise_xor_11x1_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Int8        tin1 = *(Int8 *) buffers[1];
    Int8       *tout0 =  (Int8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 ^ tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_xor_11x1_vsxv_descr =
{ "bitwise_xor_11x1_vsxv", (void *) bitwise_xor_11x1_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Int8) }, { 0, 1, 0, 0 } };

static int bitwise_xor_11x1_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Int8       *tin1 =  (Int8 *) buffers[1];
    Int8       *tout0 =  (Int8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 ^ *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_xor_11x1_vvxv_descr =
{ "bitwise_xor_11x1_vvxv", (void *) bitwise_xor_11x1_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Int8) }, { 0, 0, 0, 0 } };

static void _bitwise_xor_1x1_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int8  *tin0   = (Int8 *) ((char *) input  + inboffset);
    Int8 *tout0  = (Int8 *) ((char *) output + outboffset);
    Int8 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Int8 *) ((char *) tin0 + inbstrides[dim]);
            net    =     net ^ *tin0;
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _bitwise_xor_1x1_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int bitwise_xor_1x1_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _bitwise_xor_1x1_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(bitwise_xor_1x1_R, CHECK_ALIGN, sizeof(Int8), sizeof(Int8));

static void _bitwise_xor_1x1_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int8 *tin0   = (Int8 *) ((char *) input  + inboffset);
    Int8 *tout0 = (Int8 *) ((char *) output + outboffset);
    Int8 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Int8 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Int8 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = lastval ^ *tin0;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _bitwise_xor_1x1_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  bitwise_xor_1x1_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _bitwise_xor_1x1_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(bitwise_xor_1x1_A, CHECK_ALIGN, sizeof(Int8), sizeof(Int8));

static int bitwise_xor_11x1_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8        tin0 = *(Int8 *) buffers[0];
    Int8       *tin1 =  (Int8 *) buffers[1];
    Int8       *tout0 =  (Int8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 ^ *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_xor_11x1_svxv_descr =
{ "bitwise_xor_11x1_svxv", (void *) bitwise_xor_11x1_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Int8) }, { 1, 0, 0, 0 } };
/*********************  bitwise_not  *********************/

static int bitwise_not_1x1_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Int8       *tout0 =  (Int8 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ~*tin0;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_not_1x1_vxv_descr =
{ "bitwise_not_1x1_vxv", (void *) bitwise_not_1x1_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int8), sizeof(Int8) }, { 0, 0, 0 } };
/*********************  bitwise_not  *********************/
/*********************  rshift  *********************/

static int rshift_11x1_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Int8        tin1 = *(Int8 *) buffers[1];
    Int8       *tout0 =  (Int8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 >> tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor rshift_11x1_vsxv_descr =
{ "rshift_11x1_vsxv", (void *) rshift_11x1_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Int8) }, { 0, 1, 0, 0 } };

static int rshift_11x1_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Int8       *tin1 =  (Int8 *) buffers[1];
    Int8       *tout0 =  (Int8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 >> *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor rshift_11x1_vvxv_descr =
{ "rshift_11x1_vvxv", (void *) rshift_11x1_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Int8) }, { 0, 0, 0, 0 } };

static int rshift_11x1_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8        tin0 = *(Int8 *) buffers[0];
    Int8       *tin1 =  (Int8 *) buffers[1];
    Int8       *tout0 =  (Int8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 >> *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor rshift_11x1_svxv_descr =
{ "rshift_11x1_svxv", (void *) rshift_11x1_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Int8) }, { 1, 0, 0, 0 } };
/*********************  lshift  *********************/

static int lshift_11x1_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Int8        tin1 = *(Int8 *) buffers[1];
    Int8       *tout0 =  (Int8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 << tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor lshift_11x1_vsxv_descr =
{ "lshift_11x1_vsxv", (void *) lshift_11x1_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Int8) }, { 0, 1, 0, 0 } };

static int lshift_11x1_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Int8       *tin1 =  (Int8 *) buffers[1];
    Int8       *tout0 =  (Int8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 << *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor lshift_11x1_vvxv_descr =
{ "lshift_11x1_vvxv", (void *) lshift_11x1_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Int8) }, { 0, 0, 0, 0 } };

static int lshift_11x1_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8        tin0 = *(Int8 *) buffers[0];
    Int8       *tin1 =  (Int8 *) buffers[1];
    Int8       *tout0 =  (Int8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 << *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor lshift_11x1_svxv_descr =
{ "lshift_11x1_svxv", (void *) lshift_11x1_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Int8) }, { 1, 0, 0, 0 } };
/*********************  floor  *********************/

static int floor_1x1_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Int8       *tout0 =  (Int8 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = (*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor floor_1x1_vxf_descr =
{ "floor_1x1_vxf", (void *) floor_1x1_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int8), sizeof(Int8) }, { 0, 0, 0 } };
/*********************  floor  *********************/
/*********************  ceil  *********************/

static int ceil_1x1_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Int8       *tout0 =  (Int8 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = (*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor ceil_1x1_vxf_descr =
{ "ceil_1x1_vxf", (void *) ceil_1x1_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int8), sizeof(Int8) }, { 0, 0, 0 } };
/*********************  ceil  *********************/
/*********************  maximum  *********************/

static int maximum_11x1_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Int8        tin1 = *(Int8 *) buffers[1];
    Int8       *tout0 =  (Int8 *) buffers[2];
Int8 temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufmaximum(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor maximum_11x1_vsxf_descr =
{ "maximum_11x1_vsxf", (void *) maximum_11x1_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Int8) }, { 0, 1, 0, 0 } };

static int maximum_11x1_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Int8       *tin1 =  (Int8 *) buffers[1];
    Int8       *tout0 =  (Int8 *) buffers[2];
Int8 temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufmaximum(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor maximum_11x1_vvxf_descr =
{ "maximum_11x1_vvxf", (void *) maximum_11x1_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Int8) }, { 0, 0, 0, 0 } };

static void _maximum_1x1_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int8  *tin0   = (Int8 *) ((char *) input  + inboffset);
    Int8 *tout0  = (Int8 *) ((char *) output + outboffset);
    Int8 net;
    Int8 temp1, temp2;
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Int8 *) ((char *) tin0 + inbstrides[dim]);
            net   = ufmaximum(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _maximum_1x1_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int maximum_1x1_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _maximum_1x1_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(maximum_1x1_R, CHECK_ALIGN, sizeof(Int8), sizeof(Int8));

static void _maximum_1x1_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int8 *tin0   = (Int8 *) ((char *) input  + inboffset);
    Int8 *tout0 = (Int8 *) ((char *) output + outboffset);
    Int8 lastval;
    Int8 temp1, temp2;
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Int8 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Int8 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = ufmaximum(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _maximum_1x1_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  maximum_1x1_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _maximum_1x1_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(maximum_1x1_A, CHECK_ALIGN, sizeof(Int8), sizeof(Int8));

static int maximum_11x1_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8        tin0 = *(Int8 *) buffers[0];
    Int8       *tin1 =  (Int8 *) buffers[1];
    Int8       *tout0 =  (Int8 *) buffers[2];
Int8 temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufmaximum(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor maximum_11x1_svxf_descr =
{ "maximum_11x1_svxf", (void *) maximum_11x1_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Int8) }, { 1, 0, 0, 0 } };
/*********************  minimum  *********************/

static int minimum_11x1_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Int8        tin1 = *(Int8 *) buffers[1];
    Int8       *tout0 =  (Int8 *) buffers[2];
Int8 temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufminimum(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor minimum_11x1_vsxf_descr =
{ "minimum_11x1_vsxf", (void *) minimum_11x1_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Int8) }, { 0, 1, 0, 0 } };

static int minimum_11x1_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Int8       *tin1 =  (Int8 *) buffers[1];
    Int8       *tout0 =  (Int8 *) buffers[2];
Int8 temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufminimum(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor minimum_11x1_vvxf_descr =
{ "minimum_11x1_vvxf", (void *) minimum_11x1_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Int8) }, { 0, 0, 0, 0 } };

static void _minimum_1x1_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int8  *tin0   = (Int8 *) ((char *) input  + inboffset);
    Int8 *tout0  = (Int8 *) ((char *) output + outboffset);
    Int8 net;
    Int8 temp1, temp2;
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Int8 *) ((char *) tin0 + inbstrides[dim]);
            net   = ufminimum(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _minimum_1x1_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int minimum_1x1_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _minimum_1x1_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(minimum_1x1_R, CHECK_ALIGN, sizeof(Int8), sizeof(Int8));

static void _minimum_1x1_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int8 *tin0   = (Int8 *) ((char *) input  + inboffset);
    Int8 *tout0 = (Int8 *) ((char *) output + outboffset);
    Int8 lastval;
    Int8 temp1, temp2;
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Int8 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Int8 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = ufminimum(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _minimum_1x1_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  minimum_1x1_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _minimum_1x1_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(minimum_1x1_A, CHECK_ALIGN, sizeof(Int8), sizeof(Int8));

static int minimum_11x1_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8        tin0 = *(Int8 *) buffers[0];
    Int8       *tin1 =  (Int8 *) buffers[1];
    Int8       *tout0 =  (Int8 *) buffers[2];
Int8 temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufminimum(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor minimum_11x1_svxf_descr =
{ "minimum_11x1_svxf", (void *) minimum_11x1_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Int8) }, { 1, 0, 0, 0 } };
/*********************  fabs  *********************/

static int fabs_1xd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = fabs(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor fabs_1xd_vxf_descr =
{ "fabs_1xd_vxf", (void *) fabs_1xd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int8), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  _round  *********************/

static int _round_1xd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_round(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor _round_1xd_vxf_descr =
{ "_round_1xd_vxf", (void *) _round_1xd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int8), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  hypot  *********************/

static int hypot_11xd_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Int8        tin1 = *(Int8 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = hypot(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor hypot_11xd_vsxf_descr =
{ "hypot_11xd_vsxf", (void *) hypot_11xd_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Float64) }, { 0, 1, 0, 0 } };

static int hypot_11xd_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8       *tin0 =  (Int8 *) buffers[0];
    Int8       *tin1 =  (Int8 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = hypot(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor hypot_11xd_vvxf_descr =
{ "hypot_11xd_vvxf", (void *) hypot_11xd_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Float64) }, { 0, 0, 0, 0 } };

static void _hypot_1xd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int8  *tin0   = (Int8 *) ((char *) input  + inboffset);
    Float64 *tout0  = (Float64 *) ((char *) output + outboffset);
    Float64 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Int8 *) ((char *) tin0 + inbstrides[dim]);
            net   = hypot(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _hypot_1xd_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int hypot_1xd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _hypot_1xd_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(hypot_1xd_R, CHECK_ALIGN, sizeof(Int8), sizeof(Float64));

static void _hypot_1xd_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int8 *tin0   = (Int8 *) ((char *) input  + inboffset);
    Float64 *tout0 = (Float64 *) ((char *) output + outboffset);
    Float64 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Int8 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Float64 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = hypot(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _hypot_1xd_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  hypot_1xd_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _hypot_1xd_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(hypot_1xd_A, CHECK_ALIGN, sizeof(Int8), sizeof(Float64));

static int hypot_11xd_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int8        tin0 = *(Int8 *) buffers[0];
    Int8       *tin1 =  (Int8 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = hypot(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor hypot_11xd_svxf_descr =
{ "hypot_11xd_svxf", (void *) hypot_11xd_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int8), sizeof(Int8), sizeof(Float64) }, { 1, 0, 0, 0 } };
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

static PyMethodDef _ufuncInt8Methods[] = {

	{NULL,      NULL}        /* Sentinel */
};

static PyObject *init_funcDict(void) {
    PyObject *dict, *keytuple, *cfunc;
    dict = PyDict_New();
    /* minus_1x1_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","minus","v","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minus_1x1_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_11x1_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","add","vs","Int8","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_11x1_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_11x1_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","add","vv","Int8","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_11x1_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_1x1_R */
    keytuple=Py_BuildValue("ss((s)(s))","add","R","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_1x1_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_1x1_A */
    keytuple=Py_BuildValue("ss((s)(s))","add","A","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_1x1_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_11x1_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","add","sv","Int8","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_11x1_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_11x1_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","subtract","vs","Int8","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_11x1_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_11x1_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","subtract","vv","Int8","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_11x1_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_1x1_R */
    keytuple=Py_BuildValue("ss((s)(s))","subtract","R","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_1x1_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_1x1_A */
    keytuple=Py_BuildValue("ss((s)(s))","subtract","A","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_1x1_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_11x1_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","subtract","sv","Int8","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_11x1_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* multiply_11x1_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","multiply","vs","Int8","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&multiply_11x1_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* multiply_11x1_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","multiply","vv","Int8","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&multiply_11x1_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* multiply_1x1_R */
    keytuple=Py_BuildValue("ss((s)(s))","multiply","R","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&multiply_1x1_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* multiply_1x1_A */
    keytuple=Py_BuildValue("ss((s)(s))","multiply","A","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&multiply_1x1_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* multiply_11x1_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","multiply","sv","Int8","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&multiply_11x1_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_11x1_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","divide","vs","Int8","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_11x1_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_11x1_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","divide","vv","Int8","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_11x1_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_1x1_R */
    keytuple=Py_BuildValue("ss((s)(s))","divide","R","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_1x1_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_1x1_A */
    keytuple=Py_BuildValue("ss((s)(s))","divide","A","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_1x1_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_11x1_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","divide","sv","Int8","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_11x1_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_divide_11x1_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","floor_divide","vs","Int8","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_divide_11x1_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_divide_11x1_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","floor_divide","vv","Int8","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_divide_11x1_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_divide_1x1_R */
    keytuple=Py_BuildValue("ss((s)(s))","floor_divide","R","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_divide_1x1_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_divide_1x1_A */
    keytuple=Py_BuildValue("ss((s)(s))","floor_divide","A","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_divide_1x1_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_divide_11x1_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","floor_divide","sv","Int8","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_divide_11x1_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_11xf_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","true_divide","vs","Int8","Int8","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_11xf_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_11xf_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","true_divide","vv","Int8","Int8","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_11xf_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_1xf_R */
    keytuple=Py_BuildValue("ss((s)(s))","true_divide","R","Int8","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_1xf_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_1xf_A */
    keytuple=Py_BuildValue("ss((s)(s))","true_divide","A","Int8","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_1xf_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_11xf_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","true_divide","sv","Int8","Int8","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_11xf_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_11x1_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","remainder","vs","Int8","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_11x1_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_11x1_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","remainder","vv","Int8","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_11x1_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_1x1_R */
    keytuple=Py_BuildValue("ss((s)(s))","remainder","R","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_1x1_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_1x1_A */
    keytuple=Py_BuildValue("ss((s)(s))","remainder","A","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_1x1_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_11x1_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","remainder","sv","Int8","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_11x1_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_11x1_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","power","vs","Int8","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_11x1_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_11x1_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","power","vv","Int8","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_11x1_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_1x1_R */
    keytuple=Py_BuildValue("ss((s)(s))","power","R","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_1x1_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_1x1_A */
    keytuple=Py_BuildValue("ss((s)(s))","power","A","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_1x1_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_11x1_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","power","sv","Int8","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_11x1_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* abs_1x1_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","abs","v","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&abs_1x1_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* sin_1xd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","sin","v","Int8","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&sin_1xd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* cos_1xd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","cos","v","Int8","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&cos_1xd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* tan_1xd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","tan","v","Int8","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&tan_1xd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arcsin_1xd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arcsin","v","Int8","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arcsin_1xd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arccos_1xd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arccos","v","Int8","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arccos_1xd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan_1xd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arctan","v","Int8","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan_1xd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan2_11xd_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","arctan2","vs","Int8","Int8","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan2_11xd_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan2_11xd_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","arctan2","vv","Int8","Int8","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan2_11xd_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan2_1xd_R */
    keytuple=Py_BuildValue("ss((s)(s))","arctan2","R","Int8","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan2_1xd_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan2_1xd_A */
    keytuple=Py_BuildValue("ss((s)(s))","arctan2","A","Int8","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan2_1xd_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan2_11xd_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","arctan2","sv","Int8","Int8","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan2_11xd_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* log_1xd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","log","v","Int8","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&log_1xd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* log10_1xd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","log10","v","Int8","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&log10_1xd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* exp_1xd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","exp","v","Int8","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&exp_1xd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* sinh_1xd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","sinh","v","Int8","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&sinh_1xd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* cosh_1xd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","cosh","v","Int8","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&cosh_1xd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* tanh_1xd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","tanh","v","Int8","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&tanh_1xd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arcsinh_1xd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arcsinh","v","Int8","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arcsinh_1xd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arccosh_1xd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arccosh","v","Int8","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arccosh_1xd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctanh_1xd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arctanh","v","Int8","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctanh_1xd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* sqrt_1xd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","sqrt","v","Int8","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&sqrt_1xd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* equal_11xB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","equal","vs","Int8","Int8","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&equal_11xB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* equal_11xB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","equal","vv","Int8","Int8","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&equal_11xB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* equal_11xB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","equal","sv","Int8","Int8","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&equal_11xB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* not_equal_11xB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","not_equal","vs","Int8","Int8","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&not_equal_11xB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* not_equal_11xB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","not_equal","vv","Int8","Int8","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&not_equal_11xB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* not_equal_11xB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","not_equal","sv","Int8","Int8","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&not_equal_11xB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_11xB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater","vs","Int8","Int8","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_11xB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_11xB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater","vv","Int8","Int8","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_11xB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_11xB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater","sv","Int8","Int8","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_11xB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_equal_11xB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater_equal","vs","Int8","Int8","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_equal_11xB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_equal_11xB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater_equal","vv","Int8","Int8","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_equal_11xB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_equal_11xB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater_equal","sv","Int8","Int8","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_equal_11xB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_11xB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less","vs","Int8","Int8","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_11xB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_11xB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less","vv","Int8","Int8","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_11xB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_11xB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less","sv","Int8","Int8","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_11xB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_equal_11xB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less_equal","vs","Int8","Int8","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_equal_11xB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_equal_11xB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less_equal","vv","Int8","Int8","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_equal_11xB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_equal_11xB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less_equal","sv","Int8","Int8","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_equal_11xB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_and_11xB_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_and","vs","Int8","Int8","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_and_11xB_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_and_11xB_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_and","vv","Int8","Int8","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_and_11xB_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_and_11xB_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_and","sv","Int8","Int8","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_and_11xB_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_or_11xB_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_or","vs","Int8","Int8","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_or_11xB_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_or_11xB_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_or","vv","Int8","Int8","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_or_11xB_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_or_11xB_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_or","sv","Int8","Int8","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_or_11xB_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_xor_11xB_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_xor","vs","Int8","Int8","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_xor_11xB_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_xor_11xB_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_xor","vv","Int8","Int8","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_xor_11xB_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_xor_11xB_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_xor","sv","Int8","Int8","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_xor_11xB_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_not_1xB_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","logical_not","v","Int8","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_not_1xB_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_and_11x1_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_and","vs","Int8","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_and_11x1_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_and_11x1_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_and","vv","Int8","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_and_11x1_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_and_1x1_R */
    keytuple=Py_BuildValue("ss((s)(s))","bitwise_and","R","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_and_1x1_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_and_1x1_A */
    keytuple=Py_BuildValue("ss((s)(s))","bitwise_and","A","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_and_1x1_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_and_11x1_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_and","sv","Int8","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_and_11x1_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_or_11x1_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_or","vs","Int8","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_or_11x1_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_or_11x1_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_or","vv","Int8","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_or_11x1_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_or_1x1_R */
    keytuple=Py_BuildValue("ss((s)(s))","bitwise_or","R","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_or_1x1_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_or_1x1_A */
    keytuple=Py_BuildValue("ss((s)(s))","bitwise_or","A","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_or_1x1_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_or_11x1_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_or","sv","Int8","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_or_11x1_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_xor_11x1_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_xor","vs","Int8","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_xor_11x1_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_xor_11x1_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_xor","vv","Int8","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_xor_11x1_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_xor_1x1_R */
    keytuple=Py_BuildValue("ss((s)(s))","bitwise_xor","R","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_xor_1x1_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_xor_1x1_A */
    keytuple=Py_BuildValue("ss((s)(s))","bitwise_xor","A","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_xor_1x1_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_xor_11x1_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_xor","sv","Int8","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_xor_11x1_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_not_1x1_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","bitwise_not","v","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_not_1x1_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* rshift_11x1_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","rshift","vs","Int8","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&rshift_11x1_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* rshift_11x1_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","rshift","vv","Int8","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&rshift_11x1_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* rshift_11x1_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","rshift","sv","Int8","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&rshift_11x1_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* lshift_11x1_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","lshift","vs","Int8","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&lshift_11x1_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* lshift_11x1_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","lshift","vv","Int8","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&lshift_11x1_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* lshift_11x1_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","lshift","sv","Int8","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&lshift_11x1_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_1x1_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","floor","v","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_1x1_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* ceil_1x1_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","ceil","v","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&ceil_1x1_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_11x1_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","maximum","vs","Int8","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_11x1_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_11x1_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","maximum","vv","Int8","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_11x1_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_1x1_R */
    keytuple=Py_BuildValue("ss((s)(s))","maximum","R","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_1x1_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_1x1_A */
    keytuple=Py_BuildValue("ss((s)(s))","maximum","A","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_1x1_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_11x1_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","maximum","sv","Int8","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_11x1_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_11x1_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","minimum","vs","Int8","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_11x1_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_11x1_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","minimum","vv","Int8","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_11x1_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_1x1_R */
    keytuple=Py_BuildValue("ss((s)(s))","minimum","R","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_1x1_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_1x1_A */
    keytuple=Py_BuildValue("ss((s)(s))","minimum","A","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_1x1_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_11x1_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","minimum","sv","Int8","Int8","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_11x1_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* fabs_1xd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","fabs","v","Int8","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&fabs_1xd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* _round_1xd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","_round","v","Int8","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&_round_1xd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_11xd_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","hypot","vs","Int8","Int8","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_11xd_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_11xd_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","hypot","vv","Int8","Int8","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_11xd_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_1xd_R */
    keytuple=Py_BuildValue("ss((s)(s))","hypot","R","Int8","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_1xd_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_1xd_A */
    keytuple=Py_BuildValue("ss((s)(s))","hypot","A","Int8","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_1xd_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_11xd_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","hypot","sv","Int8","Int8","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_11xd_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    return dict;
}

/* platform independent*/
#ifdef MS_WIN32
__declspec(dllexport)
#endif
void init_ufuncInt8(void) {
    PyObject *m, *d, *functions;
    m = Py_InitModule("_ufuncInt8", _ufuncInt8Methods);
    d = PyModule_GetDict(m);
    import_libnumarray();
    functions = init_funcDict();
    PyDict_SetItemString(d, "functionDict", functions);
    Py_DECREF(functions);
    ADD_VERSION(m);
}
