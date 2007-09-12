
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

static int minus_ixi_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Int32      *tout0 =  (Int32 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = -*tin0;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor minus_ixi_vxv_descr =
{ "minus_ixi_vxv", (void *) minus_ixi_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int32), sizeof(Int32) }, { 0, 0, 0 } };
/*********************  add  *********************/

static int add_iixi_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Int32       tin1 = *(Int32 *) buffers[1];
    Int32      *tout0 =  (Int32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 + tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor add_iixi_vsxv_descr =
{ "add_iixi_vsxv", (void *) add_iixi_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Int32) }, { 0, 1, 0, 0 } };

static int add_iixi_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Int32      *tin1 =  (Int32 *) buffers[1];
    Int32      *tout0 =  (Int32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 + *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor add_iixi_vvxv_descr =
{ "add_iixi_vvxv", (void *) add_iixi_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Int32) }, { 0, 0, 0, 0 } };

static void _add_ixi_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int32  *tin0   = (Int32 *) ((char *) input  + inboffset);
    Int32 *tout0  = (Int32 *) ((char *) output + outboffset);
    Int32 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Int32 *) ((char *) tin0 + inbstrides[dim]);
            net    =     net + *tin0;
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _add_ixi_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int add_ixi_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _add_ixi_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(add_ixi_R, CHECK_ALIGN, sizeof(Int32), sizeof(Int32));

static void _add_ixi_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int32 *tin0   = (Int32 *) ((char *) input  + inboffset);
    Int32 *tout0 = (Int32 *) ((char *) output + outboffset);
    Int32 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Int32 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Int32 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = lastval + *tin0;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _add_ixi_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  add_ixi_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _add_ixi_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(add_ixi_A, CHECK_ALIGN, sizeof(Int32), sizeof(Int32));

static int add_iixi_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32       tin0 = *(Int32 *) buffers[0];
    Int32      *tin1 =  (Int32 *) buffers[1];
    Int32      *tout0 =  (Int32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 + *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor add_iixi_svxv_descr =
{ "add_iixi_svxv", (void *) add_iixi_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Int32) }, { 1, 0, 0, 0 } };
/*********************  subtract  *********************/

static int subtract_iixi_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Int32       tin1 = *(Int32 *) buffers[1];
    Int32      *tout0 =  (Int32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 - tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor subtract_iixi_vsxv_descr =
{ "subtract_iixi_vsxv", (void *) subtract_iixi_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Int32) }, { 0, 1, 0, 0 } };

static int subtract_iixi_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Int32      *tin1 =  (Int32 *) buffers[1];
    Int32      *tout0 =  (Int32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 - *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor subtract_iixi_vvxv_descr =
{ "subtract_iixi_vvxv", (void *) subtract_iixi_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Int32) }, { 0, 0, 0, 0 } };

static void _subtract_ixi_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int32  *tin0   = (Int32 *) ((char *) input  + inboffset);
    Int32 *tout0  = (Int32 *) ((char *) output + outboffset);
    Int32 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Int32 *) ((char *) tin0 + inbstrides[dim]);
            net    =     net - *tin0;
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _subtract_ixi_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int subtract_ixi_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _subtract_ixi_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(subtract_ixi_R, CHECK_ALIGN, sizeof(Int32), sizeof(Int32));

static void _subtract_ixi_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int32 *tin0   = (Int32 *) ((char *) input  + inboffset);
    Int32 *tout0 = (Int32 *) ((char *) output + outboffset);
    Int32 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Int32 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Int32 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = lastval - *tin0;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _subtract_ixi_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  subtract_ixi_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _subtract_ixi_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(subtract_ixi_A, CHECK_ALIGN, sizeof(Int32), sizeof(Int32));

static int subtract_iixi_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32       tin0 = *(Int32 *) buffers[0];
    Int32      *tin1 =  (Int32 *) buffers[1];
    Int32      *tout0 =  (Int32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 - *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor subtract_iixi_svxv_descr =
{ "subtract_iixi_svxv", (void *) subtract_iixi_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Int32) }, { 1, 0, 0, 0 } };
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/

static int multiply_iixi_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Int32       tin1 = *(Int32 *) buffers[1];
    Int32      *tout0 =  (Int32 *) buffers[2];
Float64 temp;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        temp = ((Float64) *tin0) * ((Float64) tin1);
    if (temp > 2147483647) temp = (Float64) int_overflow_error(2147483647.);
if (temp < (-2147483648.)) temp = (Float64) int_overflow_error(-2147483648.);
    *tout0 = (Int32) temp;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor multiply_iixi_vsxv_descr =
{ "multiply_iixi_vsxv", (void *) multiply_iixi_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Int32) }, { 0, 1, 0, 0 } };

static int multiply_iixi_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Int32      *tin1 =  (Int32 *) buffers[1];
    Int32      *tout0 =  (Int32 *) buffers[2];
Float64 temp;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        temp = ((Float64) *tin0) * ((Float64) *tin1);
    if (temp > 2147483647) temp = (Float64) int_overflow_error(2147483647.);
if (temp < (-2147483648.)) temp = (Float64) int_overflow_error(-2147483648.);
    *tout0 = (Int32) temp;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor multiply_iixi_vvxv_descr =
{ "multiply_iixi_vvxv", (void *) multiply_iixi_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Int32) }, { 0, 0, 0, 0 } };

static void _multiply_ixi_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int32  *tin0   = (Int32 *) ((char *) input  + inboffset);
    Int32 *tout0  = (Int32 *) ((char *) output + outboffset);
    Int32 net;
    Float64 temp;
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Int32 *) ((char *) tin0 + inbstrides[dim]);
            temp = ((Float64) net) * ((Float64) *tin0);
    if (temp > 2147483647) temp = (Float64) int_overflow_error(2147483647.);
if (temp < (-2147483648.)) temp = (Float64) int_overflow_error(-2147483648.);
    net = (Int32) temp;
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _multiply_ixi_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int multiply_ixi_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _multiply_ixi_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(multiply_ixi_R, CHECK_ALIGN, sizeof(Int32), sizeof(Int32));

static void _multiply_ixi_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int32 *tin0   = (Int32 *) ((char *) input  + inboffset);
    Int32 *tout0 = (Int32 *) ((char *) output + outboffset);
    Int32 lastval;
    Float64 temp;
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Int32 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Int32 *) ((char *) tout0 + outbstrides[dim]);
            temp = ((Float64) lastval) * ((Float64) *tin0);
    if (temp > 2147483647) temp = (Float64) int_overflow_error(2147483647.);
if (temp < (-2147483648.)) temp = (Float64) int_overflow_error(-2147483648.);
    *tout0 = (Int32) temp;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _multiply_ixi_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  multiply_ixi_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _multiply_ixi_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(multiply_ixi_A, CHECK_ALIGN, sizeof(Int32), sizeof(Int32));

static int multiply_iixi_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32       tin0 = *(Int32 *) buffers[0];
    Int32      *tin1 =  (Int32 *) buffers[1];
    Int32      *tout0 =  (Int32 *) buffers[2];
