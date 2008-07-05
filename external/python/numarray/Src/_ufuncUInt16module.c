
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

static int minus_wxw_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    UInt16     *tout0 =  (UInt16 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = -*tin0;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor minus_wxw_vxv_descr =
{ "minus_wxw_vxv", (void *) minus_wxw_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt16), sizeof(UInt16) }, { 0, 0, 0 } };
/*********************  add  *********************/

static int add_wwxw_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    UInt16      tin1 = *(UInt16 *) buffers[1];
    UInt16     *tout0 =  (UInt16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 + tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor add_wwxw_vsxv_descr =
{ "add_wwxw_vsxv", (void *) add_wwxw_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(UInt16) }, { 0, 1, 0, 0 } };

static int add_wwxw_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    UInt16     *tin1 =  (UInt16 *) buffers[1];
    UInt16     *tout0 =  (UInt16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 + *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor add_wwxw_vvxv_descr =
{ "add_wwxw_vvxv", (void *) add_wwxw_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(UInt16) }, { 0, 0, 0, 0 } };

static void _add_wxw_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt16  *tin0   = (UInt16 *) ((char *) input  + inboffset);
    UInt16 *tout0  = (UInt16 *) ((char *) output + outboffset);
    UInt16 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (UInt16 *) ((char *) tin0 + inbstrides[dim]);
            net    =     net + *tin0;
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _add_wxw_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int add_wxw_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _add_wxw_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(add_wxw_R, CHECK_ALIGN, sizeof(UInt16), sizeof(UInt16));

static void _add_wxw_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt16 *tin0   = (UInt16 *) ((char *) input  + inboffset);
    UInt16 *tout0 = (UInt16 *) ((char *) output + outboffset);
    UInt16 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (UInt16 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (UInt16 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = lastval + *tin0;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _add_wxw_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  add_wxw_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _add_wxw_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(add_wxw_A, CHECK_ALIGN, sizeof(UInt16), sizeof(UInt16));

static int add_wwxw_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16      tin0 = *(UInt16 *) buffers[0];
    UInt16     *tin1 =  (UInt16 *) buffers[1];
    UInt16     *tout0 =  (UInt16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 + *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor add_wwxw_svxv_descr =
{ "add_wwxw_svxv", (void *) add_wwxw_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(UInt16) }, { 1, 0, 0, 0 } };
/*********************  subtract  *********************/

static int subtract_wwxw_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    UInt16      tin1 = *(UInt16 *) buffers[1];
    UInt16     *tout0 =  (UInt16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 - tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor subtract_wwxw_vsxv_descr =
{ "subtract_wwxw_vsxv", (void *) subtract_wwxw_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(UInt16) }, { 0, 1, 0, 0 } };

static int subtract_wwxw_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    UInt16     *tin1 =  (UInt16 *) buffers[1];
    UInt16     *tout0 =  (UInt16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 - *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor subtract_wwxw_vvxv_descr =
{ "subtract_wwxw_vvxv", (void *) subtract_wwxw_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(UInt16) }, { 0, 0, 0, 0 } };

static void _subtract_wxw_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt16  *tin0   = (UInt16 *) ((char *) input  + inboffset);
    UInt16 *tout0  = (UInt16 *) ((char *) output + outboffset);
    UInt16 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (UInt16 *) ((char *) tin0 + inbstrides[dim]);
            net    =     net - *tin0;
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _subtract_wxw_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int subtract_wxw_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _subtract_wxw_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(subtract_wxw_R, CHECK_ALIGN, sizeof(UInt16), sizeof(UInt16));

static void _subtract_wxw_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt16 *tin0   = (UInt16 *) ((char *) input  + inboffset);
    UInt16 *tout0 = (UInt16 *) ((char *) output + outboffset);
    UInt16 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (UInt16 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (UInt16 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = lastval - *tin0;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _subtract_wxw_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  subtract_wxw_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _subtract_wxw_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(subtract_wxw_A, CHECK_ALIGN, sizeof(UInt16), sizeof(UInt16));

static int subtract_wwxw_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16      tin0 = *(UInt16 *) buffers[0];
    UInt16     *tin1 =  (UInt16 *) buffers[1];
    UInt16     *tout0 =  (UInt16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 - *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor subtract_wwxw_svxv_descr =
{ "subtract_wwxw_svxv", (void *) subtract_wwxw_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(UInt16) }, { 1, 0, 0, 0 } };
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/

static int multiply_wwxw_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    UInt16      tin1 = *(UInt16 *) buffers[1];
    UInt16     *tout0 =  (UInt16 *) buffers[2];
Int32 temp;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        temp = ((Int32) *tin0) * ((Int32) tin1);
    if (temp > 65535) temp = int_overflow_error(65535.);
    *tout0 = (UInt16) temp;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor multiply_wwxw_vsxv_descr =
{ "multiply_wwxw_vsxv", (void *) multiply_wwxw_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(UInt16) }, { 0, 1, 0, 0 } };

static int multiply_wwxw_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    UInt16     *tin1 =  (UInt16 *) buffers[1];
    UInt16     *tout0 =  (UInt16 *) buffers[2];
Int32 temp;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        temp = ((Int32) *tin0) * ((Int32) *tin1);
    if (temp > 65535) temp = int_overflow_error(65535.);
    *tout0 = (UInt16) temp;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor multiply_wwxw_vvxv_descr =
{ "multiply_wwxw_vvxv", (void *) multiply_wwxw_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(UInt16) }, { 0, 0, 0, 0 } };

static void _multiply_wxw_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt16  *tin0   = (UInt16 *) ((char *) input  + inboffset);
    UInt16 *tout0  = (UInt16 *) ((char *) output + outboffset);
    UInt16 net;
    Int32 temp;
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (UInt16 *) ((char *) tin0 + inbstrides[dim]);
            temp = ((Int32) net) * ((Int32) *tin0);
    if (temp > 65535) temp = int_overflow_error(65535.);
    net = (UInt16) temp;
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _multiply_wxw_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int multiply_wxw_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _multiply_wxw_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(multiply_wxw_R, CHECK_ALIGN, sizeof(UInt16), sizeof(UInt16));

static void _multiply_wxw_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt16 *tin0   = (UInt16 *) ((char *) input  + inboffset);
    UInt16 *tout0 = (UInt16 *) ((char *) output + outboffset);
    UInt16 lastval;
    Int32 temp;
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (UInt16 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (UInt16 *) ((char *) tout0 + outbstrides[dim]);
            temp = ((Int32) lastval) * ((Int32) *tin0);
    if (temp > 65535) temp = int_overflow_error(65535.);
    *tout0 = (UInt16) temp;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _multiply_wxw_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  multiply_wxw_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _multiply_wxw_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(multiply_wxw_A, CHECK_ALIGN, sizeof(UInt16), sizeof(UInt16));

static int multiply_wwxw_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16      tin0 = *(UInt16 *) buffers[0];
    UInt16     *tin1 =  (UInt16 *) buffers[1];
    UInt16     *tout0 =  (UInt16 *) buffers[2];
Int32 temp;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        temp = ((Int32) tin0) * ((Int32) *tin1);
    if (temp > 65535) temp = int_overflow_error(65535.);
    *tout0 = (UInt16) temp;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor multiply_wwxw_svxv_descr =
{ "multiply_wwxw_svxv", (void *) multiply_wwxw_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(UInt16) }, { 1, 0, 0, 0 } };
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  divide  *********************/

static int divide_wwxw_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    UInt16      tin1 = *(UInt16 *) buffers[1];
    UInt16     *tout0 =  (UInt16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((tin1==0) ? int_dividebyzero_error(tin1, *tin0) : *tin0 / tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor divide_wwxw_vsxv_descr =
{ "divide_wwxw_vsxv", (void *) divide_wwxw_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(UInt16) }, { 0, 1, 0, 0 } };

static int divide_wwxw_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    UInt16     *tin1 =  (UInt16 *) buffers[1];
    UInt16     *tout0 =  (UInt16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, *tin0) : *tin0 / *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor divide_wwxw_vvxv_descr =
{ "divide_wwxw_vvxv", (void *) divide_wwxw_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(UInt16) }, { 0, 0, 0, 0 } };

static void _divide_wxw_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt16  *tin0   = (UInt16 *) ((char *) input  + inboffset);
    UInt16 *tout0  = (UInt16 *) ((char *) output + outboffset);
    UInt16 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (UInt16 *) ((char *) tin0 + inbstrides[dim]);
            net = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0) : net / *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _divide_wxw_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int divide_wxw_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _divide_wxw_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(divide_wxw_R, CHECK_ALIGN, sizeof(UInt16), sizeof(UInt16));

static void _divide_wxw_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt16 *tin0   = (UInt16 *) ((char *) input  + inboffset);
    UInt16 *tout0 = (UInt16 *) ((char *) output + outboffset);
    UInt16 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (UInt16 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (UInt16 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0) : lastval / *tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _divide_wxw_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  divide_wxw_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _divide_wxw_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(divide_wxw_A, CHECK_ALIGN, sizeof(UInt16), sizeof(UInt16));

static int divide_wwxw_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16      tin0 = *(UInt16 *) buffers[0];
    UInt16     *tin1 =  (UInt16 *) buffers[1];
    UInt16     *tout0 =  (UInt16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, 0) : tin0 / *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor divide_wwxw_svxv_descr =
{ "divide_wwxw_svxv", (void *) divide_wwxw_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(UInt16) }, { 1, 0, 0, 0 } };
/*********************  divide  *********************/
/*********************  floor_divide  *********************/

static int floor_divide_wwxw_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    UInt16      tin1 = *(UInt16 *) buffers[1];
    UInt16     *tout0 =  (UInt16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = (( tin1==0) ? int_dividebyzero_error( tin1, *tin0) : floor(*tin0   / (double)  tin1));
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor floor_divide_wwxw_vsxv_descr =
{ "floor_divide_wwxw_vsxv", (void *) floor_divide_wwxw_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(UInt16) }, { 0, 1, 0, 0 } };

static int floor_divide_wwxw_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    UInt16     *tin1 =  (UInt16 *) buffers[1];
    UInt16     *tout0 =  (UInt16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, *tin0) : floor(*tin0   / (double) *tin1));
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor floor_divide_wwxw_vvxv_descr =
{ "floor_divide_wwxw_vvxv", (void *) floor_divide_wwxw_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(UInt16) }, { 0, 0, 0, 0 } };

