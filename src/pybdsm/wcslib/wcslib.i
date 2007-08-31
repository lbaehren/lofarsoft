%module wcslib

%{
#include "wcs.h"
#include "wcsunits.h"
%}

%include "wcs.h"
%include "wcsunits.h"

%extend wcsprm {
  wcsprm() {
    struct wcsprm *w = (struct wcsprm *)calloc(1, sizeof(struct wcsprm));    
    //printf("Initializing wcsprm (%p)\n", w);
    w->flag = -1;     // Only for the first wcsini
    wcsini(1, 2, w);
    return w;
  }

  ~wcsprm() {
    //printf("Releasing wcsprm (%p)\n", self);
    wcsfree(self);
    free(self);
  }

  void pprint() {
    wcsprt(self);
  }

  void set_crval(double v1, double v2) {
    //printf("SET crval = (%f, %f)\n", v1, v2);
    self->crval[0] = v1;
    self->crval[1] = v2;
  }

  void set_crpix(double v1, double v2) {
    //printf("SET crpix = (%f, %f)\n", v1, v2);
    self->crpix[0] = v1;
    self->crpix[1] = v2;
  }

  void set_cdelt(double v1, double v2) {
    //printf("SET cdelt = (%f, %f)\n", v1, v2);
    self->cdelt[0] = v1;
    self->cdelt[1] = v2;
  }

  void set_crota(double v1, double v2) {
    //printf("SET crota = (%f, %f)\n", v1, v2);
    self->crota[0] = v1;
    self->crota[1] = v2;
  }

  void set_ctype(const char *v1, const char *v2) {
    //printf("SET ctype = (%s, %s)\n", v1, v2);
    strncpy(self->ctype[0], v1, 72);
    strncpy(self->ctype[1], v2, 72);
  }

  void set_cunit(const char *v1, const char *v2) {
    //printf("SET cunit = (%s, %s)\n", v1, v2);
    strncpy(self->cunit[0], v1, 72);
    strncpy(self->cunit[1], v2, 72);
  }
  
  PyObject *wcsset() {
    int err = wcsset(self);

    if (err)
      {
	PyErr_Format(PyExc_RuntimeError, "wcsset error: %d: %s",
		     err, wcs_errmsg[err]);
	return NULL;
      }

    return Py_INCREF(Py_None), Py_None;
  }

  PyObject *xy2radec(double x1, double x2) {
    double pixcrd[2];
    double imgcrd[2];
    double phi, theta;
    double world[2];
    int stat, err;

    pixcrd[0] = x1 + 1; // C & Python indices are counted from 0
    pixcrd[1] = x2 + 1;
    err = wcsp2s(self, 1, 2, pixcrd, imgcrd, &phi, &theta, world, &stat);

    /*printf("Pixel2sky conversion:\n"
	   "pixcrd: %f %f\n"
	   "imgcrd: %f %f\n"
	   "ph,th : %f %f\n"
	   "world : %f %f\n"
	   "stat  : %d\n",
	   pixcrd[0], pixcrd[1], imgcrd[0], imgcrd[1],
	   phi, theta, world[0], world[1], stat);*/

    if (err)
      {
	PyErr_Format(PyExc_RuntimeError, "wcsp2s error: %d: %s",
		     err, wcs_errmsg[err]);
	return NULL;
      }

    return Py_BuildValue("(f,f)", world[0], world[1]);
  }

  PyObject *radec2xy(double x1, double x2) {
    double pixcrd[2];
    double imgcrd[2];
    double phi, theta;
    double world[2];
    int stat, err;

    world[0] = x1;
    world[1] = x2;
    err = wcss2p(self, 1, 2, world, &phi, &theta, imgcrd, pixcrd, &stat);

    /*printf("Pixel2sky conversion:\n"
	   "pixcrd: %f %f\n"
	   "imgcrd: %f %f\n"
	   "ph,th : %f %f\n"
	   "world : %f %f\n"
	   "stat  : %d\n",
	   pixcrd[0], pixcrd[1], imgcrd[0], imgcrd[1],
	   phi, theta, world[0], world[1], stat);*/

    if (err)
      {
	PyErr_Format(PyExc_RuntimeError, "wcss2p error: %d: %s",
		     err, wcs_errmsg[err]);
	return NULL;
      }

                           // C & Python indices are counted from 0
    return Py_BuildValue("(f,f)", pixcrd[0] - 1, pixcrd[1] - 1);
  }
}