Float64 temp;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        temp = ((Float64) tin0) * ((Float64) *tin1);
    if (temp > 2147483647) temp = (Float64) int_overflow_error(2147483647.);
if (temp < (-2147483648.)) temp = (Float64) int_overflow_error(-2147483648.);
    *tout0 = (Int32) temp;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor multiply_iixi_svxv_descr =
{ "multiply_iixi_svxv", (void *) multiply_iixi_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Int32) }, { 1, 0, 0, 0 } };
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  divide  *********************/

static int divide_iixi_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Int32       tin1 = *(Int32 *) buffers[1];
    Int32      *tout0 =  (Int32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((tin1==0) ? int_dividebyzero_error(tin1, *tin0) : *tin0 / tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor divide_iixi_vsxv_descr =
{ "divide_iixi_vsxv", (void *) divide_iixi_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Int32) }, { 0, 1, 0, 0 } };

static int divide_iixi_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Int32      *tin1 =  (Int32 *) buffers[1];
    Int32      *tout0 =  (Int32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, *tin0) : *tin0 / *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor divide_iixi_vvxv_descr =
{ "divide_iixi_vvxv", (void *) divide_iixi_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Int32) }, { 0, 0, 0, 0 } };

static void _divide_ixi_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int32  *tin0   = (Int32 *) ((char *) input  + inboffset);
    Int32 *tout0  = (Int32 *) ((char *) output + outboffset);
    Int32 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Int32 *) ((char *) tin0 + inbstrides[dim]);
            net = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0) : net / *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _divide_ixi_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int divide_ixi_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _divide_ixi_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(divide_ixi_R, CHECK_ALIGN, sizeof(Int32), sizeof(Int32));

static void _divide_ixi_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int32 *tin0   = (Int32 *) ((char *) input  + inboffset);
    Int32 *tout0 = (Int32 *) ((char *) output + outboffset);
    Int32 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Int32 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Int32 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0) : lastval / *tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _divide_ixi_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  divide_ixi_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _divide_ixi_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(divide_ixi_A, CHECK_ALIGN, sizeof(Int32), sizeof(Int32));

static int divide_iixi_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32       tin0 = *(Int32 *) buffers[0];
    Int32      *tin1 =  (Int32 *) buffers[1];
    Int32      *tout0 =  (Int32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, 0) : tin0 / *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor divide_iixi_svxv_descr =
{ "divide_iixi_svxv", (void *) divide_iixi_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Int32) }, { 1, 0, 0, 0 } };
/*********************  divide  *********************/
/*********************  floor_divide  *********************/

static int floor_divide_iixi_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Int32       tin1 = *(Int32 *) buffers[1];
    Int32      *tout0 =  (Int32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = (( tin1==0) ? int_dividebyzero_error( tin1, *tin0) : floor(*tin0   / (double)  tin1));
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor floor_divide_iixi_vsxv_descr =
{ "floor_divide_iixi_vsxv", (void *) floor_divide_iixi_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Int32) }, { 0, 1, 0, 0 } };

static int floor_divide_iixi_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Int32      *tin1 =  (Int32 *) buffers[1];
    Int32      *tout0 =  (Int32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, *tin0) : floor(*tin0   / (double) *tin1));
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor floor_divide_iixi_vvxv_descr =
{ "floor_divide_iixi_vvxv", (void *) floor_divide_iixi_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Int32) }, { 0, 0, 0, 0 } };

static void _floor_divide_ixi_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int32  *tin0   = (Int32 *) ((char *) input  + inboffset);
    Int32 *tout0  = (Int32 *) ((char *) output + outboffset);
    Int32 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Int32 *) ((char *) tin0 + inbstrides[dim]);
            net    = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0)     : floor(net     / (double) *tin0));
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _floor_divide_ixi_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int floor_divide_ixi_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _floor_divide_ixi_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(floor_divide_ixi_R, CHECK_ALIGN, sizeof(Int32), sizeof(Int32));

static void _floor_divide_ixi_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int32 *tin0   = (Int32 *) ((char *) input  + inboffset);
    Int32 *tout0 = (Int32 *) ((char *) output + outboffset);
    Int32 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Int32 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Int32 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0)     : floor(lastval / (double) *tin0));
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _floor_divide_ixi_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  floor_divide_ixi_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _floor_divide_ixi_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(floor_divide_ixi_A, CHECK_ALIGN, sizeof(Int32), sizeof(Int32));

static int floor_divide_iixi_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32       tin0 = *(Int32 *) buffers[0];
    Int32      *tin1 =  (Int32 *) buffers[1];
    Int32      *tout0 =  (Int32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, 0)     : floor(tin0    / (double) *tin1));
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor floor_divide_iixi_svxv_descr =
{ "floor_divide_iixi_svxv", (void *) floor_divide_iixi_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Int32) }, { 1, 0, 0, 0 } };
/*********************  floor_divide  *********************/
/*********************  true_divide  *********************/

static int true_divide_iixf_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Int32       tin1 = *(Int32 *) buffers[1];
    Float32    *tout0 =  (Float32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = (( tin1==0) ? int_dividebyzero_error( tin1, *tin0) : *tin0         / (double) tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor true_divide_iixf_vsxv_descr =
{ "true_divide_iixf_vsxv", (void *) true_divide_iixf_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Float32) }, { 0, 1, 0, 0 } };

static int true_divide_iixf_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Int32      *tin1 =  (Int32 *) buffers[1];
    Float32    *tout0 =  (Float32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, *tin0) : *tin0         / (double) *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor true_divide_iixf_vvxv_descr =
{ "true_divide_iixf_vvxv", (void *) true_divide_iixf_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Float32) }, { 0, 0, 0, 0 } };

static void _true_divide_ixf_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int32  *tin0   = (Int32 *) ((char *) input  + inboffset);
    Float32 *tout0  = (Float32 *) ((char *) output + outboffset);
    Float32 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Int32 *) ((char *) tin0 + inbstrides[dim]);
            net    = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0)     : net           / (double) *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _true_divide_ixf_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int true_divide_ixf_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _true_divide_ixf_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(true_divide_ixf_R, CHECK_ALIGN, sizeof(Int32), sizeof(Float32));

static void _true_divide_ixf_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int32 *tin0   = (Int32 *) ((char *) input  + inboffset);
    Float32 *tout0 = (Float32 *) ((char *) output + outboffset);
    Float32 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Int32 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Float32 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0)     : lastval       / (double) *tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _true_divide_ixf_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  true_divide_ixf_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _true_divide_ixf_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(true_divide_ixf_A, CHECK_ALIGN, sizeof(Int32), sizeof(Float32));

static int true_divide_iixf_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32       tin0 = *(Int32 *) buffers[0];
    Int32      *tin1 =  (Int32 *) buffers[1];
    Float32    *tout0 =  (Float32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, 0)     : tin0          / (double) *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor true_divide_iixf_svxv_descr =
{ "true_divide_iixf_svxv", (void *) true_divide_iixf_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Float32) }, { 1, 0, 0, 0 } };
/*********************  true_divide  *********************/
/*********************  remainder  *********************/

