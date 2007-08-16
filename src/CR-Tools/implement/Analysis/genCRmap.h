/***************************************************************************
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

/* $Id: genCRmap.h,v 1.2 2007/05/02 09:37:10 bahren Exp $*/

#ifndef GENCRMAP_H
#define GENCRMAP_H

// Standard library header files
#include <string>

// AIPS++/CASA header files
#include <casa/aips.h>
#include <casa/Exceptions/Error.h>

#include <casa/Arrays.h>
// #include <casa/Containers/RecordField.h>

#include <Imaging/Beamformer.h>
#include <Imaging/Skymapper.h>
#include <Calibration/CalTableReader.h>

#include <casa/namespace.h>

namespace CR { // Namespace CR -- begin
  
  /*!
    \class genCRmap
    
    \ingroup Analysis
    
    \brief Brief description for class genCRmap
    
    \author Andreas Horneffer
    
    \date 2007/04/17
    
    \test tgenCRmap.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
    <li>[start filling in your text here]
    </ul>
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>
    
  */  
  class genCRmap {

  protected:
    
    //! SkymapCoordinates object that holds all the Meta-Information
    SkymapCoordinates SkyCoord_p;

    //! Unmodified antenna positions, i.e. relative to coordinate system and not shower core.
    Matrix<Double> AntPositions_p;

    //! Reference position, e.g. position of shower core
    Vector<Double> ReferencePos;
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    genCRmap ();
    
    
    // -------------------------------------------------------------- Destruction
    
    /*!
      \brief Destructor
    */
    ~genCRmap ();
    
    // --------------------------------------------------------------- Parameters
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, genCRmap.
    */
    std::string className () const {
      return "genCRmap";
    }
    
    /*!
      \brief Get the record that stores parameters for this function
      
      \return parameters -- the parameters record
    */ 
    //    inline Record& parameters() { return parameters_p; };
    
    /*!
      \brief Set part of the parameters from a DataReader and CalTableReader
      
      \param dr -- DataReader object. Mainly the header-record is read.
      \param CTRead -- (pointer to) already initialized CalTableReader
      
      \return ok -- Was operation successful? Returns <tt>True</tt> if yes.
    */
    Bool setMapHeader(DataReader &dr, CalTableReader *CTRead);
   
    
    // ------------------------------------------------------------------ Methods
    
    /*!
      \brief Calculate the map
      
      \param filename -- Filename of the map that is to be generated
      \param data --     Array with the input data.
      
      \return ok -- Was operation successful? Returns <tt>True</tt> if yes.
    */
    Bool GenerateMap(const String &filename, const Matrix<DComplex> &data);
    
    
  private:
        
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);

    MEpoch LOPES2Epoch(uInt JDR, int TL=0);
    
  };
  
} // Namespace CR -- end

#endif /* GENCRMAP_H */
  
