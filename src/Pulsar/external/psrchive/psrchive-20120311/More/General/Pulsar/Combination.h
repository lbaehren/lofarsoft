//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/Combination.h,v $
   $Revision: 1.1 $
   $Date: 2007/10/12 02:45:40 $
   $Author: straten $ */

#ifndef __Pulsar_Combination_h
#define __Pulsar_Combination_h

#include "Pulsar/Transformation.h"

namespace Pulsar {

  //! Transformations that modify data in the Container
  template<class Container> class Combination
    : public Transformation<Container>
  {
  public:

    //! Set the Container from which data will be taken during tranformation
    void set_operand (const Container* o) { operand = o; }

    //! Get the Container from which data will be taken during transformation
    const Container* get_operand () const { return operand; }

  protected:

    //!
    Reference::To<const Container> operand;

  };

}

#endif
