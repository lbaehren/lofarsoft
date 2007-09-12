
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

static int minus_Bx1_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Int8       *tout0 =  (Int8 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = -*tin0;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor minus_Bx1_vxv_descr =
{ "minus_Bx1_vxv", (void *) minus_Bx1_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Bool), sizeof(Int8) }, { 0, 0, 0 } };
/*********************  add  *********************/

static int add_BBx1_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Bool        tin1 = *(Bool *) buffers[1];
    Int8       *tout0 =  (Int8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 + tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor add_BBx1_vsxv_descr =
{ "add_BBx1_vsxv", (void *) add_BBx1_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Int8) }, { 0, 1, 0, 0 } };

static int add_BBx1_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Bool       *tin1 =  (Bool *) buffers[1];
    Int8       *tout0 =  (Int8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 + *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor add_BBx1_vvxv_descr =
{ "add_BBx1_vvxv", (void *) add_BBx1_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Int8) }, { 0, 0, 0, 0 } };

static void _add_Bx1_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Bool  *tin0   = (Bool *) ((char *) input  + inboffset);
    Int8 *tout0  = (Int8 *) ((char *) output + outboffset);
    Int8 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Bool *) ((char *) tin0 + inbstrides[dim]);
            net    =     net + *tin0;
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _add_Bx1_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int add_Bx1_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _add_Bx1_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(add_Bx1_R, CHECK_ALIGN, sizeof(Bool), sizeof(Int8));

static void _add_Bx1_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Bool *tin0   = (Bool *) ((char *) input  + inboffset);
    Int8 *tout0 = (Int8 *) ((char *) output + outboffset);
    Int8 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Bool *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Int8 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = lastval + *tin0;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _add_Bx1_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  add_Bx1_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _add_Bx1_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(add_Bx1_A, CHECK_ALIGN, sizeof(Bool), sizeof(Int8));

static int add_BBx1_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool        tin0 = *(Bool *) buffers[0];
    Bool       *tin1 =  (Bool *) buffers[1];
    Int8       *tout0 =  (Int8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 + *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor add_BBx1_svxv_descr =
{ "add_BBx1_svxv", (void *) add_BBx1_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Int8) }, { 1, 0, 0, 0 } };
/*********************  subtract  *********************/

static int subtract_BBx1_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Bool        tin1 = *(Bool *) buffers[1];
    Int8       *tout0 =  (Int8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 - tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor subtract_BBx1_vsxv_descr =
{ "subtract_BBx1_vsxv", (void *) subtract_BBx1_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Int8) }, { 0, 1, 0, 0 } };

static int subtract_BBx1_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Bool       *tin1 =  (Bool *) buffers[1];
    Int8       *tout0 =  (Int8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 - *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor subtract_BBx1_vvxv_descr =
{ "subtract_BBx1_vvxv", (void *) subtract_BBx1_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Int8) }, { 0, 0, 0, 0 } };

static void _subtract_Bx1_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Bool  *tin0   = (Bool *) ((char *) input  + inboffset);
    Int8 *tout0  = (Int8 *) ((char *) output + outboffset);
    Int8 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Bool *) ((char *) tin0 + inbstrides[dim]);
            net    =     net - *tin0;
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _subtract_Bx1_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int subtract_Bx1_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _subtract_Bx1_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(subtract_Bx1_R, CHECK_ALIGN, sizeof(Bool), sizeof(Int8));

static void _subtract_Bx1_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Bool *tin0   = (Bool *) ((char *) input  + inboffset);
    Int8 *tout0 = (Int8 *) ((char *) output + outboffset);
    Int8 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Bool *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Int8 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = lastval - *tin0;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _subtract_Bx1_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  subtract_Bx1_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _subtract_Bx1_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(subtract_Bx1_A, CHECK_ALIGN, sizeof(Bool), sizeof(Int8));

static int subtract_BBx1_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool        tin0 = *(Bool *) buffers[0];
    Bool       *tin1 =  (Bool *) buffers[1];
    Int8       *tout0 =  (Int8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 - *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor subtract_BBx1_svxv_descr =
{ "subtract_BBx1_svxv", (void *) subtract_BBx1_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Int8) }, { 1, 0, 0, 0 } };
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  multiply  *********************/
/*********************  divide  *********************/

static int divide_BBxB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Bool        tin1 = *(Bool *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((tin1==0) ? int_dividebyzero_error(tin1, *tin0) : *tin0 / tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor divide_BBxB_vsxv_descr =
{ "divide_BBxB_vsxv", (void *) divide_BBxB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int divide_BBxB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Bool       *tin1 =  (Bool *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, *tin0) : *tin0 / *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor divide_BBxB_vvxv_descr =
{ "divide_BBxB_vvxv", (void *) divide_BBxB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Bool) }, { 0, 0, 0, 0 } };

static void _divide_BxB_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Bool  *tin0   = (Bool *) ((char *) input  + inboffset);
    Bool *tout0  = (Bool *) ((char *) output + outboffset);
    Bool net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Bool *) ((char *) tin0 + inbstrides[dim]);
            net = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0) : net / *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _divide_BxB_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int divide_BxB_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _divide_BxB_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(divide_BxB_R, CHECK_ALIGN, sizeof(Bool), sizeof(Bool));

static void _divide_BxB_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Bool *tin0   = (Bool *) ((char *) input  + inboffset);
    Bool *tout0 = (Bool *) ((char *) output + outboffset);
    Bool lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Bool *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Bool *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0) : lastval / *tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _divide_BxB_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  divide_BxB_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _divide_BxB_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(divide_BxB_A, CHECK_ALIGN, sizeof(Bool), sizeof(Bool));

static int divide_BBxB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool        tin0 = *(Bool *) buffers[0];
    Bool       *tin1 =  (Bool *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, 0) : tin0 / *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor divide_BBxB_svxv_descr =
{ "divide_BBxB_svxv", (void *) divide_BBxB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  divide  *********************/
/*********************  floor_divide  *********************/

static int floor_divide_BBxB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Bool        tin1 = *(Bool *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = (( tin1==0) ? int_dividebyzero_error( tin1, *tin0) : floor(*tin0   / (double)  tin1));
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor floor_divide_BBxB_vsxv_descr =
{ "floor_divide_BBxB_vsxv", (void *) floor_divide_BBxB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int floor_divide_BBxB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Bool       *tin1 =  (Bool *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, *tin0) : floor(*tin0   / (double) *tin1));
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor floor_divide_BBxB_vvxv_descr =
{ "floor_divide_BBxB_vvxv", (void *) floor_divide_BBxB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Bool) }, { 0, 0, 0, 0 } };

static void _floor_divide_BxB_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Bool  *tin0   = (Bool *) ((char *) input  + inboffset);
    Bool *tout0  = (Bool *) ((char *) output + outboffset);
    Bool net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Bool *) ((char *) tin0 + inbstrides[dim]);
            net    = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0)     : floor(net     / (double) *tin0));
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _floor_divide_BxB_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int floor_divide_BxB_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _floor_divide_BxB_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(floor_divide_BxB_R, CHECK_ALIGN, sizeof(Bool), sizeof(Bool));

