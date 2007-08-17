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

/* $Id: ppfinversion.cc,v 1.5 2007/08/08 15:29:50 singh Exp $*/

#include <Analysis/ppfinversion.h>
#include <Analysis/SubbandID.h>

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  //Default Constructor
  
  ppfinversion::ppfinversion ()
  {;}
  
 // Argumented Constructor
  
  ppfinversion::ppfinversion( const Matrix<DComplex>& FTData,
                              const Vector<Double>& FIRCoefficients ) 
  {
  //Matrix<DComplex> timeseriesdata ;
  }
  
  //--------end of Argumented Constructor : ppfinversion------------------------
  //-----------------------------------------------------------------------------
  
  ppfinversion::ppfinversion (ppfinversion const &other)
  {
    copy (other);
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  ppfinversion::~ppfinversion ()
  {
    destroy();
  }
  
  void ppfinversion::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================
  
  ppfinversion& ppfinversion::operator= (ppfinversion const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }
  
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
            Double pi = 3.1416 ;
	    
	    Matrix<DComplex> twiddleFactor( nofsubbands , nofsubbands ) ;
	 
	    Matrix<DComplex> twiddleFactor_inv( nofsubbands , nofsubbands ) ;
	 
	    Matrix<DComplex> conjugate_twiddle( nofsubbands , nofsubbands ) ;
	 
	    DComplex j(0, -2.*pi/nofsubbands );
	 
	    for( uint r=0; r< nofsubbands; r++ ){
	 
	        for( uint s=0; s< nofsubbands; s++ ){
	      
	             twiddleFactor_inv( r,s ) = exp(j*Double(r*s)) ;
		     }
             }
	    
	    twiddleFactor = twiddleFactor_inv*(1./1024.) ;
	    
	    return twiddleFactor ;
	      
	   }
        catch( AipsError x ){
	cerr << " ppfinversion::setDFTMatrix " << x.getMesg () << endl ;
	return Matrix<DComplex> () ;
	}
    }
    
    

    Matrix<Double> ppfinversion :: DFTinversion ( const Matrix<DComplex>& generatedSubbands,
                                                  const Matrix<DComplex>& DFTMatrix )

    {
      try {

           uint nOfColumns = generatedSubbands.ncolumn();

           uint nOfRows = DFTMatrix.nrow() ;

           Matrix<Double> DFTinvertedMatrix ( nOfRows, nOfColumns, 0.0 );

           for( uint c= 0; c < nOfColumns; c++ ){
 
                for( uint r= 0 ; r < nOfRows ; r++ ){

                     DFTinvertedMatrix( r,c ) = real( sum ( DFTMatrix.row(r)*generatedSubbands.column(c)) ) ;

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
                               	                const Matrix<DComplex>& FTData ) 
						    
    {
     try {
          
          ppfinversion ppfinv ;
	  
          Matrix<Double> inv_fir_coeff = ppfinv.setInverseFilterCoefficients( FIRCoeff_inv ) ;
	  
	  uint NOFROWS = inv_fir_coeff.nrow () ;
	  
	  uint NOFCOLUMNS = inv_fir_coeff.ncolumn() ;
	 
	  Matrix<DComplex> DFT_matrix = ppfinv.setDFTMatrix( NOFROWS ) ; 
	 
	  Matrix<Double> DFTinverted = ppfinv.DFTinversion ( FTData,
                                                             DFT_matrix ) ;
							     
	  uint nofRows = DFTinverted.nrow() ;
	  
	  uint nofColumns = DFTinverted.ncolumn() ;
          
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
  			
			
 } // Namespace CR -- end
