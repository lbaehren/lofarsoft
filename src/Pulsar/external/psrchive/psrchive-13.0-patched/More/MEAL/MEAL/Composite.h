//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Composite.h,v $
   $Revision: 1.12 $
   $Date: 2008/06/15 16:12:34 $
   $Author: straten $ */

#ifndef __Composite_H
#define __Composite_H

#include "MEAL/Function.h"
#include "MEAL/Projection.h"

namespace MEAL {

  //! Parameter policy for composite functions
  class Composite : public ParameterPolicy {

  public:

    //! Default constructor
    Composite (Function* context);

    //! Clone constructor
    Composite* clone (Function* context) const { return 0; }

    //! Get the number of models mapped into this model
    unsigned get_nmodel () const;

    //! Get the name used in verbose output
    std::string class_name() const;

    // ///////////////////////////////////////////////////////////////////
    //
    // ParameterPolicy implementation
    //
    // ///////////////////////////////////////////////////////////////////
 
    //! Return the number of parameters
    unsigned get_nparam () const;

    //! Return the name of the specified parameter
    std::string get_name (unsigned index) const;

    //! Return the name of the specified parameter
    std::string get_description (unsigned index) const;

    //! Return the value of the specified parameter
    double get_param (unsigned index) const;

    //! Set the value of the specified parameter
    void set_param (unsigned index, double value);

    //! Return the variance of the specified parameter
    double get_variance (unsigned index) const;

    //! Set the variance of the specified parameter
    void set_variance (unsigned index, double variance);

    //! Return true if parameter at index is to be fitted
    bool get_infit (unsigned index) const;

    //! Set flag for parameter at index to be fitted
    void set_infit (unsigned index, bool flag);

    // ///////////////////////////////////////////////////////////////////
    //
    // ArgumentPolicy implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Set the independent variable of the specified dimension
    void set_argument (unsigned dimension, Argument* axis);

    //! Convenience interface to map (Projection*)
    template <class Type>
    void map (Project<Type>& model)
    {
      if (Function::very_verbose)
	std::cerr << class_name() + "map (Project<Type>)" << std::endl;
      map (model.get_map());
    }

    //! Convenience interface to unmap (Projection*)
    template <class Type>
    void unmap (Project<Type>& model)
    {
      if (Function::very_verbose)
	std::cerr << class_name() + "unmap (Project<Type>)" << std::endl;
      unmap (model.get_map());
    }

    //! Map the Projection into the composite mappting
    void map (Projection* model);

    //! Remove the Projection from the composite mapping
    void unmap (Projection* model);

    //! Get the mapping for the given Function
    void get_imap (const Function* model,
		   std::vector<unsigned>::iterator& imap) const;

    //! Method called when a Function attribute has changed
    void attribute_changed (Function::Attribute attribute);

  protected:

    //! Provide access to Projection base class
    friend class Projection;

    //! Return the index for the specified model
    unsigned find_Function (Function* model) const;

    //! Return the index for the specified model
    unsigned find_Projection (Projection* model) const;

  private:

    //! References to Projection instances
    std::vector< Reference::To<Projection> > maps;

    //! References to Function instances
    std::vector< Reference::To<Function> > models;
    
    //! The total number of Function parameters
    mutable unsigned nparameters;
    
    //! Optimization: keep track of the current model
    mutable unsigned current_model;

    //! Optimization: keep track of the base index of the current model
    mutable unsigned current_index;

    //! Flag set when remap is needed
    mutable bool remap_needed;

    //! Flag set to temporarily disable callbacks
    mutable bool disable_callbacks;

    //! Flag set when this is shared by Function passed to add_component
    mutable bool component_shares_this;

    //! Recursive function does the work for map
    void add_component (Function*, std::vector<unsigned>& imap);

    //! Recursive function does the work for unmap
    void remove_component (Function*);

    //! Remap the parameter indeces
    void remap ();

    //! Set flag to remap
    void remap_later ();

    //! Send callbacks when changes are made
    void callbacks ();

    //! Recount the number of parameters
    void recount () const;

    //! Get the const Function that corresponds to the given index
    const Function* get_Function (unsigned& index) const;

    //! Get the Function that corresponds to the given index
    Function* get_Function (unsigned& index);

    //! Check the the reference to the specified model is still valid
    void reference_check (unsigned imodel, char* method) const;

  };

  void get_imap (const Function* composite, const Function* component,
		 std::vector<unsigned>& imap);

}


#endif