static void _floor_divide_BxB_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Bool *tin0   = (Bool *) ((char *) input  + inboffset);
    Bool *tout0 = (Bool *) ((char *) output + outboffset);
    Bool lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Bool *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Bool *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0)     : floor(lastval / (double) *tin0));
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _floor_divide_BxB_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  floor_divide_BxB_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _floor_divide_BxB_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(floor_divide_BxB_A, CHECK_ALIGN, sizeof(Bool), sizeof(Bool));

static int floor_divide_BBxB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool        tin0 = *(Bool *) buffers[0];
    Bool       *tin1 =  (Bool *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, 0)     : floor(tin0    / (double) *tin1));
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor floor_divide_BBxB_svxv_descr =
{ "floor_divide_BBxB_svxv", (void *) floor_divide_BBxB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  floor_divide  *********************/
/*********************  true_divide  *********************/

static int true_divide_BBxf_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Bool        tin1 = *(Bool *) buffers[1];
    Float32    *tout0 =  (Float32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = (( tin1==0) ? int_dividebyzero_error( tin1, *tin0) : *tin0         / (double) tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor true_divide_BBxf_vsxv_descr =
{ "true_divide_BBxf_vsxv", (void *) true_divide_BBxf_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Float32) }, { 0, 1, 0, 0 } };

static int true_divide_BBxf_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Bool       *tin1 =  (Bool *) buffers[1];
    Float32    *tout0 =  (Float32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, *tin0) : *tin0         / (double) *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor true_divide_BBxf_vvxv_descr =
{ "true_divide_BBxf_vvxv", (void *) true_divide_BBxf_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Float32) }, { 0, 0, 0, 0 } };

static void _true_divide_Bxf_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Bool  *tin0   = (Bool *) ((char *) input  + inboffset);
    Float32 *tout0  = (Float32 *) ((char *) output + outboffset);
    Float32 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Bool *) ((char *) tin0 + inbstrides[dim]);
            net    = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0)     : net           / (double) *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _true_divide_Bxf_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int true_divide_Bxf_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _true_divide_Bxf_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(true_divide_Bxf_R, CHECK_ALIGN, sizeof(Bool), sizeof(Float32));

static void _true_divide_Bxf_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Bool *tin0   = (Bool *) ((char *) input  + inboffset);
    Float32 *tout0 = (Float32 *) ((char *) output + outboffset);
    Float32 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Bool *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Float32 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0)     : lastval       / (double) *tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _true_divide_Bxf_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  true_divide_Bxf_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _true_divide_Bxf_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(true_divide_Bxf_A, CHECK_ALIGN, sizeof(Bool), sizeof(Float32));

static int true_divide_BBxf_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool        tin0 = *(Bool *) buffers[0];
    Bool       *tin1 =  (Bool *) buffers[1];
    Float32    *tout0 =  (Float32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, 0)     : tin0          / (double) *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor true_divide_BBxf_svxv_descr =
{ "true_divide_BBxf_svxv", (void *) true_divide_BBxf_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Float32) }, { 1, 0, 0, 0 } };
/*********************  true_divide  *********************/
/*********************  remainder  *********************/

static int remainder_BBxB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Bool        tin1 = *(Bool *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((tin1==0) ? int_dividebyzero_error(tin1, *tin0) : *tin0  %  tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor remainder_BBxB_vsxv_descr =
{ "remainder_BBxB_vsxv", (void *) remainder_BBxB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int remainder_BBxB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Bool       *tin1 =  (Bool *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, *tin0) : *tin0  %  *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor remainder_BBxB_vvxv_descr =
{ "remainder_BBxB_vvxv", (void *) remainder_BBxB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Bool) }, { 0, 0, 0, 0 } };

static void _remainder_BxB_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Bool  *tin0   = (Bool *) ((char *) input  + inboffset);
    Bool *tout0  = (Bool *) ((char *) output + outboffset);
    Bool net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Bool *) ((char *) tin0 + inbstrides[dim]);
            net = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0) : net  %  *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _remainder_BxB_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int remainder_BxB_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _remainder_BxB_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(remainder_BxB_R, CHECK_ALIGN, sizeof(Bool), sizeof(Bool));

static void _remainder_BxB_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Bool *tin0   = (Bool *) ((char *) input  + inboffset);
    Bool *tout0 = (Bool *) ((char *) output + outboffset);
    Bool lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Bool *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Bool *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0) : lastval  %  *tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _remainder_BxB_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  remainder_BxB_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _remainder_BxB_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(remainder_BxB_A, CHECK_ALIGN, sizeof(Bool), sizeof(Bool));

