//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2012 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __Pulsar_IntegrationBarycentre_h
#define __Pulsar_IntegrationBarycentre_h

#include "Reference.h"
#include "Barycentre.h"

namespace Pulsar {

  class Integration;

  //! Convenience interface to Barycentre class
  class IntegrationBarycentre : public Reference::Able {

  public:

    //! Default constructor
    IntegrationBarycentre ();

    //! Construct with given integration
    IntegrationBarycentre (const Integration*);

    //! Set the Integration for which to compute barycenter values
    void set_Integration (const Integration* subint);

    //! Return Doppler factor
    double get_Doppler() const;

    //! Return barycentric subint epoch
    MJD get_barycentric_epoch() const;

  protected:

    Reference::To<const Integration> subint;

    Barycentre bary;

  };

}


#endif // !defined __Pulsar_PhaseWeight_h
