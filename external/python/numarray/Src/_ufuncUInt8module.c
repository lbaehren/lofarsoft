
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

static int minus_bxb_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    UInt8      *tout0 =  (UInt8 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = -*tin0;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor minus_bxb_vxv_descr =
{ "minus_bxb_vxv", (void *) minus_bxb_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt8), sizeof(UInt8) }, { 0, 0, 0 } };
/*********************  add  *********************/

static int add_bbxb_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    UInt8       tin1 = *(UInt8 *) buffers[1];
    UInt8      *tout0 =  (UInt8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 + tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor add_bbxb_vsxv_descr =
{ "add_bbxb_vsxv", (void *) add_bbxb_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(UInt8) }, { 0, 1, 0, 0 } };

static int add_bbxb_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    UInt8      *tin1 =  (UInt8 *) buffers[1];
    UInt8      *tout0 =  (UInt8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 + *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor add_bbxb_vvxv_descr =
{ "add_bbxb_vvxv", (void *) add_bbxb_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(UInt8) }, { 0, 0, 0, 0 } };

static void _add_bxb_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt8  *tin0   = (UInt8 *) ((char *) input  + inboffset);
    UInt8 *tout0  = (UInt8 *) ((char *) output + outboffset);
    UInt8 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (UInt8 *) ((char *) tin0 + inbstrides[dim]);
            net    =     net + *tin0;
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _add_bxb_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int add_bxb_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _add_bxb_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(add_bxb_R, CHECK_ALIGN, sizeof(UInt8), sizeof(UInt8));

static void _add_bxb_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt8 *tin0   = (UInt8 *) ((char *) input  + inboffset);
    UInt8 *tout0 = (UInt8 *) ((char *) output + outboffset);
    UInt8 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (UInt8 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (UInt8 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = lastval + *tin0;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _add_bxb_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  add_bxb_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _add_bxb_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(add_bxb_A, CHECK_ALIGN, sizeof(UInt8), sizeof(UInt8));

static int add_bbxb_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8       tin0 = *(UInt8 *) buffers[0];
    UInt8      *tin1 =  (UInt8 *) buffers[1];
    UInt8      *tout0 =  (UInt8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 + *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor add_bbxb_svxv_descr =
{ "add_bbxb_svxv", (void *) add_bbxb_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(UInt8) }, { 1, 0, 0, 0 } };
/*********************  subtract  *********************/

static int subtract_bbxb_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    UInt8       tin1 = *(UInt8 *) buffers[1];
    UInt8      *tout0 =  (UInt8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 - tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor subtract_bbxb_vsxv_descr =
{ "subtract_bbxb_vsxv", (void *) subtract_bbxb_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(UInt8) }, { 0, 1, 0, 0 } };

static int subtract_bbxb_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    UInt8      *tin1 =  (UInt8 *) buffers[1];
    UInt8      *tout0 =  (UInt8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 - *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor subtract_bbxb_vvxv_descr =
{ "subtract_bbxb_vvxv", (void *) subtract_bbxb_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(UInt8) }, { 0, 0, 0, 0 } };

static void _subtract_bxb_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt8  *tin0   = (UInt8 *) ((char *) input  + inboffset);
    UInt8 *tout0  = (UInt8 *) ((char *) output + outboffset);
    UInt8 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (UInt8 *) ((char *) tin0 + inbstrides[dim]);
            net    =     net - *tin0;
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _subtract_bxb_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int subtract_bxb_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _subtract_bxb_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(subtract_bxb_R, CHECK_ALIGN, sizeof(UInt8), sizeof(UInt8));

static void _subtract_bxb_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt8 *tin0   = (UInt8 *) ((char *) input  + inboffset);
    UInt8 *tout0 = (UInt8 *) ((char *) output + outboffset);
    UInt8 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (UInt8 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (UInt8 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = lastval - *tin0;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _subtract_bxb_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  subtract_bxb_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _subtract_bxb_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(subtract_bxb_A, CHECK_ALIGN, sizeof(UInt8), sizeof(UInt8));

static int subtract_bbxb_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8       tin0 = *(UInt8 *) buffers[0];
    UInt8      *tin1 =  (UInt8 *) buffers[1];
    UInt8      *tout0 =  (UInt8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 - *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor subtract_bbxb_svxv_descr =
{ "subtract_bbxb_svxv", (void *) subtract_bbxb_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(UInt8) }, { 1, 0, 0, 0 } };
/*********************  multiply  *********************/
/*********************  multiply  *********************/

static int multiply_bbxb_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    UInt8       tin1 = *(UInt8 *) buffers[1];
    UInt8      *tout0 =  (UInt8 *) buffers[2];
Int32 temp;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        temp = ((Int32) *tin0) * ((Int32) tin1);
    if (temp > 255) temp = int_overflow_error(255.);
    *tout0 = (UInt8) temp;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor multiply_bbxb_vsxv_descr =
{ "multiply_bbxb_vsxv", (void *) multiply_bbxb_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(UInt8) }, { 0, 1, 0, 0 } };

static int multiply_bbxb_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    UInt8      *tin1 =  (UInt8 *) buffers[1];
    UInt8      *tout0 =  (UInt8 *) buffers[2];
Int32 temp;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        temp = ((Int32) *tin0) * ((Int32) *tin1);
    if (temp > 255) temp = int_overflow_error(255.);
    *tout0 = (UInt8) temp;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor multiply_bbxb_vvxv_descr =
{ "multiply_bbxb_vvxv", (void *) multiply_bbxb_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(UInt8) }, { 0, 0, 0, 0 } };

static void _multiply_bxb_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt8  *tin0   = (UInt8 *) ((char *) input  + inboffset);
    UInt8 *tout0  = (UInt8 *) ((char *) output + outboffset);
    UInt8 net;
    Int32 temp;
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (UInt8 *) ((char *) tin0 + inbstrides[dim]);
            temp = ((Int32) net) * ((Int32) *tin0);
    if (temp > 255) temp = int_overflow_error(255.);
    net = (UInt8) temp;
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _multiply_bxb_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int multiply_bxb_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _multiply_bxb_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(multiply_bxb_R, CHECK_ALIGN, sizeof(UInt8), sizeof(UInt8));

static void _multiply_bxb_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt8 *tin0   = (UInt8 *) ((char *) input  + inboffset);
    UInt8 *tout0 = (UInt8 *) ((char *) output + outboffset);
    UInt8 lastval;
    Int32 temp;
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (UInt8 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (UInt8 *) ((char *) tout0 + outbstrides[dim]);
            temp = ((Int32) lastval) * ((Int32) *tin0);
    if (temp > 255) temp = int_overflow_error(255.);
    *tout0 = (UInt8) temp;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _multiply_bxb_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  multiply_bxb_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _multiply_bxb_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(multiply_bxb_A, CHECK_ALIGN, sizeof(UInt8), sizeof(UInt8));

static int multiply_bbxb_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8       tin0 = *(UInt8 *) buffers[0];
    UInt8      *tin1 =  (UInt8 *) buffers[1];
    UInt8      *tout0 =  (UInt8 *) buffers[2];
Int32 temp;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        temp = ((Int32) tin0) * ((Int32) *tin1);
    if (temp > 255) temp = int_overflow_error(255.);
    *tout0 = (UInt8) temp;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor multiply_bbxb_svxv_descr =
{ "multiply_bbxb_svxv", (void *) multiply_bbxb_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(UInt8) }, { 1, 0, 0, 0 } };
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  divide  *********************/

static int divide_bbxb_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    UInt8       tin1 = *(UInt8 *) buffers[1];
    UInt8      *tout0 =  (UInt8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((tin1==0) ? int_dividebyzero_error(tin1, *tin0) : *tin0 / tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor divide_bbxb_vsxv_descr =
{ "divide_bbxb_vsxv", (void *) divide_bbxb_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(UInt8) }, { 0, 1, 0, 0 } };

static int divide_bbxb_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    UInt8      *tin1 =  (UInt8 *) buffers[1];
    UInt8      *tout0 =  (UInt8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, *tin0) : *tin0 / *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor divide_bbxb_vvxv_descr =
{ "divide_bbxb_vvxv", (void *) divide_bbxb_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(UInt8) }, { 0, 0, 0, 0 } };

static void _divide_bxb_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt8  *tin0   = (UInt8 *) ((char *) input  + inboffset);
    UInt8 *tout0  = (UInt8 *) ((char *) output + outboffset);
    UInt8 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (UInt8 *) ((char *) tin0 + inbstrides[dim]);
            net = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0) : net / *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _divide_bxb_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int divide_bxb_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _divide_bxb_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(divide_bxb_R, CHECK_ALIGN, sizeof(UInt8), sizeof(UInt8));

static void _divide_bxb_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt8 *tin0   = (UInt8 *) ((char *) input  + inboffset);
    UInt8 *tout0 = (UInt8 *) ((char *) output + outboffset);
    UInt8 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (UInt8 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (UInt8 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0) : lastval / *tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _divide_bxb_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  divide_bxb_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _divide_bxb_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(divide_bxb_A, CHECK_ALIGN, sizeof(UInt8), sizeof(UInt8));

static int divide_bbxb_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8       tin0 = *(UInt8 *) buffers[0];
    UInt8      *tin1 =  (UInt8 *) buffers[1];
    UInt8      *tout0 =  (UInt8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, 0) : tin0 / *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor divide_bbxb_svxv_descr =
{ "divide_bbxb_svxv", (void *) divide_bbxb_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(UInt8) }, { 1, 0, 0, 0 } };
/*********************  divide  *********************/
/*********************  floor_divide  *********************/

static int floor_divide_bbxb_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    UInt8       tin1 = *(UInt8 *) buffers[1];
    UInt8      *tout0 =  (UInt8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = (( tin1==0) ? int_dividebyzero_error( tin1, *tin0) : floor(*tin0   / (double)  tin1));
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor floor_divide_bbxb_vsxv_descr =
{ "floor_divide_bbxb_vsxv", (void *) floor_divide_bbxb_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(UInt8) }, { 0, 1, 0, 0 } };

static int floor_divide_bbxb_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    UInt8      *tin1 =  (UInt8 *) buffers[1];
    UInt8      *tout0 =  (UInt8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, *tin0) : floor(*tin0   / (double) *tin1));
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor floor_divide_bbxb_vvxv_descr =
{ "floor_divide_bbxb_vvxv", (void *) floor_divide_bbxb_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(UInt8) }, { 0, 0, 0, 0 } };

