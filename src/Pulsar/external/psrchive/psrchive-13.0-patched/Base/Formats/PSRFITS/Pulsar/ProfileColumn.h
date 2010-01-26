//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Formats/PSRFITS/Pulsar/ProfileColumn.h,v $
   $Revision: 1.2 $
   $Date: 2009/06/08 19:12:58 $
   $Author: straten $ */

#ifndef __Pulsar_ProfileColumn_h
#define __Pulsar_ProfileColumn_h

#include "Reference.h"

#include <fitsio.h>

namespace Pulsar {

  class Profile;

  //! Loads and unloads Profile vector from PSRFITS archives

  class ProfileColumn : public Reference::Able {

  public:

    //! Default constructor
    ProfileColumn ();

    //! Set the fitsfile to/from which data are written/read
    void set_fitsfile (fitsfile* fptr);

    //! Set the name of the data column
    void set_data_colname (const std::string&);

    //! Get the index of the data column
    unsigned get_data_colnum ();

    //! Set the name of the offset column
    void set_offset_colname (const std::string&);

    //! Get the index of the data column
    unsigned get_offset_colnum ();
    
    //! Set the name of the scale column
    void set_scale_colname (const std::string&);

    //! Get the index of the data column
    unsigned get_scale_colnum ();

    //! Set the number of phase bins
    void set_nbin (unsigned);

    //! Set the number of frequency channels
    void set_nchan (unsigned);

    //! Set the number of profiles in each frequency channel
    void set_nprof (unsigned);

    //! Resize the columns
    void resize ();

    //! Insert the columns and resize
    void create (unsigned start_column);

    //! Unload the given vector of profiles
    void unload (int row, const std::vector<const Profile*>&);

    //! Load the given vector of profiles
    void load (int row, const std::vector<Profile*>&);

  protected:

    fitsfile* fptr;

    std::string data_colname;
    std::string offset_colname;
    std::string scale_colname;

    int data_colnum;
    int offset_colnum;
    int scale_colnum;

    unsigned nbin, nchan, nprof;

    bool verbose;

    //! reset the column indeces
    void reset ();

    //! Get the column number for the specified column name
    int get_colnum (const std::string& name);

    template<typename T, typename C>
    void load_amps (int row, C&);
  };

}

#endif
