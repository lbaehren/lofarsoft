/***************************************************************************
 *   Copyright (C) 2005 by Lars B"ahren                                    *
 *   bahren@astron.nl                                                      *
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

#ifndef ANTENNAGAIN_H
#define ANTENNAGAIN_H

// AIPS++/CASA header files
#include <casa/aips.h>
#include <casa/iostream.h>
#include <casa/Arrays.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Matrix.h>
#include <tables/Tables/TableDesc.h>
#include <tables/Tables/SetupNewTab.h>
#include <tables/Tables/Table.h>
#include <tables/Tables/ScaColDesc.h>
#include <tables/Tables/ScaRecordColDesc.h>
#include <tables/Tables/ArrColDesc.h>
#include <tables/Tables/ScalarColumn.h>
#include <tables/Tables/ArrayColumn.h>
#include <tables/Tables/StandardStMan.h>
#include <tables/Tables/IncrementalStMan.h>

// LOPES-Tools header files
#include <Math/MathAlgebra.h>

using std::ostringstream;
using std::cout;
using std::cerr;
using std::endl;

using casa::IPosition;
using casa::Matrix;
using casa::TableDesc;
using casa::Vector;

namespace LOPES {
  
  /*!
    \class AntennaGain
    
    \ingroup Calibration
    
    \brief Extraction and book-keeping of antenna gain tables.
    
    \author Lars B&auml;hren
    
    \date 2005/06/10
    
    \test tAntennaGain.cc
    
    <h3>Prerequisite(s)</h3>
    
    <ul type="square">
    <li>[AIPS++] MathArray -- Mathematical operations for Arrays.
    </ul>
    
    <h3>Synopsis</h3>
    
    <h3>Examples</h3>
    
    Slicing
    \verbatim
    IPosition blc (x1,y1);
    IPosition trc (x2,y2);
    
    Matrix<double> subarray = array (blc,trc);
    
    for (int ant(0); ...) {
    for (int freq(0); ...) {
    minvalues(freq,ant) = min(array (blc,trc));
    }
    }
    \endverbatim
  */
  
  class AntennaGain {
    
    /*!
      Number of antennas.
    */
    int nofAntennas_p;
    
    /*!
      Number of frequency bands for which the antenna gains are determined
    */
    int nofBands_p;
    
    /*!
      Indices for splitting the spectrum of N channels intp a set of M subbands.
    */ 
    Vector<int> bandIndices_p;
    
    //! The antenna gain curves for a given number of frequency bands
    Matrix<double> gainCurves_p;
    
  public:
    
    // --- Construction -----------------------------------------------
    
    /*!
      \brief Construtor
      
      \param nofAntennas -- Number of antennas.
      \param nofBands    -- Number of frequency band, for which the gain values
      are determined
    */
    AntennaGain (const int& nofAntennas,
		 const int& nofBands);
    
    /*!
      \brief Constructor
      
      \param gainCurves -- Antenna gain curves
    */
    AntennaGain (const Matrix<double>& gainCurves);
    
    /*!
      \brief Constructor
      
      \param spectra  -- Antenna spectra.
      \param nofBands -- Number of frequency band, for which the gain values
      are determined
    */
    AntennaGain (const Matrix<double>& spectra,
		 const int& nofBands);
    
    // --- Destruction ------------------------------------------------
    
    ~AntennaGain ();
    
    // --- Access to the antenna gain curves --------------------------
    
    /*!
      \brief Get the antenna gain-curves
     */
    Matrix<double> gainCurves (const Matrix<double>& gainCurves);
    
    /*!
      \brief Set the antenna gain-curves
     */
    void setGainCurves (const Matrix<double>& gainCurves);
    
    /*!
      \brief Set the antenna gain-curves
     */
    void setGainCurves (const Matrix<double>& spectra,
			const int& nofBands);
    
    // --- Calibration ------------------------------------------------
    
    /*!
      \brief Apply calibration to a set of spectra
    */
    void calibrate (Matrix<double>& spectra);
    
    /*!
      \brief Apply gain calibration to a spectrum
      
      \param spectrum -- Spectrum, to which the calibration is applied.
      \param antenna  -- Antenna to which the provided spectrum belongs.
    */
    void calibrate (Vector<double>& spectrum,
		    const int& antenna);
    
    // --- Export of the calibration data ------------------------------
    
    /*!
      \brief Export the antenna gain table to an AIPS++ table
      
      \param tableName -- Name of the written table.
      
      \return ok -- Was writing successful?
      
      \todo Finish implementation! Use code from ccData2Gaincurve.cc
    */
    bool toTable (const String& tableName);
    
  private:
    
    /*!
      \brief Set up the array indices for rebinning of the frequency axis
      
      \param spectra  -- Antenna spectra, [freq,ant]
      \param nofBands -- Number of frequency bands.
    */
    void setBandIndices (const Matrix<double>& spectra,
			 const int& nofBands);
    
  };
  
}  // Namespace LOPES -- END

#endif