static int remainder_BBxB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool        tin0 = *(Bool *) buffers[0];
    Bool       *tin1 =  (Bool *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, 0) : tin0  %  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor remainder_BBxB_svxv_descr =
{ "remainder_BBxB_svxv", (void *) remainder_BBxB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  remainder  *********************/
/*********************  power  *********************/

static int power_BBx1_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Bool        tin1 = *(Bool *) buffers[1];
    Int8       *tout0 =  (Int8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_pow(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor power_BBx1_vsxf_descr =
{ "power_BBx1_vsxf", (void *) power_BBx1_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Int8) }, { 0, 1, 0, 0 } };

static int power_BBx1_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Bool       *tin1 =  (Bool *) buffers[1];
    Int8       *tout0 =  (Int8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_pow(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor power_BBx1_vvxf_descr =
{ "power_BBx1_vvxf", (void *) power_BBx1_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Int8) }, { 0, 0, 0, 0 } };

static void _power_Bx1_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Bool  *tin0   = (Bool *) ((char *) input  + inboffset);
    Int8 *tout0  = (Int8 *) ((char *) output + outboffset);
    Int8 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Bool *) ((char *) tin0 + inbstrides[dim]);
            net   = num_pow(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _power_Bx1_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int power_Bx1_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _power_Bx1_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(power_Bx1_R, CHECK_ALIGN, sizeof(Bool), sizeof(Int8));

static void _power_Bx1_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Bool *tin0   = (Bool *) ((char *) input  + inboffset);
    Int8 *tout0 = (Int8 *) ((char *) output + outboffset);
    Int8 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Bool *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Int8 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = num_pow(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _power_Bx1_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  power_Bx1_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _power_Bx1_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(power_Bx1_A, CHECK_ALIGN, sizeof(Bool), sizeof(Int8));

static int power_BBx1_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool        tin0 = *(Bool *) buffers[0];
    Bool       *tin1 =  (Bool *) buffers[1];
    Int8       *tout0 =  (Int8 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_pow(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor power_BBx1_svxf_descr =
{ "power_BBx1_svxf", (void *) power_BBx1_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Int8) }, { 1, 0, 0, 0 } };
/*********************  abs  *********************/

static int abs_Bx1_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Int8       *tout0 =  (Int8 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = fabs(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor abs_Bx1_vxf_descr =
{ "abs_Bx1_vxf", (void *) abs_Bx1_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Bool), sizeof(Int8) }, { 0, 0, 0 } };
/*********************  sin  *********************/

static int sin_Bxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = sin(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor sin_Bxd_vxf_descr =
{ "sin_Bxd_vxf", (void *) sin_Bxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Bool), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  cos  *********************/

static int cos_Bxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = cos(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor cos_Bxd_vxf_descr =
{ "cos_Bxd_vxf", (void *) cos_Bxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Bool), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  tan  *********************/

static int tan_Bxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = tan(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor tan_Bxd_vxf_descr =
{ "tan_Bxd_vxf", (void *) tan_Bxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Bool), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arcsin  *********************/

static int arcsin_Bxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = asin(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arcsin_Bxd_vxf_descr =
{ "arcsin_Bxd_vxf", (void *) arcsin_Bxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Bool), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arccos  *********************/

static int arccos_Bxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = acos(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arccos_Bxd_vxf_descr =
{ "arccos_Bxd_vxf", (void *) arccos_Bxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Bool), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arctan  *********************/

static int arctan_Bxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = atan(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arctan_Bxd_vxf_descr =
{ "arctan_Bxd_vxf", (void *) arctan_Bxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Bool), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arctan2  *********************/

static int arctan2_BBxd_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Bool        tin1 = *(Bool *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = atan2(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arctan2_BBxd_vsxf_descr =
{ "arctan2_BBxd_vsxf", (void *) arctan2_BBxd_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Float64) }, { 0, 1, 0, 0 } };

static int arctan2_BBxd_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Bool       *tin1 =  (Bool *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = atan2(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arctan2_BBxd_vvxf_descr =
{ "arctan2_BBxd_vvxf", (void *) arctan2_BBxd_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Float64) }, { 0, 0, 0, 0 } };

static void _arctan2_Bxd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Bool  *tin0   = (Bool *) ((char *) input  + inboffset);
    Float64 *tout0  = (Float64 *) ((char *) output + outboffset);
    Float64 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Bool *) ((char *) tin0 + inbstrides[dim]);
            net   = atan2(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _arctan2_Bxd_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int arctan2_Bxd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _arctan2_Bxd_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(arctan2_Bxd_R, CHECK_ALIGN, sizeof(Bool), sizeof(Float64));

static void _arctan2_Bxd_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Bool *tin0   = (Bool *) ((char *) input  + inboffset);
    Float64 *tout0 = (Float64 *) ((char *) output + outboffset);
    Float64 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Bool *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Float64 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = atan2(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _arctan2_Bxd_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  arctan2_Bxd_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _arctan2_Bxd_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(arctan2_Bxd_A, CHECK_ALIGN, sizeof(Bool), sizeof(Float64));

static int arctan2_BBxd_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool        tin0 = *(Bool *) buffers[0];
    Bool       *tin1 =  (Bool *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = atan2(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arctan2_BBxd_svxf_descr =
{ "arctan2_BBxd_svxf", (void *) arctan2_BBxd_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Float64) }, { 1, 0, 0, 0 } };
/*********************  log  *********************/

static int log_Bxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_log(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor log_Bxd_vxf_descr =
{ "log_Bxd_vxf", (void *) log_Bxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Bool), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  log10  *********************/

static int log10_Bxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_log10(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor log10_Bxd_vxf_descr =
{ "log10_Bxd_vxf", (void *) log10_Bxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Bool), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  exp  *********************/

static int exp_Bxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = exp(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor exp_Bxd_vxf_descr =
{ "exp_Bxd_vxf", (void *) exp_Bxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Bool), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  sinh  *********************/

static int sinh_Bxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = sinh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor sinh_Bxd_vxf_descr =
{ "sinh_Bxd_vxf", (void *) sinh_Bxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Bool), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  cosh  *********************/

static int cosh_Bxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = cosh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor cosh_Bxd_vxf_descr =
{ "cosh_Bxd_vxf", (void *) cosh_Bxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Bool), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  tanh  *********************/

static int tanh_Bxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = tanh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor tanh_Bxd_vxf_descr =
{ "tanh_Bxd_vxf", (void *) tanh_Bxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Bool), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arcsinh  *********************/

static int arcsinh_Bxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_asinh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arcsinh_Bxd_vxf_descr =
{ "arcsinh_Bxd_vxf", (void *) arcsinh_Bxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Bool), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arccosh  *********************/

static int arccosh_Bxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_acosh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arccosh_Bxd_vxf_descr =
{ "arccosh_Bxd_vxf", (void *) arccosh_Bxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Bool), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  arctanh  *********************/

static int arctanh_Bxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_atanh(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor arctanh_Bxd_vxf_descr =
{ "arctanh_Bxd_vxf", (void *) arctanh_Bxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Bool), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  ieeemask  *********************/
/*********************  ieeemask  *********************/
/*********************  isnan  *********************/
/*********************  isnan  *********************/
/*********************  isnan  *********************/
/*********************  isnan  *********************/
/*********************  sqrt  *********************/

static int sqrt_Bxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = sqrt(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor sqrt_Bxd_vxf_descr =
{ "sqrt_Bxd_vxf", (void *) sqrt_Bxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Bool), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  equal  *********************/

static int equal_BBxB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Bool        tin1 = *(Bool *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 == tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor equal_BBxB_vsxv_descr =
{ "equal_BBxB_vsxv", (void *) equal_BBxB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int equal_BBxB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Bool       *tin1 =  (Bool *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 == *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor equal_BBxB_vvxv_descr =
{ "equal_BBxB_vvxv", (void *) equal_BBxB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int equal_BBxB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool        tin0 = *(Bool *) buffers[0];
    Bool       *tin1 =  (Bool *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 == *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor equal_BBxB_svxv_descr =
{ "equal_BBxB_svxv", (void *) equal_BBxB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  not_equal  *********************/

static int not_equal_BBxB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Bool        tin1 = *(Bool *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 != tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor not_equal_BBxB_vsxv_descr =
{ "not_equal_BBxB_vsxv", (void *) not_equal_BBxB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int not_equal_BBxB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Bool       *tin1 =  (Bool *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 != *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor not_equal_BBxB_vvxv_descr =
{ "not_equal_BBxB_vvxv", (void *) not_equal_BBxB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int not_equal_BBxB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool        tin0 = *(Bool *) buffers[0];
    Bool       *tin1 =  (Bool *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 != *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor not_equal_BBxB_svxv_descr =
{ "not_equal_BBxB_svxv", (void *) not_equal_BBxB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  greater  *********************/

static int greater_BBxB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Bool        tin1 = *(Bool *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 > tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_BBxB_vsxv_descr =
{ "greater_BBxB_vsxv", (void *) greater_BBxB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int greater_BBxB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Bool       *tin1 =  (Bool *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 > *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_BBxB_vvxv_descr =
{ "greater_BBxB_vvxv", (void *) greater_BBxB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int greater_BBxB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool        tin0 = *(Bool *) buffers[0];
    Bool       *tin1 =  (Bool *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 > *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_BBxB_svxv_descr =
{ "greater_BBxB_svxv", (void *) greater_BBxB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  greater_equal  *********************/

static int greater_equal_BBxB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Bool        tin1 = *(Bool *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 >= tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_equal_BBxB_vsxv_descr =
{ "greater_equal_BBxB_vsxv", (void *) greater_equal_BBxB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int greater_equal_BBxB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Bool       *tin1 =  (Bool *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 >= *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_equal_BBxB_vvxv_descr =
{ "greater_equal_BBxB_vvxv", (void *) greater_equal_BBxB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int greater_equal_BBxB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool        tin0 = *(Bool *) buffers[0];
    Bool       *tin1 =  (Bool *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 >= *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor greater_equal_BBxB_svxv_descr =
{ "greater_equal_BBxB_svxv", (void *) greater_equal_BBxB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  less  *********************/

static int less_BBxB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Bool        tin1 = *(Bool *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 < tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_BBxB_vsxv_descr =
{ "less_BBxB_vsxv", (void *) less_BBxB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int less_BBxB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Bool       *tin1 =  (Bool *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 < *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_BBxB_vvxv_descr =
{ "less_BBxB_vvxv", (void *) less_BBxB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int less_BBxB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool        tin0 = *(Bool *) buffers[0];
    Bool       *tin1 =  (Bool *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 < *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_BBxB_svxv_descr =
{ "less_BBxB_svxv", (void *) less_BBxB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  less_equal  *********************/

static int less_equal_BBxB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Bool        tin1 = *(Bool *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 <= tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_equal_BBxB_vsxv_descr =
{ "less_equal_BBxB_vsxv", (void *) less_equal_BBxB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int less_equal_BBxB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Bool       *tin1 =  (Bool *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 <= *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_equal_BBxB_vvxv_descr =
{ "less_equal_BBxB_vvxv", (void *) less_equal_BBxB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int less_equal_BBxB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool        tin0 = *(Bool *) buffers[0];
    Bool       *tin1 =  (Bool *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 <= *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor less_equal_BBxB_svxv_descr =
{ "less_equal_BBxB_svxv", (void *) less_equal_BBxB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  logical_and  *********************/

static int logical_and_BBxB_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Bool        tin1 = *(Bool *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_and(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_and_BBxB_vsxf_descr =
{ "logical_and_BBxB_vsxf", (void *) logical_and_BBxB_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int logical_and_BBxB_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Bool       *tin1 =  (Bool *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_and(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_and_BBxB_vvxf_descr =
{ "logical_and_BBxB_vvxf", (void *) logical_and_BBxB_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int logical_and_BBxB_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool        tin0 = *(Bool *) buffers[0];
    Bool       *tin1 =  (Bool *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_and(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_and_BBxB_svxf_descr =
{ "logical_and_BBxB_svxf", (void *) logical_and_BBxB_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  logical_or  *********************/

static int logical_or_BBxB_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Bool        tin1 = *(Bool *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_or(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_or_BBxB_vsxf_descr =
{ "logical_or_BBxB_vsxf", (void *) logical_or_BBxB_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int logical_or_BBxB_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Bool       *tin1 =  (Bool *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_or(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_or_BBxB_vvxf_descr =
{ "logical_or_BBxB_vvxf", (void *) logical_or_BBxB_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int logical_or_BBxB_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool        tin0 = *(Bool *) buffers[0];
    Bool       *tin1 =  (Bool *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_or(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_or_BBxB_svxf_descr =
{ "logical_or_BBxB_svxf", (void *) logical_or_BBxB_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  logical_xor  *********************/

static int logical_xor_BBxB_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Bool        tin1 = *(Bool *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_xor(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_xor_BBxB_vsxf_descr =
{ "logical_xor_BBxB_vsxf", (void *) logical_xor_BBxB_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int logical_xor_BBxB_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Bool       *tin1 =  (Bool *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_xor(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_xor_BBxB_vvxf_descr =
{ "logical_xor_BBxB_vvxf", (void *) logical_xor_BBxB_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int logical_xor_BBxB_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool        tin0 = *(Bool *) buffers[0];
    Bool       *tin1 =  (Bool *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = logical_xor(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_xor_BBxB_svxf_descr =
{ "logical_xor_BBxB_svxf", (void *) logical_xor_BBxB_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  logical_and  *********************/

static void _logical_and_BxB_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Bool  *tin0   = (Bool *) ((char *) input  + inboffset);
    Bool *tout0  = (Bool *) ((char *) output + outboffset);
    Bool net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Bool *) ((char *) tin0 + inbstrides[dim]);
            net   = logical_and(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _logical_and_BxB_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int logical_and_BxB_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _logical_and_BxB_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(logical_and_BxB_R, CHECK_ALIGN, sizeof(Bool), sizeof(Bool));

static void _logical_and_BxB_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Bool *tin0   = (Bool *) ((char *) input  + inboffset);
    Bool *tout0 = (Bool *) ((char *) output + outboffset);
    Bool lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Bool *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Bool *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = logical_and(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _logical_and_BxB_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  logical_and_BxB_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _logical_and_BxB_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(logical_and_BxB_A, CHECK_ALIGN, sizeof(Bool), sizeof(Bool));
/*********************  logical_or  *********************/

static void _logical_or_BxB_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Bool  *tin0   = (Bool *) ((char *) input  + inboffset);
    Bool *tout0  = (Bool *) ((char *) output + outboffset);
    Bool net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Bool *) ((char *) tin0 + inbstrides[dim]);
            net   = logical_or(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _logical_or_BxB_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int logical_or_BxB_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _logical_or_BxB_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(logical_or_BxB_R, CHECK_ALIGN, sizeof(Bool), sizeof(Bool));

static void _logical_or_BxB_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Bool *tin0   = (Bool *) ((char *) input  + inboffset);
    Bool *tout0 = (Bool *) ((char *) output + outboffset);
    Bool lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Bool *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Bool *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = logical_or(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _logical_or_BxB_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  logical_or_BxB_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _logical_or_BxB_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(logical_or_BxB_A, CHECK_ALIGN, sizeof(Bool), sizeof(Bool));
/*********************  logical_xor  *********************/

static void _logical_xor_BxB_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Bool  *tin0   = (Bool *) ((char *) input  + inboffset);
    Bool *tout0  = (Bool *) ((char *) output + outboffset);
    Bool net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Bool *) ((char *) tin0 + inbstrides[dim]);
            net   = logical_xor(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _logical_xor_BxB_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int logical_xor_BxB_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _logical_xor_BxB_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(logical_xor_BxB_R, CHECK_ALIGN, sizeof(Bool), sizeof(Bool));

static void _logical_xor_BxB_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Bool *tin0   = (Bool *) ((char *) input  + inboffset);
    Bool *tout0 = (Bool *) ((char *) output + outboffset);
    Bool lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Bool *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Bool *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = logical_xor(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _logical_xor_BxB_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  logical_xor_BxB_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _logical_xor_BxB_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(logical_xor_BxB_A, CHECK_ALIGN, sizeof(Bool), sizeof(Bool));
/*********************  logical_not  *********************/

static int logical_not_BxB_vxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Bool       *tout0 =  (Bool *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = !*tin0;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor logical_not_BxB_vxv_descr =
{ "logical_not_BxB_vxv", (void *) logical_not_BxB_vxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Bool), sizeof(Bool) }, { 0, 0, 0 } };
/*********************  bitwise_and  *********************/

static int bitwise_and_BBxB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Bool        tin1 = *(Bool *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 & tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_and_BBxB_vsxv_descr =
{ "bitwise_and_BBxB_vsxv", (void *) bitwise_and_BBxB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int bitwise_and_BBxB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Bool       *tin1 =  (Bool *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 & *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_and_BBxB_vvxv_descr =
{ "bitwise_and_BBxB_vvxv", (void *) bitwise_and_BBxB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Bool) }, { 0, 0, 0, 0 } };

static void _bitwise_and_BxB_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Bool  *tin0   = (Bool *) ((char *) input  + inboffset);
    Bool *tout0  = (Bool *) ((char *) output + outboffset);
    Bool net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Bool *) ((char *) tin0 + inbstrides[dim]);
            net    =     net & *tin0;
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _bitwise_and_BxB_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int bitwise_and_BxB_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _bitwise_and_BxB_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(bitwise_and_BxB_R, CHECK_ALIGN, sizeof(Bool), sizeof(Bool));

static void _bitwise_and_BxB_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Bool *tin0   = (Bool *) ((char *) input  + inboffset);
    Bool *tout0 = (Bool *) ((char *) output + outboffset);
    Bool lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Bool *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Bool *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = lastval & *tin0;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _bitwise_and_BxB_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  bitwise_and_BxB_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _bitwise_and_BxB_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(bitwise_and_BxB_A, CHECK_ALIGN, sizeof(Bool), sizeof(Bool));

static int bitwise_and_BBxB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool        tin0 = *(Bool *) buffers[0];
    Bool       *tin1 =  (Bool *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 & *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_and_BBxB_svxv_descr =
{ "bitwise_and_BBxB_svxv", (void *) bitwise_and_BBxB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  bitwise_or  *********************/

static int bitwise_or_BBxB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Bool        tin1 = *(Bool *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 | tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_or_BBxB_vsxv_descr =
{ "bitwise_or_BBxB_vsxv", (void *) bitwise_or_BBxB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int bitwise_or_BBxB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Bool       *tin1 =  (Bool *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 | *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_or_BBxB_vvxv_descr =
{ "bitwise_or_BBxB_vvxv", (void *) bitwise_or_BBxB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Bool) }, { 0, 0, 0, 0 } };

static void _bitwise_or_BxB_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Bool  *tin0   = (Bool *) ((char *) input  + inboffset);
    Bool *tout0  = (Bool *) ((char *) output + outboffset);
    Bool net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Bool *) ((char *) tin0 + inbstrides[dim]);
            net    =     net | *tin0;
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _bitwise_or_BxB_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int bitwise_or_BxB_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _bitwise_or_BxB_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(bitwise_or_BxB_R, CHECK_ALIGN, sizeof(Bool), sizeof(Bool));

static void _bitwise_or_BxB_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Bool *tin0   = (Bool *) ((char *) input  + inboffset);
    Bool *tout0 = (Bool *) ((char *) output + outboffset);
    Bool lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Bool *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Bool *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = lastval | *tin0;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _bitwise_or_BxB_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  bitwise_or_BxB_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _bitwise_or_BxB_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(bitwise_or_BxB_A, CHECK_ALIGN, sizeof(Bool), sizeof(Bool));

static int bitwise_or_BBxB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool        tin0 = *(Bool *) buffers[0];
    Bool       *tin1 =  (Bool *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 | *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_or_BBxB_svxv_descr =
{ "bitwise_or_BBxB_svxv", (void *) bitwise_or_BBxB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  bitwise_xor  *********************/

static int bitwise_xor_BBxB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Bool        tin1 = *(Bool *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 ^ tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_xor_BBxB_vsxv_descr =
{ "bitwise_xor_BBxB_vsxv", (void *) bitwise_xor_BBxB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int bitwise_xor_BBxB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Bool       *tin1 =  (Bool *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 ^ *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_xor_BBxB_vvxv_descr =
{ "bitwise_xor_BBxB_vvxv", (void *) bitwise_xor_BBxB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Bool) }, { 0, 0, 0, 0 } };

static void _bitwise_xor_BxB_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Bool  *tin0   = (Bool *) ((char *) input  + inboffset);
    Bool *tout0  = (Bool *) ((char *) output + outboffset);
    Bool net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Bool *) ((char *) tin0 + inbstrides[dim]);
            net    =     net ^ *tin0;
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _bitwise_xor_BxB_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int bitwise_xor_BxB_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _bitwise_xor_BxB_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(bitwise_xor_BxB_R, CHECK_ALIGN, sizeof(Bool), sizeof(Bool));

static void _bitwise_xor_BxB_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Bool *tin0   = (Bool *) ((char *) input  + inboffset);
    Bool *tout0 = (Bool *) ((char *) output + outboffset);
    Bool lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Bool *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Bool *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = lastval ^ *tin0;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _bitwise_xor_BxB_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  bitwise_xor_BxB_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _bitwise_xor_BxB_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(bitwise_xor_BxB_A, CHECK_ALIGN, sizeof(Bool), sizeof(Bool));

static int bitwise_xor_BBxB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool        tin0 = *(Bool *) buffers[0];
    Bool       *tin1 =  (Bool *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 ^ *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_xor_BBxB_svxv_descr =
{ "bitwise_xor_BBxB_svxv", (void *) bitwise_xor_BBxB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  bitwise_not  *********************/
/*********************  bitwise_not  *********************/

static int bitwise_not_BxB_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Bool       *tout0 =  (Bool *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = BOOLEAN_BITWISE_NOT(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bitwise_not_BxB_vxf_descr =
{ "bitwise_not_BxB_vxf", (void *) bitwise_not_BxB_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Bool), sizeof(Bool) }, { 0, 0, 0 } };
/*********************  rshift  *********************/

static int rshift_BBxB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Bool        tin1 = *(Bool *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 >> tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor rshift_BBxB_vsxv_descr =
{ "rshift_BBxB_vsxv", (void *) rshift_BBxB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int rshift_BBxB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Bool       *tin1 =  (Bool *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 >> *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor rshift_BBxB_vvxv_descr =
{ "rshift_BBxB_vvxv", (void *) rshift_BBxB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int rshift_BBxB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool        tin0 = *(Bool *) buffers[0];
    Bool       *tin1 =  (Bool *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 >> *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor rshift_BBxB_svxv_descr =
{ "rshift_BBxB_svxv", (void *) rshift_BBxB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  lshift  *********************/

static int lshift_BBxB_vsxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Bool        tin1 = *(Bool *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 << tin1;
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor lshift_BBxB_vsxv_descr =
{ "lshift_BBxB_vsxv", (void *) lshift_BBxB_vsxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Bool) }, { 0, 1, 0, 0 } };

static int lshift_BBxB_vvxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Bool       *tin1 =  (Bool *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =   *tin0 << *tin1;
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor lshift_BBxB_vvxv_descr =
{ "lshift_BBxB_vvxv", (void *) lshift_BBxB_vvxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Bool) }, { 0, 0, 0, 0 } };

static int lshift_BBxB_svxv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool        tin0 = *(Bool *) buffers[0];
    Bool       *tin1 =  (Bool *) buffers[1];
    Bool       *tout0 =  (Bool *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 =    tin0 << *tin1;
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor lshift_BBxB_svxv_descr =
{ "lshift_BBxB_svxv", (void *) lshift_BBxB_svxv, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Bool) }, { 1, 0, 0, 0 } };
/*********************  floor  *********************/

static int floor_BxB_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Bool       *tout0 =  (Bool *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = (*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor floor_BxB_vxf_descr =
{ "floor_BxB_vxf", (void *) floor_BxB_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Bool), sizeof(Bool) }, { 0, 0, 0 } };
/*********************  floor  *********************/
/*********************  ceil  *********************/

static int ceil_BxB_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Bool       *tout0 =  (Bool *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = (*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor ceil_BxB_vxf_descr =
{ "ceil_BxB_vxf", (void *) ceil_BxB_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Bool), sizeof(Bool) }, { 0, 0, 0 } };
/*********************  ceil  *********************/
/*********************  maximum  *********************/

static int maximum_BBx1_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Bool        tin1 = *(Bool *) buffers[1];
    Int8       *tout0 =  (Int8 *) buffers[2];
Bool temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufmaximum(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor maximum_BBx1_vsxf_descr =
{ "maximum_BBx1_vsxf", (void *) maximum_BBx1_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Int8) }, { 0, 1, 0, 0 } };

static int maximum_BBx1_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Bool       *tin1 =  (Bool *) buffers[1];
    Int8       *tout0 =  (Int8 *) buffers[2];
Bool temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufmaximum(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor maximum_BBx1_vvxf_descr =
{ "maximum_BBx1_vvxf", (void *) maximum_BBx1_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Int8) }, { 0, 0, 0, 0 } };

static void _maximum_Bx1_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Bool  *tin0   = (Bool *) ((char *) input  + inboffset);
    Int8 *tout0  = (Int8 *) ((char *) output + outboffset);
    Int8 net;
    Bool temp1, temp2;
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Bool *) ((char *) tin0 + inbstrides[dim]);
            net   = ufmaximum(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _maximum_Bx1_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int maximum_Bx1_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _maximum_Bx1_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(maximum_Bx1_R, CHECK_ALIGN, sizeof(Bool), sizeof(Int8));

static void _maximum_Bx1_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Bool *tin0   = (Bool *) ((char *) input  + inboffset);
    Int8 *tout0 = (Int8 *) ((char *) output + outboffset);
    Int8 lastval;
    Bool temp1, temp2;
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Bool *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Int8 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = ufmaximum(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _maximum_Bx1_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  maximum_Bx1_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _maximum_Bx1_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(maximum_Bx1_A, CHECK_ALIGN, sizeof(Bool), sizeof(Int8));

static int maximum_BBx1_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool        tin0 = *(Bool *) buffers[0];
    Bool       *tin1 =  (Bool *) buffers[1];
    Int8       *tout0 =  (Int8 *) buffers[2];
Bool temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufmaximum(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor maximum_BBx1_svxf_descr =
{ "maximum_BBx1_svxf", (void *) maximum_BBx1_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Int8) }, { 1, 0, 0, 0 } };
/*********************  minimum  *********************/

static int minimum_BBx1_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Bool        tin1 = *(Bool *) buffers[1];
    Int8       *tout0 =  (Int8 *) buffers[2];
Bool temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufminimum(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor minimum_BBx1_vsxf_descr =
{ "minimum_BBx1_vsxf", (void *) minimum_BBx1_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Int8) }, { 0, 1, 0, 0 } };

static int minimum_BBx1_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Bool       *tin1 =  (Bool *) buffers[1];
    Int8       *tout0 =  (Int8 *) buffers[2];
Bool temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufminimum(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor minimum_BBx1_vvxf_descr =
{ "minimum_BBx1_vvxf", (void *) minimum_BBx1_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Int8) }, { 0, 0, 0, 0 } };

static void _minimum_Bx1_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Bool  *tin0   = (Bool *) ((char *) input  + inboffset);
    Int8 *tout0  = (Int8 *) ((char *) output + outboffset);
    Int8 net;
    Bool temp1, temp2;
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Bool *) ((char *) tin0 + inbstrides[dim]);
            net   = ufminimum(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _minimum_Bx1_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int minimum_Bx1_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _minimum_Bx1_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(minimum_Bx1_R, CHECK_ALIGN, sizeof(Bool), sizeof(Int8));

static void _minimum_Bx1_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Bool *tin0   = (Bool *) ((char *) input  + inboffset);
    Int8 *tout0 = (Int8 *) ((char *) output + outboffset);
    Int8 lastval;
    Bool temp1, temp2;
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Bool *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Int8 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = ufminimum(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _minimum_Bx1_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  minimum_Bx1_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _minimum_Bx1_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(minimum_Bx1_A, CHECK_ALIGN, sizeof(Bool), sizeof(Int8));

static int minimum_BBx1_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool        tin0 = *(Bool *) buffers[0];
    Bool       *tin1 =  (Bool *) buffers[1];
    Int8       *tout0 =  (Int8 *) buffers[2];
Bool temp1, temp2;
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = ufminimum(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor minimum_BBx1_svxf_descr =
{ "minimum_BBx1_svxf", (void *) minimum_BBx1_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Int8) }, { 1, 0, 0, 0 } };
/*********************  fabs  *********************/

static int fabs_Bxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = fabs(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor fabs_Bxd_vxf_descr =
{ "fabs_Bxd_vxf", (void *) fabs_Bxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Bool), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  _round  *********************/

static int _round_Bxd_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = num_round(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor _round_Bxd_vxf_descr =
{ "_round_Bxd_vxf", (void *) _round_Bxd_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(Bool), sizeof(Float64) }, { 0, 0, 0 } };
/*********************  hypot  *********************/

static int hypot_BBxd_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Bool        tin1 = *(Bool *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = hypot(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor hypot_BBxd_vsxf_descr =
{ "hypot_BBxd_vsxf", (void *) hypot_BBxd_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Float64) }, { 0, 1, 0, 0 } };

static int hypot_BBxd_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool       *tin0 =  (Bool *) buffers[0];
    Bool       *tin1 =  (Bool *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = hypot(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor hypot_BBxd_vvxf_descr =
{ "hypot_BBxd_vvxf", (void *) hypot_BBxd_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Float64) }, { 0, 0, 0, 0 } };

static void _hypot_Bxd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Bool  *tin0   = (Bool *) ((char *) input  + inboffset);
    Float64 *tout0  = (Float64 *) ((char *) output + outboffset);
    Float64 net;
    
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (Bool *) ((char *) tin0 + inbstrides[dim]);
            net   = hypot(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _hypot_Bxd_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int hypot_Bxd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _hypot_Bxd_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(hypot_Bxd_R, CHECK_ALIGN, sizeof(Bool), sizeof(Float64));

static void _hypot_Bxd_A(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    Bool *tin0   = (Bool *) ((char *) input  + inboffset);
    Float64 *tout0 = (Float64 *) ((char *) output + outboffset);
    Float64 lastval;
    
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (Bool *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (Float64 *) ((char *) tout0 + outbstrides[dim]);
            *tout0 = hypot(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _hypot_Bxd_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  hypot_Bxd_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _hypot_Bxd_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(hypot_Bxd_A, CHECK_ALIGN, sizeof(Bool), sizeof(Float64));

static int hypot_BBxd_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Bool        tin0 = *(Bool *) buffers[0];
    Bool       *tin1 =  (Bool *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = hypot(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor hypot_BBxd_svxf_descr =
{ "hypot_BBxd_svxf", (void *) hypot_BBxd_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Bool), sizeof(Bool), sizeof(Float64) }, { 1, 0, 0, 0 } };
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

static PyMethodDef _ufuncBoolMethods[] = {

	{NULL,      NULL}        /* Sentinel */
};

static PyObject *init_funcDict(void) {
    PyObject *dict, *keytuple, *cfunc;
    dict = PyDict_New();
    /* minus_Bx1_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","minus","v","Bool","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minus_Bx1_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_BBx1_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","add","vs","Bool","Bool","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_BBx1_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_BBx1_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","add","vv","Bool","Bool","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_BBx1_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_Bx1_R */
    keytuple=Py_BuildValue("ss((s)(s))","add","R","Bool","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_Bx1_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_Bx1_A */
    keytuple=Py_BuildValue("ss((s)(s))","add","A","Bool","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_Bx1_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* add_BBx1_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","add","sv","Bool","Bool","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&add_BBx1_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_BBx1_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","subtract","vs","Bool","Bool","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_BBx1_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_BBx1_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","subtract","vv","Bool","Bool","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_BBx1_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_Bx1_R */
    keytuple=Py_BuildValue("ss((s)(s))","subtract","R","Bool","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_Bx1_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_Bx1_A */
    keytuple=Py_BuildValue("ss((s)(s))","subtract","A","Bool","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_Bx1_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* subtract_BBx1_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","subtract","sv","Bool","Bool","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&subtract_BBx1_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_BBxB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","divide","vs","Bool","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_BBxB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_BBxB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","divide","vv","Bool","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_BBxB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_BxB_R */
    keytuple=Py_BuildValue("ss((s)(s))","divide","R","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_BxB_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_BxB_A */
    keytuple=Py_BuildValue("ss((s)(s))","divide","A","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_BxB_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* divide_BBxB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","divide","sv","Bool","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&divide_BBxB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_divide_BBxB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","floor_divide","vs","Bool","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_divide_BBxB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_divide_BBxB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","floor_divide","vv","Bool","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_divide_BBxB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_divide_BxB_R */
    keytuple=Py_BuildValue("ss((s)(s))","floor_divide","R","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_divide_BxB_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_divide_BxB_A */
    keytuple=Py_BuildValue("ss((s)(s))","floor_divide","A","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_divide_BxB_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_divide_BBxB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","floor_divide","sv","Bool","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_divide_BBxB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_BBxf_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","true_divide","vs","Bool","Bool","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_BBxf_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_BBxf_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","true_divide","vv","Bool","Bool","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_BBxf_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_Bxf_R */
    keytuple=Py_BuildValue("ss((s)(s))","true_divide","R","Bool","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_Bxf_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_Bxf_A */
    keytuple=Py_BuildValue("ss((s)(s))","true_divide","A","Bool","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_Bxf_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* true_divide_BBxf_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","true_divide","sv","Bool","Bool","Float32");
    cfunc = (PyObject*)NA_new_cfunc((void*)&true_divide_BBxf_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_BBxB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","remainder","vs","Bool","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_BBxB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_BBxB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","remainder","vv","Bool","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_BBxB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_BxB_R */
    keytuple=Py_BuildValue("ss((s)(s))","remainder","R","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_BxB_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_BxB_A */
    keytuple=Py_BuildValue("ss((s)(s))","remainder","A","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_BxB_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* remainder_BBxB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","remainder","sv","Bool","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&remainder_BBxB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_BBx1_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","power","vs","Bool","Bool","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_BBx1_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_BBx1_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","power","vv","Bool","Bool","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_BBx1_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_Bx1_R */
    keytuple=Py_BuildValue("ss((s)(s))","power","R","Bool","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_Bx1_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_Bx1_A */
    keytuple=Py_BuildValue("ss((s)(s))","power","A","Bool","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_Bx1_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* power_BBx1_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","power","sv","Bool","Bool","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&power_BBx1_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* abs_Bx1_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","abs","v","Bool","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&abs_Bx1_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* sin_Bxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","sin","v","Bool","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&sin_Bxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* cos_Bxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","cos","v","Bool","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&cos_Bxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* tan_Bxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","tan","v","Bool","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&tan_Bxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arcsin_Bxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arcsin","v","Bool","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arcsin_Bxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arccos_Bxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arccos","v","Bool","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arccos_Bxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan_Bxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arctan","v","Bool","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan_Bxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan2_BBxd_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","arctan2","vs","Bool","Bool","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan2_BBxd_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan2_BBxd_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","arctan2","vv","Bool","Bool","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan2_BBxd_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan2_Bxd_R */
    keytuple=Py_BuildValue("ss((s)(s))","arctan2","R","Bool","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan2_Bxd_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan2_Bxd_A */
    keytuple=Py_BuildValue("ss((s)(s))","arctan2","A","Bool","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan2_Bxd_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctan2_BBxd_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","arctan2","sv","Bool","Bool","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctan2_BBxd_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* log_Bxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","log","v","Bool","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&log_Bxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* log10_Bxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","log10","v","Bool","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&log10_Bxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* exp_Bxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","exp","v","Bool","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&exp_Bxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* sinh_Bxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","sinh","v","Bool","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&sinh_Bxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* cosh_Bxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","cosh","v","Bool","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&cosh_Bxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* tanh_Bxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","tanh","v","Bool","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&tanh_Bxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arcsinh_Bxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arcsinh","v","Bool","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arcsinh_Bxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arccosh_Bxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arccosh","v","Bool","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arccosh_Bxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* arctanh_Bxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","arctanh","v","Bool","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&arctanh_Bxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* sqrt_Bxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","sqrt","v","Bool","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&sqrt_Bxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* equal_BBxB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","equal","vs","Bool","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&equal_BBxB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* equal_BBxB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","equal","vv","Bool","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&equal_BBxB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* equal_BBxB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","equal","sv","Bool","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&equal_BBxB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* not_equal_BBxB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","not_equal","vs","Bool","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&not_equal_BBxB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* not_equal_BBxB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","not_equal","vv","Bool","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&not_equal_BBxB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* not_equal_BBxB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","not_equal","sv","Bool","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&not_equal_BBxB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_BBxB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater","vs","Bool","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_BBxB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_BBxB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater","vv","Bool","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_BBxB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_BBxB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater","sv","Bool","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_BBxB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_equal_BBxB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater_equal","vs","Bool","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_equal_BBxB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_equal_BBxB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater_equal","vv","Bool","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_equal_BBxB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* greater_equal_BBxB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","greater_equal","sv","Bool","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&greater_equal_BBxB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_BBxB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less","vs","Bool","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_BBxB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_BBxB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less","vv","Bool","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_BBxB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_BBxB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less","sv","Bool","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_BBxB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_equal_BBxB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less_equal","vs","Bool","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_equal_BBxB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_equal_BBxB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less_equal","vv","Bool","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_equal_BBxB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* less_equal_BBxB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","less_equal","sv","Bool","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&less_equal_BBxB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_and_BBxB_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_and","vs","Bool","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_and_BBxB_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_and_BBxB_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_and","vv","Bool","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_and_BBxB_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_and_BBxB_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_and","sv","Bool","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_and_BBxB_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_or_BBxB_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_or","vs","Bool","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_or_BBxB_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_or_BBxB_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_or","vv","Bool","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_or_BBxB_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_or_BBxB_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_or","sv","Bool","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_or_BBxB_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_xor_BBxB_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_xor","vs","Bool","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_xor_BBxB_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_xor_BBxB_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_xor","vv","Bool","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_xor_BBxB_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_xor_BBxB_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","logical_xor","sv","Bool","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_xor_BBxB_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_and_BxB_R */
    keytuple=Py_BuildValue("ss((s)(s))","logical_and","R","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_and_BxB_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_and_BxB_A */
    keytuple=Py_BuildValue("ss((s)(s))","logical_and","A","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_and_BxB_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_or_BxB_R */
    keytuple=Py_BuildValue("ss((s)(s))","logical_or","R","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_or_BxB_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_or_BxB_A */
    keytuple=Py_BuildValue("ss((s)(s))","logical_or","A","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_or_BxB_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_xor_BxB_R */
    keytuple=Py_BuildValue("ss((s)(s))","logical_xor","R","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_xor_BxB_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_xor_BxB_A */
    keytuple=Py_BuildValue("ss((s)(s))","logical_xor","A","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_xor_BxB_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* logical_not_BxB_vxv */
    keytuple=Py_BuildValue("ss((s)(s))","logical_not","v","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&logical_not_BxB_vxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_and_BBxB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_and","vs","Bool","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_and_BBxB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_and_BBxB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_and","vv","Bool","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_and_BBxB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_and_BxB_R */
    keytuple=Py_BuildValue("ss((s)(s))","bitwise_and","R","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_and_BxB_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_and_BxB_A */
    keytuple=Py_BuildValue("ss((s)(s))","bitwise_and","A","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_and_BxB_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_and_BBxB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_and","sv","Bool","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_and_BBxB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_or_BBxB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_or","vs","Bool","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_or_BBxB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_or_BBxB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_or","vv","Bool","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_or_BBxB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_or_BxB_R */
    keytuple=Py_BuildValue("ss((s)(s))","bitwise_or","R","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_or_BxB_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_or_BxB_A */
    keytuple=Py_BuildValue("ss((s)(s))","bitwise_or","A","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_or_BxB_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_or_BBxB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_or","sv","Bool","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_or_BBxB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_xor_BBxB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_xor","vs","Bool","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_xor_BBxB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_xor_BBxB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_xor","vv","Bool","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_xor_BBxB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_xor_BxB_R */
    keytuple=Py_BuildValue("ss((s)(s))","bitwise_xor","R","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_xor_BxB_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_xor_BxB_A */
    keytuple=Py_BuildValue("ss((s)(s))","bitwise_xor","A","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_xor_BxB_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_xor_BBxB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","bitwise_xor","sv","Bool","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_xor_BBxB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* bitwise_not_BxB_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","bitwise_not","v","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&bitwise_not_BxB_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* rshift_BBxB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","rshift","vs","Bool","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&rshift_BBxB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* rshift_BBxB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","rshift","vv","Bool","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&rshift_BBxB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* rshift_BBxB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","rshift","sv","Bool","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&rshift_BBxB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* lshift_BBxB_vsxv */
    keytuple=Py_BuildValue("ss((ss)(s))","lshift","vs","Bool","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&lshift_BBxB_vsxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* lshift_BBxB_vvxv */
    keytuple=Py_BuildValue("ss((ss)(s))","lshift","vv","Bool","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&lshift_BBxB_vvxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* lshift_BBxB_svxv */
    keytuple=Py_BuildValue("ss((ss)(s))","lshift","sv","Bool","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&lshift_BBxB_svxv_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* floor_BxB_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","floor","v","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&floor_BxB_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* ceil_BxB_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","ceil","v","Bool","Bool");
    cfunc = (PyObject*)NA_new_cfunc((void*)&ceil_BxB_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_BBx1_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","maximum","vs","Bool","Bool","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_BBx1_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_BBx1_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","maximum","vv","Bool","Bool","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_BBx1_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_Bx1_R */
    keytuple=Py_BuildValue("ss((s)(s))","maximum","R","Bool","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_Bx1_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_Bx1_A */
    keytuple=Py_BuildValue("ss((s)(s))","maximum","A","Bool","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_Bx1_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* maximum_BBx1_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","maximum","sv","Bool","Bool","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&maximum_BBx1_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_BBx1_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","minimum","vs","Bool","Bool","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_BBx1_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_BBx1_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","minimum","vv","Bool","Bool","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_BBx1_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_Bx1_R */
    keytuple=Py_BuildValue("ss((s)(s))","minimum","R","Bool","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_Bx1_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_Bx1_A */
    keytuple=Py_BuildValue("ss((s)(s))","minimum","A","Bool","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_Bx1_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* minimum_BBx1_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","minimum","sv","Bool","Bool","Int8");
    cfunc = (PyObject*)NA_new_cfunc((void*)&minimum_BBx1_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* fabs_Bxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","fabs","v","Bool","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&fabs_Bxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* _round_Bxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","_round","v","Bool","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&_round_Bxd_vxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_BBxd_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","hypot","vs","Bool","Bool","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_BBxd_vsxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_BBxd_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","hypot","vv","Bool","Bool","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_BBxd_vvxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_Bxd_R */
    keytuple=Py_BuildValue("ss((s)(s))","hypot","R","Bool","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_Bxd_R_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_Bxd_A */
    keytuple=Py_BuildValue("ss((s)(s))","hypot","A","Bool","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_Bxd_A_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    /* hypot_BBxd_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","hypot","sv","Bool","Bool","Float64");
    cfunc = (PyObject*)NA_new_cfunc((void*)&hypot_BBxd_svxf_descr);
    PyDict_SetItem(dict,keytuple,cfunc);

    Py_DECREF(keytuple);
    Py_DECREF(cfunc);

    return dict;
}

/* platform independent*/
#ifdef MS_WIN32
__declspec(dllexport)
#endif
void init_ufuncBool(void) {
    PyObject *m, *d, *functions;
    m = Py_InitModule("_ufuncBool", _ufuncBoolMethods);
    d = PyModule_GetDict(m);
    import_libnumarray();
    functions = init_funcDict();
    PyDict_SetItemString(d, "functionDict", functions);
    Py_DECREF(functions);
    ADD_VERSION(m);
}
