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

#include <Analysis/ppfimplement.h>

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  ppfimplement::ppfimplement ()
  {
    filterRows_p    = 1024;
    filterColumns_p = 16;
  }
  
  ppfimplement::ppfimplement( const Vector<Double>& samples,
			      const Vector<Double>& FIRCoefficients )
  {
    filterRows_p    = 1024;
    filterColumns_p = 16;
    Vector<Double> signal ;
  }
  
  ppfimplement::ppfimplement (ppfimplement const &other)
  {
    copy (other);
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  ppfimplement::~ppfimplement ()
  {
    destroy();
  }
  
  void ppfimplement::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================
  
  ppfimplement& ppfimplement::operator= (ppfimplement const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }
  
  void ppfimplement::copy (ppfimplement const &other)
  {
    filterRows_p    = other.filterRows_p;
    filterColumns_p = other.filterColumns_p;
  }
  
  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  void ppfimplement::summary ()
  {;}
  
  // ============================================================================
  //
  //  Computational Methods
  //
  // ============================================================================
  
  Matrix<Double> ppfimplement::setFilterCoefficients ( const Vector<Double> &FIRCoefficients)
  {   
    Matrix<Double> ppf_coeff (filterRows_p,filterColumns_p);

    ppf_coeff = 0.0;
    
    try {
      uint h1 = 0;
      uint row=0;
      uint col=0;
      
      for(col=0; col<filterColumns_p; col++ ){
	for(row=0; row<filterRows_p; row++ ){
	  ppf_coeff(row,col) = FIRCoefficients(row+h1*col) ;
	}
	h1 = filterRows_p;
      }
    } catch (AipsError x) {
      cerr << "[ppfimplement::setFilterCoefficients] " << x.getMesg() << endl ;
    }
    
    return ppf_coeff;
  }  
  
  
  
  Matrix<DComplex> ppfimplement::setIDFTMatrix ( const uint& filterRows )
    
  {
    try {
    uint filterRows = filterRows_p;
       
      Double pi = 3.14159265358979323846 ;
      
      Matrix<DComplex> twiddleFactor_inv(filterRows , filterRows ) ;
      
      Matrix<DComplex> twiddleFactorinv(filterRows , filterRows ) ;
      
      DComplex expo(0, 2.*pi/filterRows);
      
      for( uint r=0; r< filterRows; r++ ){
	
	for( uint s=0; s< filterRows; s++ ){
	  
	  twiddleFactor_inv( r,s ) = exp(expo*Double(r*s)) ;
	}
      }
      twiddleFactorinv = twiddleFactor_inv*(1./(filterRows)) ; // adding 1024 factor from it just to check
      
      return twiddleFactorinv ;
    }
    
    catch( AipsError x ){
      cerr << "ppfimplementation :: setIDFTMatrix " << x.getMesg () << endl ;
      return Matrix<DComplex> ();
    }
  }
  
  
  Matrix<Double> ppfimplement::FIRimplementation( Vector<Double>& samples,
						  const Matrix<Double>& FIRCoefficientArray)
    
  {
    try {
      uint nofRows = FIRCoefficientArray.nrow() ;
      
      uint nofsample = samples.nelements();
      
      uint nofSegment = int(nofsample/nofRows);
      
      Vector<Double> ppf_multiplied( filterRows_p, 0.0 );
      
      Vector<Double> sliced_sampled_vector( filterRows_p, 0.0 );
      
      Matrix<Double> sliced_sampled_matrix( filterRows_p, filterColumns_p, 0.0 );
      
      Matrix<Double> shifted_sampled_matrix( filterRows_p, filterColumns_p, 0.0 );	
      
      Matrix<Double> ppf_implemented( filterRows_p, filterColumns_p, 0.0 );	
      
      Matrix<Double> ppf_output( filterRows_p, nofSegment, 0.0 );
      
      uint Sample = 0 ;
      
      for( uint columns = 0; columns< nofSegment; columns++ ){
	
	for( uint rest_columns = 0 ; rest_columns < 15 ; rest_columns++ ){
	  
	  shifted_sampled_matrix.column( rest_columns +1 ) = sliced_sampled_matrix.column( rest_columns ) ;
	}
	
	sliced_sampled_vector = samples( Slice( Sample, filterRows_p ) ) ;
	
	shifted_sampled_matrix.column( 0 ) = sliced_sampled_vector ;
	
	sliced_sampled_matrix = shifted_sampled_matrix ;
	
	for( uint multi =0; multi < filterColumns_p ; multi++ ){
	  
	  ppf_implemented.column( multi ) = sliced_sampled_matrix.column( multi )*FIRCoefficientArray.column( multi );  
	}
        
	for( uint summing =0; summing < filterRows_p; summing++ ){
	  
	  ppf_multiplied( summing ) = sum( ppf_implemented.row( summing ) ) ;
	}
        
	//   ppf_multiplied_corrected = ceil(ppf_multiplied*10.)/10. ;
	ppf_output.column( columns ) = ppf_multiplied ;
	
	Sample = Sample + nofRows ;        
        
      }
      
      return ppf_output ;
    }
    
    catch( AipsError x){
      cerr << "ppfimplementation :: FIRimplementation " << x.getMesg () << endl ;
      return Matrix<Double> ();
    }
  }  
  
  
  
  Matrix<DComplex> ppfimplement::FFTSamples( Vector<Double>& samples,
                                             const Vector<Double>& FIRCoefficients )
			   
 {
    try {
         ppfimplement ppfimpl ;
	 
	Matrix<Double> filter_coeff = ppfimpl.setFilterCoefficients( FIRCoefficients ) ;
	         
        uint nofrows = filter_coeff.nrow() ;
	
	Matrix<DComplex> IDFT_matrix = ppfimpl.setIDFTMatrix( nofrows ) ;
	
	Matrix<Double> fir_output = ppfimpl.FIRimplementation( samples,
                                                               filter_coeff ) ;

	uint nofcolumns = fir_output.ncolumn() ;
							        
        Matrix<DComplex> fir_output_complex( filterRows_p, nofcolumns, 0.0 ) ;
	 
	convertArray( fir_output_complex, fir_output ) ;
	 
	Vector<DComplex> frequencyResponse(filterRows_p,0.0) ;
	 
	Matrix<DComplex> inverse_DFT_matrix( filterRows_p, nofcolumns, 0.0 ) ;
	 
	for( uint COLUMN =0; COLUMN < nofcolumns; COLUMN++ ){
	 
	    for( uint ROW =0; ROW < filterRows_p; ROW++ ){
	    
	         frequencyResponse( ROW ) = sum( IDFT_matrix.row( ROW )*fir_output_complex.column( COLUMN ) ) ;
		 
	     }
	     inverse_DFT_matrix.column( COLUMN ) = frequencyResponse ;
	     
	 }
 
       return inverse_DFT_matrix ; 
    }
    catch( AipsError x ){
    cerr << "ppfimplementation :: FFTSamples " << x.getMesg () << endl ;
    return Matrix<DComplex> () ;
    }
  }  
  
  
} // Namespace CR -- end
