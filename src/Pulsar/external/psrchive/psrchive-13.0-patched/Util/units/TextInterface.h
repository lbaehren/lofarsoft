//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/units/TextInterface.h,v $
   $Revision: 1.55 $
   $Date: 2009/11/29 12:13:50 $
   $Author: straten $ */

#ifndef __TextInterface_h
#define __TextInterface_h

#include "ModifyRestore.h"
#include "Alias.h"
#include "Error.h"

#include "tostring.h"
#include "stringtok.h"
#include "separate.h"

// #define _DEBUG 1

#ifdef _DEBUG
#include <iostream>
#endif

namespace TextInterface {

  //! Text interface to a value of undefined type
  class Value : public Reference::Able
  {
  public:
    
    //! Get the name of the value
    virtual std::string get_name () const = 0;

    //! Get the value as text
    virtual std::string get_value () const = 0;

    //! Set the value as text
    virtual void set_value (const std::string& value) = 0;

    //! Get the description of the value
    virtual std::string get_description () const = 0;

    //! Get the detailed description of the value
    virtual std::string get_detailed_description () const { return "none"; }

    //! Return true if the name argument matches the value name
    virtual bool matches (const std::string& name) const
    {
#ifdef _DEBUG
      std::cerr << "TextInterface::Value::matches" << std::endl;
#endif
      return name == get_name();
    }
  };

  //! Proxy enables value interfaces to be imported with a name
  class NestedValue : public Value
  {
  public:
    
    //! Construct from a name and pointer to Value
    NestedValue (const std::string& pre, Value* val)
      { prefix = pre; value = val; }

    //! Get the name of the attribute
    std::string get_name () const
      { return prefix + ":" + value->get_name(); }

    //! Get the description of the value
    std::string get_description () const
      { return value->get_description(); }

    //! Get the value of the value
    std::string get_value () const 
      { return value->get_value (); }

    //! Set the value of the value
    void set_value (const std::string& txt)
      { value->set_value (txt); }

    //! Return true if the name argument matches
    bool matches (const std::string& name) const
      { return matches (name, prefix, value); }

    static bool matches (const std::string& name,
			 const std::string& prefix,
			 const Value* value);

  protected:

    //! The parent value interface
    Reference::To< Value > value;

    //! The name of the value
    std::string prefix;

  };


  //! Read-only interface to a value of type T
  template<class T>
  class ValueGet : public Value {

  public:
    
    //! Set all attributes on construction
    ValueGet (T* ptr, const std::string& _name, const std::string& _describe)
      { value_ptr = ptr; name = _name; description = _describe; }

    //! Get the name of the value
    std::string get_name () const
      { return name; }

    //! Get the value as text
    std::string get_value () const
      { return tostring (*value_ptr); }

    //! Set the value as text
    void set_value (const std::string& text)
      { throw Error (InvalidState, "ValueGet::set_value", 
		     name + " cannot be set"); }

    //! Get the description of the value
    std::string get_description () const
      { return description; }

    //! Set the description of the value
    void set_description (const std::string& text)
      { description = text; }
       
    //! Get the detailed description of the value
    std::string get_detailed_description () const
      { return detailed_description; }

    //! Set the detailed description of the value
    void set_detailed_description (const std::string& text)
      { detailed_description = text; }

  protected:

    std::string name;
    std::string description;
    std::string detailed_description;
    T* value_ptr;

  };

  //! Read and write interface to a value of type T
  template<class T>
  class ValueGetSet : public ValueGet<T> {

  public:
    
    //! Set all attributes on construction
    ValueGetSet (T* ptr, const std::string& name, const std::string& desc)
      : ValueGet<T> (ptr, name, desc) { }

    //! Set the value as text
    void set_value (const std::string& text)
    { *(this->value_ptr) = fromstring<T>(text); }

  };

  //! Text interface to a text interpreter
  /*! In this template: I is a the interpreter class; Get is the
    method of I that returns a string; and Set is the method of I that
    accepts a string */
  template<class I, class Get, class Set>
  class Interpreter : public Value {

  public:
    
    //! Set all attributes on construction
    Interpreter (const std::string& _name, I* ptr, Get _get, Set _set)
    { name = _name, interpreter = ptr, get = _get; set = _set; }

    //! Get the name of the value
    std::string get_name () const
      { return name; }

    //! Get the value as text
    std::string get_value () const
      { return (interpreter->*get)(); }

    //! Set the value as text
    void set_value (const std::string& text)
      { (interpreter->*set)(text); }

    //! Get the description of the value
    std::string get_description () const
      { return description; }

    //! Set the description of the value
    void set_description (const std::string& text)
      { description = text; }
       
    //! Get the detailed description of the value
    std::string get_detailed_description () const
      { return detailed_description; }

