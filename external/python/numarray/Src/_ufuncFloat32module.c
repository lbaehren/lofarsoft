
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

static int _distance3d_fffxf_vvvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32    *tin1 =  (Float32 *) buffers[1];
    Float32    *tin2 =  (Float32 *) buffers[2];
    Float32    *tout0 =  (Float32 *) buffers[3];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = distance3d(*tin0, *tin1, *tin2);
        
	++tin0; ++tin1; ++tin2; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor _distance3d_fffxf_vvvxf_descr =
{ "_distance3d_fffxf_vvvxf", (void *) _distance3d_fffxf_vvvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 3, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Float32), sizeof(Float32) }, { 0, 0, 0, 0, 0 } };

static int _distance3d_fffxf_vssxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32     tin1 = *(Float32 *) buffers[1];
    Float32     tin2 = *(Float32 *) buffers[2];
    Float32    *tout0 =  (Float32 *) buffers[3];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = distance3d(*tin0, tin1, tin2);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor _distance3d_fffxf_vssxf_descr =
{ "_distance3d_fffxf_vssxf", (void *) _distance3d_fffxf_vssxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 3, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Float32), sizeof(Float32) }, { 0, 1, 1, 0, 0 } };
/*********************  minus  *********************/

static int minus_fxf_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32    *tout0 =  (Float32 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = -*tin0;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor minus_fxf_vxv_descr =
{ "minus_fxf_vxv", (void *) minus_fxf_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Float32), sizeof(Float32) }, { 0, 0, 0 } };
/*********************  add  *********************/

static int add_ffxf_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32     tin1 = *(Float32 *) buffers[1];
    Float32    *tout0 =  (Float32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 + tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor add_ffxf_vsxv_descr =
{ "add_ffxf_vsxv", (void *) add_ffxf_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Float32) }, { 0, 1, 0, 0 } };

static int add_ffxf_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32    *tin1 =  (Float32 *) buffers[1];
    Float32    *tout0 =  (Float32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 + *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor add_ffxf_vvxv_descr =
{ "add_ffxf_vvxv", (void *) add_ffxf_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Float32) }, { 0, 0, 0, 0 } };

static void _add_fxf_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Float32  *tin0   = (Float32 *) ((char *) input  + inboffset);
    Float32 *tout0  = (Float32 *) ((char *) output + outboffset);
    Float32 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Float32 *) ((char *) tin0 + inbstrides[dim]);
            net    =     net + *tin0;
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _add_fxf_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int add_fxf_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _add_fxf_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(add_fxf_R, CHECK_ALIGN, sizeof(Float32), sizeof(Float32));

static void _add_fxf_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Float32 *tin0   = (Float32 *) ((char *) input  + inboffset);
    Float32 *tout0 = (Float32 *) ((char *) output + outboffset);
    Float32 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Float32 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Float32 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = lastval + *tin0;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _add_fxf_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  add_fxf_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _add_fxf_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(add_fxf_A, CHECK_ALIGN, sizeof(Float32), sizeof(Float32));

static int add_ffxf_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32     tin0 = *(Float32 *) buffers[0];
    Float32    *tin1 =  (Float32 *) buffers[1];
    Float32    *tout0 =  (Float32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 + *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor add_ffxf_svxv_descr =
{ "add_ffxf_svxv", (void *) add_ffxf_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Float32) }, { 1, 0, 0, 0 } };
/*********************  subtract  *********************/

static int subtract_ffxf_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32     tin1 = *(Float32 *) buffers[1];
    Float32    *tout0 =  (Float32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 - tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor subtract_ffxf_vsxv_descr =
{ "subtract_ffxf_vsxv", (void *) subtract_ffxf_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Float32) }, { 0, 1, 0, 0 } };

static int subtract_ffxf_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32    *tin1 =  (Float32 *) buffers[1];
    Float32    *tout0 =  (Float32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 - *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor subtract_ffxf_vvxv_descr =
{ "subtract_ffxf_vvxv", (void *) subtract_ffxf_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Float32) }, { 0, 0, 0, 0 } };

static void _subtract_fxf_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Float32  *tin0   = (Float32 *) ((char *) input  + inboffset);
    Float32 *tout0  = (Float32 *) ((char *) output + outboffset);
    Float32 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Float32 *) ((char *) tin0 + inbstrides[dim]);
            net    =     net - *tin0;
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _subtract_fxf_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int subtract_fxf_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _subtract_fxf_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(subtract_fxf_R, CHECK_ALIGN, sizeof(Float32), sizeof(Float32));

static void _subtract_fxf_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Float32 *tin0   = (Float32 *) ((char *) input  + inboffset);
    Float32 *tout0 = (Float32 *) ((char *) output + outboffset);
    Float32 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Float32 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Float32 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = lastval - *tin0;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _subtract_fxf_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  subtract_fxf_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _subtract_fxf_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(subtract_fxf_A, CHECK_ALIGN, sizeof(Float32), sizeof(Float32));

static int subtract_ffxf_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32     tin0 = *(Float32 *) buffers[0];
    Float32    *tin1 =  (Float32 *) buffers[1];
    Float32    *tout0 =  (Float32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 - *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor subtract_ffxf_svxv_descr =
{ "subtract_ffxf_svxv", (void *) subtract_ffxf_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Float32) }, { 1, 0, 0, 0 } };
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/

static int multiply_ffxf_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32     tin1 = *(Float32 *) buffers[1];
    Float32    *tout0 =  (Float32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 * tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor multiply_ffxf_vsxv_descr =
{ "multiply_ffxf_vsxv", (void *) multiply_ffxf_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Float32) }, { 0, 1, 0, 0 } };

static int multiply_ffxf_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32    *tin1 =  (Float32 *) buffers[1];
    Float32    *tout0 =  (Float32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 * *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor multiply_ffxf_vvxv_descr =
{ "multiply_ffxf_vvxv", (void *) multiply_ffxf_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Float32) }, { 0, 0, 0, 0 } };

static void _multiply_fxf_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Float32  *tin0   = (Float32 *) ((char *) input  + inboffset);
    Float32 *tout0  = (Float32 *) ((char *) output + outboffset);
    Float32 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Float32 *) ((char *) tin0 + inbstrides[dim]);
            net    =     net * *tin0;
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _multiply_fxf_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int multiply_fxf_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _multiply_fxf_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(multiply_fxf_R, CHECK_ALIGN, sizeof(Float32), sizeof(Float32));