static void _floor_divide_bxb_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt8  *tin0   = (UInt8 *) ((char *) input  + inboffset);
    UInt8 *tout0  = (UInt8 *) ((char *) output + outboffset);
    UInt8 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (UInt8 *) ((char *) tin0 + inbstrides[dim]);
            net    = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0)     : floor(net     / (double) *tin0));
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _floor_divide_bxb_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int floor_divide_bxb_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _floor_divide_bxb_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(floor_divide_bxb_R, CHECK_ALIGN, sizeof(UInt8), sizeof(UInt8));

static void _floor_divide_bxb_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt8 *tin0   = (UInt8 *) ((char *) input  + inboffset);
    UInt8 *tout0 = (UInt8 *) ((char *) output + outboffset);
    UInt8 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (UInt8 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (UInt8 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0)     : floor(lastval / (double) *tin0));
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _floor_divide_bxb_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  floor_divide_bxb_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _floor_divide_bxb_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(floor_divide_bxb_A, CHECK_ALIGN, sizeof(UInt8), sizeof(UInt8));

static int floor_divide_bbxb_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8       tin0 = *(UInt8 *) buffers[0];
    UInt8      *tin1 =  (UInt8 *) buffers[1];
    UInt8      *tout0 =  (UInt8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, 0)     : floor(tin0    / (double) *tin1));
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor floor_divide_bbxb_svxv_descr =
{ "floor_divide_bbxb_svxv", (void *) floor_divide_bbxb_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(UInt8) }, { 1, 0, 0, 0 } };
/*********************  floor_divide  *********************/
/*********************  true_divide  *********************/

static int true_divide_bbxf_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    UInt8       tin1 = *(UInt8 *) buffers[1];
    Float32    *tout0 =  (Float32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = (( tin1==0) ? int_dividebyzero_error( tin1, *tin0) : *tin0         / (double) tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor true_divide_bbxf_vsxv_descr =
{ "true_divide_bbxf_vsxv", (void *) true_divide_bbxf_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(Float32) }, { 0, 1, 0, 0 } };

static int true_divide_bbxf_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    UInt8      *tin1 =  (UInt8 *) buffers[1];
    Float32    *tout0 =  (Float32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, *tin0) : *tin0         / (double) *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor true_divide_bbxf_vvxv_descr =
{ "true_divide_bbxf_vvxv", (void *) true_divide_bbxf_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(Float32) }, { 0, 0, 0, 0 } };

static void _true_divide_bxf_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt8  *tin0   = (UInt8 *) ((char *) input  + inboffset);
    Float32 *tout0  = (Float32 *) ((char *) output + outboffset);
    Float32 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (UInt8 *) ((char *) tin0 + inbstrides[dim]);
            net    = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0)     : net           / (double) *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _true_divide_bxf_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int true_divide_bxf_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _true_divide_bxf_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(true_divide_bxf_R, CHECK_ALIGN, sizeof(UInt8), sizeof(Float32));

static void _true_divide_bxf_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt8 *tin0   = (UInt8 *) ((char *) input  + inboffset);
    Float32 *tout0 = (Float32 *) ((char *) output + outboffset);
    Float32 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (UInt8 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Float32 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0)     : lastval       / (double) *tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _true_divide_bxf_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  true_divide_bxf_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _true_divide_bxf_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(true_divide_bxf_A, CHECK_ALIGN, sizeof(UInt8), sizeof(Float32));

static int true_divide_bbxf_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8       tin0 = *(UInt8 *) buffers[0];
    UInt8      *tin1 =  (UInt8 *) buffers[1];
    Float32    *tout0 =  (Float32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, 0)     : tin0          / (double) *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor true_divide_bbxf_svxv_descr =
{ "true_divide_bbxf_svxv", (void *) true_divide_bbxf_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(Float32) }, { 1, 0, 0, 0 } };
/*********************  true_divide  *********************/
/*********************  remainder  *********************/

static int remainder_bbxb_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    UInt8       tin1 = *(UInt8 *) buffers[1];
    UInt8      *tout0 =  (UInt8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((tin1==0) ? int_dividebyzero_error(tin1, *tin0) : *tin0  %  tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor remainder_bbxb_vsxv_descr =
{ "remainder_bbxb_vsxv", (void *) remainder_bbxb_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(UInt8) }, { 0, 1, 0, 0 } };

static int remainder_bbxb_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    UInt8      *tin1 =  (UInt8 *) buffers[1];
    UInt8      *tout0 =  (UInt8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, *tin0) : *tin0  %  *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor remainder_bbxb_vvxv_descr =
{ "remainder_bbxb_vvxv", (void *) remainder_bbxb_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(UInt8) }, { 0, 0, 0, 0 } };

