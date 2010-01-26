//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/PlotFactory.h,v $
   $Revision: 1.9 $
   $Date: 2007/08/17 06:10:02 $
   $Author: straten $ */

#ifndef __Pulsar_PlotFactory_h
#define __Pulsar_PlotFactory_h

#include <string>
#include <vector>

namespace Pulsar {

  class Plot;

  //! Provides a text interface to get and set Plot attributes
  class PlotFactory {

  public:

    //! Default constructor loads up all of the plot types
    PlotFactory ();

    //! Return a new instance of the named plot
    Plot* construct (std::string name);

    //! Return the number of plots
    unsigned get_nplot () { return agents.size(); }

    //! Return the name of the ith plot
    std::string get_name (unsigned i);

    //! Return the name of the ith plot
    std::string get_description (unsigned i);

    //! Return a descriptive list of the available plots
    std::string help();

  protected:

    class Agent;
    
    std::vector<Agent*> agents;

  public:

    template<class P> class Advocate;

    //! Add a plot
    void add (Agent* agent) { agents.push_back(agent); }

  };

  class PlotFactory::Agent
  {
  public:

    Agent (char c, std::string n, std::string d)
      : shortcut(c), name (n), description (d) {}

    //! Virtual destructor
    virtual ~Agent () { }

    //! Return a new instance of Plot class
    virtual Plot* construct () = 0;

    //! Return the name of the Plot class
    std::string get_name () { return name; }

    //! Return the shortcut name of the Plot class
    char get_shortcut () { return shortcut; }

    //! Return a description of the Plot class
    std::string get_description () { return description; }

  protected:

    char shortcut;
    std::string name;
    std::string description;

  };

  template<class P> class PlotFactory::Advocate : public Agent
  {  
  public:

    //! Construct with name and description
    Advocate (std::string _name, std::string _description)
      : Agent (' ', _name, _description) {}
 
    //! Construct with shortcut, name and description
    Advocate (char shortcut, std::string _name, std::string _description)
      : Agent (shortcut, _name, _description) {}
 
    //! Return a new instance of Plot class
    Plot* construct () { return new P; }

  };

}

#endif
