//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __FTransform_test_libraries_h_
#define __FTransform_test_libraries_h_

#include "FTransform.h"

//! Defines a single interface to a variety of Fourier transform libraries
namespace FTransform {

  template<typename Method>
  void test_libraries (Method runtest, const char* test_name)
  {

    unsigned nlib = FTransform::get_num_libraries ();

    for (unsigned ilib=0; ilib < nlib; ilib++) {

      std::string name = FTransform::get_library_name (ilib);
      
      FTransform::set_library (name);

      std::cerr << "\n"
	"************************************************************\n"
	"************************************************************\n"
	"********\n"
	"********\n"
	"\n"
	"Test of " << test_name << " with "
	   << FTransform::get_library() << std::endl <<
	"\n"
	"********\n"
	"********\n"
	"************************************************************\n"
	"************************************************************\n"
	"\n"
	   << std::endl;
      
      runtest ();
      
    }

  }

}

#endif

    
