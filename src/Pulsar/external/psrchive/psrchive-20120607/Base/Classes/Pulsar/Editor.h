//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __Pulsar_Editor_h
#define __Pulsar_Editor_h

#include "TextEditor.h"

namespace Pulsar {

  class Archive;

  //! Edit the metadata in a pulsar archive
  class Editor : public TextEditor<Archive>
  {

  public:

    //! Add the named extension to the given archive
    void add_extension (Archive*, const std::string& name);

    //! Return the archive filename
    std::string get_identifier (const Archive*);

  };

}

#endif
