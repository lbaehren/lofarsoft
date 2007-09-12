
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

static int _distance3d_dddxd_vvvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64    *tin1 =  (Float64 *) buffers[1];
    Float64    *tin2 =  (Float64 *) buffers[2];
    Float64    *tout0 =  (Float64 *) buffers[3];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = distance3d(*tin0, *tin1, *tin2);
        
	++tin0; ++tin1; ++tin2; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor _distance3d_dddxd_vvvxf_descr =
{ "_distance3d_dddxd_vvvxf", (void *) _distance3d_dddxd_vvvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 3, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Float64), sizeof(Float64) }, { 0, 0, 0, 0, 0 } };

static int _distance3d_dddxd_vssxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64     tin1 = *(Float64 *) buffers[1];
    Float64     tin2 = *(Float64 *) buffers[2];
    Float64    *tout0 =  (Float64 *) buffers[3];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = distance3d(*tin0, tin1, tin2);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor _distance3d_dddxd_vssxf_descr =
{ "_distance3d_dddxd_vssxf", (void *) _distance3d_dddxd_vssxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 3, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Float64), sizeof(Float64) }, { 0, 1, 1, 0, 0 } };
/*********************  minus  *********************/

static int minus_dxd_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = -*tin0;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor minus_dxd_vxv_descr =
{ "minus_dxd_vxv", (void *) minus_dxd_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Float64), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  add  *********************/

static int add_ddxd_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64     tin1 = *(Float64 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 + tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor add_ddxd_vsxv_descr =
{ "add_ddxd_vsxv", (void *) add_ddxd_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Float64) }, { 0, 1, 0, 0 } };

static int add_ddxd_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64    *tin1 =  (Float64 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 + *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor add_ddxd_vvxv_descr =
{ "add_ddxd_vvxv", (void *) add_ddxd_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Float64) }, { 0, 0, 0, 0 } };

static void _add_dxd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Float64  *tin0   = (Float64 *) ((char *) input  + inboffset);
    Float64 *tout0  = (Float64 *) ((char *) output + outboffset);
    Float64 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Float64 *) ((char *) tin0 + inbstrides[dim]);
            net    =     net + *tin0;
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _add_dxd_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int add_dxd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _add_dxd_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(add_dxd_R, CHECK_ALIGN, sizeof(Float64), sizeof(Float64));

static void _add_dxd_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Float64 *tin0   = (Float64 *) ((char *) input  + inboffset);
    Float64 *tout0 = (Float64 *) ((char *) output + outboffset);
    Float64 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Float64 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Float64 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = lastval + *tin0;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _add_dxd_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  add_dxd_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _add_dxd_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(add_dxd_A, CHECK_ALIGN, sizeof(Float64), sizeof(Float64));

static int add_ddxd_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64     tin0 = *(Float64 *) buffers[0];
    Float64    *tin1 =  (Float64 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 + *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor add_ddxd_svxv_descr =
{ "add_ddxd_svxv", (void *) add_ddxd_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Float64) }, { 1, 0, 0, 0 } };
/*********************  subtract  *********************/

static int subtract_ddxd_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64     tin1 = *(Float64 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 - tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor subtract_ddxd_vsxv_descr =
{ "subtract_ddxd_vsxv", (void *) subtract_ddxd_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Float64) }, { 0, 1, 0, 0 } };

static int subtract_ddxd_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64    *tin1 =  (Float64 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 - *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor subtract_ddxd_vvxv_descr =
{ "subtract_ddxd_vvxv", (void *) subtract_ddxd_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Float64) }, { 0, 0, 0, 0 } };

static void _subtract_dxd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Float64  *tin0   = (Float64 *) ((char *) input  + inboffset);
    Float64 *tout0  = (Float64 *) ((char *) output + outboffset);
    Float64 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Float64 *) ((char *) tin0 + inbstrides[dim]);
            net    =     net - *tin0;
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _subtract_dxd_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int subtract_dxd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _subtract_dxd_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(subtract_dxd_R, CHECK_ALIGN, sizeof(Float64), sizeof(Float64));

static void _subtract_dxd_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Float64 *tin0   = (Float64 *) ((char *) input  + inboffset);
    Float64 *tout0 = (Float64 *) ((char *) output + outboffset);
    Float64 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Float64 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Float64 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = lastval - *tin0;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _subtract_dxd_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  subtract_dxd_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _subtract_dxd_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(subtract_dxd_A, CHECK_ALIGN, sizeof(Float64), sizeof(Float64));

static int subtract_ddxd_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64     tin0 = *(Float64 *) buffers[0];
    Float64    *tin1 =  (Float64 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 - *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor subtract_ddxd_svxv_descr =
{ "subtract_ddxd_svxv", (void *) subtract_ddxd_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Float64) }, { 1, 0, 0, 0 } };
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/

static int multiply_ddxd_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64     tin1 = *(Float64 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 * tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor multiply_ddxd_vsxv_descr =
{ "multiply_ddxd_vsxv", (void *) multiply_ddxd_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Float64) }, { 0, 1, 0, 0 } };

static int multiply_ddxd_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64    *tin1 =  (Float64 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 * *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor multiply_ddxd_vvxv_descr =
{ "multiply_ddxd_vvxv", (void *) multiply_ddxd_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Float64) }, { 0, 0, 0, 0 } };

static void _multiply_dxd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Float64  *tin0   = (Float64 *) ((char *) input  + inboffset);
    Float64 *tout0  = (Float64 *) ((char *) output + outboffset);
    Float64 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Float64 *) ((char *) tin0 + inbstrides[dim]);
            net    =     net * *tin0;
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _multiply_dxd_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int multiply_dxd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _multiply_dxd_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(multiply_dxd_R, CHECK_ALIGN, sizeof(Float64), sizeof(Float64));

