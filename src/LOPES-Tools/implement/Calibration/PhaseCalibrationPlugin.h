/***************************************************************************
 *   Copyright (C) 2006                                                    *
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

/* $Id: PhaseCalibrationPlugin.h,v 1.3 2006/11/08 16:06:41 bahren Exp $*/

#ifndef PHASECALIBRATIONPLUGIN_H
#define PHASECALIBRATIONPLUGIN_H

#include <string>

// AIPS++/CASA header files
#include <casa/aips.h>
#include <casa/BasicSL/Complex.h>
#include <casa/Exceptions/Error.h>

#include <Calibration/PluginBase.h>

namespace LOPES { // Namespace LOPES -- begin
  
  /*!
    \class PhaseCalibrationPlugin
    
    \ingroup Calibration
    
    \brief Brief description for class PhaseCalibrationPlugin
    
    \author Kalpana Singh
    
    \date 2006/11/02
    
    \test tPhaseCalibrationPlugin.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
    <li>[start filling in your text here]
    </ul>
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>
    
  */
  class PhaseCalibrationPlugin : public PluginBase<Complex> {
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    PhaseCalibrationPlugin ();
    
    /*!
      \brief Copy constructor
      
      \param other -- Another PhaseCalibrationPlugin object from which to create
                      this new one.
    */
    PhaseCalibrationPlugin (PhaseCalibrationPlugin const &other);
    
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    ~PhaseCalibrationPlugin ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another PhaseCalibrationPlugin object from which to make a copy.
    */
    PhaseCalibrationPlugin& operator= (PhaseCalibrationPlugin const &other); 
    
    // --------------------------------------------------------------- Parameters
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, PhaseCalibrationPlugin.
    */
    std::string className () const {
      return "PhaseCalibrationPlugin";
    }
    
    // ------------------------------------------------------------------ Methods
    
    
    
  private:
    
    /*!
      \brief Unconditional copying
    */
    void copy (PhaseCalibrationPlugin const &other);
    
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);
    
  };
  
} // Namespace LOPES -- end

#endif /* PHASECALIBRATIONPLUGIN_H */
  
