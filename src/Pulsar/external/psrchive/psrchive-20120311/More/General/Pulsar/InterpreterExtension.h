//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/InterpreterExtension.h,v $
   $Revision: 1.4 $
   $Date: 2007/10/12 02:46:08 $
   $Author: straten $ */

#ifndef __Pulsar_Interpreter_Extension_h
#define __Pulsar_Interpreter_Extension_h

#include "Pulsar/Interpreter.h"

namespace Pulsar {

  class Interpreter::Extension : public CommandParser {

  public:

    enum Status { Good, Warn, Fail };

    //! get the current Archive
    Archive* get ()
    { return interpreter->get(); }

    //! get the named Archive
    Archive* getmap (const std::string& name)
    { return interpreter->getmap(name); }

    std::vector<std::string> setup (const std::string& text)
    { return interpreter->setup (text); }

    //! Parses arguments as a single instance of T
    template<typename T> T setup (const std::string& args)
    { return interpreter->setup<T> (args); }

    //! Parses arguments as an optional single instance of T
    template<typename T> T setup (const std::string& args, T default_value)
    { return interpreter->setup<T> (args, default_value); }

    std::string response (Status s, const std::string& text = "")
      { return interpreter->response ((Interpreter::Status)s, text); }

  protected:

    friend class Interpreter;

    //! The parent Interpreter
    Reference::To<Interpreter, false> interpreter;

  };

}

#endif
