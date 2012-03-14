//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 by Aidan Hotan
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
                                                                                
/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/IntegrationOrder.h,v $
   $Revision: 1.13 $
   $Date: 2008/11/07 22:15:17 $
   $Author: straten $ */
                                                                                
#ifndef __IntegrationOrder_h
#define __IntegrationOrder_h

#include "Pulsar/ArchiveExtension.h"

namespace Pulsar {
  
  //! Alternative ways of ordering Integration instances
  /*! This class provides a means for developers to re-index the way
    Integrations are stored. It is a virtual base class that must be
    inherited by developers who wish to define a new way to index the
    Integrations in their Archive */
  
  class IntegrationOrder : public Archive::Extension {
                                                                                
  public:
    
    // These are the standard Archive::Extension methods:

    //! Default constructor
    IntegrationOrder (const char* name);
    
    //! Copy constructor
    IntegrationOrder (const IntegrationOrder& extension);
    
    //! Operator =
    const IntegrationOrder& operator= (const IntegrationOrder& extension);
    
    //! Destructor
    virtual ~IntegrationOrder ();
    
    //! Clone method
    virtual IntegrationOrder* clone () const = 0;
    
    //! Return a string describing the type of indexing used
    std::string get_IndexState () const;
    
    //! Return a string describing the units of this state
    std::string get_Unit () const;

    //! Return the size of the index array
    unsigned size() const;

    //! Change the size of the index array
    void resize(unsigned i);
    
    //! Erase the specified index
    void erase (unsigned i);

    //! Set the custom index value associated with an Integration
    void set_Index (unsigned subint, double i);
    
    //! Get the custom index value associated with an Integration
    double get_Index (unsigned subint) const;

    // These are the virtual base methods of the IntegrationOrder class:

    //! Re-order the Integrations into the desired state
    virtual void organise (Archive*, unsigned) = 0;
    
    //! Add two Archive instances that have the same IntegrationOrder
    virtual void append (Archive*, const Archive*) = 0;
    
    /*! Scrunch together this many consecutive Integrations in an Archive 
      with this IntegrationOrder */
    virtual void combine (Archive*, unsigned) = 0;

    //! Return a short name
    std::string get_short_name () const { return "order"; }

  protected:

    //! An index state identifier string
    std::string IndexState;

    //! The units (if any) of this state
    std::string Unit;
    
    //! Storage for the actual Integration indices
    std::vector<double> indices;                                                    

  };                                                                            
                                                                                
}
                                                                                
#endif
