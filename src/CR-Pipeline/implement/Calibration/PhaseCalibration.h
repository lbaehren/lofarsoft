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

/* $Id: PhaseCalibration.h,v 1.12 2007/03/26 13:15:30 bahren Exp $*/

#ifndef PHASECALIBRATION_H
#define PHASECALIBRATION_H

// AIPS++ header files
#include <casa/aips.h>
#include <casa/iostream.h>
#include <casa/fstream.h>
#include <casa/string.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Vector.h>

#include <casa/BasicSL/Complex.h>
#include <casa/Quanta.h>
#include <scimath/Functionals/Interpolate1D.h>
#include <scimath/Functionals/ScalarSampledFunctional.h>
#include <tables/Tables/Table.h>
#include <tables/Tables/TableDesc.h>
#include <tables/Tables/SetupNewTab.h>
#include <tables/Tables/ScalarColumn.h>
#include <tables/Tables/ScaColDesc.h>
#include <tables/Tables/ScaRecordColDesc.h>
#include <tables/Tables/ArrColDesc.h>
#include <tables/Tables/ArrayColumn.h>

// LOPES-Tools header files
#include <Math/Math.h>
#include <Functionals/StatisticsFilter.h>
#include <scimath/Mathematics/InterpolateArray1D.h>

#include <casa/namespace.h>


/*!
  \class PhaseCalibration

  \ingroup Calibration

  \brief Brief description for class PhaseCalibration

  \author Kalpana Singh

  \date 2006/07/03

  \test tPhaseCalibration.cc

  <h3>Prerequisite</h3>

  <ul type="square">
    <li>[start filling in your text here]
  </ul>

  <h3>Synopsis</h3>
  
  By monitoring the relative phases of a TV transmitter the phase stability of the system can be 
  monitored and one can get the time delay calibration values for every day. As the position of the 
  TV transmitter does not change, the relative delayes and thus the relative phases of its signal in 
  the different antennas remains constant.
  
  The signal from the TV transmitter does not have enough fine structure to allow us to directly get 
  the relative delay via the cross-correlation. Checking the relative phases of just a single frequency 
  cannot detect larger shifts due to the ambigGreisenousness of the phase. But by checking various frequencies
  this ambiguity can be reduced so far that shifts of an integer number of samples can be detected.
  
  The method uses the frequency domain data, i.e., Fourier transformed time domain data, and delays 
  are measured relative to a reference antenna.
  
  <h3>Example(s)</h3>
  
  <h3>Terminology</h3>
  
  In the PhaseCalibration class, we employ specific terminology. To clarify subtle 
  differences, the important terms (and their derivatives) are explained below in 
  logical groups.
  
  
  <h3> Motivation</h3>
  This class was motivated by need :
  
  <ol>
  <li> We wish to do phase calibration of our antennas with respect to TV transmitter, 
  which is at fixed position and hence will provide a constant relative delays and thus 
  relative phases of signal from TV transmitter in the different antennas remains constant.

*/

class PhaseCalibration {
 
  private : 
   
   enum antBoolVector{ F, T } ; 
 
//  private:
// 
//   /*!
//     \brief Unconditional copying
//   */
//   void copy (PhaseCalibration const &other);
// 
//   /*!
//     \brief Unconditional deletion 
//   */
//   void destroy(void);

public :

 // --------------------------------------------------------------- Construction

  /*!
    \brief Default constructor
  */
  
  PhaseCalibration ();

  /*!
    \brief argumented constructor
    
      \param spectra		--	Sets the data array of the gain curve to the 2-dimensional complex
    					array in which number of rows indicates the number of frequency channels
					however, number of columns indicate the number of antennas to be scanned.
    
    \param frequencyRanges	--	Minimum and maximum value of frequencies to define frequency ranges
    					in which power peaks of the signal lies (we set these frequency ranges 
			         	greater than three, i.e., picture and audio subcarriers of TV transmission)

    \param expectedPhases	--	Expected Phase delays for various antennas due to the fact that different
    					antennas are pointing towards different directions and due to the time delay 
					of electronic circuit as well.
					
    \param frequencyValues	--	Vector gives the exact frequency values correspoding to the frequency indices
    					(or row index) of the given data array (i.e., spectra or FFT Pattern) of 
					complex values.	
	
    \param sampleJumps	   	--	multisamle jumps to correct time delays because of the electronic circuits.
    
    \param referenceAntenna	--	Phase calibration is done with respect to this particular antenna
    
    \param sampleRate		--	rate with which sampling of the data is performed.
    
    \param badnessWeight	--	Wheight for the two components of the "badness". (Value between 0 and 1, default 0.5)

    \param badnessThreshold	--	Maximum acceptable "badness" before antenna is flagged. (default 0.15 sample times)

   	
				
  */
 
