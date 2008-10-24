/*-------------------------------------------------------------------------*
 | $Id:: NewClass.h 1964 2008-09-06 17:52:38Z baehren                    $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2008                                                    *
 *   Frank Schroeder (<mail>)                                              *
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

#ifndef PULSEPROPERTIES_H
#define PULSEPROPERTIES_H

// Standard library header files
#include <iostream>
#include <string>

// Library for ROOT Dictionary
#include <Rtypes.h>

// Don't forget to run the following commands in implement/Analysis, if something has changed:
// rm RootDict.*
// rootcint RootDict.C -c PulseProperties.h


// namespace declaration causes problems with root dictionary
// should be solved in ROOT 5.20
//namespace CR { // Namespace CR -- begin
  
  /*!
    \class PulseProperties
    
    \ingroup Analysis
    
    \brief Contains basic pulse parameters like times and heights
    
    \author Frank Schroeder

    \date 2008/10/21

    \test tPulseProperties.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>Root dictionary, created by
      <li>rootcint RootDict.C -c PulseProperties.h
    </ul>
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>
    
  */  
  class PulseProperties {
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    PulseProperties ();
    
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    virtual ~PulseProperties ();
    
    // ---------------------------------------------------------------- Variables
    //! ID of antenna which recorde the pulse
    int antennaID;
    //! number of antenna
    int antenna;
    //! maximum of trace
    double maximum;
    //! maximum of envelope of trace
    double envelopeMaximum;
    //! minimum
    double minimum;
    //! time of maximum
    double maximumTime;
    //! time of maximum of envelope
    double envelopeTime;
    //! time of minimum
    double minimumTime;
    //! time when crossing half height
    double halfheightTime;
    //! FWHM
    double fwhm;

    // --------------------------------------------------------------- Parameters
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, PulseProperties.
    */
    inline std::string className () const {
      return "PulseProperties";
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


    //! Make class available in ROOT dictionary (needed for ROOT I/O)
    ClassDef(PulseProperties,1)
  };
  
//} // Namespace CR -- end

#endif /* PULSEPROPERTIES_H */
  
