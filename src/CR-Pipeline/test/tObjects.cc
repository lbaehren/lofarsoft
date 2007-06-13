/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Lars Baehren (bahren@astron.nl)                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <string>
#include <iostream>

using std::cerr;
using std::cout;
using std::endl;
using std::string;

/*!
  \file tObjects.cc

  \ingroup CR-Pipeline

  \brief A number of simple tests for inheritance relationships between objects

  \author Lars B&auml;hren

  \date 2007/04/17
*/

// ------------------------------------------------------------------- Base class

/*!
  \class BaseClass

  \ingroup CR 
*/
class BaseClass {

protected:

  string className_p;
  int level_p;

  inline void incrementLevel () {
    level_p++;
  }

public:

  //! Default constructor
  BaseClass ()
    : className_p ("BaseClass"),
      level_p (0)
  {;}

  //! Destructor
  ~BaseClass () {};

  //! Get the object's hierarchy level
  inline int level () const {
    return level_p;
  }

  //! Set the name of the object's class
  inline void setClassName (string const &name) {
    className_p = name;
  }

  //! Get the name of the object's class
  inline string className () const {
    return className_p;
  }

  inline void summary () {
    cout << "-- Class name = " << BaseClass::className() << endl;
    cout << "-- Level      = " << BaseClass::level()     << endl;
  }

};

// -------------------------------------------------- First-level derived classes

/*!
  \class ChildClass

  \ingroup CR
*/
class ChildClass : public BaseClass {

public:

  //! Constructor
  ChildClass ()
    : BaseClass() {
    className_p = "ChildClass";
    level_p    += 1;
  }

  //! Destructor
  ~ChildClass () {};
  
};

/*!
  \class TemplatedChildClass

  \ingroup CR
*/
template <class T> class TemplatedChildClass : public BaseClass {

  T data_p;

public:

  //! Constructor
  TemplatedChildClass ()
    : BaseClass() {
    className_p = "TemplatedChildClass";
    level_p    += 1;
    data_p      = T(0);
  }

  //! Destructor
  ~TemplatedChildClass () {};

  inline T data () {
    return data_p;
  }

  inline void setData (T const &data) {
    data_p = data;
  }
  
};

// ------------------------------------------------- Second-level derived classes

/*!
  \class ChildChildClass

  \ingroup CR
*/
class ChildChildClass : public ChildClass {

public:

  //! Constructor
  ChildChildClass ()
    : ChildClass() {
    className_p = "ChildChildClass";
    level_p    += 1;
  }

  //! Destructor
  ~ChildChildClass () {};
  
  //! Name of parent class
  std::string parent () {
    return "ChildClass";
  }
  
};

/*!
  \class TemplatedChildChildClass

  \ingroup CR
*/
template <class T> class TemplatedChildChildClass : public TemplatedChildClass<T> {

public:

  //! Constructor
  TemplatedChildChildClass ()
    : TemplatedChildClass<T>() {
    BaseClass::setClassName ("TemplatedChildChildClass");
    BaseClass::incrementLevel();
  }

  //! Destructor
  ~TemplatedChildChildClass () {};

  //! Name of parent class
  std::string parent () {
    return "TemplatedChildClass";
  }
  
};


// ==============================================================================
// Template instantiation

template class TemplatedChildClass<int>;
template class TemplatedChildClass<float>;
template class TemplatedChildClass<double>;

template class TemplatedChildChildClass<int>;
template class TemplatedChildChildClass<float>;
template class TemplatedChildChildClass<double>;


// ==============================================================================
// Main function of the test program

int main () {

  int nofFailedTests (0);

  cout << "[1] Construction of object of type BaseClass..." << endl;
  try {
    BaseClass obj;
    obj.summary();
  } catch (string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  } 

  cout << "[2] Construction of object of type ChildClass..." << endl;
  try {
    ChildClass obj;
    obj.summary();
  } catch (string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  } 

  cout << "[3] Construction of object of type ChildChildClass..." << endl;
  try {
    ChildChildClass obj;
    obj.summary();
  } catch (string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  } 

  cout << "[4] Construction of object of type TemplatedChildClass..." << endl;
  try {
    TemplatedChildClass<int> objInt;
    TemplatedChildClass<float> objFloat;
    TemplatedChildClass<double> objDouble;
    objInt.summary();

    // Test manipulation of embedded data
    std::cout << "-- Data       = " << objInt.data() << std::endl;
    objInt.setData(1);
    std::cout << "-- Data       = " << objInt.data() << std::endl;
  } catch (string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  } 

  cout << "[5] Construction of object of type TemplatedChildChildClass..." << endl;
  try {
    TemplatedChildChildClass<int> objInt;
    TemplatedChildChildClass<float> objFloat;
    TemplatedChildChildClass<double> objDouble;
    objInt.summary();

    // Test manipulation of embedded data
    std::cout << "-- Data       = " << objInt.data() << std::endl;
    objInt.setData(1);
    std::cout << "-- Data       = " << objInt.data() << std::endl;
  } catch (string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  } 

  return nofFailedTests;
}