    //! Set the detailed description of the value
    void set_detailed_description (const std::string& text)
      { detailed_description = text; }

  protected:

    std::string name;
    std::string description;
    std::string detailed_description;
    I* interpreter;
    Get get;
    Set set;

  };

  template<class I, class Get, class Set>
    Interpreter<I,Get,Set>* 
    new_Interpreter (const std::string& n, I* i, Get g, Set s)
    {
      return new Interpreter<I,Get,Set> (n, i, g, s);
    }

  template<class I, class Get, class Set>
    Interpreter<I,Get,Set>* 
    new_Interpreter (const std::string& n, const std::string& d, 
		     I* i, Get g, Set s)
    {
      Interpreter<I,Get,Set>* result = new Interpreter<I,Get,Set> (n, i, g, s);
      result->set_description (d);
      return result;
    }

  template<class I, class Get, class Set>
    Interpreter<I,Get,Set>* 
    new_Interpreter (const std::string& n, const std::string& d, 
		     const std::string& dd,
		     I* i, Get g, Set s)
    {
      Interpreter<I,Get,Set>* result = new Interpreter<I,Get,Set> (n, i, g, s);
      result->set_description (d);
      result->set_detailed_description (dd);
      return result;
    }

  //! Text interface to a class attribute of undefined type
  template<class C>
  class Attribute : public Value {

  public:

    //! Default constructor
    Attribute ()
      { instance = 0; }

    //! Get the value of the attribute
    std::string get_value () const
      { if (!instance) return "N/A"; else return get_value (instance); }

    //! Set the value of the attribute
    void set_value (const std::string& value)
      { if (instance) set_value (instance, value); }

    //! Retun a newly constructed copy
    virtual Attribute* clone () const = 0;

    //! Get the value of the attribute
    virtual std::string get_value (const C*) const = 0;

    //! Set the value of the attribute
    virtual void set_value (C*, const std::string& value) = 0;

    //! Set the description of the value
    virtual void set_description (const std::string&) = 0;

    //! Set the detailed description of the value
    virtual void set_detailed_description (const std::string&) = 0;

     //! Pointer to the instance from which attribute value will be obtained
    mutable C* instance;

  };

  //! Proxy enables attribute interface from parent to be used by child
  /*! In this template: C is a P */
  template<class C, class P>
  class IsAProxy : public Attribute<C> {

  public:
    
    //! Construct from a pointer to parent class attribute interface
    IsAProxy (const Attribute<P>* pa) { attribute = pa->clone(); }

    //! Copy constructor
    IsAProxy (const IsAProxy& copy)
      { attribute = copy.attribute->clone(); }

    //! Retun a newly constructed copy
    Attribute<C>* clone () const
      { return new IsAProxy(*this); }

    //! Get the name of the attribute
    std::string get_name () const
      { return attribute->get_name(); }

    //! Get the description of the attribute
    std::string get_description () const
      { return attribute->get_description(); }

    //! Get the value of the attribute
    std::string get_value (const C* ptr) const 
      { return attribute->get_value (ptr); }

    //! Set the value of the attribute
    void set_value (C* ptr, const std::string& value)
      { attribute->set_value (ptr, value); }

    //! Set the description of the attribute
    void set_description (const std::string& description)
      { attribute->set_description (description); }

    //! Set the detailed description of the attribute
    void set_detailed_description (const std::string& description)
      { attribute->set_detailed_description (description); }

    bool matches (const std::string& name) const
      { return attribute->matches (name); }

  protected:

    //! The parent attribute interface
    Reference::To< Attribute<P> > attribute;

  };

  //! Proxy enables attribute interface of member to be used by class
  /*! In this template: C has a M; Get is the method of C that returns M* */
  template<class C, class M, class Get>
  class HasAProxy : public Attribute<C> {

  public:
    
    //! Construct from a pointer to member attribute interface
    HasAProxy (const std::string& pre, const Attribute<M>* pa, Get g)
      { prefix = pre; attribute = pa->clone(); get = g; }

    //! Copy constructor
    HasAProxy (const HasAProxy& copy)
      { attribute=copy.attribute->clone(); get=copy.get; prefix=copy.prefix; }

    //! Retun a newly constructed copy
    Attribute<C>* clone () const
      { return new HasAProxy(*this); }

    //! Get the name of the attribute
    std::string get_name () const
      { return prefix + ":" + attribute->get_name(); }

    //! Get the description of the attribute
    std::string get_description () const
      { return attribute->get_description(); }

    //! Get the value of the attribute
    std::string get_value (const C* ptr) const 
      { return attribute->get_value ((const_cast<C*>(ptr)->*get)()); }

