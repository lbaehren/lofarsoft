//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/TapeInfo.h,v $
   $Revision: 1.3 $
   $Date: 2008/04/21 06:20:00 $
   $Author: straten $ */

#ifndef __Pulsar_TapeInfo_h
#define __Pulsar_TapeInfo_h

#include "Pulsar/ArchiveExtension.h"

namespace Pulsar {

  //! Stores information about the tape on which the data were recorded
  class TapeInfo : public Archive::Extension {

  public:
    
    //! Default constructor
    TapeInfo ();

    //! Copy constructor
    TapeInfo (const TapeInfo&);

    //! Operator =
    const TapeInfo& operator= (const TapeInfo&);

    //! Destructor
    ~TapeInfo ();

    //! Clone method
    TapeInfo* clone () const { return new TapeInfo( *this ); }
 
    //! Get file number for raw data (FB only)
    int get_file_number() const { return file_number; }
    
    //! Set file number for raw data (FB only)
    void set_file_number(int number) { file_number = number; }
    
    //! Get tape label for raw data (FB only)
    std::string get_tape_label() const { return tape_label; }
    
    //! Set tape label for raw data (FB only)
    void set_tape_label(std::string label) { tape_label = label; }

  protected:

    std::string tape_label;
    int file_number;

  };
 

}

#endif