static void _multiply_fxf_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Float32 *tin0   = (Float32 *) ((char *) input  + inboffset);
    Float32 *tout0 = (Float32 *) ((char *) output + outboffset);
    Float32 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Float32 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Float32 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = lastval * *tin0;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _multiply_fxf_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  multiply_fxf_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _multiply_fxf_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(multiply_fxf_A, CHECK_ALIGN, sizeof(Float32), sizeof(Float32));

static int multiply_ffxf_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32     tin0 = *(Float32 *) buffers[0];
    Float32    *tin1 =  (Float32 *) buffers[1];
    Float32    *tout0 =  (Float32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 * *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor multiply_ffxf_svxv_descr =
{ "multiply_ffxf_svxv", (void *) multiply_ffxf_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Float32) }, { 1, 0, 0, 0 } };
/*********************  divide  *********************/
/*********************  divide  *********************/

static int divide_ffxf_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32     tin1 = *(Float32 *) buffers[1];
    Float32    *tout0 =  (Float32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 / tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor divide_ffxf_vsxv_descr =
{ "divide_ffxf_vsxv", (void *) divide_ffxf_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Float32) }, { 0, 1, 0, 0 } };

static int divide_ffxf_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32    *tin1 =  (Float32 *) buffers[1];
    Float32    *tout0 =  (Float32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 / *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor divide_ffxf_vvxv_descr =
{ "divide_ffxf_vvxv", (void *) divide_ffxf_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Float32) }, { 0, 0, 0, 0 } };

static void _divide_fxf_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Float32  *tin0   = (Float32 *) ((char *) input  + inboffset);
    Float32 *tout0  = (Float32 *) ((char *) output + outboffset);
    Float32 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Float32 *) ((char *) tin0 + inbstrides[dim]);
            net    =     net / *tin0;
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _divide_fxf_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int divide_fxf_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _divide_fxf_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(divide_fxf_R, CHECK_ALIGN, sizeof(Float32), sizeof(Float32));

static void _divide_fxf_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Float32 *tin0   = (Float32 *) ((char *) input  + inboffset);
    Float32 *tout0 = (Float32 *) ((char *) output + outboffset);
    Float32 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Float32 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Float32 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = lastval / *tin0;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _divide_fxf_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  divide_fxf_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _divide_fxf_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(divide_fxf_A, CHECK_ALIGN, sizeof(Float32), sizeof(Float32));

static int divide_ffxf_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32     tin0 = *(Float32 *) buffers[0];
    Float32    *tin1 =  (Float32 *) buffers[1];
    Float32    *tout0 =  (Float32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 / *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor divide_ffxf_svxv_descr =
{ "divide_ffxf_svxv", (void *) divide_ffxf_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Float32) }, { 1, 0, 0, 0 } };
/*********************  floor_divide  *********************/
/*********************  floor_divide  *********************/

static int floor_divide_ffxf_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32     tin1 = *(Float32 *) buffers[1];
    Float32    *tout0 =  (Float32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_FLOORDIVIDE(*tin0, tin1, *tout0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor floor_divide_ffxf_vsxv_descr =
{ "floor_divide_ffxf_vsxv", (void *) floor_divide_ffxf_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Float32) }, { 0, 1, 0, 0 } };

static int floor_divide_ffxf_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32    *tin1 =  (Float32 *) buffers[1];
    Float32    *tout0 =  (Float32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_FLOORDIVIDE(*tin0, *tin1, *tout0);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor floor_divide_ffxf_vvxv_descr =
{ "floor_divide_ffxf_vvxv", (void *) floor_divide_ffxf_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Float32) }, { 0, 0, 0, 0 } };

static void _floor_divide_fxf_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Float32  *tin0   = (Float32 *) ((char *) input  + inboffset);
    Float32 *tout0  = (Float32 *) ((char *) output + outboffset);
    Float32 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Float32 *) ((char *) tin0 + inbstrides[dim]);
            NUM_FLOORDIVIDE(net, *tin0, net);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _floor_divide_fxf_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int floor_divide_fxf_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _floor_divide_fxf_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(floor_divide_fxf_R, CHECK_ALIGN, sizeof(Float32), sizeof(Float32));

static void _floor_divide_fxf_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Float32 *tin0   = (Float32 *) ((char *) input  + inboffset);
    Float32 *tout0 = (Float32 *) ((char *) output + outboffset);
    Float32 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Float32 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Float32 *) ((char *) tout0 + outbstrides[dim]);
            NUM_FLOORDIVIDE(lastval ,*tin0, *tout0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _floor_divide_fxf_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  floor_divide_fxf_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _floor_divide_fxf_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(floor_divide_fxf_A, CHECK_ALIGN, sizeof(Float32), sizeof(Float32));

static int floor_divide_ffxf_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32     tin0 = *(Float32 *) buffers[0];
    Float32    *tin1 =  (Float32 *) buffers[1];
    Float32    *tout0 =  (Float32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_FLOORDIVIDE(tin0, *tin1, *tout0);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor floor_divide_ffxf_svxv_descr =
{ "floor_divide_ffxf_svxv", (void *) floor_divide_ffxf_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Float32) }, { 1, 0, 0, 0 } };
/*********************  true_divide  *********************/
/*********************  true_divide  *********************/

static int true_divide_ffxf_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32     tin1 = *(Float32 *) buffers[1];
    Float32    *tout0 =  (Float32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 / tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor true_divide_ffxf_vsxv_descr =
{ "true_divide_ffxf_vsxv", (void *) true_divide_ffxf_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Float32) }, { 0, 1, 0, 0 } };

static int true_divide_ffxf_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32    *tin1 =  (Float32 *) buffers[1];
    Float32    *tout0 =  (Float32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 / *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor true_divide_ffxf_vvxv_descr =
{ "true_divide_ffxf_vvxv", (void *) true_divide_ffxf_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Float32) }, { 0, 0, 0, 0 } };

static void _true_divide_fxf_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Float32  *tin0   = (Float32 *) ((char *) input  + inboffset);
    Float32 *tout0  = (Float32 *) ((char *) output + outboffset);
    Float32 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Float32 *) ((char *) tin0 + inbstrides[dim]);
            net    =     net / *tin0;
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _true_divide_fxf_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int true_divide_fxf_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _true_divide_fxf_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(true_divide_fxf_R, CHECK_ALIGN, sizeof(Float32), sizeof(Float32));