static void _multiply_dxd_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Float64 *tin0   = (Float64 *) ((char *) input  + inboffset);
    Float64 *tout0 = (Float64 *) ((char *) output + outboffset);
    Float64 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Float64 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Float64 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = lastval * *tin0;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _multiply_dxd_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  multiply_dxd_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _multiply_dxd_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(multiply_dxd_A, CHECK_ALIGN, sizeof(Float64), sizeof(Float64));

static int multiply_ddxd_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64     tin0 = *(Float64 *) buffers[0];
    Float64    *tin1 =  (Float64 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 * *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor multiply_ddxd_svxv_descr =
{ "multiply_ddxd_svxv", (void *) multiply_ddxd_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Float64) }, { 1, 0, 0, 0 } };
/*********************  divide  *********************/
/*********************  divide  *********************/

static int divide_ddxd_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64     tin1 = *(Float64 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 / tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor divide_ddxd_vsxv_descr =
{ "divide_ddxd_vsxv", (void *) divide_ddxd_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Float64) }, { 0, 1, 0, 0 } };

static int divide_ddxd_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64    *tin1 =  (Float64 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 / *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor divide_ddxd_vvxv_descr =
{ "divide_ddxd_vvxv", (void *) divide_ddxd_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Float64) }, { 0, 0, 0, 0 } };

static void _divide_dxd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Float64  *tin0   = (Float64 *) ((char *) input  + inboffset);
    Float64 *tout0  = (Float64 *) ((char *) output + outboffset);
    Float64 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Float64 *) ((char *) tin0 + inbstrides[dim]);
            net    =     net / *tin0;
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _divide_dxd_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int divide_dxd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _divide_dxd_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(divide_dxd_R, CHECK_ALIGN, sizeof(Float64), sizeof(Float64));

static void _divide_dxd_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Float64 *tin0   = (Float64 *) ((char *) input  + inboffset);
    Float64 *tout0 = (Float64 *) ((char *) output + outboffset);
    Float64 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Float64 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Float64 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = lastval / *tin0;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _divide_dxd_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  divide_dxd_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _divide_dxd_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(divide_dxd_A, CHECK_ALIGN, sizeof(Float64), sizeof(Float64));

static int divide_ddxd_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64     tin0 = *(Float64 *) buffers[0];
    Float64    *tin1 =  (Float64 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 / *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor divide_ddxd_svxv_descr =
{ "divide_ddxd_svxv", (void *) divide_ddxd_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Float64) }, { 1, 0, 0, 0 } };
/*********************  floor_divide  *********************/
/*********************  floor_divide  *********************/

static int floor_divide_ddxd_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64     tin1 = *(Float64 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_FLOORDIVIDE(*tin0, tin1, *tout0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor floor_divide_ddxd_vsxv_descr =
{ "floor_divide_ddxd_vsxv", (void *) floor_divide_ddxd_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Float64) }, { 0, 1, 0, 0 } };

static int floor_divide_ddxd_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64    *tin1 =  (Float64 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_FLOORDIVIDE(*tin0, *tin1, *tout0);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor floor_divide_ddxd_vvxv_descr =
{ "floor_divide_ddxd_vvxv", (void *) floor_divide_ddxd_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Float64) }, { 0, 0, 0, 0 } };

static void _floor_divide_dxd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Float64  *tin0   = (Float64 *) ((char *) input  + inboffset);
    Float64 *tout0  = (Float64 *) ((char *) output + outboffset);
    Float64 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Float64 *) ((char *) tin0 + inbstrides[dim]);
            NUM_FLOORDIVIDE(net, *tin0, net);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _floor_divide_dxd_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int floor_divide_dxd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _floor_divide_dxd_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(floor_divide_dxd_R, CHECK_ALIGN, sizeof(Float64), sizeof(Float64));

static void _floor_divide_dxd_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Float64 *tin0   = (Float64 *) ((char *) input  + inboffset);
    Float64 *tout0 = (Float64 *) ((char *) output + outboffset);
    Float64 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Float64 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Float64 *) ((char *) tout0 + outbstrides[dim]);
            NUM_FLOORDIVIDE(lastval ,*tin0, *tout0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _floor_divide_dxd_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  floor_divide_dxd_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _floor_divide_dxd_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(floor_divide_dxd_A, CHECK_ALIGN, sizeof(Float64), sizeof(Float64));

static int floor_divide_ddxd_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64     tin0 = *(Float64 *) buffers[0];
    Float64    *tin1 =  (Float64 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        NUM_FLOORDIVIDE(tin0, *tin1, *tout0);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor floor_divide_ddxd_svxv_descr =
{ "floor_divide_ddxd_svxv", (void *) floor_divide_ddxd_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Float64) }, { 1, 0, 0, 0 } };
/*********************  true_divide  *********************/
/*********************  true_divide  *********************/

static int true_divide_ddxd_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64     tin1 = *(Float64 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 / tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor true_divide_ddxd_vsxv_descr =
{ "true_divide_ddxd_vsxv", (void *) true_divide_ddxd_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Float64) }, { 0, 1, 0, 0 } };

static int true_divide_ddxd_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64    *tin1 =  (Float64 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 / *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor true_divide_ddxd_vvxv_descr =
{ "true_divide_ddxd_vvxv", (void *) true_divide_ddxd_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Float64) }, { 0, 0, 0, 0 } };

