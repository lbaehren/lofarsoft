/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include <iostream>

#include "Registry.h"


class A {

public:
  virtual ~A() { }

  virtual bool match (char num) = 0;

  static A* factory (char num) {

    Registry::List<A>& registry = Registry::List<A>::get_registry();

    for (unsigned ichild=0; ichild<get_nchild(); ichild++)
      if (registry[ichild]->match (num))
	return registry.create(ichild);

    std::cerr << "A::factory no matching child" << std::endl;
    return 0;
  }

  static unsigned get_nchild ()
  { return Registry::List<A>::get_registry().size(); }
};

// correct way: B has a static Registry::List<A>::Enter<B> entry;
class B : public A {
  bool match (char num) { return num == 'B'; }
  static Registry::List<A>::Enter<B> entry;  
};

Registry::List<A>::Enter<B> B::entry;

// the Registry is robust against multiple sub-class registration instances
Registry::List<A>::Enter<B> mistake; 

class C : public A {
  bool match (char num) { return num == 'C'; }
};

// the Registry entry need not be a member of the class
Registry::List<A>::Enter<C> C_entry;  


int main ()
{
  if (A::get_nchild() != 2) {
    std::cerr << "A::children != 2!" << std::endl;
    return -1;
  }

  std::cerr << "A::children=" << A::get_nchild() << std::endl;
  return 0;
}
