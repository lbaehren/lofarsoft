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

#include <crtools.h>

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
#include <casa/BasicMath/Math.h>

#include <casa/Quanta.h>
#include <casa/Exceptions/Error.h>
#include <tables/Tables/Table.h>
#include <tables/Tables/TableDesc.h>
#include <tables/Tables/SetupNewTab.h>

#include <tables/Tables/ScalarColumn.h>
#include <tables/Tables/ScaColDesc.h>
#include <tables/Tables/ScaRecordColDesc.h>

#include <casa/namespace.h>

#ifdef CR_WITH_PLOTTING
#include <Display/SimplePlot.h>
using CR::SimplePlot;
#endif

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
    
    //! Declination angle of the source
    Double source_declination_p;
    //! Hour angle of the source
    Double source_hr_angle_p;

  public:
    
    // ------------------------------------------------------------- Construction
    
    //! Default constructor
    StationBeam ();
    
    /*!
      \brief Argumented constructor
      
      \param source_declination -- 
      \param source_hr_angle    -- 
      \param sky_temp           -- 
      \param moon_temp          -- 
      \param station_id         -- 
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

    /*!
      \brief Copy constructor
      
      \param other -- Another StationBeam object from which to create this new
             one.
    */
    StationBeam (StationBeam const &other);
    
    // -------------------------------------------------------------- Destruction

    //! Destructor
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

    //! Provide a summary of the internal status
    inline void summary () {
      summary (std::cout);
    }

    /*!
      \brief Provide a summary of the internal parameters
      
      \param os -- Output stream, to which the summary is written
    */
    void summary (std::ostream &os);    
    
    // ------------------------------------------------------------------ Methods
    
    /*!
      \brief Compute the station beam
      
      \param frequency 
      \param hr_angle
      \param declination
      \param source_declination
      \param source_hr_angle
      
      \return power -- 
    */
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
    
    
    Double incoherent_beam( const Double& frequency,
			    const Double& hr_angle,
			    const Double& declination,
			    const Double& source_declination,
			    const Double& source_hr_angle,
			    const Double& station_radii,
			    const Vector<uint>& station_id,
			    const Vector<Double>& legendre_root,
			    const Vector<Double>& legendre_weight ) ;
    
    
    Double integrate_freq( const Double& hr_angle,
			   const Double& declination,
			   const Double& source_declination,
			   const Double& source_hr_angle,
			   const Double& station_radii,
			   const Vector<uint>& station_id,
			   const Double& freq_init,
			   const Double& bandwidth,
			   const Double& freq_interval,
			   Vector<Double>& position_x,
			   Vector<Double>& position_y,
			   const Vector<Double>& legendre_root,
			   const Vector<Double>& legendre_weight );
    
    
    Double incoherent_beaming( const Double& hr_angle,
			       const Double& declination,
			       const Double& source_declination,
			       const Double& source_hr_angle,
			       const Double& station_radii,
			       const Vector<uint>& station_id,
			       const Double& frequency,
			       Vector<Double>& position_x,
			       Vector<Double>& position_y,
			       const Vector<Double>& legendre_root,
			       const Vector<Double>& legendre_weight );
    
    Double integrate_phi( const Double& declination,
			  const Double& source_declination,
			  const Double& source_hr_angle,
			  const Double& station_radii,
			  const Vector<uint>& station_id,
			  const Double& freq_init,
			  const Double& bandwidth,
			  const Double& freq_interval,
			  Vector<Double>& position_x,
			  Vector<Double>& position_y,
			  const Vector<Double>& legendre_root,
			  const Vector<Double>& legendre_weight )   ;
    
    
    Double integrate_decli( const Double& source_declination,
			    const Double& source_hr_angle,
			    const Double& station_radii,
			    const Vector<uint>& station_id,
			    const Double& freq_init,
			    const Double& bandwidth,
			    const Double& freq_interval,
			    Vector<Double>& position_x,
			    Vector<Double>& position_y,
			    const Vector<Double>& legendre_root,
			    const Vector<Double>& legendre_weight )   ;
    
    
    Double beamwidth_decli(  const Double& source_declination,
			     const Double& source_hr_angle,
			     const Double& station_radii,
			     const Vector<uint>& station_id,
			     const Double& freq_init,
			     const Double& bandwidth,
			     const Double& freq_interval,
			     Vector<Double>& position_x,
			     Vector<Double>& position_y,
			     const Vector<Double>& legendre_root,
			     const Vector<Double>& legendre_weight ) ;
    
    
    Double beamwidth_hr(  const Double& source_declination,
			  const Double& source_hr_angle,
			  const Double& station_radii,
			  const Vector<uint>& station_id,
			  const Double& freq_init,
			  const Double& bandwidth,
			  const Double& freq_interval,
			  Vector<Double>& position_x,
			  Vector<Double>& position_y,
			  const Vector<Double>& legendre_root,
			  const Vector<Double>& legendre_weight )  ;
    
    void generate_statistics_table( const Double& station_radii,
				    const Vector<uint>& station_id,
				    const Double& freq_init,
				    const Double& bandwidth,
				    const Double& freq_interval,
				    Vector<Double>& position_x,
				    Vector<Double>& position_y,
				    const Vector<Double>& legendre_root,
				    const Vector<Double>& legendre_weight ) ;
    
    Double power_moon( const Double& source_declination,
		       const Double& source_hr_angle,
		       const Double& station_radii,
		       const Vector<uint>& station_id,
		       const Double& freq_init,
		       const Double& bandwidth,
		       const Double& freq_interval,
		       Vector<Double>& position_x,
		       Vector<Double>& position_y,
		       const Vector<Double>& legendre_root,
		       const Vector<Double>& legendre_weight );
    
    Double min_power_moon( const Double& source_declination,
			   const Double& source_hr_angle,
			   const Double& station_radii,
			   const Vector<uint>& station_id,
			   const Double& freq_init,
			   const Double& bandwidth,
			   const Double& freq_interval,
			   Vector<Double>& position_x,
			   Vector<Double>& position_y,
			   const Vector<Double>& legendre_root,
			   const Vector<Double>& legendre_weight );

#ifdef CR_WITH_PLOTTING
    /*!
      \brief Create 2-dimensional plot
      
      \param source_declination -- Declination of the source under observation
      \param source_hr_angle    -- Hour angle of the source under observation
      \param station_radii      -- Radii of the LOFAR stations involved in the 
             observation.
      \param station_id         -- IDs of the stations involved in the
             observation.
      \param freq_init
    */
    void plot_2dimen(  const Double& source_declination,
		       const Double& source_hr_angle,
		       const Double& station_radii,
		       const Vector<uint>& station_id,
		       const Double& freq_init,
		       const Double& bandwidth,
		       const Double& freq_interval,
		       Vector<Double>& position_x,
		       Vector<Double>& position_y,
		       const Vector<Double>& legendre_root,
		       const Vector<Double>& legendre_weight ) ;
#endif    
    
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
  
