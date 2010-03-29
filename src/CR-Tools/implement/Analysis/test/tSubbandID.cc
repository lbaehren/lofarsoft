/***************************************************************************
 *   Copyright (C) 2007                                                    *
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

/* $Id$*/

#include <casa/aips.h>
#include <casa/iostream.h>
#include <casa/iostream.h>
#include <casa/iomanip.h>
#include <casa/namespace.h>
#include <casa/Arrays.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/BasicMath/Random.h>
#include <casa/BasicSL/Complex.h>
#include <casa/Exceptions/Error.h>

#include <Analysis/SubbandID.h>

using CR::SubbandID ;
/*!
  \file tSubbandID.cc

  \ingroup Analysis

  \brief A collection of test routines for SubbandID
 
  \author Kalpana Singh
 
  \date 2007/06/05
*/

Double sampling_freq            = 160e6 ;
Double first_subband_freq       = 24218750 ;
uint band_ID                    = 155 ;
uint nofSubbands                 = 24 ;
Double subband_frequencies [24] = { 24218750, 24375000, 24531250, 24687500,
				    24843750, 25000000, 25156250, 25312500,
				    25468750, 25625000, 25781250, 25937500,
				    28593750, 28750000, 28906250, 29062500,
				    29218750, 29375000, 29531250, 29687500,
				    29843750, 30000000, 30156250, 30312500 };

//_______________________________________________________________________________
//                                                                 test_SubbandID

/*!
  \brief Test constructors for a new SubbandID object

  \return nofFailedTests -- The number of failed tests.
*/
int test_SubbandID ()
{
  cout << "\n[tSubbandID::test_SubbandID]\n" << endl;

  int nofFailedTests (0);
  
  try {
    SubbandID subbandID;
  }
  catch ( AipsError x ) {
    cerr << x.getMesg() << endl;
    ++nofFailedTests;
  }
  return nofFailedTests ;
}

//_______________________________________________________________________________
//                                                                test_SubbandIDs

Bool test_SubbandIDs ()
{ 
  Bool ok(True) ;
  casa::Vector<double> band_frequencies (nofSubbands);

  try {
    for(uint i=0; i<nofSubbands; i++ ) {
      band_frequencies(i)= subband_frequencies [i] ;
    }
    
    SubbandID sub_band_ID_1(sampling_freq, first_subband_freq, nofSubbands );
    SubbandID sub_band_ID_2(sampling_freq, band_ID, nofSubbands );
    Vector<Double> subband_freq(nofSubbands, 0.0 );
    Vector<uint> subband_ID( nofSubbands, 0 );
    Vector<uint> band_IDS(nofSubbands, 0 );
    
    subband_freq = sub_band_ID_2.calcFrequency( sampling_freq, 
						band_ID,
						nofSubbands ) ;
    
     cout << " subband frequencies :" << subband_freq << endl ;
     
     subband_ID = sub_band_ID_1.calcSubbandID( sampling_freq,
					       first_subband_freq,
					       nofSubbands ) ;
     
     cout << "-- subband IDs        = " << subband_ID << endl;
     cout << "-- Vector of band IDs = " << band_IDS   << endl;		  
   }
   catch( AipsError x ){
     cerr << x.getMesg() << endl ;
     ok = False ;
   }
   return ok ;	
   
 }

// -----------------------------------------------------------------------------

int main ()
{
  Bool ok (True);
  Int retval(0);
  if(ok){
  ok = test_SubbandIDs();
  
  if(!ok){
   retval = 1;
   cout << "Error..............early exit " << endl ;
   }
  }
return retval ;
}

 
