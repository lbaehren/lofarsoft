/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2006                                                    *
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

#ifndef SKYMAPQUANTITY_H
#define SKYMAPQUANTITY_H

#include <iostream>
#include <string>

// AIPS++/CASA header files
#include <casa/aips.h>
#include <casa/iostream.h>
#include <casa/Exceptions/Error.h>

#include <Coordinates/CoordinateType.h>

using CR::CoordinateType;

namespace CR {  // Namespace CR -- begin
  
  /*!
    \class SkymapQuantity
    
    \ingroup CR_Coordinates
    
    \brief Container for the settings to determine the quantity of a skymap
    
    \author Lars B&auml;hren
    
    \date 2006/09/18
    
    \test tSkymapQuantity.cc
    
    <h3>Prerequisite</h3>
    
    --/--
    
    <h3>Synopsis</h3>
    
    In order to provide a unified scheme by which to propagate information on the
    quantity to be stored in the sky map, this class holds a list of the various 
    quantities.
    
    <h3>Example(s)</h3>
    
    The code example below show a typical usage scenario within a <tt>switch</tt>
    statement (taken from the Skymap class):
    \code
    SkymapQuantity qnt = SkymapQuantity (SkymapQuantity::TIME_FIELD);
    
    switch (qnt) {
      case SkymapQuantity::FREQ_FIELD:
        domain   = "Frequency";
	quantity = "Field";
	pFunction = &Skymap::addAntennaSignals;
      break;
      case SkymapQuantity::FREQ_POWER:
        domain   = "Frequency";
	quantity = "power";
	pFunction = &Skymap::addAntennaSignals;
      break;
    }
    \endcode
  */
  
  class SkymapQuantity {
    
  public:
    
