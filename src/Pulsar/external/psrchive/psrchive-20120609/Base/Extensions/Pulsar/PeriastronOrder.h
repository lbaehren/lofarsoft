//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 by Aidan Hotan
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
                                                                                
/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/PeriastronOrder.h,v $
   $Revision: 1.5 $
   $Date: 2006/10/06 21:05:50 $
   $Author: straten $ */
                                                                                
#ifndef __PeriastronOrder_h
#define __PeriastronOrder_h
                                                                                
#include "Pulsar/IntegrationOrder.h"
                                                                            
namespace Pulsar {
  
  /*! This PeriastronOrder class provides a means for developers to
    re-index the way Integrations are stored. It is a virtual base class
    that must be inherited by developers who wish to define a new way to
    index the Integrations in their Archive */
  
  class PeriastronOrder : public Pulsar::IntegrationOrder {
                                                                                
  public:
    
    // These are the standard Archive::Extension methods:

    //! Default constructor
    PeriastronOrder ();
    
    //! Destructor
    ~PeriastronOrder ();
    
    //! Copy constructor
    PeriastronOrder (const PeriastronOrder& extension);
    
    //! Operator =
    const PeriastronOrder& operator= (const PeriastronOrder& extension);
    
    //! Clone method
    IntegrationOrder* clone () const;
    
    void organise (Archive*, unsigned);
    void append   (Archive*, const Archive*);    
    void combine  (Archive*, unsigned);

  };                                                                            
  
}
                                                                                
#endif
