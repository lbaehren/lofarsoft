#ifndef __defined_RecRadioBasic_hh
#define __defined_RecRadioBasic_hh

//!RecRadio headers
#include "RecRadioCalib.hh"
#include "RecRadioTrigger.hh"
#include "RecRadioUtil.hh"


/*!
  \file RecRadioBasic.hh
  \ingroup startools
  \brief basic functions of LOPES-STAR for external lib support

 \date    Feb 15 2008
 \author  Thomas Asch
 
 to do
 - extract basic functions from LOPES-STAR libSTARutils
 - define interfaces wihtout dependence of STAR syntax
 - check everything well :-)
*/



//! subtract the pedestal of a given trace (type: short int)
/*!
\param window_size is the length of the trace pointer (e.g. 1024 or 1k). 
\param trace is just the pointer to raw data trace.
\param start and \param stop define the length to average over the samples
\param stop is set to window_size/2-100 if nothing is defined
*/
void SubtractPedestal(int window_size, short int *trace, int start = 0, int stop = -1);


//! subtract the pedestal of a given trace (type: float)
/*!
same as above with the trace of type float
*/

void SubtractPedestal(int window_size, float *trace, int start = 0, int stop = -1);


#endif