    //! Set the value of the attribute
    void set_value (C* ptr, const std::string& value)
      { attribute->set_value ((ptr->*get)(), value); }

    //! Set the description of the attribute
    void set_description (const std::string& description)
      { attribute->set_description (description); }

    //! Set the detailed description of the attribute
    void set_detailed_description (const std::string& description)
      { attribute->set_detailed_description (description); }

    //! Return true if the name argument matches
    bool matches (const std::string& name) const
      { return NestedValue::matches (name, prefix, attribute); }

  protected:

    //! The parent attribute interface
    Reference::To< Attribute<M> > attribute;

    //! Method of C that returns M*
    Get get;

    //! The name of the M attribute within C
    std::string prefix;

  };

  //! Proxy enables attribute interface of elements in a vector
  /*! In this template: V is a vector of E; Get is the method of V
    that returns E*; and Size is the method of V that returns the number
    of elements in it */
  template<class V, class E, class Get, class Size>
  class VectorOfProxy : public Attribute<V> {

  public:
    
    //! Construct from a pointer to element attribute interface
    VectorOfProxy (const std::string& p, const Attribute<E>* a, Get g, Size s)
      { prefix = p; attribute = a->clone(); get = g; size = s; }

    //! Copy constructor
    VectorOfProxy (const VectorOfProxy& copy)
      { attribute=copy.attribute->clone(); 
	get=copy.get; size=copy.size; prefix=copy.prefix; }

    //! Retun a newly constructed copy
    Attribute<V>* clone () const
      { return new VectorOfProxy(*this); }

    //! Get the name of the attribute
    std::string get_name () const
      { return prefix + "*:" + attribute->get_name(); }

    //! Get the description of the attribute
    std::string get_description () const
      { return attribute->get_description(); }

    //! Get the value of the attribute
    std::string get_value (const V* ptr) const;

    //! Set the value of the attribute
    void set_value (V* ptr, const std::string& value);

    //! Set the description of the attribute
    void set_description (const std::string& description)
      { attribute->set_description (description); }

    //! Set the detailed description of the attribute
    void set_detailed_description (const std::string& description)
      { attribute->set_detailed_description (description); }

    //! Return true if the name argument matches
    bool matches (const std::string& name) const;

  protected:

    //! The parent attribute interface
    Reference::To< Attribute<E> > attribute;

    //! Method of V that returns E*
    Get get;

    //! Method of V that returns size of vector
    Size size;

    //! The name of the vector instance
    std::string prefix;

    //! Range parsed from name during matches
    mutable std::string range;

  };


  //! Proxy enables attribute interface of elements in a map
  /*! In this template: M is a map of key K to element E; 
    Get is the method of V that returns E* given K */
  template<class M, class K, class E, class Get>
  class MapOfProxy : public Attribute<M> {

  public:
    
    //! Construct from a pointer to parent class attribute interface
    MapOfProxy (const std::string& pre, const Attribute<E>* pa, Get g)
      { prefix = pre; attribute = pa->clone(); get = g; }

    //! Copy constructor
    MapOfProxy (const MapOfProxy& copy)
      { prefix=copy.prefix; attribute=copy.attribute->clone(); get=copy.get;  }

    //! Set the prefix to be added before attribute name
    
    //! Retun a newly constructed copy
    Attribute<M>* clone () const
      { return new MapOfProxy(*this); }

    //! Get the name of the attribute
    std::string get_name () const
      { return prefix + "?:" + attribute->get_name(); }

    //! Get the description of the attribute
    std::string get_description () const
      { return attribute->get_description(); }

    //! Get the value of the attribute
    std::string get_value (const M* ptr) const;

    //! Set the value of the attribute
    void set_value (M* ptr, const std::string& value);

    //! Set the description of the attribute
    void set_description (const std::string& description)
      { attribute->set_description (description); }

    //! Set the detailed description of the attribute
    void set_detailed_description (const std::string& description)
      { attribute->set_detailed_description (description); }

    //! Return true if the name argument matches
    bool matches (const std::string& name) const;

  protected:

    //! The parent attribute interface
    Reference::To< Attribute<E> > attribute;

    //! Method of M that returns E*, given K
    Get get;

    //! The name of the map instance
    std::string prefix;

    //! Range parsed from name during matches
    mutable std::string range;

    //! Worker function parses keys for get_value and set_value
    void get_indeces (std::vector<K>& keys,
		      const std::string& param) const;

  };

  //! Pointer to attribute get method, C::Get(), that returns some type
  template<class C, class Get>
  class AttributeGet : public Attribute<C> {

  public:

    //! Constructor
    AttributeGet (const std::string& _name, Get _get)
      { name = _name; get = _get; }

    //! Copy constructor
    AttributeGet (const AttributeGet& copy)
      { name = copy.name; description = copy.description; get = copy.get; }