static int remainder_iixi_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Int32       tin1 = *(Int32 *) buffers[1];
    Int32      *tout0 =  (Int32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((tin1==0) ? int_dividebyzero_error(tin1, *tin0) : *tin0  %  tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor remainder_iixi_vsxv_descr =
{ "remainder_iixi_vsxv", (void *) remainder_iixi_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Int32) }, { 0, 1, 0, 0 } };

static int remainder_iixi_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Int32      *tin1 =  (Int32 *) buffers[1];
    Int32      *tout0 =  (Int32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, *tin0) : *tin0  %  *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor remainder_iixi_vvxv_descr =
{ "remainder_iixi_vvxv", (void *) remainder_iixi_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Int32) }, { 0, 0, 0, 0 } };

static void _remainder_ixi_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int32  *tin0   = (Int32 *) ((char *) input  + inboffset);
    Int32 *tout0  = (Int32 *) ((char *) output + outboffset);
    Int32 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Int32 *) ((char *) tin0 + inbstrides[dim]);
            net = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0) : net  %  *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _remainder_ixi_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int remainder_ixi_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _remainder_ixi_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(remainder_ixi_R, CHECK_ALIGN, sizeof(Int32), sizeof(Int32));

static void _remainder_ixi_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int32 *tin0   = (Int32 *) ((char *) input  + inboffset);
    Int32 *tout0 = (Int32 *) ((char *) output + outboffset);
    Int32 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Int32 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Int32 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0) : lastval  %  *tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _remainder_ixi_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  remainder_ixi_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _remainder_ixi_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(remainder_ixi_A, CHECK_ALIGN, sizeof(Int32), sizeof(Int32));

static int remainder_iixi_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32       tin0 = *(Int32 *) buffers[0];
    Int32      *tin1 =  (Int32 *) buffers[1];
    Int32      *tout0 =  (Int32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, 0) : tin0  %  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor remainder_iixi_svxv_descr =
{ "remainder_iixi_svxv", (void *) remainder_iixi_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Int32) }, { 1, 0, 0, 0 } };
/*********************  remainder  *********************/
/*********************  power  *********************/

static int power_iixi_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Int32       tin1 = *(Int32 *) buffers[1];
    Int32      *tout0 =  (Int32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_pow(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor power_iixi_vsxf_descr =
{ "power_iixi_vsxf", (void *) power_iixi_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Int32) }, { 0, 1, 0, 0 } };

static int power_iixi_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Int32      *tin1 =  (Int32 *) buffers[1];
    Int32      *tout0 =  (Int32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_pow(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor power_iixi_vvxf_descr =
{ "power_iixi_vvxf", (void *) power_iixi_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Int32) }, { 0, 0, 0, 0 } };

