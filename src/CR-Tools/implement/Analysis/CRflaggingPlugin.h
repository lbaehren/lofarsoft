/*-------------------------------------------------------------------------*
 | $Id: CRflaggingPlugin.h,v 1.1 2007/07/31 12:22:26 horneff Exp $ |
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

#ifndef CRFLAGGINGPLUGIN_H
#define CRFLAGGINGPLUGIN_H

#include <crtools.h>

// Standard library header files
#include <iostream>
#include <string>

// AIPS++/CASA header files
#include <casa/Arrays/Matrix.h>
#include <casa/Exceptions/Error.h>

#include <Calibration/PluginBase.h>

#include <casa/namespace.h>


namespace CR { // Namespace CR -- begin
  
  /*!
    \class CRflaggingPlugin
    
    \ingroup Analysis
    
    \brief Brief description for class CRflaggingPlugin
    
    \author Andreas Horneffer

    \date 2007/07/27

    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li> PluginBase
    </ul>
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>
    
  */  
  class CRflaggingPlugin : public PluginBase<Double> {
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    CRflaggingPlugin ();
        
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    ~CRflaggingPlugin ();
    
    // --------------------------------------------------------------- Parameters
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, CRflaggingPlugin.
    */
    std::string className () const {
      return "CRflaggingPlugin";
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
    
     /*!
      \brief Calculate the "weights". As the usefull side effect updates the AntennaMask

      \param data -- The (filtered) time series data
      
      \return ok -- Was operation successful? Returns <tt>True</tt> if yes.
    */
    virtual Bool calcWeights (const Matrix<Double> &data);    
   
    
  private:
    
    /*!
      \brief Initialization 
    */
    void init(void);

    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);
    
  };
  
} // Namespace CR -- end

#endif /* CRFLAGGINGPLUGIN_H */
  