    //! Return a clone
    Attribute<C>* clone () const { return new AttributeGet(*this); }

    //! Get the name of the attribute
    std::string get_name () const { return name; }

    //! Get the description of the attribute
    std::string get_description () const { return description; }

    //! Get the description of the attribute
    void set_description (const std::string& d) { description = d; }

    //! Get the detailed description of the attribute
    std::string get_detailed_description () const
    { return detailed_description; }

    //! Get the detailed description of the attribute
    void set_detailed_description (const std::string& d)
    { detailed_description = d; }

    //! Get the value of the attribute
    std::string get_value (const C* ptr) const
      { if (!ptr) return ""; return tostring( (ptr->*get) () ); }

    //! Set the value of the attribute
    void set_value (C* ptr, const std::string& value)
      { throw Error (InvalidState, "AttributeGet::set_value", 
		     name + " cannot be set"); }

  protected:

    //! The name of the attribute
    std::string name;

    //! The description of the attribute
    std::string description;

    //! The detailed description of the attribute
    std::string detailed_description;

    //! The get method
    Get get;

  };


  //! Pointer to attribute get method, C::Get(index), that returns some type
  /*! In this template: C is a class with a vector of elements of
    unspecified type; Get is the method of C that returns E; and Size
    is the method of C that returns the number of elements in the vector. */
  template<class C, class Get, class Size>
  class ElementGet : public Attribute<C> {

  public:

    //! Constructor
    ElementGet (const std::string& _name, Get _get, Size _size)
      { name = _name; get = _get; size = _size; }

    //! Copy constructor
    ElementGet (const ElementGet& copy)
      { name = copy.name; description = copy.description; 
	get = copy.get; size = copy.size; }

    //! Return a clone
    Attribute<C>* clone () const { return new ElementGet(*this); }

    //! Get the name of the attribute
    std::string get_name () const { return name + "*"; }

    //! Get the description of the attribute
    std::string get_description () const { return description; }

    //! Get the description of the attribute
    void set_description (const std::string& d) { description = d; }

    //! Get the detailed description of the attribute
    std::string get_detailed_description () const
    { return detailed_description; }

    //! Get the detailed description of the attribute
    void set_detailed_description (const std::string& d)
    { detailed_description = d; }

    //! Get the value of the attribute
    std::string get_value (const C* ptr) const;

    //! Set the value of the attribute
    void set_value (C* ptr, const std::string& value)
      { throw Error (InvalidState, "ElementGet::set_value", 
		     name + " cannot be set"); }

    //! Return true if the name argument matches
    bool matches (const std::string& name) const;

  protected:

    //! The name of the attribute
    std::string name;

    //! The description of the attribute
    std::string description;

    //! The detailed description of the attribute
    std::string detailed_description;

    //! The get method
    Get get;

    //! The size method
    Size size;

    //! Range parsed from name during matches
    mutable std::string range;

  };

  //! Pointer to a Unary Function that receives C* and returns some type
  template<class C, class Unary>
  class UnaryGet : public Attribute<C> {

  public:

    //! Constructor
    UnaryGet (const std::string& _name, Unary _get)
      : get (_get) { name = _name; }
    
    //! Copy constructor
    UnaryGet (const UnaryGet& copy)
      : get (copy.get) { name = copy.name; description = copy.description; }

    //! Return a clone
    Attribute<C>* clone () const { return new UnaryGet(*this); }

    //! Get the name of the attribute
    std::string get_name () const { return name; }

    //! Get the description of the attribute
    std::string get_description () const { return description; }

    //! Get the description of the attribute
    void set_description (const std::string& d) { description = d; }

    //! Get the detailed description of the attribute
    std::string get_detailed_description () const
    { return detailed_description; }

    //! Get the detailed description of the attribute
    void set_detailed_description (const std::string& d)
    { detailed_description = d; }

    //! Get the value of the attribute
    std::string get_value (const C* ptr) const
      { if (!ptr) return ""; return tostring( get(ptr) ); }

    //! Set the value of the attribute
    void set_value (C* ptr, const std::string& value)
      { throw Error (InvalidState, "UnaryGet::set_value", 
		     name + " cannot be set"); }

  protected:

    //! The name of the attribute
    std::string name;

    //! The description of the attribute
    std::string description;

    //! The detailed description of the attribute
    std::string detailed_description;

    //! The get function object (functor)
    Unary get;

  };

  //! Pointers to attribute get and set methods, Type C::Get() and C::Set(Type)
  template<class C, class Type, class Get, class Set>
  class AttributeGetSet : public AttributeGet<C, Get> {

  public:

    //! Constructor
    AttributeGetSet (const std::string& _name, Get _get, Set _set)
      : AttributeGet<C,Get> (_name, _get) { set = _set; }