static void _power_ixi_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int32  *tin0   = (Int32 *) ((char *) input  + inboffset);
    Int32 *tout0  = (Int32 *) ((char *) output + outboffset);
    Int32 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Int32 *) ((char *) tin0 + inbstrides[dim]);
            net   = num_pow(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _power_ixi_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int power_ixi_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _power_ixi_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(power_ixi_R, CHECK_ALIGN, sizeof(Int32), sizeof(Int32));

static void _power_ixi_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int32 *tin0   = (Int32 *) ((char *) input  + inboffset);
    Int32 *tout0 = (Int32 *) ((char *) output + outboffset);
    Int32 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Int32 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Int32 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = num_pow(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _power_ixi_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  power_ixi_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _power_ixi_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(power_ixi_A, CHECK_ALIGN, sizeof(Int32), sizeof(Int32));

static int power_iixi_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32       tin0 = *(Int32 *) buffers[0];
    Int32      *tin1 =  (Int32 *) buffers[1];
    Int32      *tout0 =  (Int32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_pow(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor power_iixi_svxf_descr =
{ "power_iixi_svxf", (void *) power_iixi_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Int32) }, { 1, 0, 0, 0 } };
/*********************  abs  *********************/

static int abs_ixi_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Int32      *tout0 =  (Int32 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = fabs(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor abs_ixi_vxf_descr =
{ "abs_ixi_vxf", (void *) abs_ixi_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int32), sizeof(Int32) }, { 0, 0, 0 } };
/*********************  sin  *********************/

static int sin_ixd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = sin(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor sin_ixd_vxf_descr =
{ "sin_ixd_vxf", (void *) sin_ixd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int32), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  cos  *********************/

static int cos_ixd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = cos(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor cos_ixd_vxf_descr =
{ "cos_ixd_vxf", (void *) cos_ixd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int32), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  tan  *********************/

static int tan_ixd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = tan(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor tan_ixd_vxf_descr =
{ "tan_ixd_vxf", (void *) tan_ixd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int32), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arcsin  *********************/

static int arcsin_ixd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = asin(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arcsin_ixd_vxf_descr =
{ "arcsin_ixd_vxf", (void *) arcsin_ixd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int32), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arccos  *********************/

static int arccos_ixd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = acos(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arccos_ixd_vxf_descr =
{ "arccos_ixd_vxf", (void *) arccos_ixd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int32), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arctan  *********************/

static int arctan_ixd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = atan(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arctan_ixd_vxf_descr =
{ "arctan_ixd_vxf", (void *) arctan_ixd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int32), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arctan2  *********************/

static int arctan2_iixd_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Int32       tin1 = *(Int32 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = atan2(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arctan2_iixd_vsxf_descr =
{ "arctan2_iixd_vsxf", (void *) arctan2_iixd_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Float64) }, { 0, 1, 0, 0 } };

static int arctan2_iixd_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Int32      *tin1 =  (Int32 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = atan2(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arctan2_iixd_vvxf_descr =
{ "arctan2_iixd_vvxf", (void *) arctan2_iixd_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Float64) }, { 0, 0, 0, 0 } };

static void _arctan2_ixd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int32  *tin0   = (Int32 *) ((char *) input  + inboffset);
    Float64 *tout0  = (Float64 *) ((char *) output + outboffset);
    Float64 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Int32 *) ((char *) tin0 + inbstrides[dim]);
            net   = atan2(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _arctan2_ixd_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int arctan2_ixd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _arctan2_ixd_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(arctan2_ixd_R, CHECK_ALIGN, sizeof(Int32), sizeof(Float64));

static void _arctan2_ixd_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int32 *tin0   = (Int32 *) ((char *) input  + inboffset);
    Float64 *tout0 = (Float64 *) ((char *) output + outboffset);
    Float64 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Int32 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Float64 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = atan2(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _arctan2_ixd_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  arctan2_ixd_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _arctan2_ixd_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(arctan2_ixd_A, CHECK_ALIGN, sizeof(Int32), sizeof(Float64));

static int arctan2_iixd_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32       tin0 = *(Int32 *) buffers[0];
    Int32      *tin1 =  (Int32 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = atan2(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arctan2_iixd_svxf_descr =
{ "arctan2_iixd_svxf", (void *) arctan2_iixd_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Float64) }, { 1, 0, 0, 0 } };
/*********************  log  *********************/

static int log_ixd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_log(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor log_ixd_vxf_descr =
{ "log_ixd_vxf", (void *) log_ixd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int32), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  log10  *********************/

static int log10_ixd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_log10(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor log10_ixd_vxf_descr =
{ "log10_ixd_vxf", (void *) log10_ixd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int32), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  exp  *********************/

static int exp_ixd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = exp(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor exp_ixd_vxf_descr =
{ "exp_ixd_vxf", (void *) exp_ixd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int32), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  sinh  *********************/

static int sinh_ixd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = sinh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor sinh_ixd_vxf_descr =
{ "sinh_ixd_vxf", (void *) sinh_ixd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int32), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  cosh  *********************/

static int cosh_ixd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = cosh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor cosh_ixd_vxf_descr =
{ "cosh_ixd_vxf", (void *) cosh_ixd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int32), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  tanh  *********************/

static int tanh_ixd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = tanh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor tanh_ixd_vxf_descr =
{ "tanh_ixd_vxf", (void *) tanh_ixd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int32), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arcsinh  *********************/

static int arcsinh_ixd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_asinh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arcsinh_ixd_vxf_descr =
{ "arcsinh_ixd_vxf", (void *) arcsinh_ixd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int32), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arccosh  *********************/

static int arccosh_ixd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_acosh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arccosh_ixd_vxf_descr =
{ "arccosh_ixd_vxf", (void *) arccosh_ixd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int32), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arctanh  *********************/

static int arctanh_ixd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_atanh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arctanh_ixd_vxf_descr =
{ "arctanh_ixd_vxf", (void *) arctanh_ixd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int32), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  ieeemask  *********************/
/*********************  ieeemask  *********************/
/*********************  isnan  *********************/
/*********************  isnan  *********************/
/*********************  isnan  *********************/
/*********************  isnan  *********************/
/*********************  sqrt  *********************/

static int sqrt_ixd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = sqrt(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor sqrt_ixd_vxf_descr =
{ "sqrt_ixd_vxf", (void *) sqrt_ixd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int32), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  equal  *********************/

static int equal_iixB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Int32       tin1 = *(Int32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 == tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor equal_iixB_vsxv_descr =
{ "equal_iixB_vsxv", (void *) equal_iixB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int equal_iixB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Int32      *tin1 =  (Int32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 == *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor equal_iixB_vvxv_descr =
{ "equal_iixB_vvxv", (void *) equal_iixB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int equal_iixB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32       tin0 = *(Int32 *) buffers[0];
    Int32      *tin1 =  (Int32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 == *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor equal_iixB_svxv_descr =
{ "equal_iixB_svxv", (void *) equal_iixB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  not_equal  *********************/

static int not_equal_iixB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Int32       tin1 = *(Int32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 != tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor not_equal_iixB_vsxv_descr =
{ "not_equal_iixB_vsxv", (void *) not_equal_iixB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int not_equal_iixB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Int32      *tin1 =  (Int32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 != *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor not_equal_iixB_vvxv_descr =
{ "not_equal_iixB_vvxv", (void *) not_equal_iixB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int not_equal_iixB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32       tin0 = *(Int32 *) buffers[0];
    Int32      *tin1 =  (Int32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 != *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor not_equal_iixB_svxv_descr =
{ "not_equal_iixB_svxv", (void *) not_equal_iixB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  greater  *********************/

static int greater_iixB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Int32       tin1 = *(Int32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 > tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_iixB_vsxv_descr =
{ "greater_iixB_vsxv", (void *) greater_iixB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int greater_iixB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Int32      *tin1 =  (Int32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 > *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_iixB_vvxv_descr =
{ "greater_iixB_vvxv", (void *) greater_iixB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int greater_iixB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32       tin0 = *(Int32 *) buffers[0];
    Int32      *tin1 =  (Int32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 > *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_iixB_svxv_descr =
{ "greater_iixB_svxv", (void *) greater_iixB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  greater_equal  *********************/

static int greater_equal_iixB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Int32       tin1 = *(Int32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 >= tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_equal_iixB_vsxv_descr =
{ "greater_equal_iixB_vsxv", (void *) greater_equal_iixB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int greater_equal_iixB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Int32      *tin1 =  (Int32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 >= *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_equal_iixB_vvxv_descr =
{ "greater_equal_iixB_vvxv", (void *) greater_equal_iixB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int greater_equal_iixB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32       tin0 = *(Int32 *) buffers[0];
    Int32      *tin1 =  (Int32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 >= *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_equal_iixB_svxv_descr =
{ "greater_equal_iixB_svxv", (void *) greater_equal_iixB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  less  *********************/

static int less_iixB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Int32       tin1 = *(Int32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 < tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_iixB_vsxv_descr =
{ "less_iixB_vsxv", (void *) less_iixB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int less_iixB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Int32      *tin1 =  (Int32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 < *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_iixB_vvxv_descr =
{ "less_iixB_vvxv", (void *) less_iixB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int less_iixB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32       tin0 = *(Int32 *) buffers[0];
    Int32      *tin1 =  (Int32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 < *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_iixB_svxv_descr =
{ "less_iixB_svxv", (void *) less_iixB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  less_equal  *********************/

static int less_equal_iixB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Int32       tin1 = *(Int32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 <= tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_equal_iixB_vsxv_descr =
{ "less_equal_iixB_vsxv", (void *) less_equal_iixB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int less_equal_iixB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Int32      *tin1 =  (Int32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 <= *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_equal_iixB_vvxv_descr =
{ "less_equal_iixB_vvxv", (void *) less_equal_iixB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int less_equal_iixB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32       tin0 = *(Int32 *) buffers[0];
    Int32      *tin1 =  (Int32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 <= *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_equal_iixB_svxv_descr =
{ "less_equal_iixB_svxv", (void *) less_equal_iixB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  logical_and  *********************/

static int logical_and_iixB_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Int32       tin1 = *(Int32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_and(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_and_iixB_vsxf_descr =
{ "logical_and_iixB_vsxf", (void *) logical_and_iixB_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int logical_and_iixB_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Int32      *tin1 =  (Int32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_and(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_and_iixB_vvxf_descr =
{ "logical_and_iixB_vvxf", (void *) logical_and_iixB_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int logical_and_iixB_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32       tin0 = *(Int32 *) buffers[0];
    Int32      *tin1 =  (Int32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_and(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_and_iixB_svxf_descr =
{ "logical_and_iixB_svxf", (void *) logical_and_iixB_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  logical_or  *********************/

static int logical_or_iixB_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Int32       tin1 = *(Int32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_or(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_or_iixB_vsxf_descr =
{ "logical_or_iixB_vsxf", (void *) logical_or_iixB_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int logical_or_iixB_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Int32      *tin1 =  (Int32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_or(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_or_iixB_vvxf_descr =
{ "logical_or_iixB_vvxf", (void *) logical_or_iixB_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int logical_or_iixB_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32       tin0 = *(Int32 *) buffers[0];
    Int32      *tin1 =  (Int32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_or(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_or_iixB_svxf_descr =
{ "logical_or_iixB_svxf", (void *) logical_or_iixB_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  logical_xor  *********************/

static int logical_xor_iixB_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Int32       tin1 = *(Int32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_xor(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_xor_iixB_vsxf_descr =
{ "logical_xor_iixB_vsxf", (void *) logical_xor_iixB_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int logical_xor_iixB_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Int32      *tin1 =  (Int32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_xor(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_xor_iixB_vvxf_descr =
{ "logical_xor_iixB_vvxf", (void *) logical_xor_iixB_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int logical_xor_iixB_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32       tin0 = *(Int32 *) buffers[0];
    Int32      *tin1 =  (Int32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_xor(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_xor_iixB_svxf_descr =
{ "logical_xor_iixB_svxf", (void *) logical_xor_iixB_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  logical_and  *********************/
/*********************  logical_or  *********************/
/*********************  logical_xor  *********************/
/*********************  logical_not  *********************/

static int logical_not_ixB_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Bool       *tout0 =  (Bool *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = !*tin0;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_not_ixB_vxv_descr =
{ "logical_not_ixB_vxv", (void *) logical_not_ixB_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int32), sizeof(Bool) }, { 0, 0, 0 } };
/*********************  bitwise_and  *********************/

static int bitwise_and_iixi_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Int32       tin1 = *(Int32 *) buffers[1];
    Int32      *tout0 =  (Int32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 & tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_and_iixi_vsxv_descr =
{ "bitwise_and_iixi_vsxv", (void *) bitwise_and_iixi_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Int32) }, { 0, 1, 0, 0 } };

static int bitwise_and_iixi_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Int32      *tin1 =  (Int32 *) buffers[1];
    Int32      *tout0 =  (Int32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 & *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_and_iixi_vvxv_descr =
{ "bitwise_and_iixi_vvxv", (void *) bitwise_and_iixi_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Int32) }, { 0, 0, 0, 0 } };

static void _bitwise_and_ixi_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int32  *tin0   = (Int32 *) ((char *) input  + inboffset);
    Int32 *tout0  = (Int32 *) ((char *) output + outboffset);
    Int32 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Int32 *) ((char *) tin0 + inbstrides[dim]);
            net    =     net & *tin0;
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _bitwise_and_ixi_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int bitwise_and_ixi_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _bitwise_and_ixi_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(bitwise_and_ixi_R, CHECK_ALIGN, sizeof(Int32), sizeof(Int32));

static void _bitwise_and_ixi_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int32 *tin0   = (Int32 *) ((char *) input  + inboffset);
    Int32 *tout0 = (Int32 *) ((char *) output + outboffset);
    Int32 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Int32 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Int32 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = lastval & *tin0;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _bitwise_and_ixi_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  bitwise_and_ixi_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _bitwise_and_ixi_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(bitwise_and_ixi_A, CHECK_ALIGN, sizeof(Int32), sizeof(Int32));

static int bitwise_and_iixi_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32       tin0 = *(Int32 *) buffers[0];
    Int32      *tin1 =  (Int32 *) buffers[1];
    Int32      *tout0 =  (Int32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 & *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_and_iixi_svxv_descr =
{ "bitwise_and_iixi_svxv", (void *) bitwise_and_iixi_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Int32) }, { 1, 0, 0, 0 } };
/*********************  bitwise_or  *********************/

static int bitwise_or_iixi_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Int32       tin1 = *(Int32 *) buffers[1];
    Int32      *tout0 =  (Int32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 | tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_or_iixi_vsxv_descr =
{ "bitwise_or_iixi_vsxv", (void *) bitwise_or_iixi_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Int32) }, { 0, 1, 0, 0 } };

static int bitwise_or_iixi_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Int32      *tin1 =  (Int32 *) buffers[1];
    Int32      *tout0 =  (Int32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 | *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_or_iixi_vvxv_descr =
{ "bitwise_or_iixi_vvxv", (void *) bitwise_or_iixi_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Int32) }, { 0, 0, 0, 0 } };

static void _bitwise_or_ixi_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int32  *tin0   = (Int32 *) ((char *) input  + inboffset);
    Int32 *tout0  = (Int32 *) ((char *) output + outboffset);
    Int32 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Int32 *) ((char *) tin0 + inbstrides[dim]);
            net    =     net | *tin0;
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _bitwise_or_ixi_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int bitwise_or_ixi_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _bitwise_or_ixi_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(bitwise_or_ixi_R, CHECK_ALIGN, sizeof(Int32), sizeof(Int32));

static void _bitwise_or_ixi_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int32 *tin0   = (Int32 *) ((char *) input  + inboffset);
    Int32 *tout0 = (Int32 *) ((char *) output + outboffset);
    Int32 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Int32 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Int32 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = lastval | *tin0;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _bitwise_or_ixi_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  bitwise_or_ixi_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _bitwise_or_ixi_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(bitwise_or_ixi_A, CHECK_ALIGN, sizeof(Int32), sizeof(Int32));

static int bitwise_or_iixi_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32       tin0 = *(Int32 *) buffers[0];
    Int32      *tin1 =  (Int32 *) buffers[1];
    Int32      *tout0 =  (Int32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 | *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_or_iixi_svxv_descr =
{ "bitwise_or_iixi_svxv", (void *) bitwise_or_iixi_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Int32) }, { 1, 0, 0, 0 } };
/*********************  bitwise_xor  *********************/

static int bitwise_xor_iixi_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Int32       tin1 = *(Int32 *) buffers[1];
    Int32      *tout0 =  (Int32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 ^ tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_xor_iixi_vsxv_descr =
{ "bitwise_xor_iixi_vsxv", (void *) bitwise_xor_iixi_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Int32) }, { 0, 1, 0, 0 } };

static int bitwise_xor_iixi_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Int32      *tin1 =  (Int32 *) buffers[1];
    Int32      *tout0 =  (Int32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 ^ *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_xor_iixi_vvxv_descr =
{ "bitwise_xor_iixi_vvxv", (void *) bitwise_xor_iixi_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Int32) }, { 0, 0, 0, 0 } };

static void _bitwise_xor_ixi_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int32  *tin0   = (Int32 *) ((char *) input  + inboffset);
    Int32 *tout0  = (Int32 *) ((char *) output + outboffset);
    Int32 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Int32 *) ((char *) tin0 + inbstrides[dim]);
            net    =     net ^ *tin0;
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _bitwise_xor_ixi_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int bitwise_xor_ixi_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _bitwise_xor_ixi_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(bitwise_xor_ixi_R, CHECK_ALIGN, sizeof(Int32), sizeof(Int32));

static void _bitwise_xor_ixi_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int32 *tin0   = (Int32 *) ((char *) input  + inboffset);
    Int32 *tout0 = (Int32 *) ((char *) output + outboffset);
    Int32 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Int32 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Int32 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = lastval ^ *tin0;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _bitwise_xor_ixi_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  bitwise_xor_ixi_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _bitwise_xor_ixi_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(bitwise_xor_ixi_A, CHECK_ALIGN, sizeof(Int32), sizeof(Int32));

static int bitwise_xor_iixi_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32       tin0 = *(Int32 *) buffers[0];
    Int32      *tin1 =  (Int32 *) buffers[1];
    Int32      *tout0 =  (Int32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 ^ *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_xor_iixi_svxv_descr =
{ "bitwise_xor_iixi_svxv", (void *) bitwise_xor_iixi_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Int32) }, { 1, 0, 0, 0 } };
/*********************  bitwise_not  *********************/

static int bitwise_not_ixi_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Int32      *tout0 =  (Int32 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ~*tin0;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_not_ixi_vxv_descr =
{ "bitwise_not_ixi_vxv", (void *) bitwise_not_ixi_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int32), sizeof(Int32) }, { 0, 0, 0 } };
/*********************  bitwise_not  *********************/
/*********************  rshift  *********************/

static int rshift_iixi_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Int32       tin1 = *(Int32 *) buffers[1];
    Int32      *tout0 =  (Int32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 >> tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor rshift_iixi_vsxv_descr =
{ "rshift_iixi_vsxv", (void *) rshift_iixi_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Int32) }, { 0, 1, 0, 0 } };

static int rshift_iixi_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Int32      *tin1 =  (Int32 *) buffers[1];
    Int32      *tout0 =  (Int32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 >> *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor rshift_iixi_vvxv_descr =
{ "rshift_iixi_vvxv", (void *) rshift_iixi_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Int32) }, { 0, 0, 0, 0 } };

static int rshift_iixi_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32       tin0 = *(Int32 *) buffers[0];
    Int32      *tin1 =  (Int32 *) buffers[1];
    Int32      *tout0 =  (Int32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 >> *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor rshift_iixi_svxv_descr =
{ "rshift_iixi_svxv", (void *) rshift_iixi_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Int32) }, { 1, 0, 0, 0 } };
/*********************  lshift  *********************/

static int lshift_iixi_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Int32       tin1 = *(Int32 *) buffers[1];
    Int32      *tout0 =  (Int32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 << tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor lshift_iixi_vsxv_descr =
{ "lshift_iixi_vsxv", (void *) lshift_iixi_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Int32) }, { 0, 1, 0, 0 } };

static int lshift_iixi_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Int32      *tin1 =  (Int32 *) buffers[1];
    Int32      *tout0 =  (Int32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 << *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor lshift_iixi_vvxv_descr =
{ "lshift_iixi_vvxv", (void *) lshift_iixi_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Int32) }, { 0, 0, 0, 0 } };

static int lshift_iixi_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32       tin0 = *(Int32 *) buffers[0];
    Int32      *tin1 =  (Int32 *) buffers[1];
    Int32      *tout0 =  (Int32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 << *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor lshift_iixi_svxv_descr =
{ "lshift_iixi_svxv", (void *) lshift_iixi_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Int32) }, { 1, 0, 0, 0 } };
/*********************  floor  *********************/

static int floor_ixi_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Int32      *tout0 =  (Int32 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = (*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor floor_ixi_vxf_descr =
{ "floor_ixi_vxf", (void *) floor_ixi_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int32), sizeof(Int32) }, { 0, 0, 0 } };
/*********************  floor  *********************/
/*********************  ceil  *********************/

static int ceil_ixi_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Int32      *tout0 =  (Int32 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = (*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor ceil_ixi_vxf_descr =
{ "ceil_ixi_vxf", (void *) ceil_ixi_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int32), sizeof(Int32) }, { 0, 0, 0 } };
/*********************  ceil  *********************/
/*********************  maximum  *********************/

static int maximum_iixi_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Int32       tin1 = *(Int32 *) buffers[1];
    Int32      *tout0 =  (Int32 *) buffers[2];
Int32 temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufmaximum(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor maximum_iixi_vsxf_descr =
{ "maximum_iixi_vsxf", (void *) maximum_iixi_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Int32) }, { 0, 1, 0, 0 } };

static int maximum_iixi_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Int32      *tin1 =  (Int32 *) buffers[1];
    Int32      *tout0 =  (Int32 *) buffers[2];
Int32 temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufmaximum(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor maximum_iixi_vvxf_descr =
{ "maximum_iixi_vvxf", (void *) maximum_iixi_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Int32) }, { 0, 0, 0, 0 } };

static void _maximum_ixi_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int32  *tin0   = (Int32 *) ((char *) input  + inboffset);
    Int32 *tout0  = (Int32 *) ((char *) output + outboffset);
    Int32 net;
    Int32 temp1, temp2;
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Int32 *) ((char *) tin0 + inbstrides[dim]);
            net   = ufmaximum(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _maximum_ixi_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int maximum_ixi_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _maximum_ixi_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(maximum_ixi_R, CHECK_ALIGN, sizeof(Int32), sizeof(Int32));

static void _maximum_ixi_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int32 *tin0   = (Int32 *) ((char *) input  + inboffset);
    Int32 *tout0 = (Int32 *) ((char *) output + outboffset);
    Int32 lastval;
    Int32 temp1, temp2;
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Int32 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Int32 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = ufmaximum(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _maximum_ixi_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  maximum_ixi_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _maximum_ixi_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(maximum_ixi_A, CHECK_ALIGN, sizeof(Int32), sizeof(Int32));

static int maximum_iixi_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32       tin0 = *(Int32 *) buffers[0];
    Int32      *tin1 =  (Int32 *) buffers[1];
    Int32      *tout0 =  (Int32 *) buffers[2];
Int32 temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufmaximum(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor maximum_iixi_svxf_descr =
{ "maximum_iixi_svxf", (void *) maximum_iixi_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Int32) }, { 1, 0, 0, 0 } };
/*********************  minimum  *********************/

static int minimum_iixi_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Int32       tin1 = *(Int32 *) buffers[1];
    Int32      *tout0 =  (Int32 *) buffers[2];
Int32 temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufminimum(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor minimum_iixi_vsxf_descr =
{ "minimum_iixi_vsxf", (void *) minimum_iixi_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Int32) }, { 0, 1, 0, 0 } };

static int minimum_iixi_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Int32      *tin1 =  (Int32 *) buffers[1];
    Int32      *tout0 =  (Int32 *) buffers[2];
Int32 temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufminimum(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor minimum_iixi_vvxf_descr =
{ "minimum_iixi_vvxf", (void *) minimum_iixi_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Int32) }, { 0, 0, 0, 0 } };

static void _minimum_ixi_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int32  *tin0   = (Int32 *) ((char *) input  + inboffset);
    Int32 *tout0  = (Int32 *) ((char *) output + outboffset);
    Int32 net;
    Int32 temp1, temp2;
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Int32 *) ((char *) tin0 + inbstrides[dim]);
            net   = ufminimum(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _minimum_ixi_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int minimum_ixi_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _minimum_ixi_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(minimum_ixi_R, CHECK_ALIGN, sizeof(Int32), sizeof(Int32));

static void _minimum_ixi_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int32 *tin0   = (Int32 *) ((char *) input  + inboffset);
    Int32 *tout0 = (Int32 *) ((char *) output + outboffset);
    Int32 lastval;
    Int32 temp1, temp2;
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Int32 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Int32 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = ufminimum(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _minimum_ixi_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  minimum_ixi_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _minimum_ixi_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(minimum_ixi_A, CHECK_ALIGN, sizeof(Int32), sizeof(Int32));

static int minimum_iixi_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32       tin0 = *(Int32 *) buffers[0];
    Int32      *tin1 =  (Int32 *) buffers[1];
    Int32      *tout0 =  (Int32 *) buffers[2];
Int32 temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufminimum(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor minimum_iixi_svxf_descr =
{ "minimum_iixi_svxf", (void *) minimum_iixi_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Int32) }, { 1, 0, 0, 0 } };
/*********************  fabs  *********************/

static int fabs_ixd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = fabs(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor fabs_ixd_vxf_descr =
{ "fabs_ixd_vxf", (void *) fabs_ixd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int32), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  _round  *********************/

static int _round_ixd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_round(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor _round_ixd_vxf_descr =
{ "_round_ixd_vxf", (void *) _round_ixd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Int32), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  hypot  *********************/

static int hypot_iixd_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Int32       tin1 = *(Int32 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = hypot(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor hypot_iixd_vsxf_descr =
{ "hypot_iixd_vsxf", (void *) hypot_iixd_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Float64) }, { 0, 1, 0, 0 } };

static int hypot_iixd_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32      *tin0 =  (Int32 *) buffers[0];
    Int32      *tin1 =  (Int32 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = hypot(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor hypot_iixd_vvxf_descr =
{ "hypot_iixd_vvxf", (void *) hypot_iixd_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Float64) }, { 0, 0, 0, 0 } };

static void _hypot_ixd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int32  *tin0   = (Int32 *) ((char *) input  + inboffset);
    Float64 *tout0  = (Float64 *) ((char *) output + outboffset);
    Float64 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Int32 *) ((char *) tin0 + inbstrides[dim]);
            net   = hypot(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _hypot_ixd_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int hypot_ixd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _hypot_ixd_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(hypot_ixd_R, CHECK_ALIGN, sizeof(Int32), sizeof(Float64));

static void _hypot_ixd_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Int32 *tin0   = (Int32 *) ((char *) input  + inboffset);
    Float64 *tout0 = (Float64 *) ((char *) output + outboffset);
    Float64 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Int32 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Float64 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = hypot(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _hypot_ixd_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  hypot_ixd_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _hypot_ixd_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(hypot_ixd_A, CHECK_ALIGN, sizeof(Int32), sizeof(Float64));

static int hypot_iixd_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Int32       tin0 = *(Int32 *) buffers[0];
    Int32      *tin1 =  (Int32 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = hypot(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor hypot_iixd_svxf_descr =
{ "hypot_iixd_svxf", (void *) hypot_iixd_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Int32), sizeof(Int32), sizeof(Float64) }, { 1, 0, 0, 0 } };
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

static PyMethodDef _ufuncInt32Methods[] = {

	{NULL,      NULL}        /* Sentinel */
};

static PyObject *init_funcDict(void) {
    PyObject *dict, *keytuple, *cfunc;
    dict = PyDict_New();
    /* minus_ixi_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","minus","v","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minus_ixi_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_iixi_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","add","vs","Int32","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_iixi_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_iixi_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","add","vv","Int32","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_iixi_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_ixi_R */
    keytuple=Py_BuildValue("ss((s)(s))","add","R","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_ixi_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_ixi_A */
    keytuple=Py_BuildValue("ss((s)(s))","add","A","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_ixi_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_iixi_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","add","sv","Int32","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_iixi_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_iixi_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","subtract","vs","Int32","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_iixi_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_iixi_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","subtract","vv","Int32","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_iixi_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_ixi_R */
    keytuple=Py_BuildValue("ss((s)(s))","subtract","R","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_ixi_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_ixi_A */
    keytuple=Py_BuildValue("ss((s)(s))","subtract","A","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_ixi_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_iixi_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","subtract","sv","Int32","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_iixi_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* multiply_iixi_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","multiply","vs","Int32","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&multiply_iixi_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* multiply_iixi_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","multiply","vv","Int32","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&multiply_iixi_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* multiply_ixi_R */
    keytuple=Py_BuildValue("ss((s)(s))","multiply","R","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&multiply_ixi_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* multiply_ixi_A */
    keytuple=Py_BuildValue("ss((s)(s))","multiply","A","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&multiply_ixi_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* multiply_iixi_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","multiply","sv","Int32","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&multiply_iixi_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_iixi_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","divide","vs","Int32","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_iixi_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_iixi_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","divide","vv","Int32","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_iixi_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_ixi_R */
    keytuple=Py_BuildValue("ss((s)(s))","divide","R","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_ixi_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_ixi_A */
    keytuple=Py_BuildValue("ss((s)(s))","divide","A","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_ixi_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_iixi_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","divide","sv","Int32","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_iixi_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_divide_iixi_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","floor_divide","vs","Int32","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_divide_iixi_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_divide_iixi_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","floor_divide","vv","Int32","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_divide_iixi_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_divide_ixi_R */
    keytuple=Py_BuildValue("ss((s)(s))","floor_divide","R","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_divide_ixi_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_divide_ixi_A */
    keytuple=Py_BuildValue("ss((s)(s))","floor_divide","A","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_divide_ixi_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_divide_iixi_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","floor_divide","sv","Int32","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_divide_iixi_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_iixf_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","true_divide","vs","Int32","Int32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_iixf_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_iixf_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","true_divide","vv","Int32","Int32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_iixf_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_ixf_R */
    keytuple=Py_BuildValue("ss((s)(s))","true_divide","R","Int32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_ixf_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_ixf_A */
    keytuple=Py_BuildValue("ss((s)(s))","true_divide","A","Int32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_ixf_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_iixf_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","true_divide","sv","Int32","Int32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_iixf_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_iixi_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","remainder","vs","Int32","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_iixi_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_iixi_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","remainder","vv","Int32","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_iixi_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_ixi_R */
    keytuple=Py_BuildValue("ss((s)(s))","remainder","R","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_ixi_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_ixi_A */
    keytuple=Py_BuildValue("ss((s)(s))","remainder","A","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_ixi_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_iixi_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","remainder","sv","Int32","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_iixi_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_iixi_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","power","vs","Int32","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_iixi_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_iixi_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","power","vv","Int32","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_iixi_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_ixi_R */
    keytuple=Py_BuildValue("ss((s)(s))","power","R","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_ixi_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_ixi_A */
    keytuple=Py_BuildValue("ss((s)(s))","power","A","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_ixi_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_iixi_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","power","sv","Int32","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_iixi_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* abs_ixi_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","abs","v","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&abs_ixi_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* sin_ixd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","sin","v","Int32","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&sin_ixd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* cos_ixd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","cos","v","Int32","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&cos_ixd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* tan_ixd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","tan","v","Int32","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&tan_ixd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arcsin_ixd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arcsin","v","Int32","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arcsin_ixd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arccos_ixd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arccos","v","Int32","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arccos_ixd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan_ixd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arctan","v","Int32","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan_ixd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan2_iixd_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","arctan2","vs","Int32","Int32","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan2_iixd_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan2_iixd_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","arctan2","vv","Int32","Int32","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan2_iixd_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan2_ixd_R */
    keytuple=Py_BuildValue("ss((s)(s))","arctan2","R","Int32","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan2_ixd_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan2_ixd_A */
    keytuple=Py_BuildValue("ss((s)(s))","arctan2","A","Int32","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan2_ixd_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan2_iixd_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","arctan2","sv","Int32","Int32","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan2_iixd_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* log_ixd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","log","v","Int32","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&log_ixd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* log10_ixd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","log10","v","Int32","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&log10_ixd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* exp_ixd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","exp","v","Int32","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&exp_ixd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* sinh_ixd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","sinh","v","Int32","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&sinh_ixd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* cosh_ixd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","cosh","v","Int32","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&cosh_ixd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* tanh_ixd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","tanh","v","Int32","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&tanh_ixd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arcsinh_ixd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arcsinh","v","Int32","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arcsinh_ixd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arccosh_ixd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arccosh","v","Int32","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arccosh_ixd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctanh_ixd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arctanh","v","Int32","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctanh_ixd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* sqrt_ixd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","sqrt","v","Int32","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&sqrt_ixd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* equal_iixB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","equal","vs","Int32","Int32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&equal_iixB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* equal_iixB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","equal","vv","Int32","Int32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&equal_iixB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* equal_iixB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","equal","sv","Int32","Int32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&equal_iixB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* not_equal_iixB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","not_equal","vs","Int32","Int32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&not_equal_iixB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* not_equal_iixB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","not_equal","vv","Int32","Int32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&not_equal_iixB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* not_equal_iixB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","not_equal","sv","Int32","Int32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&not_equal_iixB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_iixB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater","vs","Int32","Int32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_iixB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_iixB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater","vv","Int32","Int32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_iixB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_iixB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater","sv","Int32","Int32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_iixB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_equal_iixB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater_equal","vs","Int32","Int32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_equal_iixB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_equal_iixB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater_equal","vv","Int32","Int32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_equal_iixB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_equal_iixB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater_equal","sv","Int32","Int32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_equal_iixB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_iixB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less","vs","Int32","Int32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_iixB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_iixB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less","vv","Int32","Int32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_iixB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_iixB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less","sv","Int32","Int32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_iixB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_equal_iixB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less_equal","vs","Int32","Int32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_equal_iixB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_equal_iixB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less_equal","vv","Int32","Int32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_equal_iixB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_equal_iixB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less_equal","sv","Int32","Int32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_equal_iixB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_and_iixB_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_and","vs","Int32","Int32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_and_iixB_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_and_iixB_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_and","vv","Int32","Int32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_and_iixB_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_and_iixB_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_and","sv","Int32","Int32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_and_iixB_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_or_iixB_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_or","vs","Int32","Int32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_or_iixB_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_or_iixB_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_or","vv","Int32","Int32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_or_iixB_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_or_iixB_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_or","sv","Int32","Int32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_or_iixB_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_xor_iixB_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_xor","vs","Int32","Int32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_xor_iixB_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_xor_iixB_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_xor","vv","Int32","Int32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_xor_iixB_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_xor_iixB_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_xor","sv","Int32","Int32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_xor_iixB_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_not_ixB_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","logical_not","v","Int32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_not_ixB_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_and_iixi_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_and","vs","Int32","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_and_iixi_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_and_iixi_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_and","vv","Int32","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_and_iixi_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_and_ixi_R */
    keytuple=Py_BuildValue("ss((s)(s))","bitwise_and","R","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_and_ixi_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_and_ixi_A */
    keytuple=Py_BuildValue("ss((s)(s))","bitwise_and","A","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_and_ixi_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_and_iixi_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_and","sv","Int32","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_and_iixi_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_or_iixi_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_or","vs","Int32","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_or_iixi_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_or_iixi_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_or","vv","Int32","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_or_iixi_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_or_ixi_R */
    keytuple=Py_BuildValue("ss((s)(s))","bitwise_or","R","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_or_ixi_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_or_ixi_A */
    keytuple=Py_BuildValue("ss((s)(s))","bitwise_or","A","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_or_ixi_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_or_iixi_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_or","sv","Int32","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_or_iixi_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_xor_iixi_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_xor","vs","Int32","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_xor_iixi_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_xor_iixi_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_xor","vv","Int32","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_xor_iixi_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_xor_ixi_R */
    keytuple=Py_BuildValue("ss((s)(s))","bitwise_xor","R","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_xor_ixi_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_xor_ixi_A */
    keytuple=Py_BuildValue("ss((s)(s))","bitwise_xor","A","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_xor_ixi_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_xor_iixi_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_xor","sv","Int32","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_xor_iixi_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_not_ixi_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","bitwise_not","v","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_not_ixi_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* rshift_iixi_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","rshift","vs","Int32","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&rshift_iixi_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* rshift_iixi_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","rshift","vv","Int32","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&rshift_iixi_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* rshift_iixi_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","rshift","sv","Int32","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&rshift_iixi_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* lshift_iixi_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","lshift","vs","Int32","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&lshift_iixi_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* lshift_iixi_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","lshift","vv","Int32","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&lshift_iixi_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* lshift_iixi_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","lshift","sv","Int32","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&lshift_iixi_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_ixi_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","floor","v","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_ixi_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* ceil_ixi_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","ceil","v","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&ceil_ixi_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_iixi_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","maximum","vs","Int32","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_iixi_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_iixi_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","maximum","vv","Int32","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_iixi_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_ixi_R */
    keytuple=Py_BuildValue("ss((s)(s))","maximum","R","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_ixi_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_ixi_A */
    keytuple=Py_BuildValue("ss((s)(s))","maximum","A","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_ixi_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_iixi_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","maximum","sv","Int32","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_iixi_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_iixi_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","minimum","vs","Int32","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_iixi_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_iixi_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","minimum","vv","Int32","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_iixi_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_ixi_R */
    keytuple=Py_BuildValue("ss((s)(s))","minimum","R","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_ixi_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_ixi_A */
    keytuple=Py_BuildValue("ss((s)(s))","minimum","A","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_ixi_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_iixi_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","minimum","sv","Int32","Int32","Int32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_iixi_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* fabs_ixd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","fabs","v","Int32","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&fabs_ixd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* _round_ixd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","_round","v","Int32","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&_round_ixd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_iixd_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","hypot","vs","Int32","Int32","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_iixd_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_iixd_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","hypot","vv","Int32","Int32","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_iixd_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_ixd_R */
    keytuple=Py_BuildValue("ss((s)(s))","hypot","R","Int32","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_ixd_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_ixd_A */
    keytuple=Py_BuildValue("ss((s)(s))","hypot","A","Int32","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_ixd_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_iixd_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","hypot","sv","Int32","Int32","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_iixd_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    return dict;
}

/* platform independent*/
#ifdef MS_WIN32
__declspec(dllexport)
#endif
void init_ufuncInt32(void) {
    PyObject *m, *d, *functions;
    m = Py_InitModule("_ufuncInt32", _ufuncInt32Methods);
    d = PyModule_GetDict(m);
    import_libnumarray();
    functions = init_funcDict();
    PyDict_SetItemString(d, "functionDict", functions);
    Py_DECREF(functions);
    ADD_VERSION(m);
}
