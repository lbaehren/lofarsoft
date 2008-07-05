
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

static int minus_sxs_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Int16      *tout0 =  (Int16 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = -*tin0;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor minus_sxs_vxv_descr =
{ "minus_sxs_vxv", (void *) minus_sxs_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int16), sizeof(Int16) }, { 0, 0, 0 } };
/*********************  add  *********************/

static int add_ssxs_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Int16       tin1 = *(Int16 *) buffers[1];
    Int16      *tout0 =  (Int16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 + tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor add_ssxs_vsxv_descr =
{ "add_ssxs_vsxv", (void *) add_ssxs_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Int16) }, { 0, 1, 0, 0 } };

static int add_ssxs_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Int16      *tin1 =  (Int16 *) buffers[1];
    Int16      *tout0 =  (Int16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 + *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor add_ssxs_vvxv_descr =
{ "add_ssxs_vvxv", (void *) add_ssxs_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Int16) }, { 0, 0, 0, 0 } };

static void _add_sxs_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int16  *tin0   = (Int16 *) ((char *) input  + inboffset);
    Int16 *tout0  = (Int16 *) ((char *) output + outboffset);
    Int16 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Int16 *) ((char *) tin0 + inbstrides[dim]);
            net    =     net + *tin0;
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _add_sxs_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int add_sxs_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _add_sxs_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(add_sxs_R, CHECK_ALIGN, sizeof(Int16), sizeof(Int16));

static void _add_sxs_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int16 *tin0   = (Int16 *) ((char *) input  + inboffset);
    Int16 *tout0 = (Int16 *) ((char *) output + outboffset);
    Int16 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Int16 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Int16 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = lastval + *tin0;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _add_sxs_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  add_sxs_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _add_sxs_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(add_sxs_A, CHECK_ALIGN, sizeof(Int16), sizeof(Int16));

static int add_ssxs_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16       tin0 = *(Int16 *) buffers[0];
    Int16      *tin1 =  (Int16 *) buffers[1];
    Int16      *tout0 =  (Int16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 + *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor add_ssxs_svxv_descr =
{ "add_ssxs_svxv", (void *) add_ssxs_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Int16) }, { 1, 0, 0, 0 } };
/*********************  subtract  *********************/

static int subtract_ssxs_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Int16       tin1 = *(Int16 *) buffers[1];
    Int16      *tout0 =  (Int16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 - tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor subtract_ssxs_vsxv_descr =
{ "subtract_ssxs_vsxv", (void *) subtract_ssxs_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Int16) }, { 0, 1, 0, 0 } };

static int subtract_ssxs_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Int16      *tin1 =  (Int16 *) buffers[1];
    Int16      *tout0 =  (Int16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 - *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor subtract_ssxs_vvxv_descr =
{ "subtract_ssxs_vvxv", (void *) subtract_ssxs_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Int16) }, { 0, 0, 0, 0 } };

static void _subtract_sxs_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int16  *tin0   = (Int16 *) ((char *) input  + inboffset);
    Int16 *tout0  = (Int16 *) ((char *) output + outboffset);
    Int16 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Int16 *) ((char *) tin0 + inbstrides[dim]);
            net    =     net - *tin0;
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _subtract_sxs_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int subtract_sxs_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _subtract_sxs_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(subtract_sxs_R, CHECK_ALIGN, sizeof(Int16), sizeof(Int16));

static void _subtract_sxs_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int16 *tin0   = (Int16 *) ((char *) input  + inboffset);
    Int16 *tout0 = (Int16 *) ((char *) output + outboffset);
    Int16 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Int16 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Int16 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = lastval - *tin0;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _subtract_sxs_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  subtract_sxs_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _subtract_sxs_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(subtract_sxs_A, CHECK_ALIGN, sizeof(Int16), sizeof(Int16));

static int subtract_ssxs_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16       tin0 = *(Int16 *) buffers[0];
    Int16      *tin1 =  (Int16 *) buffers[1];
    Int16      *tout0 =  (Int16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 - *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor subtract_ssxs_svxv_descr =
{ "subtract_ssxs_svxv", (void *) subtract_ssxs_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Int16) }, { 1, 0, 0, 0 } };
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/

static int multiply_ssxs_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Int16       tin1 = *(Int16 *) buffers[1];
    Int16      *tout0 =  (Int16 *) buffers[2];
Int32 temp;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        temp = ((Int32) *tin0) * ((Int32) tin1);
    if (temp > 32767) temp = int_overflow_error(32767.);
if (temp < -32768) temp = int_overflow_error(-32768);
    *tout0 = (Int16) temp;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor multiply_ssxs_vsxv_descr =
{ "multiply_ssxs_vsxv", (void *) multiply_ssxs_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Int16) }, { 0, 1, 0, 0 } };

static int multiply_ssxs_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Int16      *tin1 =  (Int16 *) buffers[1];
    Int16      *tout0 =  (Int16 *) buffers[2];
Int32 temp;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        temp = ((Int32) *tin0) * ((Int32) *tin1);
    if (temp > 32767) temp = int_overflow_error(32767.);
if (temp < -32768) temp = int_overflow_error(-32768);
    *tout0 = (Int16) temp;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor multiply_ssxs_vvxv_descr =
{ "multiply_ssxs_vvxv", (void *) multiply_ssxs_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Int16) }, { 0, 0, 0, 0 } };

static void _multiply_sxs_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int16  *tin0   = (Int16 *) ((char *) input  + inboffset);
    Int16 *tout0  = (Int16 *) ((char *) output + outboffset);
    Int16 net;
    Int32 temp;
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Int16 *) ((char *) tin0 + inbstrides[dim]);
            temp = ((Int32) net) * ((Int32) *tin0);
    if (temp > 32767) temp = int_overflow_error(32767.);
if (temp < -32768) temp = int_overflow_error(-32768);
    net = (Int16) temp;
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _multiply_sxs_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int multiply_sxs_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _multiply_sxs_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(multiply_sxs_R, CHECK_ALIGN, sizeof(Int16), sizeof(Int16));

static void _multiply_sxs_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int16 *tin0   = (Int16 *) ((char *) input  + inboffset);
    Int16 *tout0 = (Int16 *) ((char *) output + outboffset);
    Int16 lastval;
    Int32 temp;
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Int16 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Int16 *) ((char *) tout0 + outbstrides[dim]);
            temp = ((Int32) lastval) * ((Int32) *tin0);
    if (temp > 32767) temp = int_overflow_error(32767.);
if (temp < -32768) temp = int_overflow_error(-32768);
    *tout0 = (Int16) temp;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _multiply_sxs_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  multiply_sxs_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _multiply_sxs_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(multiply_sxs_A, CHECK_ALIGN, sizeof(Int16), sizeof(Int16));

static int multiply_ssxs_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16       tin0 = *(Int16 *) buffers[0];
    Int16      *tin1 =  (Int16 *) buffers[1];
    Int16      *tout0 =  (Int16 *) buffers[2];
Int32 temp;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        temp = ((Int32) tin0) * ((Int32) *tin1);
    if (temp > 32767) temp = int_overflow_error(32767.);