    //! Copy constructor
    AttributeGetSet (const AttributeGetSet& copy) 
      : AttributeGet<C,Get> (copy) { set = copy.set; }

    //! Return a clone
    Attribute<C>* clone () const { return new AttributeGetSet(*this); }

    //! Set the value of the attribute
    void set_value (C* ptr, const std::string& value)
      { (ptr->*set) (fromstring<Type>(value)); }

  protected:

    //! The set method
    Set set;

  };

  //! Pointers to attribute get and set methods, Type C::Get() and C::Set(Type)
  template<class C, class Type, class Get, class Set, class Size>
    class ElementGetSet : public ElementGet<C, Get, Size> {

  public:

    //! Constructor
    ElementGetSet (const std::string& _name, Get _get, Set _set, Size _size)
      : ElementGet<C,Get,Size> (_name, _get, _size) { set = _set; }

    //! Copy constructor
    ElementGetSet (const ElementGetSet& copy) 
      : ElementGet<C,Get,Size> (copy) { set = copy.set; }

    //! Return a clone
    Attribute<C>* clone () const { return new ElementGetSet(*this); }

    //! Set the value of the attribute
    void set_value (C* ptr, const std::string& value);

  protected:

    //! The set method
    Set set;

  };

  //! AttributeGet and AttributeGetSet factory
  /*! Use this generator function object whenever the get and set
    methods to not match the patterns supplied by the add template
    methods of the TextInterface::To class */
  template<class C, class Type>
  class Allocator {

  public:

    //! Generate a new AttributeGet instance
    template<class Get>
      AttributeGet<C,Get>* 
      operator () (const std::string& n, Get g)
      { return new AttributeGet<C,Get> (n, g); }
    
    //! Generate a new AttributeGetSet instance
    template<class Get, class Set>
      AttributeGetSet<C,Type,Get,Set>* 
      operator () (const std::string& n, Get g, Set s)
      { return new AttributeGetSet<C,Type,Get,Set> (n, g, s); }
    
    //! Generate a new AttributeGet instance with description
    template<class Get>
      AttributeGet<C,Get>* 
      operator () (const std::string& n, const std::string& d, Get g)
      {
	AttributeGet<C,Get>* get = operator () (n,g);
	get->set_description (d); return get;
      }

    //! Generate a new AttributeGetSet instance with description
    template<class Get, class Set>
      AttributeGetSet<C,Type,Get,Set>* 
      operator () (const std::string& n, const std::string& d, Get g, Set s)
      {
	AttributeGetSet<C,Type,Get,Set>* get = operator () (n,g,s);
	get->set_description (d); return get;
      }

  };

  //! ElementGet and ElementGetSet factory
  /*! Use this generator function object whenever the get and set
    methods to not match the patterns supplied by the add template
    methods of the TextInterface::To class */
  template<class C, class Type>
  class VAllocator {

  public:

    //! Generate a new ElementGet instance
    template<class Get, class Size>
      ElementGet<C,Get,Size>* 
      operator () (const std::string& n, Get g, Size z)
      { return new ElementGet<C,Get,Size> (n, g, z); }
    
    //! Generate a new ElementGetSet instance
    template<class Get, class Set, class Size>
      ElementGetSet<C,Type,Get,Set,Size>* 
      operator () (const std::string& n, Get g, Set s, Size z)
      { return new ElementGetSet<C,Type,Get,Set,Size> (n, g, s, z); }
    
    //! Generate a new ElementGet instance with description
    template<class Get, class Size>
      ElementGet<C,Get,Size>* 
      operator () (const std::string& n, const std::string& d, Get g, Size z)
      {
	ElementGet<C,Get,Size>* get = operator () (n,g,z);
	get->set_description (d); return get;
      }

    //! Generate a new ElementGetSet instance with description
    template<class Get, class Set, class Size>
      ElementGetSet<C,Type,Get,Set,Size>* 
      operator () (const std::string& n, const std::string& d,
		   Get g, Set s, Size z)
      {
	ElementGetSet<C,Type,Get,Set,Size>* get = operator () (n,g,s,z);
	get->set_description (d); return get;
      }

  };

  //! Abstract base class of class text interface
  class Parser : public Reference::Able {

  public:

    //! Default constructor
    Parser ();

    //! Get the named value
    std::string get_value (const std::string& name) const;

    //! Get the named value and update name for display purposes
    std::string get_name_value (std::string& name) const;

    //! Set the named value
    void set_value (const std::string& name, const std::string& value);

    //! Find the named value
    Value* find (const std::string& name, bool throw_exception = true) const;

    //! Return true if the named value is found
    bool found (const std::string& name) const;