static void _true_divide_fxf_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Float32 *tin0   = (Float32 *) ((char *) input  + inboffset);
    Float32 *tout0 = (Float32 *) ((char *) output + outboffset);
    Float32 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Float32 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Float32 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = lastval / *tin0;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _true_divide_fxf_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  true_divide_fxf_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _true_divide_fxf_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(true_divide_fxf_A, CHECK_ALIGN, sizeof(Float32), sizeof(Float32));

static int true_divide_ffxf_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32     tin0 = *(Float32 *) buffers[0];
    Float32    *tin1 =  (Float32 *) buffers[1];
    Float32    *tout0 =  (Float32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 / *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor true_divide_ffxf_svxv_descr =
{ "true_divide_ffxf_svxv", (void *) true_divide_ffxf_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Float32) }, { 1, 0, 0, 0 } };
/*********************  remainder  *********************/
/*********************  remainder  *********************/

static int remainder_ffxf_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32     tin1 = *(Float32 *) buffers[1];
    Float32    *tout0 =  (Float32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = fmod(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor remainder_ffxf_vsxf_descr =
{ "remainder_ffxf_vsxf", (void *) remainder_ffxf_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Float32) }, { 0, 1, 0, 0 } };

static int remainder_ffxf_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32    *tin1 =  (Float32 *) buffers[1];
    Float32    *tout0 =  (Float32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = fmod(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor remainder_ffxf_vvxf_descr =
{ "remainder_ffxf_vvxf", (void *) remainder_ffxf_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Float32) }, { 0, 0, 0, 0 } };