if (temp < -32768) temp = int_overflow_error(-32768);
    *tout0 = (Int16) temp;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor multiply_ssxs_svxv_descr =
{ "multiply_ssxs_svxv", (void *) multiply_ssxs_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Int16) }, { 1, 0, 0, 0 } };
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  divide  *********************/

static int divide_ssxs_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Int16       tin1 = *(Int16 *) buffers[1];
    Int16      *tout0 =  (Int16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((tin1==0) ? int_dividebyzero_error(tin1, *tin0) : *tin0 / tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor divide_ssxs_vsxv_descr =
{ "divide_ssxs_vsxv", (void *) divide_ssxs_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Int16) }, { 0, 1, 0, 0 } };

static int divide_ssxs_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Int16      *tin1 =  (Int16 *) buffers[1];
    Int16      *tout0 =  (Int16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, *tin0) : *tin0 / *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor divide_ssxs_vvxv_descr =
{ "divide_ssxs_vvxv", (void *) divide_ssxs_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Int16) }, { 0, 0, 0, 0 } };

static void _divide_sxs_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int16  *tin0   = (Int16 *) ((char *) input  + inboffset);
    Int16 *tout0  = (Int16 *) ((char *) output + outboffset);
    Int16 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Int16 *) ((char *) tin0 + inbstrides[dim]);
            net = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0) : net / *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _divide_sxs_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int divide_sxs_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _divide_sxs_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(divide_sxs_R, CHECK_ALIGN, sizeof(Int16), sizeof(Int16));

static void _divide_sxs_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int16 *tin0   = (Int16 *) ((char *) input  + inboffset);
    Int16 *tout0 = (Int16 *) ((char *) output + outboffset);
    Int16 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Int16 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Int16 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0) : lastval / *tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _divide_sxs_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  divide_sxs_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _divide_sxs_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(divide_sxs_A, CHECK_ALIGN, sizeof(Int16), sizeof(Int16));

static int divide_ssxs_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16       tin0 = *(Int16 *) buffers[0];
    Int16      *tin1 =  (Int16 *) buffers[1];
    Int16      *tout0 =  (Int16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, 0) : tin0 / *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor divide_ssxs_svxv_descr =
{ "divide_ssxs_svxv", (void *) divide_ssxs_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Int16) }, { 1, 0, 0, 0 } };
/*********************  divide  *********************/
/*********************  floor_divide  *********************/

static int floor_divide_ssxs_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Int16       tin1 = *(Int16 *) buffers[1];
    Int16      *tout0 =  (Int16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = (( tin1==0) ? int_dividebyzero_error( tin1, *tin0) : floor(*tin0   / (double)  tin1));
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor floor_divide_ssxs_vsxv_descr =
{ "floor_divide_ssxs_vsxv", (void *) floor_divide_ssxs_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Int16) }, { 0, 1, 0, 0 } };

static int floor_divide_ssxs_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Int16      *tin1 =  (Int16 *) buffers[1];
    Int16      *tout0 =  (Int16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, *tin0) : floor(*tin0   / (double) *tin1));
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor floor_divide_ssxs_vvxv_descr =
{ "floor_divide_ssxs_vvxv", (void *) floor_divide_ssxs_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Int16) }, { 0, 0, 0, 0 } };

static void _floor_divide_sxs_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int16  *tin0   = (Int16 *) ((char *) input  + inboffset);
    Int16 *tout0  = (Int16 *) ((char *) output + outboffset);
    Int16 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Int16 *) ((char *) tin0 + inbstrides[dim]);
            net    = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0)     : floor(net     / (double) *tin0));
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _floor_divide_sxs_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int floor_divide_sxs_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _floor_divide_sxs_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(floor_divide_sxs_R, CHECK_ALIGN, sizeof(Int16), sizeof(Int16));

static void _floor_divide_sxs_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int16 *tin0   = (Int16 *) ((char *) input  + inboffset);
    Int16 *tout0 = (Int16 *) ((char *) output + outboffset);
    Int16 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Int16 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Int16 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0)     : floor(lastval / (double) *tin0));
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _floor_divide_sxs_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  floor_divide_sxs_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _floor_divide_sxs_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(floor_divide_sxs_A, CHECK_ALIGN, sizeof(Int16), sizeof(Int16));

static int floor_divide_ssxs_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16       tin0 = *(Int16 *) buffers[0];
    Int16      *tin1 =  (Int16 *) buffers[1];
    Int16      *tout0 =  (Int16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, 0)     : floor(tin0    / (double) *tin1));
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor floor_divide_ssxs_svxv_descr =
{ "floor_divide_ssxs_svxv", (void *) floor_divide_ssxs_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Int16) }, { 1, 0, 0, 0 } };
/*********************  floor_divide  *********************/
/*********************  true_divide  *********************/

static int true_divide_ssxf_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Int16       tin1 = *(Int16 *) buffers[1];
    Float32    *tout0 =  (Float32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = (( tin1==0) ? int_dividebyzero_error( tin1, *tin0) : *tin0         / (double) tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor true_divide_ssxf_vsxv_descr =
{ "true_divide_ssxf_vsxv", (void *) true_divide_ssxf_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Float32) }, { 0, 1, 0, 0 } };

static int true_divide_ssxf_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Int16      *tin1 =  (Int16 *) buffers[1];
    Float32    *tout0 =  (Float32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, *tin0) : *tin0         / (double) *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor true_divide_ssxf_vvxv_descr =
{ "true_divide_ssxf_vvxv", (void *) true_divide_ssxf_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Float32) }, { 0, 0, 0, 0 } };

static void _true_divide_sxf_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int16  *tin0   = (Int16 *) ((char *) input  + inboffset);
    Float32 *tout0  = (Float32 *) ((char *) output + outboffset);
    Float32 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Int16 *) ((char *) tin0 + inbstrides[dim]);
            net    = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0)     : net           / (double) *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _true_divide_sxf_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int true_divide_sxf_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _true_divide_sxf_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(true_divide_sxf_R, CHECK_ALIGN, sizeof(Int16), sizeof(Float32));

static void _true_divide_sxf_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int16 *tin0   = (Int16 *) ((char *) input  + inboffset);
    Float32 *tout0 = (Float32 *) ((char *) output + outboffset);
    Float32 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Int16 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Float32 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0)     : lastval       / (double) *tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _true_divide_sxf_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  true_divide_sxf_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _true_divide_sxf_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(true_divide_sxf_A, CHECK_ALIGN, sizeof(Int16), sizeof(Float32));

static int true_divide_ssxf_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16       tin0 = *(Int16 *) buffers[0];
    Int16      *tin1 =  (Int16 *) buffers[1];
    Float32    *tout0 =  (Float32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, 0)     : tin0          / (double) *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor true_divide_ssxf_svxv_descr =
{ "true_divide_ssxf_svxv", (void *) true_divide_ssxf_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Float32) }, { 1, 0, 0, 0 } };
/*********************  true_divide  *********************/
/*********************  remainder  *********************/

static int remainder_ssxs_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Int16       tin1 = *(Int16 *) buffers[1];
    Int16      *tout0 =  (Int16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((tin1==0) ? int_dividebyzero_error(tin1, *tin0) : *tin0  %  tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor remainder_ssxs_vsxv_descr =
{ "remainder_ssxs_vsxv", (void *) remainder_ssxs_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Int16) }, { 0, 1, 0, 0 } };