    //! Return true if prefix:name is found
    bool found (const std::string& prefix, const std::string& name) const;

    //! Allow derived types to setup a Value instance before use
    virtual void setup (const Value*) { }

    //! Get the number of values
    unsigned get_nvalue () const;

    //! Get the name of the value
    std::string get_name (unsigned) const;

    //! Get the value
    std::string get_value (unsigned) const;

    //! Get the description of the value
    std::string get_description (unsigned) const;

    //! Process a command
    virtual std::string process (const std::string& command);
    
    //! Return the list of available values
    virtual std::string help (bool show_default_values = false);

    //! Get the name of this interface
    virtual std::string get_interface_name () const { return ""; }

    //! Set the indentation that precedes the output of a call to process
    void set_indentation (const std::string& indent) { indentation = indent; }

    //! Prefix output with "name="
    void set_prefix_name (bool flag) { prefix_name = flag; }

    //! Set aliases for value names
    void set_aliases (const Alias* alias) { aliases = alias; }

    //! Insert Parser into self
    void insert (Parser*);

    //! Import a nested interface
    void insert (const std::string& prefix, Parser*);

    //! Clear all nested interfaces
    void clean ();

  protected:

    //! The indentation that precedes the output of a call to process
    std::string indentation;

    //! Maintain alphabetical order of parameter names
    bool alphabetical;

    //! Filter duplicate value names during import
    bool import_filter;

    //! Prefix "name=" when retrieving a value
    bool prefix_name;

    //! The aliases for the value names
    Reference::To<const Alias> aliases;

    //! Add a new value interface
    void add_value (Value* value);

    //! Remove the named value interface
    void remove (const std::string& name);

    //! Clean up invalid references in values vector
    void clean_invalid ();

    //! The vector of values
    std::vector< Reference::To<Value> > values;

  };


  //! Class text interface: an instance of C and a vector of Attribute<C>
  template<class C>
  class To : public Parser {

  public:

    template<class Type> class Generator : public Allocator<C,Type> { };

    template<class Type> class VGenerator : public VAllocator<C,Type> { };

    //! Set the instance
    virtual void set_instance (C* c) 
      { instance = c; }

    //! Set the instance of the Attribute<C>
    void setup (const Value* value)
    {
      const Attribute<C>* attribute = dynamic_cast<const Attribute<C>*>(value);
      if (attribute) 
	attribute->instance = instance.ptr();
    }

    //! Import the attribute interfaces from a parent text interface
    template<class P> 
      void import (const To<P>* parent)
      {
	for (unsigned i=0; i < parent->size(); i++)
	  if (!import_filter || !found(parent->get(i)->get_name()))
	    add_value( new IsAProxy<C,P>(parent->get(i)) );
      }

    //! Import the attribute interfaces from a member text interface
    /*! In this template, G should be of the type pointer to member function
      of C that returns pointer to M */
    template<class M, class G> 
      void import (const std::string& name, const To<M>* member, G get)
      {
	for (unsigned i=0; i < member->size(); i++)
	  if (!import_filter || !found(name, member->get(i)->get_name()))
	    add_value( new HasAProxy<C,M,G>(name, member->get(i), get) );
      }

    //! Import the attribute interfaces from a vector element text interface
    /*! In this template: G should be of the type pointer to member
      function of C that returns pointer to the indexed element of type E,
      S should be a pointer to the member function that returns the
      number of elements of type E in C. */
    template<class E, class G, class S>
      void import (const std::string& name, const To<E>* member, G g, S s)
      {
	for (unsigned i=0; i < member->size(); i++)
	  if (!import_filter || !found(name, member->get(i)->get_name()))
	    add_value(new VectorOfProxy<C,E,G,S>(name, member->get(i), g, s));
      }

    //! Import the attribute interfaces from a map data text interface
    /*! In this template: G should be of the type pointer to member
      function of C that accepts key K and returns pointer to data element E. 
    */
    template<class K, class E, class G>
      void import (const std::string& name, K k, const To<E>* member, G g)
      {
	for (unsigned i=0; i < member->size(); i++)
	  if (!import_filter || !found(name, member->get(i)->get_name()))
	    add_value( new MapOfProxy<C,K,E,G>(name, member->get(i), g) );
      }

    //! Import the attribute interfaces from a parent text interface
    template<class P> 
      void import (const To<P>& parent)
      { import (&parent); }

    //! Import the attribute interfaces from a member text interface
    template<class M, class G> 
      void import ( const std::string& name, const To<M>& member, G get )
      { import (name, &member, get); }

    template<class E, class G, class S>
      void import ( const std::string& name, const To<E>& element, G g, S size)
      { import (name, &element, g, size); }

    template<class K, class E, class G>
      void import ( const std::string& name, K k, const To<E>& element, G g)
      { import (name, k, &element, g); }

