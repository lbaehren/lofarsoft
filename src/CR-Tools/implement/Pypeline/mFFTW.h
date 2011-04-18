/**************************************************************************
 *  Header file of the FFTW module for the CR Pipeline Python bindings.    *
 *                                                                        *
 *  Copyright (c) 2010                                                    *
 *                                                                        *
 *  P.Schellart@astro.ru.nl <p.schellart@astro.ru.nl>                     *
 *                                                                        *
 *  This library is free software: you can redistribute it and/or modify  *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation, either version 3 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        *
 *  This library is distributed in the hope that it will be useful,       *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *  GNU General Public License for more details.                          *
 *                                                                        *
 *  You should have received a copy of the GNU General Public License     *
 *  along with this library. If not, see <http://www.gnu.org/licenses/>.  *
 **************************************************************************/

#ifndef CR_PIPELINE_FFTW_H
#define CR_PIPELINE_FFTW_H

#include "core.h"
#include "mArray.h"
#include "mModule.h"

#include <fftw3.h>

enum fftw_flags {
  ESTIMATE = FFTW_ESTIMATE,
  MEASURE = FFTW_MEASURE,
  PATIENT = FFTW_PATIENT,
  EXHAUSTIVE = FFTW_EXHAUSTIVE
};

enum fftw_sign {
  FORWARD = FFTW_FORWARD,
  BACKWARD = FFTW_BACKWARD
};

class FFTWPlanManyDft {

  fftw_plan p;

public:

  // === Variables =========================================================
  int isize;
  int osize;
  fftw_complex *in;
  fftw_complex *out;

  // === Methods ===========================================================
  FFTWPlanManyDft (int N, int howmany, int istride, int idist, int ostride, int odist, enum fftw_sign sign, enum fftw_flags flags);

  void execute() { fftw_execute(p); };

  // === Operators =========================================================
  friend std::ostream& operator<<(std::ostream& output, const FFTWPlanManyDft& d);

};

class FFTWPlanManyDftR2c {

  fftw_plan p;

public:

  // === Variables =========================================================
  int isize;
  int osize;
  double *in;
  fftw_complex *out;

  // === Methods ===========================================================
  FFTWPlanManyDftR2c (int N, int howmany, int istride, int idist, int ostride, int odist, enum fftw_flags flags);

  void execute() { fftw_execute(p); };

  // === Operators =========================================================
  friend std::ostream& operator<<(std::ostream& output, const FFTWPlanManyDftR2c& d);

};

class FFTWPlanManyDftC2r {

  fftw_plan p;

public:

  // === Variables =========================================================
  int isize;
  int osize;
  fftw_complex *in;
  double *out;

  // === Methods ===========================================================
  FFTWPlanManyDftC2r (int N, int howmany, int istride, int idist, int ostride, int odist, enum fftw_flags flags);

  void execute() { fftw_execute(p); };

  // === Operators =========================================================
  friend std::ostream& operator<<(std::ostream& output, const FFTWPlanManyDftC2r& d);

};

// ________________________________________________________________________
//                                    Add declarations of wrapper functions

// Tell the wrapper preprocessor that this is a c++ header file
#undef HFPP_FILETYPE
//-----------------------
#define HFPP_FILETYPE hFILE
//-----------------------
#include "mFFTW.def.h"

#endif /* CR_PIPELINE_FFTW_H */