static void _remainder_bxb_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt8  *tin0   = (UInt8 *) ((char *) input  + inboffset);
    UInt8 *tout0  = (UInt8 *) ((char *) output + outboffset);
    UInt8 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (UInt8 *) ((char *) tin0 + inbstrides[dim]);
            net = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0) : net  %  *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _remainder_bxb_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int remainder_bxb_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _remainder_bxb_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(remainder_bxb_R, CHECK_ALIGN, sizeof(UInt8), sizeof(UInt8));

static void _remainder_bxb_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt8 *tin0   = (UInt8 *) ((char *) input  + inboffset);
    UInt8 *tout0 = (UInt8 *) ((char *) output + outboffset);
    UInt8 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (UInt8 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (UInt8 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0) : lastval  %  *tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _remainder_bxb_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  remainder_bxb_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _remainder_bxb_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(remainder_bxb_A, CHECK_ALIGN, sizeof(UInt8), sizeof(UInt8));

static int remainder_bbxb_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8       tin0 = *(UInt8 *) buffers[0];
    UInt8      *tin1 =  (UInt8 *) buffers[1];
    UInt8      *tout0 =  (UInt8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, 0) : tin0  %  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor remainder_bbxb_svxv_descr =
{ "remainder_bbxb_svxv", (void *) remainder_bbxb_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(UInt8) }, { 1, 0, 0, 0 } };
/*********************  remainder  *********************/
/*********************  power  *********************/

