/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
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

#ifndef HORIZONSCAN_H
#define HORIZONSCAN_H

// Standard library header files
#include <string>

// include custom header files
#include <Imaging/GeometricalWeight.h>

namespace CR { // NAMESPACE CR -- BEGIN
  
  /*!
    \class HorizonScan
    
    \ingroup CR_Imaging
    
    \brief Wrapper for scanning locations on the horizon as function of frequency
    
    \author Lars B&auml;hren

    \date 2007/01/20

    \test tHorizonScan.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>GeometricalDelay
    </ul>
    
    <h3>Synopsis</h3>

    Besides the more complex imaging tasks -- such as e.g. creating an all-sky
    map or mapping out the contents of a 3-dimensional region -- the scanning
    for the location of RFI sources poses a rather simple task. The task is the
    creation of an imaging mapping the radio intensity distribution as function
    of azimuth angle and frequency, \f$ I = I(\theta,\nu) \f$.
    
    <h3>Example(s)</h3>
    
  */  
  class HorizonScan {

    //! Increment in Azimuth along the the horizon (angular resolution)
    double angularResolution_p;

    //! Geometrical weights applied for the beamforming
    GeometricalWeight weights_p;
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    HorizonScan ();
    
    /*!
      \brief Copy constructor
      
      \param other -- Another HorizonScan object from which to create this new
      one.
    */
    HorizonScan (HorizonScan const &other);
    
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    ~HorizonScan ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another HorizonScan object from which to make a copy.
    */
    HorizonScan& operator= (HorizonScan const &other); 
    
    // --------------------------------------------------------------- Parameters

    /*!
      \brief Get the angular resolution in azimuth along the horion

      \return agularResolution -- The angular resolution in azimuth along the 
                                  horizon; [deg].
    */
    double angularResolution () {
      return angularResolution_p;
    }

    /*!
      \brief Set the angular resolution in azimuth along the horizon

      \todo Not yet implemented!

      \param angularResolution -- The angular resolution in azimuth along the 
                                  horizon; [deg].

      \return status -- Status of the operation; returns <i>false</i> if an
                        error occured.
     */
    bool setAngularResolution (const double &angularResolution);

    /*!
      \brief Get the geometrical weights required for the beamforming

      \return weights -- The geometrical weights required for the beamforming
                         process
    */
    GeometricalWeight weights () {
      return weights_p;
    }

    /*!
      \brief Set the geometrical weights required for the beamforming

      \param weights -- 

      \return status -- 
    */
    bool setWeights (const GeometricalWeight &weights);
    
    // ------------------------------------------------------------------ Methods



    // ----------------------------------------------------------------- Feedback
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, HorizonScan.
    */
    std::string className () const {
      return "HorizonScan";
    }
    
    /*!
      \brief Provide a summary of the class's internal data and status
    */
    void summary ();
    
  private:
    
    /*!
      \brief Unconditional copying
    */
    void copy (HorizonScan const &other);
    
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);
    
  };  // CLASS HORIZONSCAN -- END
  
}  // NAMESPACE CR -- END

#endif /* HORIZONSCAN_H */
  