static void _floor_divide_wxw_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt16  *tin0   = (UInt16 *) ((char *) input  + inboffset);
    UInt16 *tout0  = (UInt16 *) ((char *) output + outboffset);
    UInt16 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (UInt16 *) ((char *) tin0 + inbstrides[dim]);
            net    = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0)     : floor(net     / (double) *tin0));
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _floor_divide_wxw_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int floor_divide_wxw_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _floor_divide_wxw_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(floor_divide_wxw_R, CHECK_ALIGN, sizeof(UInt16), sizeof(UInt16));

static void _floor_divide_wxw_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt16 *tin0   = (UInt16 *) ((char *) input  + inboffset);
    UInt16 *tout0 = (UInt16 *) ((char *) output + outboffset);
    UInt16 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (UInt16 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (UInt16 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0)     : floor(lastval / (double) *tin0));
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _floor_divide_wxw_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  floor_divide_wxw_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _floor_divide_wxw_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(floor_divide_wxw_A, CHECK_ALIGN, sizeof(UInt16), sizeof(UInt16));

static int floor_divide_wwxw_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16      tin0 = *(UInt16 *) buffers[0];
    UInt16     *tin1 =  (UInt16 *) buffers[1];
    UInt16     *tout0 =  (UInt16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, 0)     : floor(tin0    / (double) *tin1));
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor floor_divide_wwxw_svxv_descr =
{ "floor_divide_wwxw_svxv", (void *) floor_divide_wwxw_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(UInt16) }, { 1, 0, 0, 0 } };
/*********************  floor_divide  *********************/
/*********************  true_divide  *********************/

static int true_divide_wwxf_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    UInt16      tin1 = *(UInt16 *) buffers[1];
    Float32    *tout0 =  (Float32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = (( tin1==0) ? int_dividebyzero_error( tin1, *tin0) : *tin0         / (double) tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor true_divide_wwxf_vsxv_descr =
{ "true_divide_wwxf_vsxv", (void *) true_divide_wwxf_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(Float32) }, { 0, 1, 0, 0 } };

static int true_divide_wwxf_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    UInt16     *tin1 =  (UInt16 *) buffers[1];
    Float32    *tout0 =  (Float32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, *tin0) : *tin0         / (double) *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor true_divide_wwxf_vvxv_descr =
{ "true_divide_wwxf_vvxv", (void *) true_divide_wwxf_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(Float32) }, { 0, 0, 0, 0 } };

static void _true_divide_wxf_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt16  *tin0   = (UInt16 *) ((char *) input  + inboffset);
    Float32 *tout0  = (Float32 *) ((char *) output + outboffset);
    Float32 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (UInt16 *) ((char *) tin0 + inbstrides[dim]);
            net    = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0)     : net           / (double) *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _true_divide_wxf_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int true_divide_wxf_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _true_divide_wxf_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(true_divide_wxf_R, CHECK_ALIGN, sizeof(UInt16), sizeof(Float32));

static void _true_divide_wxf_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt16 *tin0   = (UInt16 *) ((char *) input  + inboffset);
    Float32 *tout0 = (Float32 *) ((char *) output + outboffset);
    Float32 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (UInt16 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Float32 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0)     : lastval       / (double) *tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _true_divide_wxf_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  true_divide_wxf_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _true_divide_wxf_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(true_divide_wxf_A, CHECK_ALIGN, sizeof(UInt16), sizeof(Float32));

static int true_divide_wwxf_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16      tin0 = *(UInt16 *) buffers[0];
    UInt16     *tin1 =  (UInt16 *) buffers[1];
    Float32    *tout0 =  (Float32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, 0)     : tin0          / (double) *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor true_divide_wwxf_svxv_descr =
{ "true_divide_wwxf_svxv", (void *) true_divide_wwxf_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(Float32) }, { 1, 0, 0, 0 } };
/*********************  true_divide  *********************/
/*********************  remainder  *********************/

static int remainder_wwxw_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    UInt16      tin1 = *(UInt16 *) buffers[1];
    UInt16     *tout0 =  (UInt16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((tin1==0) ? int_dividebyzero_error(tin1, *tin0) : *tin0  %  tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor remainder_wwxw_vsxv_descr =
{ "remainder_wwxw_vsxv", (void *) remainder_wwxw_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(UInt16) }, { 0, 1, 0, 0 } };

static int remainder_wwxw_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    UInt16     *tin1 =  (UInt16 *) buffers[1];
    UInt16     *tout0 =  (UInt16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, *tin0) : *tin0  %  *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor remainder_wwxw_vvxv_descr =
{ "remainder_wwxw_vvxv", (void *) remainder_wwxw_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(UInt16) }, { 0, 0, 0, 0 } };

