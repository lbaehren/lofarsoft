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

/* $Id: RFIMitigation.cc,v 1.9 2007/03/07 14:36:52 singh Exp $*/

#include <Calibration/RFIMitigation.h>

using casa::AipsError;

namespace CR {  // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  RFIMitigation :: RFIMitigation ()
  {;}   
  
  RFIMitigation :: RFIMitigation( const Matrix<DComplex>& spectra,
				  const uint& nOfSegments,
				  const uint& dataBlockSize )
    
  { 
    Matrix<DComplex> gainValues;
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  RFIMitigation :: ~RFIMitigation()
  {;} 
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  // ------------------------------------------------------ getAbsoluteGainValues
  
  Matrix<Double> getAbsoluteGainValues(Matrix<DComplex> const &spectra)   
  {
    Matrix<Double> absoluteValueArray( amplitude( spectra )) ;
    
    return absoluteValueArray ;
  }
  
  // ------------------------------------------------------ getSegmentationVector
  
  Vector<uint> RFIMitigation ::getSegmentationVector(uint const &dataBlockSize, 
						     uint const &nOfSegments) 
  {  
    try {
      
      // nOfSegments = 25; optimized no. of segment for each data blocksize
      Int fft_Length = ( dataBlockSize/2 )+1 ;
      
      Vector<uint> segmentation( nOfSegments-1 );
      
      Int frequencyChannels = int( fft_Length/ nOfSegments ) ;
      
      for( uint ns=0; ns <( nOfSegments -1 ); ns++ ) {
	
	segmentation( ns ) = ( ns +1 )*frequencyChannels ;
      }
      
      return segmentation ;
    }
    catch ( AipsError x ) {
      cerr << "RFIMitigation :: getSegmentationVector " << x.getMesg () << endl ;
      return Vector<uint> ();
    }
  }
  
  // ------------------------------------------------------- getinterpolatedGains

  Matrix<Double> RFIMitigation :: getinterpolatedGains(Matrix<DComplex> const &spectra,
						       uint const &dataBlockSize,
						       uint const &nOfSegments  ) 
    
  {
    try {
      
      RFIMitigation rf;
      
      Matrix<Double> absoluteArray ( amplitude ( spectra )) ;
      Vector<uint> segmentationIndices = rf.getSegmentationVector(dataBlockSize,
								  nOfSegments);
      
      uInt segCount     (segmentationIndices.nelements()+1);
      uInt nOfRows      (absoluteArray.nrow());
      uInt nOfColumns   (absoluteArray.ncolumn());
      uInt ele          (1);
      uInt segmentInit  (0);
      uInt segmentFinal (0);
      
      Matrix<Double> filteredMatrix (segCount, nOfColumns) ;
      Vector<Double> filteredIndexMedian( segCount );
      Vector<Double> filteredDataIndexInit( segCount+1 );
      Vector<Double> filteredDataIndexFinal( segCount );
      
      for(uint i=0; i< segCount; i++) {
	
	filteredDataIndexInit( i )= segmentInit ;
	
	if( i == ( segCount-1 )) {
	  segmentFinal = nOfRows -1;
	  filteredDataIndexFinal( i )= segmentFinal;
	  filteredIndexMedian( i )=( filteredDataIndexInit( i )+ filteredDataIndexFinal( i ))/2;
	}
	else {
	  segmentFinal = segmentationIndices( ele-1 ) ;
	  filteredDataIndexFinal( i )= segmentFinal;
	  filteredIndexMedian( i )=( filteredDataIndexInit( i )+ filteredDataIndexFinal( i ))/2;
	} ;
	
	filteredDataIndexInit( i+1 )= filteredDataIndexFinal( i )+1;
	
	uint segmentLength = ( segmentFinal - segmentInit + 1) ;
	
	Matrix<Double> segmentMatrix( absoluteArray(Slice(segmentInit, segmentLength),
						    Slice(0, nOfColumns) ) );
	
	//StatisticsFilter <Double> mf ( filterStrength, filterType_p );
	Bool sorted( False );
	Bool takeEvenMean( False );
	Bool inPlace( False );
	
	for ( uint j = 0; j < nOfColumns; j++) {
	  
	  Vector<Double> segmentVector = segmentMatrix.column(j);
	  
	  filteredMatrix (i,j) = median( segmentVector,
					 sorted,
					 takeEvenMean,
					 inPlace);
	} 
	segmentInit = segmentFinal+1;
	ele = ele+1;
      }
      
      uInt nOfRowsFilteredGains = filteredMatrix.nrow() ;
      
      Vector<Double> xin( nOfRowsFilteredGains );
      
      Vector<Double> xout( nOfRows );
      
      Vector<Double> yout( nOfRows );
      
      for( uint k = 0; k< nOfRows; k++){
	
	xout( k )= k*1.0000;
      }
      
      Matrix<Double> interpolatedGains( nOfRows, nOfColumns, 0.0);
      
      for( uint p =0; p< segCount; p++){
	
	xin( p )= filteredIndexMedian( p );
      }
      
      for( uint column = 0; column< nOfColumns ; column++ ){
	
	Vector<Double> yin = filteredMatrix.column( column );
	
	InterpolateArray1D<Double,Double>::interpolate (yout,
							xout,
							xin,
							yin,
							InterpolateArray1D<Double,Double>::linear);
	
	interpolatedGains.column( column ) = yout;
      }
      
      return interpolatedGains ; 
    }
    catch ( AipsError x ) {
      cerr << "RFIMitigation :: getFilteredGainValues " << x.getMesg () << endl ;
      return Matrix<Double> ();
    }
    
  } // end of the function getinterpolatedGains
  
  // ------------------------- getdifferenceSpectra-------------------------------------
  
  Matrix<Double> RFIMitigation :: getdifferenceSpectra( const Matrix<DComplex>& spectra,
							const uint& dataBlockSize,
							const uint& nOfSegments )
  {
    
    try {
      
      RFIMitigation rf;
      
      Matrix<Double> gainValues( amplitude( spectra ) );
      // no. of rows of the segmentation matrix
      uint rows_segArray = gainValues.nrow();
      
      // no. of columns of the segmentation matrix
      uint columns_segArray = gainValues.ncolumn();		
      
      Vector<uint> segmentationIndices = rf.getSegmentationVector( dataBlockSize, nOfSegments ) ;
      
      Matrix<Double> derivedSpectra = rf. getinterpolatedGains( spectra, 
								dataBlockSize, 
								nOfSegments ) ;
      
      // no. of rows of the interpolated segmented matrix
      uInt rows_derivedSpectra = derivedSpectra.nrow();
      
      // no. of columns of the interpolated segmented matrix
      uInt columns_derivedSpectra = derivedSpectra.ncolumn();   
      
      Matrix<Double> spectra ( rows_derivedSpectra, columns_derivedSpectra, 0.0);
      
      if ( (rows_segArray == rows_derivedSpectra) &&
	   (columns_segArray == columns_derivedSpectra) ) {
	
	for( uInt column=0; column< columns_segArray; column++) {
	  
	  segmentInit = segmentFinal ;
	  
	  for(uInt row=0; row < rows_segArray; row++){
	    
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
      return Matrix<Double> ();
    }
    
  } // end of the function differenceSpectra
  
  
  
  // ------------------------------ getnormalizeSpectra---------------------------------
  
  
  Matrix<Double> RFIMitigation :: getnormalizeSpectra( const Matrix<DComplex>& spectra,
						       const uint& dataBlockSize,
						       const uint& nOfSegments )
    
  {
    
    try {
      
      RFIMitigation rf;
      
      Matrix<Double> gainValues( amplitude( spectra ) );
      
      // no. of rows of the segmentation matrix
      uInt rows_segArray = gainValues.nrow();
      
      // no. of columns of the segmentation matrix
      uInt columns_segArray = gainValues.ncolumn();		
      
      Vector<uint> segmentationIndices = rf.getSegmentationVector( dataBlockSize,
								   nOfSegments ) ;
      
      Matrix<Double> derivedSpectra = rf. getinterpolatedGains( spectra, 
								dataBlockSize, 
								nOfSegments ) ;
      
      // no. of rows of the interpolated segmented matrix
      uInt rows_derivedSpectra = derivedSpectra.nrow();
      
      // no. of columns of the interpolated segmented matrix
      uInt columns_derivedSpectra = derivedSpectra.ncolumn();   
      
      Int wasDivError (0);
      Int Counter     (0);
      uInt segcolumn  (0);
      uInt segrow     (0);
      
      //define a matrix to store the normalize spectra
      Matrix<Double> normalizeSpectra( rows_segArray, columns_segArray, 0.0); 
      
      if( (rows_segArray == rows_derivedSpectra) && 
	  (columns_segArray == columns_derivedSpectra) ){
	
	for (segcolumn=0; segcolumn< columns_segArray; segcolumn++ ){
	  
	  for (segrow=0; segrow< rows_segArray; segrow++ ){
	    
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
	      normalizeSpectra ( segrow, segcolumn)= gainValues( segrow,segcolumn)/derivedSpectra( segrow, segcolumn) ;
	      //	cout << segrow << segcolumn << gainValues( segrow,segcolumn)<< derivedSpectra( segrow, segcolumn)
	      //	       << normalizeSpectra (segrow, segcolumn) << endl ;
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
      return Matrix<Double> ();
    }
  } // end of the function getNormalizeSpectra
  
  // -------------------------- retraceOriginalSpectra---------------------------------
  
  
  
  Matrix<Double> RFIMitigation :: getOptimizedSpectra( const Matrix<DComplex>& spectra,
						       const uint& dataBlockSize,
						       const uint& nOfSegments )
    
  {
    
    try {
      
      RFIMitigation rf;
      
      Matrix<Double> gainValueSpectra = rf.getnormalizeSpectra( spectra,
								dataBlockSize,
								nOfSegments );
      
      uInt nOfRows = gainValueSpectra.nrow();
      
      uInt nOfColumns = gainValueSpectra.ncolumn();
      
      Matrix<Double> spectra( nOfRows, nOfColumns, 1.0 );
      
      Vector<uint> segmentationIndices = rf.getSegmentationVector( dataBlockSize, 
								   nOfSegments ) ;

      uInt i (0);
      uInt j (0);
      uInt k (0);
      
      for (i=0; i< nOfColumns ; i++ ) {
	
	uInt segCount (segmentationIndices.nelements()+1);
	uInt spectraVecElements (0);
	Int ele (1);
	Int segmentInit (0);
	Int segmentFinal (1);
	Int segmentLength (0);
	Double stdDeviationFinal( 1e35 );
	Double stdDeviationValue( 0.0);
	
	//Double mean(1.000);
	
	Vector<Double> spectraColumn = gainValueSpectra.column( i );
	
	for (j=0; j< segCount ; j++) {
	  
	  if( j == ( segCount -1 )) {
	    segmentFinal = nOfRows-1 ;
	    //cout<< " segment final :" << segmentFinal << endl ;
	  }
	  else {
	    segmentFinal = segmentationIndices ( ele -1);
	    
	  }
	  
	  segmentLength = (segmentFinal - segmentInit + 1) ;
	  
	  Vector<Double> segmentedVector( spectraColumn ( Slice( segmentInit,
								 segmentLength)));
	  
	  stdDeviationValue = stddev( segmentedVector ) ;
	  
	  if ( stdDeviationValue < stdDeviationFinal ){
	    
	    stdDeviationFinal = stdDeviationValue ; 
	  }
	  
	  //  cout<< " Final standard deviation value : " << stdDeviationFinal << endl; 
	  
	  segmentInit = segmentFinal + 1;
	  
	  ele = ele + 1 ;
	}  
	
	spectraVecElements = spectraColumn.nelements() ;
	
	Vector<Double> rfirejectedspectra( spectraVecElements, 1.0 );
	
	for(k = 0; k < spectraVecElements ; k++ ) {    
	  if(  spectraColumn( k ) > 5*stdDeviationFinal ) {
	    rfirejectedspectra( k ) = stdDeviationFinal/ spectraColumn( k ) ;
	  }
	}
	spectra.column(i) = rfirejectedspectra ;
      } 
      return spectra ;
    } catch ( AipsError x ) {
      cerr << "RFIMitigation :: getRFIRemovedSpectra " << x.getMesg () << endl ;
      return Matrix<Double> ();
    }
    
  } // end of the function retraceOriginalSpectra
  
}  // Namespace CR -- end
