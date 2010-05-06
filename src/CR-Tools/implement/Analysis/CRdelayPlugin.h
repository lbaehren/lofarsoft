/***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Andreas Horneffer (<mail>)                                            *
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

/* $Id$*/

#ifndef CRDELAYPLUGIN_H
#define CRDELAYPLUGIN_H

#include <crtools.h>

// Standard library header files
#include <string>

// AIPS++/CASA header files
#include <casa/Exceptions/Error.h>

#include <Calibration/PluginBase.h>

#include <casa/namespace.h>

#define LIGHTSPEED 299792458.

namespace CR { // Namespace CR -- begin
  
  /*!
    \class CRdelayPlugin
    
    \ingroup Analysis
    
    \brief Brief description for class CRdelayPlugin
    
    \author Andreas Horneffer

    \date 2007/04/19

    \test tCRdelayPlugin.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>[start filling in your text here]
    </ul>
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>
    
  */  
  class CRdelayPlugin : public PluginBase<DComplex> {
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    CRdelayPlugin ();
        
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    ~CRdelayPlugin ();
    
    // --------------------------------------------------------------- Parameters
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, CRdelayPlugin.
    */
    std::string className () const {
      return "CRdelayPlugin";
    }

    /*!
      \brief Provide a summary of the internal status
    */
    void summary ();    

    // ------------------------------------------------------------------ Methods
    
    /*!
      \brief Calculate the (complex) weights.

      \param data -- Not used but needed for the plugin interface...
      
      \return ok -- Was operation successful? Returns <tt>True</tt> if yes.
    */
    virtual Bool calcWeights(const Matrix<DComplex> &data);    
    
    
  private:
        
    /*!
      \brief Initialization 
    */
    void init(void);
    
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);

    /*!
      \brief Rotate to shower coordinates

      \param pos - positions to rotate 

      \return rotated positions
    */
    Matrix<Double> toShower(Matrix<Double> & pos);
    
  };
  
} // Namespace CR -- end

#endif /* CRDELAYPLUGIN_H */
  
