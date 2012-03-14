//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Checks/Pulsar/SetReceiver.h,v $
   $Revision: 1.1 $
   $Date: 2009/02/18 02:57:09 $
   $Author: straten $ */

#ifndef __Pulsar_SetReceiver_h
#define __Pulsar_SetReceiver_h

#include "Pulsar/Correction.h"
#include "LogFile.h"

namespace Pulsar {

  //! Looks up the receiver name from a log file and installs parameters
  /*! Currently, the telescope name is hard-wired to Parkes; however,
      this class could be readily adapted to handle multiple telescopes
      if needed.
  */
  class SetReceiver : public Correction {

  public:
    
    // Return the name of the check
    std::string get_name () { return "SetReceiver"; }

    //! Correct the Signal::Source type attribute of the Archive
    void apply (Archive* archive);

    //! Get the name of the log directory for the given observation
    std::string get_directory (const Archive* archive);


  protected:

    //! Database of standard candles and their coordinates
    Reference::To<LogFile> receiver_log;

    //! Name of current log directory
    std::string current;

  };
 

}

#endif
