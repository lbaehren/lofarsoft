
#include <iostream>
#include <cstring>

using std::string;

// ==============================================================================
//
//  Parent class
//
// ==============================================================================

namespace LOPES {  // Namespace LOPES -- begin

  /*!
    \class Parent
  
    \brief The class functioning as parent to a number of chidren

    This class - and its Children - is intended for testing compiler dependent
    recognition of inheriated features (e.g. variables defined in the base
    class and later on used by the children classes).
  */
  class Parent {

    //! Name of the class, private variable
    string myname_p;

  protected:
    
    //! The name of the class, shared variable
    string className_p;
    
  public:
    
    //! Constructor
    Parent ();
    
    //! Destructor
    ~Parent ();
    
    //! Initialization of internal data
    virtual void init ();
    
    //! Get the name of the class
    virtual string name () {
      return className_p;
    }
    
  };
  
}  // Namespace LOPES -- end

// ==============================================================================
//
//  Child class, A
//
// ==============================================================================

namespace LOPES {  // Namespace LOPES -- begin

  /*!
    \class ChildA

    \brief One of the children of Parent
  */
  class ChildA : public Parent {

  public:

    //! Constructor
    ChildA ();

    //! Destructor
    ~ChildA ();

    //! Initialization of internal data
    void init ();

  };

}  // Namespace LOPES -- end

// ==============================================================================
//
//  Child class, B
//
// ==============================================================================

namespace LOPES {  // Namespace LOPES -- begin

  /*!
    \class ChildB

    \brief One of the children of Parent
  */
  class ChildB : public Parent {

  public:

    //! Constructor
    ChildB ();

    //! Destructor
    ~ChildB ();

    //! Initialization of internal data
    void init ();

  };

}  // Namespace LOPES -- end

// ==============================================================================
//
//  Child class, AA
//
// ==============================================================================

namespace LOPES {  // Namespace LOPES -- begin

  /*!
    \class ChildAA

    \brief One of the children of Parent
  */
  class ChildAA : public ChildA {

  public:

    //! Constructor
    ChildAA ();

    //! Destructor
    ~ChildAA ();

    //! Initialization of internal data
    void init ();

  };

}  // Namespace LOPES -- end

// ==============================================================================
//
//  Child class, AB
//
// ==============================================================================

namespace LOPES {  // Namespace LOPES -- begin

  /*!
    \class ChildAB

    \brief One of the children of Parent
  */
  class ChildAB : public ChildA {
    
  public:

    //! Constructor
    ChildAB ();

    //! Destructor
    ~ChildAB ();

    //! Initialization of internal data
    void init ();

  };

}  // Namespace LOPES -- end

// ==============================================================================
//
//  Child class, AC
//
// ==============================================================================

namespace LOPES {  // Namespace LOPES -- begin

  /*!
    \class ChildAC

    \brief One of the children of Parent
  */
  class ChildAC : public ChildA {
    
  public:

    //! Constructor
    ChildAC ();

    //! Destructor
    ~ChildAC ();

    //! Initialization of internal data
    void init ();

  };

}  // Namespace LOPES -- end

