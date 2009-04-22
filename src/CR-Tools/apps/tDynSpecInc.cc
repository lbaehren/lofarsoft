/*-------------------------------------------------------------------------*
 | $Id:: tDynSpecInc.cc                  $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2009                                                    *
 *   Sander ter Veen (<mail>)                                            *
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

#include <crtools.h>
#include <Data/LOFAR_TBB.h>
#include <Display/SimplePlot.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
using namespace std;
/*
  \file ttbbctlIn.cc

  \ingroup CR_Applications

  \brief A file to make a dynamic spectrum of hdf5 data
 
  \author Sander ter Veen
 
  \date 2009/03/30
*/

// -----------------------------------------------------------------------------

/*!
  \brief Show the contents of the data array

  \param data -- Array with the ADC or voltage data per antenna
*/
void show_data (Matrix<Double> const &data)
{
  std::cout << "  Data shape : " << data.shape() << std::endl;
  std::cout << "  Data ..... : "
	    << data(1,1) << " "
	    << data(2,1) << " "
	    << data(3,1)
	    << std::endl;
}

#define CHANNELS        256
#define SAMPLES         768
/*
  \brief Structure to read the incoherent beam files 

*/
struct stokesdata {
  /* big endian (ppc) */
  unsigned int  sequence_number;

  /* little endian (intel) */
  float         samples[CHANNELS][SAMPLES|2];
};

void swap_endian( char *x )
{
  char c;

  c = x[0];
  x[0] = x[3];
  x[3] = c;


  c = x[1];
  x[1] = x[2];
  x[2] = c;
}



int main ()
{
  int nofFailedTests (0);
  FILE * pFile;
  pFile = fopen("/Users/STV/Astro/data/L2009_11117/SB26.MS","rb");
 // pFile = fopen("/Users/STV/Astro/data/SB25.MS.incoherentStokesI","rb");
  if (pFile==NULL) {cout << "File error" << endl;} 
   //construct the datatype
   unsigned num;
   unsigned time,channel;
   float sum, average;
   unsigned validsamples;
   stokesdata data;
   int kmax=1;
   casa::Matrix<double> matrix(CHANNELS,kmax*SAMPLES);
  for(int k=0;k<kmax;k++){ 
   num = fread( &data, sizeof data, 1, pFile);
   if( !num) {
        cout << "Error, EOF reached.\n";
   }
       swap_endian( (char*)&(data.sequence_number) );
   
  //     cout << data.sequence_number << endl;
   float validsamples = 0;
   float sum =0;
   for(int time=0;time<SAMPLES;time++)
   {
       
       //DAL::swapbytes((char*)&(data->sequence_number), 4);
       //  cout << &data->samples[0][j] << " ";
       
       for(int channel=0;channel<CHANNELS;channel++){
         const float value =  data.samples[channel][time] ; 
         matrix(channel,time+k * SAMPLES ) = value;
              
       //cout <<  endl;
       if( !isnan( value ) ) {
            sum += value;
            validsamples++;
       }
      }

   }
   //  cout << endl;
   float average= sum / validsamples;
   float average0;
   if(k==1){average0 = average;}
   if(average > (average0 + 50)) cout << "------------------------------------------------------------------------\n";
   cout << "k= " << k << " " << min(matrix) << " min | max " << max(matrix) << " average " << average << endl;
 
    // Plot the data to a PS file.

}
   matrix = log(matrix);
   if(1){   SimplePlot plot = SimplePlot();
    plot.CR::SimplePlot::quick2Dplot( "/Users/STV/Astro/Programming/debugging/DynSpecInc.ps" , matrix,	0,1, 0, 1, "time (arbitraty)", "freq (arbitrary)", "Dynamic spectrum from beamformed(?) data", True, 16, 0, 2, 1);
}

//} //k   
    
  return nofFailedTests;
}