static int power_bbxb_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    UInt8       tin1 = *(UInt8 *) buffers[1];
    UInt8      *tout0 =  (UInt8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_pow(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor power_bbxb_vsxf_descr =
{ "power_bbxb_vsxf", (void *) power_bbxb_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(UInt8) }, { 0, 1, 0, 0 } };

static int power_bbxb_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    UInt8      *tin1 =  (UInt8 *) buffers[1];
    UInt8      *tout0 =  (UInt8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_pow(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor power_bbxb_vvxf_descr =
{ "power_bbxb_vvxf", (void *) power_bbxb_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(UInt8) }, { 0, 0, 0, 0 } };

static void _power_bxb_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt8  *tin0   = (UInt8 *) ((char *) input  + inboffset);
    UInt8 *tout0  = (UInt8 *) ((char *) output + outboffset);
    UInt8 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (UInt8 *) ((char *) tin0 + inbstrides[dim]);
            net   = num_pow(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _power_bxb_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int power_bxb_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _power_bxb_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(power_bxb_R, CHECK_ALIGN, sizeof(UInt8), sizeof(UInt8));

static void _power_bxb_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt8 *tin0   = (UInt8 *) ((char *) input  + inboffset);
    UInt8 *tout0 = (UInt8 *) ((char *) output + outboffset);
    UInt8 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (UInt8 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (UInt8 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = num_pow(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _power_bxb_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  power_bxb_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _power_bxb_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(power_bxb_A, CHECK_ALIGN, sizeof(UInt8), sizeof(UInt8));

static int power_bbxb_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8       tin0 = *(UInt8 *) buffers[0];
    UInt8      *tin1 =  (UInt8 *) buffers[1];
    UInt8      *tout0 =  (UInt8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_pow(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor power_bbxb_svxf_descr =
{ "power_bbxb_svxf", (void *) power_bbxb_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(UInt8) }, { 1, 0, 0, 0 } };
/*********************  abs  *********************/

static int abs_bxb_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    UInt8      *tout0 =  (UInt8 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = fabs(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor abs_bxb_vxf_descr =
{ "abs_bxb_vxf", (void *) abs_bxb_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt8), sizeof(UInt8) }, { 0, 0, 0 } };
/*********************  sin  *********************/

static int sin_bxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = sin(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor sin_bxd_vxf_descr =
{ "sin_bxd_vxf", (void *) sin_bxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt8), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  cos  *********************/

static int cos_bxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = cos(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor cos_bxd_vxf_descr =
{ "cos_bxd_vxf", (void *) cos_bxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt8), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  tan  *********************/

static int tan_bxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = tan(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor tan_bxd_vxf_descr =
{ "tan_bxd_vxf", (void *) tan_bxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt8), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arcsin  *********************/

static int arcsin_bxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = asin(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arcsin_bxd_vxf_descr =
{ "arcsin_bxd_vxf", (void *) arcsin_bxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt8), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arccos  *********************/

static int arccos_bxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = acos(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arccos_bxd_vxf_descr =
{ "arccos_bxd_vxf", (void *) arccos_bxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt8), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arctan  *********************/

static int arctan_bxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = atan(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arctan_bxd_vxf_descr =
{ "arctan_bxd_vxf", (void *) arctan_bxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt8), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arctan2  *********************/

static int arctan2_bbxd_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    UInt8       tin1 = *(UInt8 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = atan2(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arctan2_bbxd_vsxf_descr =
{ "arctan2_bbxd_vsxf", (void *) arctan2_bbxd_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(Float64) }, { 0, 1, 0, 0 } };

static int arctan2_bbxd_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    UInt8      *tin1 =  (UInt8 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = atan2(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arctan2_bbxd_vvxf_descr =
{ "arctan2_bbxd_vvxf", (void *) arctan2_bbxd_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(Float64) }, { 0, 0, 0, 0 } };

static void _arctan2_bxd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt8  *tin0   = (UInt8 *) ((char *) input  + inboffset);
    Float64 *tout0  = (Float64 *) ((char *) output + outboffset);
    Float64 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (UInt8 *) ((char *) tin0 + inbstrides[dim]);
            net   = atan2(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _arctan2_bxd_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int arctan2_bxd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _arctan2_bxd_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(arctan2_bxd_R, CHECK_ALIGN, sizeof(UInt8), sizeof(Float64));

static void _arctan2_bxd_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt8 *tin0   = (UInt8 *) ((char *) input  + inboffset);
    Float64 *tout0 = (Float64 *) ((char *) output + outboffset);
    Float64 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (UInt8 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Float64 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = atan2(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _arctan2_bxd_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  arctan2_bxd_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _arctan2_bxd_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(arctan2_bxd_A, CHECK_ALIGN, sizeof(UInt8), sizeof(Float64));

static int arctan2_bbxd_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8       tin0 = *(UInt8 *) buffers[0];
    UInt8      *tin1 =  (UInt8 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = atan2(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arctan2_bbxd_svxf_descr =
{ "arctan2_bbxd_svxf", (void *) arctan2_bbxd_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(Float64) }, { 1, 0, 0, 0 } };
/*********************  log  *********************/

static int log_bxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_log(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor log_bxd_vxf_descr =
{ "log_bxd_vxf", (void *) log_bxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt8), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  log10  *********************/

static int log10_bxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_log10(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor log10_bxd_vxf_descr =
{ "log10_bxd_vxf", (void *) log10_bxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt8), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  exp  *********************/

static int exp_bxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = exp(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor exp_bxd_vxf_descr =
{ "exp_bxd_vxf", (void *) exp_bxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt8), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  sinh  *********************/

static int sinh_bxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = sinh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor sinh_bxd_vxf_descr =
{ "sinh_bxd_vxf", (void *) sinh_bxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt8), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  cosh  *********************/

static int cosh_bxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = cosh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor cosh_bxd_vxf_descr =
{ "cosh_bxd_vxf", (void *) cosh_bxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt8), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  tanh  *********************/

static int tanh_bxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = tanh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor tanh_bxd_vxf_descr =
{ "tanh_bxd_vxf", (void *) tanh_bxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt8), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arcsinh  *********************/

static int arcsinh_bxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_asinh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arcsinh_bxd_vxf_descr =
{ "arcsinh_bxd_vxf", (void *) arcsinh_bxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt8), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arccosh  *********************/

static int arccosh_bxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_acosh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arccosh_bxd_vxf_descr =
{ "arccosh_bxd_vxf", (void *) arccosh_bxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt8), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arctanh  *********************/

static int arctanh_bxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_atanh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arctanh_bxd_vxf_descr =
{ "arctanh_bxd_vxf", (void *) arctanh_bxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt8), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  ieeemask  *********************/
/*********************  ieeemask  *********************/
/*********************  isnan  *********************/
/*********************  isnan  *********************/
/*********************  isnan  *********************/
/*********************  isnan  *********************/
/*********************  sqrt  *********************/

static int sqrt_bxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = sqrt(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor sqrt_bxd_vxf_descr =
{ "sqrt_bxd_vxf", (void *) sqrt_bxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt8), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  equal  *********************/

static int equal_bbxB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    UInt8       tin1 = *(UInt8 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 == tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor equal_bbxB_vsxv_descr =
{ "equal_bbxB_vsxv", (void *) equal_bbxB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int equal_bbxB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    UInt8      *tin1 =  (UInt8 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 == *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor equal_bbxB_vvxv_descr =
{ "equal_bbxB_vvxv", (void *) equal_bbxB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int equal_bbxB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8       tin0 = *(UInt8 *) buffers[0];
    UInt8      *tin1 =  (UInt8 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 == *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor equal_bbxB_svxv_descr =
{ "equal_bbxB_svxv", (void *) equal_bbxB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  not_equal  *********************/

static int not_equal_bbxB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    UInt8       tin1 = *(UInt8 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 != tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor not_equal_bbxB_vsxv_descr =
{ "not_equal_bbxB_vsxv", (void *) not_equal_bbxB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int not_equal_bbxB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    UInt8      *tin1 =  (UInt8 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 != *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor not_equal_bbxB_vvxv_descr =
{ "not_equal_bbxB_vvxv", (void *) not_equal_bbxB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int not_equal_bbxB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8       tin0 = *(UInt8 *) buffers[0];
    UInt8      *tin1 =  (UInt8 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 != *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor not_equal_bbxB_svxv_descr =
{ "not_equal_bbxB_svxv", (void *) not_equal_bbxB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  greater  *********************/

static int greater_bbxB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    UInt8       tin1 = *(UInt8 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 > tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_bbxB_vsxv_descr =
{ "greater_bbxB_vsxv", (void *) greater_bbxB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int greater_bbxB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    UInt8      *tin1 =  (UInt8 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 > *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_bbxB_vvxv_descr =
{ "greater_bbxB_vvxv", (void *) greater_bbxB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int greater_bbxB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8       tin0 = *(UInt8 *) buffers[0];
    UInt8      *tin1 =  (UInt8 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 > *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_bbxB_svxv_descr =
{ "greater_bbxB_svxv", (void *) greater_bbxB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  greater_equal  *********************/

static int greater_equal_bbxB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    UInt8       tin1 = *(UInt8 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 >= tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_equal_bbxB_vsxv_descr =
{ "greater_equal_bbxB_vsxv", (void *) greater_equal_bbxB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int greater_equal_bbxB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    UInt8      *tin1 =  (UInt8 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 >= *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_equal_bbxB_vvxv_descr =
{ "greater_equal_bbxB_vvxv", (void *) greater_equal_bbxB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int greater_equal_bbxB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8       tin0 = *(UInt8 *) buffers[0];
    UInt8      *tin1 =  (UInt8 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 >= *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_equal_bbxB_svxv_descr =
{ "greater_equal_bbxB_svxv", (void *) greater_equal_bbxB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  less  *********************/

static int less_bbxB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    UInt8       tin1 = *(UInt8 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 < tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_bbxB_vsxv_descr =
{ "less_bbxB_vsxv", (void *) less_bbxB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int less_bbxB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    UInt8      *tin1 =  (UInt8 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 < *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_bbxB_vvxv_descr =
{ "less_bbxB_vvxv", (void *) less_bbxB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int less_bbxB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8       tin0 = *(UInt8 *) buffers[0];
    UInt8      *tin1 =  (UInt8 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 < *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_bbxB_svxv_descr =
{ "less_bbxB_svxv", (void *) less_bbxB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  less_equal  *********************/

static int less_equal_bbxB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    UInt8       tin1 = *(UInt8 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 <= tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_equal_bbxB_vsxv_descr =
{ "less_equal_bbxB_vsxv", (void *) less_equal_bbxB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int less_equal_bbxB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    UInt8      *tin1 =  (UInt8 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 <= *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_equal_bbxB_vvxv_descr =
{ "less_equal_bbxB_vvxv", (void *) less_equal_bbxB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int less_equal_bbxB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8       tin0 = *(UInt8 *) buffers[0];
    UInt8      *tin1 =  (UInt8 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 <= *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_equal_bbxB_svxv_descr =
{ "less_equal_bbxB_svxv", (void *) less_equal_bbxB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  logical_and  *********************/

static int logical_and_bbxB_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    UInt8       tin1 = *(UInt8 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_and(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_and_bbxB_vsxf_descr =
{ "logical_and_bbxB_vsxf", (void *) logical_and_bbxB_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int logical_and_bbxB_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    UInt8      *tin1 =  (UInt8 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_and(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_and_bbxB_vvxf_descr =
{ "logical_and_bbxB_vvxf", (void *) logical_and_bbxB_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int logical_and_bbxB_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8       tin0 = *(UInt8 *) buffers[0];
    UInt8      *tin1 =  (UInt8 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_and(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_and_bbxB_svxf_descr =
{ "logical_and_bbxB_svxf", (void *) logical_and_bbxB_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  logical_or  *********************/

static int logical_or_bbxB_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    UInt8       tin1 = *(UInt8 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_or(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_or_bbxB_vsxf_descr =
{ "logical_or_bbxB_vsxf", (void *) logical_or_bbxB_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int logical_or_bbxB_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    UInt8      *tin1 =  (UInt8 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_or(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_or_bbxB_vvxf_descr =
{ "logical_or_bbxB_vvxf", (void *) logical_or_bbxB_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int logical_or_bbxB_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8       tin0 = *(UInt8 *) buffers[0];
    UInt8      *tin1 =  (UInt8 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_or(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_or_bbxB_svxf_descr =
{ "logical_or_bbxB_svxf", (void *) logical_or_bbxB_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  logical_xor  *********************/

static int logical_xor_bbxB_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    UInt8       tin1 = *(UInt8 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_xor(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_xor_bbxB_vsxf_descr =
{ "logical_xor_bbxB_vsxf", (void *) logical_xor_bbxB_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int logical_xor_bbxB_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    UInt8      *tin1 =  (UInt8 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_xor(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_xor_bbxB_vvxf_descr =
{ "logical_xor_bbxB_vvxf", (void *) logical_xor_bbxB_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int logical_xor_bbxB_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8       tin0 = *(UInt8 *) buffers[0];
    UInt8      *tin1 =  (UInt8 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_xor(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_xor_bbxB_svxf_descr =
{ "logical_xor_bbxB_svxf", (void *) logical_xor_bbxB_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  logical_and  *********************/
/*********************  logical_or  *********************/
/*********************  logical_xor  *********************/
/*********************  logical_not  *********************/

static int logical_not_bxB_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    Bool       *tout0 =  (Bool *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = !*tin0;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_not_bxB_vxv_descr =
{ "logical_not_bxB_vxv", (void *) logical_not_bxB_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt8), sizeof(Bool) }, { 0, 0, 0 } };
/*********************  bitwise_and  *********************/

static int bitwise_and_bbxb_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    UInt8       tin1 = *(UInt8 *) buffers[1];
    UInt8      *tout0 =  (UInt8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 & tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_and_bbxb_vsxv_descr =
{ "bitwise_and_bbxb_vsxv", (void *) bitwise_and_bbxb_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(UInt8) }, { 0, 1, 0, 0 } };

static int bitwise_and_bbxb_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    UInt8      *tin1 =  (UInt8 *) buffers[1];
    UInt8      *tout0 =  (UInt8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 & *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_and_bbxb_vvxv_descr =
{ "bitwise_and_bbxb_vvxv", (void *) bitwise_and_bbxb_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(UInt8) }, { 0, 0, 0, 0 } };

static void _bitwise_and_bxb_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt8  *tin0   = (UInt8 *) ((char *) input  + inboffset);
    UInt8 *tout0  = (UInt8 *) ((char *) output + outboffset);
    UInt8 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (UInt8 *) ((char *) tin0 + inbstrides[dim]);
            net    =     net & *tin0;
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _bitwise_and_bxb_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int bitwise_and_bxb_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _bitwise_and_bxb_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(bitwise_and_bxb_R, CHECK_ALIGN, sizeof(UInt8), sizeof(UInt8));

static void _bitwise_and_bxb_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt8 *tin0   = (UInt8 *) ((char *) input  + inboffset);
    UInt8 *tout0 = (UInt8 *) ((char *) output + outboffset);
    UInt8 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (UInt8 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (UInt8 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = lastval & *tin0;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _bitwise_and_bxb_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  bitwise_and_bxb_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _bitwise_and_bxb_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(bitwise_and_bxb_A, CHECK_ALIGN, sizeof(UInt8), sizeof(UInt8));

static int bitwise_and_bbxb_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8       tin0 = *(UInt8 *) buffers[0];
    UInt8      *tin1 =  (UInt8 *) buffers[1];
    UInt8      *tout0 =  (UInt8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 & *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_and_bbxb_svxv_descr =
{ "bitwise_and_bbxb_svxv", (void *) bitwise_and_bbxb_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(UInt8) }, { 1, 0, 0, 0 } };
/*********************  bitwise_or  *********************/

static int bitwise_or_bbxb_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    UInt8       tin1 = *(UInt8 *) buffers[1];
    UInt8      *tout0 =  (UInt8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 | tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_or_bbxb_vsxv_descr =
{ "bitwise_or_bbxb_vsxv", (void *) bitwise_or_bbxb_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(UInt8) }, { 0, 1, 0, 0 } };

static int bitwise_or_bbxb_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    UInt8      *tin1 =  (UInt8 *) buffers[1];
    UInt8      *tout0 =  (UInt8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 | *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_or_bbxb_vvxv_descr =
{ "bitwise_or_bbxb_vvxv", (void *) bitwise_or_bbxb_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(UInt8) }, { 0, 0, 0, 0 } };

static void _bitwise_or_bxb_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt8  *tin0   = (UInt8 *) ((char *) input  + inboffset);
    UInt8 *tout0  = (UInt8 *) ((char *) output + outboffset);
    UInt8 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (UInt8 *) ((char *) tin0 + inbstrides[dim]);
            net    =     net | *tin0;
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _bitwise_or_bxb_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int bitwise_or_bxb_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _bitwise_or_bxb_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(bitwise_or_bxb_R, CHECK_ALIGN, sizeof(UInt8), sizeof(UInt8));

static void _bitwise_or_bxb_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt8 *tin0   = (UInt8 *) ((char *) input  + inboffset);
    UInt8 *tout0 = (UInt8 *) ((char *) output + outboffset);
    UInt8 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (UInt8 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (UInt8 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = lastval | *tin0;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _bitwise_or_bxb_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  bitwise_or_bxb_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _bitwise_or_bxb_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(bitwise_or_bxb_A, CHECK_ALIGN, sizeof(UInt8), sizeof(UInt8));

static int bitwise_or_bbxb_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8       tin0 = *(UInt8 *) buffers[0];
    UInt8      *tin1 =  (UInt8 *) buffers[1];
    UInt8      *tout0 =  (UInt8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 | *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_or_bbxb_svxv_descr =
{ "bitwise_or_bbxb_svxv", (void *) bitwise_or_bbxb_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(UInt8) }, { 1, 0, 0, 0 } };
/*********************  bitwise_xor  *********************/

static int bitwise_xor_bbxb_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    UInt8       tin1 = *(UInt8 *) buffers[1];
    UInt8      *tout0 =  (UInt8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 ^ tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_xor_bbxb_vsxv_descr =
{ "bitwise_xor_bbxb_vsxv", (void *) bitwise_xor_bbxb_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(UInt8) }, { 0, 1, 0, 0 } };

static int bitwise_xor_bbxb_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    UInt8      *tin1 =  (UInt8 *) buffers[1];
    UInt8      *tout0 =  (UInt8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 ^ *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_xor_bbxb_vvxv_descr =
{ "bitwise_xor_bbxb_vvxv", (void *) bitwise_xor_bbxb_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(UInt8) }, { 0, 0, 0, 0 } };

static void _bitwise_xor_bxb_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt8  *tin0   = (UInt8 *) ((char *) input  + inboffset);
    UInt8 *tout0  = (UInt8 *) ((char *) output + outboffset);
    UInt8 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (UInt8 *) ((char *) tin0 + inbstrides[dim]);
            net    =     net ^ *tin0;
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _bitwise_xor_bxb_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int bitwise_xor_bxb_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _bitwise_xor_bxb_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(bitwise_xor_bxb_R, CHECK_ALIGN, sizeof(UInt8), sizeof(UInt8));

static void _bitwise_xor_bxb_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt8 *tin0   = (UInt8 *) ((char *) input  + inboffset);
    UInt8 *tout0 = (UInt8 *) ((char *) output + outboffset);
    UInt8 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (UInt8 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (UInt8 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = lastval ^ *tin0;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _bitwise_xor_bxb_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  bitwise_xor_bxb_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _bitwise_xor_bxb_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(bitwise_xor_bxb_A, CHECK_ALIGN, sizeof(UInt8), sizeof(UInt8));

static int bitwise_xor_bbxb_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8       tin0 = *(UInt8 *) buffers[0];
    UInt8      *tin1 =  (UInt8 *) buffers[1];
    UInt8      *tout0 =  (UInt8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 ^ *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_xor_bbxb_svxv_descr =
{ "bitwise_xor_bbxb_svxv", (void *) bitwise_xor_bbxb_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(UInt8) }, { 1, 0, 0, 0 } };
/*********************  bitwise_not  *********************/

static int bitwise_not_bxb_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    UInt8      *tout0 =  (UInt8 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ~*tin0;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_not_bxb_vxv_descr =
{ "bitwise_not_bxb_vxv", (void *) bitwise_not_bxb_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt8), sizeof(UInt8) }, { 0, 0, 0 } };
/*********************  bitwise_not  *********************/
/*********************  rshift  *********************/

static int rshift_bbxb_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    UInt8       tin1 = *(UInt8 *) buffers[1];
    UInt8      *tout0 =  (UInt8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 >> tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor rshift_bbxb_vsxv_descr =
{ "rshift_bbxb_vsxv", (void *) rshift_bbxb_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(UInt8) }, { 0, 1, 0, 0 } };

static int rshift_bbxb_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    UInt8      *tin1 =  (UInt8 *) buffers[1];
    UInt8      *tout0 =  (UInt8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 >> *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor rshift_bbxb_vvxv_descr =
{ "rshift_bbxb_vvxv", (void *) rshift_bbxb_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(UInt8) }, { 0, 0, 0, 0 } };

static int rshift_bbxb_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8       tin0 = *(UInt8 *) buffers[0];
    UInt8      *tin1 =  (UInt8 *) buffers[1];
    UInt8      *tout0 =  (UInt8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 >> *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor rshift_bbxb_svxv_descr =
{ "rshift_bbxb_svxv", (void *) rshift_bbxb_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(UInt8) }, { 1, 0, 0, 0 } };
/*********************  lshift  *********************/

static int lshift_bbxb_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    UInt8       tin1 = *(UInt8 *) buffers[1];
    UInt8      *tout0 =  (UInt8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 << tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor lshift_bbxb_vsxv_descr =
{ "lshift_bbxb_vsxv", (void *) lshift_bbxb_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(UInt8) }, { 0, 1, 0, 0 } };

static int lshift_bbxb_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    UInt8      *tin1 =  (UInt8 *) buffers[1];
    UInt8      *tout0 =  (UInt8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 << *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor lshift_bbxb_vvxv_descr =
{ "lshift_bbxb_vvxv", (void *) lshift_bbxb_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(UInt8) }, { 0, 0, 0, 0 } };

static int lshift_bbxb_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8       tin0 = *(UInt8 *) buffers[0];
    UInt8      *tin1 =  (UInt8 *) buffers[1];
    UInt8      *tout0 =  (UInt8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 << *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor lshift_bbxb_svxv_descr =
{ "lshift_bbxb_svxv", (void *) lshift_bbxb_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(UInt8) }, { 1, 0, 0, 0 } };
/*********************  floor  *********************/

static int floor_bxb_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    UInt8      *tout0 =  (UInt8 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = (*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor floor_bxb_vxf_descr =
{ "floor_bxb_vxf", (void *) floor_bxb_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt8), sizeof(UInt8) }, { 0, 0, 0 } };
/*********************  floor  *********************/
/*********************  ceil  *********************/

static int ceil_bxb_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    UInt8      *tout0 =  (UInt8 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = (*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor ceil_bxb_vxf_descr =
{ "ceil_bxb_vxf", (void *) ceil_bxb_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt8), sizeof(UInt8) }, { 0, 0, 0 } };
/*********************  ceil  *********************/
/*********************  maximum  *********************/

static int maximum_bbxb_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    UInt8       tin1 = *(UInt8 *) buffers[1];
    UInt8      *tout0 =  (UInt8 *) buffers[2];
UInt8 temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufmaximum(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor maximum_bbxb_vsxf_descr =
{ "maximum_bbxb_vsxf", (void *) maximum_bbxb_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(UInt8) }, { 0, 1, 0, 0 } };

static int maximum_bbxb_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    UInt8      *tin1 =  (UInt8 *) buffers[1];
    UInt8      *tout0 =  (UInt8 *) buffers[2];
UInt8 temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufmaximum(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor maximum_bbxb_vvxf_descr =
{ "maximum_bbxb_vvxf", (void *) maximum_bbxb_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(UInt8) }, { 0, 0, 0, 0 } };

static void _maximum_bxb_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt8  *tin0   = (UInt8 *) ((char *) input  + inboffset);
    UInt8 *tout0  = (UInt8 *) ((char *) output + outboffset);
    UInt8 net;
    UInt8 temp1, temp2;
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (UInt8 *) ((char *) tin0 + inbstrides[dim]);
            net   = ufmaximum(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _maximum_bxb_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int maximum_bxb_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _maximum_bxb_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(maximum_bxb_R, CHECK_ALIGN, sizeof(UInt8), sizeof(UInt8));

static void _maximum_bxb_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt8 *tin0   = (UInt8 *) ((char *) input  + inboffset);
    UInt8 *tout0 = (UInt8 *) ((char *) output + outboffset);
    UInt8 lastval;
    UInt8 temp1, temp2;
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (UInt8 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (UInt8 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = ufmaximum(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _maximum_bxb_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  maximum_bxb_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _maximum_bxb_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(maximum_bxb_A, CHECK_ALIGN, sizeof(UInt8), sizeof(UInt8));

static int maximum_bbxb_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8       tin0 = *(UInt8 *) buffers[0];
    UInt8      *tin1 =  (UInt8 *) buffers[1];
    UInt8      *tout0 =  (UInt8 *) buffers[2];
UInt8 temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufmaximum(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor maximum_bbxb_svxf_descr =
{ "maximum_bbxb_svxf", (void *) maximum_bbxb_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(UInt8) }, { 1, 0, 0, 0 } };
/*********************  minimum  *********************/

static int minimum_bbxb_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    UInt8       tin1 = *(UInt8 *) buffers[1];
    UInt8      *tout0 =  (UInt8 *) buffers[2];
UInt8 temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufminimum(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor minimum_bbxb_vsxf_descr =
{ "minimum_bbxb_vsxf", (void *) minimum_bbxb_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(UInt8) }, { 0, 1, 0, 0 } };

static int minimum_bbxb_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    UInt8      *tin1 =  (UInt8 *) buffers[1];
    UInt8      *tout0 =  (UInt8 *) buffers[2];
UInt8 temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufminimum(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor minimum_bbxb_vvxf_descr =
{ "minimum_bbxb_vvxf", (void *) minimum_bbxb_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(UInt8) }, { 0, 0, 0, 0 } };

static void _minimum_bxb_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt8  *tin0   = (UInt8 *) ((char *) input  + inboffset);
    UInt8 *tout0  = (UInt8 *) ((char *) output + outboffset);
    UInt8 net;
    UInt8 temp1, temp2;
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (UInt8 *) ((char *) tin0 + inbstrides[dim]);
            net   = ufminimum(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _minimum_bxb_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int minimum_bxb_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _minimum_bxb_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(minimum_bxb_R, CHECK_ALIGN, sizeof(UInt8), sizeof(UInt8));

static void _minimum_bxb_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt8 *tin0   = (UInt8 *) ((char *) input  + inboffset);
    UInt8 *tout0 = (UInt8 *) ((char *) output + outboffset);
    UInt8 lastval;
    UInt8 temp1, temp2;
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (UInt8 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (UInt8 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = ufminimum(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _minimum_bxb_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  minimum_bxb_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _minimum_bxb_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(minimum_bxb_A, CHECK_ALIGN, sizeof(UInt8), sizeof(UInt8));

static int minimum_bbxb_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8       tin0 = *(UInt8 *) buffers[0];
    UInt8      *tin1 =  (UInt8 *) buffers[1];
    UInt8      *tout0 =  (UInt8 *) buffers[2];
UInt8 temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufminimum(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor minimum_bbxb_svxf_descr =
{ "minimum_bbxb_svxf", (void *) minimum_bbxb_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(UInt8) }, { 1, 0, 0, 0 } };
/*********************  fabs  *********************/

static int fabs_bxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = fabs(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor fabs_bxd_vxf_descr =
{ "fabs_bxd_vxf", (void *) fabs_bxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt8), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  _round  *********************/

static int _round_bxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_round(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor _round_bxd_vxf_descr =
{ "_round_bxd_vxf", (void *) _round_bxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt8), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  hypot  *********************/

static int hypot_bbxd_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    UInt8       tin1 = *(UInt8 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = hypot(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor hypot_bbxd_vsxf_descr =
{ "hypot_bbxd_vsxf", (void *) hypot_bbxd_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(Float64) }, { 0, 1, 0, 0 } };

static int hypot_bbxd_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8      *tin0 =  (UInt8 *) buffers[0];
    UInt8      *tin1 =  (UInt8 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = hypot(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor hypot_bbxd_vvxf_descr =
{ "hypot_bbxd_vvxf", (void *) hypot_bbxd_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(Float64) }, { 0, 0, 0, 0 } };

static void _hypot_bxd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt8  *tin0   = (UInt8 *) ((char *) input  + inboffset);
    Float64 *tout0  = (Float64 *) ((char *) output + outboffset);
    Float64 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (UInt8 *) ((char *) tin0 + inbstrides[dim]);
            net   = hypot(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _hypot_bxd_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int hypot_bxd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _hypot_bxd_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(hypot_bxd_R, CHECK_ALIGN, sizeof(UInt8), sizeof(Float64));

static void _hypot_bxd_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    UInt8 *tin0   = (UInt8 *) ((char *) input  + inboffset);
    Float64 *tout0 = (Float64 *) ((char *) output + outboffset);
    Float64 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (UInt8 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Float64 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = hypot(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _hypot_bxd_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  hypot_bxd_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _hypot_bxd_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(hypot_bxd_A, CHECK_ALIGN, sizeof(UInt8), sizeof(Float64));

static int hypot_bbxd_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt8       tin0 = *(UInt8 *) buffers[0];
    UInt8      *tin1 =  (UInt8 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = hypot(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor hypot_bbxd_svxf_descr =
{ "hypot_bbxd_svxf", (void *) hypot_bbxd_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(UInt8), sizeof(UInt8), sizeof(Float64) }, { 1, 0, 0, 0 } };
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

static PyMethodDef _ufuncUInt8Methods[] = {

	{NULL,      NULL}        /* Sentinel */
};

static PyObject *init_funcDict(void) {
    PyObject *dict, *keytuple, *cfunc;
    dict = PyDict_New();
    /* minus_bxb_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","minus","v","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minus_bxb_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_bbxb_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","add","vs","UInt8","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_bbxb_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_bbxb_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","add","vv","UInt8","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_bbxb_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_bxb_R */
    keytuple=Py_BuildValue("ss((s)(s))","add","R","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_bxb_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_bxb_A */
    keytuple=Py_BuildValue("ss((s)(s))","add","A","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_bxb_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_bbxb_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","add","sv","UInt8","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_bbxb_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_bbxb_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","subtract","vs","UInt8","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_bbxb_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_bbxb_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","subtract","vv","UInt8","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_bbxb_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_bxb_R */
    keytuple=Py_BuildValue("ss((s)(s))","subtract","R","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_bxb_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_bxb_A */
    keytuple=Py_BuildValue("ss((s)(s))","subtract","A","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_bxb_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_bbxb_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","subtract","sv","UInt8","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_bbxb_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* multiply_bbxb_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","multiply","vs","UInt8","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&multiply_bbxb_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* multiply_bbxb_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","multiply","vv","UInt8","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&multiply_bbxb_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* multiply_bxb_R */
    keytuple=Py_BuildValue("ss((s)(s))","multiply","R","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&multiply_bxb_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* multiply_bxb_A */
    keytuple=Py_BuildValue("ss((s)(s))","multiply","A","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&multiply_bxb_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* multiply_bbxb_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","multiply","sv","UInt8","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&multiply_bbxb_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_bbxb_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","divide","vs","UInt8","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_bbxb_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_bbxb_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","divide","vv","UInt8","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_bbxb_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_bxb_R */
    keytuple=Py_BuildValue("ss((s)(s))","divide","R","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_bxb_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_bxb_A */
    keytuple=Py_BuildValue("ss((s)(s))","divide","A","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_bxb_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_bbxb_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","divide","sv","UInt8","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_bbxb_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_divide_bbxb_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","floor_divide","vs","UInt8","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_divide_bbxb_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_divide_bbxb_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","floor_divide","vv","UInt8","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_divide_bbxb_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_divide_bxb_R */
    keytuple=Py_BuildValue("ss((s)(s))","floor_divide","R","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_divide_bxb_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_divide_bxb_A */
    keytuple=Py_BuildValue("ss((s)(s))","floor_divide","A","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_divide_bxb_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_divide_bbxb_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","floor_divide","sv","UInt8","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_divide_bbxb_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_bbxf_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","true_divide","vs","UInt8","UInt8","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_bbxf_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_bbxf_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","true_divide","vv","UInt8","UInt8","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_bbxf_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_bxf_R */
    keytuple=Py_BuildValue("ss((s)(s))","true_divide","R","UInt8","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_bxf_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_bxf_A */
    keytuple=Py_BuildValue("ss((s)(s))","true_divide","A","UInt8","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_bxf_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_bbxf_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","true_divide","sv","UInt8","UInt8","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_bbxf_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_bbxb_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","remainder","vs","UInt8","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_bbxb_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_bbxb_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","remainder","vv","UInt8","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_bbxb_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_bxb_R */
    keytuple=Py_BuildValue("ss((s)(s))","remainder","R","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_bxb_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_bxb_A */
    keytuple=Py_BuildValue("ss((s)(s))","remainder","A","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_bxb_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_bbxb_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","remainder","sv","UInt8","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_bbxb_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_bbxb_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","power","vs","UInt8","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_bbxb_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_bbxb_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","power","vv","UInt8","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_bbxb_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_bxb_R */
    keytuple=Py_BuildValue("ss((s)(s))","power","R","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_bxb_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_bxb_A */
    keytuple=Py_BuildValue("ss((s)(s))","power","A","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_bxb_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_bbxb_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","power","sv","UInt8","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_bbxb_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* abs_bxb_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","abs","v","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&abs_bxb_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* sin_bxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","sin","v","UInt8","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&sin_bxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* cos_bxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","cos","v","UInt8","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&cos_bxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* tan_bxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","tan","v","UInt8","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&tan_bxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arcsin_bxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arcsin","v","UInt8","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arcsin_bxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arccos_bxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arccos","v","UInt8","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arccos_bxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan_bxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arctan","v","UInt8","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan_bxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan2_bbxd_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","arctan2","vs","UInt8","UInt8","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan2_bbxd_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan2_bbxd_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","arctan2","vv","UInt8","UInt8","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan2_bbxd_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan2_bxd_R */
    keytuple=Py_BuildValue("ss((s)(s))","arctan2","R","UInt8","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan2_bxd_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan2_bxd_A */
    keytuple=Py_BuildValue("ss((s)(s))","arctan2","A","UInt8","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan2_bxd_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan2_bbxd_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","arctan2","sv","UInt8","UInt8","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan2_bbxd_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* log_bxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","log","v","UInt8","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&log_bxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* log10_bxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","log10","v","UInt8","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&log10_bxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* exp_bxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","exp","v","UInt8","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&exp_bxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* sinh_bxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","sinh","v","UInt8","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&sinh_bxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* cosh_bxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","cosh","v","UInt8","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&cosh_bxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* tanh_bxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","tanh","v","UInt8","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&tanh_bxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arcsinh_bxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arcsinh","v","UInt8","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arcsinh_bxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arccosh_bxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arccosh","v","UInt8","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arccosh_bxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctanh_bxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arctanh","v","UInt8","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctanh_bxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* sqrt_bxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","sqrt","v","UInt8","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&sqrt_bxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* equal_bbxB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","equal","vs","UInt8","UInt8","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&equal_bbxB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* equal_bbxB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","equal","vv","UInt8","UInt8","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&equal_bbxB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* equal_bbxB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","equal","sv","UInt8","UInt8","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&equal_bbxB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* not_equal_bbxB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","not_equal","vs","UInt8","UInt8","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&not_equal_bbxB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* not_equal_bbxB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","not_equal","vv","UInt8","UInt8","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&not_equal_bbxB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* not_equal_bbxB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","not_equal","sv","UInt8","UInt8","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&not_equal_bbxB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_bbxB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater","vs","UInt8","UInt8","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_bbxB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_bbxB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater","vv","UInt8","UInt8","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_bbxB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_bbxB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater","sv","UInt8","UInt8","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_bbxB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_equal_bbxB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater_equal","vs","UInt8","UInt8","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_equal_bbxB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_equal_bbxB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater_equal","vv","UInt8","UInt8","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_equal_bbxB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_equal_bbxB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater_equal","sv","UInt8","UInt8","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_equal_bbxB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_bbxB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less","vs","UInt8","UInt8","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_bbxB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_bbxB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less","vv","UInt8","UInt8","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_bbxB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_bbxB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less","sv","UInt8","UInt8","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_bbxB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_equal_bbxB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less_equal","vs","UInt8","UInt8","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_equal_bbxB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_equal_bbxB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less_equal","vv","UInt8","UInt8","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_equal_bbxB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_equal_bbxB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less_equal","sv","UInt8","UInt8","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_equal_bbxB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_and_bbxB_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_and","vs","UInt8","UInt8","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_and_bbxB_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_and_bbxB_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_and","vv","UInt8","UInt8","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_and_bbxB_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_and_bbxB_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_and","sv","UInt8","UInt8","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_and_bbxB_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_or_bbxB_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_or","vs","UInt8","UInt8","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_or_bbxB_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_or_bbxB_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_or","vv","UInt8","UInt8","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_or_bbxB_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_or_bbxB_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_or","sv","UInt8","UInt8","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_or_bbxB_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_xor_bbxB_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_xor","vs","UInt8","UInt8","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_xor_bbxB_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_xor_bbxB_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_xor","vv","UInt8","UInt8","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_xor_bbxB_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_xor_bbxB_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_xor","sv","UInt8","UInt8","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_xor_bbxB_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_not_bxB_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","logical_not","v","UInt8","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_not_bxB_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_and_bbxb_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_and","vs","UInt8","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_and_bbxb_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_and_bbxb_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_and","vv","UInt8","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_and_bbxb_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_and_bxb_R */
    keytuple=Py_BuildValue("ss((s)(s))","bitwise_and","R","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_and_bxb_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_and_bxb_A */
    keytuple=Py_BuildValue("ss((s)(s))","bitwise_and","A","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_and_bxb_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_and_bbxb_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_and","sv","UInt8","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_and_bbxb_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_or_bbxb_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_or","vs","UInt8","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_or_bbxb_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_or_bbxb_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_or","vv","UInt8","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_or_bbxb_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_or_bxb_R */
    keytuple=Py_BuildValue("ss((s)(s))","bitwise_or","R","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_or_bxb_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_or_bxb_A */
    keytuple=Py_BuildValue("ss((s)(s))","bitwise_or","A","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_or_bxb_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_or_bbxb_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_or","sv","UInt8","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_or_bbxb_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_xor_bbxb_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_xor","vs","UInt8","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_xor_bbxb_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_xor_bbxb_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_xor","vv","UInt8","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_xor_bbxb_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_xor_bxb_R */
    keytuple=Py_BuildValue("ss((s)(s))","bitwise_xor","R","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_xor_bxb_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_xor_bxb_A */
    keytuple=Py_BuildValue("ss((s)(s))","bitwise_xor","A","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_xor_bxb_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_xor_bbxb_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_xor","sv","UInt8","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_xor_bbxb_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_not_bxb_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","bitwise_not","v","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_not_bxb_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* rshift_bbxb_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","rshift","vs","UInt8","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&rshift_bbxb_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* rshift_bbxb_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","rshift","vv","UInt8","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&rshift_bbxb_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* rshift_bbxb_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","rshift","sv","UInt8","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&rshift_bbxb_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* lshift_bbxb_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","lshift","vs","UInt8","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&lshift_bbxb_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* lshift_bbxb_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","lshift","vv","UInt8","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&lshift_bbxb_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* lshift_bbxb_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","lshift","sv","UInt8","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&lshift_bbxb_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_bxb_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","floor","v","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_bxb_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* ceil_bxb_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","ceil","v","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&ceil_bxb_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_bbxb_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","maximum","vs","UInt8","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_bbxb_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_bbxb_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","maximum","vv","UInt8","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_bbxb_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_bxb_R */
    keytuple=Py_BuildValue("ss((s)(s))","maximum","R","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_bxb_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_bxb_A */
    keytuple=Py_BuildValue("ss((s)(s))","maximum","A","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_bxb_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_bbxb_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","maximum","sv","UInt8","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_bbxb_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_bbxb_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","minimum","vs","UInt8","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_bbxb_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_bbxb_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","minimum","vv","UInt8","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_bbxb_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_bxb_R */
    keytuple=Py_BuildValue("ss((s)(s))","minimum","R","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_bxb_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_bxb_A */
    keytuple=Py_BuildValue("ss((s)(s))","minimum","A","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_bxb_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_bbxb_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","minimum","sv","UInt8","UInt8","UInt8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_bbxb_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* fabs_bxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","fabs","v","UInt8","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&fabs_bxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* _round_bxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","_round","v","UInt8","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&_round_bxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_bbxd_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","hypot","vs","UInt8","UInt8","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_bbxd_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_bbxd_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","hypot","vv","UInt8","UInt8","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_bbxd_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_bxd_R */
    keytuple=Py_BuildValue("ss((s)(s))","hypot","R","UInt8","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_bxd_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_bxd_A */
    keytuple=Py_BuildValue("ss((s)(s))","hypot","A","UInt8","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_bxd_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_bbxd_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","hypot","sv","UInt8","UInt8","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_bbxd_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    return dict;
}

/* platform independent*/
#ifdef MS_WIN32
__declspec(dllexport)
#endif
void init_ufuncUInt8(void) {
    PyObject *m, *d, *functions;
    m = Py_InitModule("_ufuncUInt8", _ufuncUInt8Methods);
    d = PyModule_GetDict(m);
    import_libnumarray();
    functions = init_funcDict();
    PyDict_SetItemString(d, "functionDict", functions);
    Py_DECREF(functions);
    ADD_VERSION(m);
}
