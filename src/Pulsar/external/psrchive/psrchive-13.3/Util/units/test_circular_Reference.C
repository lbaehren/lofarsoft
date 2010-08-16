/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#define _DEBUG 1

#include <iostream>
using namespace std;

#include "Reference.h"

class Manager;

class ManageAble: public Reference::Able {

public:

  ~ManageAble () { cerr << "ManageAble destruct" << endl; }
  Reference::To<Manager, false> my_manager;
  
};

class Manager: public Reference::Able {
 
public:

  Manager () { instance_count ++; }
  ~Manager () { cerr << "Manager destruct" << endl; instance_count --; }

  void manage (ManageAble* manage_this) 
  {
    cerr << "Manager::manage set ManageAble::my_manager" << endl;
    manage_this->my_manager = this;
    cerr << "Manager::manage my_managed.push_back" << endl;
    my_managed.push_back (manage_this);
    cerr << "Manager::manage return" << endl;
  }
  
  std::vector< Reference::To<ManageAble> > my_managed;

  static unsigned instance_count;
};

unsigned Manager::instance_count = 0;


int main (int argc, char** argv)
{
  Manager* manager_ptr = new Manager;            // refcount = 0

  cerr << "Set reference" << endl;
  Reference::To<Manager> manager = manager_ptr;  // refcount = 1

  cerr << "Manager::manage" << endl;
  manager->manage (new ManageAble);              // refcount = 2 !

  cerr << "Remove reference" << endl;
  manager = 0;                                   // refcount = 1 !!!

  if (Manager::instance_count != 0) {
    cerr << "The Manager instance is not deleted becuase the ManageAble\n"
      "instance is still refering to it!" << endl;
    return -1;
  }

  cerr << "Success!" << endl;
  return 0;
}
