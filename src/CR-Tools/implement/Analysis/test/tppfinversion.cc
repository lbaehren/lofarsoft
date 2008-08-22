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

/* $Id: tppfinversion.cc,v 1.2 2007/08/08 15:30:04 singh Exp $*/

#include <Analysis/ppfinversion.h>
#include <Analysis/ppfimplement.h>
#include <Data/rawSubbandIn.h>
#include <Data/tbbctlIn.h>
#include <scimath/Mathematics/FFTServer.h>

/*!
  \file tppfinversion.cc

  \ingroup Analysis

  \brief A collection of test routines for ppfinversion
 
  \author Kalpana Singh
 
  \date 2007/06/01
*/
using CR::ppfinversion ;
using CR::ppfimplement ;
using CR::tbbctlIn ;
using CR::rawSubbandIn ;

uint dataBlockSize (1024);
uint nofsegmentation(16*6) ;
Vector<Double> samples( dataBlockSize*nofsegmentation, 0.0 ) ;
 
// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new ppfinversion object

  \return nofFailedTests -- The number of failed tests.
*/

void show_data (Matrix<Double> const &data )
 {
//   std::cout << " Data Shape : " << data.shape() << std::endl ;
//   std::cout << " Data ......: " 
//             << data(1,1) << ""
// 	    << data(2,1) << ""
// 	    << data(3,1)
// 	    <<std::endl ;
	    
 }
 int nofFailedTests (0);
 
