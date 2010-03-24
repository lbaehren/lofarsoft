/***************************************************************************
 *   Copyright (C) 2007                                                    *
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

#include <Analysis/ppfinversion.h>
#include <Analysis/SubbandID.h>

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================

  //_____________________________________________________________________________
  //                                                                 ppfinversion

  ppfinversion::ppfinversion ()
  {;}
  
  //_____________________________________________________________________________
  //                                                                 ppfinversion

  ppfinversion::ppfinversion (const Matrix<DComplex>& FTData,
                              const Vector<Double>& FIRCoefficients,
			      const Vector<uint> subBandIDs) 
  {
    FTData_p          = FTData;
    FIRCoefficients_p = FIRCoefficients;
    subBandIDs_p      = subBandIDs;
  }

  //_____________________________________________________________________________
  //                                                                 ppfinversion

  ppfinversion::ppfinversion (ppfinversion const &other)
  {
    copy (other);
  }
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================
  
  void ppfinversion::copy (ppfinversion const &other)
  {;}

  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  void ppfinversion::summary ()
  {;}
  
  
  
  // ============================================================================
  //
  //  Computational Methods
  //
  // ============================================================================
  
    
   Matrix<Double> ppfinversion::setInverseFilterCoefficients( const Vector<Double>& FIRCoeff_inv )
    
    {
      try{
           uint accumulation = 0;   
	   
	   Matrix<Double> ppf_coeff( 1024, 16, 0.0 );
	   
	   for( uint r =0; r <1024; r++ ){
	   
	        for( uint s =0; s <16; s++ ){
		
		         ppf_coeff( r,s ) = FIRCoeff_inv( s + accumulation*r ) ;
		     }
		 accumulation = 16 ;
	    }
	    
	  return   ppf_coeff ;
	 }
       catch ( AipsError x ){
       cerr << " ppfinversion::setFilterCoefficients " << x.getMesg() << endl ;
       return Matrix<Double> ();
       }
     }
     
          
     Matrix<DComplex> ppfinversion::setDFTMatrix( const uint& nofsubbands ) 
     
     {
       try {
            Double pi = 3.14159265358979323846 ;
	    
	   uint nofsubbands = 1024 ;
	   
	    Matrix<DComplex> twiddleFactor( nofsubbands , nofsubbands ) ;
	 
	    Matrix<DComplex> twiddleFactor_inv( nofsubbands , nofsubbands ) ;
	 
	    Matrix<DComplex> conjugate_twiddle( nofsubbands , nofsubbands ) ;
	 
	    DComplex expo(0, -2.*pi/Double(nofsubbands) );
	 
	    for( uint r=0; r< nofsubbands; r++ ){
	 
	        for( uint s=0; s< nofsubbands; s++ ){
	      
	             twiddleFactor_inv( r,s ) = exp(expo*Double(r*s)) ;
		     }
             }
	    
	    twiddleFactor = twiddleFactor_inv ;  // removing 1024 factor from it just to check
	    
	//    cout << " twiddle factor matrix : " << twiddleFactor.row(3) << endl ;
	    
	    return twiddleFactor ;
	      
	   }
        catch( AipsError x ){
	cerr << " ppfinversion::setDFTMatrix " << x.getMesg () << endl ;
	return Matrix<DComplex> () ;
	}
    }
    
    
   Matrix<DComplex> 
   ppfinversion::setGeneratedSubbands( const Matrix<DComplex>& filteredMatrix,
				       const Vector<uint> subBand_IDs ) 
     
   {
     try {
       
       uint nColumns = filteredMatrix.ncolumn() ;
       // uint nRows = filteredMatrix.nrow() ;	    	    
       
       Matrix <DComplex> generatedSubbands(1024, nColumns,0.0 ) ;
       
       uint nofelement = subBand_IDs.nelements() ;
       
       for(uint i=0; i< nofelement; i++ ){
	 
	 uint subbandID = subBand_IDs(i) ;
	 
	 //	 Vector<DComplex> filtered_vector = filteredMatrix.row(512-subbandID) ;
	 Vector<DComplex> filtered_vector = (filteredMatrix.row(i)) ;
	 
	 //		 generatedSubbands.row(512-subbandID) = filtered_vector ;
	 generatedSubbands.row(subbandID) = filtered_vector ;
	 
	 // if( subbandID< 512 ){	 
	 //		 generatedSubbands.row(512+subbandID) = conj(filteredMatrix.row(512-subbandID)) ;
	 generatedSubbands.row(1024-subbandID) = (conj(filteredMatrix.row(i))) ;
	 
	 // }
	 
       }
       
       return generatedSubbands ;
     }
     
     catch( AipsError x ){
       cerr << " ppfinversion::setGeneratedSubbands " << x.getMesg () << endl ;
       return Matrix<DComplex> () ;
     }
   }
  
  
  
  Matrix<Double> ppfinversion::DFTinversion ( const Matrix<DComplex>& generatedSubbands,
					      const Matrix<DComplex>& DFTMatrix,
					      const Vector<uint> subBand_IDs )
    
  {
    try {
      uint nofelement = subBand_IDs.nelements() ;
      double base_p   = 512/nofelement ;
      uint nOfColumns = generatedSubbands.ncolumn();
      uint nOfRows    = DFTMatrix.nrow() ;
      
      Matrix<Double> DFTinvertedMatrix ( nOfRows, nOfColumns, 0.0 );
      
      for( uint c= 0; c < nOfColumns; c++ ){
	
	for( uint r= 0 ; r < nOfRows ; r++ ) {
	  
	  Double DFTinverted = (base_p)*real( sum ( DFTMatrix.row(r)*generatedSubbands.column(c)) ) ;
	  
	  DFTinvertedMatrix( r,c ) = DFTinverted ;
	}
      }
      
      
      return DFTinvertedMatrix ;
    }
    catch( AipsError x ){
      cerr << "ppfinversion :: DFTinversion " <<  x.getMesg () << endl ;
      return Matrix<Double> () ;
    }
  }
  
  
    Vector<Double> ppfinversion::FIR_inversion( const Vector<Double>& FIRCoeff_inv,
                               	                const Matrix<DComplex>& FTData,
						const Vector<uint> subBand_IDs ) 
						    
    {
     try {
          
          ppfinversion ppfinv ;
	  
	  Matrix<Double> inv_fir_coeff = ppfinv.setInverseFilterCoefficients( FIRCoeff_inv ) ;
	 
	  uint NOFROWS = inv_fir_coeff.nrow () ;
	  
	  uint NOFCOLUMNS = inv_fir_coeff.ncolumn() ;
		  
	  Matrix<DComplex> DFT_matrix = ppfinv.setDFTMatrix( NOFROWS ) ; 
	  
	  uint n_Rows = FTData.nrow() ;
	  
	  uint n_Columns = FTData.ncolumn() ;
	  
	 Matrix<Double> DFTinverted( 1024, n_Columns, 0.0 ) ;
	  
	  cout << " number of rows in the input matrix to inversion : "<< n_Rows <<endl ;
	  if(n_Rows <512)
		{  
	  Matrix<DComplex> generatedsubbands = ppfinv.setGeneratedSubbands( FTData,
                                                                            subBand_IDs )  ;
	
	  cout << "number of subbands is less than 512  "<< endl ;								    
	  Matrix<Double> DFTinverted_cal = ppfinv.DFTinversion ( generatedsubbands,
                                                                 DFT_matrix,
								 subBand_IDs  ) ;
	 
	   DFTinverted = DFTinverted_cal ;
		}
	 else 
	   {
	   cout << "number of subband is greater than 512 " << endl ;
	   Matrix<Double> DFTinverted_cal = ppfinv.DFTinversion ( FTData,
                                                            DFT_matrix,
							    subBand_IDs ) ;
							    
           DFTinverted = DFTinverted_cal ;							    
	   } 
         
	 uint nofRows = DFTinverted.nrow() ;
	 cout << " number of rows in DFTinverted matrix :" << nofRows <<endl ;
	  
	  uint nofColumns = DFTinverted.ncolumn() ;
	  cout << " number of columns in DFTinverted matrix :" << nofColumns <<endl ;
	  
	  Vector<Double> ppf_multiplied_inv( nofRows, 0.0 );
	
	  Matrix<Double> sliced_matrix_inv( nofRows, NOFCOLUMNS, 0.0 );
	
	  Matrix<Double> shifted_matrix_inv( nofRows, NOFCOLUMNS, 0.0 );	

	  Matrix<Double> ppf_implemented_inv( nofRows, NOFCOLUMNS, 0.0 );	
         
	  Matrix<Double> output( nofRows, nofColumns, 0.0 );
	  
	  Vector<Double> data_retrieved( nofRows*nofColumns, 0.0 );
	  
	  uint sample_inv =0 ;
	  
	  for( uint first_columns =0; first_columns < 15; first_columns++ ){
	     
	     for( uint rest_columns = 0 ; rest_columns < (NOFCOLUMNS-1) ; rest_columns++ ){
	     
	           shifted_matrix_inv.column( rest_columns +1 ) = sliced_matrix_inv.column( rest_columns ) ;
	          }
	     
	     shifted_matrix_inv.column( 0 ) = DFTinverted.column( first_columns ) ;
	     
	     sliced_matrix_inv = shifted_matrix_inv ;
	     }
          
          for( uint columns = 15; columns< (nofColumns) ; columns++ ){

               for( uint rest_columns = 0 ; rest_columns < (NOFCOLUMNS-1) ; rest_columns++ ){
	     
	           shifted_matrix_inv.column( rest_columns +1 ) = sliced_matrix_inv.column( rest_columns ) ;
	          }
	              
               shifted_matrix_inv.column( 0 ) = DFTinverted.column( columns ) ;
	       
	       sliced_matrix_inv = shifted_matrix_inv ;
	       
	       for( uint multi =0; multi < NOFCOLUMNS ; multi++ ){
	     
	         ppf_implemented_inv.column( multi ) = sliced_matrix_inv.column( multi )*inv_fir_coeff.column( multi );  
		 }
        
               for( uint summing =0; summing < nofRows; summing++ ){
	     
	           ppf_multiplied_inv( summing ) = sum( ppf_implemented_inv.row( summing ) ) ;
	           }
		   
              for( uint r=0; r< nofRows; r++ ){
	      
	           data_retrieved( r + nofRows*sample_inv ) = ppf_multiplied_inv(r);
		   }
	      sample_inv = sample_inv +1 ;
	         
          }
	  
	return  data_retrieved ;
       }
	
       catch ( AipsError x ){
       cerr << "  ppfinversion::FIR_inversion " << x.getMesg () << endl ;
       return Vector<Double> () ;
       }
   }	    
 
  
  
  
  Vector<Double> ppfinversion::simple_fft( Vector<Double> input,
                                           const uint dataBlockSize ) 
  {
     try {
           
     	   FFTServer <Double,DComplex> server ;
	   
	   uint nofelements =input.nelements() ;
	   
	   uint nblocks = int( nofelements/dataBlockSize ) ;
	 //  cout << " number of blocks :" << nblocks <<endl ;
	   
	   Vector<DComplex> FFTVector_f ; 
	   
	   Vector<Double> sliced_vector( dataBlockSize, 0.0 ) ;
	   
	   Vector<Double> time_sliced( dataBlockSize, 0.0 );
	   
	   Vector<Double> time_series( nofelements, 0.0 ) ;
	   	   
	   uint sample(0);
	             
	   for( uint i=0; i< nblocks; i++ ){
	       
	      sliced_vector = input( Slice( sample, dataBlockSize ) ) ;
	      
	      Vector<DComplex> FFTVector( dataBlockSize/2+1, 0.0 ) ;
	      
	      uint element = FFTVector.nelements();
	      
	      uint div_subs = int(element/3);
	      
	      server.fft( FFTVector, sliced_vector ) ;
	      
	      for( uint j=0; j<div_subs; j++ ){
	      
	           FFTVector(j)= 0. ;
		   
		   }
		   
              for( uint k=2*div_subs; k< element; k++) {
	      
	          FFTVector(k)= 0. ;
		  
	          }
	      
	      server.fft( time_sliced, FFTVector ) ;
	      
	      uint m(0);
	      
	      for( uint l=sample; l< (sample+dataBlockSize); l++ ){
	      
	          time_series(l) = time_sliced(m) ;
		  
		  m = m+1 ;
		  
		  }
	      
	      cout << " sample number with which blocks has been cut :" << sample << endl ;
	      sample = sample + dataBlockSize ;
	     }
	        
	    return time_series  ;
       }
	
       catch ( AipsError x ){
       cerr << "  ppfinversion::simple_fft " << x.getMesg () << endl ;
       return Vector<Double> () ;
       }
   }	     			       

    
  Vector<Double> ppfinversion::simple_ifft( const Vector<DComplex> fftvector,
  			                    Vector<uint> subBandVector)   
  			                   
  {
     try {
     
          FFTServer <Double,DComplex> server ;
	  
	  uint nofelements = subBandVector.nelements() ;
	  
	  Vector<DComplex> slicedfft_vector( nofelements, 0.0 ) ;
	  
	  for( uint i=0; i< nofelements; i++ ){
	  
	       uint bandID = subBandVector(i) ;
	       
	       slicedfft_vector(i) = fftvector( bandID );
	       
	       }
	       
	  Vector<Double> ifft_vector( (nofelements-1)*2, 0.0 ) ;
	  
	  server.fft( ifft_vector, slicedfft_vector ) ;
	  
       return  ifft_vector ;
       }
	
       catch ( AipsError x ){
       cerr << "  ppfinversion:: simple_ifft " << x.getMesg () << endl ;
       return Vector<Double> () ;
       }
   }	 
 
   
 } // Namespace CR -- end
