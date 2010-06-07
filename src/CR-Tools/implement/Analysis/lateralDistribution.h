/*-------------------------------------------------------------------------*
 | $Id:: lateralDistribution.h 4151 2010-02-10 17:23:31Z schroeder        $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2010                                                    *
 *   Frank Schroeder (<mail>), Nunzia Palmieri                             *
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

#ifndef LATERALDISTRIBUTION_H
#define LATERALDISTRIBUTION_H

#include <crtools.h>

// Standard library header files
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <algorithm>

#ifdef HAVE_STARTOOLS

#include <Analysis/analyseLOPESevent.h>
#include <Analysis/CompletePipeline.h>

namespace CR { // Namespace CR -- begin

  /*!
    \class lateralDistribution

    \ingroup Analysis

    \brief Brief description for class lateralDistribution

    \author Frank Schroeder

    \date 2008/01/30

    \test tlateralDistribution.cc

    <h3>Prerequisite</h3>
    
    ROOT

    <h3>Synopsis</h3>

    routines for fitting the lateral distribution of LOPES data and REAS simulations

    <h3>Example(s)</h3>

  */
  class lateralDistribution {

  protected:

    //! SNR cut for lateral distribution
    double lateralSNRcut;
    //! Time window cut for lateral distribution (pulse time must be close to cc-beam center)
    double lateralTimeCut;
    
  public:

    // ------------------------------------------------------------- Construction

    /*!
      \brief Default constructor
    */
    lateralDistribution ();

    /*!
      \brief Copy constructor

      \param other -- Another lateralDistribution object from which to create
             this new one.
    */
    lateralDistribution (lateralDistribution const &other);

    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    ~lateralDistribution ();

    // ---------------------------------------------------------------- Operators

    /*!
      \brief Overloading of the copy operator

      \param other -- Another lateralDistribution object from which to make a copy.
    */
    lateralDistribution& operator= (lateralDistribution const &other); 

    // --------------------------------------------------------------- Parameters

    /*!
      \brief Get the name of the class

      \return className -- The name of the class, lateralDistribution.
    */
    std::string className () const {
      return "lateralDistribution";
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

    /*!
      \brief Get the SNR cut for the lateral distribution

      \return SNRcut -- SNR cut
    */
    inline double getLateralSNRcut () {
       return lateralSNRcut;
    }

    /*!
      \brief Set the SNR cut for the lateral distribution

      \param  SNRcut -- SNR cut
    */
    inline void setLateralSNRcut (double const &SNRcut) {
      lateralSNRcut = SNRcut;
    }

    /*!
      \brief Get the timing cut for the lateral distribution

      \return timeCut -- timing cut
    */
    inline double getLateralTimeCut () {
       return lateralTimeCut;
    }

    /*!
      \brief Set the timing cut for the lateral distribution

      \param  timeCut -- timing cut
    */
    inline void setLateralTimeCut (double const &timeCut) {
      lateralTimeCut = timeCut;
    }


    // ------------------------------------------------------------------ Methods

 
    /*!
      \brief Plots lateral distribution of pulse arrival times

      \param filePrefix - the filename will be prefix+GT+".dat".
      \param pulsesRec  - map with information about pulses from LOPES data
      \param pulsesSim  - map with information about pulses from simulated data (leave it empty if there is no simulation)
      \param Gt         - gt from the pipeline root file
      \param az         - azimuth from the pipeline root file
      \param ze         - zenith from the pipeline root file
   */

  Record fitLateralDistribution (const string filePrefix,
                                 map <int, PulseProperties> pulsesRec,
                                 map <int, PulseProperties> pulsesSim,
                                 int Gt, double az, double ze);
                                     
   /*!
      \brief Plots lateral distribution of pulse arrival times

      \param filePrefix - the filename will be prefix+GT+".dat".
      \param pulses     - map with information about pulses
      \param erg        - results of the pipeline (results from the fit will be added)
   */
   void lateralTimeDistribution (const string& filePrefix,
                                 map <int, PulseProperties>& pulses,
                                 Record& erg);
                                   
  };
} // Namespace CR -- end

#endif /* HAVE_STARTOOLS */

#endif /* LATERALDISTRIBUTION_H */
  
