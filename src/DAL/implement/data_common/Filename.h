/*-------------------------------------------------------------------------*
 | $Id:: NewClass.h 2286 2009-02-03 10:50:48Z baehren                    $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2009                                                    *
 *   Lars B"ahren (bahren@astron.nl)                                       *
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

#ifndef FILENAME_H
#define FILENAME_H

// Standard library header files
#include <iostream>
#include <map>
#include <string>

namespace DAL { // Namespace DAL -- begin
  
  /*!
    \class Filename
    
    \ingroup DAL
    \ingroup data_common
    
    \brief Class to filenames matching convention
    
    \author Lars B&auml;hren

    \date 2009/09/28

    \test tFilename.cpp
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>LOFAR Data Format ICD: Naming conventions (LOFAR-USG-ICD-005)
    </ul>
    
    <h3>Synopsis</h3>

    This class implements the naming conventions for files, as laid down in
    the LOFAR Data Format ICD LOFAR-USG-ICD-005. According to the ICD the name
    of a LOFAR data file is constructed as follows:

    \verbatim
    L<Observation ID>_<Optional Descriptors>_<Filetype>.<Extension>
    \endverbatim

    In this <tt>Observation ID</tt> is a unique identifier to the observation.

    <h3>Example(s)</h3>
    
  */  
  class Filename {

  public:

    /*!
      \brief Marker for the contents of the file
      
      There will be several different kinds of data produced by
      LOFAR, see table below. Importantly, filetype signifies the kind of
      LOFAR data that comprise the particular data file, and therefore,
      will also signal the appropriate interface control document for
      further reference, should users find that necessary.
    */
    enum FileType {
      /*! LOFAR visibility file containing correlation UV information. */
      uv,
      /*! Standard LOFAR Image cube containing RA, Dec, frequency and
	polarization information. */
      sky,
      /*! Rotation Measure Synthesis Cube with axes of RA, Dec, Faraday
	Depth and polarization. */
      rm,
      /*! A Near Field sky Image with axes of position on the sky (x, y, z),
	frequency time and polarization. */
      nfi,
      /*! Dynamic Spectra file with axes of time, frequency and polarization. */
      dynspec,
      /*! A Beam-Formed file contains time series data with axes of frequency
	vs time. */
      bf,
      /*! TBB dump file is raw time-series: (1) with intensity as a function of
	frequency, or (2) containing voltage vs time. */
      tbb
    };
    
    /*!
      \brief File extensions
      
      Files generated by CASA/casacore will continue the currently
      existing conventions using upper-case suffixes.
    */
    enum FileExtension {
      //! CASA/casacore MeasurementSet
      MS,
      //! HDF5 file
      h5,
      //! FITS file
      fits,
      //! Logfile
      log,
      //! A parset file
      parset,
      //! Local sky model
      lsm,
      //! CASA/casacore image file (PagedImage)
      IM,
      //! ParmDB file generated by BBS
      PD,
      //! Dataset description file
      vds,
      //! Dataset description file
      gds,
      //! Configuration file (mostly local to station)
      conf
    };

  private:

    //! Unique identifier for the observation
    std::string observationID_p;
    //! Optional descriptors
    std::string optionalDescription_p;
    //! Marker for the contents of the file
    Filename::FileType filetype_p;
    //! Extension of the file
    Filename::FileExtension extension_p;
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    //! Default constructor
    Filename ();

    //! Argumented constructor
    Filename (std::string const &observationID,
	      std::string const &optionalDescription,
	      FileType const &filetype,
	      FileExtension const &extension);
    
    /*!
      \brief Copy constructor
      
      \param other -- Another Filename object from which to create this new
             one.
    */
    Filename (Filename const &other);
    
    // -------------------------------------------------------------- Destruction

    //! Destructor
    ~Filename ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another Filename object from which to make a copy.
    */
    Filename& operator= (Filename const &other); 
    
    // --------------------------------------------------------------- Parameters

    //! Get the unique observation ID
    inline std::string observationID () const {
      return observationID_p;
    }

    //! Set the unique observation ID
    inline void setObservationID (std::string const &observationID) {
      observationID_p = observationID;
    }

    //! Get optional descriptors
    inline std::string optionalDescription () const {
      return optionalDescription_p;
    }

    //! Set optional descriptors
    void setOptionalDescription (std::string const &optionalDescription);
    
    //! Get the file type
    inline FileType filetype () const {
      return filetype_p;
    }
    
    //! Get the file type name
    inline std::string filetypeName () const {
      return getName(filetype_p);
    }

    //! Set the file type
    inline void setFiletype (FileType const &filetype) {
      filetype_p = filetype;
    }

    //! Get the file extension type
    inline FileExtension extension () const {
      return extension_p;
    }

    //! Get the file extension name
    inline std::string extensionName () const {
      return getName (extension_p);
    }
    
    //! Set the file extension
    inline void setExtension (FileExtension const &extension) {
      extension_p = extension;
    }
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, Filename.
    */
    inline std::string className () const {
      return "Filename";
    }

    //! Provide a summary of the internal status
    inline void summary () {
      summary (std::cout);
    }

    /*!
      \brief Provide a summary of the internal status

      \param os -- Output stream to which the summary is written.
    */
    void summary (std::ostream &os);    

    // ------------------------------------------------------------------ Methods

    //! Get the name of the file
    std::string filename ();

    //! Get the file extension as string
    static std::string getName (Filename::FileExtension const &extension);
    
    //! Get the file type as string
    static std::string getName (Filename::FileType const &filetype);
    
  private:

    //! Initialize the internal parameters held by an object of this class
    void init ();
    
    //! Unconditional copying
    void copy (Filename const &other);
    
    //! Unconditional deletion 
    void destroy(void);
    
  }; // Class Filename -- end
  
} // Namespace DAL -- end

#endif /* FILENAME_H */
  
