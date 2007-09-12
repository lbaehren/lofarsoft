
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

#include "airy.h"
static int airy_dxdddd_vxvvvv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64    *tout0 =  (Float64 *) buffers[1];
    Float64    *tout1 =  (Float64 *) buffers[2];
    Float64    *tout2 =  (Float64 *) buffers[3];
    Float64    *tout3 =  (Float64 *) buffers[4];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        airy(*tin0, tout0, tout1, tout2, tout3);
        
	++tin0; ++tout0; ++tout1; ++tout2; ++tout3; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor airy_dxdddd_vxvvvv_descr =
{ "airy_dxdddd_vxvvvv", (void *) airy_dxdddd_vxvvvv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 4,
  { sizeof(Float64), sizeof(Float64), sizeof(Float64), sizeof(Float64), sizeof(Float64) }, { 0, 0, 0, 0, 0, 0 } };

static int airy_fxffff_vxvvvv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32    *tout0 =  (Float32 *) buffers[1];
    Float32    *tout1 =  (Float32 *) buffers[2];
    Float32    *tout2 =  (Float32 *) buffers[3];
    Float32    *tout3 =  (Float32 *) buffers[4];
    Float64     result0;
    Float64     result1;
    Float64     result2;
    Float64     result3;

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        airy(*tin0, &result0, &result1, &result2, &result3);
                *tout0 = result0;
        *tout1 = result1;
        *tout2 = result2;
        *tout3 = result3;

	++tin0; ++tout0; ++tout1; ++tout2; ++tout3; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor airy_fxffff_vxvvvv_descr =
{ "airy_fxffff_vxvvvv", (void *) airy_fxffff_vxvvvv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 4,
  { sizeof(Float32), sizeof(Float32), sizeof(Float32), sizeof(Float32), sizeof(Float32) }, { 0, 0, 0, 0, 0, 0 } };

static int airy_DxDDDD_vxvvvv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex64  *tin0 =  (Complex64 *) buffers[0];
    Complex64  *tout0 =  (Complex64 *) buffers[1];
    Complex64  *tout1 =  (Complex64 *) buffers[2];
    Complex64  *tout2 =  (Complex64 *) buffers[3];
    Complex64  *tout3 =  (Complex64 *) buffers[4];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        cairy_fake(*tin0, tout0, tout1, tout2, tout3);
        
	++tin0; ++tout0; ++tout1; ++tout2; ++tout3; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor airy_DxDDDD_vxvvvv_descr =
{ "airy_DxDDDD_vxvvvv", (void *) airy_DxDDDD_vxvvvv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 4,
  { sizeof(Complex64), sizeof(Complex64), sizeof(Complex64), sizeof(Complex64), sizeof(Complex64) }, { 0, 0, 0, 0, 0, 0 } };

static int airy_FxFFFF_vxvvvv(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Complex32  *tin0 =  (Complex32 *) buffers[0];
    Complex32  *tout0 =  (Complex32 *) buffers[1];
    Complex32  *tout1 =  (Complex32 *) buffers[2];
    Complex32  *tout2 =  (Complex32 *) buffers[3];
    Complex32  *tout3 =  (Complex32 *) buffers[4];
    Complex64     input0;
    Complex64     result0;
    Complex64     result1;
    Complex64     result2;
    Complex64     result3;

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
                input0.r = tin0->r;
        input0.i = tin0->i;

        cairy_fake(input0, &result0, &result1, &result2, &result3);
                tout0->r = result0.r;
        tout0->i = result0.i;
        tout1->r = result1.r;
        tout1->i = result1.i;
        tout2->r = result2.r;
        tout2->i = result2.i;
        tout3->r = result3.r;
        tout3->i = result3.i;

	++tin0; ++tout0; ++tout1; ++tout2; ++tout3; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor airy_FxFFFF_vxvvvv_descr =
{ "airy_FxFFFF_vxvvvv", (void *) airy_FxFFFF_vxvvvv, CFUNC_UFUNC, 0, CHECK_ALIGN, 1, 4,
  { sizeof(Complex32), sizeof(Complex32), sizeof(Complex32), sizeof(Complex32), sizeof(Complex32) }, { 0, 0, 0, 0, 0, 0 } };

static double
vsdemo(double v, double s1, double s2, double s3, double s4)
{
     return v + s1 + s2 + s3 + s4;
}

static int vsdemo_dddddxd_vvvvvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64    *tin1 =  (Float64 *) buffers[1];
    Float64    *tin2 =  (Float64 *) buffers[2];
    Float64    *tin3 =  (Float64 *) buffers[3];
    Float64    *tin4 =  (Float64 *) buffers[4];
    Float64    *tout0 =  (Float64 *) buffers[5];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = vsdemo(*tin0, *tin1, *tin2, *tin3, *tin4);
        
	++tin0; ++tin1; ++tin2; ++tin3; ++tin4; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor vsdemo_dddddxd_vvvvvxf_descr =
{ "vsdemo_dddddxd_vvvvvxf", (void *) vsdemo_dddddxd_vvvvvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 5, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Float64), sizeof(Float64), sizeof(Float64), sizeof(Float64) }, { 0, 0, 0, 0, 0, 0, 0 } };

static int vsdemo_dddddxd_vssssxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float64    *tin0 =  (Float64 *) buffers[0];
    Float64     tin1 = *(Float64 *) buffers[1];
    Float64     tin2 = *(Float64 *) buffers[2];
    Float64     tin3 = *(Float64 *) buffers[3];
    Float64     tin4 = *(Float64 *) buffers[4];
    Float64    *tout0 =  (Float64 *) buffers[5];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = vsdemo(*tin0, tin1, tin2, tin3, tin4);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor vsdemo_dddddxd_vssssxf_descr =
{ "vsdemo_dddddxd_vssssxf", (void *) vsdemo_dddddxd_vssssxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 5, 1,
  { sizeof(Float64), sizeof(Float64), sizeof(Float64), sizeof(Float64), sizeof(Float64), sizeof(Float64) }, { 0, 1, 1, 1, 1, 0, 0 } };

