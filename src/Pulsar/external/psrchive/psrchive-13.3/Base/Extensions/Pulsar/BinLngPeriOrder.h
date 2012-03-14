//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Aidan Hotan
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
                                                                                
/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/BinLngPeriOrder.h,v $
   $Revision: 1.4 $
   $Date: 2006/10/06 21:05:50 $
   $Author: straten $ */
                                                                                
#ifndef __BinLngPeriOrder_h
#define __BinLngPeriOrder_h
                                                                                
#include "Pulsar/IntegrationOrder.h"
                                                                            
namespace Pulsar {
  
  /*! This BinLngPeriOrder class provides a means for developers to
    re-index the way Integrations are stored. It is a virtual base class
    that must be inherited by developers who wish to define a new way to
    index the Integrations in their Archive */
  
  class BinLngPeriOrder : public Pulsar::IntegrationOrder {
                                                                                
  public:
    
    // These are the standard Archive::Extension methods:

    //! Default constructor
    BinLngPeriOrder ();
    
    //! Destructor
    ~BinLngPeriOrder ();
    
    //! Copy constructor
    BinLngPeriOrder (const BinLngPeriOrder& extension);
    
    //! Operator =
    const BinLngPeriOrder& operator= (const BinLngPeriOrder& extension);
    
    //! Clone method
    IntegrationOrder* clone () const;
    
    void organise (Archive*, unsigned);
    void append   (Archive*, const Archive*);    
    void combine  (Archive*, unsigned);

  };                                                                            
  
}
                                                                                
#endif
