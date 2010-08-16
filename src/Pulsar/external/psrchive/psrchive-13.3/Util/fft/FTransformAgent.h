//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __FTransformAgent_h_
#define __FTransformAgent_h_

#include "FTransformPlan.h"
#include "ThreadContext.h"

namespace FTransform {

  //! Base class of one-dimensional FFT agents
  class Agent : public Reference::Able {

  public:

    Agent();

    virtual ~Agent();

    //! Name of the transform library
    std::string name;

    //! The normalization type
    normalization norm;

    //! Return an appropriate one-dimensional FFT plan from this library
    virtual Plan* get_plan (size_t nfft, type call) = 0;

    //! Return an appropriate two-dimensional FFT plan from this library
    virtual Plan2* get_plan2 (size_t nx, size_t ny, type call) = 0;

    //! Clean up the plans for this library
    virtual void clean_plans () = 0;

    //! Get the number of available libraries
    static unsigned get_num_libraries ()
    { return libraries.size(); }

    //! Get the name of the ith available library
    static std::string get_library_name (unsigned i) 
    { return libraries[i]->name; }

    //! Set the current library by name
    static void set_library (const std::string& name);

    //! The currently installed agent
    static Reference::To<Agent> current;

    //! For use in multithreaded programs
    static ThreadContext* context;

  protected:

    //! Add a pointer to this instance to the libraries attribute
    void add ();

  private:

    //! List of all libraries
    static std::vector< Reference::To<Agent> > libraries;

    friend void set_library (const std::string& name);
    friend void clean_plans();

  };


  //! Template virtual base class of FFT library agents
  /*! To use this template, the Library class must have nested classes
    named Plan, Plan2, and Agent */

  template <class Library>
  class PlanAgent : public Agent {

  public:

    //! Default constructor
    PlanAgent (const std::string& name, normalization norm);

    //! Return an appropriate plan from this library
    typename Library::Plan* get_plan (size_t nfft, type call);

    //! Return an appropriate two-dimensional FFT plan from this library
    typename Library::Plan2* get_plan2 (size_t nx, size_t ny, type call);

    //! Clean up the plans for this library
    void clean_plans ();

    //! Add an instance of this class to the Agent::libraries attribute
    static void enlist ();

  protected:

    //! The one-dimensional plans managed by the agent for this library
    std::vector< Reference::To<typename Library::Plan> > plans;

    //! The two-dimensional plans managed by the agent for this library
    std::vector< Reference::To<typename Library::Plan2> > plans2;

  };

  //! Clean up the plans for this library
  template<class Library>
  void PlanAgent<Library>::clean_plans ()
  {
    plans.resize (0);
  }

  template<class Library>
  PlanAgent<Library>::PlanAgent (const std::string& _name, normalization _norm)
  {
    name = _name;
    norm = _norm;
  }

  template<class Library>
  typename Library::Plan*
  PlanAgent<Library>::get_plan (size_t nfft, type t)
  {
    ThreadContext::Lock lock (Agent::context);

    for (unsigned iplan=0; iplan<plans.size(); iplan++)
      if (plans[iplan]->matches (nfft, t))
	return plans[iplan];
    
    plans.push_back( new typename Library::Plan (nfft, t) );
    return plans.back();
  }

  template<class Library> 
  typename Library::Plan2* 
  PlanAgent<Library>::get_plan2 (size_t nx, size_t ny, type t)
  {
    for (unsigned iplan=0; iplan<plans.size(); iplan++)
      if (plans2[iplan]->matches (nx, ny, t))
	return plans2[iplan];
    
    plans2.push_back( new typename Library::Plan2 (nx, ny, t) );
    return plans2.back();
    
  }

  template<class Library>
  void PlanAgent<Library>::enlist ()
  {
    PlanAgent<Library>* instance = new typename Library::Agent;
    instance->add();
  }

}

#endif