    //! Return the number of attributes
    unsigned size () const { return values.size(); }

    //! Provide access to the attributes
    const Attribute<C>* get (unsigned i) const
      { return dynamic_cast< const Attribute<C>* >( values[i].get() ); }

  protected:

    //! Factory generates a new AttributeGet instance
    template<class P, typename T> 
      void add (T(P::*get)()const,
		const char* name, const char* description = 0)
      {
	Generator<T> gen;
	Attribute<C>* getset = gen (name, get);
	if (description)
	  getset->set_description (description);
	add_value (getset);
      }

    //! Factory generates a new AttributeGetSet instance with description
    template<class P, typename T>
      void add (T(P::*get)()const, void(P::*set)(const T&),
		const char* name, const char* description = 0)
      {
	Generator<T> gen;
	Attribute<C>* getset = gen (name, get, set);
	if (description)
	  getset->set_description (description);
	add_value (getset);
      }

    //! Factory generates a new AttributeGetSet instance with description
    template<class P, typename T>
      void add (T(P::*get)()const, void(P::*set)(T),
		const char* name, const char* description = 0)
      {
	Generator<T> gen;
	Attribute<C>* getset = gen (name, get, set);
	if (description)
	  getset->set_description (description);
	add_value (getset);
      }


    //! Add adaptable unary function object template
    template<class U>
      void add (U get, const char* name, const char* description = 0)
      {
	Attribute<C>* fget = new UnaryGet<C,U> (name, get);
	if (description)
	  fget->set_description (description);
	add_value (fget);
      }

    //! The instance of the class with which this interfaces
    Reference::To<C,false> instance;

  };

  //! Parse a range of indeces from a string
  void parse_indeces (std::vector<unsigned>& indeces, const std::string&,
		      unsigned size);

  //! Does the work for VectorOfProxy::matches and MapOfProxy::matches
  bool match (const std::string& name, const std::string& text,
	      std::string* range, std::string* remainder = 0);

  template<typename Type, typename Container>
  Type* factory ( Container& ptrs, std::string name_parse);

  template<typename T>
  std::ostream& insertion (std::ostream& ostr, T* e);
  
  template<typename T>
  std::istream& extraction (std::istream& istr, T* &e);

  //! Label elements in ElementGetSet<C,E>::get_value
  extern bool label_elements;

}

template<class V, class E, class G, class S> 
std::string
TextInterface::VectorOfProxy<V,E,G,S>::get_value (const V* ptr) const
{
  std::vector<unsigned> ind;
  parse_indeces (ind, range, (ptr->*size)());
  std::string result;

  for (unsigned i=0; i<ind.size(); i++)
  {
    if (i)
      result += ",";  // place a comma between elements
    if (label_elements && ind.size() > 1)
      result += tostring(ind[i]) + ")";  // label the elements

    E* element = (const_cast<V*>(ptr)->*get)(ind[i]);
#ifdef _DEBUG
    std::cerr << "VectorOfProxy[" << prefix << "]::get_value (" 
	      << ptr << ") element=" << element << std::endl;
#endif
    result += attribute->get_value (element);
  }

  return result;
}

template<class V, class E, class G, class S>
void TextInterface::VectorOfProxy<V,E,G,S>::set_value (V* ptr,
						       const std::string& val)
{
  std::vector<unsigned> ind;
  parse_indeces (ind, range, (ptr->*size)());

  for (unsigned i=0; i<ind.size(); i++) {
    E* element = (ptr->*get)(ind[i]);
#ifdef _DEBUG
    std::cerr << "VectorOfProxy[" << prefix << "]::set_value (" 
	      << ptr << "," << val << ")" << std::endl;
#endif
    attribute->set_value (element, val);
  }
}


template<class C,class M,class Get,class Size>
 bool TextInterface::VectorOfProxy<C,M,Get,Size>::matches
  (const std::string& name) const
{
#ifdef _DEBUG
  std::cerr << "TextInterface::VectorOfProxy::matches" << std::endl;
#endif

  std::string remainder;
  if (!match (prefix, name, &range, &remainder))
    return false;

  return attribute->matches (remainder);
}



template<class M, class K, class E, class G> 
std::string
TextInterface::MapOfProxy<M,K,E,G>::get_value (const M* ptr) const
{
  std::vector<K> ind;
  get_indeces (ind, range);
  std::string result;

  for (unsigned i=0; i<ind.size(); i++)
  {
    // place a comma between elements
    if (i)
      result += ",";

    // label the elements
    if (label_elements && ind.size() > 1)
      result += tostring(ind[i]) + ")";

    E* element = (const_cast<M*>(ptr)->*get) (ind[i]);
    if (element)
      result += attribute->get_value (element);
  }

  return result;
}

