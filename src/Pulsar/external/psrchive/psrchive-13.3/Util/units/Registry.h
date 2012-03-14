//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/units/Registry.h,v $
   $Revision: 1.5 $
   $Date: 2007/11/28 05:18:24 $
   $Author: straten $ */

#ifndef __Registry_h
#define __Registry_h

#include <vector>
#include <iostream>

namespace Registry {

  template<class Parent> class List;

  //! Pure virtual template base class of Registry::List<Parent>::Enter<Child>
  template<class Parent>
  class Entry
  {
    public:

    friend class List<Parent>;

    //! Return the pointer to the instance
    Parent* get () const { return instance; }

    //! Null constructor
    Entry () { instance = 0; }

  protected:

    //! Adds this instance to the Parent::Registry::List<Parent>
    void register_child (Parent* _instance) 
    {
      if (List<Parent>::verbose)
        std::cerr << "Registry::Entry<Parent>::register_child"
	  " instance=" << _instance << " registry=" << &Parent::registry <<
	  " registry.size=" << Parent::registry.size() + 1 << std::endl;

      instance = _instance;
      Parent::registry.add (this);
    }

    //! Destructor deletes instance of Parent
    virtual ~Entry () { if (instance) delete instance; }
    
    //! Return a pointer to a new instance of a Parent (or its children)
    virtual Parent* create () const = 0;

  private:
    Parent* instance;
  };

  //! List of Registry::Entry
  template<class Parent>
  class List
  {

    friend class Entry<Parent>;

  public:

    //! Counts the number of instances of Child entries
    template<class Child> class Instances;

    //! Enter a Child with default constructor into Registry::List<Parent>
    template<class Child> class Enter;
  
    //! Enter Child with unary constructor into Registry::List<Parent>
    template<class Child, class Argument> class Unary;

    //! Return the size of the list
    unsigned size () const { return entries.size(); }

    //! Return the specified entry
    Parent* operator[] (unsigned i) const { return entries[i]->get(); }

    //! Remove the specified entry
    void erase (unsigned i) { entries.erase( entries.begin() + i ); }

    //! Return pointer to a null-constructed instance of Parent class
    Parent* create (unsigned i) const { return entries[i]->create(); }

    //! verbosity flag used for debugging
    static bool verbose;

  protected:

    //! Add an entry to the registry
    void add (Entry<Parent>* entry) { entries.push_back (entry); }

    //! The vector of registry entries
    std::vector< Entry<Parent>* > entries;
  };

  template<class Parent>
  template<class Child>
  class List<Parent>::Instances
  {
  public:
    static unsigned get_instances () { return instances; }
    static void add_instance () { instances++; }
  private:
    static unsigned instances;
  };

  //! Enter a Child with default constructor into Registry::List<Parent>
  template<class Parent>
  template<class Child>
  class List<Parent>::Enter : public Entry<Parent>
  {
  public:
    //! Constructor registers only the first instance
    Enter () 
    { 
      if (Instances<Child>::get_instances() == 0)
	register_child (new Child);
      Instances<Child>::add_instance();
    }

  protected:
    //! Create a new instance of Child
    Parent* create () const { return new Child; }
  };
  
  //! Enter Child with unary constructor into Registry::List<Parent>
  template<class Parent>
  template<class Child, class Argument>
  class List<Parent>::Unary : public Entry<Parent>
  {
  public:

    //! Constructor registers only the first instance
    Unary (const Argument& arg)
    { 
      argument = arg;
      if (Instances<Child>::get_instances() == 0)
	register_child( new Child(arg) );
      Instances<Child>::add_instance();
    }
  protected:
    //! Create a new instance of Child
    Parent* create () const { return new Child(argument); }
    //! Argument to unary constructor
    Argument argument;
  };


}

template<class Parent> 
template<class Child>
unsigned Registry::List<Parent>::Instances<Child>::instances = 0;

template<class Parent>
bool Registry::List<Parent>::verbose = false;

#endif