int test_ppfinversion ()
{
  std::cout << "\n[test_ppfinversion]\n" << std::endl;

  std::cout << "[1] Testing default constructor ..." << std::endl;
  
  try {
           
      ppfinversion ppf_inv ;
      ppfimplement ppf_imp ;
      tbbctlIn newtbbctlIn ;
      rawSubbandIn newrawSubbandIn ;
     
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
 }

 Bool test_ppfinversions ()

{
 Bool ok(True) ;
  
   try {
        int nofFailedTests (0);
        Vector<Double> ppfcoeff(16384,0.0) ;
	readAsciiVector( ppfcoeff,"Coeffs16384Kaiser-quant.dat" ) ;
	Vector<Double> ppfcoeff_inv(16384, 0.0) ;
	readAsciiVector( ppfcoeff_inv, "ppf_inv.dat") ;
	
	ppfinversion ppf_inv ;
        ppfimplement ppf_imp ;
        tbbctlIn newtbbctlIn ;
        rawSubbandIn newrawSubbandIn ;     
	
	Vector<String> filenames(1) ;
//	filenames(0) = "/mnt/lofar/kalpana/rw_20080306_140807_2300.dat" ;
//	filenames(0) = "/mnt/lofar/kalpana/rw_20080306_141000_2300.dat" ;
	filenames(0) = "/mnt/lofar/kalpana/rw_20080306_141000_2301.dat" ;
	
	
	std::cout << "[2] Testing attaching a file... " << std::endl ;
	
	if(!newtbbctlIn.attachFile(filenames)) {
	  cout << " Failed to attach file(s)!" <<endl ;
	  nofFailedTests++ ;
	  return nofFailedTests ;
	  } ;
	 
	cout << "[3]Testing retrieval of fx() data....." << endl ;
	Matrix<Double> data ;
	data = newtbbctlIn.fx() ;
	uint rows_data = data.nrow() ;
	cout << "number of rows of real raw data : " <<  rows_data << endl ;
	uint columns_data = data.ncolumn() ;
	cout << "number of columns of real raw data : " << columns_data<< endl ;
// 	cout << "[4] Testing of voltage data...." << endl ;
// 	data = newtbbctlIn.voltage() ;
         	
 
	Vector<Double> data_input = data.column(0) ;
	uint ndatasamples_raw = data_input.nelements() ;
	
	cout << " number of elements in the raw vector samples :" << ndatasamples_raw << endl ;
	
	Vector<Double> chopped_data(6144000) ;
	chopped_data = data_input(Slice(0,6144000)) ;
	
	uint n_chopped_data= chopped_data.nelements() ;
	cout << "number of elements in the chopped data vector: " << n_chopped_data <<endl ;
	
	ofstream logfile1 ;
 	logfile1.open("data", ios::out) ;
 	for( uint i(0); i< n_chopped_data ; i++ ){
 	     logfile1 << chopped_data(i) << endl ;
 	     }
	     
 	
	Matrix<DComplex> ppfimp_data = ppf_imp.FFTSamples( chopped_data,
                                                           ppfcoeff ) ;

        Matrix<Double> absppf_impdata = amplitude(ppfimp_data) ;
	
	uint nrow_ppfimp = absppf_impdata.nrow() ;
	cout << " number of rows in simulated ppf implemented data " << nrow_ppfimp <<endl ;
	uint ncolumn_ppfimp = absppf_impdata.ncolumn() ;
	cout << " number of columns in simulated ppf implemented  data " << ncolumn_ppfimp <<endl ;
	
	ofstream logfile2 ;
 	logfile2.open("ppfimpdata", ios::out) ;
 	for( uint i(0); i< ncolumn_ppfimp ; i++ ){
 	     logfile2 << absppf_impdata(256,i) << endl ;
 	     }
	     
	cout <<"[5] testing header record...." << endl ;
	
	for( uint i=0; i< newtbbctlIn.header().nfields(); i++ ){
	   cout<< "Field No: " << i<< " Name: " << newtbbctlIn.header().name(i) <<endl ;
	  }
	  
	cout << "values:\n date: " << newtbbctlIn.header().asDouble("dDate") << endl ;
	Double DATE = newtbbctlIn.header().asDouble("dDate") ;
	cout <<"Observatory:" <<newtbbctlIn.header().asString("Observatory") << endl ;
	cout << "IDs :" << newtbbctlIn.header().asArrayInt("AntennaIDs") << endl ;
		
	cout << "\n[test_rawsubbandIn]\n" << endl ;
	
	cout << "[1] Testing default constructor..." << endl ;
	
	newrawSubbandIn.attachFile("/mnt/lofar/kalpana/raw1.out") ;
	
	Matrix<DComplex> data_filtered ;
	
	data_filtered = newrawSubbandIn.getData(DATE, 6000, 0);

	//cout <<endl <<data_filtered(0,0) <<";" <<data_filtered(0,1) <<";" <<data_filtered(1,0) <<endl ;
	uint rows_filtered = data_filtered.nrow() ;
	cout << " number of rows of filtered real data " << rows_filtered << endl ;
	
	uint columns_filtered = data_filtered.ncolumn() ;
	cout << " Columns of filtered real data :" << columns_filtered << endl ;
		
	Matrix<DComplex> FFT_data(columns_filtered, rows_filtered,0.0 ) ;
	
	for( uint i=0; i < columns_filtered; i++ ){
	
	     FFT_data.row(i)= data_filtered.column(i) ;
	}
	uint rows_fftdata = FFT_data.nrow() ;
	
	Matrix<Double> abs_FFT = amplitude(FFT_data) ;
	uint nofc = abs_FFT.ncolumn() ;
	uint nofr = abs_FFT.nrow() ;
	
	cout << " number of columns in ppf implemented data " << nofc <<endl ;
	cout << " number of rows in ppf implemented data " << nofr << endl ;
	
	ofstream logfile3 ;
		
 	logfile3.open("absdata", ios::out) ;
 	
	for( uint j(0); j<nofc  ; j++ ){
 	     logfile3 << abs_FFT(0,j) << endl ;
 	}	
	
	FFTServer<Double,DComplex> server1;
	//FFTServer<Double,DComplex> server2;
	
	uint m = absppf_impdata.ncolumn() ;
        cout << " number of columns in simulated ppf implemented  data " << m <<endl ;
	
	uint n = abs_FFT.ncolumn() ;
	cout << " number of columns in ppf implemented data " << n <<endl ;
	
// 	uint l = m +n-1 ;
// 	cout << " number of elements for which vector has to be extended :" << l<< endl;
// 	
// 	Vector<Double> extend_1((l-m),0.0) ;
// 	Vector<Double> extend_2((l-n),0.0) ;
// 	
// 	Vector<Double> manup_1(l,0.0);
// 	Vector<Double> manup_2(l,0.0);
// 	
// 	manup_1(Slice(0,m))= absppf_impdata.row(255) ;
// 	cout << " Half of vector of manupulated vector has been done :" << endl ;
// 	
// 	manup_1(Slice(m-1,l-m))=extend_1 ;
// 	cout << " Second half of the vector has been fed to the manupulated vector "<< endl ;
// 	
// 	manup_2(Slice(0,n)) = abs_FFT.row(0) ;
// 	manup_2(Slice(n-1,l-n)) = extend_2 ;
// 	
// 	cout << "Each vector has been manupulated to get the same vector strength " << endl ;
	
// 	Vector<Double> flipped(l,0.0) ;
// 	
// 	for(uint i=0; i<l; i++ ){
// 	
// 	      flipped(i)=manup_1(l-i-1) ;
// 	      } 
// 	
     //   Vector<Double> manup_1(l,0);
     uint nsample = m/2+1 ;
     cout << " number of samples in the fourier transformed data : " << nsample <<endl ;
        Vector<DComplex> output_1;
        server1.fft(output_1, absppf_impdata.row(255));
        cout << "FFT on simulated ppf implemented data has been performed "<< endl;
	
//	Vector<Double> flipped(l,0);
        Vector<DComplex> output_2;
        server1.fft(output_2, abs_FFT.row(0));   
	cout << " FFT on ppf implemented data has been performed " <<endl ;
	
	Vector<DComplex> conjugated(nsample,0.0);
	conjugated= conj(output_1);	
	
	Vector<DComplex> product = conjugated*output_2 ;
	
	Vector<Double> correlation(m, 0.0);
	
	//FFTServer<DComplex,Double> server3;
	server1.fft(correlation,product);
	cout << "IFFT is performed on the product " << endl ;
	
	Double max_value = max(correlation) ;
	cout << "Maximum value : " << max_value << endl ;
	
	ofstream logfile4 ;
		
 	logfile4.open("correlation", ios::out) ;
 	
	for( uint j(0); j<m  ; j++ ){
 	     logfile4 << correlation(j) << endl ;
 	}	
	
	
	
	
//         Vector<uint> subBandID = newrawSubbandIn.getSubbandIndices() ;
// 	cout << " sub band ids to identify which are the subbands which are filtered " << subBandID <<endl ;
// 		
// 	Vector<Double> ppfinverted = ppf_inv.FIR_inversion( ppfcoeff_inv,
//                                	                            FFT_data,
// 					                    subBandID ) ;
//         	
// 	uint ninverted = ppfinverted.nelements() ;
//         cout << " number of elements in the inverted data vector " << ninverted << endl ; 
// 		
// 	ofstream logfile4 ;
// 		
//  	logfile4.open("filterdata", ios::out) ;
//  	
// 	for( uint j(0); j< ninverted ; j++ ){
//  	     logfile4<< ppfinverted(j) << endl ;
//  	     }
       
//        uint N = n_chopped_data + ninverted -1 ;
//        
//        uint N_expected(0);
//        
//        uint power(0) ;
//        
//        while(N_expected<N ){
//       
//          N_expected = pow(2,power)  ;
// 	 
// 	 power = power +1 ;
//       }
//       
//       FFTServer<Double,DComplex> server ;
//       Vector<Double> signal1= absppf_impdata.row(255) ;
//       Vector<Double> signal2= abs_FFT.row(0);
//      
               
        	     
   } catch( AipsError x) {
   cerr << x.getMesg() << endl ;
   ok = False ;
   }
 return ok ;
 
 }   
  
// -----------------------------------------------------------------------------

int main ()
{
  Bool ok(True);
  
  Int retval(0) ;
  if(ok) {
  ok= test_ppfinversions ();
  if(!ok){
  retval = 1;
  cout <<"Error............... early exit " << endl;
  }
 }
 return retval;
}
