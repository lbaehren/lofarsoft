/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include <iostream>
#include <vector>

using namespace std;

#define _DEBUG 1
#include "Reference.h"

class parent : public Reference::Able {

  friend void func (parent&);

protected:
  int value;

public:
  parent (int val=0) { value = val; };
  virtual int getval () const { return value; }
};

class child : public parent {
public:
  child (int val=0) : parent (val) { valch = val * 2; }
  virtual int getval () const { return valch; }
protected:
  int valch;
  char space[4096];
};

void func (parent& ref)
{
  ref.value = 700;
}

child* create (int val)
{
  Reference::To<child> ref = new child (val);

  cerr << "create: created new child" << endl;

  // other things can happen that cause an exception to be thrown, at
  // which time the destructor of ref will result in the automatic
  // deletion of the new child to which it points.  However, when returning
  // this value it must be released so that the ref destructor does not
  // delete the new object

  return ref.release();
}

child* failed_create (int val)
{
  bool ok = false;

  Reference::To<child> ref = new child (val);

  if (ok)
    return ref.release();
  
  throw Error (InvalidParam, "failed_create", "test exception handling");
}


int runtest ();

int main (int argc, char** argv)
{
  Error::verbose = true;

  try {
    return runtest ();
  }
  catch (Error& error) {
    cerr << "Unexpected Error:" << error << endl;
    return -1;
  }
  catch (...) {
    cerr << "Unexpected and unknown exception" << endl;
    return -1;
  }

  return 0;

}