template<class M, class K, class E, class G>
void TextInterface::MapOfProxy<M,K,E,G>::set_value (M* ptr,
						    const std::string& val)
{
  std::vector<K> ind;
  get_indeces (ind, range);

  for (unsigned i=0; i<ind.size(); i++)
    attribute->set_value ((ptr->*get)(ind[i]), val);
}

template<class M, class K, class E, class G>
void
TextInterface::MapOfProxy<M,K,E,G>::get_indeces (std::vector<K>& indeces,
						 const std::string& par) const
{
#ifdef _DEBUG
  std::cerr << "MapOfProxy::get_indeces " << par << std::endl;
#endif

  std::string::size_type length = par.length();

  std::string range = par;

  if (prefix.length()) {
    if (par[0] != '[' || par[length-1] != ']')
      return;
    range = par.substr (1, length-2);
  }
  else if (par == "?")
    return;

  std::vector<std::string> key_str;
  separate (range, key_str, ", ");

  indeces.resize (key_str.size());
  for (unsigned i=0; i<key_str.size(); i++)
    indeces[i] = fromstring<K>(key_str[i]);
}

template<class M, class K, class E, class G>
 bool TextInterface::MapOfProxy<M,K,E,G>::matches
  (const std::string& name) const
{
#ifdef _DEBUG
  std::cerr << "TextInterface::MapOfProxy::matches" << std::endl;
#endif

  std::string remainder;
  if (!match (prefix, name, &range, &remainder))
    return false;

  return attribute->matches (remainder);
}

//#include <string.h>

template<class C,class Get,class Size>
 bool TextInterface::ElementGet<C,Get,Size>::matches
  (const std::string& var) const
{
#ifdef _DEBUG
  std::cerr << "TextInterface::ElementGet::matches" << std::endl;
#endif

  if (!match (name, var, &range))
    return false;

  return true;
}

template<class C, class G, class S> 
std::string TextInterface::ElementGet<C,G,S>::get_value (const C* ptr) const
{
#ifdef _DEBUG
  std::cerr << "TextInterface::ElementGet::get_value name=" << name 
	    << " range=" << range << std::endl;
#endif

  std::vector<unsigned> ind;
  parse_indeces (ind, range, (ptr->*(size))());
  std::string result;

  for (unsigned i=0; i<ind.size(); i++)
  {
    if (i)
      result += ",";  // place a comma between elements
    if (label_elements && ind.size() > 1)
      result += tostring(ind[i]) + ")";  // label the elements

    result += tostring( (ptr->*get)(ind[i]) );
  }

  return result;
}

template<class C, class T, class G, class S, class Z>
void TextInterface::ElementGetSet<C,T,G,S,Z>::set_value (C* ptr,
							 const std::string& v)
{
  std::vector<unsigned> ind;
  parse_indeces (ind, this->range, (ptr->*(this->size))());

  for (unsigned i=0; i<ind.size(); i++)
    (ptr->*set)(ind[i], fromstring<T>(v));
}

template<typename T, typename C>
T* TextInterface::factory ( C& ptrs, std::string name_parse)
{
  std::string name = stringtok (name_parse, ":");

  Reference::To<T> result;

  for (typename C::iterator ptr=ptrs.begin(); ptr != ptrs.end(); ptr++)
  {
    Reference::To<TextInterface::Parser> interface = (*ptr)->get_interface();
    if (interface->get_interface_name() == name)
    {
      result = (*ptr)->clone ();
      break;
    }
  }

  if (!result)
    throw Error (InvalidState, std::string(),
		 "no instance named '" + name + "'");

  Reference::To<TextInterface::Parser> interface = result->get_interface();

  while (!name_parse.empty())
  {
    std::string option = stringtok (name_parse, ",");
    interface->process (option);
  }

  return result.release();
}

template<typename T>
std::ostream& TextInterface::insertion (std::ostream& ostr, T* e) try
{
  Reference::To<TextInterface::Parser> interface = e->get_interface();

  ostr << interface->get_interface_name ();

  if (interface->get_nvalue() == 0)
    return ostr;

  ostr << ":";

  for (unsigned i=0; i<interface->get_nvalue(); i++)
  {
    if (i > 0)
      ostr << ",";

    ostr << interface->get_name(i) << "=" << interface->get_value(i);
  }

  return ostr;
}
catch (Error& error)
{
  ostr.setstate (std::ios::failbit);
  return ostr;
}

template<typename T>
std::istream& TextInterface::extraction (std::istream& istr, T* &e) try
{
  std::string parse;
  istr >> parse;

  e = T::factory (parse);
  return istr;
}
catch (Error& error)
{
  istr.setstate (std::ios::failbit);
  return istr;
}

#endif
