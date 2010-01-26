//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/ProjectGradient.h,v $
   $Revision: 1.7 $
   $Date: 2008/06/15 16:12:34 $
   $Author: straten $ */

#ifndef __ProjectProductGradient_H
#define __ProjectProductGradient_H

#include "MEAL/Projection.h"

namespace MEAL {

  template <class Function, class Grad>
  void ProjectGradient (const Project<Function>& model, unsigned& igrad,
			const std::vector<Grad>& input,
			std::vector<Grad>& output)
  {
    unsigned nparam = model->get_nparam();
    
    for (unsigned iparam = 0; iparam < nparam; iparam++)
    {
      unsigned imap = model.get_map()->get_imap (iparam);

      if (Function::verbose)
	std::cerr << "ProjectGradient iparam=" << iparam << " imap=" << imap
	     << " igrad=" << igrad << std::endl;

      if (imap >= output.size())
	throw Error (InvalidRange, "MEAL::ProjectGradient",
		     "iparam=%u -> imap=%u >= composite.nparam=%u",
		     iparam, imap, output.size());

      output[imap] += input[igrad];
      
      igrad ++;
    }
  }

  template <class Function, class Grad>
  void ProjectGradient (const Project<Function>& model,
			const std::vector<Grad>& input,
			std::vector<Grad>& output)
  {
    unsigned igrad = 0;
    ProjectGradient (model, igrad, input, output);
  }

  template <class Function, class Grad>
  void ProjectGradient (const std::vector<Project<Function> >& model,
			const std::vector<Grad>& input,
			std::vector<Grad>& output)
  {
    unsigned nparam = output.size();
    unsigned nmodel = model.size();
    
    // set each element of the gradient to zero
    for (unsigned iparam=0; iparam<nparam; iparam++)
      output[iparam] = 0.0;
    
    unsigned igrad = 0;
    
    for (unsigned imodel=0; imodel<nmodel; imodel++)
    {
      if (Function::verbose)
	std::cerr << "ProjectGradient imodel=" << imodel 
	     << " igrad=" << igrad << std::endl;

      ProjectGradient (model[imodel], igrad, input, output);
    }

    // sanity check, ensure that all elements have been set
    if (igrad != input.size())
      throw Error (InvalidState, "MEAL::ProjectGradient",
		   "on completion igrad=%d != ngrad=%d",
		   igrad, input.size());
  }
}

#endif
