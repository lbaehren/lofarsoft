
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



static double c_bessel(double x, double y)
{
      double x2=x*x, y2=y*y, diff=x2-y2;
      return diff*diff/(x2+y2);
}

static UInt8  c_bar(UInt32 x )
{
      return (x >> 24) & 0xFF;
}


extern int airy ( double x, double *ai, double *aip, double *bi, double *bip );


static int bar_uxb_vxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    UInt32     *tin0 =  (UInt32 *) buffers[0];
    UInt8      *tout0 =  (UInt8 *) buffers[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = c_bar(*tin0);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bar_uxb_vxf_descr =
{ "bar_uxb_vxf", (void *) bar_uxb_vxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 1,
  { sizeof(UInt32), sizeof(UInt8) }, { 0, 0, 0 } };

static int bessel_ffxf_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32     tin1 = *(Float32 *) buffers[1];
    Float32    *tout0 =  (Float32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = c_bessel(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bessel_ffxf_vsxf_descr =
{ "bessel_ffxf_vsxf", (void *) bessel_ffxf_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Float32) }, { 0, 1, 0, 0 } };

static int bessel_ffxf_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32    *tin1 =  (Float32 *) buffers[1];
    Float32    *tout0 =  (Float32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = c_bessel(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bessel_ffxf_vvxf_descr =
{ "bessel_ffxf_vvxf", (void *) bessel_ffxf_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Float32) }, { 0, 0, 0, 0 } };

static void _bessel_ffxf_R(long dim, long dummy, maybelong *niters,
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
            net   = c_bessel(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _bessel_ffxf_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int bessel_ffxf_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _bessel_ffxf_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(bessel_ffxf_R, CHECK_ALIGN, sizeof(Float32), sizeof(Float32));

static void _bessel_ffxf_A(long dim, long dummy, maybelong *niters,
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
            *tout0 = c_bessel(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _bessel_ffxf_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  bessel_ffxf_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _bessel_ffxf_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(bessel_ffxf_A, CHECK_ALIGN, sizeof(Float32), sizeof(Float32));

static int bessel_ffxf_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32     tin0 = *(Float32 *) buffers[0];
    Float32    *tin1 =  (Float32 *) buffers[1];
    Float32    *tout0 =  (Float32 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = c_bessel(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bessel_ffxf_svxf_descr =
{ "bessel_ffxf_svxf", (void *) bessel_ffxf_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Float32) }, { 1, 0, 0, 0 } };

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

static int bessel_ddxd_vsxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64     tin1 = *(Float64 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = c_bessel(*tin0, tin1);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bessel_ddxd_vsxf_descr =
{ "bessel_ddxd_vsxf", (void *) bessel_ddxd_vsxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Float64) }, { 0, 1, 0, 0 } };

static int bessel_ddxd_vvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64    *tin1 =  (Float64 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = c_bessel(*tin0, *tin1);
        
	++tin0; ++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bessel_ddxd_vvxf_descr =
{ "bessel_ddxd_vvxf", (void *) bessel_ddxd_vvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Float64) }, { 0, 0, 0, 0 } };

static void _bessel_ddxd_R(long dim, long dummy, maybelong *niters,
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
            net   = c_bessel(net, *tin0);
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _bessel_ddxd_R(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int bessel_ddxd_R(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _bessel_ddxd_R(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(bessel_ddxd_R, CHECK_ALIGN, sizeof(Float64), sizeof(Float64));

static void _bessel_ddxd_A(long dim, long dummy, maybelong *niters,
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
            *tout0 = c_bessel(lastval ,*tin0);
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _bessel_ddxd_A(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  bessel_ddxd_A(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _bessel_ddxd_A(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(bessel_ddxd_A, CHECK_ALIGN, sizeof(Float64), sizeof(Float64));

static int bessel_ddxd_svxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64     tin0 = *(Float64 *) buffers[0];
    Float64    *tin1 =  (Float64 *) buffers[1];
    Float64    *tout0 =  (Float64 *) buffers[2];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = c_bessel(tin0,  *tin1);
        
	++tin1; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor bessel_ddxd_svxf_descr =
{ "bessel_ddxd_svxf", (void *) bessel_ddxd_svxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 2, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Float64) }, { 1, 0, 0, 0 } };

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

static PyMethodDef _fooMethods[] = {

	{NULL,      NULL}        /* Sentinel */
};

static PyObject *init_funcDict(void) {
    PyObject *dict, *keytuple;
    dict = PyDict_New();
    /* bar_uxb_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","bar","v","UInt32","UInt8");
    PyDict_SetItem(dict,keytuple,
                   (PyObject*)NA_new_cfunc((void*)&bar_uxb_vxf_descr));

    /* bessel_ffxf_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","bessel","vs","Float32","Float32","Float32");
    PyDict_SetItem(dict,keytuple,
                   (PyObject*)NA_new_cfunc((void*)&bessel_ffxf_vsxf_descr));

    /* bessel_ffxf_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","bessel","vv","Float32","Float32","Float32");
    PyDict_SetItem(dict,keytuple,
                   (PyObject*)NA_new_cfunc((void*)&bessel_ffxf_vvxf_descr));

    /* bessel_ffxf_R */
    keytuple=Py_BuildValue("ss((ss)(s))","bessel","R","Float32","Float32","Float32");
    PyDict_SetItem(dict,keytuple,
                   (PyObject*)NA_new_cfunc((void*)&bessel_ffxf_R_descr));

    /* bessel_ffxf_A */
    keytuple=Py_BuildValue("ss((ss)(s))","bessel","A","Float32","Float32","Float32");
    PyDict_SetItem(dict,keytuple,
                   (PyObject*)NA_new_cfunc((void*)&bessel_ffxf_A_descr));

    /* bessel_ffxf_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","bessel","sv","Float32","Float32","Float32");
    PyDict_SetItem(dict,keytuple,
                   (PyObject*)NA_new_cfunc((void*)&bessel_ffxf_svxf_descr));

    /* cos_fxf_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","cos","v","Float32","Float32");
    PyDict_SetItem(dict,keytuple,
                   (PyObject*)NA_new_cfunc((void*)&cos_fxf_vxf_descr));

    /* bessel_ddxd_vsxf */
    keytuple=Py_BuildValue("ss((ss)(s))","bessel","vs","Float64","Float64","Float64");
    PyDict_SetItem(dict,keytuple,
                   (PyObject*)NA_new_cfunc((void*)&bessel_ddxd_vsxf_descr));

    /* bessel_ddxd_vvxf */
    keytuple=Py_BuildValue("ss((ss)(s))","bessel","vv","Float64","Float64","Float64");
    PyDict_SetItem(dict,keytuple,
                   (PyObject*)NA_new_cfunc((void*)&bessel_ddxd_vvxf_descr));

    /* bessel_ddxd_R */
    keytuple=Py_BuildValue("ss((ss)(s))","bessel","R","Float64","Float64","Float64");
    PyDict_SetItem(dict,keytuple,
                   (PyObject*)NA_new_cfunc((void*)&bessel_ddxd_R_descr));

    /* bessel_ddxd_A */
    keytuple=Py_BuildValue("ss((ss)(s))","bessel","A","Float64","Float64","Float64");
    PyDict_SetItem(dict,keytuple,
                   (PyObject*)NA_new_cfunc((void*)&bessel_ddxd_A_descr));

    /* bessel_ddxd_svxf */
    keytuple=Py_BuildValue("ss((ss)(s))","bessel","sv","Float64","Float64","Float64");
    PyDict_SetItem(dict,keytuple,
                   (PyObject*)NA_new_cfunc((void*)&bessel_ddxd_svxf_descr));

    /* cos_dxd_vxf */
    keytuple=Py_BuildValue("ss((s)(s))","cos","v","Float64","Float64");
    PyDict_SetItem(dict,keytuple,
                   (PyObject*)NA_new_cfunc((void*)&cos_dxd_vxf_descr));

    return dict;
}

/* platform independent*/
#ifdef MS_WIN32
__declspec(dllexport)
#endif
void init_foo(void) {
    PyObject *m, *d;
    m = Py_InitModule("_foo", _fooMethods);
    d = PyModule_GetDict(m);
    import_libnumarray();
    PyDict_SetItemString(d, "functionDict", init_funcDict());
}
