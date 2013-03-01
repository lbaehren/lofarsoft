//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/units/Functor.h,v $
   $Revision: 1.7 $
   $Date: 2007/10/02 05:19:41 $
   $Author: straten $ */

#ifndef __Swinburne_Functor_h
#define __Swinburne_Functor_h

#include "Reference.h"

// an empty type used to allow one template name
struct __functor_empty {};

//! Implements an adaptable function object in compliance with the STL
/*!  The Functor template class implements a an adaptable function
  object interface using the excellent syntax found in the Tiny
  Template Library, http://www.codeproject.com/cpp/TTLFunction.asp.
  Functors are instantiated as in the following examples:

  // one integer argument, returns double
  Functor< double(int) > f1;

  // two string arguments, returns boolean
  Functor< bool(string, string) > f2;

  They are then called as a normal function would be called, e.g.

  string s1, s2;
  if( f2(s1, s2) )
    [blah blah blah]

*/
template<typename R, typename A1=__functor_empty, typename A2=__functor_empty>
class Functor;

//! Pure virtual base class of functor implementations
class FunctorBase : public Reference::Able {
public:  
  //! Return true if valid (able to be called)
  virtual bool is_valid () const = 0;

  //! Return true if this matches that
  virtual bool matches (const FunctorBase* that) const = 0;
};
  
//! Implements scaffolding of Function functor implementations
template<typename Function, typename Inherit> 
class FunctorFunction : public Inherit {
public:
      
  //! Return true if the function matches
  bool matches (Function _function) const
  { return function == _function; }

  //! Return true if valid (able to be called)
  bool is_valid () const { return function != 0; }

  //! Return true if this matches that
  virtual bool matches (const FunctorBase* that) const 
  { const FunctorFunction* like = dynamic_cast<const FunctorFunction*> (that);
    return like && like->matches(function); }

  protected:
    
  //! The function to be called
  Function function;
    
};

//! Implements scaffolding of Class/Method functor implementations
template<class Class, typename Method, class Inherit>
class FunctorMethod : public Inherit {
public:
      
  //! Return true if the instance and method match
  bool matches (const Class* _instance, Method _method) const
  { return (instance && instance == _instance && method == _method); }
    
  //! Return true if valid (able to be called)
  bool is_valid () const { return instance && method != 0; }

  //! Return true if this matches that
  virtual bool matches (const FunctorBase* that) const 
  { const FunctorMethod* like = dynamic_cast<const FunctorMethod*> (that);
    return like && like->matches(instance,method); }

protected:
    
  //! The instance of the class
  Reference::To<Class,false> instance;
  
  //! The method to be called
  Method method;
    
};

// ///////////////////////////////////////////////////////////////////////////
//
// Generator Functor specialization
//
// ///////////////////////////////////////////////////////////////////////////

//! Template specialization of generators (functions with no arguments)
/*! This class satisfies the STL definition of an Adaptable Generator */
template< typename R, typename A1, typename A2 > 
class Functor< R (), A1, A2 >
{
 public:

  typedef R result_type;

  //
  // the interface
  //

  //! Default constructor
  Functor () { }

  //! Generator function call
  R operator() () const { return functor->call(); }

  //! Construct from a class instance and generator method
  template<class C, typename M> Functor (C* instance, M method)
    { functor = new Method<C, M> (instance, method); }

  //! Construct from a generator function
  template<typename F> Functor (F function)
    { functor = new Function<F> (function); }

  //! Set equal to a class instance and generator method
  template<class C, typename M> void set (C* instance, M method)
    { functor = new Method<C, M> (instance, method); }

  //! Set equal to a generator function
  template<typename F> void set (F function)
    { functor = new Function<F> (function); }

  //
  // the implementation
  //

  //! Pure virtual base class of generator functor implementations
  class Base : public FunctorBase {
  public:  
    //! The function call
    virtual R call () const = 0;
  };
  
  //! Implementation calls generator function F
  template<typename F> class Function : public FunctorFunction<F,Base> {
  public:
      
    //! Construct from a pointer to a function
    Function (F _function)
    { this->function = _function; }
    
    //! Call the function
    R call () const
    { return R( (*(this->function)) () ); }
    
  };

  //! Implementation calls generator method M of class C
  template<class C, typename M> class Method : public FunctorMethod<C,M,Base> {
  public:
      
    //! Construct from a pointer to a class instance and method
    Method (C* _instance, M _method)
    { this->instance = _instance; this->method = _method; }
    
    //! Call the method through the class instance
    R call () const
    { return R( (this->instance->*(this->method)) () ); }
      
  };

  //! Return true if functor is not defined
  bool operator ! () const { return !functor; }
    
  //! Return true if functor is defined
  operator bool () const { return functor; }

  //! Get the pointer to the functor implementation
  const FunctorBase* get_functor () const { return functor; }

 protected: 
  
  //! The implementation
  Reference::To<Base> functor;

};

// ///////////////////////////////////////////////////////////////////////////
//
// Unary Functor specialization
//
// ///////////////////////////////////////////////////////////////////////////

