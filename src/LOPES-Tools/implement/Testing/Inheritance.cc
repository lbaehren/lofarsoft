
#include <Testing/Inheritance.h>

// ==============================================================================
//
//  Parent class
//
// ==============================================================================

namespace LOPES {

  Parent::Parent () {
    init ();
  }

  Parent::~Parent () {
  }

  void Parent::init () {
    className_p = "Parent";
  }

}

// ==============================================================================
//
//  Child class, A
//
// ==============================================================================

namespace LOPES {

  ChildA::ChildA ()
    : Parent () {
    init ();
  }

  ChildA::~ChildA () {
  }

  void ChildA::init () {
    className_p = "ChildA";
  }

}

// ==============================================================================
//
//  Child class, B
//
// ==============================================================================

namespace LOPES {

  ChildB::ChildB ()
    : Parent () {
    init ();
  }

  ChildB::~ChildB () {
  }

  void ChildB::init () {
    className_p = "ChildB";
  }

}

// ==============================================================================
//
//  Child class, AA
//
// ==============================================================================

namespace LOPES {

  ChildAA::ChildAA ()
    : ChildA () {
    init ();
  }

  ChildAA::~ChildAA () {
  }

  void ChildAA::init () {
    className_p = "ChildAA";
  }

}

// ==============================================================================
//
//  Child class, AB
//
// ==============================================================================

namespace LOPES {

  ChildAB::ChildAB ()
    : ChildA () {
    init ();
  }

  ChildAB::~ChildAB () {
  }

  void ChildAB::init () {
    className_p = "ChildAB";
  }

}

// ==============================================================================
//
//  Child class, AC
//
// ==============================================================================

namespace LOPES {

  ChildAC::ChildAC ()
    : ChildA () {
    init ();
  }

  ChildAC::~ChildAC () {
  }

  void ChildAC::init () {
    className_p = "ChildAC";
  }

}
