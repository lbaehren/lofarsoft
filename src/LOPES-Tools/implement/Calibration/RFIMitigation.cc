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

/* $Id: RFIMitigation.cc,v 1.7 2006/11/02 13:39:26 singh Exp $*/

/*!
  \class RFIMitigation
*/

#include <Calibration/RFIMitigation.h>

namespace LOPES {

// ==============================================================================
//
//  Construction
//
// ==============================================================================

// ---------------------------------------------------------------- RFIMitigation
  
RFIMitigation :: RFIMitigation ()
{
  // shouldn't we at least perform internal initialization here??
}   

// ---------------------------------------------------------------- RFIMitigation
  
RFIMitigation :: RFIMitigation( const Matrix<DComplex>& spectra,
				const int& nOfSegments,
		                const int& dataBlockSize )
				
{ 
  Matrix<DComplex> gainValues;
}

// ==============================================================================
//
//  Destruction
//
// ==============================================================================

RFIMitigation :: ~RFIMitigation()
{;} 

// ==============================================================================
//
//  Computation methods
//
// ==============================================================================

// -------------------------------------------------------- getAbsoluteGainValues

Matrix<double> getAbsoluteGainValues (const Matrix<DComplex>& spectra) 
  
{
  Matrix<double> absoluteValueArray( amplitude( spectra )) ;
  
  return absoluteValueArray ;
}

// -------------------------------------------------------- getSegmentationVector

Vector<int> RFIMitigation ::getSegmentationVector( const int& dataBlockSize, 
						   const int& nOfSegments ) 
{
  try {
    int fft_Length = ( dataBlockSize/2 )+1 ;
    Vector<int> segmentation( nOfSegments-1 );    
    int frequencyChannels = int( fft_Length/ nOfSegments ) ;

    // assign the values for the segment index ranges
    for (int ns=0; ns <( nOfSegments -1 ); ns++) {
      segmentation( ns ) = ( ns +1 )*frequencyChannels ;
    }
    
    return segmentation ;
  }
  catch ( AipsError x ) {
    cerr << "RFIMitigation :: getSegmentationVector " << x.getMesg () << endl ;
    return Vector<int> ();
  }
}

// --------------------------------------------------------- getinterpolatedGains

Matrix<double> RFIMitigation::getinterpolatedGains (const Matrix<DComplex>& spectra,
						    const int& dataBlockSize,
						    const int& nOfSegments  ) 
  
{
  try {
    RFIMitigation rf;
    Matrix<double> absoluteArray ( amplitude ( spectra )) ;
    Vector<int> segmentationIndices = rf.getSegmentationVector( dataBlockSize, nOfSegments ) ;
    unsigned int segCount (segmentationIndices.nelements()+1);
    unsigned int nOfRows (absoluteArray.nrow());
    unsigned int nOfColumns (absoluteArray.ncolumn());
    unsigned int ele (1);
    unsigned int segmentInit (0);
    unsigned int segmentFinal (0);
    unsigned int segmentLength (0);
    Matrix<double> filteredMatrix (segCount, nOfColumns) ;
    Vector<double> filteredIndexMedian (segCount);
    Vector<double> filteredDataIndexInit (segCount+1);
    Vector<double> filteredDataIndexFinal (segCount);

    // logical switches for computation of mean values
    bool sorted (false);
    bool takeEvenMean (false);
    bool inPlace (false);
    
    for (unsigned int i=0; i< segCount; i++) {
      
      filteredDataIndexInit(i) = segmentInit ;
      
      if( i == ( segCount-1 )) {
	segmentFinal              = nOfRows-1;
	filteredDataIndexFinal(i) = segmentFinal;
	filteredIndexMedian(i)    = (filteredDataIndexInit(i)+ filteredDataIndexFinal(i))/2;
      }
      else {
	segmentFinal = segmentationIndices( ele-1 ) ;
	filteredDataIndexFinal( i )= segmentFinal;
	filteredIndexMedian( i )=( filteredDataIndexInit( i )+ filteredDataIndexFinal( i ))/2;
      } ;
      
      filteredDataIndexInit( i+1 )= filteredDataIndexFinal( i )+1;
      
      segmentLength = ( segmentFinal - segmentInit + 1);
      
      Matrix<double> segmentMatrix( absoluteArray(Slice(segmentInit, segmentLength),
						  Slice(0, nOfColumns) ) );
      
      // logical switches for computation of mean values
      sorted       = false;
      takeEvenMean = false;
      inPlace      = false;
      
      for ( unsigned int j = 0; j < nOfColumns; j++) {
	Vector<double> segmentVector = segmentMatrix.column(j);
	filteredMatrix (i,j) = median( segmentVector,sorted,takeEvenMean,inPlace );
      } 
      segmentInit = segmentFinal+1;
      ele = ele+1;
    }
    
    unsigned int nOfRowsFilteredGains = filteredMatrix.nrow() ;
    
    Vector<double> xin( nOfRowsFilteredGains );
    Vector<double> xout( nOfRows );
    Vector<double> yout( nOfRows );
    
    for( unsigned int k = 0; k< nOfRows; k++){
      xout( k )= k*1.0000;
    }
    
    Matrix<double> interpolatedGains( nOfRows, nOfColumns, 0.0);
    
    for( unsigned int p =0; p< segCount; p++){
      xin( p )= filteredIndexMedian( p );
    }
    
    for( unsigned int column = 0; column< nOfColumns ; column++ ){
      Vector<double> yin = filteredMatrix.column( column );
      InterpolateArray1D<double,double>::interpolate(yout,
						     xout,
						     xin,
						     yin,
						     InterpolateArray1D<double,double>::linear);
      
      interpolatedGains.column( column ) = yout;
    }
    
    return interpolatedGains ; 
  }
  catch ( AipsError x ) {
    cerr << "RFIMitigation :: getFilteredGainValues " << x.getMesg () << endl ;
    return Matrix<double> ();
  }
  
}

// --------------------------------------------------------- getdifferenceSpectra

Matrix<double> RFIMitigation :: getdifferenceSpectra (const Matrix<DComplex>& spectra,
 						      const int& dataBlockSize,
				                      const int& nOfSegments )
{
  
  try {
  
  RFIMitigation rf;
  
  Matrix<double> gainValues( amplitude( spectra ) );
  // no. of rows of the segmentation matrix
  unsigned int rows_segArray = gainValues.nrow();
  
  // no. of columns of the segmentation matrix
  unsigned int columns_segArray = gainValues.ncolumn();		
  
  Vector<int> segmentationIndices = rf.getSegmentationVector (dataBlockSize,
							      nOfSegments);
  
  Matrix<double> derivedSpectra = rf. getinterpolatedGains (spectra, 
							    dataBlockSize, 
							    nOfSegments);
  
  // no. of rows of the interpolated segmented matrix
  unsigned int rows_derivedSpectra = derivedSpectra.nrow();
  // no. of columns of the interpolated segmented matrix
  unsigned int columns_derivedSpectra = derivedSpectra.ncolumn();   
  // counter for columns
  unsigned int column (0);
  // counter for rows
  unsigned int row (0);

  Matrix<double> spectra ( rows_derivedSpectra, columns_derivedSpectra, 0.0);
  
  if ( (rows_segArray == rows_derivedSpectra) &&
       (columns_segArray == columns_derivedSpectra) ) {
    
    for(column=0; column< columns_segArray; column++) {
      segmentInit = segmentFinal;
      for(row=0; row < rows_segArray; row++) {
	spectra( row, column) = gainValues( row, column )-derivedSpectra( row, column);
      }
    }
  } else {
    cerr << "[RFIMitigation::differenceSpectra ]"
	 << " Arguments are of incompatible dimensions. "
	 << endl;
  }
  
  return spectra ;
  }
  
  catch ( AipsError x ) {
    cerr << "RFIMitigation :: getdifferenceSpectra " << x.getMesg () << endl ;
    return Matrix<double> ();
  }
  
}

// ---------------------------------------------------------- getnormalizeSpectra

Matrix<double> RFIMitigation::getnormalizeSpectra (const Matrix<DComplex>& spectra,
						   const int& dataBlockSize,
						   const int& nOfSegments)  
{
 
  try {
    RFIMitigation rf;
    Matrix<double> gainValues( amplitude( spectra ) );
    
    // no. of rows of the segmentation matrix
    unsigned int rows_segArray = gainValues.nrow();
    // no. of columns of the segmentation matrix
    unsigned int columns_segArray = gainValues.ncolumn();		
    // Array indices for frequency band segmentation
    Vector<int> segmentationIndices = rf.getSegmentationVector (dataBlockSize,
								nOfSegments);
    
    Matrix<double> derivedSpectra = rf. getinterpolatedGains (spectra,
							      dataBlockSize, 
							      nOfSegments);
    
    // no. of rows of the interpolated segmented matrix
    unsigned int rows_derivedSpectra = derivedSpectra.nrow();
    // no. of columns of the interpolated segmented matrix
    unsigned int columns_derivedSpectra = derivedSpectra.ncolumn();   
    
    int wasDivError (0);
    int Counter (0);											  
    //define a matrix to store the normalize spectra
    Matrix<double> normalizeSpectra( rows_segArray, columns_segArray, 0.0); 
    
    if( (rows_segArray == rows_derivedSpectra) && 
	(columns_segArray == columns_derivedSpectra) ){
      
      for (unsigned int segcolumn = 0; segcolumn< columns_segArray; segcolumn++ ){
	
	for (unsigned int segrow = 0; segrow< rows_segArray; segrow++ ){
	  
	  //division by zero error flagging
	  
	  if ( derivedSpectra( segrow,segcolumn )== 0 )
	    
	    wasDivError++;
	  
	  if( wasDivError ){
	    
	    Counter++;
	    
	    normalizeSpectra( segrow, segcolumn) = 0.0 ;
	    
	    cerr << "[Error: normalizeSpectra ]"
		 << "Division by zero."
		 << Counter
		 << "errors out of"
		 << gainValues.nelements()
		 << "elements."
		 << endl;
	  }
	  else {
	    normalizeSpectra (segrow,segcolumn) = gainValues(segrow,segcolumn)/derivedSpectra( segrow, segcolumn) ;
	  }
	  /*
	    It must be set to zero otherwise in every iteration will be satisfied
	    and else condition, i.e., division will never be incorporated "counter"
	    will count no. of times you have done division by zero.
	  */
	  wasDivError = 0;
	} // end of the for loop for rows
      } // end of the for loop for columns
      
    } // end of the outer if loop
    
    else {
      cerr << "[Error: normalizeSpectra ]"
	   << "Arguments are incompatible dimensions"
	   << endl;
    }
    
    return normalizeSpectra ;
  }
  
  catch ( AipsError x ) {
    cerr << "RFIMitigation :: getNormalizeSpectra " << x.getMesg () << endl ;
    return Matrix<double> ();
  }
} // end of the function getNormalizeSpectra

// --------------------------------------------------------- getRFIRemovedSpectra
 
Matrix<double> RFIMitigation::getRFIRemovedSpectra (const Matrix<DComplex>& spectra,
						    const int& dataBlockSize,
						    const int& nOfSegments)
{
 try {
   RFIMitigation rf;
   Matrix<double> gainValueSpectra = rf.getnormalizeSpectra (spectra,
							     dataBlockSize,
							     nOfSegments);
   unsigned int nOfRows = gainValueSpectra.nrow();
   unsigned int nOfColumns = gainValueSpectra.ncolumn();
   Matrix<double> spectra( nOfRows, nOfColumns, 1.0 );
   Vector<int> segmentationIndices = rf.getSegmentationVector (dataBlockSize, 
							       nOfSegments);

   // variables for book-keeping
   unsigned int segCount (0);
   unsigned int spectraVecElements (0);
   int ele (1);
   int segmentInit (0);
   int segmentFinal (0);
   int segmentLength (1);
   double stdDeviationFinal (1e35);
   double stdDeviationValue (0.0);
   
   for( unsigned int i =0; i< nOfColumns ; i++ ) {

     // internal book-keeping
     segCount = segmentationIndices.nelements()+1 ;	 
     ele = 1;
     segmentInit = 0;
     stdDeviationFinal = 1e35;
     stdDeviationValue = 0.0;
     
    Vector<double> spectraColumn = gainValueSpectra.column( i );
    
    for(unsigned int j = 0; j<segCount; j++) {
      if( j == ( segCount -1 )) {
	segmentFinal = nOfRows-1 ;
      }
      else {
	segmentFinal = segmentationIndices ( ele -1);
      }
      
      segmentLength = (segmentFinal - segmentInit + 1) ;
      
      Vector<double> segmentedVector( spectraColumn ( Slice( segmentInit, segmentLength ) ) );
      
      stdDeviationValue = stddev( segmentedVector ) ;
      
      if ( stdDeviationValue < stdDeviationFinal ) {
	stdDeviationFinal = stdDeviationValue ; 
      }
      
      segmentInit = segmentFinal + 1;
      
      ele = ele + 1 ;
    }  
    
    spectraVecElements = spectraColumn.nelements() ;
    
    Vector<double> rfirejectedspectra( spectraVecElements, 1.0 );
    
    for( unsigned int k = 0; k < spectraVecElements ; k++ ) {    
      
      if(  spectraColumn( k ) > 5*stdDeviationFinal ) {
	
	rfirejectedspectra( k ) = stdDeviationFinal/ spectraColumn( k ) ;
	
      }
    }
    
    spectra.column(i) = rfirejectedspectra ;
    
  } 
  
   return spectra ;
 }
 
 catch ( AipsError x ) {
   cerr << "RFIMitigation :: retraceOriginalSpectra " << x.getMesg () << endl ;
   return Matrix<double> ();
 }
 
}

} // Namespace LOPES -- END