//! Template specialization of unary functions
/*! This class satisfies the STL definition of an Adaptable Unary Function */
template< typename R, typename A1, typename A2 > 
class Functor< R (A1), A2 >
{
 public:

  typedef A1 argument_type;
  typedef R result_type;

  //
  // the interface
  //

  //! Default constructor
  Functor () { }

  //! Unary function call
  R operator() (const A1& p1) const { return functor->call(p1); }

  //! Construct from a class instance and unary method
  template<class C, typename M> Functor (C* instance, M method)
    { functor = new Method<C, M> (instance, method); }

  //! Construct from a unary function
  template<typename F> Functor (F function)
    { functor = new Function<F> (function); }

  //! Set equal to a class instance and unary method
  template<class C, typename M> void set (C* instance, M method)
    { functor = new Method<C, M> (instance, method); }

  //! Set equal to a unary function
  template<typename F> void set (F function)
    { functor = new Function<F> (function); }

  //
  // the implementation
  //

  //! Pure virtual base class of unary functor implementations
  class Base : public FunctorBase {
  public:  
    //! The function call
    virtual R call (const A1& p1) const = 0;
  };
  
  //! Implementation calls unary function F
  template<typename F> class Function : public FunctorFunction<F,Base> {
  public:
      
    //! Construct from a pointer to a function
    Function (F _function)
    { this->function = _function; }

    //! Call the function
    R call (const A1& p1) const
    { return static_cast<R>( (*(this->function))(p1) ); }
    
  };

  //! Implementation calls unary method M of class C
  template<class C, typename M> class Method : public FunctorMethod<C,M,Base> {
  public:
      
    //! Construct from a pointer to a class instance and method
    Method (C* _instance, M _method)
      { this->instance = _instance; this->method = _method; }
    
    //! Call the method through the class instance
    R call (const A1& p1) const try
      { return R( (this->instance->*(this->method)) (p1) ); }
    catch (Error& error) { throw error += "Functor<R(T)>::Method::call"; }
    
  };
  
  //! Return true if functor is not defined
  bool operator ! () const { return !functor; }
    
  //! Return true if functor is defined
  operator bool () const { return functor; }

  //! Get the pointer to the functor implementation
  const FunctorBase* get_functor () const { return functor; }

 protected: 
  
  //! The implementation
  Reference::To<Base> functor;
  
};


// ///////////////////////////////////////////////////////////////////////////
//
// Binary Functor specialization
//
// ///////////////////////////////////////////////////////////////////////////

//! Template specialization of binary functions
/*! This class satisfies the STL definition of an Adaptable Binary Function */
template< typename R, typename A1, typename A2 > 
class Functor< R (A1, A2) >
{
 public:

  typedef A1 first_argument_type;
  typedef A2 second_argument_type;
  typedef R result_type;

  //
  // the interface
  //

  //! Default constructor
  Functor () { }

  //! Binary function call
  R operator() (const A1& p1, const A2& p2) const 
    { return functor->call(p1,p2); }
  
  //! Construct from a class instance and binary method
  template<class C, typename M> Functor (C* instance, M method)
    { functor = new Method<C, M> (instance, method); }

  //! Construct from a binary function
  template<typename F> Functor (F function)
    { functor = new Function<F> (function); }

  //! Set equal to a class instance and binary method
  template<class C, typename M>  void set (C* instance, M method)
    { functor = new Method<C, M> (instance, method); }

  //! Set equal to a binary function
  template<typename F> void set (F function)
    { functor = new Function<F> (function); }

  //
  // the implementation
  //

  //! Pure virtual base class of binary functor implementations
  class Base : public FunctorBase {
  public:  
    //! The function call
    virtual R call (const A1& p1, const A2& p2) const = 0;
  };
  
  //! Implementation calls binary function F
  template<typename F> class Function : public FunctorFunction<F,Base> {
  public:
      
    //! Construct from a pointer to a function
    Function (F _function)
    { this->function = _function; }
    
    //! Call the function
    R call (const A1& p1, const A2& p2) const
    { return R( (*(this->function)) (p1, p2) ); }
    
  };

  //! Implementation calls binary method M of class C
  template<class C, typename M> class Method : public FunctorMethod<C,M,Base> {
  public:
      
    //! Construct from a pointer to a class instance and method
    Method (C* _instance, M _method)
    { this->instance = _instance; this->method = _method; }
    
    //! Call the method through the class instance
    R call (const A1& p1, const A2& p2) const
    { return R( (this->instance->*(this->method)) (p1, p2) ); }
    
  };
  
  //! Return true if functor is not defined
  bool operator ! () const { return !functor; }
    
  //! Return true if functor is defined
  operator bool () const { return functor; }

  //! Get the pointer to the functor implementation
  const FunctorBase* get_functor () const { return functor; }

 protected: 
  
  //! The implementation
  Reference::To<Base> functor;
  
};

template<typename R, typename A1, typename A2>
bool operator == (const Functor<R,A1,A2>& f1, const Functor<R,A1,A2>& f2)
{
  if (!f1 && !f2)
    return true;

  if (!f1 || !f2)
    return false;

  if (f1.get_functor() == f2.get_functor())
    return true;

  return f1.get_functor()->matches( f2.get_functor() );

}

#endif
