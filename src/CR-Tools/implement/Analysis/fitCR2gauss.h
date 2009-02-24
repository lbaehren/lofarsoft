/*-------------------------------------------------------------------------*
 | $Id$ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                  *
 *   Andreas Horneffer (<mail>)                                                     *
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

#ifndef FITCR2GAUSS_H
#define FITCR2GAUSS_H

#include <crtools.h>

// Standard library header files
#include <iostream>
#include <string>

// AIPS++/CASA header files
#include <casa/aips.h>
#include <casa/Arrays.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Containers/Record.h>
#include <casa/Exceptions/Error.h>

#include <casa/namespace.h>

namespace CR { // Namespace CR -- begin
  
  /*!
    \class fitCR2gauss
    
    \ingroup Analysis
    
    \brief guipipeline style fitting of a gaussian to the CR pulse
    
    \author Andreas Horneffer

    \date 2007/06/18

    \test tfitCR2gauss.cc
        
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>
    
  */  
  class fitCR2gauss {

    // \brief time range that is outside the pulse ([startsample, stopsample])
    Vector<uInt> remoteRange_p;

    // \brief time range in which the fit is performed ([startsample, stopsample])
    Vector<uInt> fitRange_p;

    //! standard time axis values
    Vector<Double> timeAxis_p;

    //! start center for the fit
    Double defCenter_p;

    //! start width for the fit
    Double defWidth_p;
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    fitCR2gauss ();
        
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    ~fitCR2gauss ();
    
    
    // --------------------------------------------------------------- Stuff
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, fitCR2gauss.
    */
    std::string className () const {
      return "fitCR2gauss";
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

    // --------------------------------------------------------------- Parameters

    /*!
      \brief Set the initial center
    */
    inline void setCenter(Double newcenter) { defCenter_p = newcenter; };


    /*!
      \brief Set the time axis values

      \param newTimeAxis - Vector with the time axis values in seconds
      
      \return ok  -- Was operation successful? Returns <tt>True</tt> if yes.
    */
    Bool setTimeAxis(Vector<Double> newTimeAxis);

    /*!
      \brief Set the fit range

      \param newrange - New time range as [startsample, stopsample]
      
      \return ok  -- Was operation successful? Returns <tt>True</tt> if yes.
    */
    Bool setFitRange(Vector<uInt> newrange);

    /*!
      \brief Set the fit range in seconds

      \param starttime - time in seconds at which the range starts
      \param stoptime - time in seconds at which the range stops
      
      \return ok  -- Was operation successful? Returns <tt>True</tt> if yes.
    */
    Bool setFitRangeSeconds (Double starttime,
			     Double stoptime);

    /*!
      \brief Set the remote range

      \param newrange - New time range as [startsample, stopsample]
      
      \return ok  -- Was operation successful? Returns <tt>True</tt> if yes.
    */
    Bool setRemoteRange(Vector<uInt> newrange);

    /*!
      \brief Set the remote range in seconds

      \param starttime - time in seconds at which the range starts
      \param stoptime - time in seconds at which the range stops
      
      \return ok  -- Was operation successful? Returns <tt>True</tt> if yes.
    */
    Bool setRemoteRangeSeconds (Double starttime,
				Double stoptime);

    // ------------------------------------------------------------------ Methods
    
    /*!
      \brief Fit a gaussian first to the x-beam then to the cc-beam

      \param xBeam - time-series data tof the x-beam (same length as timeaxis!)
      \param ccBeam - time-series data tof the cc-beam (same length as timeaxis or empty Vector!)
      \param EvalGauss=False - add a vector with the resulting gaussian to the record.
      \param center=-1e37 - start value for the center, instead of the default.
      
      If the <tt>ccBeam</tt> parameter is an empry vector, then only a fit to the X-beam is done.

      \return Record with the fit results
    */
    Record Fitgauss (Vector<Double> xBeam,
		     Vector<Double> ccBeam,
		     Bool EvalGauss=False,
		     Double center=2e99);
    
  private:
        
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);
    
  };
  
} // Namespace CR -- end

#endif /* FITCR2GAUSS_H */
  
