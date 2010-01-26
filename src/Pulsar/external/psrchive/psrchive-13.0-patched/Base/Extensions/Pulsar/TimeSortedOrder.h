//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Haydon Knight
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
                                                                                                                                                                
#ifndef __TimeSortedOrder_h
#define __TimeSortedOrder_h
                                                                                
#include "Pulsar/ArchiveExtension.h"
#include "Pulsar/Integration.h"
#include "Pulsar/IntegrationOrder.h"
                                                                            
namespace Pulsar {
  
  class TimeSortedOrder : public Pulsar::IntegrationOrder {
                                                                                
  public:
    
    // These are the standard Archive::Extension methods:

    //! Default constructor
    TimeSortedOrder ();
    
    //! Destructor
    ~TimeSortedOrder ();
    
    //! Copy constructor
    TimeSortedOrder (const TimeSortedOrder& extension);
    
    //! Operator =
    const TimeSortedOrder& operator= (const TimeSortedOrder& extension);
    
    //! Clone method
    IntegrationOrder* clone () const;
    
    void organise (Archive*, unsigned);
    void append   (Archive*, const Archive*);    
    void combine  (Archive*, unsigned);

  };                                                                            
  
}
                                                                                
#endif
