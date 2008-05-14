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

/* $Id: inverseFFT.cc,v 1.4 2007/04/03 14:03:07 bahren Exp $*/

#include <Analysis/inverseFFT.h>

namespace CR {  // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================

  // ----------------------------------------------------------------- inverseFFT
  
  inverseFFT::inverseFFT ()
  {
 
  }
  
  // ----------------------------------------------------------------- inverseFFT
  
  inverseFFT::inverseFFT (const Double& F107,
                          const Double& mean_F107,
		          const uint& tableno,
                          const uint& col,
                          const uint& t_d,
                          const Double& tau,
                          const Double& t,
                          const Double& Ap,
                          const Double& latitude,
                          const Double& longitude,
                          const Double& altitude,
                          const Double& molecular_weight,
                          const Double& n_l_average )
  
  {
  ;
  }
  
  // ----------------------------------------------------------------- inverseFFT

  inverseFFT::inverseFFT (inverseFFT const &other)
  {
    copy (other);
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  inverseFFT::~inverseFFT ()
  {
    destroy();
  }
  
  void inverseFFT::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================
  
  inverseFFT& inverseFFT::operator= (inverseFFT const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }
  
  void inverseFFT::copy (inverseFFT const &other)
  {
    ppfCoefficients_p = other.ppfCoefficients_p;
    dataBlocksize_p   = other.dataBlocksize_p;
  }
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================
   Double inverseFFT::diff_legendre( const Double& latitude,
                                     const uint& degree,
                                     const uint& order ) 
   {				     				     
     try {
         Double pi = 3.1416 ;
         
	 Double arguement = cos(latitude*pi/180.) ;
	 Double derivative(0.0) ;
	 
	 if((degree==1) && (order==0)) 
	 
	    derivative = 2*arguement ;
	    
	    else if((degree==2) && (order==0)  )
	    
	    derivative = 12*square(arguement)-4 ;
	    
	    else if((degree==4) && (order==0) )
	    
	    derivative =1344.*square(arguement)*square(arguement)-720.*square(arguement)-192.*arguement+48. ; 
	    
	    else if((degree==3) && (order==0) )
	    
	    derivative = 120.*cube(arguement)-72.*arguement ; 
	    
	    else if((degree==3) && (order==1) )
	    
	    derivative =360.*square(arguement)-72. ; 
	    
	    else if((degree==5) && (order==1) )
	    
	    derivative =122400.*square(arguement)*square(arguement)-48960.*square(arguement)-4320. ; 
	    
	    else if((degree==2) && (order==1) )
	    
	    derivative = 24.*arguement ; 
	    
	    else if((degree==4) && (order==2) )
	    
	    derivative = 18432.*square(arguement)-2016. ; 
	    
	    else if((degree==3) && (order==2) )
	    
	    derivative =1584.*arguement ; 
	    
	    else if((degree==5) && (order==2) )
	    
	    derivative =489600.*cube(arguement) -97920.*arguement -4320. ; 
	    
	    else if((degree==2) && (order==2) )
	    
	    derivative =24. ; 
	    
	    else if((degree==3) && (order==3) )
	    
	    derivative =1584.; 
	    
	    else if((degree==4) && (order==3) )
	    
	    derivative =368964.*arguement ; 
	    
	    else if((degree==6) && (order==3) )
	    
	    derivative =4515840.*cube(arguement) -1391040.*square(arguement)-1877760.*arguement -138240. ; 
	    
	    else if((degree==4) && (order==1) )
	    
	    derivative =5376.*cube(arguement) -1440.*arguement -192. ; 
	    
	    else if((degree==6) && (order==1) )
	    
	    derivative =3991680.*square(arguement)*cube(arguement) -2592000.*cube(arguement)-604800.*arguement-1036800. ; 
	   
	   return derivative ; 
    } catch( AipsError x ) {
      cerr << "inverseFFT::diff_legendre" <<x.getMesg() << endl ;
    } 
    
   }
  
  
    Double inverseFFT::legendre( const Double& latitude,
                                 const uint& degree,
                                 const uint& order ) 
 {				     				     
     try {
     
          inverseFFT inFFT ;
	  
          Double pi = 3.1416 ;
         
	  Double arguement = cos(latitude*pi/180.) ;
	  
	  Double derivative =inFFT.diff_legendre(latitude, degree, order) ;
	  
	  Double legendre_fun(0.0) ;
	  
	  Double factorial(0.0) ;
	  
	  for( uint i=degree; i>0; i-- ){
	  
	        factorial = factorial*i ;
          
          }		
	  
	  Double pow_order = order/2. ;
	  Double pow_degree = pow(2,degree );
	  Double pow_arguement = (1.-square(arguement)) ;
	  
	  Double first_factor = pow( pow_arguement, pow_order) ;
	  
	  legendre_fun = first_factor/pow_degree*factorial*derivative ;
	  return legendre_fun ;	  
      } catch( AipsError x ) {
      cerr << "inverseFFT::legendre" <<x.getMesg() << endl ;
    } 
    
    //return legendre_fun ;
  }	 
  
  
    Double inverseFFT::solar_activity( const Double& F107,
                                       const Double& mean_F107,
                                       const uint& tableno,
                                       const uint& col,
                                       const Double& latitude ) 
   {				     				     
     try {
        
	inverseFFT inFFT ;
        Double solar_act(0.0) ;
	
        Double del_F = F107 - mean_F107 ;
	Double del_mean_F = mean_F107 - 150 ;
	
	Matrix<Double> mcoeff1( 106, 7, 0.0) ;
	Matrix<Double> mcoeff2( 27, 6, 0.0 ) ;
	
	readAsciiMatrix( mcoeff1, "modelcoeff1.dat" ) ;
	readAsciiMatrix( mcoeff2, "modelcoeff2.dat" ) ;
	
	Vector<Double> coeff1 = mcoeff1.column(col) ;
	Vector<Double> coeff2 = mcoeff2.column(col) ;
	
	Double as_legendre = inFFT.legendre(latitude, 2, 0) ;
	
	if( tableno==1)
	     
solar_act=coeff1(7)*del_mean_F+coeff1(8)*square(del_mean_F)+coeff1(5)*del_F+coeff1(6)*square(del_F)+coeff1(90)*as_legendre*del_mean_F ;
    
        else
             if(tableno==2)
solar_act=coeff2(4)*del_mean_F+0*square(del_mean_F)+0*del_F+0*square(del_F)+0*as_legendre*del_mean_F ;
     
     return solar_act ;
     
      } catch( AipsError x ) {
      cerr << "inverseFFT::solar_activity" <<x.getMesg() << endl ;
    } 
    
  //  return solar_act ;
  }
  
      
    
    
    
  Double inverseFFT::symmetrical( const uint& tableno,
                                  const uint& col,
                                  const Double& t_d,
                                  const Double& latitude ) 
  {				     				     
     try {
        inverseFFT inFFT ;
        Double sym_func(0.0) ;
	
	Double pi = 3.1416 ;
	Double omega_d = 2*pi/365. ;
		
	Matrix<Double> mcoeff1( 106, 7, 0.0) ;
	Matrix<Double> mcoeff2( 27, 6, 0.0 ) ;
	
	readAsciiMatrix( mcoeff1, "modelcoeff1.dat" ) ;
	readAsciiMatrix( mcoeff2, "modelcoeff2.dat" ) ;
	
	Vector<Double> coeff1 = mcoeff1.column(col) ;
	Vector<Double> coeff2 = mcoeff2.column(col) ;
	
	Double as_legendre = inFFT.legendre(latitude, 2, 0) ;
	
	if( tableno ==1)
	
	sym_func=coeff1(23)*cos(omega_d*(t_d-coeff1(24)))+(coeff1(25)+coeff1(26)*as_legendre)*cos(2*omega_d*(t_d-coeff1(27))) ;
        
	else
             if(tableno ==2)     
	
        sym_func = coeff2(9)*cos(omega_d*(t_d-coeff2(10)))+(coeff2(11)+0*as_legendre)*cos(2*omega_d*(t_d-coeff2(12))) ;     
   
        return sym_func ;
	
    } catch( AipsError x ) {
      cerr << "inverseFFT::symmetrical" <<x.getMesg() << endl ;
    } 
    
  //  return sym_func ;
 }
  
  
  
  Double inverseFFT::asymmetrical( const Double& F107,
                                   const Double& mean_F107,
                                   const uint& tableno,
                                   const uint& col,
                                   const Double& t_d,
                                   const Double& latitude ) 
 {				     				     
     try {
         
	 inverseFFT inFFT ; 
	 Double asym_func(0.0) ;
	
	 Double pi = 3.1416 ;
	 Double omega_d = 2*pi/365. ;
	
         Double del_F = F107 - mean_F107 ;
	 Double del_mean_F = mean_F107 - 150 ;
		
	 Matrix<Double> mcoeff1( 106, 7, 0.0) ;
	 Matrix<Double> mcoeff2( 27, 6, 0.0 ) ;
	
	 readAsciiMatrix( mcoeff1, "modelcoeff1.dat" ) ;
	 readAsciiMatrix( mcoeff2, "modelcoeff2.dat" ) ;
	
	 Vector<Double> coeff1 = mcoeff1.column(col) ;
	 Vector<Double> coeff2 = mcoeff2.column(col) ;
	
	 Double F1 = 1+coeff1(9)*del_mean_F +coeff1(5)*del_F +coeff1(6)*square(del_F) ;
	
	 Double asl_30 = inFFT.legendre(latitude, 3, 0) ;
	 Double asl_10 = inFFT.legendre(latitude, 1, 0) ;
	
	 if( tableno ==1)
	
asym_func=(coeff1(28)*asl_10+coeff1(29)*asl_30)*F1*cos(omega_d*(t_d-coeff1(30)))+coeff1(31)*asl_10*cos(2*omega_d*(t_d-coeff1(32))) ;
        
	 else
             if(tableno ==2)     
	
asym_func =(coeff2(13)*asl_10+coeff2(14)*asl_30)*F1*cos(omega_d*(t_d-coeff2(15)))+0*asl_10*cos(2*omega_d*(t_d-coeff2(12))) ;
      
      return asym_func ;
     
     } catch( AipsError x ) {
      cerr << "inverseFFT::asymmetrical" <<x.getMesg() << endl ;
    } 
    
 //   return asym_func ;
 }
     
        

 
 
   
 Double inverseFFT::diurnal( const Double& F107,
                             const Double& mean_F107,
                             const uint& tableno,
                             const uint& col,
                             const Double& t_d,
                             const Double& tau,
                             const Double& latitude ) 
 {				     				     
     try {
         inverseFFT inFFT ; 
	 
	 Double diurnal_func1(0.0) ;
	 Double diurnal_func2(0.0) ;
	 Double diurnal_func(0.0) ;
	 
	 Double pi = 3.1416 ;
	 Double omega_d = 2*pi/365. ;
	 Double omega = 2*pi/24. ;
	 
         Double del_F = F107 -mean_F107 ;
	 Double del_mean_F = mean_F107 - 150 ;
		
	 Matrix<Double> mcoeff1( 106, 7, 0.0) ;
	 Matrix<Double> mcoeff2( 27, 6, 0.0 ) ;
	
	 readAsciiMatrix( mcoeff1, "modelcoeff1.dat" ) ;
	 readAsciiMatrix( mcoeff2, "modelcoeff2.dat" ) ;

	 Vector<Double> coeff1 = mcoeff1.column(col) ;
	 Vector<Double> coeff2 = mcoeff2.column(col) ;
	
	 Double F2 = 1+coeff1(10)*del_mean_F +coeff1(5)*del_F +coeff1(6)*square(del_F) ;
	
	 Double asl_11 = inFFT.legendre(latitude, 1, 1) ;
	 Double asl_31 = inFFT.legendre(latitude, 3, 1) ;
	 Double asl_51 = inFFT.legendre(latitude, 5, 1) ;
	 Double asl_21 = inFFT.legendre(latitude, 2, 1) ;
	
	 if( tableno ==1)
	{
diurnal_func1=(coeff1(33)*asl_11+coeff1(34)*asl_31+coeff1(51)*asl_51+coeff1(36)*asl_21*cos(omega_d*(t_d-coeff1(30))))*F2*cos(omega*tau) ;
diurnal_func2=(coeff1(37)*asl_11+coeff1(38)*asl_31+coeff1(39)*asl_51+coeff1(40)*asl_21*cos(omega_d*(t_d-coeff1(30))))*F2*sin(omega*tau) ;
	 diurnal_func = diurnal_func1+ diurnal_func2 ;
	  }
	  else
             if(tableno ==2)     
	{
diurnal_func1=(coeff2(16)*asl_11+0*asl_31+0*asl_51+0*asl_21*cos(omega_d*(t_d-coeff2(15))))*F2*cos(omega*tau);
diurnal_func2=(coeff2(17)*asl_11+0*asl_31+0*asl_51+0*asl_21*cos(omega_d*(t_d-coeff2(15))))*F2*sin(omega*tau);
	 diurnal_func =diurnal_func1 +diurnal_func2 ;          
        }
	return diurnal_func ;
	
     } catch( AipsError x ) {
      cerr << "inverseFFT::diurnal" <<x.getMesg() << endl ;
    } 
    
  //  return diurnal_func ;
 }
 
 			     

 
 
   
  Double inverseFFT::semidiurnal( const Double& F107,
                                  const Double& mean_F107,
                                  const uint& tableno,
                                  const uint& col,
                                  const Double& t_d,
                                  const Double& tau,
                                  const Double& latitude ) 
				  
  {				     				     
     try {
         
	  inverseFFT inFFT ; 
	  Double semidiurnal1(0.0) ;
	  Double semidiurnal2(0.0) ;
	  Double semidiurnal_func(0.0) ;
	  
	  Double pi = 3.1416 ;
	  Double o_d = 2*pi/365. ;
	  Double omega = 2*pi/24. ;
	 
          Double del_F = F107 -mean_F107 ;
	  Double del_mean_F = mean_F107 -150 ;
		
	  Matrix<Double> mcoeff1( 106, 7, 0.0) ;
	  Matrix<Double> mcoeff2( 27, 6, 0.0 ) ;
	
	  readAsciiMatrix( mcoeff1, "modelcoeff1.dat" ) ;
	  readAsciiMatrix( mcoeff2, "modelcoeff2.dat" ) ;
	
	  Vector<Double> coeff1 = mcoeff1.column(col) ;
	  Vector<Double> coeff2 = mcoeff2.column(col) ;
	
	  Double F2 = 1+coeff1(10)*del_mean_F +coeff1(5)*del_F +coeff1(6)*square(del_F) ;
	
	  Double asl_22 = inFFT.legendre(latitude, 2, 2) ;
	  Double asl_42 = inFFT.legendre(latitude, 4, 2) ;
	  Double asl_32 = inFFT.legendre(latitude, 3, 2) ;
	  Double asl_52 = inFFT.legendre(latitude, 5, 2) ;
	
	  if( tableno ==1)
	  {
semidiurnal1=(coeff1(41)*asl_22+coeff1(42)*asl_42+(coeff1(43)*asl_32+coeff1(91)*asl_52)*cos(o_d*(t_d-coeff1(30))))*F2*cos(2*omega*tau);
semidiurnal2=(coeff1(44)*asl_22+coeff1(45)*asl_42+(coeff1(46)*asl_32+coeff1(92)*asl_52)*cos(o_d*(t_d-coeff1(30))))*F2*sin(2*omega*tau);
          semidiurnal_func= semidiurnal1 +semidiurnal2 ;
	  }
	  else
             if(tableno ==2)     
          {
semidiurnal1=(coeff2(18)*asl_22+coeff2(19)*asl_42+(coeff2(20)*asl_32+coeff2(21)*asl_52)*cos(o_d*(t_d-coeff2(15))))*F2*cos(2*omega*tau);
semidiurnal2=(coeff2(22)*asl_22+coeff2(23)*asl_42+(coeff2(24)*asl_32+coeff2(25)*asl_52)*cos(o_d*(t_d-coeff2(15))))*F2*sin(2*omega*tau);
          semidiurnal_func= semidiurnal1 +semidiurnal2 ;
          }
	  return semidiurnal_func ;
	  
      } catch( AipsError x ) {
       cerr << "inverseFFT::semidiurnal" <<x.getMesg() << endl ;
    } 
    
  //  return semidiurnal_func ;
 }
 
 
 
 					  

    
  Double inverseFFT::terdiurnal( const Double& F107,
                                 const Double& mean_F107,
                                 const uint& tableno,
                                 const uint& col,
                                 const Double& t_d,
                                 const Double& tau,
                                 const Double& latitude ) 
				  
  {				     				     
     try {
         
	  inverseFFT inFFT ; 
	  Double terdiurnal1(0.0) ;
	  Double terdiurnal2(0.0) ;
	  Double terdiurnal_func(0.0) ;
	  
	  Double pi = 3.1416 ;
	  Double o_d = 2*pi/365. ;
	  Double omega = 2*pi/24. ;
	 
          Double del_F = F107 -mean_F107 ;
	  Double del_mean_F = mean_F107 -150 ;
		
	  Matrix<Double> mcoeff1( 106, 7, 0.0) ;
	  Matrix<Double> mcoeff2( 27, 6, 0.0 ) ;
	
	  readAsciiMatrix( mcoeff1, "modelcoeff1.dat" ) ;
	  readAsciiMatrix( mcoeff2, "modelcoeff2.dat" ) ;
	
	  Vector<Double> coeff1 = mcoeff1.column(col) ;
	  Vector<Double> coeff2 = mcoeff2.column(col) ;
	
	  Double F2 = 1+coeff1(10)*del_mean_F +coeff1(5)*del_F +coeff1(6)*square(del_F) ;
	
	  Double asl_33 = inFFT.legendre(latitude, 3, 3) ;
	  Double asl_43 = inFFT.legendre(latitude, 4, 3) ;
	  Double asl_63 = inFFT.legendre(latitude, 6, 3) ;
	
	  if( tableno ==1)
	    {
terdiurnal1=(coeff1(47)*asl_33+(coeff1(48)*asl_43+coeff1(49)*asl_63)*cos(o_d*(t_d-coeff1(30))))*F2*cos(3*omega*tau) ;
terdiurnal2=(coeff1(50)*asl_33+(coeff1(51)*asl_43+coeff1(52)*asl_63)*cos(o_d*(t_d-coeff1(30))))*F2*sin(3*omega*tau) ;
          terdiurnal_func= terdiurnal1 +terdiurnal2 ;
	  }
	  else
             if(tableno ==2)     
          {
terdiurnal1=(coeff2(26)*asl_33+(0*asl_43+0*asl_63)*cos(o_d*(t_d-coeff2(15))))*F2*cos(3*omega*tau) ;
terdiurnal2=(coeff2(27)*asl_33+(0*asl_43+0*asl_63)*cos(o_d*(t_d-coeff2(15))))*F2*sin(3*omega*tau) ;
          terdiurnal_func= terdiurnal1 +terdiurnal2 ;
          }
	  return terdiurnal_func ;
	  
      } catch( AipsError x ) {
       cerr << "inverseFFT::semidiurnal" <<x.getMesg() << endl ;
    } 
    
   // return terdiurnal_func ;
 }
 
 
 
 
  Double inverseFFT::magnetic( const uint& tableno,
                               const uint& col,
                               const Double& t_d,
                               const Double& tau,
                               const Double& Ap,
                               const Double& latitude ) 
  {				     				     
     try {
          inverseFFT inFFT ; 
	  Double mag1(0.0) ;
	  Double mag2(0.0) ;
	  Double magnetic_func(0.0) ;
	  
	  Double pi = 3.1416 ;
	  Double o_d = 2*pi/365. ;
	  Double omega = 2*pi/24. ;
	 
          Matrix<Double> mcoeff1( 106, 7, 0.0) ;
	  Matrix<Double> mcoeff2( 27, 6, 0.0 ) ;
	
	  readAsciiMatrix( mcoeff1, "modelcoeff1.dat" ) ;
	  readAsciiMatrix( mcoeff2, "modelcoeff2.dat" ) ;
	
	  Vector<Double> coeff1 = mcoeff1.column(col) ;
	  Vector<Double> coeff2 = mcoeff2.column(col) ;
	
	  Double del_A(0.0);
	  
	  Double asl_20 = inFFT.legendre(latitude, 2, 0) ;
	  Double asl_40 = inFFT.legendre(latitude, 4, 0) ;
	  Double asl_10 = inFFT.legendre(latitude, 1, 0) ;
	  Double asl_11 = inFFT.legendre(latitude, 1, 1) ;
	  Double asl_30 = inFFT.legendre(latitude, 3, 0) ;
	  Double asl_31 = inFFT.legendre(latitude, 3, 1) ;
	  Double asl_50 = inFFT.legendre(latitude, 5, 0) ;
	  Double asl_51 = inFFT.legendre(latitude, 5, 1) ;
	  
	
	  if( tableno ==1)
	  {
	  del_A = (Ap-4)+(coeff1(94)-1)*(Ap-4+(exp(-coeff1(93)*(Ap-4))-1)/coeff1(93)) ;
	  
mag1 =(coeff1(12)*asl_20+coeff1(13)*asl_40+(coeff1(14)*asl_10+coeff1(15)*asl_30+coeff1(60)*asl_50)*cos(o_d*(t_d-coeff1(30))))*del_A ;
mag2 =(coeff1(11)+(coeff1(17)*asl_11+coeff1(18)*asl_31+coeff1(19)*asl_51)*cos(omega*(tau-coeff1(20))))*del_A ;
          magnetic_func= mag1 +mag2 ;
	  }
	  else
             if(tableno ==2)     
            { 
	     del_A = (Ap-4)+(coeff2(8)-1)*(Ap-4+ (exp(-coeff2(7)*(Ap-4))-1)/coeff2(7)) ;
	     
             mag1=(coeff2(5)+coeff2(6)*asl_20+0*asl_40+(0*asl_10+0*asl_30+0*asl_50)*cos(o_d*(t_d-coeff2(15))))*del_A ;
             mag2=0 ;
           magnetic_func= mag1 +mag2 ;
          }
         return magnetic_func ;
	 
     } catch( AipsError x ) {
      cerr << "inverseFFT::magnetic" <<x.getMesg() << endl ;
    } 
    
   // return magnetic_func ;
 }
 
 
 
 		       
 			       
 
  Double inverseFFT::longitudinal( const Double& F107,
                                   const Double& mean_F107,
                                   const uint& tableno,
                                   const uint& col,
                                   const Double& t_d,
                                   const Double& tau,
                                   const Double& latitude,
                                   const Double& longitude )
				   
  {				     				     
     try {
          inverseFFT inFFT ; 
	  
	  Double long1(0.0) ;
	  Double long2(0.0) ;
	  Double long3(0.0) ;
	  Double long4(0.0) ;
	  
	  Double long_func(0.0) ;
	  
	  Double pi = 3.1416 ;
	  Double o_d = 2*pi/365. ;
	  Double omega = 2*pi/24. ;
	 
          Double del_F = F107 -mean_F107 ;
	  Double del_mean_F = mean_F107 -150 ;
		
	  Matrix<Double> mcoeff1( 106, 7, 0.0) ;
	  Matrix<Double> mcoeff2( 27, 6, 0.0 ) ;
	
	  readAsciiMatrix( mcoeff1, "modelcoeff1.dat" ) ;
	  readAsciiMatrix( mcoeff2, "modelcoeff2.dat" ) ;
	
	  Vector<Double> coeff1 = mcoeff1.column(col) ;
	  Vector<Double> coeff2 = mcoeff2.column(col) ;
	
	  Double del_A(0.0);
	  
	  Double asl_21 = inFFT.legendre(latitude, 2, 1) ;
	  Double asl_41 = inFFT.legendre(latitude, 4, 1) ;
	  Double asl_61 = inFFT.legendre(latitude, 6, 1) ;
	  Double asl_11 = inFFT.legendre(latitude, 1, 1) ;
	  Double asl_31 = inFFT.legendre(latitude, 3, 1) ;
	  Double asl_51 = inFFT.legendre(latitude, 5, 1) ;
	  
	
	  if( tableno ==1)
	     {
long1=(coeff1(59)*asl_11+coeff1(60)*asl_31)*cos(omega*(t_d-coeff1(30))) ;	    
long2= coeff1(53)*asl_21+coeff1(54)*asl_41+coeff1(55)*asl_61+coeff1(56)*asl_11+coeff1(57)*asl_31+coeff1(58)*asl_51 ;
long3= coeff1(61)*asl_21+coeff1(62)*asl_41+coeff1(63)*asl_61+coeff1(64)*asl_11+coeff1(65)*asl_31+coeff1(66)*asl_51 ;
long4= (coeff1(67)*asl_11+coeff1(68)*asl_31)*cos(o_d*(t_d-coeff1(30))) ;
long_func=(long2+long1)*(1+coeff1(69)*del_mean_F)*cos(longitude*pi/180.)+(long3+long4)*(1+coeff1(69)*del_mean_F)*sin(longitude*pi/180.) ;
	   }
	  else
             if(tableno ==2)     
             { 
	     long_func =0.0 ;
          }
	  return long_func ;
	  
     } catch( AipsError x ) {
      cerr << "inverseFFT::longitudinal" <<x.getMesg() << endl ;
    } 
    
  //  return long_func ;
 }
 
 
 
 					   
 Double inverseFFT::UT( const Double& F107,
                        const Double& mean_F107,
                        const uint& tableno,
                        const uint& col,
                        const Double& t_d,
                        const Double& tau,
                        const Double& t,
                        const Double& latitude,
                        const Double& longitude ) 
			
  {				     				     
     try {
          inverseFFT inFFT ; 
	  
	  Double UT1(0.0) ;
	  Double UT2(0.0) ;
	  Double UT3(0.0) ;
	    
	  Double UT_func(0.0) ;
	  
	  Double pi = 3.1416 ;
	  Double o_d = 2*pi/365. ;
	  Double omega = 2*pi/24. ;
	  Double omega_dash = 2*pi/86400. ;
	  
          Double del_F = F107 -mean_F107 ;
	  Double del_mean_F = mean_F107 -150 ;
		
	  Matrix<Double> mcoeff1( 106, 7, 0.0) ;
	  Matrix<Double> mcoeff2( 27, 6, 0.0 ) ;
	
	  readAsciiMatrix( mcoeff1, "modelcoeff1.dat" ) ;
	  readAsciiMatrix( mcoeff2, "modelcoeff2.dat" ) ;
	
	  Vector<Double> coeff1 = mcoeff1.column(col) ;
	  Vector<Double> coeff2 = mcoeff2.column(col) ;
	
	  Double del_A(0.0);
	  
	  Double asl_10 = inFFT.legendre(latitude, 1, 0) ;
	  Double asl_30 = inFFT.legendre(latitude, 3, 0) ;
	  Double asl_50 = inFFT.legendre(latitude, 5, 0) ;
	  Double asl_32 = inFFT.legendre(latitude, 3, 2) ;
	  Double asl_52 = inFFT.legendre(latitude, 5, 2) ;
	   
	
	  if( tableno ==1)
	    {
           UT1=(coeff1(70)*asl_10+coeff1(71)*asl_30+coeff1(72)*asl_50)*(1+coeff1(95)*del_mean_F)*(1+coeff1(74)*asl_10) ;
           UT2= (1+coeff1(75)*cos(o_d*(t_d-coeff1(30))))*cos(omega_dash*(t-coeff1(73))) ;
           UT3= (coeff1(76)*asl_32+coeff1(77)*asl_52)*cos(omega_dash*(t-coeff1(78))+2*longitude*pi/180.) ;
           UT_func= UT1*UT2 +UT3 ;
	  }
	  else
             if(tableno ==2)     
             {  
	     UT_func =0.0 ;
	     }
	     return UT_func ;
	     
     } catch( AipsError x ) {
      cerr << "inverseFFT::UT" <<x.getMesg() << endl ;
    } 
    
  //  return UT_func ;
 }
 
 	

  
  Double inverseFFT::combined( const Double& F107,
                               const Double& mean_F107,
                               const uint& tableno,
                               const uint& col,
                               const Double& t_d,
                               const Double& tau,
                               const Double& t,
                               const Double& Ap,
                               const Double& latitude,
                               const Double& longitude ) 
  
  {				     				     
     try {
           inverseFFT inFFT ; 
	  
	  Double com1(0.0) ;
	  Double com2(0.0) ;
	  Double com3(0.0) ;
	    
	  Double combined_func(0.0) ;
	  
	  Double pi = 3.1416 ;
	  Double o_d = 2*pi/365. ;
	  Double omega = 2*pi/24. ;
	  Double omega_dash = 2*pi/86400. ;
	  
          Double del_F = F107 -mean_F107 ;
	  Double del_mean_F = mean_F107 -150 ;
		
	  Matrix<Double> mcoeff1( 106, 7, 0.0) ;
	  Matrix<Double> mcoeff2( 27, 6, 0.0 ) ;
	
	  readAsciiMatrix( mcoeff1, "modelcoeff1.dat" ) ;
	  readAsciiMatrix( mcoeff2, "modelcoeff2.dat" ) ;
	
	  Vector<Double> coeff1 = mcoeff1.column(col) ;
	  Vector<Double> coeff2 = mcoeff2.column(col) ;
	
	  Double del_A(0.0);
	  
	  Double asl_21 = inFFT.legendre(latitude, 2, 1) ;
	  Double asl_41 = inFFT.legendre(latitude, 4, 1) ;
	  Double asl_61 = inFFT.legendre(latitude, 6, 1) ;
	  Double asl_10 = inFFT.legendre(latitude, 1, 0) ;
	  Double asl_11 = inFFT.legendre(latitude, 1, 1) ;
	  Double asl_30 = inFFT.legendre(latitude, 3, 0) ; 
	  Double asl_50 = inFFT.legendre(latitude, 5, 0) ;
	  
	  if( tableno ==1)
	   {
	   del_A = (Ap-4)+(coeff2(8)-1)*(Ap-4+ (exp(-coeff2(7)*(Ap-4))-1)/coeff2(7)) ;
	   
           com1=(coeff1(96)*asl_21+coeff1(97)*asl_41+coeff1(98)*asl_61)*(1+coeff1(100)*asl_10)*del_A*cos((longitude-coeff1(99))*pi/180.) ;
           com2=coeff1(101)*asl_11*cos(o_d*(t_d-coeff1(30)))*del_A*cos((longitude-coeff1(106))*pi/180.) ;
           com3=(coeff1(101)*asl_10+coeff1(102)*asl_30+coeff1(103)*asl_50)*del_A*cos(omega_dash*(t-coeff1(104))) ;
           combined_func= com1+com2 +com3 ;
	  }
	  else
             if(tableno ==2)     
             {
	     combined_func =0.0 ;
	     }
	     return combined_func ;
	     
     } catch( AipsError x ) {
      cerr << "inverseFFT::combined" <<x.getMesg() << endl ;
    } 
    
 //   return combined_func ;
 }
 
 	


 Double inverseFFT::expansion( const Double& F107,
                               const Double& mean_F107,
                               const uint& tableno,
                               const uint& col,
                               const Double& t_d,
                               const Double& tau,
                               const Double& t,
                               const Double& Ap,
                               const Double& latitude,
                               const Double& longitude ) 
		      
  {				     				     
     try {
          inverseFFT inFFT ; 
	  
	  Double expansion1(0.0) ;
	  Double expansion2(0.0) ;
	      
	  Double expansion_func(0.0) ;
		
	  Matrix<Double> mcoeff1( 106, 7, 0.0) ;
	  Matrix<Double> mcoeff2( 27, 6, 0.0 ) ;
	
	  readAsciiMatrix( mcoeff1, "modelcoeff1.dat" ) ;
	  readAsciiMatrix( mcoeff2, "modelcoeff2.dat" ) ;
	
	  Vector<Double> coeff1 = mcoeff1.column(col) ;
	  Vector<Double> coeff2 = mcoeff2.column(col) ;
	
	  Double asl_10 = inFFT.legendre(latitude, 1, 0) ;
	  Double asl_20 = inFFT.legendre(latitude, 2, 0) ; 
	  Double asl_40 = inFFT.legendre(latitude, 4, 0) ;
	  
	  Double solar_fun = inFFT.solar_activity(F107, mean_F107, tableno, col, latitude) ;
	  Double sym_fun = inFFT.symmetrical(tableno, col, t_d, latitude ) ;
	  Double asym_fun = inFFT.asymmetrical(F107, mean_F107, tableno, col, t_d, latitude ) ;
	  Double diurnal_fun =inFFT.diurnal(F107, mean_F107, tableno, col, t_d, tau, latitude) ;
	  Double semidiurnal_fun = inFFT.semidiurnal(F107, mean_F107, tableno, col, t_d, tau, latitude ) ;
	  Double terdiurnal_fun = inFFT.terdiurnal( F107, mean_F107, tableno, col, t_d, tau, latitude ) ;
	  Double magnetic_fun = inFFT.magnetic( tableno, col, t_d, tau, Ap, latitude ) ;
	  Double longitudinal_fun = inFFT.longitudinal( F107, mean_F107, tableno, col, t_d, tau, latitude, longitude ) ;
	  Double UT_fun = inFFT.UT( F107, mean_F107, tableno, col, t_d, tau, t, latitude, longitude ) ;
	  Double combined_fun = inFFT.combined( F107, mean_F107, tableno, col, t_d, tau, t, Ap, latitude, longitude ) ;
	  
	  if( tableno ==1)
	   {
	   expansion1 = coeff1(2)*asl_10+coeff1(3)*asl_20+coeff1(4)*asl_40+solar_fun+sym_fun+asym_fun ;
	   expansion2 = diurnal_fun+semidiurnal_fun+terdiurnal_fun+magnetic_fun+longitudinal_fun+UT_fun+combined_fun ;
           expansion_func= expansion1+expansion2 ;
	   }
	  else
             if(tableno ==2)     
             {
	     expansion1 = 0*asl_10+coeff2(2)*asl_20+coeff2(3)*asl_40+solar_fun+sym_fun+asym_fun ;
	     expansion2 = diurnal_fun+semidiurnal_fun+terdiurnal_fun+magnetic_fun ;
             expansion_func= expansion1+expansion2 ;
	     }
	  return expansion_func ;
	  
     } catch( AipsError x ) {
      cerr << "inverseFFT::expansion" <<x.getMesg() << endl ;
    } 
    
  //  return expansion_func ;
 }
 
 
 
 
 		      
		      		     
 Double inverseFFT::temperature( const Double& F107,
                                 const Double& mean_F107,
                                 const uint& tableno,
                                 const uint& col,
                                 const Double& t_d,
                                 const Double& tau,
                                 const Double& t,
                                 const Double& Ap,
                                 const Double& latitude,
                                 const Double& longitude,
                                 const Double& altitude ) 

  {				     				     
     try {
          inverseFFT inFFT ; 
	  
	  Double temp_profile(0.0) ;
	  
          Matrix<Double> mcoeff1( 106, 7, 0.0) ;
	  Matrix<Double> mcoeff2( 27, 6, 0.0 ) ;
	
	  readAsciiMatrix( mcoeff1, "modelcoeff1.dat" ) ;
	  readAsciiMatrix( mcoeff2, "modelcoeff2.dat" ) ;
	
	  Vector<Double> coeff1 = mcoeff1.column(col) ;
	  Vector<Double> coeff2 = mcoeff2.column(col) ;
	  
	  Double temp_l_dash_G_L = inFFT.expansion(F107, mean_F107, 2, 5, t_d, tau, t, Ap, latitude, longitude) ;
	  Double temp_inf_G_L = inFFT.expansion(F107, mean_F107, 1, 0, t_d, tau, t, Ap, latitude, longitude) ;
	  Double temp_l_G_L = inFFT.expansion(F107, mean_F107, 2, 1, t_d, tau, t, Ap, latitude, longitude) ;
	  Double temp_0_G_L = inFFT.expansion(F107, mean_F107, 2, 3, t_d, tau, t, Ap, latitude, longitude) ;
	  Double z_0_G_L = inFFT.expansion(F107, mean_F107, 2, 2, t_d, tau, t, Ap, latitude, longitude) ;
	  Double temp_R_G_L = inFFT.expansion(F107, mean_F107, 2, 4, t_d, tau, t, Ap, latitude, longitude) ;
	  
	  Double temp_l_dash = mcoeff2(1,5)*(1+temp_l_dash_G_L);
	  Double temp_inf = mcoeff1(1,0)*(1+temp_inf_G_L);
	  Double temp_l = mcoeff2(1,1)*(1+temp_l_G_L) ;
	  Double temp_0 = mcoeff2(1,3)*(1+temp_0_G_L) ;
	  Double z_0 = mcoeff2(1,2)*(1+z_0_G_L) ;
	  Double temp_R = mcoeff2(1,4)*(1+temp_R_G_L ) ;
	  
	  Double sigma = temp_l_dash/(temp_inf -temp_l ) ;
	  
	  Double zl = 120. ;
	  Double za = 117.2 ;
	  Double Rp = 6356.77 ;
	  
	  Double eta_z_zl = (altitude-zl)*(Rp+zl)/(Rp+altitude) ;
	  Double eta_z_za =(altitude-za)*(Rp+za)/(Rp+altitude) ;
	  Double eta_za_zl = (za-zl)*(Rp+zl)/(Rp+za) ;
	  Double eta_z0_za = (z_0-za)*(Rp+za)/(Rp+z_0) ;
	  
	  Double x =-(eta_z_za-eta_z0_za)/eta_z0_za ;
	  
	  Double temp_a = temp_inf-(temp_inf-temp_l)*exp(-sigma*eta_za_zl);
	  Double temp_a_dash =(temp_inf-temp_a)*sigma*square((Rp+zl)/(Rp+za)) ;
	  Double temp_12 = temp_0+temp_R*(temp_a-temp_0) ;
	  Double temp_D = 0.666666*eta_z0_za*temp_a_dash/square(temp_a)-3.111111*(1./temp_a-1./temp_0)+7.11111*(1./temp_12-1./temp_0) ;
	  Double temp_C = eta_z0_za*temp_a_dash/(2*square(temp_a))-(1./temp_a-1./temp_0)-2*temp_D ;
	  Double temp_B = (1./temp_a-1./temp_0)-temp_C - temp_D ;
	  
	  temp_profile =1./(1./temp_0+temp_B*square(x)+temp_C*square(x)*square(x)+temp_D*cube(x)*cube(x)) ;
	  
	  return temp_profile ;
	  
     } catch( AipsError x ) {
      cerr << "inverseFFT::temperature" <<x.getMesg() << endl ;
    } 
    
  //  return temp_profile ;
 }
 
 	

  
 Double inverseFFT::Dprofile( const Double& F107,
                              const Double& mean_F107,
                              const uint& tableno,
                              const uint& col,
                              const Double& t_d,
                              const Double& tau,
                              const Double& t,
                              const Double& Ap,
                              const Double& latitude,
                              const Double& longitude,
                              const Double& altitude,
                              const Double& molecular_weight ) 
   {				     				     
     try {
           inverseFFT inFFT ; 
	   
	   Double D_function(0.0) ;
	   
	   Double gs = 9.80665e-3 ;
	   
	   Double zl = 120. ;
	   
	   Double za = 117.2 ;
	   
	   Double Rp = 6356.77 ;
	   
	   Double Rg = 8.314e-3 ;
	   
	   Double gl = gs/square(1+zl/Rp) ;
	   
	   Double ga = gs*square(1+za/Rp) ;
	   
	   Matrix<Double> mcoeff1( 106, 7, 0.0) ;
	   Matrix<Double> mcoeff2( 27, 6, 0.0 ) ;
	
	   readAsciiMatrix( mcoeff1, "modelcoeff1.dat" ) ;
	   readAsciiMatrix( mcoeff2, "modelcoeff2.dat" ) ;
	
	   Vector<Double> coeff1 = mcoeff1.column(col) ;
	   Vector<Double> coeff2 = mcoeff2.column(col) ;
	   
	   Double temp_l_dash_G_L = inFFT.expansion(F107, mean_F107, 2, 5, t_d, tau, t, Ap, latitude, longitude) ;
	   Double temp_inf_G_L = inFFT.expansion(F107, mean_F107, 1, 0, t_d, tau, t, Ap, latitude, longitude) ;
	   Double temp_l_G_L = inFFT.expansion(F107, mean_F107, 2, 1, t_d, tau, t, Ap, latitude, longitude) ;
	   Double temp_0_G_L = inFFT.expansion(F107, mean_F107, 2, 3, t_d, tau, t, Ap, latitude, longitude) ;
	   Double z_0_G_L = inFFT.expansion(F107, mean_F107, 2, 2, t_d, tau, t, Ap, latitude, longitude) ;
	   Double temp_R_G_L = inFFT.expansion(F107, mean_F107, 2, 4, t_d, tau, t, Ap, latitude, longitude) ;
	  
	   Double temp_l_dash = mcoeff2(1,6)*(1+temp_l_dash_G_L);
	   Double temp_inf = mcoeff1(1,0)*(1+temp_inf_G_L);
	   Double temp_l = mcoeff2(1,1)*(1+temp_l_G_L) ;
	   Double temp_0 = mcoeff2(1,3)*(1+temp_0_G_L) ;
	   Double z_0 = mcoeff2(1,2)*(1+z_0_G_L) ;
	   Double temp_R = mcoeff2(1,4)*(1+temp_R_G_L ) ;
	   
	   Double eta_z_zl = (altitude-zl)*(Rp+zl)/(Rp+altitude) ;
	   Double eta_z_za =(altitude-za)*(Rp+za)/(Rp+altitude) ;
	   Double eta_za_zl = (za-zl)*(Rp+zl)/(Rp+za) ;
	   Double eta_z0_za = (z_0-za)*(Rp+za)/(Rp+z_0) ;
	  
	   Double x =-(eta_z_za-eta_z0_za)/eta_z0_za ;
	   
	   Double temp_zl =inFFT.temperature(F107, mean_F107, tableno, col, t_d, tau, t, Ap, latitude, longitude, zl) ;
	   Double temp_za =inFFT.temperature(F107, mean_F107, tableno, col, t_d, tau, t, Ap, latitude, longitude, za) ;
	   Double temp_z =inFFT.temperature(F107, mean_F107, tableno, col, t_d, tau, t, Ap, latitude, longitude, altitude) ; 
	   	   
	   Double sigma = temp_l_dash/(temp_inf -temp_l ) ;
	   Double temp_a = temp_inf -(temp_inf-temp_l)*exp(-sigma*eta_za_zl);
	   Double temp_a_dash =(temp_inf-temp_a)*sigma*square((Rp+zl)/(Rp+za)) ;
	   Double temp_12 = temp_0+temp_R*(temp_a-temp_0) ;
	   Double temp_D = 0.666666*eta_z0_za*temp_a_dash/square(temp_a)-3.111111*(1./temp_a-1./temp_0)+7.11111*(1./temp_12-1./temp_0) ;
	   Double temp_C = eta_z0_za*temp_a_dash/(2*square(temp_a))-(1./temp_a-1./temp_0)-2*temp_D ;
	   Double temp_B = (1./temp_a-1./temp_0)-temp_C - temp_D ;
	   
	   Double gamma_2 = molecular_weight*gl/(sigma*Rg*temp_inf) ;
	   Double gamma_1 = molecular_weight*ga*eta_z0_za/Rg ;
	   Double alpha(0.0) ;
	   
	   if(((tableno==1)&&(col==1))||((tableno==1)&&(col==5)))
	   
	      alpha = 0.4 ;
	      
	      else
	          
		  alpha = 0.0 ;
		  
          Double arguement = (temp_zl/temp_z) ;  
          
          Double D_B_pow = pow(arguement, gamma_2) ;
	    
	  Double D_B_z_M = D_B_pow*exp(-alpha*gamma_2*eta_z_zl) ;	  
		  
 D_function = D_B_z_M*exp(gamma_1*((x-1)/temp_0+temp_B*(cube(x)-1)/3+temp_C*(square(x)*cube(x)-1)/5+temp_D*(cube(x)*cube(x)*x-1)/7)) ;
	  
         return D_function ; 
	 
     } catch( AipsError x ) {
      cerr << "inverseFFT::Dprofile" <<x.getMesg() << endl ;
    } 
    
 //   return D_function ;
 }
 
 	

    
  Double inverseFFT::N2densityprofile( const Double& F107,
				       const Double& mean_F107,
				       const Double& t_d,
				       const Double& tau,
				       const Double& t,
				       const Double& Ap,
				       const Double& latitude,
				       const Double& longitude,
				       const Double& altitude ) 
    
  {				     				     
    Double N2densityprofile(0.0) ;

    try {
      
      inverseFFT inFFT ;
      
      Double alpha(0.0) ;
      uint tableno(2) ;
      uint col(0) ;
      Double molecular_weight(28.0) ;
      Double n_l_average(3.004e+17) ;
      
      if(((tableno==1)&&(col==1))||((tableno==1)&&(col==5)))
	
	alpha = 0.4 ;
      
      else
	alpha = 0.0 ;
      
      Double A = 28./(28.95-molecular_weight) ;
      
      Double zh = 105. ;
      Double zl = 120. ;
      
      Double G_L = inFFT.expansion( F107, mean_F107, tableno, col, t_d, tau, t, Ap, latitude, longitude ) ;
      Double n_l = n_l_average*exp(G_L) ;
      Double D_z_M = inFFT.Dprofile( F107,mean_F107,tableno,col,t_d,tau,t,Ap,latitude,longitude,altitude,molecular_weight) ;
      
      Double temp_zl = inFFT.temperature(F107, mean_F107, tableno, col, t_d, tau, t, Ap, latitude, longitude, zl ) ;
      Double temp_zh = inFFT.temperature(F107, mean_F107, tableno, col, t_d, tau, t, Ap, latitude, longitude, zh ) ;
      Double temp_z = inFFT.temperature(F107, mean_F107, tableno, col, t_d, tau, t, Ap, latitude, longitude, altitude ) ;
      
      Double arguement1 = (temp_zl/temp_z) ;
      Double pow_factor1 =pow(arguement1, (1+alpha));
      
      Double nd_z_M = n_l*D_z_M*pow_factor1 ;   // Diffusive profile
      
      Double D_zh_M = inFFT.Dprofile( F107,mean_F107,tableno,col,t_d,tau,t,Ap,latitude,longitude,zh,molecular_weight) ;
      
      //mixing profile
      Double D_zh_M0 =inFFT.Dprofile( F107,mean_F107,tableno,col,t_d,tau,t,Ap,latitude,longitude,zh,28.95) ;	  
      Double D_z_M0 =inFFT.Dprofile( F107,mean_F107,tableno,col,t_d,tau,t,Ap,latitude,longitude,altitude,28.95) ;
      
      Double arguement2 = (temp_zl/temp_zh) ;
      Double pow_factor2 =pow(arguement2, alpha);
      
      Double nm_z_M =n_l*(D_zh_M/D_zh_M0)*(pow_factor2*D_z_M0*arguement1) ;
      
      // mixing profiles have been omitted right now
      Double power_diffusive = pow(nd_z_M, A) ;
      Double power_mixing = pow(nm_z_M, A) ;
      
      N2densityprofile = pow((power_diffusive+power_mixing),(1/A)) ;
      
    } catch( AipsError x ) {
      cerr << "inverseFFT::N2densityprofile" <<x.getMesg() << endl ;
      return .0;
    } 
    
    return N2densityprofile ;
  }

  Double inverseFFT::O2densityprofile( const Double& F107,
                           	      const Double& mean_F107,
                                      const Double& t_d,
                                      const Double& tau,
                                      const Double& t,
                                      const Double& Ap,
                                      const Double& latitude,
                                      const Double& longitude,
                                      const Double& altitude ) 
				      
 {				     				     
   Double O2densityprofile(0.0) ;
   
   try {
     
     inverseFFT inFFT ;
     
     Double alpha(0.0) ;
     uint tableno(1) ;
     uint col(3) ;
     Double molecular_weight(32.0) ;
     Double n_l_average(3.074e+16) ;
     
     if(((tableno==1)&&(col==1))||((tableno==1)&&(col==5)))
       
       alpha = 0.4 ;
     
     else
       alpha = 0.0 ;
     
     Double A = 28./(28.95-molecular_weight) ;
     
     Double zh = 105. ;
     Double zl = 120. ;
     
     Double G_L = inFFT.expansion( F107, mean_F107, tableno, col, t_d, tau, t, Ap, latitude, longitude ) ;
     Double n_l = n_l_average*exp(G_L) ;
     Double D_z_M = inFFT.Dprofile( F107,mean_F107,tableno,col,t_d,tau,t,Ap,latitude,longitude,altitude,molecular_weight) ;
     
     Double temp_zl = inFFT.temperature(F107, mean_F107, tableno, col, t_d, tau, t, Ap, latitude, longitude, zl ) ;
     Double temp_zh = inFFT.temperature(F107, mean_F107, tableno, col, t_d, tau, t, Ap, latitude, longitude, zh ) ;
     Double temp_z = inFFT.temperature(F107, mean_F107, tableno, col, t_d, tau, t, Ap, latitude, longitude, altitude ) ;
	
     Double arguement1 = (temp_zl/temp_z) ;
     Double pow_factor1 =pow(arguement1, (1+alpha));
     
     Double nd_z_M = n_l*D_z_M*pow_factor1 ;   // Diffusive profile
     
     Double D_zh_M = inFFT.Dprofile( F107,mean_F107,tableno,col,t_d,tau,t,Ap,latitude,longitude,zh,molecular_weight) ;
     
     //mixing profile
     Double D_zh_M0 =inFFT.Dprofile( F107,mean_F107,tableno,col,t_d,tau,t,Ap,latitude,longitude,zh,28.95) ;	  
     Double D_z_M0 =inFFT.Dprofile( F107,mean_F107,tableno,col,t_d,tau,t,Ap,latitude,longitude,altitude,28.95) ;
     
     Double arguement2 = (temp_zl/temp_zh) ;
     Double pow_factor2 =pow(arguement2, alpha);
     
     Double nm_z_M =n_l*(D_zh_M/D_zh_M0)*(pow_factor2*D_z_M0*arguement1) ;
	
     // mixing profiles have been omitted right now
     Double power_diffusive = pow(nd_z_M, A) ;
     Double power_mixing = pow(nm_z_M, A) ;
     
     O2densityprofile = pow((power_diffusive+power_mixing),(1/A)) ;
     
   } catch( AipsError x ) {
     cerr << "inverseFFT::O2densityprofile" <<x.getMesg() << endl ;
     return .0;
   } 
   
   return O2densityprofile ;
 }
 
 	

  				      
  
  Double inverseFFT::Ardensityprofile( const Double& F107,
                           	       const Double& mean_F107,
                                       const Double& t_d,
                                       const Double& tau,
                                       const Double& t,
                                       const Double& Ap,
                                       const Double& latitude,
                                       const Double& longitude,
                                       const Double& altitude ) 
				       
  {				     				     
    Double Ardensityprofile(0.0) ;

    try {
      
      inverseFFT inFFT ;
      
      Double alpha(0.0) ;
      uint tableno(1) ;
      uint col(4) ;
      Double molecular_weight(39.948) ;
      Double n_l_average(1.155e+15) ;
      
      if(((tableno==1)&&(col==1))||((tableno==1)&&(col==5)))
	
	alpha = 0.4 ;
      
      else
	alpha = 0.0 ;
      
      Double A = 28./(28.95-molecular_weight) ;
      
      Double zh = 105. ;
      Double zl = 120. ;
      
      Double G_L = inFFT.expansion( F107, mean_F107, tableno, col, t_d, tau, t, Ap, latitude, longitude ) ;
      Double n_l = n_l_average*exp(G_L) ;
      Double D_z_M = inFFT.Dprofile( F107,mean_F107,tableno,col,t_d,tau,t,Ap,latitude,longitude,altitude,molecular_weight) ;
      
      Double temp_zl = inFFT.temperature(F107, mean_F107, tableno, col, t_d, tau, t, Ap, latitude, longitude, zl ) ;
      Double temp_zh = inFFT.temperature(F107, mean_F107, tableno, col, t_d, tau, t, Ap, latitude, longitude, zh ) ;
      Double temp_z = inFFT.temperature(F107, mean_F107, tableno, col, t_d, tau, t, Ap, latitude, longitude, altitude ) ;
      
      Double arguement1 = (temp_zl/temp_z) ;
      Double pow_factor1 =pow(arguement1, (1+alpha));
      
      Double nd_z_M = n_l*D_z_M*pow_factor1 ;   // Diffusive profile
      
      Double D_zh_M = inFFT.Dprofile( F107,mean_F107,tableno,col,t_d,tau,t,Ap,latitude,longitude,zh,molecular_weight) ;
      
      //mixing profile
      Double D_zh_M0 =inFFT.Dprofile( F107,mean_F107,tableno,col,t_d,tau,t,Ap,latitude,longitude,zh,28.95) ;	  
      Double D_z_M0 =inFFT.Dprofile( F107,mean_F107,tableno,col,t_d,tau,t,Ap,latitude,longitude,altitude,28.95) ;
      
      Double arguement2 = (temp_zl/temp_zh) ;
      Double pow_factor2 =pow(arguement2, alpha);
      
      Double nm_z_M =n_l*(D_zh_M/D_zh_M0)*(pow_factor2*D_z_M0*arguement1) ;
      
      // mixing profiles have been omitted right now
      Double power_diffusive = pow(nd_z_M, A) ;
      Double power_mixing = pow(nm_z_M, A) ;
      
      Ardensityprofile = pow((power_diffusive+power_mixing),(1/A)) ;
      
    } catch( AipsError x ) {
      cerr << "inverseFFT::Ardensityprofile" <<x.getMesg() << endl ;
      return .0;
    } 
    
    return Ardensityprofile ;
 }
 
 	

		   
 Double inverseFFT::Odensityprofile( const Double& F107,
                           	     const Double& mean_F107,
                                     const Double& t_d,
                                     const Double& tau,
                                     const Double& t,
                                     const Double& Ap,
                                     const Double& latitude,
                                     const Double& longitude,
                                     const Double& altitude ) 
				       
 {				     				     
     try {
       
           inverseFFT inFFT ;
	 
	 Double Odensityprofile(0.0) ;
	 Double alpha(0.0) ;
	 uint tableno(1) ;
	 uint col(2) ;
	 Double molecular_weight(16.00) ;
	 Double n_l_average(1.155e+15) ;
	  
	 if(((tableno==1)&&(col==1))||((tableno==1)&&(col==5)))
	   
	      alpha = 0.4 ;
	      
	      else
	          alpha = 0.0 ;
	
		Double A = 28./(28.95-molecular_weight) ;
	
        Double zh = 105. ;
	Double zl = 120. ;
	
	Double G_L = inFFT.expansion( F107, mean_F107, tableno, col, t_d, tau, t, Ap, latitude, longitude ) ;
	Double n_l = n_l_average*exp(G_L) ;
	Double D_z_M = inFFT.Dprofile( F107,mean_F107,tableno,col,t_d,tau,t,Ap,latitude,longitude,altitude,molecular_weight) ;
	
	Double temp_zl = inFFT.temperature(F107, mean_F107, tableno, col, t_d, tau, t, Ap, latitude, longitude, zl ) ;
	Double temp_zh = inFFT.temperature(F107, mean_F107, tableno, col, t_d, tau, t, Ap, latitude, longitude, zh ) ;
	Double temp_z = inFFT.temperature(F107, mean_F107, tableno, col, t_d, tau, t, Ap, latitude, longitude, altitude ) ;
	
	Double arguement1 = (temp_zl/temp_z) ;
	Double pow_factor1 =pow(arguement1, (1+alpha));
	
	Double nd_z_M = n_l*D_z_M*pow_factor1 ;   // Diffusive profile
	
	Double D_zh_M = inFFT.Dprofile( F107,mean_F107,tableno,col,t_d,tau,t,Ap,latitude,longitude,zh,molecular_weight) ;
	
	//mixing profile
	Double D_zh_M0 =inFFT.Dprofile( F107,mean_F107,tableno,col,t_d,tau,t,Ap,latitude,longitude,zh,28.95) ;	  
	Double D_z_M0 =inFFT.Dprofile( F107,mean_F107,tableno,col,t_d,tau,t,Ap,latitude,longitude,altitude,28.95) ;
	
	Double arguement2 = (temp_zl/temp_zh) ;
	Double pow_factor2 =pow(arguement2, alpha);
	
	Double nm_z_M =n_l*(D_zh_M/D_zh_M0)*(pow_factor2*D_z_M0*arguement1) ;
	
	// mixing profiles have been omitted right now
	 Double power_diffusive = pow(nd_z_M, A) ;
	 Double power_mixing = pow(nm_z_M, A) ;
	 
	 Odensityprofile = pow((power_diffusive+power_mixing),(1/A)) ;
       
       return Odensityprofile ;
       
     } catch( AipsError x ) {
      cerr << "inverseFFT::Odensityprofile" <<x.getMesg() << endl ;
    } 
    
   // return Odensityprofile ;
 }
 
 
 Double inverseFFT::Ndensityprofile( const Double& F107,
                           	     const Double& mean_F107,
                                     const Double& t_d,
                                     const Double& tau,
                                     const Double& t,
                                     const Double& Ap,
                                     const Double& latitude,
                                     const Double& longitude,
                                     const Double& altitude ) 
				       
 {
   try {
     
     inverseFFT inFFT ;
     
     Double Ndensityprofile(0.0) ;
     Double alpha(0.0) ;
     uint tableno(1) ;
     uint col(6) ;
     Double molecular_weight(14.00) ;
     Double n_l_average(5.736e+14) ;
     
     if(((tableno==1)&&(col==1))||((tableno==1)&&(col==5)))
       
       alpha = 0.4 ;
     
     else
       alpha = 0.0 ;
     
     
     Double A = 28./(28.95-molecular_weight) ;
     
     Double zh = 105. ;
     Double zl = 120. ;
     
     Double G_L = inFFT.expansion( F107, mean_F107, tableno, col, t_d, tau, t, Ap, latitude, longitude ) ;
     Double n_l = n_l_average*exp(G_L) ;
     Double D_z_M = inFFT.Dprofile( F107,mean_F107,tableno,col,t_d,tau,t,Ap,latitude,longitude,altitude,molecular_weight) ;
     
     Double temp_zl = inFFT.temperature(F107, mean_F107, tableno, col, t_d, tau, t, Ap, latitude, longitude, zl ) ;
     Double temp_zh = inFFT.temperature(F107, mean_F107, tableno, col, t_d, tau, t, Ap, latitude, longitude, zh ) ;
     Double temp_z = inFFT.temperature(F107, mean_F107, tableno, col, t_d, tau, t, Ap, latitude, longitude, altitude ) ;
     
     Double arguement1 = (temp_zl/temp_z) ;
     Double pow_factor1 =pow(arguement1, (1+alpha));
     
     Double nd_z_M = n_l*D_z_M*pow_factor1 ;   // Diffusive profile
     
     Double D_zh_M = inFFT.Dprofile( F107,mean_F107,tableno,col,t_d,tau,t,Ap,latitude,longitude,zh,molecular_weight) ;
     
     //mixing profile
     Double D_zh_M0 =inFFT.Dprofile( F107,mean_F107,tableno,col,t_d,tau,t,Ap,latitude,longitude,zh,28.95) ;	  
     Double D_z_M0 =inFFT.Dprofile( F107,mean_F107,tableno,col,t_d,tau,t,Ap,latitude,longitude,altitude,28.95) ;
     
     Double arguement2 = (temp_zl/temp_zh) ;
     Double pow_factor2 =pow(arguement2, alpha);
     
     Double nm_z_M =n_l*(D_zh_M/D_zh_M0)*(pow_factor2*D_z_M0*arguement1) ;
     
     // mixing profiles have been omitted right now
     Double power_diffusive = pow(nd_z_M, A) ;
     Double power_mixing = pow(nm_z_M, A) ;
     
     Ndensityprofile = pow((power_diffusive+power_mixing),(1/A)) ;
     
     return Ndensityprofile ;
     
   } catch( AipsError x ) {
     cerr << "inverseFFT::Ndensityprofile" <<x.getMesg() << endl ;
     return .0;
   } 
 }

  

 Double inverseFFT::Hedensityprofile( const Double& F107,
                           	      const Double& mean_F107,
                                      const Double& t_d,
                                      const Double& tau,
                                      const Double& t,
                                      const Double& Ap,
                                      const Double& latitude,
                                      const Double& longitude,
                                      const Double& altitude ) 
   
 {				     				     
   try {
     
     inverseFFT inFFT ;
     
     Double Hedensityprofile(0.0) ;
     Double alpha(0.0) ;
     uint tableno(1) ;
     uint col(1) ;
     Double molecular_weight(4.00) ;
     Double n_l_average(2.577e+15) ;
     
     if(((tableno==1)&&(col==1))||((tableno==1)&&(col==5)))
       
       alpha = 0.4 ;
     
     else
       alpha = 0.0 ;
     
     
     Double A = 28./(28.95-molecular_weight) ;
     
     Double zh = 105. ;
     Double zl = 120. ;
     
     Double G_L = inFFT.expansion( F107, mean_F107, tableno, col, t_d, tau, t, Ap, latitude, longitude ) ;
     Double n_l = n_l_average*exp(G_L) ;
     Double D_z_M = inFFT.Dprofile( F107,mean_F107,tableno,col,t_d,tau,t,Ap,latitude,longitude,altitude,molecular_weight) ;
     
     Double temp_zl = inFFT.temperature(F107, mean_F107, tableno, col, t_d, tau, t, Ap, latitude, longitude, zl ) ;
     Double temp_zh = inFFT.temperature(F107, mean_F107, tableno, col, t_d, tau, t, Ap, latitude, longitude, zh ) ;
     Double temp_z = inFFT.temperature(F107, mean_F107, tableno, col, t_d, tau, t, Ap, latitude, longitude, altitude ) ;
     
     Double arguement1 = (temp_zl/temp_z) ;
     Double pow_factor1 =pow(arguement1, (1+alpha));
     
     Double nd_z_M = n_l*D_z_M*pow_factor1 ;   // Diffusive profile
     
     Double D_zh_M = inFFT.Dprofile( F107,mean_F107,tableno,col,t_d,tau,t,Ap,latitude,longitude,zh,molecular_weight) ;
     
     //mixing profile
     Double D_zh_M0 =inFFT.Dprofile( F107,mean_F107,tableno,col,t_d,tau,t,Ap,latitude,longitude,zh,28.95) ;	  
     Double D_z_M0 =inFFT.Dprofile( F107,mean_F107,tableno,col,t_d,tau,t,Ap,latitude,longitude,altitude,28.95) ;
     
     Double arguement2 = (temp_zl/temp_zh) ;
     Double pow_factor2 =pow(arguement2, alpha);
     
     Double nm_z_M =n_l*(D_zh_M/D_zh_M0)*(pow_factor2*D_z_M0*arguement1) ;
     
     // mixing profiles have been omitted right now
     Double power_diffusive = pow(nd_z_M, A) ;
     Double power_mixing = pow(nm_z_M, A) ;
     
     Hedensityprofile = pow((power_diffusive+power_mixing),(1/A)) ;
     
     return Hedensityprofile ;
     
   } catch( AipsError x ) {
     cerr << "inverseFFT::Hedensityprofile" <<x.getMesg() << endl ;
     return .0;
   } 
 }

  

  Double inverseFFT::Hdensityprofile( const Double& F107,
                           	      const Double& mean_F107,
                                      const Double& t_d,
                                      const Double& tau,
                                      const Double& t,
                                      const Double& Ap,
                                      const Double& latitude,
                                      const Double& longitude,
                                      const Double& altitude ) 
				       
 {				     				     
     try {
       
           inverseFFT inFFT ;
	 
	 Double Hdensityprofile(0.0) ;
	 Double alpha(0.0) ;
	 uint tableno(1) ;
	 uint col(5) ;
	 Double molecular_weight(2.00) ;
	 Double n_l_average(2.246e+11) ;
	  
	 alpha = 0.4 ;
	
	 Double A = 28./(28.95-molecular_weight) ;
	
         Double zh = 105. ;
	 Double zl = 120. ;
	
	 Double G_L = inFFT.expansion( F107, mean_F107, tableno, col, t_d, tau, t, Ap, latitude, longitude ) ;
	 Double n_l = n_l_average*exp(G_L) ;
	 Double D_z_M = inFFT.Dprofile( F107,mean_F107,tableno,col,t_d,tau,t,Ap,latitude,longitude,altitude,molecular_weight) ;
	
	 Double temp_zl = inFFT.temperature(F107, mean_F107, tableno, col, t_d, tau, t, Ap, latitude, longitude, zl ) ;
	 Double temp_zh = inFFT.temperature(F107, mean_F107, tableno, col, t_d, tau, t, Ap, latitude, longitude, zh ) ;
	 Double temp_z = inFFT.temperature(F107, mean_F107, tableno, col, t_d, tau, t, Ap, latitude, longitude, altitude ) ;
	
	 Double arguement1 = (temp_zl/temp_z) ;
	 Double pow_factor1 =pow(arguement1, (1+alpha));
	
	 Double nd_z_M = n_l*D_z_M*pow_factor1 ;   // Diffusive profile
	
	 Double D_zh_M = inFFT.Dprofile( F107,mean_F107,tableno,col,t_d,tau,t,Ap,latitude,longitude,zh,molecular_weight) ;
	
	//mixing profile
	 Double D_zh_M0 =inFFT.Dprofile( F107,mean_F107,tableno,col,t_d,tau,t,Ap,latitude,longitude,zh,28.95) ;	  
	 Double D_z_M0 =inFFT.Dprofile( F107,mean_F107,tableno,col,t_d,tau,t,Ap,latitude,longitude,altitude,28.95) ;
	
	 Double arguement2 = (temp_zl/temp_zh) ;
	 Double pow_factor2 =pow(arguement2, alpha);
	
	 Double nm_z_M =n_l*(D_zh_M/D_zh_M0)*(pow_factor2*D_z_M0*arguement1) ;
	
	 // mixing profiles have been omitted right now
	 Double power_diffusive = pow(nd_z_M, A) ;
	 Double power_mixing = pow(nm_z_M, A) ;
	 
	 Hdensityprofile = pow((power_diffusive+power_mixing),(1/A)) ;
       
       return Hdensityprofile ;
       
     } catch( AipsError x ) {
      cerr << "inverseFFT::Hdensityprofile" <<x.getMesg() << endl ;
    } 
    
 //   return Hdensityprofile ;
 }
 
 
  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================
  
}  // Namespace CR -- end
