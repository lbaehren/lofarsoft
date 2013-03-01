//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/ExampleArchive.h,v $
   $Revision: 1.13 $
   $Date: 2008/11/07 22:15:36 $
   $Author: straten $ */

#ifndef __ExampleArchive_h
#define __ExampleArchive_h

#include "Pulsar/BasicArchive.h"
#include "Pulsar/Agent.h"

namespace Pulsar {

  //! Loads and unloads Example Pulsar archives
  /*! This ExampleArchive class provides an example of how to inherit the
    BasicArchive class and add functionality specific to a file format. 
    By copying the files ExampleArchive.h and ExampleArchive.C and performing
    simple text-substitution, the skeleton of a new file-format plugin may
    be easily developed. */
  class ExampleArchive : public BasicArchive {

  public:
    
    //! Default constructor
    ExampleArchive ();

    //! Copy constructor
    ExampleArchive (const ExampleArchive& archive);

    //! Destructor
    ~ExampleArchive ();

    //! Assignment operator
    const ExampleArchive& operator = (const ExampleArchive& archive);
    
    //! Base copy constructor
    ExampleArchive (const Archive& archive);

    //! Copy all of the class attributes and the selected Integration data
    void copy (const Archive& archive);

    //! Return a new copy-constructed ExampleArchive instance
    ExampleArchive* clone () const;

  protected:

    //! Load the Example header information from filename
    virtual void load_header (const char* filename);

    //! Load the specified Integration from filename, returning new instance
    virtual Integration*
    load_Integration (const char* filename, unsigned subint);

    //! The unload_file method is not implemented
    bool can_unload () const { return false; }

    //! Unload the ExampleArchive (header and Integration data) to filename
    virtual void unload_file (const char* filename) const;

    // Advocates the use of the ExampleArchive plugin
    class Agent;

    //! Enable Advocate template class to access protected Agent class
    friend class Archive::Advocate<ExampleArchive>;

  private:

    //! Initialize all values to null
    void init ();

  };
 

  // Advocates the use of the ExampleArchive plugin
  class ExampleArchive::Agent : public Archive::Advocate<ExampleArchive> {
    
  public:
    
    Agent () { } 
    
    //! Advocate the use of ExampleArchive to interpret filename
    bool advocate (const char* filename);
    
    //! Return the name of the ExampleArchive plugin
    std::string get_name () { return "ExampleArchive"; }
    
    //! Return description of this plugin
    std::string get_description ();
    
  };

}

#endif