static void _remainder_fxf_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Float32  *tin0   = (Float32 *) ((char *) input  + inboffset);
    Float32 *tout0  = (Float32 *) ((char *) output + outboffset);
    Float32 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Float32 *) ((char *) tin0 + inbstrides[dim]);
            net   = fmod(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _remainder_fxf_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int remainder_fxf_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _remainder_fxf_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(remainder_fxf_R, CHECK_ALIGN, sizeof(Float32), sizeof(Float32));

static void _remainder_fxf_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Float32 *tin0   = (Float32 *) ((char *) input  + inboffset);
    Float32 *tout0 = (Float32 *) ((char *) output + outboffset);
    Float32 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Float32 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Float32 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = fmod(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _remainder_fxf_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  remainder_fxf_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _remainder_fxf_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(remainder_fxf_A, CHECK_ALIGN, sizeof(Float32), sizeof(Float32));

static int remainder_ffxf_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32     tin0 = *(Float32 *) buffers[0];
    Float32    *tin1 =  (Float32 *) buffers[1];
    Float32    *tout0 =  (Float32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = fmod(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor remainder_ffxf_svxf_descr =
{ "remainder_ffxf_svxf", (void *) remainder_ffxf_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Float32) }, { 1, 0, 0, 0 } };
/*********************  power  *********************/

static int power_ffxf_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32     tin1 = *(Float32 *) buffers[1];
    Float32    *tout0 =  (Float32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_pow(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor power_ffxf_vsxf_descr =
{ "power_ffxf_vsxf", (void *) power_ffxf_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Float32) }, { 0, 1, 0, 0 } };

static int power_ffxf_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32    *tin1 =  (Float32 *) buffers[1];
    Float32    *tout0 =  (Float32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_pow(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor power_ffxf_vvxf_descr =
{ "power_ffxf_vvxf", (void *) power_ffxf_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Float32) }, { 0, 0, 0, 0 } };

static void _power_fxf_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Float32  *tin0   = (Float32 *) ((char *) input  + inboffset);
    Float32 *tout0  = (Float32 *) ((char *) output + outboffset);
    Float32 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Float32 *) ((char *) tin0 + inbstrides[dim]);
            net   = num_pow(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _power_fxf_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int power_fxf_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _power_fxf_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(power_fxf_R, CHECK_ALIGN, sizeof(Float32), sizeof(Float32));

static void _power_fxf_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Float32 *tin0   = (Float32 *) ((char *) input  + inboffset);
    Float32 *tout0 = (Float32 *) ((char *) output + outboffset);
    Float32 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Float32 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Float32 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = num_pow(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _power_fxf_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  power_fxf_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _power_fxf_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(power_fxf_A, CHECK_ALIGN, sizeof(Float32), sizeof(Float32));

static int power_ffxf_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32     tin0 = *(Float32 *) buffers[0];
    Float32    *tin1 =  (Float32 *) buffers[1];
    Float32    *tout0 =  (Float32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_pow(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor power_ffxf_svxf_descr =
{ "power_ffxf_svxf", (void *) power_ffxf_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Float32) }, { 1, 0, 0, 0 } };
/*********************  abs  *********************/

static int abs_fxf_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32    *tout0 =  (Float32 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = fabs(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor abs_fxf_vxf_descr =
{ "abs_fxf_vxf", (void *) abs_fxf_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Float32), sizeof(Float32) }, { 0, 0, 0 } };
/*********************  sin  *********************/

static int sin_fxf_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32    *tout0 =  (Float32 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = sin(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor sin_fxf_vxf_descr =
{ "sin_fxf_vxf", (void *) sin_fxf_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Float32), sizeof(Float32) }, { 0, 0, 0 } };
/*********************  cos  *********************/

static int cos_fxf_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32    *tout0 =  (Float32 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = cos(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor cos_fxf_vxf_descr =
{ "cos_fxf_vxf", (void *) cos_fxf_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Float32), sizeof(Float32) }, { 0, 0, 0 } };
/*********************  tan  *********************/

static int tan_fxf_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32    *tout0 =  (Float32 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = tan(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor tan_fxf_vxf_descr =
{ "tan_fxf_vxf", (void *) tan_fxf_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Float32), sizeof(Float32) }, { 0, 0, 0 } };
/*********************  arcsin  *********************/

static int arcsin_fxf_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32    *tout0 =  (Float32 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = asin(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arcsin_fxf_vxf_descr =
{ "arcsin_fxf_vxf", (void *) arcsin_fxf_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Float32), sizeof(Float32) }, { 0, 0, 0 } };
/*********************  arccos  *********************/

static int arccos_fxf_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32    *tout0 =  (Float32 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = acos(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arccos_fxf_vxf_descr =
{ "arccos_fxf_vxf", (void *) arccos_fxf_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Float32), sizeof(Float32) }, { 0, 0, 0 } };
/*********************  arctan  *********************/

static int arctan_fxf_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32    *tout0 =  (Float32 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = atan(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arctan_fxf_vxf_descr =
{ "arctan_fxf_vxf", (void *) arctan_fxf_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Float32), sizeof(Float32) }, { 0, 0, 0 } };
/*********************  arctan2  *********************/

static int arctan2_ffxf_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32     tin1 = *(Float32 *) buffers[1];
    Float32    *tout0 =  (Float32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = atan2(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arctan2_ffxf_vsxf_descr =
{ "arctan2_ffxf_vsxf", (void *) arctan2_ffxf_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Float32) }, { 0, 1, 0, 0 } };

static int arctan2_ffxf_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32    *tin1 =  (Float32 *) buffers[1];
    Float32    *tout0 =  (Float32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = atan2(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arctan2_ffxf_vvxf_descr =
{ "arctan2_ffxf_vvxf", (void *) arctan2_ffxf_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Float32) }, { 0, 0, 0, 0 } };

static void _arctan2_fxf_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Float32  *tin0   = (Float32 *) ((char *) input  + inboffset);
    Float32 *tout0  = (Float32 *) ((char *) output + outboffset);
    Float32 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Float32 *) ((char *) tin0 + inbstrides[dim]);
            net   = atan2(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _arctan2_fxf_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int arctan2_fxf_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _arctan2_fxf_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(arctan2_fxf_R, CHECK_ALIGN, sizeof(Float32), sizeof(Float32));

static void _arctan2_fxf_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Float32 *tin0   = (Float32 *) ((char *) input  + inboffset);
    Float32 *tout0 = (Float32 *) ((char *) output + outboffset);
    Float32 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Float32 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Float32 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = atan2(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _arctan2_fxf_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  arctan2_fxf_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _arctan2_fxf_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(arctan2_fxf_A, CHECK_ALIGN, sizeof(Float32), sizeof(Float32));

static int arctan2_ffxf_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32     tin0 = *(Float32 *) buffers[0];
    Float32    *tin1 =  (Float32 *) buffers[1];
    Float32    *tout0 =  (Float32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = atan2(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arctan2_ffxf_svxf_descr =
{ "arctan2_ffxf_svxf", (void *) arctan2_ffxf_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Float32) }, { 1, 0, 0, 0 } };
/*********************  log  *********************/

static int log_fxf_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32    *tout0 =  (Float32 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_log(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor log_fxf_vxf_descr =
{ "log_fxf_vxf", (void *) log_fxf_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Float32), sizeof(Float32) }, { 0, 0, 0 } };
/*********************  log10  *********************/

static int log10_fxf_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32    *tout0 =  (Float32 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_log10(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor log10_fxf_vxf_descr =
{ "log10_fxf_vxf", (void *) log10_fxf_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Float32), sizeof(Float32) }, { 0, 0, 0 } };
/*********************  exp  *********************/

static int exp_fxf_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32    *tout0 =  (Float32 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = exp(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor exp_fxf_vxf_descr =
{ "exp_fxf_vxf", (void *) exp_fxf_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Float32), sizeof(Float32) }, { 0, 0, 0 } };
/*********************  sinh  *********************/

static int sinh_fxf_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32    *tout0 =  (Float32 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = sinh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor sinh_fxf_vxf_descr =
{ "sinh_fxf_vxf", (void *) sinh_fxf_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Float32), sizeof(Float32) }, { 0, 0, 0 } };
/*********************  cosh  *********************/

static int cosh_fxf_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32    *tout0 =  (Float32 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = cosh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor cosh_fxf_vxf_descr =
{ "cosh_fxf_vxf", (void *) cosh_fxf_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Float32), sizeof(Float32) }, { 0, 0, 0 } };
/*********************  tanh  *********************/

static int tanh_fxf_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32    *tout0 =  (Float32 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = tanh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor tanh_fxf_vxf_descr =
{ "tanh_fxf_vxf", (void *) tanh_fxf_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Float32), sizeof(Float32) }, { 0, 0, 0 } };
/*********************  arcsinh  *********************/

static int arcsinh_fxf_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32    *tout0 =  (Float32 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_asinh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arcsinh_fxf_vxf_descr =
{ "arcsinh_fxf_vxf", (void *) arcsinh_fxf_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Float32), sizeof(Float32) }, { 0, 0, 0 } };
/*********************  arccosh  *********************/

static int arccosh_fxf_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32    *tout0 =  (Float32 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_acosh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arccosh_fxf_vxf_descr =
{ "arccosh_fxf_vxf", (void *) arccosh_fxf_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Float32), sizeof(Float32) }, { 0, 0, 0 } };
/*********************  arctanh  *********************/

static int arctanh_fxf_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32    *tout0 =  (Float32 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_atanh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arctanh_fxf_vxf_descr =
{ "arctanh_fxf_vxf", (void *) arctanh_fxf_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Float32), sizeof(Float32) }, { 0, 0, 0 } };
/*********************  ieeemask  *********************/