    /*!
      \brief Electrical quantities, for which the sky map is generated
    */
    typedef enum {
      /*!
	Electric field strength as function of frequency,
	\f[ \widetilde S (\vec\rho,\nu) = \frac{1}{N} \sum_{j=1}^{N}
	w_{j}(\vec \rho,\nu) \widetilde{s_{j}}(\nu) \f]
      */
      FREQ_FIELD,
      /*!
	<b>Directed spectral power</b> (power of the electric field as function
	of frequency) <br>
	\f[ \widetilde P (\vec\rho,\nu) = \overline{\widetilde S (\vec\rho,\nu)}
	\cdot \widetilde S (\vec\rho,\nu) \f]
	where 
	\f[ \widetilde S (\vec\rho,\nu) = \frac{1}{N_{\rm Ant}}
	\sum_{j=1}^{N_{\rm Ant}} \widetilde S_{j} (\vec\rho,\nu) =
	\frac{1}{N_{\rm Ant}} \sum_{j=1}^{N_{\rm Ant}} w (\vec x_j, \vec \rho, \nu)
	\widetilde s_{j} (\nu) \f]
	in which \f$ w \f$ is the weighting factor for each combination of antenna,
	pointing direction and frequency and \f$ \widetilde s_j \f$ is the Fourier
	transform of the data from antenna \f$ j \f$.
	
	While in the above step the beam beam is computed by summation over all 
	selected antennas, there also is the option to sum over antenna pairs,
	i.e. baselines
	\f[ \widetilde P (\vec\rho,\nu) = \left( \frac{N^2-N}{2} \right)^{-1}
	\sum_{j=1}^{N} \sum_{k>j}^{N} \overline{w_{j}(\vec\rho,\nu)
	\widetilde s_{j}(\nu)} w_{k}(\vec\rho,\nu) \widetilde s_{k}(\nu) \f]
	
	Instead of computing the beam from the spectra \f$ \widetilde s_j (\nu) \f$
	of the individual antennas, the directed power also can be computed from
	the array correlation matrix (ACM), \f$ C_{jk}(\nu) =
	\overline{\widetilde s_j(\nu)} \widetilde s_k(\nu) \f$.
	\f[ \widetilde P (\vec\rho,\nu) = \left( \frac{N^2-N}{2} \right)^{-1}
	\sum_{j=1}^{N} \sum_{k>j}^{N} \overline{w_{j}(\vec\rho,\nu)}
	w_{k}(\vec\rho,\nu) \, C_{jk}(\nu) \f]
      */
      FREQ_POWER,
      /*!
	Electric field strength as function of time (sample resolution)
	
	\f[ S (\vec\rho,t) = \mathcal{F}^{-1} \left\{ \widetilde S(\vec\rho,\nu)
	\right\} \f]
      */
      TIME_FIELD,
      /*!
	Power of the electric field as function of time
	
	\f[ P (\vec\rho,\nu) = \left| S (\vec\rho,t) \right|^{2} \f]
      */
      TIME_POWER,
      /*!
	<b>Cross-correlation beam</b> (cc-beam)<br>
	The data from each unique pair of antennas is multiplied, the resulting
	values are averaged, and then the square root is taken while preserving
	the sign.
	\f[
	cc(\vec \rho)[t] = \, ^+_- \sqrt{\left| \frac{1}{N_{Pairs}} \sum^{N-1}_{i=1}
	\sum^{N}_{j>i} s_i(\vec\rho)[t] s_j(\vec\rho)[t] \right|}
	\f]
	where 
	\f[
	s_j(\vec\rho)[t]
	= \mathcal{F}^{-1} \left\{ \tilde s_j(\vec\rho)[k] \right\}
	= \mathcal{F}^{-1} \Bigl\{ w_j(\vec\rho)[k] \cdot \tilde s_j[k] \Bigr\}
	\f]
	is the time shifted field strength of the single antennas for a direction
	\f$\vec \rho \f$. \f$ N \f$ is the number of antennas, \f$t\f$ the time or
	pixel index and \f$N_{Pairs}\f$ the number of unique pairs of antennas.
	The negative sign is taken if the sum had a negative sign before taking the
	absolute values, and the positive sign otherwise.
	Computation - for each direction in the sky - is performed as follows:
	<ol>
	<li>Compute the shifted time-series for all antennas,
	\f$ s_j (\vec\rho,t) \f$, by first applying the weights to the Fourier
	transformed data and thereafter transforming back to time domain.
	<li>Sum over unique products from all antenna pairs and normalize by
	the number of such pairs.
	<li>Take the square root of the sum and multiply with the sign of the
	some.
	</ol>       */
      TIME_CC,
      /*!
	<b>Power-beam</b> (p-beam)
	\f[
	P (\vec\rho,\nu)
	\ = \ \sqrt{\frac{1}{N} \sum_{j=1}^{N} \left( s_{j}(\vec\rho,\nu) \right)^2}
	\ = \ \sqrt{\frac{1}{N} \sum_{j=1}^{N} \left( \mathcal{F}^{-1} \Bigl\{
	\widetilde s_{j}(\vec\rho,\nu) \Bigr\} \right)^{2}} 
	\ = \ \sqrt{\frac{1}{N} \sum_{j=1}^{N} \left( \mathcal{F}^{-1} \Bigl\{
	w_{j}(\vec\rho,\nu) \widetilde s_{j}(\nu) \Bigr\} \right)^{2}} 
	\f]
      */
      TIME_P,
      /*!
	<b>Excess-beam</b> (x-beam)
	
	\f[
	P_{x}(\vec\rho,t) = P_{cc}(\vec\rho,t) \cdot \left| \frac{\langle
	P_{cc}(\vec\rho,t) \rangle}{\langle P(\vec\rho,t) \rangle} \right|
	\f]
      */
      TIME_X
    } Type ;
    
  private:
    
    //! Coordinate type
    CoordinateType coordType_p;
    //! Electrical quantity
    SkymapQuantity::Type quantity_p;
    
  public:
    
    // --------------------------------------------------------------- Construction
    
