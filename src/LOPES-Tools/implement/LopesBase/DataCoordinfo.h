
#ifndef DATACOORDINFO_H
#define DATACOORDINFO_H

// C++ Standard library
#include <ostream>
#include <fstream>
#include <sstream>

// AIPS++ wrapper classes
#include <casa/aips.h>
#include <casa/string.h>
#include <casa/Arrays.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Matrix.h>

using casa::String;

namespace LOPES {  // Namespace LOPES -- begin
  
  /*!
    \class DataCoordinfo
    
    \ingroup LopesBase
    
    \brief Coordinate information for the fields for the TIM40 system
    
    \author Lars B&auml;hren
    
    <h3>Synopsis</h3>
    
    This class implements the basic building block of the data structure created
    in <tt>TIM40coordinforec()</tt>.
    
    \verbatim
    ci := [=];
    ci.Pos  := [Units='Sample', DefAxis='x', Domain='T', Stored=F, 
                Ref='-', Type='real', GenFunc=symbol_value('TIM40genPos')];
    ci.Time := [Units='Seconds', DefAxis='x', Domain='T', Stored=F, 
                Ref='Pos', Type='real', DomDefault='Tx',
		ConvFunc=symbol_value('TIM40convPos2Time')];
    \endverbatim
  */
  
  class DataCoordinfo {
    
  private:
    
    String name_p;
    String units_p;
    String defAxis_p;
    String domain_p;
    bool stored_p;
    String ref_p;
    String type_p;
    String genFunc_p;
    
  public:
    
    //! Empty constructor
    DataCoordinfo ();
    
    //! Destructor
    ~DataCoordinfo();
    
    //! Get the name of the coordinate
    String name ();
    
    //! Get the name of the coordinate
    void setName (String const &);
    
    //! Get the units of the coordinate
    String units ();
    
    //! Set the units of the coordinate
    void setUnits (String const &);
    
    //! Get the name of the definition domain axis
    String defAxis ();
    
    //! Set the name of the definition domain axis
    void setDefAxis (String const &);
    
    //! Get the domain to which the coordinate belongs to.
    String domain ();
    
    //! Set the domain to which the coordinate belongs to.
    void setDomain (String const &);
    
    //! Are the data values for this coordinate stored/cached?
    bool stored ();
    
    //! Enable/Disable storage/caching of data values for this coordinate.
    void setStored (const bool);
    
    String ref ();
    
    void setRef (String const &);
    
    String type ();
    
    void setType (String const &);
    
    String genFunc ();
    
    void setGenFunc (String const &);
  };
  
}  // Namespace LOPES -- end

#endif