  PhaseCalibration ( const Matrix<DComplex>& spectra,
  		     const Matrix<Double>& frequencyRanges,
		     const Matrix<Double>& expectedPhases,
		     const Matrix<Double>& phaseGrad,
		     const Vector<Double>& frequencyValues,
		     const Vector<Double>& sampleJumps,
		     const Int& referenceAntenna,
		     const Double& sampleRate,
		     const Double& badnessWeight,
		     const Double& badnessThreshold ) ;
		     

  // ---------------------------------------------------------------- Destruction

  /*!\param frequencyRanges	--	Minimum and maximum value of frequencies to define frequency ranges
    					in which power peaks of the signal lies (we set these frequency ranges 
			         	greater than three, i.e., picture and audio subcarriers of TV transmission)
    \brief Destructor
  */
   ~PhaseCalibration ();
  
  //---------------------------------------------Computation methods---------------
  
    
  
/*!
  
  \ brief get the frequency indices of the input frequency values vector corresponding to the frequency 
    ranges for which minimum and maximum ranges are given in a form of matrix for various peaks of the signal.
  
  \param frequencyRanges	--	Minimum and maximum value of frequencies to define frequency ranges
    					in which power peaks of the signal lies (we set these frequency ranges 
			         	greater than three, i.e., picture and audio subcarriers of TV transmission)

  \param frequencyValues	--	Vector gives the exact frequency values correspoding to the frequency indices
    					(or row index) of the given data array (i.e., spectra or FFT Pattern) of 
					complex values.	
				
 */
  
  Vector<Int> getFrequencyIndices( const Matrix<Double>& frequencyRanges,
  			       	   const Vector<Double>& frequencyValues );
			       			

 /*!
  
  \ brief get the indices of the input matrix (array) at which peaks of the amplitude have been idenfied.
  
     \param spectra		--	Sets the data array of the gain curve to the 2-dimensional complex
    					array in which number of rows indicates the number of frequency channels
					however, number of columns indicate the number of antennas to be scanned.

    \param frequencyIndex	--	frequency indices are the index values in the data array spectra at which the 
    					power of the input signal is scanned maximum.
					
   \param frequencyValues	--	Vector gives the exact frequency values correspoding to the frequency indices
    					(or row index) of the given data array (i.e., spectra or FFT Pattern) of 
					complex values.	
				
 */
  
  Matrix<Int> getIndexOfPowerPeaks( const Matrix<DComplex>& spectra,
  			       	    const Vector<Int>& frequencyIndex,
				    const Vector<Double>& frequencyValues );
				      
 /*!
  
  \ brief get the exact frequency values for which the amplitude of the signal (input array) is found maximum.
  
  
  \param powerIndex		--	power index is the index values of the input array at which 
  					power of the signal is found maximum for each and every antenna.
					
  \param frequencyValues	--	Vector gives the exact frequency values correspoding to the frequency indices
    					(or row index) of the given data array (i.e., spectra or FFT Pattern) of 
					complex values.
				
 */
  
  Vector<Double> getPeakFrequencyValues( const Matrix<Int>& powerIndex,
  			       	        const Vector<Double>& frequencyValues );

					
 /*!
  
  \ brief get the phase values of the input complex  data array at which amplitude or power of the input 
    signal is scanned as maximum for each antenna.
  
  \param spectra		--	Sets the data array of the gain curve to the 2-dimensional complex
    					array in which number of rows indicates the number of frequency channels
					however, number of columns indicate the number of antennas to be scanned.
					
  \param powerIndex		--	power index is the indexvalues of the input array at which 
  					power of the signal if found maximum for each and every antenna.
				
 */
  
