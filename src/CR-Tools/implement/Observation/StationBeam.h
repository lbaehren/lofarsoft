/*-------------------------------------------------------------------------*
 | $Id: template-class.h,v 1.20 2007/06/13 09:41:37 bahren Exp           $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Kalpana Singh (<k.singh@astro.ru.nl>)                                 *
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

#ifndef STATIONBEAM_H
#define STATIONBEAM_H

// Standard library header files
#include <cmath>
#include <iostream>
#include <string>

// AIPS++/CASA header files

#include <casa/aips.h>
#include <casa/iostream.h>
#include <casa/fstream.h>
#include <casa/string.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/ArrayIO.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Vector.h>
#include <casa/BasicSL/Complex.h>

#include <casa/Quanta.h>
#include <casa/Exceptions/Error.h>
#include <tables/Tables/Table.h>
#include <tables/Tables/TableDesc.h>
#include <tables/Tables/SetupNewTab.h>

#include <tables/Tables/ScalarColumn.h>
#include <tables/Tables/ScaColDesc.h>
#include <tables/Tables/ScaRecordColDesc.h>

#include <casa/namespace.h>

namespace CR { // Namespace CR -- begin
  
  /*!
    \class StationBeam
    
    \ingroup Observation
    
    \brief Brief description for class StationBeam
    
    \author Lars Baehren

    \date 2007/08/10

    \test tStationBeam.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>[start filling in your text here]
    </ul>
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>
    
  */  
  class StationBeam {
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    StationBeam ();
    
    /*!
      \brief Copy constructor
      
      \param other -- Another StationBeam object from which to create this new
      one.
    */
    
    StationBeam( const Double& source_declination,
                 const Double& source_hr_angle,
		 const Double& sky_temp,
		 const Double& moon_temp,
		 const Vector<uint>& station_id,
                 const Double& freq_init,
		 const Double& bandwidth,
		 const Double& station_radii,
		 Vector<Double>& position_x,
		 Vector<Double>& position_y,
		 const Vector<Double>& legendre_root,
		 const Vector<Double>& legendre_weight ) ;
		 
		 
    StationBeam (StationBeam const &other);
    
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    virtual ~StationBeam ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another StationBeam object from which to make a copy.
    */
    StationBeam& operator= (StationBeam const &other); 
    
    // --------------------------------------------------------------- Parameters
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, StationBeam.
    */
    std::string className () const {
      return "StationBeam";
    }

    /*!
      \brief Provide a summary of the internal status
    */
    inline void summary () {
      summary (std::cout);
    }

    /*!
      \brief Provide a summary of the internal status
    */
    void summary (std::ostream &os);    

    // ------------------------------------------------------------------ Methods
    
    
  Double station_beam(const Double& frequency,
		      const Double& hr_angle,
		      const Double& declination,
		      const Double& source_declination,
          	      const Double& source_hr_angle,
		      const Double& station_radii,
		      const Vector<Double>& legendre_root,
		      const Vector<Double>& legendre_weight ) ;
				      
 
   
  Double tied_array( const Double& frequency,
	             const Double& hr_angle,
		     const Double& declination,
		     const Double& source_declination,
		     const Double& source_hr_angle,
		     const Double& station_radii,
		     const Vector<uint>& station_id,
		     Vector<Double>& position_x,
		     Vector<Double>& position_y,
		     const Vector<Double>& legendre_root,
		     const Vector<Double>& legendre_weight ) ;
		     

 Double integrate_moon(  const Double& source_declination,
		         const Double& source_hr_angle,
		         const Double& station_radii,
		         const Vector<uint>& station_id,
		         const Double& freq_init,
		         const Double& bandwidth,
		         Vector<Double>& position_x,
		         Vector<Double>& position_y,
		         const Vector<Double>& legendre_root,
		         const Vector<Double>& legendre_weight )  ;
		    
		    
		    
 Double integrate_sky(  const Double& source_declination,
	                const Double& source_hr_angle,
	                const Double& station_radii,
	                const Vector<uint>& station_id,
		        const Double& freq_init,
		        const Double& bandwidth,
		        Vector<Double>& position_x,
		        Vector<Double>& position_y,
		        const Vector<Double>& legendre_root,
		        const Vector<Double>& legendre_weight )  ;

			    
Double beam_width(  const Double& source_declination,
                    const Double& source_hr_angle,
	            const Double& station_radii,
	            const Vector<uint>& station_id,
		    const Double& freq_init,
		    const Double& bandwidth,
		    Vector<Double>& position_x,
		    Vector<Double>& position_y,
		    const Vector<Double>& legendre_root,
		    const Vector<Double>& legendre_weight ) ;    
		       		    		     		     		     
 Double temp_final(    const Double& source_declination,
                       const Double& source_hr_angle,
		       const Double& sky_temp,
		       const Double& moon_temp,
		       const Vector<uint>& station_id,
                       const Double& freq_init,
		       const Double& bandwidth,
		       const Double& station_radii,
		       Vector<Double>& position_x,
		       Vector<Double>& position_y,
		       const Vector<Double>& legendre_root,
		       const Vector<Double>& legendre_weight   )      ;
		    
 Double int_azimuth( const Double& frequency,
	             const Vector<Double>& hr_angle,
		     const Double& source_declination,
		     const Double& source_hr_angle,
		     const Double& station_radii,
		     const Vector<uint>& station_id,
		     Vector<Double>& position_x,
		     Vector<Double>& position_y,
		     const Vector<Double>& legendre_root,
		     const Vector<Double>& legendre_weight )      ;    

Double int_frequency( const Vector<Double>& frequency,
	              const Double& source_declination,
		      const Double& source_hr_angle,
		      const Double& station_radii,
		      const Vector<uint>& station_id,
		      Vector<Double>& position_x,
		      Vector<Double>& position_y,
		      const Vector<Double>& legendre_root,
		      const Vector<Double>& legendre_weight )      ;	    
		     
  private:
    
    /*!
      \brief Unconditional copying
    */
    void copy (StationBeam const &other);
    
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);
    
  };
  
} // Namespace CR -- end

#endif /* STATIONBEAM_H */
  