static int vsdemo_fffffxd_vvvvvxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32    *tin1 =  (Float32 *) buffers[1];
    Float32    *tin2 =  (Float32 *) buffers[2];
    Float32    *tin3 =  (Float32 *) buffers[3];
    Float32    *tin4 =  (Float32 *) buffers[4];
    Float64    *tout0 =  (Float64 *) buffers[5];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = vsdemo(*tin0, *tin1, *tin2, *tin3, *tin4);
        
	++tin0; ++tin1; ++tin2; ++tin3; ++tin4; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor vsdemo_fffffxd_vvvvvxf_descr =
{ "vsdemo_fffffxd_vvvvvxf", (void *) vsdemo_fffffxd_vvvvvxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 5, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Float32), sizeof(Float32), sizeof(Float32), sizeof(Float64) }, { 0, 0, 0, 0, 0, 0, 0 } };

static int vsdemo_fffffxd_vssssxf(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    Float32    *tin0 =  (Float32 *) buffers[0];
    Float32     tin1 = *(Float32 *) buffers[1];
    Float32     tin2 = *(Float32 *) buffers[2];
    Float32     tin3 = *(Float32 *) buffers[3];
    Float32     tin4 = *(Float32 *) buffers[4];
    Float64    *tout0 =  (Float64 *) buffers[5];

    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        
        *tout0 = vsdemo(*tin0, tin1, tin2, tin3, tin4);
        
	++tin0; ++tout0; 
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor vsdemo_fffffxd_vssssxf_descr =
{ "vsdemo_fffffxd_vssssxf", (void *) vsdemo_fffffxd_vssssxf, CFUNC_UFUNC, 0, CHECK_ALIGN, 5, 1,
  { sizeof(Float32), sizeof(Float32), sizeof(Float32), sizeof(Float32), sizeof(Float32), sizeof(Float64) }, { 0, 1, 1, 1, 1, 0, 0 } };

static PyMethodDef _na_specialMethods[] = {

	{NULL,      NULL}        /* Sentinel */
};

static PyObject *init_funcDict(void) {
    PyObject *dict, *keytuple;
    dict = PyDict_New();
    /* airy_dxdddd_vxvvvv */
    keytuple=Py_BuildValue("ss((s)(ssss))","airy","v","Float64","Float64","Float64","Float64","Float64");
    PyDict_SetItem(dict,keytuple,
                   (PyObject*)NA_new_cfunc((void*)&airy_dxdddd_vxvvvv_descr));

    /* airy_fxffff_vxvvvv */
    keytuple=Py_BuildValue("ss((s)(ssss))","airy","v","Float32","Float32","Float32","Float32","Float32");
    PyDict_SetItem(dict,keytuple,
                   (PyObject*)NA_new_cfunc((void*)&airy_fxffff_vxvvvv_descr));

    /* airy_DxDDDD_vxvvvv */
    keytuple=Py_BuildValue("ss((s)(ssss))","airy","v","Complex64","Complex64","Complex64","Complex64","Complex64");
    PyDict_SetItem(dict,keytuple,
                   (PyObject*)NA_new_cfunc((void*)&airy_DxDDDD_vxvvvv_descr));

    /* airy_FxFFFF_vxvvvv */
    keytuple=Py_BuildValue("ss((s)(ssss))","airy","v","Complex32","Complex32","Complex32","Complex32","Complex32");
    PyDict_SetItem(dict,keytuple,
                   (PyObject*)NA_new_cfunc((void*)&airy_FxFFFF_vxvvvv_descr));

    /* vsdemo_dddddxd_vvvvvxf */
    keytuple=Py_BuildValue("ss((sssss)(s))","vsdemo","vvvvv","Float64","Float64","Float64","Float64","Float64","Float64");
    PyDict_SetItem(dict,keytuple,
                   (PyObject*)NA_new_cfunc((void*)&vsdemo_dddddxd_vvvvvxf_descr));

    /* vsdemo_dddddxd_vssssxf */
    keytuple=Py_BuildValue("ss((sssss)(s))","vsdemo","vssss","Float64","Float64","Float64","Float64","Float64","Float64");
    PyDict_SetItem(dict,keytuple,
                   (PyObject*)NA_new_cfunc((void*)&vsdemo_dddddxd_vssssxf_descr));

    /* vsdemo_fffffxd_vvvvvxf */
    keytuple=Py_BuildValue("ss((sssss)(s))","vsdemo","vvvvv","Float32","Float32","Float32","Float32","Float32","Float64");
    PyDict_SetItem(dict,keytuple,
                   (PyObject*)NA_new_cfunc((void*)&vsdemo_fffffxd_vvvvvxf_descr));

    /* vsdemo_fffffxd_vssssxf */
    keytuple=Py_BuildValue("ss((sssss)(s))","vsdemo","vssss","Float32","Float32","Float32","Float32","Float32","Float64");
    PyDict_SetItem(dict,keytuple,
                   (PyObject*)NA_new_cfunc((void*)&vsdemo_fffffxd_vssssxf_descr));

    return dict;
}

/* platform independent*/
#ifdef MS_WIN32
__declspec(dllexport)
#endif
void init_na_special(void) {
    PyObject *m, *d;
    m = Py_InitModule("_na_special", _na_specialMethods);
    d = PyModule_GetDict(m);
    import_libnumarray();
    PyDict_SetItemString(d, "functionDict", init_funcDict());
    ADD_VERSION(m);
}