  Matrix<Double> getPhaseValues( const Matrix<DComplex>& spectra,
  			        const Matrix<Int>& powerIndex );
				
				
/*!

  \ brief get the residual phases for each and every antenna with respect to the chosen reference antenna, i.e.,
    relative phases are calculated first with respect to the selected antenna and then residualphases are
    by subtracting expectedPhases from relativePhases.
  
  \param calculatedPhases 	--	Matrix of the phase values calculated for the imput complex array
  					for the indices at which power of the signal is scanned maximum.

  \param expectedPhases		--	Expected Phase delays for various antennas due to the fact that different
    					antennas are pointing towards different directions and due to the time delay 
					of electronic circuit as well.

  \param referenceAntenna	--	reference Antenna has to be selected by the user, this has to be one of the
  					column number of the input data array.
  
  */
  
  Matrix<Double> getPhaseDiffInDegree( const Matrix<Double>& calculatedPhases,
  			       	       const Matrix<Double>& expectedPhases,
				       const Matrix<Double>& phaseGrad,
				       const Int& referenceAntenna );
				   
 

  
  /*!
  
  \ brief get the relative delays for various frequencies with respect to reference antenna at which 
    power peaks have been idenfied.
  
  \param residualDegree 	--	residual degree is the matrix form of the residual phases expressed in degree.
  
  \param sampleRate		--	rate with which sampling of the data is performed.
  
  \param powerIndex		--	power index is the indexvalues of the input array at which 
  					power of the signal if found maximum for each and every antenna.
	
  \param frequencyValues	--	Vector gives the exact frequency values correspoding to the frequency indices
    					(or row index) of the given data array (i.e., spectra or FFT Pattern) of 
					complex values.
				
 */
  
  Matrix<Double> getRelativeDelay( const Matrix<Double>& phaseDiff,
  			       	  const Double& sampleRate,
				  const Matrix<Int>& powerPeakIndices,
				  const Vector<Double>& frequencyValues );
				      
  

   /*!
  
  \  brief get the boolean vector with number of element equal to the number of antenna selected in the input
     data array, if the antenna is flagged then it is flagged as false antenna.
  
   \param spectra		--	Sets the data array of the gain curve to the 2-dimensional complex
    					array in which number of rows indicates the number of frequency channels
					however, number of columns indicate the number of antennas to be scanned.
    
    \param frequencyRanges	--	Minimum and maximum value of frequencies to define frequency ranges
    					in which power peaks of the signal lies (we set these frequency ranges 
			         	greater than three, i.e., picture and audio subcarriers of TV transmission)

    \param expectedPhases	--	Expected Phase delays for various antennas due to the fact that different
    					antennas are pointing towards different directions and due to the time delay 
					of electronic circuit as well.
					
    \param frequencyValues	--	Vector gives the exact frequency values correspoding to the frequency indices
    					(or row index) of the given data array (i.e., spectra or FFT Pattern) of 
					complex values.	
	
    \param sampleJumps	   	--	multisamle jumps to correct time delays because of the electronic circuits.
    
    \param referenceAntenna	--	Phase calibration is done with respect to this particular antenna
    
    \param sampleRate		--	rate with which sampling of the data is performed.
    
    \param badnessWeight	--	Wheight for the two components of the "badness". (Value between 0 and 1, default 0.5)

    \param badnessThreshold	--	Maximum acceptable "badness" before antenna is flagged. (default 0.15 sample times)

    \param newCorrDelays	--	Space in which the correction delays can be returned. (In sample times!)
  
 */
  
  Vector<Bool> getAntennaReturn(  Matrix<DComplex> const &spectra,
				  Matrix<Double> const &frequencyRanges,
				  Matrix<Double> const &expectedPhases,
				  Matrix<Double> const &phaseGrad,
				  Vector<Double> const &frequencyValues,
				  Vector<Double> const &sampleJumps,
				  Int const &referenceAntenna,
				  Double const &sampleRate,
				  Double const &badnessWeight = 0.5,
				  Double const &badnessThreshold =0.15,
				  Vector<Double> *newCorrDelays=NULL);
  			  
  
  
  // ------------------------------------------------------------------ Operators

  /*!
    \brief Overloading of the copy operator

    \param other -- Another PhaseCalibration object from which to make a copy.
  */
  PhaseCalibration& operator= (PhaseCalibration const &other); 
  
  void init(const Matrix<DComplex>& spectra ) ;

  // ----------------------------------------------------------------- Parameters

  

  // -------------------------------------------------------------------- Methods

};

#endif /* PHASECALIBRATION_H */