static void _remainder_wxw_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt16  *tin0   = (UInt16 *) ((char *) input  + inboffset);
    UInt16 *tout0  = (UInt16 *) ((char *) output + outboffset);
    UInt16 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (UInt16 *) ((char *) tin0 + inbstrides[dim]);
            net = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0) : net  %  *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _remainder_wxw_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int remainder_wxw_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _remainder_wxw_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(remainder_wxw_R, CHECK_ALIGN, sizeof(UInt16), sizeof(UInt16));

static void _remainder_wxw_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt16 *tin0   = (UInt16 *) ((char *) input  + inboffset);
    UInt16 *tout0 = (UInt16 *) ((char *) output + outboffset);
    UInt16 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (UInt16 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (UInt16 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0) : lastval  %  *tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _remainder_wxw_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  remainder_wxw_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _remainder_wxw_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(remainder_wxw_A, CHECK_ALIGN, sizeof(UInt16), sizeof(UInt16));

static int remainder_wwxw_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16      tin0 = *(UInt16 *) buffers[0];
    UInt16     *tin1 =  (UInt16 *) buffers[1];
    UInt16     *tout0 =  (UInt16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, 0) : tin0  %  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor remainder_wwxw_svxv_descr =
{ "remainder_wwxw_svxv", (void *) remainder_wwxw_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(UInt16) }, { 1, 0, 0, 0 } };
/*********************  remainder  *********************/
/*********************  power  *********************/