static void _true_divide_dxd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Float64  *tin0   = (Float64 *) ((char *) input  + inboffset);
    Float64 *tout0  = (Float64 *) ((char *) output + outboffset);
    Float64 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Float64 *) ((char *) tin0 + inbstrides[dim]);
            net    =     net / *tin0;
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _true_divide_dxd_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int true_divide_dxd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _true_divide_dxd_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(true_divide_dxd_R, CHECK_ALIGN, sizeof(Float64), sizeof(Float64));

static void _true_divide_dxd_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Float64 *tin0   = (Float64 *) ((char *) input  + inboffset);
    Float64 *tout0 = (Float64 *) ((char *) output + outboffset);
    Float64 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Float64 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Float64 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = lastval / *tin0;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _true_divide_dxd_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  true_divide_dxd_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _true_divide_dxd_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(true_divide_dxd_A, CHECK_ALIGN, sizeof(Float64), sizeof(Float64));

static int true_divide_ddxd_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64     tin0 = *(Float64 *) buffers[0];
    Float64    *tin1 =  (Float64 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 / *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor true_divide_ddxd_svxv_descr =
{ "true_divide_ddxd_svxv", (void *) true_divide_ddxd_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Float64) }, { 1, 0, 0, 0 } };
/*********************  remainder  *********************/
/*********************  remainder  *********************/

static int remainder_ddxd_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64     tin1 = *(Float64 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = fmod(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor remainder_ddxd_vsxf_descr =
{ "remainder_ddxd_vsxf", (void *) remainder_ddxd_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Float64) }, { 0, 1, 0, 0 } };

static int remainder_ddxd_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64    *tin1 =  (Float64 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = fmod(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor remainder_ddxd_vvxf_descr =
{ "remainder_ddxd_vvxf", (void *) remainder_ddxd_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Float64) }, { 0, 0, 0, 0 } };