    //! Default constructor
    SkymapQuantity ();
    
    /*!
      \brief Argumented constructor

      \param quantity -- Enumeration ID of the skymap quantity for which to create
             the object.
    */
    SkymapQuantity (SkymapQuantity::Type const &quantity);

    /*!
      \brief Argumented constructor

      If no valid SkymapQuantity::Type can be constructed from the provided input
      parameters, \c FREQ_POWER is used as default value for the new object.
      
      \param domain   -- Name of the domain (\c TIME, \c FREQ)
      \param quantity -- Name of the quantity (\c FIELD, \c POWER, \c CC)
    */
    SkymapQuantity (std::string const &domain,
		    std::string const &quantity);
    
    /*!
      \brief Copy constructor
      
      \param other -- Another SkymapQuantity object from which to create this new
             one.
    */
    SkymapQuantity (SkymapQuantity const &other);
    
    // ---------------------------------------------------------------- Destruction
    
    //! Destructor
    ~SkymapQuantity ();
    
    // ------------------------------------------------------------------ Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another SkymapQuantity object from which to make a copy.
    */
    SkymapQuantity& operator= (SkymapQuantity const &other); 
    
    // ----------------------------------------------------------------- Parameters
    
    /*!
      \brief Get the SkymapQuantity type
      
      \return type -- Type of the SkymapQuantity
    */
    inline SkymapQuantity::Type type () const {
      return quantity_p;
    }
    
    /*!
      \brief Get the SkymapQuantity name
      
      \return name -- Name of the SkymapQuantity
    */
    std::string name ();
    
    /*!
      \brief Set the SkymapQuantity type
      
      \param quantity -- Type of the SkymapQuantity
    */
    void setSkymapQuantity (SkymapQuantity::Type const &quantity);

    /*!
      \brief Get the coordinate type of the skymap quantity

      \return coordType -- The coordinate type of the skymap quantity
    */
    inline CoordinateType coordinateType () const {
      return coordType_p;
    }
    
    /*!
      \brief Get the domain in which the quantity is defined
      
      \return name -- The name of the domain of the electrical quantity.
    */
    inline std::string domainName () {
      return coordType_p.name();
    }
    
    /*!
      \brief Get the type of the domain in which the quantity is defined
      
      \return domain -- The type of the domain of the electrical quantity.
    */
    inline CoordinateType::Types domainType () {
      return coordType_p.type();
    }
    
    /*!
      \brief Get the type of electrical quantity
      
      \return quantity -- Electrical quantity: <i>Field</i>, <i>Power</i>
    */
    std::string quantity ();
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, SkymapQuantity.
    */
    inline std::string className () const {
      return "SkymapQuantity";
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

    // -------------------------------------------------------------------- Methods

    /*!
      \brief Get the skymap type from combination of domain and quantity

      \retval skymapType -- Type identifying the SkymapQuantity
      \param domain      -- Name of the domain (\c TIME, \c FREQ)
      \param quantity    -- Name of the quantity (\c FIELD, \c POWER, \c CC)

      \return status -- Status of the operation; returns <tt>false</tt> in case
              no valid SkymapQuantity::Type can be constructed from the input
	      parameters.
    */
    static bool getType (SkymapQuantity::Type &skymapType,
			 std::string const &domain,
			 std::string const &quantity);
    /*!
      \brief Convert BeamType to combination of domain and quantity label
      
      \retval domain   -- 
      \retval quantity -- 
      \param beamType  -- 

      \return status -- Status of the operation; returns <i>false</i> if an
              an error was encountered
    */
    static bool getType (std::string &domain,
			 std::string &quantity,
			 SkymapQuantity::Type const &skymapType);
    
  private:
    
    //! Unconditional copying
    void copy (SkymapQuantity const &other);
    
    //! Unconditional deletion 
    void destroy(void);
    
  };
  
}  // Namespace CR -- end

#endif /* SKYMAPQUANTITY_H */