static int ieeemask_fixB_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Int32       tin1 = *(Int32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NA_IeeeMask32(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor ieeemask_fixB_vsxf_descr =
{ "ieeemask_fixB_vsxf", (void *) ieeemask_fixB_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Int32), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int ieeemask_fixB_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Int32      *tin1 =  (Int32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NA_IeeeMask32(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor ieeemask_fixB_vvxf_descr =
{ "ieeemask_fixB_vvxf", (void *) ieeemask_fixB_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Int32), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int ieeemask_fixB_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32     tin0 = *(Float32 *) buffers[0];
    Int32      *tin1 =  (Int32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NA_IeeeMask32(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor ieeemask_fixB_svxf_descr =
{ "ieeemask_fixB_svxf", (void *) ieeemask_fixB_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Int32), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  ieeemask  *********************/
/*********************  isnan  *********************/

static int isnan_fxB_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Bool       *tout0 =  (Bool *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NA_isnan32(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor isnan_fxB_vxf_descr =
{ "isnan_fxB_vxf", (void *) isnan_fxB_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Float32), sizeof(Bool) }, { 0, 0, 0 } };
/*********************  isnan  *********************/
/*********************  isnan  *********************/
/*********************  isnan  *********************/
/*********************  sqrt  *********************/

static int sqrt_fxf_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32    *tout0 =  (Float32 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = sqrt(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor sqrt_fxf_vxf_descr =
{ "sqrt_fxf_vxf", (void *) sqrt_fxf_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Float32), sizeof(Float32) }, { 0, 0, 0 } };
/*********************  equal  *********************/

static int equal_ffxB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32     tin1 = *(Float32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 == tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor equal_ffxB_vsxv_descr =
{ "equal_ffxB_vsxv", (void *) equal_ffxB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int equal_ffxB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32    *tin1 =  (Float32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 == *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor equal_ffxB_vvxv_descr =
{ "equal_ffxB_vvxv", (void *) equal_ffxB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int equal_ffxB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32     tin0 = *(Float32 *) buffers[0];
    Float32    *tin1 =  (Float32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 == *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor equal_ffxB_svxv_descr =
{ "equal_ffxB_svxv", (void *) equal_ffxB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  not_equal  *********************/

static int not_equal_ffxB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32     tin1 = *(Float32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 != tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor not_equal_ffxB_vsxv_descr =
{ "not_equal_ffxB_vsxv", (void *) not_equal_ffxB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int not_equal_ffxB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32    *tin1 =  (Float32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 != *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor not_equal_ffxB_vvxv_descr =
{ "not_equal_ffxB_vvxv", (void *) not_equal_ffxB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int not_equal_ffxB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32     tin0 = *(Float32 *) buffers[0];
    Float32    *tin1 =  (Float32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 != *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor not_equal_ffxB_svxv_descr =
{ "not_equal_ffxB_svxv", (void *) not_equal_ffxB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  greater  *********************/

static int greater_ffxB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32     tin1 = *(Float32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 > tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_ffxB_vsxv_descr =
{ "greater_ffxB_vsxv", (void *) greater_ffxB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int greater_ffxB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32    *tin1 =  (Float32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 > *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_ffxB_vvxv_descr =
{ "greater_ffxB_vvxv", (void *) greater_ffxB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int greater_ffxB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32     tin0 = *(Float32 *) buffers[0];
    Float32    *tin1 =  (Float32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 > *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_ffxB_svxv_descr =
{ "greater_ffxB_svxv", (void *) greater_ffxB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  greater_equal  *********************/

static int greater_equal_ffxB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32     tin1 = *(Float32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 >= tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_equal_ffxB_vsxv_descr =
{ "greater_equal_ffxB_vsxv", (void *) greater_equal_ffxB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int greater_equal_ffxB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32    *tin1 =  (Float32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 >= *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_equal_ffxB_vvxv_descr =
{ "greater_equal_ffxB_vvxv", (void *) greater_equal_ffxB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int greater_equal_ffxB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32     tin0 = *(Float32 *) buffers[0];
    Float32    *tin1 =  (Float32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 >= *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_equal_ffxB_svxv_descr =
{ "greater_equal_ffxB_svxv", (void *) greater_equal_ffxB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  less  *********************/

static int less_ffxB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32     tin1 = *(Float32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 < tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_ffxB_vsxv_descr =
{ "less_ffxB_vsxv", (void *) less_ffxB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int less_ffxB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32    *tin1 =  (Float32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 < *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_ffxB_vvxv_descr =
{ "less_ffxB_vvxv", (void *) less_ffxB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int less_ffxB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32     tin0 = *(Float32 *) buffers[0];
    Float32    *tin1 =  (Float32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 < *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_ffxB_svxv_descr =
{ "less_ffxB_svxv", (void *) less_ffxB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  less_equal  *********************/

static int less_equal_ffxB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32     tin1 = *(Float32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 <= tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_equal_ffxB_vsxv_descr =
{ "less_equal_ffxB_vsxv", (void *) less_equal_ffxB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int less_equal_ffxB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32    *tin1 =  (Float32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 <= *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_equal_ffxB_vvxv_descr =
{ "less_equal_ffxB_vvxv", (void *) less_equal_ffxB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int less_equal_ffxB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32     tin0 = *(Float32 *) buffers[0];
    Float32    *tin1 =  (Float32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 <= *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_equal_ffxB_svxv_descr =
{ "less_equal_ffxB_svxv", (void *) less_equal_ffxB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  logical_and  *********************/

static int logical_and_ffxB_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32     tin1 = *(Float32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_and(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_and_ffxB_vsxf_descr =
{ "logical_and_ffxB_vsxf", (void *) logical_and_ffxB_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int logical_and_ffxB_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32    *tin1 =  (Float32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_and(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_and_ffxB_vvxf_descr =
{ "logical_and_ffxB_vvxf", (void *) logical_and_ffxB_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int logical_and_ffxB_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32     tin0 = *(Float32 *) buffers[0];
    Float32    *tin1 =  (Float32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_and(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_and_ffxB_svxf_descr =
{ "logical_and_ffxB_svxf", (void *) logical_and_ffxB_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  logical_or  *********************/

static int logical_or_ffxB_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32     tin1 = *(Float32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_or(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_or_ffxB_vsxf_descr =
{ "logical_or_ffxB_vsxf", (void *) logical_or_ffxB_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int logical_or_ffxB_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32    *tin1 =  (Float32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_or(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_or_ffxB_vvxf_descr =
{ "logical_or_ffxB_vvxf", (void *) logical_or_ffxB_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int logical_or_ffxB_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32     tin0 = *(Float32 *) buffers[0];
    Float32    *tin1 =  (Float32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_or(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_or_ffxB_svxf_descr =
{ "logical_or_ffxB_svxf", (void *) logical_or_ffxB_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  logical_xor  *********************/

static int logical_xor_ffxB_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32     tin1 = *(Float32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_xor(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_xor_ffxB_vsxf_descr =
{ "logical_xor_ffxB_vsxf", (void *) logical_xor_ffxB_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int logical_xor_ffxB_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32    *tin1 =  (Float32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_xor(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_xor_ffxB_vvxf_descr =
{ "logical_xor_ffxB_vvxf", (void *) logical_xor_ffxB_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int logical_xor_ffxB_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32     tin0 = *(Float32 *) buffers[0];
    Float32    *tin1 =  (Float32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_xor(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_xor_ffxB_svxf_descr =
{ "logical_xor_ffxB_svxf", (void *) logical_xor_ffxB_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  logical_and  *********************/
/*********************  logical_or  *********************/
/*********************  logical_xor  *********************/
/*********************  logical_not  *********************/

static int logical_not_fxB_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Bool       *tout0 =  (Bool *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = !*tin0;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_not_fxB_vxv_descr =
{ "logical_not_fxB_vxv", (void *) logical_not_fxB_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Float32), sizeof(Bool) }, { 0, 0, 0 } };
/*********************  bitwise_and  *********************/
/*********************  bitwise_or  *********************/
/*********************  bitwise_xor  *********************/
/*********************  bitwise_not  *********************/
/*********************  bitwise_not  *********************/
/*********************  rshift  *********************/
/*********************  lshift  *********************/
/*********************  floor  *********************/
/*********************  floor  *********************/

static int floor_fxf_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32    *tout0 =  (Float32 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = floor(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor floor_fxf_vxf_descr =
{ "floor_fxf_vxf", (void *) floor_fxf_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Float32), sizeof(Float32) }, { 0, 0, 0 } };
/*********************  ceil  *********************/
/*********************  ceil  *********************/

static int ceil_fxf_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32    *tout0 =  (Float32 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ceil(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor ceil_fxf_vxf_descr =
{ "ceil_fxf_vxf", (void *) ceil_fxf_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Float32), sizeof(Float32) }, { 0, 0, 0 } };
/*********************  maximum  *********************/

static int maximum_ffxf_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32     tin1 = *(Float32 *) buffers[1];
    Float32    *tout0 =  (Float32 *) buffers[2];
Float32 temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufmaximum(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor maximum_ffxf_vsxf_descr =
{ "maximum_ffxf_vsxf", (void *) maximum_ffxf_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Float32) }, { 0, 1, 0, 0 } };

static int maximum_ffxf_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32    *tin1 =  (Float32 *) buffers[1];
    Float32    *tout0 =  (Float32 *) buffers[2];
Float32 temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufmaximum(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor maximum_ffxf_vvxf_descr =
{ "maximum_ffxf_vvxf", (void *) maximum_ffxf_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Float32) }, { 0, 0, 0, 0 } };

static void _maximum_fxf_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Float32  *tin0   = (Float32 *) ((char *) input  + inboffset);
    Float32 *tout0  = (Float32 *) ((char *) output + outboffset);
    Float32 net;
    Float32 temp1, temp2;
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Float32 *) ((char *) tin0 + inbstrides[dim]);
            net   = ufmaximum(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _maximum_fxf_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int maximum_fxf_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _maximum_fxf_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(maximum_fxf_R, CHECK_ALIGN, sizeof(Float32), sizeof(Float32));

static void _maximum_fxf_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Float32 *tin0   = (Float32 *) ((char *) input  + inboffset);
    Float32 *tout0 = (Float32 *) ((char *) output + outboffset);
    Float32 lastval;
    Float32 temp1, temp2;
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Float32 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Float32 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = ufmaximum(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _maximum_fxf_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  maximum_fxf_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _maximum_fxf_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(maximum_fxf_A, CHECK_ALIGN, sizeof(Float32), sizeof(Float32));

static int maximum_ffxf_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32     tin0 = *(Float32 *) buffers[0];
    Float32    *tin1 =  (Float32 *) buffers[1];
    Float32    *tout0 =  (Float32 *) buffers[2];
Float32 temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufmaximum(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor maximum_ffxf_svxf_descr =
{ "maximum_ffxf_svxf", (void *) maximum_ffxf_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Float32) }, { 1, 0, 0, 0 } };
/*********************  minimum  *********************/

static int minimum_ffxf_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32     tin1 = *(Float32 *) buffers[1];
    Float32    *tout0 =  (Float32 *) buffers[2];
Float32 temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufminimum(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor minimum_ffxf_vsxf_descr =
{ "minimum_ffxf_vsxf", (void *) minimum_ffxf_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Float32) }, { 0, 1, 0, 0 } };

static int minimum_ffxf_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32    *tin1 =  (Float32 *) buffers[1];
    Float32    *tout0 =  (Float32 *) buffers[2];
Float32 temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufminimum(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor minimum_ffxf_vvxf_descr =
{ "minimum_ffxf_vvxf", (void *) minimum_ffxf_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Float32) }, { 0, 0, 0, 0 } };

static void _minimum_fxf_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Float32  *tin0   = (Float32 *) ((char *) input  + inboffset);
    Float32 *tout0  = (Float32 *) ((char *) output + outboffset);
    Float32 net;
    Float32 temp1, temp2;
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Float32 *) ((char *) tin0 + inbstrides[dim]);
            net   = ufminimum(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _minimum_fxf_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int minimum_fxf_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _minimum_fxf_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(minimum_fxf_R, CHECK_ALIGN, sizeof(Float32), sizeof(Float32));

static void _minimum_fxf_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Float32 *tin0   = (Float32 *) ((char *) input  + inboffset);
    Float32 *tout0 = (Float32 *) ((char *) output + outboffset);
    Float32 lastval;
    Float32 temp1, temp2;
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Float32 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Float32 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = ufminimum(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _minimum_fxf_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  minimum_fxf_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _minimum_fxf_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(minimum_fxf_A, CHECK_ALIGN, sizeof(Float32), sizeof(Float32));

static int minimum_ffxf_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32     tin0 = *(Float32 *) buffers[0];
    Float32    *tin1 =  (Float32 *) buffers[1];
    Float32    *tout0 =  (Float32 *) buffers[2];
Float32 temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufminimum(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor minimum_ffxf_svxf_descr =
{ "minimum_ffxf_svxf", (void *) minimum_ffxf_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Float32) }, { 1, 0, 0, 0 } };
/*********************  fabs  *********************/

static int fabs_fxf_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32    *tout0 =  (Float32 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = fabs(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor fabs_fxf_vxf_descr =
{ "fabs_fxf_vxf", (void *) fabs_fxf_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Float32), sizeof(Float32) }, { 0, 0, 0 } };
/*********************  _round  *********************/

static int _round_fxf_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32    *tout0 =  (Float32 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_round(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor _round_fxf_vxf_descr =
{ "_round_fxf_vxf", (void *) _round_fxf_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Float32), sizeof(Float32) }, { 0, 0, 0 } };
/*********************  hypot  *********************/

static int hypot_ffxf_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32     tin1 = *(Float32 *) buffers[1];
    Float32    *tout0 =  (Float32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = hypot(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor hypot_ffxf_vsxf_descr =
{ "hypot_ffxf_vsxf", (void *) hypot_ffxf_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Float32) }, { 0, 1, 0, 0 } };

static int hypot_ffxf_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32    *tin1 =  (Float32 *) buffers[1];
    Float32    *tout0 =  (Float32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = hypot(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor hypot_ffxf_vvxf_descr =
{ "hypot_ffxf_vvxf", (void *) hypot_ffxf_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Float32) }, { 0, 0, 0, 0 } };

static void _hypot_fxf_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Float32  *tin0   = (Float32 *) ((char *) input  + inboffset);
    Float32 *tout0  = (Float32 *) ((char *) output + outboffset);
    Float32 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Float32 *) ((char *) tin0 + inbstrides[dim]);
            net   = hypot(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _hypot_fxf_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int hypot_fxf_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _hypot_fxf_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(hypot_fxf_R, CHECK_ALIGN, sizeof(Float32), sizeof(Float32));

static void _hypot_fxf_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Float32 *tin0   = (Float32 *) ((char *) input  + inboffset);
    Float32 *tout0 = (Float32 *) ((char *) output + outboffset);
    Float32 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Float32 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Float32 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = hypot(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _hypot_fxf_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  hypot_fxf_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _hypot_fxf_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(hypot_fxf_A, CHECK_ALIGN, sizeof(Float32), sizeof(Float32));

static int hypot_ffxf_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32     tin0 = *(Float32 *) buffers[0];
    Float32    *tin1 =  (Float32 *) buffers[1];
    Float32    *tout0 =  (Float32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = hypot(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor hypot_ffxf_svxf_descr =
{ "hypot_ffxf_svxf", (void *) hypot_ffxf_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Float32) }, { 1, 0, 0, 0 } };
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

static PyMethodDef _ufuncFloat32Methods[] = {

	{NULL,      NULL}        /* Sentinel */
};

static PyObject *init_funcDict(void) {
    PyObject *dict, *keytuple, *cfunc;
    dict = PyDict_New();
    /* _distance3d_fffxf_vvvxf */
    keytuple=Py_BuildValue("ss((sss)(s))","_distance3d","vvv","Float32","Float32","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&_distance3d_fffxf_vvvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* _distance3d_fffxf_vssxf */
    keytuple=Py_BuildValue("ss((sss)(s))","_distance3d","vss","Float32","Float32","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&_distance3d_fffxf_vssxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minus_fxf_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","minus","v","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minus_fxf_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_ffxf_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","add","vs","Float32","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_ffxf_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_ffxf_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","add","vv","Float32","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_ffxf_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_fxf_R */
    keytuple=Py_BuildValue("ss((s)(s))","add","R","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_fxf_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_fxf_A */
    keytuple=Py_BuildValue("ss((s)(s))","add","A","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_fxf_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_ffxf_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","add","sv","Float32","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_ffxf_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_ffxf_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","subtract","vs","Float32","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_ffxf_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_ffxf_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","subtract","vv","Float32","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_ffxf_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_fxf_R */
    keytuple=Py_BuildValue("ss((s)(s))","subtract","R","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_fxf_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_fxf_A */
    keytuple=Py_BuildValue("ss((s)(s))","subtract","A","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_fxf_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_ffxf_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","subtract","sv","Float32","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_ffxf_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* multiply_ffxf_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","multiply","vs","Float32","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&multiply_ffxf_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* multiply_ffxf_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","multiply","vv","Float32","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&multiply_ffxf_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* multiply_fxf_R */
    keytuple=Py_BuildValue("ss((s)(s))","multiply","R","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&multiply_fxf_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* multiply_fxf_A */
    keytuple=Py_BuildValue("ss((s)(s))","multiply","A","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&multiply_fxf_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* multiply_ffxf_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","multiply","sv","Float32","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&multiply_ffxf_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_ffxf_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","divide","vs","Float32","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_ffxf_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_ffxf_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","divide","vv","Float32","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_ffxf_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_fxf_R */
    keytuple=Py_BuildValue("ss((s)(s))","divide","R","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_fxf_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_fxf_A */
    keytuple=Py_BuildValue("ss((s)(s))","divide","A","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_fxf_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_ffxf_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","divide","sv","Float32","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_ffxf_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_divide_ffxf_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","floor_divide","vs","Float32","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_divide_ffxf_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_divide_ffxf_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","floor_divide","vv","Float32","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_divide_ffxf_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_divide_fxf_R */
    keytuple=Py_BuildValue("ss((s)(s))","floor_divide","R","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_divide_fxf_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_divide_fxf_A */
    keytuple=Py_BuildValue("ss((s)(s))","floor_divide","A","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_divide_fxf_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_divide_ffxf_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","floor_divide","sv","Float32","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_divide_ffxf_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_ffxf_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","true_divide","vs","Float32","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_ffxf_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_ffxf_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","true_divide","vv","Float32","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_ffxf_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_fxf_R */
    keytuple=Py_BuildValue("ss((s)(s))","true_divide","R","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_fxf_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_fxf_A */
    keytuple=Py_BuildValue("ss((s)(s))","true_divide","A","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_fxf_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_ffxf_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","true_divide","sv","Float32","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_ffxf_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_ffxf_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","remainder","vs","Float32","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_ffxf_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_ffxf_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","remainder","vv","Float32","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_ffxf_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_fxf_R */
    keytuple=Py_BuildValue("ss((s)(s))","remainder","R","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_fxf_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_fxf_A */
    keytuple=Py_BuildValue("ss((s)(s))","remainder","A","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_fxf_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_ffxf_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","remainder","sv","Float32","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_ffxf_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_ffxf_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","power","vs","Float32","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_ffxf_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_ffxf_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","power","vv","Float32","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_ffxf_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_fxf_R */
    keytuple=Py_BuildValue("ss((s)(s))","power","R","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_fxf_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_fxf_A */
    keytuple=Py_BuildValue("ss((s)(s))","power","A","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_fxf_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_ffxf_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","power","sv","Float32","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_ffxf_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* abs_fxf_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","abs","v","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&abs_fxf_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* sin_fxf_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","sin","v","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&sin_fxf_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* cos_fxf_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","cos","v","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&cos_fxf_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* tan_fxf_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","tan","v","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&tan_fxf_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arcsin_fxf_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arcsin","v","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arcsin_fxf_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arccos_fxf_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arccos","v","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arccos_fxf_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan_fxf_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arctan","v","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan_fxf_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan2_ffxf_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","arctan2","vs","Float32","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan2_ffxf_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan2_ffxf_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","arctan2","vv","Float32","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan2_ffxf_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan2_fxf_R */
    keytuple=Py_BuildValue("ss((s)(s))","arctan2","R","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan2_fxf_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan2_fxf_A */
    keytuple=Py_BuildValue("ss((s)(s))","arctan2","A","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan2_fxf_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan2_ffxf_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","arctan2","sv","Float32","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan2_ffxf_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* log_fxf_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","log","v","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&log_fxf_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* log10_fxf_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","log10","v","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&log10_fxf_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* exp_fxf_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","exp","v","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&exp_fxf_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* sinh_fxf_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","sinh","v","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&sinh_fxf_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* cosh_fxf_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","cosh","v","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&cosh_fxf_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* tanh_fxf_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","tanh","v","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&tanh_fxf_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arcsinh_fxf_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arcsinh","v","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arcsinh_fxf_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arccosh_fxf_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arccosh","v","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arccosh_fxf_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctanh_fxf_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arctanh","v","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctanh_fxf_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* ieeemask_fixB_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","ieeemask","vs","Float32","Int32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&ieeemask_fixB_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* ieeemask_fixB_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","ieeemask","vv","Float32","Int32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&ieeemask_fixB_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* ieeemask_fixB_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","ieeemask","sv","Float32","Int32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&ieeemask_fixB_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* isnan_fxB_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","isnan","v","Float32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&isnan_fxB_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* sqrt_fxf_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","sqrt","v","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&sqrt_fxf_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* equal_ffxB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","equal","vs","Float32","Float32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&equal_ffxB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* equal_ffxB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","equal","vv","Float32","Float32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&equal_ffxB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* equal_ffxB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","equal","sv","Float32","Float32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&equal_ffxB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* not_equal_ffxB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","not_equal","vs","Float32","Float32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&not_equal_ffxB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* not_equal_ffxB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","not_equal","vv","Float32","Float32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&not_equal_ffxB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* not_equal_ffxB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","not_equal","sv","Float32","Float32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&not_equal_ffxB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_ffxB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater","vs","Float32","Float32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_ffxB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_ffxB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater","vv","Float32","Float32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_ffxB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_ffxB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater","sv","Float32","Float32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_ffxB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_equal_ffxB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater_equal","vs","Float32","Float32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_equal_ffxB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_equal_ffxB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater_equal","vv","Float32","Float32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_equal_ffxB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_equal_ffxB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater_equal","sv","Float32","Float32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_equal_ffxB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_ffxB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less","vs","Float32","Float32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_ffxB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_ffxB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less","vv","Float32","Float32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_ffxB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_ffxB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less","sv","Float32","Float32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_ffxB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_equal_ffxB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less_equal","vs","Float32","Float32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_equal_ffxB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_equal_ffxB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less_equal","vv","Float32","Float32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_equal_ffxB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_equal_ffxB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less_equal","sv","Float32","Float32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_equal_ffxB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_and_ffxB_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_and","vs","Float32","Float32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_and_ffxB_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_and_ffxB_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_and","vv","Float32","Float32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_and_ffxB_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_and_ffxB_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_and","sv","Float32","Float32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_and_ffxB_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_or_ffxB_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_or","vs","Float32","Float32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_or_ffxB_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_or_ffxB_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_or","vv","Float32","Float32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_or_ffxB_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_or_ffxB_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_or","sv","Float32","Float32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_or_ffxB_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_xor_ffxB_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_xor","vs","Float32","Float32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_xor_ffxB_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_xor_ffxB_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_xor","vv","Float32","Float32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_xor_ffxB_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_xor_ffxB_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_xor","sv","Float32","Float32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_xor_ffxB_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_not_fxB_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","logical_not","v","Float32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_not_fxB_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_fxf_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","floor","v","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_fxf_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* ceil_fxf_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","ceil","v","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&ceil_fxf_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_ffxf_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","maximum","vs","Float32","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_ffxf_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_ffxf_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","maximum","vv","Float32","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_ffxf_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_fxf_R */
    keytuple=Py_BuildValue("ss((s)(s))","maximum","R","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_fxf_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_fxf_A */
    keytuple=Py_BuildValue("ss((s)(s))","maximum","A","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_fxf_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_ffxf_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","maximum","sv","Float32","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_ffxf_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_ffxf_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","minimum","vs","Float32","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_ffxf_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_ffxf_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","minimum","vv","Float32","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_ffxf_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_fxf_R */
    keytuple=Py_BuildValue("ss((s)(s))","minimum","R","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_fxf_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_fxf_A */
    keytuple=Py_BuildValue("ss((s)(s))","minimum","A","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_fxf_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_ffxf_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","minimum","sv","Float32","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_ffxf_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* fabs_fxf_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","fabs","v","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&fabs_fxf_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* _round_fxf_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","_round","v","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&_round_fxf_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_ffxf_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","hypot","vs","Float32","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_ffxf_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_ffxf_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","hypot","vv","Float32","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_ffxf_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_fxf_R */
    keytuple=Py_BuildValue("ss((s)(s))","hypot","R","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_fxf_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_fxf_A */
    keytuple=Py_BuildValue("ss((s)(s))","hypot","A","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_fxf_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_ffxf_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","hypot","sv","Float32","Float32","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_ffxf_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    return dict;
}

/* platform independent*/
#ifdef MS_WIN32
__declspec(dllexport)
#endif
void init_ufuncFloat32(void) {
    PyObject *m, *d, *functions;
    m = Py_InitModule("_ufuncFloat32", _ufuncFloat32Methods);
    d = PyModule_GetDict(m);
    import_libnumarray();
    functions = init_funcDict();
    PyDict_SetItemString(d, "functionDict", functions);
    Py_DECREF(functions);
    ADD_VERSION(m);
}
