/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2008                                                    *
 *   Lars B"ahren (lbaehren@gmail.com)                                     *
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

#ifndef COORDINATEDOMAIN_H
#define COORDINATEDOMAIN_H

// Standard library header files
#include <iostream>
#include <string>

namespace CR { // Namespace CR -- begin
  
  /*!
    \class CoordinateDomain
    
    \ingroup CR_Coordinates
    
    \brief Container for information concerning the domain of a coordinate
    
    \author Lars B&auml;hren

    \date 2008/09/09

    \test tCoordinateDomain.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>CR::CoordinateType
    </ul>
    
    <h3>Synopsis</h3>

    <h3>Example(s)</h3>
    
  */  
  class CoordinateDomain {
    
  public:
    
    /*!
      \brief Domains in which in which coordinates can be defined
    */
    enum Types {
      //! Time domain
      Time,
      //! Frequency domain, for spectral coordinates
      Frequency,
      //! Spatial domain, for positions and lengths
      Space
    };

  protected:

    //! Domain identifier
    CoordinateDomain::Types type_p;

  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Argumented constructor
    */
    CoordinateDomain (CoordinateDomain::Types const &type);
    
    /*!
      \brief Argumented constructor
    */
    CoordinateDomain (std::string const &domain);
    
    /*!
      \brief Copy constructor
      
      \param other -- Another CoordinateDomain object from which to create this new
      one.
    */
    CoordinateDomain (CoordinateDomain const &other);
    
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    ~CoordinateDomain ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another CoordinateDomain object from which to make a copy.
    */
    CoordinateDomain& operator= (CoordinateDomain const &other); 
    
    // --------------------------------------------------------------- Parameters

    /*!
      \brief Get the type (ID) of the coordinate domain

      \return type -- The type (as ID) of the coordinate domain
    */
    inline CoordinateDomain::Types type () const {
      return type_p;
    }

    /*!
      \brief Get the name of the coordinate domain 

      \return domain -- The name of the coordinate domain 
    */
    inline std::string domain () const {
      return domain (type_p);
    }
    
    /*!
      \brief Set the domain by type

      \param domain -- The type (as ID) of the coordinate domain

      \return status -- The status of the operation; returns \e false in case
              an error was encountered, e.g. if an invalid identifier was 
	      provided.
    */
    bool setDomain (CoordinateDomain::Types const &domain);

    bool setDomain (std::string const &domain);

    /*!
      \brief Is the domain spatial?

      \return isSpatial -- Returns \e true if the coordinate domain is of type
              "Space".
    */
    inline bool isSpatial () const {
      return isSpatial (type_p);
    };

    /*!
      \brief Is the domain spatial?

      \param type -- The type of the domain.

      \return isSpatial -- Returns \e true if the coordinate domain is of type
              "Space".
    */
    static bool isSpatial (CoordinateDomain::Types const &type);

    /*!
      \brief Is the domain temporal?

      \return isTemporal -- Returns \e true if the coordinate domain is of type
              "Time".
    */
    inline bool isTemporal () const {
      return isTemporal (type_p);
    }
    
    /*!
      \brief Is the domain temporal?

      \param type -- The type of the domain.

      \return isTemporal -- Returns \e true if the coordinate domain is of type
              "Time".
    */
    static bool isTemporal (CoordinateDomain::Types const &type);
    
    /*!
      \brief Is the domain spectral?

      \return isSpectral -- Returns \e true if the coordinate domain is of type
              "Frequency".
    */
    inline bool isSpectral () const {
      return isSpectral (type_p);
    }

    /*!
      \brief Is the domain spectral?

      \param type -- The type of the domain.

      \return isSpectral -- Returns \e true if the coordinate domain is of type
              "Frequency".
    */
    static bool isSpectral (CoordinateDomain::Types const &type);

    /*!
      \brief Get the name of the coordinate domain 

      \param type -- The type identifying the domain

      \return domain -- The name of the coordinate domain 
    */
    static std::string domain (CoordinateDomain::Types const &type);
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, CoordinateDomain.
    */
    inline std::string className () const {
      return "CoordinateDomain";
    }

    /*!
      \brief Provide a summary of the internal status
    */
    inline void summary () {
      summary (std::cout);
    }

    /*!
      \brief Provide a summary of the internal status

      \param os -- Output stream to which the summary is written.
    */
    void summary (std::ostream &os);    

    // ------------------------------------------------------------------ Methods
    
    /*!
      \brief Get the base unit for a quantity in the given domain

      \return unit -- The base unit for a quantity in the given domain, e.g. "s"
              for the time domain.
    */
    inline std::string baseUnit () const {
      return baseUnit (type_p);
    }

    /*!
      \brief Get the base unit for a quantity in the given domain

      \param type -- The type identifying the domain

      \return unit -- The base unit for a quantity in the given domain, e.g. "s"
              for the time domain.
    */
    static std::string baseUnit (CoordinateDomain::Types const &type);
    
  private:
    
    /*!
      \brief Unconditional copying
    */
    void copy (CoordinateDomain const &other);
    
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);
    
  };
  
} // Namespace CR -- end

#endif /* COORDINATEDOMAIN_H */
  