static int power_wwxw_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    UInt16      tin1 = *(UInt16 *) buffers[1];
    UInt16     *tout0 =  (UInt16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_pow(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor power_wwxw_vsxf_descr =
{ "power_wwxw_vsxf", (void *) power_wwxw_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(UInt16) }, { 0, 1, 0, 0 } };

static int power_wwxw_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    UInt16     *tin1 =  (UInt16 *) buffers[1];
    UInt16     *tout0 =  (UInt16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_pow(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor power_wwxw_vvxf_descr =
{ "power_wwxw_vvxf", (void *) power_wwxw_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(UInt16) }, { 0, 0, 0, 0 } };

static void _power_wxw_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt16  *tin0   = (UInt16 *) ((char *) input  + inboffset);
    UInt16 *tout0  = (UInt16 *) ((char *) output + outboffset);
    UInt16 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (UInt16 *) ((char *) tin0 + inbstrides[dim]);
            net   = num_pow(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _power_wxw_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int power_wxw_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _power_wxw_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(power_wxw_R, CHECK_ALIGN, sizeof(UInt16), sizeof(UInt16));

static void _power_wxw_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt16 *tin0   = (UInt16 *) ((char *) input  + inboffset);
    UInt16 *tout0 = (UInt16 *) ((char *) output + outboffset);
    UInt16 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (UInt16 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (UInt16 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = num_pow(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _power_wxw_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  power_wxw_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _power_wxw_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(power_wxw_A, CHECK_ALIGN, sizeof(UInt16), sizeof(UInt16));

static int power_wwxw_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16      tin0 = *(UInt16 *) buffers[0];
    UInt16     *tin1 =  (UInt16 *) buffers[1];
    UInt16     *tout0 =  (UInt16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_pow(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor power_wwxw_svxf_descr =
{ "power_wwxw_svxf", (void *) power_wwxw_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(UInt16) }, { 1, 0, 0, 0 } };
/*********************  abs  *********************/

static int abs_wxw_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    UInt16     *tout0 =  (UInt16 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = fabs(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor abs_wxw_vxf_descr =
{ "abs_wxw_vxf", (void *) abs_wxw_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt16), sizeof(UInt16) }, { 0, 0, 0 } };
/*********************  sin  *********************/

static int sin_wxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = sin(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor sin_wxd_vxf_descr =
{ "sin_wxd_vxf", (void *) sin_wxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt16), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  cos  *********************/

static int cos_wxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = cos(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor cos_wxd_vxf_descr =
{ "cos_wxd_vxf", (void *) cos_wxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt16), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  tan  *********************/

static int tan_wxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = tan(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor tan_wxd_vxf_descr =
{ "tan_wxd_vxf", (void *) tan_wxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt16), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arcsin  *********************/

static int arcsin_wxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = asin(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arcsin_wxd_vxf_descr =
{ "arcsin_wxd_vxf", (void *) arcsin_wxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt16), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arccos  *********************/

static int arccos_wxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = acos(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arccos_wxd_vxf_descr =
{ "arccos_wxd_vxf", (void *) arccos_wxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt16), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arctan  *********************/

static int arctan_wxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = atan(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arctan_wxd_vxf_descr =
{ "arctan_wxd_vxf", (void *) arctan_wxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt16), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arctan2  *********************/

static int arctan2_wwxd_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    UInt16      tin1 = *(UInt16 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = atan2(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arctan2_wwxd_vsxf_descr =
{ "arctan2_wwxd_vsxf", (void *) arctan2_wwxd_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(Float64) }, { 0, 1, 0, 0 } };

static int arctan2_wwxd_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    UInt16     *tin1 =  (UInt16 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = atan2(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arctan2_wwxd_vvxf_descr =
{ "arctan2_wwxd_vvxf", (void *) arctan2_wwxd_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(Float64) }, { 0, 0, 0, 0 } };

static void _arctan2_wxd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt16  *tin0   = (UInt16 *) ((char *) input  + inboffset);
    Float64 *tout0  = (Float64 *) ((char *) output + outboffset);
    Float64 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (UInt16 *) ((char *) tin0 + inbstrides[dim]);
            net   = atan2(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _arctan2_wxd_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int arctan2_wxd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _arctan2_wxd_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(arctan2_wxd_R, CHECK_ALIGN, sizeof(UInt16), sizeof(Float64));

static void _arctan2_wxd_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt16 *tin0   = (UInt16 *) ((char *) input  + inboffset);
    Float64 *tout0 = (Float64 *) ((char *) output + outboffset);
    Float64 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (UInt16 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Float64 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = atan2(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _arctan2_wxd_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  arctan2_wxd_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _arctan2_wxd_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(arctan2_wxd_A, CHECK_ALIGN, sizeof(UInt16), sizeof(Float64));

static int arctan2_wwxd_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16      tin0 = *(UInt16 *) buffers[0];
    UInt16     *tin1 =  (UInt16 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = atan2(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arctan2_wwxd_svxf_descr =
{ "arctan2_wwxd_svxf", (void *) arctan2_wwxd_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(Float64) }, { 1, 0, 0, 0 } };
/*********************  log  *********************/

static int log_wxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_log(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor log_wxd_vxf_descr =
{ "log_wxd_vxf", (void *) log_wxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt16), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  log10  *********************/

static int log10_wxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_log10(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor log10_wxd_vxf_descr =
{ "log10_wxd_vxf", (void *) log10_wxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt16), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  exp  *********************/

static int exp_wxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = exp(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor exp_wxd_vxf_descr =
{ "exp_wxd_vxf", (void *) exp_wxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt16), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  sinh  *********************/

static int sinh_wxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = sinh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor sinh_wxd_vxf_descr =
{ "sinh_wxd_vxf", (void *) sinh_wxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt16), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  cosh  *********************/

static int cosh_wxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = cosh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor cosh_wxd_vxf_descr =
{ "cosh_wxd_vxf", (void *) cosh_wxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt16), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  tanh  *********************/

static int tanh_wxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = tanh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor tanh_wxd_vxf_descr =
{ "tanh_wxd_vxf", (void *) tanh_wxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt16), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arcsinh  *********************/

static int arcsinh_wxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_asinh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arcsinh_wxd_vxf_descr =
{ "arcsinh_wxd_vxf", (void *) arcsinh_wxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt16), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arccosh  *********************/

static int arccosh_wxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_acosh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arccosh_wxd_vxf_descr =
{ "arccosh_wxd_vxf", (void *) arccosh_wxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt16), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arctanh  *********************/

static int arctanh_wxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_atanh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arctanh_wxd_vxf_descr =
{ "arctanh_wxd_vxf", (void *) arctanh_wxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt16), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  ieeemask  *********************/
/*********************  ieeemask  *********************/
/*********************  isnan  *********************/
/*********************  isnan  *********************/
/*********************  isnan  *********************/
/*********************  isnan  *********************/
/*********************  sqrt  *********************/

static int sqrt_wxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = sqrt(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor sqrt_wxd_vxf_descr =
{ "sqrt_wxd_vxf", (void *) sqrt_wxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt16), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  equal  *********************/

static int equal_wwxB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    UInt16      tin1 = *(UInt16 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 == tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor equal_wwxB_vsxv_descr =
{ "equal_wwxB_vsxv", (void *) equal_wwxB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int equal_wwxB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    UInt16     *tin1 =  (UInt16 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 == *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor equal_wwxB_vvxv_descr =
{ "equal_wwxB_vvxv", (void *) equal_wwxB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int equal_wwxB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16      tin0 = *(UInt16 *) buffers[0];
    UInt16     *tin1 =  (UInt16 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 == *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor equal_wwxB_svxv_descr =
{ "equal_wwxB_svxv", (void *) equal_wwxB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  not_equal  *********************/

static int not_equal_wwxB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    UInt16      tin1 = *(UInt16 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 != tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor not_equal_wwxB_vsxv_descr =
{ "not_equal_wwxB_vsxv", (void *) not_equal_wwxB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int not_equal_wwxB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    UInt16     *tin1 =  (UInt16 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 != *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor not_equal_wwxB_vvxv_descr =
{ "not_equal_wwxB_vvxv", (void *) not_equal_wwxB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int not_equal_wwxB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16      tin0 = *(UInt16 *) buffers[0];
    UInt16     *tin1 =  (UInt16 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 != *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor not_equal_wwxB_svxv_descr =
{ "not_equal_wwxB_svxv", (void *) not_equal_wwxB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  greater  *********************/

static int greater_wwxB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    UInt16      tin1 = *(UInt16 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 > tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_wwxB_vsxv_descr =
{ "greater_wwxB_vsxv", (void *) greater_wwxB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int greater_wwxB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    UInt16     *tin1 =  (UInt16 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 > *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_wwxB_vvxv_descr =
{ "greater_wwxB_vvxv", (void *) greater_wwxB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int greater_wwxB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16      tin0 = *(UInt16 *) buffers[0];
    UInt16     *tin1 =  (UInt16 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 > *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_wwxB_svxv_descr =
{ "greater_wwxB_svxv", (void *) greater_wwxB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  greater_equal  *********************/

static int greater_equal_wwxB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    UInt16      tin1 = *(UInt16 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 >= tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_equal_wwxB_vsxv_descr =
{ "greater_equal_wwxB_vsxv", (void *) greater_equal_wwxB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int greater_equal_wwxB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    UInt16     *tin1 =  (UInt16 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 >= *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_equal_wwxB_vvxv_descr =
{ "greater_equal_wwxB_vvxv", (void *) greater_equal_wwxB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int greater_equal_wwxB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16      tin0 = *(UInt16 *) buffers[0];
    UInt16     *tin1 =  (UInt16 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 >= *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_equal_wwxB_svxv_descr =
{ "greater_equal_wwxB_svxv", (void *) greater_equal_wwxB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  less  *********************/

static int less_wwxB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    UInt16      tin1 = *(UInt16 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 < tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_wwxB_vsxv_descr =
{ "less_wwxB_vsxv", (void *) less_wwxB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int less_wwxB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    UInt16     *tin1 =  (UInt16 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 < *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_wwxB_vvxv_descr =
{ "less_wwxB_vvxv", (void *) less_wwxB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int less_wwxB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16      tin0 = *(UInt16 *) buffers[0];
    UInt16     *tin1 =  (UInt16 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 < *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_wwxB_svxv_descr =
{ "less_wwxB_svxv", (void *) less_wwxB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  less_equal  *********************/

static int less_equal_wwxB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    UInt16      tin1 = *(UInt16 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 <= tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_equal_wwxB_vsxv_descr =
{ "less_equal_wwxB_vsxv", (void *) less_equal_wwxB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int less_equal_wwxB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    UInt16     *tin1 =  (UInt16 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 <= *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_equal_wwxB_vvxv_descr =
{ "less_equal_wwxB_vvxv", (void *) less_equal_wwxB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int less_equal_wwxB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16      tin0 = *(UInt16 *) buffers[0];
    UInt16     *tin1 =  (UInt16 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 <= *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_equal_wwxB_svxv_descr =
{ "less_equal_wwxB_svxv", (void *) less_equal_wwxB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  logical_and  *********************/

static int logical_and_wwxB_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    UInt16      tin1 = *(UInt16 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_and(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_and_wwxB_vsxf_descr =
{ "logical_and_wwxB_vsxf", (void *) logical_and_wwxB_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int logical_and_wwxB_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    UInt16     *tin1 =  (UInt16 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_and(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_and_wwxB_vvxf_descr =
{ "logical_and_wwxB_vvxf", (void *) logical_and_wwxB_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int logical_and_wwxB_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16      tin0 = *(UInt16 *) buffers[0];
    UInt16     *tin1 =  (UInt16 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_and(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_and_wwxB_svxf_descr =
{ "logical_and_wwxB_svxf", (void *) logical_and_wwxB_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  logical_or  *********************/

static int logical_or_wwxB_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    UInt16      tin1 = *(UInt16 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_or(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_or_wwxB_vsxf_descr =
{ "logical_or_wwxB_vsxf", (void *) logical_or_wwxB_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int logical_or_wwxB_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    UInt16     *tin1 =  (UInt16 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_or(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_or_wwxB_vvxf_descr =
{ "logical_or_wwxB_vvxf", (void *) logical_or_wwxB_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int logical_or_wwxB_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16      tin0 = *(UInt16 *) buffers[0];
    UInt16     *tin1 =  (UInt16 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_or(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_or_wwxB_svxf_descr =
{ "logical_or_wwxB_svxf", (void *) logical_or_wwxB_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  logical_xor  *********************/

static int logical_xor_wwxB_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    UInt16      tin1 = *(UInt16 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_xor(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_xor_wwxB_vsxf_descr =
{ "logical_xor_wwxB_vsxf", (void *) logical_xor_wwxB_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int logical_xor_wwxB_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    UInt16     *tin1 =  (UInt16 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_xor(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_xor_wwxB_vvxf_descr =
{ "logical_xor_wwxB_vvxf", (void *) logical_xor_wwxB_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int logical_xor_wwxB_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16      tin0 = *(UInt16 *) buffers[0];
    UInt16     *tin1 =  (UInt16 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_xor(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_xor_wwxB_svxf_descr =
{ "logical_xor_wwxB_svxf", (void *) logical_xor_wwxB_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  logical_and  *********************/
/*********************  logical_or  *********************/
/*********************  logical_xor  *********************/
/*********************  logical_not  *********************/

static int logical_not_wxB_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    Bool       *tout0 =  (Bool *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = !*tin0;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_not_wxB_vxv_descr =
{ "logical_not_wxB_vxv", (void *) logical_not_wxB_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt16), sizeof(Bool) }, { 0, 0, 0 } };
/*********************  bitwise_and  *********************/

static int bitwise_and_wwxw_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    UInt16      tin1 = *(UInt16 *) buffers[1];
    UInt16     *tout0 =  (UInt16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 & tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_and_wwxw_vsxv_descr =
{ "bitwise_and_wwxw_vsxv", (void *) bitwise_and_wwxw_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(UInt16) }, { 0, 1, 0, 0 } };

static int bitwise_and_wwxw_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    UInt16     *tin1 =  (UInt16 *) buffers[1];
    UInt16     *tout0 =  (UInt16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 & *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_and_wwxw_vvxv_descr =
{ "bitwise_and_wwxw_vvxv", (void *) bitwise_and_wwxw_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(UInt16) }, { 0, 0, 0, 0 } };

static void _bitwise_and_wxw_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt16  *tin0   = (UInt16 *) ((char *) input  + inboffset);
    UInt16 *tout0  = (UInt16 *) ((char *) output + outboffset);
    UInt16 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (UInt16 *) ((char *) tin0 + inbstrides[dim]);
            net    =     net & *tin0;
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _bitwise_and_wxw_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int bitwise_and_wxw_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _bitwise_and_wxw_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(bitwise_and_wxw_R, CHECK_ALIGN, sizeof(UInt16), sizeof(UInt16));

static void _bitwise_and_wxw_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt16 *tin0   = (UInt16 *) ((char *) input  + inboffset);
    UInt16 *tout0 = (UInt16 *) ((char *) output + outboffset);
    UInt16 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (UInt16 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (UInt16 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = lastval & *tin0;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _bitwise_and_wxw_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  bitwise_and_wxw_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _bitwise_and_wxw_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(bitwise_and_wxw_A, CHECK_ALIGN, sizeof(UInt16), sizeof(UInt16));

static int bitwise_and_wwxw_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16      tin0 = *(UInt16 *) buffers[0];
    UInt16     *tin1 =  (UInt16 *) buffers[1];
    UInt16     *tout0 =  (UInt16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 & *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_and_wwxw_svxv_descr =
{ "bitwise_and_wwxw_svxv", (void *) bitwise_and_wwxw_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(UInt16) }, { 1, 0, 0, 0 } };
/*********************  bitwise_or  *********************/

static int bitwise_or_wwxw_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    UInt16      tin1 = *(UInt16 *) buffers[1];
    UInt16     *tout0 =  (UInt16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 | tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_or_wwxw_vsxv_descr =
{ "bitwise_or_wwxw_vsxv", (void *) bitwise_or_wwxw_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(UInt16) }, { 0, 1, 0, 0 } };

static int bitwise_or_wwxw_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    UInt16     *tin1 =  (UInt16 *) buffers[1];
    UInt16     *tout0 =  (UInt16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 | *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_or_wwxw_vvxv_descr =
{ "bitwise_or_wwxw_vvxv", (void *) bitwise_or_wwxw_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(UInt16) }, { 0, 0, 0, 0 } };

static void _bitwise_or_wxw_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt16  *tin0   = (UInt16 *) ((char *) input  + inboffset);
    UInt16 *tout0  = (UInt16 *) ((char *) output + outboffset);
    UInt16 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (UInt16 *) ((char *) tin0 + inbstrides[dim]);
            net    =     net | *tin0;
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _bitwise_or_wxw_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int bitwise_or_wxw_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _bitwise_or_wxw_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(bitwise_or_wxw_R, CHECK_ALIGN, sizeof(UInt16), sizeof(UInt16));

static void _bitwise_or_wxw_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt16 *tin0   = (UInt16 *) ((char *) input  + inboffset);
    UInt16 *tout0 = (UInt16 *) ((char *) output + outboffset);
    UInt16 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (UInt16 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (UInt16 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = lastval | *tin0;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _bitwise_or_wxw_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  bitwise_or_wxw_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _bitwise_or_wxw_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(bitwise_or_wxw_A, CHECK_ALIGN, sizeof(UInt16), sizeof(UInt16));

static int bitwise_or_wwxw_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16      tin0 = *(UInt16 *) buffers[0];
    UInt16     *tin1 =  (UInt16 *) buffers[1];
    UInt16     *tout0 =  (UInt16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 | *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_or_wwxw_svxv_descr =
{ "bitwise_or_wwxw_svxv", (void *) bitwise_or_wwxw_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(UInt16) }, { 1, 0, 0, 0 } };
/*********************  bitwise_xor  *********************/

static int bitwise_xor_wwxw_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    UInt16      tin1 = *(UInt16 *) buffers[1];
    UInt16     *tout0 =  (UInt16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 ^ tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_xor_wwxw_vsxv_descr =
{ "bitwise_xor_wwxw_vsxv", (void *) bitwise_xor_wwxw_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(UInt16) }, { 0, 1, 0, 0 } };

static int bitwise_xor_wwxw_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    UInt16     *tin1 =  (UInt16 *) buffers[1];
    UInt16     *tout0 =  (UInt16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 ^ *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_xor_wwxw_vvxv_descr =
{ "bitwise_xor_wwxw_vvxv", (void *) bitwise_xor_wwxw_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(UInt16) }, { 0, 0, 0, 0 } };

static void _bitwise_xor_wxw_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt16  *tin0   = (UInt16 *) ((char *) input  + inboffset);
    UInt16 *tout0  = (UInt16 *) ((char *) output + outboffset);
    UInt16 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (UInt16 *) ((char *) tin0 + inbstrides[dim]);
            net    =     net ^ *tin0;
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _bitwise_xor_wxw_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int bitwise_xor_wxw_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _bitwise_xor_wxw_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(bitwise_xor_wxw_R, CHECK_ALIGN, sizeof(UInt16), sizeof(UInt16));

static void _bitwise_xor_wxw_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt16 *tin0   = (UInt16 *) ((char *) input  + inboffset);
    UInt16 *tout0 = (UInt16 *) ((char *) output + outboffset);
    UInt16 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (UInt16 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (UInt16 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = lastval ^ *tin0;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _bitwise_xor_wxw_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  bitwise_xor_wxw_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _bitwise_xor_wxw_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(bitwise_xor_wxw_A, CHECK_ALIGN, sizeof(UInt16), sizeof(UInt16));

static int bitwise_xor_wwxw_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16      tin0 = *(UInt16 *) buffers[0];
    UInt16     *tin1 =  (UInt16 *) buffers[1];
    UInt16     *tout0 =  (UInt16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 ^ *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_xor_wwxw_svxv_descr =
{ "bitwise_xor_wwxw_svxv", (void *) bitwise_xor_wwxw_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(UInt16) }, { 1, 0, 0, 0 } };
/*********************  bitwise_not  *********************/

static int bitwise_not_wxw_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    UInt16     *tout0 =  (UInt16 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ~*tin0;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_not_wxw_vxv_descr =
{ "bitwise_not_wxw_vxv", (void *) bitwise_not_wxw_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt16), sizeof(UInt16) }, { 0, 0, 0 } };
/*********************  bitwise_not  *********************/
/*********************  rshift  *********************/

static int rshift_wwxw_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    UInt16      tin1 = *(UInt16 *) buffers[1];
    UInt16     *tout0 =  (UInt16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 >> tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor rshift_wwxw_vsxv_descr =
{ "rshift_wwxw_vsxv", (void *) rshift_wwxw_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(UInt16) }, { 0, 1, 0, 0 } };

static int rshift_wwxw_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    UInt16     *tin1 =  (UInt16 *) buffers[1];
    UInt16     *tout0 =  (UInt16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 >> *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor rshift_wwxw_vvxv_descr =
{ "rshift_wwxw_vvxv", (void *) rshift_wwxw_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(UInt16) }, { 0, 0, 0, 0 } };

static int rshift_wwxw_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16      tin0 = *(UInt16 *) buffers[0];
    UInt16     *tin1 =  (UInt16 *) buffers[1];
    UInt16     *tout0 =  (UInt16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 >> *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor rshift_wwxw_svxv_descr =
{ "rshift_wwxw_svxv", (void *) rshift_wwxw_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(UInt16) }, { 1, 0, 0, 0 } };
/*********************  lshift  *********************/

static int lshift_wwxw_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    UInt16      tin1 = *(UInt16 *) buffers[1];
    UInt16     *tout0 =  (UInt16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 << tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor lshift_wwxw_vsxv_descr =
{ "lshift_wwxw_vsxv", (void *) lshift_wwxw_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(UInt16) }, { 0, 1, 0, 0 } };

static int lshift_wwxw_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    UInt16     *tin1 =  (UInt16 *) buffers[1];
    UInt16     *tout0 =  (UInt16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 << *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor lshift_wwxw_vvxv_descr =
{ "lshift_wwxw_vvxv", (void *) lshift_wwxw_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(UInt16) }, { 0, 0, 0, 0 } };

static int lshift_wwxw_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16      tin0 = *(UInt16 *) buffers[0];
    UInt16     *tin1 =  (UInt16 *) buffers[1];
    UInt16     *tout0 =  (UInt16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 << *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor lshift_wwxw_svxv_descr =
{ "lshift_wwxw_svxv", (void *) lshift_wwxw_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(UInt16) }, { 1, 0, 0, 0 } };
/*********************  floor  *********************/

static int floor_wxw_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    UInt16     *tout0 =  (UInt16 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = (*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor floor_wxw_vxf_descr =
{ "floor_wxw_vxf", (void *) floor_wxw_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt16), sizeof(UInt16) }, { 0, 0, 0 } };
/*********************  floor  *********************/
/*********************  ceil  *********************/

static int ceil_wxw_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    UInt16     *tout0 =  (UInt16 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = (*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor ceil_wxw_vxf_descr =
{ "ceil_wxw_vxf", (void *) ceil_wxw_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt16), sizeof(UInt16) }, { 0, 0, 0 } };
/*********************  ceil  *********************/
/*********************  maximum  *********************/

static int maximum_wwxw_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    UInt16      tin1 = *(UInt16 *) buffers[1];
    UInt16     *tout0 =  (UInt16 *) buffers[2];
UInt16 temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufmaximum(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor maximum_wwxw_vsxf_descr =
{ "maximum_wwxw_vsxf", (void *) maximum_wwxw_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(UInt16) }, { 0, 1, 0, 0 } };

static int maximum_wwxw_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    UInt16     *tin1 =  (UInt16 *) buffers[1];
    UInt16     *tout0 =  (UInt16 *) buffers[2];
UInt16 temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufmaximum(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor maximum_wwxw_vvxf_descr =
{ "maximum_wwxw_vvxf", (void *) maximum_wwxw_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(UInt16) }, { 0, 0, 0, 0 } };

static void _maximum_wxw_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt16  *tin0   = (UInt16 *) ((char *) input  + inboffset);
    UInt16 *tout0  = (UInt16 *) ((char *) output + outboffset);
    UInt16 net;
    UInt16 temp1, temp2;
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (UInt16 *) ((char *) tin0 + inbstrides[dim]);
            net   = ufmaximum(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _maximum_wxw_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int maximum_wxw_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _maximum_wxw_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(maximum_wxw_R, CHECK_ALIGN, sizeof(UInt16), sizeof(UInt16));

static void _maximum_wxw_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt16 *tin0   = (UInt16 *) ((char *) input  + inboffset);
    UInt16 *tout0 = (UInt16 *) ((char *) output + outboffset);
    UInt16 lastval;
    UInt16 temp1, temp2;
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (UInt16 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (UInt16 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = ufmaximum(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _maximum_wxw_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  maximum_wxw_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _maximum_wxw_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(maximum_wxw_A, CHECK_ALIGN, sizeof(UInt16), sizeof(UInt16));

static int maximum_wwxw_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16      tin0 = *(UInt16 *) buffers[0];
    UInt16     *tin1 =  (UInt16 *) buffers[1];
    UInt16     *tout0 =  (UInt16 *) buffers[2];
UInt16 temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufmaximum(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor maximum_wwxw_svxf_descr =
{ "maximum_wwxw_svxf", (void *) maximum_wwxw_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(UInt16) }, { 1, 0, 0, 0 } };
/*********************  minimum  *********************/

static int minimum_wwxw_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    UInt16      tin1 = *(UInt16 *) buffers[1];
    UInt16     *tout0 =  (UInt16 *) buffers[2];
UInt16 temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufminimum(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor minimum_wwxw_vsxf_descr =
{ "minimum_wwxw_vsxf", (void *) minimum_wwxw_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(UInt16) }, { 0, 1, 0, 0 } };

static int minimum_wwxw_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    UInt16     *tin1 =  (UInt16 *) buffers[1];
    UInt16     *tout0 =  (UInt16 *) buffers[2];
UInt16 temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufminimum(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor minimum_wwxw_vvxf_descr =
{ "minimum_wwxw_vvxf", (void *) minimum_wwxw_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(UInt16) }, { 0, 0, 0, 0 } };

static void _minimum_wxw_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt16  *tin0   = (UInt16 *) ((char *) input  + inboffset);
    UInt16 *tout0  = (UInt16 *) ((char *) output + outboffset);
    UInt16 net;
    UInt16 temp1, temp2;
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (UInt16 *) ((char *) tin0 + inbstrides[dim]);
            net   = ufminimum(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _minimum_wxw_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int minimum_wxw_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _minimum_wxw_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(minimum_wxw_R, CHECK_ALIGN, sizeof(UInt16), sizeof(UInt16));

static void _minimum_wxw_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt16 *tin0   = (UInt16 *) ((char *) input  + inboffset);
    UInt16 *tout0 = (UInt16 *) ((char *) output + outboffset);
    UInt16 lastval;
    UInt16 temp1, temp2;
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (UInt16 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (UInt16 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = ufminimum(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _minimum_wxw_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  minimum_wxw_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _minimum_wxw_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(minimum_wxw_A, CHECK_ALIGN, sizeof(UInt16), sizeof(UInt16));

static int minimum_wwxw_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16      tin0 = *(UInt16 *) buffers[0];
    UInt16     *tin1 =  (UInt16 *) buffers[1];
    UInt16     *tout0 =  (UInt16 *) buffers[2];
UInt16 temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufminimum(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor minimum_wwxw_svxf_descr =
{ "minimum_wwxw_svxf", (void *) minimum_wwxw_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(UInt16) }, { 1, 0, 0, 0 } };
/*********************  fabs  *********************/

static int fabs_wxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = fabs(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor fabs_wxd_vxf_descr =
{ "fabs_wxd_vxf", (void *) fabs_wxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt16), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  _round  *********************/

static int _round_wxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_round(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor _round_wxd_vxf_descr =
{ "_round_wxd_vxf", (void *) _round_wxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt16), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  hypot  *********************/

static int hypot_wwxd_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    UInt16      tin1 = *(UInt16 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = hypot(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor hypot_wwxd_vsxf_descr =
{ "hypot_wwxd_vsxf", (void *) hypot_wwxd_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(Float64) }, { 0, 1, 0, 0 } };

static int hypot_wwxd_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16     *tin0 =  (UInt16 *) buffers[0];
    UInt16     *tin1 =  (UInt16 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = hypot(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor hypot_wwxd_vvxf_descr =
{ "hypot_wwxd_vvxf", (void *) hypot_wwxd_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(Float64) }, { 0, 0, 0, 0 } };

static void _hypot_wxd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt16  *tin0   = (UInt16 *) ((char *) input  + inboffset);
    Float64 *tout0  = (Float64 *) ((char *) output + outboffset);
    Float64 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (UInt16 *) ((char *) tin0 + inbstrides[dim]);
            net   = hypot(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _hypot_wxd_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int hypot_wxd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _hypot_wxd_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(hypot_wxd_R, CHECK_ALIGN, sizeof(UInt16), sizeof(Float64));

static void _hypot_wxd_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt16 *tin0   = (UInt16 *) ((char *) input  + inboffset);
    Float64 *tout0 = (Float64 *) ((char *) output + outboffset);
    Float64 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (UInt16 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Float64 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = hypot(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _hypot_wxd_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  hypot_wxd_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _hypot_wxd_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(hypot_wxd_A, CHECK_ALIGN, sizeof(UInt16), sizeof(Float64));

static int hypot_wwxd_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt16      tin0 = *(UInt16 *) buffers[0];
    UInt16     *tin1 =  (UInt16 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = hypot(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor hypot_wwxd_svxf_descr =
{ "hypot_wwxd_svxf", (void *) hypot_wwxd_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt16), sizeof(UInt16), sizeof(Float64) }, { 1, 0, 0, 0 } };
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

static PyMethodDef _ufuncUInt16Methods[] = {

	{NULL,      NULL}        /* Sentinel */
};

static PyObject *init_funcDict(void) {
    PyObject *dict, *keytuple, *cfunc;
    dict = PyDict_New();
    /* minus_wxw_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","minus","v","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minus_wxw_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_wwxw_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","add","vs","UInt16","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_wwxw_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_wwxw_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","add","vv","UInt16","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_wwxw_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_wxw_R */
    keytuple=Py_BuildValue("ss((s)(s))","add","R","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_wxw_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_wxw_A */
    keytuple=Py_BuildValue("ss((s)(s))","add","A","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_wxw_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_wwxw_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","add","sv","UInt16","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_wwxw_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_wwxw_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","subtract","vs","UInt16","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_wwxw_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_wwxw_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","subtract","vv","UInt16","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_wwxw_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_wxw_R */
    keytuple=Py_BuildValue("ss((s)(s))","subtract","R","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_wxw_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_wxw_A */
    keytuple=Py_BuildValue("ss((s)(s))","subtract","A","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_wxw_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_wwxw_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","subtract","sv","UInt16","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_wwxw_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* multiply_wwxw_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","multiply","vs","UInt16","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&multiply_wwxw_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* multiply_wwxw_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","multiply","vv","UInt16","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&multiply_wwxw_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* multiply_wxw_R */
    keytuple=Py_BuildValue("ss((s)(s))","multiply","R","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&multiply_wxw_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* multiply_wxw_A */
    keytuple=Py_BuildValue("ss((s)(s))","multiply","A","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&multiply_wxw_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* multiply_wwxw_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","multiply","sv","UInt16","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&multiply_wwxw_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_wwxw_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","divide","vs","UInt16","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_wwxw_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_wwxw_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","divide","vv","UInt16","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_wwxw_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_wxw_R */
    keytuple=Py_BuildValue("ss((s)(s))","divide","R","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_wxw_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_wxw_A */
    keytuple=Py_BuildValue("ss((s)(s))","divide","A","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_wxw_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_wwxw_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","divide","sv","UInt16","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_wwxw_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_divide_wwxw_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","floor_divide","vs","UInt16","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_divide_wwxw_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_divide_wwxw_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","floor_divide","vv","UInt16","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_divide_wwxw_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_divide_wxw_R */
    keytuple=Py_BuildValue("ss((s)(s))","floor_divide","R","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_divide_wxw_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_divide_wxw_A */
    keytuple=Py_BuildValue("ss((s)(s))","floor_divide","A","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_divide_wxw_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_divide_wwxw_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","floor_divide","sv","UInt16","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_divide_wwxw_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_wwxf_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","true_divide","vs","UInt16","UInt16","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_wwxf_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_wwxf_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","true_divide","vv","UInt16","UInt16","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_wwxf_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_wxf_R */
    keytuple=Py_BuildValue("ss((s)(s))","true_divide","R","UInt16","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_wxf_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_wxf_A */
    keytuple=Py_BuildValue("ss((s)(s))","true_divide","A","UInt16","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_wxf_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_wwxf_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","true_divide","sv","UInt16","UInt16","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_wwxf_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_wwxw_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","remainder","vs","UInt16","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_wwxw_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_wwxw_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","remainder","vv","UInt16","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_wwxw_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_wxw_R */
    keytuple=Py_BuildValue("ss((s)(s))","remainder","R","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_wxw_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_wxw_A */
    keytuple=Py_BuildValue("ss((s)(s))","remainder","A","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_wxw_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_wwxw_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","remainder","sv","UInt16","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_wwxw_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_wwxw_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","power","vs","UInt16","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_wwxw_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_wwxw_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","power","vv","UInt16","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_wwxw_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_wxw_R */
    keytuple=Py_BuildValue("ss((s)(s))","power","R","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_wxw_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_wxw_A */
    keytuple=Py_BuildValue("ss((s)(s))","power","A","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_wxw_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_wwxw_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","power","sv","UInt16","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_wwxw_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* abs_wxw_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","abs","v","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&abs_wxw_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* sin_wxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","sin","v","UInt16","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&sin_wxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* cos_wxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","cos","v","UInt16","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&cos_wxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* tan_wxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","tan","v","UInt16","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&tan_wxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arcsin_wxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arcsin","v","UInt16","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arcsin_wxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arccos_wxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arccos","v","UInt16","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arccos_wxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan_wxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arctan","v","UInt16","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan_wxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan2_wwxd_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","arctan2","vs","UInt16","UInt16","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan2_wwxd_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan2_wwxd_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","arctan2","vv","UInt16","UInt16","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan2_wwxd_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan2_wxd_R */
    keytuple=Py_BuildValue("ss((s)(s))","arctan2","R","UInt16","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan2_wxd_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan2_wxd_A */
    keytuple=Py_BuildValue("ss((s)(s))","arctan2","A","UInt16","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan2_wxd_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan2_wwxd_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","arctan2","sv","UInt16","UInt16","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan2_wwxd_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* log_wxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","log","v","UInt16","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&log_wxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* log10_wxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","log10","v","UInt16","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&log10_wxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* exp_wxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","exp","v","UInt16","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&exp_wxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* sinh_wxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","sinh","v","UInt16","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&sinh_wxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* cosh_wxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","cosh","v","UInt16","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&cosh_wxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* tanh_wxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","tanh","v","UInt16","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&tanh_wxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arcsinh_wxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arcsinh","v","UInt16","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arcsinh_wxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arccosh_wxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arccosh","v","UInt16","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arccosh_wxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctanh_wxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arctanh","v","UInt16","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctanh_wxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* sqrt_wxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","sqrt","v","UInt16","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&sqrt_wxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* equal_wwxB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","equal","vs","UInt16","UInt16","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&equal_wwxB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* equal_wwxB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","equal","vv","UInt16","UInt16","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&equal_wwxB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* equal_wwxB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","equal","sv","UInt16","UInt16","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&equal_wwxB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* not_equal_wwxB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","not_equal","vs","UInt16","UInt16","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&not_equal_wwxB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* not_equal_wwxB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","not_equal","vv","UInt16","UInt16","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&not_equal_wwxB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* not_equal_wwxB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","not_equal","sv","UInt16","UInt16","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&not_equal_wwxB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_wwxB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater","vs","UInt16","UInt16","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_wwxB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_wwxB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater","vv","UInt16","UInt16","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_wwxB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_wwxB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater","sv","UInt16","UInt16","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_wwxB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_equal_wwxB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater_equal","vs","UInt16","UInt16","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_equal_wwxB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_equal_wwxB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater_equal","vv","UInt16","UInt16","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_equal_wwxB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_equal_wwxB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater_equal","sv","UInt16","UInt16","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_equal_wwxB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_wwxB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less","vs","UInt16","UInt16","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_wwxB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_wwxB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less","vv","UInt16","UInt16","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_wwxB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_wwxB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less","sv","UInt16","UInt16","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_wwxB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_equal_wwxB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less_equal","vs","UInt16","UInt16","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_equal_wwxB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_equal_wwxB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less_equal","vv","UInt16","UInt16","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_equal_wwxB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_equal_wwxB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less_equal","sv","UInt16","UInt16","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_equal_wwxB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_and_wwxB_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_and","vs","UInt16","UInt16","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_and_wwxB_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_and_wwxB_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_and","vv","UInt16","UInt16","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_and_wwxB_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_and_wwxB_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_and","sv","UInt16","UInt16","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_and_wwxB_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_or_wwxB_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_or","vs","UInt16","UInt16","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_or_wwxB_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_or_wwxB_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_or","vv","UInt16","UInt16","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_or_wwxB_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_or_wwxB_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_or","sv","UInt16","UInt16","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_or_wwxB_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_xor_wwxB_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_xor","vs","UInt16","UInt16","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_xor_wwxB_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_xor_wwxB_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_xor","vv","UInt16","UInt16","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_xor_wwxB_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_xor_wwxB_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_xor","sv","UInt16","UInt16","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_xor_wwxB_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_not_wxB_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","logical_not","v","UInt16","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_not_wxB_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_and_wwxw_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_and","vs","UInt16","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_and_wwxw_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_and_wwxw_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_and","vv","UInt16","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_and_wwxw_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_and_wxw_R */
    keytuple=Py_BuildValue("ss((s)(s))","bitwise_and","R","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_and_wxw_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_and_wxw_A */
    keytuple=Py_BuildValue("ss((s)(s))","bitwise_and","A","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_and_wxw_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_and_wwxw_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_and","sv","UInt16","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_and_wwxw_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_or_wwxw_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_or","vs","UInt16","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_or_wwxw_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_or_wwxw_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_or","vv","UInt16","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_or_wwxw_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_or_wxw_R */
    keytuple=Py_BuildValue("ss((s)(s))","bitwise_or","R","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_or_wxw_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_or_wxw_A */
    keytuple=Py_BuildValue("ss((s)(s))","bitwise_or","A","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_or_wxw_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_or_wwxw_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_or","sv","UInt16","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_or_wwxw_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_xor_wwxw_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_xor","vs","UInt16","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_xor_wwxw_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_xor_wwxw_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_xor","vv","UInt16","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_xor_wwxw_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_xor_wxw_R */
    keytuple=Py_BuildValue("ss((s)(s))","bitwise_xor","R","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_xor_wxw_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_xor_wxw_A */
    keytuple=Py_BuildValue("ss((s)(s))","bitwise_xor","A","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_xor_wxw_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_xor_wwxw_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_xor","sv","UInt16","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_xor_wwxw_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_not_wxw_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","bitwise_not","v","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_not_wxw_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* rshift_wwxw_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","rshift","vs","UInt16","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&rshift_wwxw_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* rshift_wwxw_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","rshift","vv","UInt16","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&rshift_wwxw_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* rshift_wwxw_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","rshift","sv","UInt16","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&rshift_wwxw_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* lshift_wwxw_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","lshift","vs","UInt16","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&lshift_wwxw_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* lshift_wwxw_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","lshift","vv","UInt16","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&lshift_wwxw_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* lshift_wwxw_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","lshift","sv","UInt16","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&lshift_wwxw_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_wxw_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","floor","v","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_wxw_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* ceil_wxw_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","ceil","v","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&ceil_wxw_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_wwxw_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","maximum","vs","UInt16","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_wwxw_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_wwxw_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","maximum","vv","UInt16","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_wwxw_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_wxw_R */
    keytuple=Py_BuildValue("ss((s)(s))","maximum","R","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_wxw_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_wxw_A */
    keytuple=Py_BuildValue("ss((s)(s))","maximum","A","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_wxw_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_wwxw_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","maximum","sv","UInt16","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_wwxw_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_wwxw_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","minimum","vs","UInt16","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_wwxw_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_wwxw_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","minimum","vv","UInt16","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_wwxw_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_wxw_R */
    keytuple=Py_BuildValue("ss((s)(s))","minimum","R","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_wxw_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_wxw_A */
    keytuple=Py_BuildValue("ss((s)(s))","minimum","A","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_wxw_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_wwxw_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","minimum","sv","UInt16","UInt16","UInt16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_wwxw_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* fabs_wxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","fabs","v","UInt16","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&fabs_wxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* _round_wxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","_round","v","UInt16","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&_round_wxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_wwxd_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","hypot","vs","UInt16","UInt16","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_wwxd_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_wwxd_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","hypot","vv","UInt16","UInt16","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_wwxd_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_wxd_R */
    keytuple=Py_BuildValue("ss((s)(s))","hypot","R","UInt16","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_wxd_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_wxd_A */
    keytuple=Py_BuildValue("ss((s)(s))","hypot","A","UInt16","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_wxd_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_wwxd_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","hypot","sv","UInt16","UInt16","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_wwxd_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    return dict;
}

/* platform independent*/
#ifdef MS_WIN32
__declspec(dllexport)
#endif
void init_ufuncUInt16(void) {
    PyObject *m, *d, *functions;
    m = Py_InitModule("_ufuncUInt16", _ufuncUInt16Methods);
    d = PyModule_GetDict(m);
    import_libnumarray();
    functions = init_funcDict();
    PyDict_SetItemString(d, "functionDict", functions);
    Py_DECREF(functions);
    ADD_VERSION(m);
}