static int remainder_ssxs_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Int16      *tin1 =  (Int16 *) buffers[1];
    Int16      *tout0 =  (Int16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, *tin0) : *tin0  %  *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor remainder_ssxs_vvxv_descr =
{ "remainder_ssxs_vvxv", (void *) remainder_ssxs_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Int16) }, { 0, 0, 0, 0 } };

static void _remainder_sxs_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int16  *tin0   = (Int16 *) ((char *) input  + inboffset);
    Int16 *tout0  = (Int16 *) ((char *) output + outboffset);
    Int16 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Int16 *) ((char *) tin0 + inbstrides[dim]);
            net = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0) : net  %  *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _remainder_sxs_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int remainder_sxs_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _remainder_sxs_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(remainder_sxs_R, CHECK_ALIGN, sizeof(Int16), sizeof(Int16));

static void _remainder_sxs_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int16 *tin0   = (Int16 *) ((char *) input  + inboffset);
    Int16 *tout0 = (Int16 *) ((char *) output + outboffset);
    Int16 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Int16 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Int16 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0) : lastval  %  *tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _remainder_sxs_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  remainder_sxs_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _remainder_sxs_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(remainder_sxs_A, CHECK_ALIGN, sizeof(Int16), sizeof(Int16));

static int remainder_ssxs_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16       tin0 = *(Int16 *) buffers[0];
    Int16      *tin1 =  (Int16 *) buffers[1];
    Int16      *tout0 =  (Int16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, 0) : tin0  %  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor remainder_ssxs_svxv_descr =
{ "remainder_ssxs_svxv", (void *) remainder_ssxs_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Int16) }, { 1, 0, 0, 0 } };
/*********************  remainder  *********************/
/*********************  power  *********************/

static int power_ssxs_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Int16       tin1 = *(Int16 *) buffers[1];
    Int16      *tout0 =  (Int16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_pow(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor power_ssxs_vsxf_descr =
{ "power_ssxs_vsxf", (void *) power_ssxs_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Int16) }, { 0, 1, 0, 0 } };

static int power_ssxs_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Int16      *tin1 =  (Int16 *) buffers[1];
    Int16      *tout0 =  (Int16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_pow(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor power_ssxs_vvxf_descr =
{ "power_ssxs_vvxf", (void *) power_ssxs_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Int16) }, { 0, 0, 0, 0 } };