int runtest ()
{
  parent parent_instance(4);

  child* child_ptr = new child(3);
    
  // ////////////////////////////////////////////////////////////////////////
  //
  // Test Reference::To<> assignment to parent and
  // ability to call parent method through Reference::To<>
  //
  // ////////////////////////////////////////////////////////////////////////

  cerr << "\ntest - Reference::To<parent> = &parent" << endl;

  Reference::To<parent> parent_ref = &parent_instance;
  if ( parent_ref->getval() != parent_instance.getval() ) {
    cerr << "ERROR: Parent method through Reference::To<>" << endl;
    return -1;
  }

  const parent const_parent_instance(987);
  
  cerr << "\ntest - Reference::To<const parent> = &parent" << endl;

  Reference::To<const parent> const_parent_ref = &const_parent_instance;
  if ( const_parent_ref->getval() != const_parent_instance.getval() ) {
    cerr << "ERROR: Parent method through Reference::To<const parent>" <<endl;
    return -1;
  }

  cerr << "\ntest - Reference::To<parent> = Reference::To<parent>" << endl;
  Reference::To<parent> parent_ref2 = parent_ref;

  cerr << "\ntest - Reference::To<const parent> = Reference::To<parent>" 
       << endl;
  const_parent_ref = parent_ref;

  cerr << "\ntest - Reference::To<parent> assignment successful" << endl;

  // ////////////////////////////////////////////////////////////////////////
  //
  // Test Reference::To<> re-assignment to child and
  // ability to call child method through Reference::To<>
  //
  // ////////////////////////////////////////////////////////////////////////

  cerr << "\ntest - Reference::To<parent> = child*" << endl;

  parent_ref = child_ptr;

  if ( parent_ref->getval() != child_ptr->getval() ) {
    cerr << "ERROR: Child method through Reference::To<>" << endl;
    return -1;
  }

  cerr << "\ntest - Reference::To<parent> assignment to child successful"
       << endl;

  // ////////////////////////////////////////////////////////////////////////
  //
  // Test the operator == and != Reference::To<>
  //
  // ////////////////////////////////////////////////////////////////////////

  cerr << "\ntest - Reference::To<parent> operator [==|!=] child*" << endl;
  
  if ( parent_ref != child_ptr ) {
    cerr << "ERROR: Reference::To<parent> != child*" << endl;
    return -1;
  }
    
  if ( child_ptr != parent_ref ) {
    cerr << "ERROR: child* != Reference::To<parent>" << endl;
    return -1;
  }
 
  parent_ref = 0;
 
  if ( parent_ref == child_ptr ) {
    cerr << "ERROR: Reference::To<parent> == child*" << endl;
    return -1;
  }

  if ( child_ptr == parent_ref ) {
    cerr << "ERROR: child* == Reference::To<parent>" << endl;
    return -1;
  }

  // ////////////////////////////////////////////////////////////////////////
  //
  // Test the Reference::To<>::release
  //
  // ////////////////////////////////////////////////////////////////////////

  cerr << "\ntest - Reference::To<child>::release" << endl;

  Reference::To<child> child_ref = create (5);

  cerr << "\ntest - Reference::To<child>::operator ->" << endl;
  if ( child_ref->getval() != 10 ) {
    cerr << "ERROR: object deleted after Reference::To<>::release" << endl;
    return -1;
  }

  child_ptr = 0;

  // and attempts to dereference should throw an exception
  try {
    cerr << "\ntest - exception during child* assignement" << endl;
    child_ptr = failed_create (6);
    cerr << "ERROR:"
      " failed_create did not throw exception" << endl;
    return -1;
  }
  catch (Error& error) {
    if (child_ptr)  {
      cerr << "ERROR:"
        " child* assigned after exception thrown" << endl;
      return -1;
    }
    cerr << "Caught expected exception: child* = failed_create" << endl;
  }

  child_ref = 0;

  // and attempts to dereference should throw an exception
  try {
    cerr << "\ntest - exception during Refernce::To<child> assignment" << endl;
    child_ref = failed_create (6);
    cerr << "ERROR:"
      " failed_create did not throw exception" << endl;
    return -1;
  }
  catch (Error& error) {
    if (child_ref) {
      cerr << "ERROR:"
        " Refernce::To<child> assigned after exception thrown" << endl;
      return -1;
    }
    cerr << "Caught expected exception: Reference::To<child> = failed_create"
	 << endl; 
  }

  // ////////////////////////////////////////////////////////////////////////
  //
  // Test the swap template
  //
  // ////////////////////////////////////////////////////////////////////////

  cerr << "\ntest - swap Reference::To<parent>" << endl;
  parent* parent1 = new parent;
  parent* parent2 = new parent;

  Reference::To<parent> pref1 = parent1;
  Reference::To<parent> pref2 = parent2;

  swap (pref1, pref2);

  if (pref1 != parent2 || parent1 != pref2)  {
    cerr << "ERROR: swap Reference::To<parent>" << endl;
    return -1;
  }

  // ////////////////////////////////////////////////////////////////////////
  //
  // Test the Reference::To<> dereferencing operator
  //
  // ////////////////////////////////////////////////////////////////////////

  cerr << "\ntest - Reference::To<parent>:: operator = parent*" << endl;
  cerr << "test - Reference::Able::~Able implicit" << endl;

  parent_ref = &parent_instance;

  cerr << "\ntest - Reference::To<parent>:: operator *" << endl;
  func (*parent_ref);

  cerr << "\ntest - Reference::To<parent>:: operator ->" << endl;
  if ( parent_ref->getval() != parent_instance.getval()
       || parent_instance.getval() != 700 ) {
    cerr << "ERROR: Reference::To<> de-reference" << endl;
    return -1;
  }

  // ////////////////////////////////////////////////////////////////////////
  //
  // Test the Reference::To<> child to parent cast
  //
  // ////////////////////////////////////////////////////////////////////////

  cerr << "\ntest - Reference::To<child> = child*" << endl;
  child_ref = new child(5);

  cerr << "\ntest - Reference::To<parent> = Reference::To<child>" << endl;
  parent_ref = child_ref;

  cerr << "\ntest - Reference::To<parent>:: operator ->" << endl;
  cerr << "test - Reference::To<child>:: operator ->" << endl;
  if ( parent_ref->getval() != child_ref->getval() ) {
    cerr << "ERROR: Reference::To<> child to parent cast failed" << endl;
    return -1;
  }

  // ////////////////////////////////////////////////////////////////////////
  //
  // Test the Reference::To<> child to parent cast
  //
  // ////////////////////////////////////////////////////////////////////////

  cerr << "\ntest - dynamic_kast<child> (Reference::To<parent>&)" << endl;
  parent_ref = new child (6);

  cerr << "\ntest - Reference::To<child> = parent* (= new child)" << endl;
  child_ref = dynamic_kast<child> (parent_ref);

  cerr << "\ntest - Reference::To<child>::operator !" << endl;

  if ( ! child_ref || parent_ref->getval() != child_ref->getval() ) {
    cerr << "ERROR: Reference::To<> parent to child dynamic_cast failed"
	 << endl;
    return -1;
  }


  parent* parent_ptr = new parent (6);

  cerr << "\ntest - dynamic_cast<child*> (parent*)" << endl;
  child_ref = dynamic_cast<child*> (parent_ptr);


  if ( child_ref ) {
    cerr << "ERROR: Reference::To<> dynamic_cast parent to child incorrect"
	 << endl;
    return -1;
  }

  cerr << "\ntest - Reference::To<> parent to child cast successful" << endl;

  // ////////////////////////////////////////////////////////////////////////
  //
  // Test the Reference::Able destructor's automatic invalidation of
  // existing Reference::To<child> references
  //
  // ////////////////////////////////////////////////////////////////////////

  cerr << "*****************************************************" << endl
       << "*****************************************************" << endl;


  child_ptr = new child(5);

  cerr << "\ntest - Reference::To<parent> = child*" << endl;
  parent_ref = child_ptr;

  cerr << "\ntest - Reference::To<child> = Reference::To<parent>" << endl;
  child_ref = dynamic_cast<child*> (parent_ref.get());

  delete child_ptr;

  if ( parent_ref || child_ref ) {
    cerr << "ERROR:"
      " Reference::Able destructor did not invalidate Reference::To" << endl;
    return -1;
  }

  cerr << "\ntest - Reference::Able destructor clean up succesful" << endl;

  // and attempts to dereference should throw an exception
  try {
    cerr << "\ntest - Reference::To<parent>::operator ->" << endl;
    parent_ref->getval();
    cerr << "ERROR:"
      " Reference::To::operator -> did not throw exception" << endl;
    return -1;
  }
  catch (Error& error) {
    cerr << "\ntest - Reference::To::operator -> throws expected exception:"
	 << error << endl;
  }

  try {
    cerr << "\ntest - Reference::To<child>::operator *" << endl;
    func (*child_ref);
    cerr << "ERROR:"
      " Reference::To::operator * did not throw exception" << endl;
    return -1;
  }
  catch (Error& error) {
    cerr << "\ntest - Reference::To::operator * throws expected exception"
	 << error << endl;
  }
  
  cerr << "*****************************************************" << endl
       << "*****************************************************" << endl;

  // ////////////////////////////////////////////////////////////////////////
  //
  // Ensure that copy construction does not copy the reference_list
  //
  // ////////////////////////////////////////////////////////////////////////

  cerr << "\ntest - Reference::To<child> = child*" << endl;
  child_ref = new child (645);
  cerr << "\ntest - Reference::To<parent> = Reference::To<child>" << endl;
  parent_ref = child_ref;

  child_ptr = new child (*child_ref);

  cerr << "\ntest - Reference::Able::~Able" << endl;
  delete child_ptr;

  if (!parent_ref) {
    cerr << "ERROR: Reference::To"
      " invalidated through copy constructed Reference::Able" << endl;
    return -1;
  }

  // ////////////////////////////////////////////////////////////////////////
  //
  // Ensure that operator= does not copy the reference_list
  //
  // ////////////////////////////////////////////////////////////////////////


  cerr << "\ntest - Reference::To<child> = Reference::To<parent>" << endl;
  child_ref = dynamic_cast <child*> (parent_ref.get());

  child_ptr = new child;

  cerr << "\ntest - Reference::To<child>::operator *" << endl;
  *child_ptr = *child_ref;

  cerr << "\ntest - Reference::Able::~Able" << endl;
  delete child_ptr;

  if (!child_ref) {
    cerr << "ERROR: Reference::To"
      " invalidated through operator= copied Reference::Able" << endl;
    return -1;
  }


  cerr << "\n ********** Success. ********** " << endl;


  return 0;
}