static void _remainder_dxd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Float64  *tin0   = (Float64 *) ((char *) input  + inboffset);
    Float64 *tout0  = (Float64 *) ((char *) output + outboffset);
    Float64 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Float64 *) ((char *) tin0 + inbstrides[dim]);
            net   = fmod(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _remainder_dxd_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int remainder_dxd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _remainder_dxd_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(remainder_dxd_R, CHECK_ALIGN, sizeof(Float64), sizeof(Float64));

static void _remainder_dxd_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Float64 *tin0   = (Float64 *) ((char *) input  + inboffset);
    Float64 *tout0 = (Float64 *) ((char *) output + outboffset);
    Float64 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Float64 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Float64 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = fmod(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _remainder_dxd_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  remainder_dxd_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _remainder_dxd_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(remainder_dxd_A, CHECK_ALIGN, sizeof(Float64), sizeof(Float64));

static int remainder_ddxd_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64     tin0 = *(Float64 *) buffers[0];
    Float64    *tin1 =  (Float64 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = fmod(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor remainder_ddxd_svxf_descr =
{ "remainder_ddxd_svxf", (void *) remainder_ddxd_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Float64) }, { 1, 0, 0, 0 } };
/*********************  power  *********************/

static int power_ddxd_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64     tin1 = *(Float64 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_pow(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor power_ddxd_vsxf_descr =
{ "power_ddxd_vsxf", (void *) power_ddxd_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Float64) }, { 0, 1, 0, 0 } };

static int power_ddxd_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64    *tin1 =  (Float64 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_pow(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor power_ddxd_vvxf_descr =
{ "power_ddxd_vvxf", (void *) power_ddxd_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Float64) }, { 0, 0, 0, 0 } };

static void _power_dxd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Float64  *tin0   = (Float64 *) ((char *) input  + inboffset);
    Float64 *tout0  = (Float64 *) ((char *) output + outboffset);
    Float64 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Float64 *) ((char *) tin0 + inbstrides[dim]);
            net   = num_pow(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _power_dxd_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int power_dxd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _power_dxd_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(power_dxd_R, CHECK_ALIGN, sizeof(Float64), sizeof(Float64));

static void _power_dxd_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Float64 *tin0   = (Float64 *) ((char *) input  + inboffset);
    Float64 *tout0 = (Float64 *) ((char *) output + outboffset);
    Float64 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Float64 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Float64 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = num_pow(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _power_dxd_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  power_dxd_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _power_dxd_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(power_dxd_A, CHECK_ALIGN, sizeof(Float64), sizeof(Float64));

static int power_ddxd_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64     tin0 = *(Float64 *) buffers[0];
    Float64    *tin1 =  (Float64 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_pow(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor power_ddxd_svxf_descr =
{ "power_ddxd_svxf", (void *) power_ddxd_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Float64) }, { 1, 0, 0, 0 } };
/*********************  abs  *********************/

static int abs_dxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = fabs(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor abs_dxd_vxf_descr =
{ "abs_dxd_vxf", (void *) abs_dxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Float64), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  sin  *********************/

static int sin_dxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = sin(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor sin_dxd_vxf_descr =
{ "sin_dxd_vxf", (void *) sin_dxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Float64), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  cos  *********************/

static int cos_dxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = cos(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor cos_dxd_vxf_descr =
{ "cos_dxd_vxf", (void *) cos_dxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Float64), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  tan  *********************/

static int tan_dxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = tan(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor tan_dxd_vxf_descr =
{ "tan_dxd_vxf", (void *) tan_dxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Float64), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arcsin  *********************/

static int arcsin_dxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = asin(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arcsin_dxd_vxf_descr =
{ "arcsin_dxd_vxf", (void *) arcsin_dxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Float64), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arccos  *********************/

static int arccos_dxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = acos(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arccos_dxd_vxf_descr =
{ "arccos_dxd_vxf", (void *) arccos_dxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Float64), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arctan  *********************/

static int arctan_dxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = atan(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arctan_dxd_vxf_descr =
{ "arctan_dxd_vxf", (void *) arctan_dxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Float64), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arctan2  *********************/

static int arctan2_ddxd_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64     tin1 = *(Float64 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = atan2(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arctan2_ddxd_vsxf_descr =
{ "arctan2_ddxd_vsxf", (void *) arctan2_ddxd_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Float64) }, { 0, 1, 0, 0 } };

static int arctan2_ddxd_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64    *tin1 =  (Float64 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = atan2(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arctan2_ddxd_vvxf_descr =
{ "arctan2_ddxd_vvxf", (void *) arctan2_ddxd_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Float64) }, { 0, 0, 0, 0 } };

static void _arctan2_dxd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Float64  *tin0   = (Float64 *) ((char *) input  + inboffset);
    Float64 *tout0  = (Float64 *) ((char *) output + outboffset);
    Float64 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Float64 *) ((char *) tin0 + inbstrides[dim]);
            net   = atan2(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _arctan2_dxd_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int arctan2_dxd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _arctan2_dxd_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(arctan2_dxd_R, CHECK_ALIGN, sizeof(Float64), sizeof(Float64));

static void _arctan2_dxd_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Float64 *tin0   = (Float64 *) ((char *) input  + inboffset);
    Float64 *tout0 = (Float64 *) ((char *) output + outboffset);
    Float64 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Float64 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Float64 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = atan2(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _arctan2_dxd_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  arctan2_dxd_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _arctan2_dxd_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(arctan2_dxd_A, CHECK_ALIGN, sizeof(Float64), sizeof(Float64));

static int arctan2_ddxd_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64     tin0 = *(Float64 *) buffers[0];
    Float64    *tin1 =  (Float64 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = atan2(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arctan2_ddxd_svxf_descr =
{ "arctan2_ddxd_svxf", (void *) arctan2_ddxd_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Float64) }, { 1, 0, 0, 0 } };
/*********************  log  *********************/

static int log_dxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_log(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor log_dxd_vxf_descr =
{ "log_dxd_vxf", (void *) log_dxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Float64), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  log10  *********************/

static int log10_dxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_log10(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor log10_dxd_vxf_descr =
{ "log10_dxd_vxf", (void *) log10_dxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Float64), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  exp  *********************/

static int exp_dxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = exp(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor exp_dxd_vxf_descr =
{ "exp_dxd_vxf", (void *) exp_dxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Float64), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  sinh  *********************/

static int sinh_dxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = sinh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor sinh_dxd_vxf_descr =
{ "sinh_dxd_vxf", (void *) sinh_dxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Float64), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  cosh  *********************/

static int cosh_dxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = cosh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor cosh_dxd_vxf_descr =
{ "cosh_dxd_vxf", (void *) cosh_dxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Float64), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  tanh  *********************/

static int tanh_dxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = tanh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor tanh_dxd_vxf_descr =
{ "tanh_dxd_vxf", (void *) tanh_dxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Float64), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arcsinh  *********************/

static int arcsinh_dxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_asinh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arcsinh_dxd_vxf_descr =
{ "arcsinh_dxd_vxf", (void *) arcsinh_dxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Float64), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arccosh  *********************/

static int arccosh_dxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_acosh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arccosh_dxd_vxf_descr =
{ "arccosh_dxd_vxf", (void *) arccosh_dxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Float64), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arctanh  *********************/

static int arctanh_dxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_atanh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arctanh_dxd_vxf_descr =
{ "arctanh_dxd_vxf", (void *) arctanh_dxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Float64), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  ieeemask  *********************/
/*********************  ieeemask  *********************/

static int ieeemask_dixB_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Int32       tin1 = *(Int32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NA_IeeeMask64(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor ieeemask_dixB_vsxf_descr =
{ "ieeemask_dixB_vsxf", (void *) ieeemask_dixB_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Int32), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int ieeemask_dixB_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Int32      *tin1 =  (Int32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NA_IeeeMask64(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor ieeemask_dixB_vvxf_descr =
{ "ieeemask_dixB_vvxf", (void *) ieeemask_dixB_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Int32), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int ieeemask_dixB_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64     tin0 = *(Float64 *) buffers[0];
    Int32      *tin1 =  (Int32 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NA_IeeeMask64(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor ieeemask_dixB_svxf_descr =
{ "ieeemask_dixB_svxf", (void *) ieeemask_dixB_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Int32), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  isnan  *********************/
/*********************  isnan  *********************/

static int isnan_dxB_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Bool       *tout0 =  (Bool *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = NA_isnan64(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor isnan_dxB_vxf_descr =
{ "isnan_dxB_vxf", (void *) isnan_dxB_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Float64), sizeof(Bool) }, { 0, 0, 0 } };
/*********************  isnan  *********************/
/*********************  isnan  *********************/
/*********************  sqrt  *********************/

static int sqrt_dxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = sqrt(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor sqrt_dxd_vxf_descr =
{ "sqrt_dxd_vxf", (void *) sqrt_dxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Float64), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  equal  *********************/

static int equal_ddxB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64     tin1 = *(Float64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 == tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor equal_ddxB_vsxv_descr =
{ "equal_ddxB_vsxv", (void *) equal_ddxB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int equal_ddxB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64    *tin1 =  (Float64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 == *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor equal_ddxB_vvxv_descr =
{ "equal_ddxB_vvxv", (void *) equal_ddxB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int equal_ddxB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64     tin0 = *(Float64 *) buffers[0];
    Float64    *tin1 =  (Float64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 == *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor equal_ddxB_svxv_descr =
{ "equal_ddxB_svxv", (void *) equal_ddxB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  not_equal  *********************/

static int not_equal_ddxB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64     tin1 = *(Float64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 != tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor not_equal_ddxB_vsxv_descr =
{ "not_equal_ddxB_vsxv", (void *) not_equal_ddxB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int not_equal_ddxB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64    *tin1 =  (Float64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 != *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor not_equal_ddxB_vvxv_descr =
{ "not_equal_ddxB_vvxv", (void *) not_equal_ddxB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int not_equal_ddxB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64     tin0 = *(Float64 *) buffers[0];
    Float64    *tin1 =  (Float64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 != *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor not_equal_ddxB_svxv_descr =
{ "not_equal_ddxB_svxv", (void *) not_equal_ddxB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  greater  *********************/

static int greater_ddxB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64     tin1 = *(Float64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 > tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_ddxB_vsxv_descr =
{ "greater_ddxB_vsxv", (void *) greater_ddxB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int greater_ddxB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64    *tin1 =  (Float64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 > *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_ddxB_vvxv_descr =
{ "greater_ddxB_vvxv", (void *) greater_ddxB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int greater_ddxB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64     tin0 = *(Float64 *) buffers[0];
    Float64    *tin1 =  (Float64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 > *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_ddxB_svxv_descr =
{ "greater_ddxB_svxv", (void *) greater_ddxB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  greater_equal  *********************/

static int greater_equal_ddxB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64     tin1 = *(Float64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 >= tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_equal_ddxB_vsxv_descr =
{ "greater_equal_ddxB_vsxv", (void *) greater_equal_ddxB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int greater_equal_ddxB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64    *tin1 =  (Float64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 >= *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_equal_ddxB_vvxv_descr =
{ "greater_equal_ddxB_vvxv", (void *) greater_equal_ddxB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int greater_equal_ddxB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64     tin0 = *(Float64 *) buffers[0];
    Float64    *tin1 =  (Float64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 >= *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_equal_ddxB_svxv_descr =
{ "greater_equal_ddxB_svxv", (void *) greater_equal_ddxB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  less  *********************/

static int less_ddxB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64     tin1 = *(Float64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 < tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_ddxB_vsxv_descr =
{ "less_ddxB_vsxv", (void *) less_ddxB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int less_ddxB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64    *tin1 =  (Float64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 < *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_ddxB_vvxv_descr =
{ "less_ddxB_vvxv", (void *) less_ddxB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int less_ddxB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64     tin0 = *(Float64 *) buffers[0];
    Float64    *tin1 =  (Float64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 < *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_ddxB_svxv_descr =
{ "less_ddxB_svxv", (void *) less_ddxB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  less_equal  *********************/

static int less_equal_ddxB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64     tin1 = *(Float64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 <= tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_equal_ddxB_vsxv_descr =
{ "less_equal_ddxB_vsxv", (void *) less_equal_ddxB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int less_equal_ddxB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64    *tin1 =  (Float64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 <= *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_equal_ddxB_vvxv_descr =
{ "less_equal_ddxB_vvxv", (void *) less_equal_ddxB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int less_equal_ddxB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64     tin0 = *(Float64 *) buffers[0];
    Float64    *tin1 =  (Float64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 <= *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_equal_ddxB_svxv_descr =
{ "less_equal_ddxB_svxv", (void *) less_equal_ddxB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  logical_and  *********************/

static int logical_and_ddxB_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64     tin1 = *(Float64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_and(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_and_ddxB_vsxf_descr =
{ "logical_and_ddxB_vsxf", (void *) logical_and_ddxB_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int logical_and_ddxB_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64    *tin1 =  (Float64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_and(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_and_ddxB_vvxf_descr =
{ "logical_and_ddxB_vvxf", (void *) logical_and_ddxB_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int logical_and_ddxB_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64     tin0 = *(Float64 *) buffers[0];
    Float64    *tin1 =  (Float64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_and(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_and_ddxB_svxf_descr =
{ "logical_and_ddxB_svxf", (void *) logical_and_ddxB_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  logical_or  *********************/

static int logical_or_ddxB_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64     tin1 = *(Float64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_or(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_or_ddxB_vsxf_descr =
{ "logical_or_ddxB_vsxf", (void *) logical_or_ddxB_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int logical_or_ddxB_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64    *tin1 =  (Float64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_or(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_or_ddxB_vvxf_descr =
{ "logical_or_ddxB_vvxf", (void *) logical_or_ddxB_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int logical_or_ddxB_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64     tin0 = *(Float64 *) buffers[0];
    Float64    *tin1 =  (Float64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_or(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_or_ddxB_svxf_descr =
{ "logical_or_ddxB_svxf", (void *) logical_or_ddxB_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  logical_xor  *********************/

static int logical_xor_ddxB_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64     tin1 = *(Float64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_xor(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_xor_ddxB_vsxf_descr =
{ "logical_xor_ddxB_vsxf", (void *) logical_xor_ddxB_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int logical_xor_ddxB_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64    *tin1 =  (Float64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_xor(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_xor_ddxB_vvxf_descr =
{ "logical_xor_ddxB_vvxf", (void *) logical_xor_ddxB_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int logical_xor_ddxB_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64     tin0 = *(Float64 *) buffers[0];
    Float64    *tin1 =  (Float64 *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_xor(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_xor_ddxB_svxf_descr =
{ "logical_xor_ddxB_svxf", (void *) logical_xor_ddxB_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  logical_and  *********************/
/*********************  logical_or  *********************/
/*********************  logical_xor  *********************/
/*********************  logical_not  *********************/

static int logical_not_dxB_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Bool       *tout0 =  (Bool *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = !*tin0;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_not_dxB_vxv_descr =
{ "logical_not_dxB_vxv", (void *) logical_not_dxB_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Float64), sizeof(Bool) }, { 0, 0, 0 } };
/*********************  bitwise_and  *********************/
/*********************  bitwise_or  *********************/
/*********************  bitwise_xor  *********************/
/*********************  bitwise_not  *********************/
/*********************  bitwise_not  *********************/
/*********************  rshift  *********************/
/*********************  lshift  *********************/
/*********************  floor  *********************/
/*********************  floor  *********************/

static int floor_dxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = floor(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor floor_dxd_vxf_descr =
{ "floor_dxd_vxf", (void *) floor_dxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Float64), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  ceil  *********************/
/*********************  ceil  *********************/

static int ceil_dxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ceil(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor ceil_dxd_vxf_descr =
{ "ceil_dxd_vxf", (void *) ceil_dxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Float64), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  maximum  *********************/

static int maximum_ddxd_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64     tin1 = *(Float64 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];
Float64 temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufmaximum(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor maximum_ddxd_vsxf_descr =
{ "maximum_ddxd_vsxf", (void *) maximum_ddxd_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Float64) }, { 0, 1, 0, 0 } };

static int maximum_ddxd_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64    *tin1 =  (Float64 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];
Float64 temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufmaximum(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor maximum_ddxd_vvxf_descr =
{ "maximum_ddxd_vvxf", (void *) maximum_ddxd_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Float64) }, { 0, 0, 0, 0 } };

static void _maximum_dxd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Float64  *tin0   = (Float64 *) ((char *) input  + inboffset);
    Float64 *tout0  = (Float64 *) ((char *) output + outboffset);
    Float64 net;
    Float64 temp1, temp2;
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Float64 *) ((char *) tin0 + inbstrides[dim]);
            net   = ufmaximum(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _maximum_dxd_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int maximum_dxd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _maximum_dxd_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(maximum_dxd_R, CHECK_ALIGN, sizeof(Float64), sizeof(Float64));

static void _maximum_dxd_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Float64 *tin0   = (Float64 *) ((char *) input  + inboffset);
    Float64 *tout0 = (Float64 *) ((char *) output + outboffset);
    Float64 lastval;
    Float64 temp1, temp2;
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Float64 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Float64 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = ufmaximum(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _maximum_dxd_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  maximum_dxd_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _maximum_dxd_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(maximum_dxd_A, CHECK_ALIGN, sizeof(Float64), sizeof(Float64));

static int maximum_ddxd_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64     tin0 = *(Float64 *) buffers[0];
    Float64    *tin1 =  (Float64 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];
Float64 temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufmaximum(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor maximum_ddxd_svxf_descr =
{ "maximum_ddxd_svxf", (void *) maximum_ddxd_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Float64) }, { 1, 0, 0, 0 } };
/*********************  minimum  *********************/

static int minimum_ddxd_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64     tin1 = *(Float64 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];
Float64 temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufminimum(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor minimum_ddxd_vsxf_descr =
{ "minimum_ddxd_vsxf", (void *) minimum_ddxd_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Float64) }, { 0, 1, 0, 0 } };

static int minimum_ddxd_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64    *tin1 =  (Float64 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];
Float64 temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufminimum(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor minimum_ddxd_vvxf_descr =
{ "minimum_ddxd_vvxf", (void *) minimum_ddxd_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Float64) }, { 0, 0, 0, 0 } };

static void _minimum_dxd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Float64  *tin0   = (Float64 *) ((char *) input  + inboffset);
    Float64 *tout0  = (Float64 *) ((char *) output + outboffset);
    Float64 net;
    Float64 temp1, temp2;
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Float64 *) ((char *) tin0 + inbstrides[dim]);
            net   = ufminimum(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _minimum_dxd_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int minimum_dxd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _minimum_dxd_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(minimum_dxd_R, CHECK_ALIGN, sizeof(Float64), sizeof(Float64));

static void _minimum_dxd_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Float64 *tin0   = (Float64 *) ((char *) input  + inboffset);
    Float64 *tout0 = (Float64 *) ((char *) output + outboffset);
    Float64 lastval;
    Float64 temp1, temp2;
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Float64 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Float64 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = ufminimum(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _minimum_dxd_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  minimum_dxd_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _minimum_dxd_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(minimum_dxd_A, CHECK_ALIGN, sizeof(Float64), sizeof(Float64));

static int minimum_ddxd_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64     tin0 = *(Float64 *) buffers[0];
    Float64    *tin1 =  (Float64 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];
Float64 temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufminimum(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor minimum_ddxd_svxf_descr =
{ "minimum_ddxd_svxf", (void *) minimum_ddxd_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Float64) }, { 1, 0, 0, 0 } };
/*********************  fabs  *********************/

static int fabs_dxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = fabs(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor fabs_dxd_vxf_descr =
{ "fabs_dxd_vxf", (void *) fabs_dxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Float64), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  _round  *********************/

static int _round_dxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_round(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor _round_dxd_vxf_descr =
{ "_round_dxd_vxf", (void *) _round_dxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Float64), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  hypot  *********************/

static int hypot_ddxd_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64     tin1 = *(Float64 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = hypot(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor hypot_ddxd_vsxf_descr =
{ "hypot_ddxd_vsxf", (void *) hypot_ddxd_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Float64) }, { 0, 1, 0, 0 } };

static int hypot_ddxd_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64    *tin1 =  (Float64 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = hypot(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor hypot_ddxd_vvxf_descr =
{ "hypot_ddxd_vvxf", (void *) hypot_ddxd_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Float64) }, { 0, 0, 0, 0 } };

static void _hypot_dxd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Float64  *tin0   = (Float64 *) ((char *) input  + inboffset);
    Float64 *tout0  = (Float64 *) ((char *) output + outboffset);
    Float64 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Float64 *) ((char *) tin0 + inbstrides[dim]);
            net   = hypot(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _hypot_dxd_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int hypot_dxd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _hypot_dxd_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(hypot_dxd_R, CHECK_ALIGN, sizeof(Float64), sizeof(Float64));

static void _hypot_dxd_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Float64 *tin0   = (Float64 *) ((char *) input  + inboffset);
    Float64 *tout0 = (Float64 *) ((char *) output + outboffset);
    Float64 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Float64 *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Float64 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = hypot(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _hypot_dxd_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  hypot_dxd_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _hypot_dxd_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(hypot_dxd_A, CHECK_ALIGN, sizeof(Float64), sizeof(Float64));

static int hypot_ddxd_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64     tin0 = *(Float64 *) buffers[0];
    Float64    *tin1 =  (Float64 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = hypot(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor hypot_ddxd_svxf_descr =
{ "hypot_ddxd_svxf", (void *) hypot_ddxd_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Float64) }, { 1, 0, 0, 0 } };
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

static PyMethodDef _ufuncFloat64Methods[] = {

	{NULL,      NULL}        /* Sentinel */
};

static PyObject *init_funcDict(void) {
    PyObject *dict, *keytuple, *cfunc;
    dict = PyDict_New();
    /* _distance3d_dddxd_vvvxf */
    keytuple=Py_BuildValue("ss((sss)(s))","_distance3d","vvv","Float64","Float64","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&_distance3d_dddxd_vvvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* _distance3d_dddxd_vssxf */
    keytuple=Py_BuildValue("ss((sss)(s))","_distance3d","vss","Float64","Float64","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&_distance3d_dddxd_vssxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minus_dxd_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","minus","v","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minus_dxd_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_ddxd_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","add","vs","Float64","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_ddxd_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_ddxd_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","add","vv","Float64","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_ddxd_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_dxd_R */
    keytuple=Py_BuildValue("ss((s)(s))","add","R","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_dxd_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_dxd_A */
    keytuple=Py_BuildValue("ss((s)(s))","add","A","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_dxd_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_ddxd_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","add","sv","Float64","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_ddxd_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_ddxd_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","subtract","vs","Float64","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_ddxd_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_ddxd_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","subtract","vv","Float64","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_ddxd_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_dxd_R */
    keytuple=Py_BuildValue("ss((s)(s))","subtract","R","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_dxd_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_dxd_A */
    keytuple=Py_BuildValue("ss((s)(s))","subtract","A","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_dxd_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_ddxd_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","subtract","sv","Float64","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_ddxd_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* multiply_ddxd_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","multiply","vs","Float64","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&multiply_ddxd_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* multiply_ddxd_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","multiply","vv","Float64","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&multiply_ddxd_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* multiply_dxd_R */
    keytuple=Py_BuildValue("ss((s)(s))","multiply","R","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&multiply_dxd_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* multiply_dxd_A */
    keytuple=Py_BuildValue("ss((s)(s))","multiply","A","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&multiply_dxd_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* multiply_ddxd_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","multiply","sv","Float64","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&multiply_ddxd_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_ddxd_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","divide","vs","Float64","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_ddxd_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_ddxd_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","divide","vv","Float64","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_ddxd_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_dxd_R */
    keytuple=Py_BuildValue("ss((s)(s))","divide","R","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_dxd_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_dxd_A */
    keytuple=Py_BuildValue("ss((s)(s))","divide","A","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_dxd_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_ddxd_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","divide","sv","Float64","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_ddxd_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_divide_ddxd_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","floor_divide","vs","Float64","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_divide_ddxd_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_divide_ddxd_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","floor_divide","vv","Float64","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_divide_ddxd_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_divide_dxd_R */
    keytuple=Py_BuildValue("ss((s)(s))","floor_divide","R","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_divide_dxd_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_divide_dxd_A */
    keytuple=Py_BuildValue("ss((s)(s))","floor_divide","A","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_divide_dxd_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_divide_ddxd_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","floor_divide","sv","Float64","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_divide_ddxd_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_ddxd_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","true_divide","vs","Float64","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_ddxd_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_ddxd_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","true_divide","vv","Float64","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_ddxd_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_dxd_R */
    keytuple=Py_BuildValue("ss((s)(s))","true_divide","R","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_dxd_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_dxd_A */
    keytuple=Py_BuildValue("ss((s)(s))","true_divide","A","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_dxd_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_ddxd_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","true_divide","sv","Float64","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_ddxd_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_ddxd_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","remainder","vs","Float64","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_ddxd_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_ddxd_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","remainder","vv","Float64","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_ddxd_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_dxd_R */
    keytuple=Py_BuildValue("ss((s)(s))","remainder","R","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_dxd_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_dxd_A */
    keytuple=Py_BuildValue("ss((s)(s))","remainder","A","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_dxd_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_ddxd_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","remainder","sv","Float64","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_ddxd_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_ddxd_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","power","vs","Float64","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_ddxd_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_ddxd_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","power","vv","Float64","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_ddxd_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_dxd_R */
    keytuple=Py_BuildValue("ss((s)(s))","power","R","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_dxd_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_dxd_A */
    keytuple=Py_BuildValue("ss((s)(s))","power","A","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_dxd_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_ddxd_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","power","sv","Float64","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_ddxd_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* abs_dxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","abs","v","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&abs_dxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* sin_dxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","sin","v","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&sin_dxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* cos_dxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","cos","v","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&cos_dxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* tan_dxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","tan","v","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&tan_dxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arcsin_dxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arcsin","v","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arcsin_dxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arccos_dxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arccos","v","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arccos_dxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan_dxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arctan","v","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan_dxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan2_ddxd_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","arctan2","vs","Float64","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan2_ddxd_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan2_ddxd_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","arctan2","vv","Float64","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan2_ddxd_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan2_dxd_R */
    keytuple=Py_BuildValue("ss((s)(s))","arctan2","R","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan2_dxd_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan2_dxd_A */
    keytuple=Py_BuildValue("ss((s)(s))","arctan2","A","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan2_dxd_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan2_ddxd_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","arctan2","sv","Float64","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan2_ddxd_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* log_dxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","log","v","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&log_dxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* log10_dxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","log10","v","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&log10_dxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* exp_dxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","exp","v","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&exp_dxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* sinh_dxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","sinh","v","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&sinh_dxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* cosh_dxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","cosh","v","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&cosh_dxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* tanh_dxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","tanh","v","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&tanh_dxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arcsinh_dxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arcsinh","v","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arcsinh_dxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arccosh_dxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arccosh","v","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arccosh_dxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctanh_dxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arctanh","v","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctanh_dxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* ieeemask_dixB_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","ieeemask","vs","Float64","Int32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&ieeemask_dixB_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* ieeemask_dixB_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","ieeemask","vv","Float64","Int32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&ieeemask_dixB_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* ieeemask_dixB_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","ieeemask","sv","Float64","Int32","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&ieeemask_dixB_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* isnan_dxB_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","isnan","v","Float64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&isnan_dxB_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* sqrt_dxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","sqrt","v","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&sqrt_dxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* equal_ddxB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","equal","vs","Float64","Float64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&equal_ddxB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* equal_ddxB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","equal","vv","Float64","Float64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&equal_ddxB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* equal_ddxB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","equal","sv","Float64","Float64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&equal_ddxB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* not_equal_ddxB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","not_equal","vs","Float64","Float64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&not_equal_ddxB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* not_equal_ddxB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","not_equal","vv","Float64","Float64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&not_equal_ddxB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* not_equal_ddxB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","not_equal","sv","Float64","Float64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&not_equal_ddxB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_ddxB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater","vs","Float64","Float64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_ddxB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_ddxB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater","vv","Float64","Float64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_ddxB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_ddxB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater","sv","Float64","Float64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_ddxB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_equal_ddxB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater_equal","vs","Float64","Float64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_equal_ddxB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_equal_ddxB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater_equal","vv","Float64","Float64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_equal_ddxB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_equal_ddxB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater_equal","sv","Float64","Float64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_equal_ddxB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_ddxB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less","vs","Float64","Float64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_ddxB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_ddxB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less","vv","Float64","Float64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_ddxB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_ddxB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less","sv","Float64","Float64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_ddxB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_equal_ddxB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less_equal","vs","Float64","Float64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_equal_ddxB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_equal_ddxB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less_equal","vv","Float64","Float64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_equal_ddxB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_equal_ddxB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less_equal","sv","Float64","Float64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_equal_ddxB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_and_ddxB_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_and","vs","Float64","Float64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_and_ddxB_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_and_ddxB_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_and","vv","Float64","Float64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_and_ddxB_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_and_ddxB_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_and","sv","Float64","Float64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_and_ddxB_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_or_ddxB_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_or","vs","Float64","Float64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_or_ddxB_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_or_ddxB_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_or","vv","Float64","Float64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_or_ddxB_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_or_ddxB_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_or","sv","Float64","Float64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_or_ddxB_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_xor_ddxB_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_xor","vs","Float64","Float64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_xor_ddxB_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_xor_ddxB_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_xor","vv","Float64","Float64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_xor_ddxB_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_xor_ddxB_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_xor","sv","Float64","Float64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_xor_ddxB_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_not_dxB_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","logical_not","v","Float64","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_not_dxB_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_dxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","floor","v","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_dxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* ceil_dxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","ceil","v","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&ceil_dxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_ddxd_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","maximum","vs","Float64","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_ddxd_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_ddxd_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","maximum","vv","Float64","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_ddxd_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_dxd_R */
    keytuple=Py_BuildValue("ss((s)(s))","maximum","R","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_dxd_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_dxd_A */
    keytuple=Py_BuildValue("ss((s)(s))","maximum","A","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_dxd_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_ddxd_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","maximum","sv","Float64","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_ddxd_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_ddxd_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","minimum","vs","Float64","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_ddxd_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_ddxd_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","minimum","vv","Float64","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_ddxd_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_dxd_R */
    keytuple=Py_BuildValue("ss((s)(s))","minimum","R","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_dxd_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_dxd_A */
    keytuple=Py_BuildValue("ss((s)(s))","minimum","A","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_dxd_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_ddxd_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","minimum","sv","Float64","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_ddxd_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* fabs_dxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","fabs","v","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&fabs_dxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* _round_dxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","_round","v","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&_round_dxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_ddxd_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","hypot","vs","Float64","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_ddxd_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_ddxd_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","hypot","vv","Float64","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_ddxd_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_dxd_R */
    keytuple=Py_BuildValue("ss((s)(s))","hypot","R","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_dxd_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_dxd_A */
    keytuple=Py_BuildValue("ss((s)(s))","hypot","A","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_dxd_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_ddxd_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","hypot","sv","Float64","Float64","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_ddxd_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    return dict;
}

/* platform independent*/
#ifdef MS_WIN32
__declspec(dllexport)
#endif
void init_ufuncFloat64(void) {
    PyObject *m, *d, *functions;
    m = Py_InitModule("_ufuncFloat64", _ufuncFloat64Methods);
    d = PyModule_GetDict(m);
    import_libnumarray();
    functions = init_funcDict();
    PyDict_SetItemString(d, "functionDict", functions);
    Py_DECREF(functions);
    ADD_VERSION(m);
}