static void _power_sxs_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int16  *tin0   = (Int16 *) ((char *) input  + inboffset);
    Int16 *tout0  = (Int16 *) ((char *) output + outboffset);
    Int16 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Int16 *) ((char *) tin0 + inbstrides[dim]);
            net   = num_pow(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _power_sxs_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int power_sxs_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _power_sxs_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(power_sxs_R, CHECK_ALIGN, sizeof(Int16), sizeof(Int16));

static void _power_sxs_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int16 *tin0   = (Int16 *) ((char *) input  + inboffset);
    Int16 *tout0 = (Int16 *) ((char *) output + outboffset);
    Int16 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Int16 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Int16 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = num_pow(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _power_sxs_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  power_sxs_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _power_sxs_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(power_sxs_A, CHECK_ALIGN, sizeof(Int16), sizeof(Int16));

static int power_ssxs_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16       tin0 = *(Int16 *) buffers[0];
    Int16      *tin1 =  (Int16 *) buffers[1];
    Int16      *tout0 =  (Int16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_pow(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor power_ssxs_svxf_descr =
{ "power_ssxs_svxf", (void *) power_ssxs_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Int16) }, { 1, 0, 0, 0 } };
/*********************  abs  *********************/

static int abs_sxs_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Int16      *tout0 =  (Int16 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = fabs(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor abs_sxs_vxf_descr =
{ "abs_sxs_vxf", (void *) abs_sxs_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int16), sizeof(Int16) }, { 0, 0, 0 } };
/*********************  sin  *********************/

static int sin_sxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = sin(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor sin_sxd_vxf_descr =
{ "sin_sxd_vxf", (void *) sin_sxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int16), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  cos  *********************/

static int cos_sxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = cos(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor cos_sxd_vxf_descr =
{ "cos_sxd_vxf", (void *) cos_sxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int16), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  tan  *********************/

static int tan_sxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = tan(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor tan_sxd_vxf_descr =
{ "tan_sxd_vxf", (void *) tan_sxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int16), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arcsin  *********************/

static int arcsin_sxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = asin(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arcsin_sxd_vxf_descr =
{ "arcsin_sxd_vxf", (void *) arcsin_sxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int16), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arccos  *********************/

static int arccos_sxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = acos(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arccos_sxd_vxf_descr =
{ "arccos_sxd_vxf", (void *) arccos_sxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int16), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arctan  *********************/

static int arctan_sxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = atan(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arctan_sxd_vxf_descr =
{ "arctan_sxd_vxf", (void *) arctan_sxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int16), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arctan2  *********************/

static int arctan2_ssxd_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Int16       tin1 = *(Int16 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = atan2(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arctan2_ssxd_vsxf_descr =
{ "arctan2_ssxd_vsxf", (void *) arctan2_ssxd_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Float64) }, { 0, 1, 0, 0 } };

static int arctan2_ssxd_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Int16      *tin1 =  (Int16 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = atan2(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arctan2_ssxd_vvxf_descr =
{ "arctan2_ssxd_vvxf", (void *) arctan2_ssxd_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Float64) }, { 0, 0, 0, 0 } };

static void _arctan2_sxd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int16  *tin0   = (Int16 *) ((char *) input  + inboffset);
    Float64 *tout0  = (Float64 *) ((char *) output + outboffset);
    Float64 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Int16 *) ((char *) tin0 + inbstrides[dim]);
            net   = atan2(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _arctan2_sxd_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int arctan2_sxd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _arctan2_sxd_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(arctan2_sxd_R, CHECK_ALIGN, sizeof(Int16), sizeof(Float64));

static void _arctan2_sxd_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int16 *tin0   = (Int16 *) ((char *) input  + inboffset);
    Float64 *tout0 = (Float64 *) ((char *) output + outboffset);
    Float64 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Int16 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Float64 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = atan2(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _arctan2_sxd_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  arctan2_sxd_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _arctan2_sxd_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(arctan2_sxd_A, CHECK_ALIGN, sizeof(Int16), sizeof(Float64));

static int arctan2_ssxd_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16       tin0 = *(Int16 *) buffers[0];
    Int16      *tin1 =  (Int16 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = atan2(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arctan2_ssxd_svxf_descr =
{ "arctan2_ssxd_svxf", (void *) arctan2_ssxd_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Float64) }, { 1, 0, 0, 0 } };
/*********************  log  *********************/

static int log_sxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_log(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor log_sxd_vxf_descr =
{ "log_sxd_vxf", (void *) log_sxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int16), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  log10  *********************/

static int log10_sxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_log10(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor log10_sxd_vxf_descr =
{ "log10_sxd_vxf", (void *) log10_sxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int16), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  exp  *********************/

static int exp_sxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = exp(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor exp_sxd_vxf_descr =
{ "exp_sxd_vxf", (void *) exp_sxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int16), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  sinh  *********************/

static int sinh_sxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = sinh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor sinh_sxd_vxf_descr =
{ "sinh_sxd_vxf", (void *) sinh_sxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int16), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  cosh  *********************/

static int cosh_sxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = cosh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor cosh_sxd_vxf_descr =
{ "cosh_sxd_vxf", (void *) cosh_sxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int16), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  tanh  *********************/

static int tanh_sxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = tanh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor tanh_sxd_vxf_descr =
{ "tanh_sxd_vxf", (void *) tanh_sxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int16), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arcsinh  *********************/

static int arcsinh_sxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_asinh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arcsinh_sxd_vxf_descr =
{ "arcsinh_sxd_vxf", (void *) arcsinh_sxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int16), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arccosh  *********************/

static int arccosh_sxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_acosh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arccosh_sxd_vxf_descr =
{ "arccosh_sxd_vxf", (void *) arccosh_sxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int16), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arctanh  *********************/

static int arctanh_sxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_atanh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arctanh_sxd_vxf_descr =
{ "arctanh_sxd_vxf", (void *) arctanh_sxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int16), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  ieeemask  *********************/
/*********************  ieeemask  *********************/
/*********************  isnan  *********************/
/*********************  isnan  *********************/
/*********************  isnan  *********************/
/*********************  isnan  *********************/
/*********************  sqrt  *********************/

static int sqrt_sxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = sqrt(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor sqrt_sxd_vxf_descr =
{ "sqrt_sxd_vxf", (void *) sqrt_sxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int16), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  equal  *********************/

static int equal_ssxB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Int16       tin1 = *(Int16 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 == tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor equal_ssxB_vsxv_descr =
{ "equal_ssxB_vsxv", (void *) equal_ssxB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int equal_ssxB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Int16      *tin1 =  (Int16 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 == *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor equal_ssxB_vvxv_descr =
{ "equal_ssxB_vvxv", (void *) equal_ssxB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int equal_ssxB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16       tin0 = *(Int16 *) buffers[0];
    Int16      *tin1 =  (Int16 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 == *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor equal_ssxB_svxv_descr =
{ "equal_ssxB_svxv", (void *) equal_ssxB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  not_equal  *********************/

static int not_equal_ssxB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Int16       tin1 = *(Int16 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 != tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor not_equal_ssxB_vsxv_descr =
{ "not_equal_ssxB_vsxv", (void *) not_equal_ssxB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int not_equal_ssxB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Int16      *tin1 =  (Int16 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 != *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor not_equal_ssxB_vvxv_descr =
{ "not_equal_ssxB_vvxv", (void *) not_equal_ssxB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int not_equal_ssxB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16       tin0 = *(Int16 *) buffers[0];
    Int16      *tin1 =  (Int16 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 != *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor not_equal_ssxB_svxv_descr =
{ "not_equal_ssxB_svxv", (void *) not_equal_ssxB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  greater  *********************/

static int greater_ssxB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Int16       tin1 = *(Int16 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 > tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_ssxB_vsxv_descr =
{ "greater_ssxB_vsxv", (void *) greater_ssxB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int greater_ssxB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Int16      *tin1 =  (Int16 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 > *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_ssxB_vvxv_descr =
{ "greater_ssxB_vvxv", (void *) greater_ssxB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int greater_ssxB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16       tin0 = *(Int16 *) buffers[0];
    Int16      *tin1 =  (Int16 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 > *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_ssxB_svxv_descr =
{ "greater_ssxB_svxv", (void *) greater_ssxB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  greater_equal  *********************/

static int greater_equal_ssxB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Int16       tin1 = *(Int16 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 >= tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_equal_ssxB_vsxv_descr =
{ "greater_equal_ssxB_vsxv", (void *) greater_equal_ssxB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int greater_equal_ssxB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Int16      *tin1 =  (Int16 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 >= *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_equal_ssxB_vvxv_descr =
{ "greater_equal_ssxB_vvxv", (void *) greater_equal_ssxB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int greater_equal_ssxB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16       tin0 = *(Int16 *) buffers[0];
    Int16      *tin1 =  (Int16 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 >= *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_equal_ssxB_svxv_descr =
{ "greater_equal_ssxB_svxv", (void *) greater_equal_ssxB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  less  *********************/

static int less_ssxB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Int16       tin1 = *(Int16 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 < tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_ssxB_vsxv_descr =
{ "less_ssxB_vsxv", (void *) less_ssxB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int less_ssxB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Int16      *tin1 =  (Int16 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 < *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_ssxB_vvxv_descr =
{ "less_ssxB_vvxv", (void *) less_ssxB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int less_ssxB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16       tin0 = *(Int16 *) buffers[0];
    Int16      *tin1 =  (Int16 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 < *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_ssxB_svxv_descr =
{ "less_ssxB_svxv", (void *) less_ssxB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  less_equal  *********************/

static int less_equal_ssxB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Int16       tin1 = *(Int16 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 <= tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_equal_ssxB_vsxv_descr =
{ "less_equal_ssxB_vsxv", (void *) less_equal_ssxB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int less_equal_ssxB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Int16      *tin1 =  (Int16 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 <= *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_equal_ssxB_vvxv_descr =
{ "less_equal_ssxB_vvxv", (void *) less_equal_ssxB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int less_equal_ssxB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16       tin0 = *(Int16 *) buffers[0];
    Int16      *tin1 =  (Int16 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 <= *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_equal_ssxB_svxv_descr =
{ "less_equal_ssxB_svxv", (void *) less_equal_ssxB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  logical_and  *********************/

static int logical_and_ssxB_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Int16       tin1 = *(Int16 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_and(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_and_ssxB_vsxf_descr =
{ "logical_and_ssxB_vsxf", (void *) logical_and_ssxB_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int logical_and_ssxB_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Int16      *tin1 =  (Int16 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_and(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_and_ssxB_vvxf_descr =
{ "logical_and_ssxB_vvxf", (void *) logical_and_ssxB_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int logical_and_ssxB_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16       tin0 = *(Int16 *) buffers[0];
    Int16      *tin1 =  (Int16 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_and(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_and_ssxB_svxf_descr =
{ "logical_and_ssxB_svxf", (void *) logical_and_ssxB_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  logical_or  *********************/

static int logical_or_ssxB_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Int16       tin1 = *(Int16 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_or(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_or_ssxB_vsxf_descr =
{ "logical_or_ssxB_vsxf", (void *) logical_or_ssxB_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int logical_or_ssxB_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Int16      *tin1 =  (Int16 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_or(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_or_ssxB_vvxf_descr =
{ "logical_or_ssxB_vvxf", (void *) logical_or_ssxB_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int logical_or_ssxB_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16       tin0 = *(Int16 *) buffers[0];
    Int16      *tin1 =  (Int16 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_or(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_or_ssxB_svxf_descr =
{ "logical_or_ssxB_svxf", (void *) logical_or_ssxB_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  logical_xor  *********************/

static int logical_xor_ssxB_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Int16       tin1 = *(Int16 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_xor(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_xor_ssxB_vsxf_descr =
{ "logical_xor_ssxB_vsxf", (void *) logical_xor_ssxB_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int logical_xor_ssxB_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Int16      *tin1 =  (Int16 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_xor(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_xor_ssxB_vvxf_descr =
{ "logical_xor_ssxB_vvxf", (void *) logical_xor_ssxB_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int logical_xor_ssxB_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16       tin0 = *(Int16 *) buffers[0];
    Int16      *tin1 =  (Int16 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_xor(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_xor_ssxB_svxf_descr =
{ "logical_xor_ssxB_svxf", (void *) logical_xor_ssxB_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  logical_and  *********************/
/*********************  logical_or  *********************/
/*********************  logical_xor  *********************/
/*********************  logical_not  *********************/

static int logical_not_sxB_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Bool       *tout0 =  (Bool *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = !*tin0;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_not_sxB_vxv_descr =
{ "logical_not_sxB_vxv", (void *) logical_not_sxB_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int16), sizeof(Bool) }, { 0, 0, 0 } };
/*********************  bitwise_and  *********************/

static int bitwise_and_ssxs_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Int16       tin1 = *(Int16 *) buffers[1];
    Int16      *tout0 =  (Int16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 & tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_and_ssxs_vsxv_descr =
{ "bitwise_and_ssxs_vsxv", (void *) bitwise_and_ssxs_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Int16) }, { 0, 1, 0, 0 } };

static int bitwise_and_ssxs_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Int16      *tin1 =  (Int16 *) buffers[1];
    Int16      *tout0 =  (Int16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 & *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_and_ssxs_vvxv_descr =
{ "bitwise_and_ssxs_vvxv", (void *) bitwise_and_ssxs_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Int16) }, { 0, 0, 0, 0 } };

static void _bitwise_and_sxs_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int16  *tin0   = (Int16 *) ((char *) input  + inboffset);
    Int16 *tout0  = (Int16 *) ((char *) output + outboffset);
    Int16 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Int16 *) ((char *) tin0 + inbstrides[dim]);
            net    =     net & *tin0;
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _bitwise_and_sxs_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int bitwise_and_sxs_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _bitwise_and_sxs_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(bitwise_and_sxs_R, CHECK_ALIGN, sizeof(Int16), sizeof(Int16));

static void _bitwise_and_sxs_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int16 *tin0   = (Int16 *) ((char *) input  + inboffset);
    Int16 *tout0 = (Int16 *) ((char *) output + outboffset);
    Int16 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Int16 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Int16 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = lastval & *tin0;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _bitwise_and_sxs_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  bitwise_and_sxs_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _bitwise_and_sxs_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(bitwise_and_sxs_A, CHECK_ALIGN, sizeof(Int16), sizeof(Int16));

static int bitwise_and_ssxs_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16       tin0 = *(Int16 *) buffers[0];
    Int16      *tin1 =  (Int16 *) buffers[1];
    Int16      *tout0 =  (Int16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 & *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_and_ssxs_svxv_descr =
{ "bitwise_and_ssxs_svxv", (void *) bitwise_and_ssxs_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Int16) }, { 1, 0, 0, 0 } };
/*********************  bitwise_or  *********************/

static int bitwise_or_ssxs_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Int16       tin1 = *(Int16 *) buffers[1];
    Int16      *tout0 =  (Int16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 | tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_or_ssxs_vsxv_descr =
{ "bitwise_or_ssxs_vsxv", (void *) bitwise_or_ssxs_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Int16) }, { 0, 1, 0, 0 } };

static int bitwise_or_ssxs_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Int16      *tin1 =  (Int16 *) buffers[1];
    Int16      *tout0 =  (Int16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 | *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_or_ssxs_vvxv_descr =
{ "bitwise_or_ssxs_vvxv", (void *) bitwise_or_ssxs_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Int16) }, { 0, 0, 0, 0 } };

static void _bitwise_or_sxs_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int16  *tin0   = (Int16 *) ((char *) input  + inboffset);
    Int16 *tout0  = (Int16 *) ((char *) output + outboffset);
    Int16 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Int16 *) ((char *) tin0 + inbstrides[dim]);
            net    =     net | *tin0;
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _bitwise_or_sxs_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int bitwise_or_sxs_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _bitwise_or_sxs_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(bitwise_or_sxs_R, CHECK_ALIGN, sizeof(Int16), sizeof(Int16));

static void _bitwise_or_sxs_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int16 *tin0   = (Int16 *) ((char *) input  + inboffset);
    Int16 *tout0 = (Int16 *) ((char *) output + outboffset);
    Int16 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Int16 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Int16 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = lastval | *tin0;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _bitwise_or_sxs_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  bitwise_or_sxs_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _bitwise_or_sxs_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(bitwise_or_sxs_A, CHECK_ALIGN, sizeof(Int16), sizeof(Int16));

static int bitwise_or_ssxs_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16       tin0 = *(Int16 *) buffers[0];
    Int16      *tin1 =  (Int16 *) buffers[1];
    Int16      *tout0 =  (Int16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 | *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_or_ssxs_svxv_descr =
{ "bitwise_or_ssxs_svxv", (void *) bitwise_or_ssxs_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Int16) }, { 1, 0, 0, 0 } };
/*********************  bitwise_xor  *********************/

static int bitwise_xor_ssxs_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Int16       tin1 = *(Int16 *) buffers[1];
    Int16      *tout0 =  (Int16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 ^ tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_xor_ssxs_vsxv_descr =
{ "bitwise_xor_ssxs_vsxv", (void *) bitwise_xor_ssxs_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Int16) }, { 0, 1, 0, 0 } };

static int bitwise_xor_ssxs_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Int16      *tin1 =  (Int16 *) buffers[1];
    Int16      *tout0 =  (Int16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 ^ *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_xor_ssxs_vvxv_descr =
{ "bitwise_xor_ssxs_vvxv", (void *) bitwise_xor_ssxs_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Int16) }, { 0, 0, 0, 0 } };

static void _bitwise_xor_sxs_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int16  *tin0   = (Int16 *) ((char *) input  + inboffset);
    Int16 *tout0  = (Int16 *) ((char *) output + outboffset);
    Int16 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Int16 *) ((char *) tin0 + inbstrides[dim]);
            net    =     net ^ *tin0;
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _bitwise_xor_sxs_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int bitwise_xor_sxs_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _bitwise_xor_sxs_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(bitwise_xor_sxs_R, CHECK_ALIGN, sizeof(Int16), sizeof(Int16));

static void _bitwise_xor_sxs_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int16 *tin0   = (Int16 *) ((char *) input  + inboffset);
    Int16 *tout0 = (Int16 *) ((char *) output + outboffset);
    Int16 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Int16 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Int16 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = lastval ^ *tin0;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _bitwise_xor_sxs_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  bitwise_xor_sxs_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _bitwise_xor_sxs_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(bitwise_xor_sxs_A, CHECK_ALIGN, sizeof(Int16), sizeof(Int16));

static int bitwise_xor_ssxs_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16       tin0 = *(Int16 *) buffers[0];
    Int16      *tin1 =  (Int16 *) buffers[1];
    Int16      *tout0 =  (Int16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 ^ *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_xor_ssxs_svxv_descr =
{ "bitwise_xor_ssxs_svxv", (void *) bitwise_xor_ssxs_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Int16) }, { 1, 0, 0, 0 } };
/*********************  bitwise_not  *********************/

static int bitwise_not_sxs_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Int16      *tout0 =  (Int16 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ~*tin0;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_not_sxs_vxv_descr =
{ "bitwise_not_sxs_vxv", (void *) bitwise_not_sxs_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int16), sizeof(Int16) }, { 0, 0, 0 } };
/*********************  bitwise_not  *********************/
/*********************  rshift  *********************/

static int rshift_ssxs_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Int16       tin1 = *(Int16 *) buffers[1];
    Int16      *tout0 =  (Int16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 >> tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor rshift_ssxs_vsxv_descr =
{ "rshift_ssxs_vsxv", (void *) rshift_ssxs_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Int16) }, { 0, 1, 0, 0 } };

static int rshift_ssxs_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Int16      *tin1 =  (Int16 *) buffers[1];
    Int16      *tout0 =  (Int16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 >> *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor rshift_ssxs_vvxv_descr =
{ "rshift_ssxs_vvxv", (void *) rshift_ssxs_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Int16) }, { 0, 0, 0, 0 } };

static int rshift_ssxs_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16       tin0 = *(Int16 *) buffers[0];
    Int16      *tin1 =  (Int16 *) buffers[1];
    Int16      *tout0 =  (Int16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 >> *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor rshift_ssxs_svxv_descr =
{ "rshift_ssxs_svxv", (void *) rshift_ssxs_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Int16) }, { 1, 0, 0, 0 } };
/*********************  lshift  *********************/

static int lshift_ssxs_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Int16       tin1 = *(Int16 *) buffers[1];
    Int16      *tout0 =  (Int16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 << tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor lshift_ssxs_vsxv_descr =
{ "lshift_ssxs_vsxv", (void *) lshift_ssxs_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Int16) }, { 0, 1, 0, 0 } };

static int lshift_ssxs_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Int16      *tin1 =  (Int16 *) buffers[1];
    Int16      *tout0 =  (Int16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 << *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor lshift_ssxs_vvxv_descr =
{ "lshift_ssxs_vvxv", (void *) lshift_ssxs_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Int16) }, { 0, 0, 0, 0 } };

static int lshift_ssxs_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16       tin0 = *(Int16 *) buffers[0];
    Int16      *tin1 =  (Int16 *) buffers[1];
    Int16      *tout0 =  (Int16 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 << *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor lshift_ssxs_svxv_descr =
{ "lshift_ssxs_svxv", (void *) lshift_ssxs_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Int16) }, { 1, 0, 0, 0 } };
/*********************  floor  *********************/

static int floor_sxs_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Int16      *tout0 =  (Int16 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = (*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor floor_sxs_vxf_descr =
{ "floor_sxs_vxf", (void *) floor_sxs_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int16), sizeof(Int16) }, { 0, 0, 0 } };
/*********************  floor  *********************/
/*********************  ceil  *********************/

static int ceil_sxs_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Int16      *tout0 =  (Int16 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = (*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor ceil_sxs_vxf_descr =
{ "ceil_sxs_vxf", (void *) ceil_sxs_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int16), sizeof(Int16) }, { 0, 0, 0 } };
/*********************  ceil  *********************/
/*********************  maximum  *********************/

static int maximum_ssxs_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Int16       tin1 = *(Int16 *) buffers[1];
    Int16      *tout0 =  (Int16 *) buffers[2];
Int16 temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufmaximum(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor maximum_ssxs_vsxf_descr =
{ "maximum_ssxs_vsxf", (void *) maximum_ssxs_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Int16) }, { 0, 1, 0, 0 } };

static int maximum_ssxs_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Int16      *tin1 =  (Int16 *) buffers[1];
    Int16      *tout0 =  (Int16 *) buffers[2];
Int16 temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufmaximum(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor maximum_ssxs_vvxf_descr =
{ "maximum_ssxs_vvxf", (void *) maximum_ssxs_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Int16) }, { 0, 0, 0, 0 } };

static void _maximum_sxs_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int16  *tin0   = (Int16 *) ((char *) input  + inboffset);
    Int16 *tout0  = (Int16 *) ((char *) output + outboffset);
    Int16 net;
    Int16 temp1, temp2;
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Int16 *) ((char *) tin0 + inbstrides[dim]);
            net   = ufmaximum(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _maximum_sxs_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int maximum_sxs_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _maximum_sxs_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(maximum_sxs_R, CHECK_ALIGN, sizeof(Int16), sizeof(Int16));

static void _maximum_sxs_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int16 *tin0   = (Int16 *) ((char *) input  + inboffset);
    Int16 *tout0 = (Int16 *) ((char *) output + outboffset);
    Int16 lastval;
    Int16 temp1, temp2;
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Int16 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Int16 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = ufmaximum(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _maximum_sxs_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  maximum_sxs_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _maximum_sxs_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(maximum_sxs_A, CHECK_ALIGN, sizeof(Int16), sizeof(Int16));

static int maximum_ssxs_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16       tin0 = *(Int16 *) buffers[0];
    Int16      *tin1 =  (Int16 *) buffers[1];
    Int16      *tout0 =  (Int16 *) buffers[2];
Int16 temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufmaximum(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor maximum_ssxs_svxf_descr =
{ "maximum_ssxs_svxf", (void *) maximum_ssxs_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Int16) }, { 1, 0, 0, 0 } };
/*********************  minimum  *********************/

static int minimum_ssxs_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Int16       tin1 = *(Int16 *) buffers[1];
    Int16      *tout0 =  (Int16 *) buffers[2];
Int16 temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufminimum(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor minimum_ssxs_vsxf_descr =
{ "minimum_ssxs_vsxf", (void *) minimum_ssxs_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Int16) }, { 0, 1, 0, 0 } };

static int minimum_ssxs_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Int16      *tin1 =  (Int16 *) buffers[1];
    Int16      *tout0 =  (Int16 *) buffers[2];
Int16 temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufminimum(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor minimum_ssxs_vvxf_descr =
{ "minimum_ssxs_vvxf", (void *) minimum_ssxs_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Int16) }, { 0, 0, 0, 0 } };

static void _minimum_sxs_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int16  *tin0   = (Int16 *) ((char *) input  + inboffset);
    Int16 *tout0  = (Int16 *) ((char *) output + outboffset);
    Int16 net;
    Int16 temp1, temp2;
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Int16 *) ((char *) tin0 + inbstrides[dim]);
            net   = ufminimum(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _minimum_sxs_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int minimum_sxs_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _minimum_sxs_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(minimum_sxs_R, CHECK_ALIGN, sizeof(Int16), sizeof(Int16));

static void _minimum_sxs_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int16 *tin0   = (Int16 *) ((char *) input  + inboffset);
    Int16 *tout0 = (Int16 *) ((char *) output + outboffset);
    Int16 lastval;
    Int16 temp1, temp2;
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Int16 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Int16 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = ufminimum(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _minimum_sxs_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  minimum_sxs_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _minimum_sxs_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(minimum_sxs_A, CHECK_ALIGN, sizeof(Int16), sizeof(Int16));

static int minimum_ssxs_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16       tin0 = *(Int16 *) buffers[0];
    Int16      *tin1 =  (Int16 *) buffers[1];
    Int16      *tout0 =  (Int16 *) buffers[2];
Int16 temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufminimum(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor minimum_ssxs_svxf_descr =
{ "minimum_ssxs_svxf", (void *) minimum_ssxs_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Int16) }, { 1, 0, 0, 0 } };
/*********************  fabs  *********************/

static int fabs_sxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = fabs(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor fabs_sxd_vxf_descr =
{ "fabs_sxd_vxf", (void *) fabs_sxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int16), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  _round  *********************/

static int _round_sxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_round(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor _round_sxd_vxf_descr =
{ "_round_sxd_vxf", (void *) _round_sxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int16), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  hypot  *********************/

static int hypot_ssxd_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Int16       tin1 = *(Int16 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = hypot(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor hypot_ssxd_vsxf_descr =
{ "hypot_ssxd_vsxf", (void *) hypot_ssxd_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Float64) }, { 0, 1, 0, 0 } };

static int hypot_ssxd_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16      *tin0 =  (Int16 *) buffers[0];
    Int16      *tin1 =  (Int16 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = hypot(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor hypot_ssxd_vvxf_descr =
{ "hypot_ssxd_vvxf", (void *) hypot_ssxd_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Float64) }, { 0, 0, 0, 0 } };

static void _hypot_sxd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int16  *tin0   = (Int16 *) ((char *) input  + inboffset);
    Float64 *tout0  = (Float64 *) ((char *) output + outboffset);
    Float64 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Int16 *) ((char *) tin0 + inbstrides[dim]);
            net   = hypot(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _hypot_sxd_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int hypot_sxd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _hypot_sxd_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(hypot_sxd_R, CHECK_ALIGN, sizeof(Int16), sizeof(Float64));

static void _hypot_sxd_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int16 *tin0   = (Int16 *) ((char *) input  + inboffset);
    Float64 *tout0 = (Float64 *) ((char *) output + outboffset);
    Float64 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Int16 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Float64 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = hypot(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _hypot_sxd_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  hypot_sxd_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _hypot_sxd_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(hypot_sxd_A, CHECK_ALIGN, sizeof(Int16), sizeof(Float64));

static int hypot_ssxd_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int16       tin0 = *(Int16 *) buffers[0];
    Int16      *tin1 =  (Int16 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = hypot(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor hypot_ssxd_svxf_descr =
{ "hypot_ssxd_svxf", (void *) hypot_ssxd_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int16), sizeof(Int16), sizeof(Float64) }, { 1, 0, 0, 0 } };
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

static PyMethodDef _ufuncInt16Methods[] = {

	{NULL,      NULL}        /* Sentinel */
};

static PyObject *init_funcDict(void) {
    PyObject *dict, *keytuple, *cfunc;
    dict = PyDict_New();
    /* minus_sxs_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","minus","v","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minus_sxs_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_ssxs_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","add","vs","Int16","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_ssxs_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_ssxs_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","add","vv","Int16","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_ssxs_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_sxs_R */
    keytuple=Py_BuildValue("ss((s)(s))","add","R","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_sxs_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_sxs_A */
    keytuple=Py_BuildValue("ss((s)(s))","add","A","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_sxs_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_ssxs_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","add","sv","Int16","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_ssxs_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_ssxs_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","subtract","vs","Int16","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_ssxs_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_ssxs_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","subtract","vv","Int16","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_ssxs_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_sxs_R */
    keytuple=Py_BuildValue("ss((s)(s))","subtract","R","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_sxs_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_sxs_A */
    keytuple=Py_BuildValue("ss((s)(s))","subtract","A","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_sxs_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_ssxs_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","subtract","sv","Int16","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_ssxs_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* multiply_ssxs_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","multiply","vs","Int16","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&multiply_ssxs_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* multiply_ssxs_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","multiply","vv","Int16","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&multiply_ssxs_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* multiply_sxs_R */
    keytuple=Py_BuildValue("ss((s)(s))","multiply","R","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&multiply_sxs_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* multiply_sxs_A */
    keytuple=Py_BuildValue("ss((s)(s))","multiply","A","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&multiply_sxs_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* multiply_ssxs_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","multiply","sv","Int16","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&multiply_ssxs_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_ssxs_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","divide","vs","Int16","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_ssxs_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_ssxs_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","divide","vv","Int16","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_ssxs_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_sxs_R */
    keytuple=Py_BuildValue("ss((s)(s))","divide","R","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_sxs_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_sxs_A */
    keytuple=Py_BuildValue("ss((s)(s))","divide","A","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_sxs_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_ssxs_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","divide","sv","Int16","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_ssxs_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_divide_ssxs_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","floor_divide","vs","Int16","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_divide_ssxs_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_divide_ssxs_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","floor_divide","vv","Int16","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_divide_ssxs_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_divide_sxs_R */
    keytuple=Py_BuildValue("ss((s)(s))","floor_divide","R","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_divide_sxs_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_divide_sxs_A */
    keytuple=Py_BuildValue("ss((s)(s))","floor_divide","A","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_divide_sxs_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_divide_ssxs_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","floor_divide","sv","Int16","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_divide_ssxs_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_ssxf_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","true_divide","vs","Int16","Int16","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_ssxf_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_ssxf_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","true_divide","vv","Int16","Int16","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_ssxf_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_sxf_R */
    keytuple=Py_BuildValue("ss((s)(s))","true_divide","R","Int16","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_sxf_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_sxf_A */
    keytuple=Py_BuildValue("ss((s)(s))","true_divide","A","Int16","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_sxf_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_ssxf_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","true_divide","sv","Int16","Int16","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_ssxf_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_ssxs_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","remainder","vs","Int16","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_ssxs_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_ssxs_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","remainder","vv","Int16","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_ssxs_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_sxs_R */
    keytuple=Py_BuildValue("ss((s)(s))","remainder","R","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_sxs_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_sxs_A */
    keytuple=Py_BuildValue("ss((s)(s))","remainder","A","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_sxs_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_ssxs_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","remainder","sv","Int16","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_ssxs_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_ssxs_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","power","vs","Int16","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_ssxs_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_ssxs_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","power","vv","Int16","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_ssxs_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_sxs_R */
    keytuple=Py_BuildValue("ss((s)(s))","power","R","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_sxs_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_sxs_A */
    keytuple=Py_BuildValue("ss((s)(s))","power","A","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_sxs_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_ssxs_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","power","sv","Int16","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_ssxs_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* abs_sxs_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","abs","v","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&abs_sxs_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* sin_sxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","sin","v","Int16","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&sin_sxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* cos_sxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","cos","v","Int16","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&cos_sxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* tan_sxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","tan","v","Int16","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&tan_sxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arcsin_sxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arcsin","v","Int16","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arcsin_sxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arccos_sxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arccos","v","Int16","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arccos_sxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan_sxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arctan","v","Int16","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan_sxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan2_ssxd_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","arctan2","vs","Int16","Int16","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan2_ssxd_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan2_ssxd_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","arctan2","vv","Int16","Int16","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan2_ssxd_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan2_sxd_R */
    keytuple=Py_BuildValue("ss((s)(s))","arctan2","R","Int16","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan2_sxd_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan2_sxd_A */
    keytuple=Py_BuildValue("ss((s)(s))","arctan2","A","Int16","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan2_sxd_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan2_ssxd_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","arctan2","sv","Int16","Int16","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan2_ssxd_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* log_sxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","log","v","Int16","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&log_sxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* log10_sxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","log10","v","Int16","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&log10_sxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* exp_sxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","exp","v","Int16","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&exp_sxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* sinh_sxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","sinh","v","Int16","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&sinh_sxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* cosh_sxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","cosh","v","Int16","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&cosh_sxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* tanh_sxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","tanh","v","Int16","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&tanh_sxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arcsinh_sxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arcsinh","v","Int16","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arcsinh_sxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arccosh_sxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arccosh","v","Int16","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arccosh_sxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctanh_sxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arctanh","v","Int16","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctanh_sxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* sqrt_sxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","sqrt","v","Int16","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&sqrt_sxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* equal_ssxB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","equal","vs","Int16","Int16","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&equal_ssxB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* equal_ssxB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","equal","vv","Int16","Int16","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&equal_ssxB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* equal_ssxB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","equal","sv","Int16","Int16","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&equal_ssxB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* not_equal_ssxB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","not_equal","vs","Int16","Int16","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&not_equal_ssxB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* not_equal_ssxB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","not_equal","vv","Int16","Int16","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&not_equal_ssxB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* not_equal_ssxB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","not_equal","sv","Int16","Int16","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&not_equal_ssxB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_ssxB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater","vs","Int16","Int16","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_ssxB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_ssxB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater","vv","Int16","Int16","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_ssxB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_ssxB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater","sv","Int16","Int16","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_ssxB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_equal_ssxB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater_equal","vs","Int16","Int16","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_equal_ssxB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_equal_ssxB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater_equal","vv","Int16","Int16","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_equal_ssxB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_equal_ssxB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater_equal","sv","Int16","Int16","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_equal_ssxB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_ssxB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less","vs","Int16","Int16","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_ssxB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_ssxB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less","vv","Int16","Int16","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_ssxB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_ssxB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less","sv","Int16","Int16","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_ssxB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_equal_ssxB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less_equal","vs","Int16","Int16","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_equal_ssxB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_equal_ssxB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less_equal","vv","Int16","Int16","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_equal_ssxB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_equal_ssxB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less_equal","sv","Int16","Int16","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_equal_ssxB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_and_ssxB_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_and","vs","Int16","Int16","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_and_ssxB_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_and_ssxB_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_and","vv","Int16","Int16","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_and_ssxB_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_and_ssxB_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_and","sv","Int16","Int16","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_and_ssxB_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_or_ssxB_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_or","vs","Int16","Int16","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_or_ssxB_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_or_ssxB_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_or","vv","Int16","Int16","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_or_ssxB_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_or_ssxB_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_or","sv","Int16","Int16","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_or_ssxB_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_xor_ssxB_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_xor","vs","Int16","Int16","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_xor_ssxB_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_xor_ssxB_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_xor","vv","Int16","Int16","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_xor_ssxB_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_xor_ssxB_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_xor","sv","Int16","Int16","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_xor_ssxB_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_not_sxB_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","logical_not","v","Int16","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_not_sxB_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_and_ssxs_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_and","vs","Int16","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_and_ssxs_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_and_ssxs_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_and","vv","Int16","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_and_ssxs_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_and_sxs_R */
    keytuple=Py_BuildValue("ss((s)(s))","bitwise_and","R","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_and_sxs_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_and_sxs_A */
    keytuple=Py_BuildValue("ss((s)(s))","bitwise_and","A","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_and_sxs_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_and_ssxs_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_and","sv","Int16","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_and_ssxs_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_or_ssxs_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_or","vs","Int16","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_or_ssxs_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_or_ssxs_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_or","vv","Int16","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_or_ssxs_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_or_sxs_R */
    keytuple=Py_BuildValue("ss((s)(s))","bitwise_or","R","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_or_sxs_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_or_sxs_A */
    keytuple=Py_BuildValue("ss((s)(s))","bitwise_or","A","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_or_sxs_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_or_ssxs_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_or","sv","Int16","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_or_ssxs_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_xor_ssxs_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_xor","vs","Int16","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_xor_ssxs_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_xor_ssxs_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_xor","vv","Int16","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_xor_ssxs_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_xor_sxs_R */
    keytuple=Py_BuildValue("ss((s)(s))","bitwise_xor","R","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_xor_sxs_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_xor_sxs_A */
    keytuple=Py_BuildValue("ss((s)(s))","bitwise_xor","A","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_xor_sxs_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_xor_ssxs_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_xor","sv","Int16","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_xor_ssxs_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_not_sxs_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","bitwise_not","v","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_not_sxs_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* rshift_ssxs_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","rshift","vs","Int16","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&rshift_ssxs_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* rshift_ssxs_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","rshift","vv","Int16","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&rshift_ssxs_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* rshift_ssxs_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","rshift","sv","Int16","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&rshift_ssxs_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* lshift_ssxs_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","lshift","vs","Int16","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&lshift_ssxs_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* lshift_ssxs_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","lshift","vv","Int16","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&lshift_ssxs_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* lshift_ssxs_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","lshift","sv","Int16","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&lshift_ssxs_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_sxs_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","floor","v","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_sxs_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* ceil_sxs_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","ceil","v","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&ceil_sxs_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_ssxs_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","maximum","vs","Int16","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_ssxs_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_ssxs_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","maximum","vv","Int16","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_ssxs_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_sxs_R */
    keytuple=Py_BuildValue("ss((s)(s))","maximum","R","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_sxs_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_sxs_A */
    keytuple=Py_BuildValue("ss((s)(s))","maximum","A","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_sxs_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_ssxs_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","maximum","sv","Int16","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_ssxs_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_ssxs_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","minimum","vs","Int16","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_ssxs_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_ssxs_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","minimum","vv","Int16","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_ssxs_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_sxs_R */
    keytuple=Py_BuildValue("ss((s)(s))","minimum","R","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_sxs_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_sxs_A */
    keytuple=Py_BuildValue("ss((s)(s))","minimum","A","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_sxs_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_ssxs_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","minimum","sv","Int16","Int16","Int16");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_ssxs_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* fabs_sxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","fabs","v","Int16","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&fabs_sxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* _round_sxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","_round","v","Int16","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&_round_sxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_ssxd_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","hypot","vs","Int16","Int16","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_ssxd_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_ssxd_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","hypot","vv","Int16","Int16","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_ssxd_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_sxd_R */
    keytuple=Py_BuildValue("ss((s)(s))","hypot","R","Int16","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_sxd_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_sxd_A */
    keytuple=Py_BuildValue("ss((s)(s))","hypot","A","Int16","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_sxd_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_ssxd_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","hypot","sv","Int16","Int16","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_ssxd_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    return dict;
}

/* platform independent*/
#ifdef MS_WIN32
__declspec(dllexport)
#endif
void init_ufuncInt16(void) {
    PyObject *m, *d, *functions;
    m = Py_InitModule("_ufuncInt16", _ufuncInt16Methods);
    d = PyModule_GetDict(m);
    import_libnumarray();
    functions = init_funcDict();
    PyDict_SetItemString(d, "functionDict", functions);
    Py_DECREF(functions);
    ADD_VERSION(m);
}
