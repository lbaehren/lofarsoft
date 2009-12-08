/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2009                                                    *
 *   Kalpana Singh (<mail>)                                                *
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

#include <Analysis/NuMoonTrigger.h>

#include <crtools.h>

// Namespace usage
using CR::NuMoonTrigger;

using namespace std;
/*!
  \file tNuMoonTrigger.cc

  \ingroup Analysis

  \brief A collection of test routines for the NuMoonTrigger class
 
  \author Kalpana Singh
 
  \date 2009/04/21
*/
Matrix<Double> posit_x(10,10,0.0) ;

double ant_pos_x[73] ={ 3827966.965, 3827966.965, 3827964.913, 3827964.913, 3827967.532, 3827967.532, 3827970.151, 3827970.151, 3827972.770, 3827972.770, 3827965.481, 3827965.481, 3827968.099, 3827968.099, 3827970.718, 3827970.718, 3827973.338, 3827973.338,  3827975.957, 3827978.576, 3827978.576, 3827968.667, 3827968.667, 3827971.286, 3827971.286, 3827973.905, 3827973.905, 3827976.525, 3827976.525, 3827979.144, 3827981.763, 3827981.763, 3827974.473, 3827974.473, 3827977.092, 3827977.092, 3827979.712, 3827979.712, 3827982.331, 3827982.331, 3827980.279, 3827980.279, 3827982.899, 3827982.899,  3827887.569, 3827890.189, 3827890.189, 3827892.808, 3827892.808, 3827895.427, 3827895.427, 3827885.518, 3827885.518, 3827888.137, 3827888.137, 3827890.756, 3827890.756, 3827893.376, 3827893.376, 3827895.995, 3827898.614, 3827898.614, 3827891.324, 3827893.943, 3827893.943, 3827896.563, 3827896.563, 3827899.182, 3827899.182, 3827897.130, 3827897.130, 3827899.750, 3827899.750} ;

double ant_pos_y[73] ={459738.871, 459738.871, 459727.827, 459727.827, 459731.613, 459731.613, 459735.400, 459735.400, 459739.186, 459739.186, 459720.570, 459720.570, 459724.356, 459724.356, 459728.143, 459728.143, 459731.929, 459731.929, 459735.716, 459739.502, 459739.502, 459717.098, 459717.098, 459720.885, 459720.885, 459724.671, 459724.671, 459728.458, 459728.458, 459732.244, 459736.031, 459736.031, 459717.414, 459717.414, 459721.200, 459721.200, 459724.987, 459724.987, 459728.773, 459728.773, 459717.730, 459717.730, 459721.516, 459721.516, 459810.001, 459813.788, 459813.788, 459817.574, 459817.574, 459821.361, 459821.361, 459798.957, 459798.957, 459802.743, 459802.743, 459806.529, 459806.529, 459810.316, 459810.316, 459814.103, 459817.889, 459817.889, 459799.272, 459803.059, 459803.059, 459806.845, 459806.845, 459810.632, 459810.632, 459799.588, 459799.588, 459803.375, 459803.375} ;

double ant_pos_z[73] ={5063979.081, 5063979.081, 5063981.619, 5063981.619, 5063979.311, 5063979.311, 5063977.003, 5063977.003, 5063974.696, 5063974.696, 5063981.848, 5063981.848, 5063979.540, 5063979.540, 5063977.233, 5063977.233, 5063974.925, 5063974.925,  5063972.617, 5063970.310, 5063970.310, 5063979.770, 5063979.770, 5063977.462, 5063977.462, 5063975.155, 5063975.155, 5063972.846, 5063972.846, 5063970.539, 5063968.231, 5063968.231, 5063975.384, 5063975.384, 5063973.076, 5063973.076, 5063970.768, 5063970.768, 5063968.461, 5063968.461, 5063970.998, 5063970.998, 5063968.690, 5063968.690,  5064032.247, 5064029.938, 5064029.938, 5064027.631, 5064027.631, 5064025.323, 5064025.323, 5064034.783, 5064034.783, 5064032.476, 5064032.476, 5064030.168, 5064030.168, 5064027.860, 5064027.860, 5064025.553, 5064023.245, 5064023.245, 5064030.397, 5064028.090, 5064028.090, 5064025.782, 5064025.782, 5064023.474, 5064023.474, 5064026.012, 5064026.012, 5064023.703, 5064023.703} ;

double scale_factor[73] ={1.0000, 0.6857, 0.8969, 0.6954, 0.7453, 0.8203, 1.2338, 0.3979, 0.8047, 0.7890, 1.0431, 0.6420, 1.0879, 0.6568, 1.2115, 1.2821, 0.6524, 0.5854, 0.4359, 3.4726, 0.8512, 0.7913, 1.3203, 0.4982, 1.1181, 1.1399, 0.5668, 0.5176, 0.3655, 1.2023, 0.5053, 1.0924, 0.9606, 0.9260, 0.7591, 0.3749, 1.2073, 1.2450, 0.9350, 0.3052, 1.3795, 0.6131, 0.6383, 1.2963, 0.6400, 0.6640, 0.9046, 0.5080, 1.1674, 1.2418, 0.8664, 0.6543, 0.5156, 0.3888, 0.3552, 0.4477, 0.4857, 0.6577, 1.0841, 1.0546, 1.3703, 0.8517, 0.9902, 0.9733, 1.0797, 1.2361, 0.5998, 0.8988, 1.2613, 0.6712, 0.9618, 1.3254, 0.7158} ;


// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new NuMoonTrigger object

  \return nofFailedTests -- The number of failed tests encountered within this
          function.
*/
int test_constructors (std::string const &filename)
{
  std::cout << "\n[tNuMoonTrigger::test_constructors]\n" << std::endl;

  int nofFailedTests (0);
  
  std::cout << "[1] Testing default constructor ..." << std::endl;
  try {
    	NuMoonTrigger numooontrigger;
	
	casa::Vector<double> antenna_posX( 73, 0.0 ) ;
	casa::Vector<double> antenna_posY( 73, 0.0 ) ;
	casa::Vector<double> antenna_posZ( 73, 0.0 ) ;
	casa::Vector<double> gain_scale_factors( 73, 0.0) ;
	
	for( uint pos=0; pos < 73; pos++) {
		antenna_posX(pos) = ant_pos_x[pos] ;
		antenna_posY(pos) = ant_pos_y[pos] ;
		antenna_posZ(pos) = ant_pos_z[pos] ;
		gain_scale_factors(pos) = scale_factor[pos] ;
	}
	cout << "antenna positions have been defined : " <<endl ;
		
	uint n_samples = 600*1024 ;  // maximum 4000 can be read
	
/*
  	 CR::LOFAR_TBB lofar_tbb( filename, n_samples ) ;
 	
 	 lofar_tbb.DataReader::setNyquistZone(2);

 	casa::Matrix<double> data = lofar_tbb.fx( ) ;	
	
	uint nSamples = data.nrow() ;
	uint nAntenna = data.ncolumn() ;
	
	ofstream logfile11;
      
           logfile11.open( "5_16_hdf5", ios::out );
	  
	   for( uint sample=0; sample < nSamples ; sample++ ) {
	   
	   	      logfile11 << data(sample,15) << endl;
	  	
		}
		
	logfile11.close() ;

	

*/
/*		
	casa::Vector<double> Tile_1_column1(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_1_column2(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_1_column3(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_1_column4(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_1_column5(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_1_column6(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_1_column7(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_1_column8(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_1_column9(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_1_column10(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_1_column11(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_1_column12(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_1_column13(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_1_column14(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_1_column15(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_1_column16(n_samples, 0.0 ) ;
	
	casa::Vector<double> Tile_2_column1(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_2_column2(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_2_column3(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_2_column4(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_2_column5(n_samples, 0.0 ) ;
	//casa::Vector<double> Tile_2_column6(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_2_column7(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_2_column8(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_2_column9(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_2_column10(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_2_column11(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_2_column12(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_2_column13(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_2_column14(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_2_column15(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_2_column16(n_samples, 0.0 ) ;
	
	casa::Vector<double> Tile_3_column1(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_3_column2(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_3_column3(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_3_column4(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_3_column5(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_3_column6(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_3_column7(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_3_column8(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_3_column9(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_3_column10(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_3_column11(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_3_column12(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_3_column13(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_3_column14(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_3_column15(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_3_column16(n_samples, 0.0 ) ;
	
	casa::Vector<double> Tile_4_column1(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_4_column2(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_4_column3(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_4_column4(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_4_column5(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_4_column6(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_4_column7(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_4_column8(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_4_column9(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_4_column10(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_4_column11(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_4_column12(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_4_column13(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_4_column14(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_4_column15(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_4_column16(n_samples, 0.0 ) ;

	casa::Vector<double> Tile_5_column1(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_5_column2(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_5_column3(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_5_column4(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_5_column5(n_samples, 0.0 ) ;
	//casa::Vector<double> Tile_5_column6(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_5_column7(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_5_column8(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_5_column9(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_5_column10(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_5_column11(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_5_column12(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_5_column13(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_5_column14(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_5_column15(n_samples, 0.0 ) ;
	casa::Vector<double> Tile_5_column16(n_samples, 0.0 ) ;
	
// 	casa::Vector<double> Tile_ppf(n_samples, 0.0 ) ;
// 	readAsciiVector( Tile_ppf,"5_ppf_hdf5") ;
// 	
	readAsciiVector( Tile_1_column1,"/mnt/lofar/ppfinversion/1_1_hdf5" ) ;
	readAsciiVector( Tile_1_column2,"/mnt/lofar/ppfinversion/1_2_hdf5" ) ;
	readAsciiVector( Tile_1_column3,"/mnt/lofar/ppfinversion/1_3_hdf5" ) ;
	readAsciiVector( Tile_1_column4,"/mnt/lofar/ppfinversion/1_4_hdf5" ) ;
	readAsciiVector( Tile_1_column5,"/mnt/lofar/ppfinversion/1_5_hdf5" ) ;
	readAsciiVector( Tile_1_column6,"/mnt/lofar/ppfinversion/1_6_hdf5" ) ;
	readAsciiVector( Tile_1_column7,"/mnt/lofar/ppfinversion/1_7_hdf5" ) ;
	readAsciiVector( Tile_1_column8,"/mnt/lofar/ppfinversion/1_8_hdf5" ) ;
	readAsciiVector( Tile_1_column9,"/mnt/lofar/ppfinversion/1_9_hdf5" ) ;
	readAsciiVector( Tile_1_column10,"/mnt/lofar/ppfinversion/1_10_hdf5" ) ;
	readAsciiVector( Tile_1_column11,"/mnt/lofar/ppfinversion/1_11_hdf5" ) ;
	readAsciiVector( Tile_1_column12,"/mnt/lofar/ppfinversion/1_12_hdf5" ) ;
	readAsciiVector( Tile_1_column13,"/mnt/lofar/ppfinversion/1_13_hdf5" ) ;
	readAsciiVector( Tile_1_column14,"/mnt/lofar/ppfinversion/1_14_hdf5" ) ;
	readAsciiVector( Tile_1_column15,"/mnt/lofar/ppfinversion/1_15_hdf5" ) ;
	readAsciiVector( Tile_1_column16,"/mnt/lofar/ppfinversion/1_16_hdf5" ) ;
		
	readAsciiVector( Tile_2_column1,"/mnt/lofar/ppfinversion/2_1_hdf5" ) ;
	readAsciiVector( Tile_2_column2,"/mnt/lofar/ppfinversion/2_2_hdf5" ) ;
	readAsciiVector( Tile_2_column3,"/mnt/lofar/ppfinversion/2_3_hdf5" ) ;
	readAsciiVector( Tile_2_column4,"/mnt/lofar/ppfinversion/2_4_hdf5" ) ;
	readAsciiVector( Tile_2_column5,"/mnt/lofar/ppfinversion/2_5_hdf5" ) ;
	//readAsciiVector( Tile_2_column6,"2_6_hdf" ) ;
	readAsciiVector( Tile_2_column7,"/mnt/lofar/ppfinversion/2_7_hdf5" ) ;
	readAsciiVector( Tile_2_column8,"/mnt/lofar/ppfinversion/2_8_hdf5" ) ;
	readAsciiVector( Tile_2_column9,"/mnt/lofar/ppfinversion/2_9_hdf5" ) ;
	readAsciiVector( Tile_2_column10,"/mnt/lofar/ppfinversion/2_10_hdf5" ) ;
	readAsciiVector( Tile_2_column11,"/mnt/lofar/ppfinversion/2_11_hdf5" ) ;
	readAsciiVector( Tile_2_column12,"/mnt/lofar/ppfinversion/2_12_hdf5" ) ;
	readAsciiVector( Tile_2_column13,"/mnt/lofar/ppfinversion/2_13_hdf5" ) ;
	readAsciiVector( Tile_2_column14,"/mnt/lofar/ppfinversion/2_14_hdf5" ) ;
	readAsciiVector( Tile_2_column15,"/mnt/lofar/ppfinversion/2_15_hdf5" ) ;
	readAsciiVector( Tile_2_column16,"/mnt/lofar/ppfinversion/2_16_hdf5" ) ;
	cout <<"number of elements in vector Tile_2_column1 " << Tile_2_column2.nelements() << endl ;
		

	readAsciiVector( Tile_3_column1,"/mnt/lofar/ppfinversion/3_1_hdf5" ) ;
	readAsciiVector( Tile_3_column2,"/mnt/lofar/ppfinversion/3_2_hdf5" ) ;
	readAsciiVector( Tile_3_column3,"/mnt/lofar/ppfinversion/3_3_hdf5" ) ;
	readAsciiVector( Tile_3_column4,"/mnt/lofar/ppfinversion/3_4_hdf5" ) ;
	readAsciiVector( Tile_3_column5,"/mnt/lofar/ppfinversion/3_5_hdf5" ) ;
	readAsciiVector( Tile_3_column6,"/mnt/lofar/ppfinversion/3_6_hdf5" ) ;
	readAsciiVector( Tile_3_column7,"/mnt/lofar/ppfinversion/3_7_hdf5" ) ;
	readAsciiVector( Tile_3_column8,"/mnt/lofar/ppfinversion/3_8_hdf5" ) ;
	readAsciiVector( Tile_3_column9,"/mnt/lofar/ppfinversion/3_9_hdf5" ) ;
	readAsciiVector( Tile_3_column10,"/mnt/lofar/ppfinversion/3_10_hdf5" ) ;
	readAsciiVector( Tile_3_column11,"/mnt/lofar/ppfinversion/3_11_hdf5" ) ;
	readAsciiVector( Tile_3_column12,"/mnt/lofar/ppfinversion/3_12_hdf5" ) ;
	readAsciiVector( Tile_3_column13,"/mnt/lofar/ppfinversion/3_13_hdf5" ) ;
	readAsciiVector( Tile_3_column14,"/mnt/lofar/ppfinversion/3_14_hdf5" ) ;
	readAsciiVector( Tile_3_column15,"/mnt/lofar/ppfinversion/3_15_hdf5" ) ;
	readAsciiVector( Tile_3_column16,"/mnt/lofar/ppfinversion/3_16_hdf5" ) ;
	cout <<"number of elements in vector Tile_3_column1 " << Tile_3_column2.nelements() << endl ;
		
	readAsciiVector( Tile_4_column1,"/mnt/lofar/ppfinversion/4_1_hdf5" ) ;
	readAsciiVector( Tile_4_column2,"/mnt/lofar/ppfinversion/4_2_hdf5" ) ;
	readAsciiVector( Tile_4_column3,"/mnt/lofar/ppfinversion/4_3_hdf5" ) ;
	readAsciiVector( Tile_4_column4,"/mnt/lofar/ppfinversion/4_4_hdf5" ) ;
	readAsciiVector( Tile_4_column5,"/mnt/lofar/ppfinversion/4_5_hdf5" ) ;
	readAsciiVector( Tile_4_column6,"/mnt/lofar/ppfinversion/4_6_hdf5" ) ;
	readAsciiVector( Tile_4_column7,"/mnt/lofar/ppfinversion/4_7_hdf5" ) ;
	readAsciiVector( Tile_4_column8,"/mnt/lofar/ppfinversion/4_8_hdf5" ) ;
	readAsciiVector( Tile_4_column9,"/mnt/lofar/ppfinversion/4_9_hdf5" ) ;
	readAsciiVector( Tile_4_column10,"/mnt/lofar/ppfinversion/4_10_hdf5" ) ;
	readAsciiVector( Tile_4_column11,"/mnt/lofar/ppfinversion/4_11_hdf5" ) ;
	readAsciiVector( Tile_4_column12,"/mnt/lofar/ppfinversion/4_12_hdf5" ) ;
	readAsciiVector( Tile_4_column13,"/mnt/lofar/ppfinversion/4_13_hdf5" ) ;
	readAsciiVector( Tile_4_column14,"/mnt/lofar/ppfinversion/4_14_hdf5" ) ;
	readAsciiVector( Tile_4_column15,"/mnt/lofar/ppfinversion/4_15_hdf5" ) ;
	readAsciiVector( Tile_4_column16,"/mnt/lofar/ppfinversion/4_16_hdf5" ) ;
	
	readAsciiVector( Tile_5_column1,"/mnt/lofar/ppfinversion/5_1_hdf5" ) ;
	readAsciiVector( Tile_5_column2,"/mnt/lofar/ppfinversion/5_2_hdf5" ) ;
	readAsciiVector( Tile_5_column3,"/mnt/lofar/ppfinversion/5_3_hdf5" ) ;
	readAsciiVector( Tile_5_column4,"/mnt/lofar/ppfinversion/5_4_hdf5" ) ;
	readAsciiVector( Tile_5_column5,"/mnt/lofar/ppfinversion/5_5_hdf5" ) ;
	//readAsciiVector( Tile_5_column6,"5_6_hdf" ) ;
	readAsciiVector( Tile_5_column7,"/mnt/lofar/ppfinversion/5_7_hdf5" ) ;
	readAsciiVector( Tile_5_column8,"/mnt/lofar/ppfinversion/5_8_hdf5" ) ;
	readAsciiVector( Tile_5_column9,"/mnt/lofar/ppfinversion/5_9_hdf5" ) ;
	readAsciiVector( Tile_5_column10,"/mnt/lofar/ppfinversion/5_10_hdf5" ) ;
	readAsciiVector( Tile_5_column11,"/mnt/lofar/ppfinversion/5_11_hdf5" ) ; 
	readAsciiVector( Tile_5_column12,"/mnt/lofar/ppfinversion/5_12_hdf5" ) ;
	readAsciiVector( Tile_5_column13,"/mnt/lofar/ppfinversion/5_13_hdf5" ) ;
	readAsciiVector( Tile_5_column14,"/mnt/lofar/ppfinversion/5_14_hdf5" ) ;
	readAsciiVector( Tile_5_column15,"/mnt/lofar/ppfinversion/5_15_hdf5" ) ;
	readAsciiVector( Tile_5_column16,"/mnt/lofar/ppfinversion/5_16_hdf5" ) ;
	
	cout <<"number of elements in vector Tile_5_column1 " << Tile_5_column2.nelements() << endl ;
	
	casa::Matrix<double> raw_data(n_samples,73, 0.0 ) ;

	raw_data.column(0) = Tile_1_column3 ;
	raw_data.column(1) = Tile_1_column4 ;
	raw_data.column(2) = Tile_1_column5 ;
	raw_data.column(3) = Tile_1_column6 ;
	raw_data.column(4) = Tile_1_column7 ;
	raw_data.column(5) = Tile_1_column8 ;
	raw_data.column(6) = Tile_1_column9 ;
	raw_data.column(7) = Tile_1_column10 ;
	raw_data.column(8) = Tile_1_column11 ;
	raw_data.column(9) = Tile_1_column12 ;
	raw_data.column(10) = Tile_1_column13 ;
	raw_data.column(11) = Tile_1_column14 ;
	raw_data.column(12) = Tile_1_column15 ;
	raw_data.column(13) = Tile_1_column16 ;
	
	raw_data.column(14) = Tile_2_column1 ;
	raw_data.column(15) = Tile_2_column2 ;
	raw_data.column(16) = Tile_2_column3 ;
	raw_data.column(17) = Tile_2_column4 ;
	raw_data.column(18) = Tile_2_column5 ;
	//raw_data.column(19) = Tile_2_column6 ;
	raw_data.column(19) = Tile_2_column7 ;
	raw_data.column(20) = Tile_2_column8 ;
	raw_data.column(21) = Tile_2_column9 ;
	raw_data.column(22) = Tile_2_column10 ;
	raw_data.column(23) = Tile_2_column11 ;
	raw_data.column(24) = Tile_2_column12 ;
	raw_data.column(25) = Tile_2_column13 ;
	raw_data.column(26) = Tile_2_column14 ;
	raw_data.column(27) = Tile_2_column15 ;
	raw_data.column(28) = Tile_2_column16 ;
	
	raw_data.column(29) = Tile_3_column2 ;
	raw_data.column(30) = Tile_3_column3 ;
	raw_data.column(31) = Tile_3_column4 ;
	raw_data.column(32) = Tile_3_column5 ;
	raw_data.column(33) = Tile_3_column6 ;
	raw_data.column(34) = Tile_3_column7 ;
	raw_data.column(35) = Tile_3_column8 ;
	raw_data.column(36) = Tile_3_column9 ;
	raw_data.column(37) = Tile_3_column10 ;
	raw_data.column(38) = Tile_3_column11 ;
	raw_data.column(39) = Tile_3_column12 ;
	raw_data.column(40) = Tile_3_column13 ;
	raw_data.column(41) = Tile_3_column14 ;
	raw_data.column(42) = Tile_3_column15 ;
	raw_data.column(43) = Tile_3_column16 ;
	
	raw_data.column(44) = Tile_4_column2 ;
	raw_data.column(45) = Tile_4_column3 ;
	raw_data.column(46) = Tile_4_column4 ;
	raw_data.column(47) = Tile_4_column5 ;
	raw_data.column(48) = Tile_4_column6 ;
	raw_data.column(49) = Tile_4_column7 ;
	raw_data.column(50) = Tile_4_column8 ;
	raw_data.column(51) = Tile_4_column9 ;
	raw_data.column(52) = Tile_4_column10 ;
	raw_data.column(53) = Tile_4_column11 ;
	raw_data.column(54) = Tile_4_column12 ;
	raw_data.column(55) = Tile_4_column13 ;
	raw_data.column(56) = Tile_4_column14 ;
	raw_data.column(57) = Tile_4_column15 ;
	raw_data.column(58) = Tile_4_column16 ;
	
	raw_data.column(59) = Tile_5_column2 ;
	raw_data.column(60) = Tile_5_column3 ;
	raw_data.column(61) = Tile_5_column4 ;
	raw_data.column(62) = Tile_5_column5 ;
	//raw_data.column(64) = Tile_5_column6 ;
	raw_data.column(63) = Tile_5_column7 ;
	raw_data.column(64) = Tile_5_column8 ;
	raw_data.column(65) = Tile_5_column9 ;
	raw_data.column(66) = Tile_5_column10 ;
	raw_data.column(67) = Tile_5_column11 ;
	raw_data.column(68) = Tile_5_column12 ;
	raw_data.column(69) = Tile_5_column13 ;
	raw_data.column(70) = Tile_5_column14 ;
	raw_data.column(71) = Tile_5_column15 ;
	raw_data.column(72) = Tile_5_column16 ;
	
	casa::Vector<double> ppfcoeff(16384,0.0) ;
	casa::Vector<double> ppfcoeff_inv(16384, 0.0) ;
	
   	readAsciiVector( ppfcoeff,"Coeffs16384Kaiser-quant.dat" ) ;
       	readAsciiVector( ppfcoeff_inv,"ppf_inv.dat" ) ;

 	double simTEC = 0.0 ;
 	double sampling_rate = 200e6 ;
 	uint nyquist_zone = 2 ;
 	uint time_int_bins =5 ;
 	double TEC = 0.0 ;
 	uint n_frames = 600 ;
	//uint n_sample = 50*1024 ;
	
 	double source_latitude = 45.0 ;
 	double source_longitude = 160.0 ;
 	double pointing_latitude = 49.0 ;
 	double pointing_longitude = 156.0 ;
     
 	casa::Vector<double> freq_range(2,0.0) ;
 
 	double peak_height = 16.0 ;
 
 	freq_range(0)= 100e6 ;
     
     	freq_range(1) = 200e6 ;
 
 	uint dataBlockSize = 1024 ;
	
	casa::Matrix<double> sorted_matrix( 600*1024, 73, 0.0 ) ;
 	
 	//casa::Matrix<double> zero_freq_removed(n_samples,73,0.0);
	
 	casa::Matrix<DComplex> fft_all_antenna(513,73,0.0) ;
	
	//for( uint ant=0; ant< 73; ant++ ){
		uint ant=1;
		casa::Vector<double> raw_vector = raw_data.column(ant) ;
		
		casa::Matrix<DComplex> FFTdata = numooontrigger.zero_channel_flagging( raw_vector,
 											n_frames,
 											nyquist_zone);
 		Vector<double> zero_freq_removed = numooontrigger.ifft_data( FFTdata,
    			    						n_frames,
									nyquist_zone ) ;
									
		//casa::Vector<double> antenna_vector = zero_freq_removed.column(1);
		
		Matrix<DComplex> ppf_data = numooontrigger.ppf_implement(zero_freq_removed,
									nyquist_zone,
									ppfcoeff );
		uint ppf_row = ppf_data.nrow() ;
		uint ppf_col = ppf_data.ncolumn() ;
		
		//Vector<DComplex> ppf_vector = ppf_data.column(0) ;
		
		Matrix<double> ampl_ppf_array = amplitude(ppf_data) ;
	
		Vector<double> Averaged_PPF(513,0.0) ;
		
		for(uint i=30; i<550; i++){ 
			Averaged_PPF = Averaged_PPF + ampl_ppf_array.column(i) ;
		}
		Averaged_PPF = Averaged_PPF/520. ;
		
		ofstream logfile5;
 		logfile5.open("ampl_ppf_data",ios::out );
 	
		
 		for( uint n=0;n< 513; n++){
		
			for( uint m=30; m<550; m++){
		
			logfile5<< ampl_ppf_array(n,m) << "\t" ;
			}
		logfile5 << endl ;
		}
		
 	 	logfile5.close() ;

		
		ofstream logfile6;
 		logfile6.open("ave_PPF",ios::out );
 	
		
 		for( uint n=0;n< 513; n++){
						
			logfile6<< Averaged_PPF(n) << endl ;
			}
		
		logfile6.close() ;
*/
		Vector<double> ampl_PPF_vector(513,0.0) ;
		
		readAsciiVector( ampl_PPF_vector,"ave_PPF" ) ;
		
		ampl_PPF_vector = ampl_PPF_vector/520. ;
		
// 		Matrix<DComplex> sorted_ppf(513,100,0.0) ;
// 		uint j=0;
// 		for(uint i=20; i<120; i++){
// 		
// 			sorted_ppf.column(j)=ppf_data.column(i);
// 			j=j+1 ;
// 			}
		
// 		 Matrix<DComplex> rfi_removed = numooontrigger.RFI_removal( sorted_ppf ) ;
// 		 
// 		 uint fra= 150 ;
// 		 
// 		 Vector<uint> PPF_Vector = numooontrigger.PPFBand_vector( sampling_rate,
//    	   	       							  nyquist_zone,
// 		       							  freq_range ) ;
		 
// 		 Vector<double> rfi_cleaned = numooontrigger.ppf_inversion( rfi_removed,
//                                						    ppfcoeff_inv,
// 									   PPF_Vector )  ;	
// 		 //Matrix<double> rfi_cleaned = amplitude(rfi_removed) ;
// 		  Vector<double> rfi_cleaned_vector = numooontrigger.ifft_data( rfi_removed,
//     			    						fra,
// 									nyquist_zone ) ;
		//casa::Vector<double> sorted_vector = raw_vector(Slice(0, 50*1024)) ;
		
		//sorted_matrix.column(ant) = sorted_vector ;
		
		//}
		
		//Matrix<DComplex> RFI_removed(513,n_frames,0.0);
 		
		//casa::Matrix<DComplex> FFTdata(513,n_frames,0.0);
		
	//	casa::Vector<double> ampl_PPF_vector(513,0.0) ;
		 
		//casa::Vector<double> ampl_FFT_vector = rfi_cleaned ;
		//casa::Vector<double> IFFT_data(1024*n_frames,0.0) ;
		
	//	readAsciiVector( ampl_PPF_vector,"ave_PPF") ;
		
// 		double value(0.0) ;
// 		double av_value(0.0) ;
// 		
// 		for( uint i=120; i<220; i++ ){
// 			value =ampl_PPF_vector(i) ;
// 			av_value = av_value +value ;
// 			cout << "Average value : "<< av_value << "and value is " << value << endl ;
// 		}
// 		
// 		for( uint i=120; i<480; i++ ){
// 			ampl_PPF_vector(i) = av_value/100. ;
// 		}
// 		
//  		double amplitude_min = min( ampl_PPF_vector ) ;
//  		double amplitude_max = max( ampl_PPF_vector ) ;
//  		
//  		char outFileNameR[256] = "ampl_PPF_m.root" ;
//  		TFile *myOutput = new TFile(outFileNameR,"RECREATE");
//  		
//  		TH1D *AveragedSpectrum ; 
//  		AveragedSpectrum = new TH1D("AveragedSpectrum","AveragedSpectrum",513,1,513) ;
//   		
//   		AveragedSpectrum->SetMaximum( 2.08127e+08);//4400.59) ;//2.08127e+08);
// 		
// 		//AveragedSpectrum =new TH1D("f1","fitting first function",513,1,513) ;
//   		
//   		for( uint freq_channel =0; freq_channel< 513; freq_channel++){
//   		
//   			double amplitude_value = ampl_PPF_vector(freq_channel) ;
//   					
//   			AveragedSpectrum->SetBinContent(freq_channel+1,amplitude_value) ;
//   			
//   			}
		
 // 		AveragedSpectrum =new TH1D("AveragedSpectrum","AveragedSpectrum",513,0,30000) ;
  		
//    		for( uint freq_channel =0; freq_channel< 513; freq_channel++){
//    		
//    			double amplitude_value = ampl_PPF_vector(freq_channel) ;
//    					
//    			AveragedSpectrum->SetBinContent(freq_channel+1,amplitude_value) ;
//    			
//    			}
//  		double par[4];
// 		//double par2[9];
// 		
//  		TF1 *f1 = new TF1("f1","gaus",1,119) ;
// 		TF1 *f2 = new TF1("f2","pol0",221,513) ;
		//TF1 *f3 = new TF1("f3","expo",289,318) ;
		//TF1 *f4 = new TF1("f4","pol0",322,393) ;
		//TF1 *f5 = new TF1("f5","expo",397,410) ;
 		//TF1 *f6 = new TF1("f6","gaus",412,513);
		
// 		TF1 *total = new TF1("mstotal","gaus(0)+pol0(3)",1,513);
	//+pol0(8)+expo(9)+gaus(11)",1,513);//+expo(7)+gaus(9)",1,513);+pol0(8)+expo(9)
 			
//		total->SetLineColor(2) ;
		
//		AveragedSpectrum->Fit(f1,"R");
//		AveragedSpectrum->Fit(f2,"R+");
		//AveragedSpectrum->Fit(f3,"R+");
		//AveragedSpectrum->Fit(f4,"R+");
		//AveragedSpectrum->Fit(f5,"R+");
		//AveragedSpectrum->Fit(f6,"R+");
		//AveragedSpectrum->Fit(f7,"R+");
		//AveragedSpectrum->Fit(f8,"R+");
 		//f2->SetLineColor(4) ;
		
 //		f1->GetParameters(&par[0]) ;
 //		f2->GetParameters(&par[3]) ;
 		//f3->GetParameters(&par[6]) ;
		//f4->GetParameters(&par[8]) ;
		//f5->GetParameters(&par[9]) ;
		//f6->GetParameters(&par[11]) ;
		//f7->GetParameters(&par[11]) ;
		//f8->GetParameters(&par[13]) ;
		
 //		total->SetParameters(par) ;
		 		
//		AveragedSpectrum->Fit(total,"R+") ;
				
//		AveragedSpectrum->Write();
		
//*****************************************************		
		
		
 	//	for( uint antenna=0; antenna< 73; antenna++ ){
	
// 		uint antenna =0 ; 
//  	
  	//	casa::Vector<double> data_vector = raw_data.column(antenna);
//  		
// 		  
// 		casa::Matrix<DComplex>  FFTdata = numooontrigger.fft_data( data_vector,
//                                   					   5,
// 									   nyquist_zone ) ;
// 									   
// 		uint NR = FFTdata.nrow() ;
// 		uint NC = FFTdata.ncolumn() ;
// 		
// 		casa::Vector<DComplex> ampl_vector(NR,0.0) ;
// 		casa::Vector<double> ampl_FFT_vector(NR,0.0) ;
// 		
// 		for( uint i=0; i< NC; i++ ){
// 		
// 			ampl_vector = FFTdata.column(i) ;
// 			cout << "ampl vector has been calculated for frame "<<i<<endl ;
// 			//sorted_vector = ampl_vector(Slice(0,513)) ;
// 			ampl_FFT_vector= ampl_FFT_vector+ amplitude(ampl_vector);
// 			
// 		}
// 		//	RFI_removed = numooontrigger.RFI_removal( FFTdata ) ;
// 									  
//  		ofstream logfile5;
// 	logfile5.open("ampl_FFT",ios::out );
//  	
//  	for( uint n=0;n< NR; n++){
//  		logfile5<< ampl_FFT_vector(n)<< endl;
//  	}
//  	logfile5.close() ;

							
 				
// 		casa::Vector<double> IFFT_data= numooontrigger.ifft_data( FFTdata,
//   								  n_frames,
//   								  nyquist_zone ) ;
//  				
// 		zero_freq_removed.column(antenna) = IFFT_data ;
// 		
// 		}
// 		
			
			//ampl_fft_vector = ampl_fft_vector/100. ;
 		
 		//}
 		
	//	
		
// 	Matrix<double> ampl_PPF_data = amplitude(ppf_data) ;
// 	uint ppf_co = ampl_PPF_data.ncolumn();
// 	uint ppf_row = ampl_PPF_data.nrow();
// 	
// 	Vector<double> integrated_ampl(1024,0.0) ;
// 	
// 	for(uint co=25; co<75; co++){
// 		
// 		integrated_ampl = integrated_ampl + ampl_PPF_data.column(co) ;
// 		
// 		}	
// 		
// 	casa::Matrix<double> Cleaned_DATA = numooontrigger.Cleaned_data( zero_freq_removed ,
//     						    		     	   n_samples,
// 								     	   simTEC,
// 								     	   nyquist_zone,
// 								           sampling_rate,
// 								           TEC,
// 								           freq_range ) ; 
// 									   
// 	 Vector<double> beamedArray_wo = numooontrigger.BeamFormed_data(  Cleaned_DATA,
//     				   					n_samples,
// 			           					simTEC,
// 				   					nyquist_zone,
// 				   					sampling_rate,
// 				   					TEC,
// 				   					freq_range,
// 		  		   					pointing_latitude,
// 		  		   					pointing_longitude,
// 				   					gain_scale_factors,
// 		  		   					antenna_posX,
// 		  		   					antenna_posY,
// 		  		   					antenna_posZ ) ; 
// 	uint n_elem = beamedArray_wo.nelements() ;	
/*
 	ofstream logfile5;
 	logfile5.open("ampl_ppf_data",ios::out );
 	
 	for( uint n=0;n< 513; n++){
		for(uint m=0; m<73; m++){
 		logfile5<< ampl_PPF_data(n,m)<< "\t";
		}
		logfile5<<endl ;
 	}
 	logfile5.close() ;

	ofstream logfile6 ;
	logfile6.open("int_ampl_ppf",ios::out);

	for(uint g=0; g<513; g++){

	double values = integrated_ampl(g);

	logfile6 << values <<endl ;
	}
	logfile6.close() ;*/





// 	 
// 	 ofstream logfile1;
// 	logfile1.open("clean_data",ios::out );
// 	
// 	for( uint r=0; r<4096; r++){
// 		for( uint c=0; c< 73; c++){
// 		 logfile1<< Cleaned_DATA(r,c) << "\t";
// 		}
// 	 logfile1<< endl ;
// 	}
// 	logfile1.close();
// 	
// 
// 	casa::Matrix<double> Signal_Array = numooontrigger.Added_SignalData( zero_freq_removed , 
//     				      						n_samples,
// 				     						simTEC,
// 				     						nyquist_zone,
// 				     						peak_height,
// 				     						sampling_rate,
// 				      						TEC,
// 				     						source_latitude,
// 				     						source_longitude,
// 		  		     						pointing_latitude,
// 		  		     						pointing_longitude,
// 				     						gain_scale_factors,
// 			    	     						antenna_posX,
// 		   	    	     						antenna_posY,
// 		   	    	     						antenna_posZ,
// 				     						freq_range ) ;  	
// 								
// 	
// 	 ofstream logfile2;
// 	logfile2.open("added_data",ios::out );
// 	
// 	for( uint rr=0; rr<4096; rr++){
// 		for( uint cc=0; cc< 73; cc++){
// 		 logfile2<< Signal_Array(rr,cc) << "\t";
// 		}
// 	 logfile2<< endl ;
// 	}
// 	logfile2.close();
// 	
// 	 Vector<double> beamedArray = numooontrigger.BeamFormed_data( Signal_Array ,
//     				   					n_samples,
// 			           					simTEC,
// 				   					nyquist_zone,
// 				   					sampling_rate,
// 				   					TEC,
// 				   					freq_range,
// 		  		   					pointing_latitude,
// 		  		   					pointing_longitude,
// 				   					gain_scale_factors,
// 		  		   					antenna_posX,
// 		  		   					antenna_posY,
// 		  		   					antenna_posZ ) ; 
// 	uint n_ele = beamedArray.nelements() ;								
// 	ofstream logfile3;
// 	logfile3.open("Beamed",ios::out );
// 	
// 	for( uint n=0;n< n_ele; n++){
// 		logfile3<< beamedArray(n)<< endl;
// 	}
// 	logfile3.close() ;
 	
//   	numooontrigger.root_ntuples( zero_freq_removed,
//       					n_samples,
//   					simTEC,
 //  					sampling_rate,
  // 					nyquist_zone,
  // 					time_int_bins,
   //  					TEC,
  // 					source_latitude,
  // 					source_longitude,
  // 			    		pointing_latitude,
  // 		   			pointing_longitude,
  // 		   			gain_scale_factors,
  // 			    	  	antenna_posX,
  // 		   	    		antenna_posY,
  // 		   	    		antenna_posZ,
 // 					ppfcoeff,
  // 		   			ppfcoeff_inv,
  // 		  			freq_range,
  // 					peak_height )   ;  
  
 				
    	 numooontrigger.summary(); 
   
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
 return nofFailedTests;
}

// -----------------------------------------------------------------------------

int main ( int argc,
	  char *argv[])
{
  int nofFailedTests (0);
  
  std::string filename;
  if (argc > 1) {
    filename = std::string(argv[1]);
  //  filename2 = std::string(argv[2]);
  } else {
    std::cerr << "Please provide a HDF5 filename.\n";
    return(DAL::FAIL);
  }
  // Test for the constructor(s)
  nofFailedTests += test_constructors (filename);

  return nofFailedTests;
}
