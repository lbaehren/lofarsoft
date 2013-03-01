/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include <iostream>

#include "HeapTracked.h"
#include "Error.h"

using namespace std;

class grandparent : public Reference::HeapTracked {

public:
  grandparent () { value = 0; };
  virtual int getval () const { return value; }
protected:
  int value;

};

class parent1 : virtual public grandparent {

public:
  parent1 (int val = 0) { value = val; }
  virtual void setval (int val) { value = val; }

};

class parent2 : virtual public grandparent {

public:
  parent2 () { }
  virtual void printval () const { cerr << "val=" << value << endl; }

};

class child1 : public parent1, public parent2 {
public:
  child1 (int val=0) : parent1 (val) { valch = val * 2; }

  virtual int getval () const { return valch; }
protected:
  int valch;
  char space[4096];
};

class child2 : public parent2 {

public:
  child2 () { }
  child1 child;

};


int main ()
{
  // ////////////////////////////////////////////////////////////////////////
  //
  // Test Reference::HeapTracked constructor and
  // Reference::HeapTracked::operator new ability to distinguish
  // between objects on and off the heap
  //
  // ////////////////////////////////////////////////////////////////////////
  parent1 parent1_instance(4);

  if ( parent1_instance.__is_on_heap() ) {
    cerr << "ERROR: Static object thought to be on heap." << endl;
    return -1;
  }

  child1* child1_ptr = new child1(3);
    
  if ( !child1_ptr->__is_on_heap() ) {
    cerr << "ERROR: Multiple Inheritance Heap object not detected." << endl;
    return -1;
  }

  try {
    delete child1_ptr;
  }
  catch (Error& error) {
    cerr << "ERROR: while deleting Multiple Inheritance Heap object" 
         << error << endl;
    return -1;
  }

  cerr << "\nReference::HeapTracked::operator new heap detection successful"
       << endl;

  // ////////////////////////////////////////////////////////////////////////
  //
  // Test Reference::HeapTracked constructor and
  // Reference::HeapTracked::operator new ability to disable automatic
  // deletion of elements within an array
  //
  // ////////////////////////////////////////////////////////////////////////

  parent2* parent2_array = new parent2 [32];

  if (parent2_array[0].__is_on_heap() || parent2_array[3].__is_on_heap()) {
    cerr << "ERROR: Cannot auto-delete objects in dynamic arrays" << endl;
    return -1;
  }

  child2 child2_static_array [16];
    
  if ( child2_static_array[0].__is_on_heap() 
       || child2_static_array[3].__is_on_heap() ) {
    cerr << "ERROR: Cannot auto-delete objects in static arrays" << endl;
    return -1;
  }

  std::vector<child1> child1_vector (13);

  if (child1_vector[0].__is_on_heap() || child1_vector[3].__is_on_heap()){
    cerr << "ERROR: Cannot auto-delete objects in vectors" << endl;
    return -1;
  }

  cerr << "\nReference::HeapTracked::operator new array detection successful"
       << endl;

  return 0;
}


