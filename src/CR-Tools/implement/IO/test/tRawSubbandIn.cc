/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2008                                                    *
 *   Andreas Horneffer (<mail>)                                            *
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

#include <IO/RawSubbandIn.h>

using CR::RawSubbandIn;  // Namespace usage

/*!
  \file tRawSubbandIn.cc

  \ingroup IO

  \brief A collection of test routines for the RawSubbandIn class
 
  \author Andreas Horneffer
 
  \date 2008/03/07
*/

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new RawSubbandIn object

  \return nofFailedTests -- The number of failed tests.
*/
int test_RawSubbandIn ()
{
  int nofFailedTests (0);
  
  std::cout << "\n[test_RawSubbandIn]\n" << std::endl;

  std::cout << "[1] Testing default constructor ..." << std::endl;
  try {
    RawSubbandIn newRawSubbandIn;
    //

    newRawSubbandIn.attachFile("/mnt/lofar/kalpana/raw1.out");
    
    Matrix<DComplex> data;
    data = newRawSubbandIn.getData(1.204812466000e+09,10,0);

    cout << endl << data(0,0) << ";" << data(0,1) << ";" << data(1,0) << endl;
    // This works with casa, but not with casacore
    //cout << endl << data << endl;
    
    /*        RawSubbandIn::FileHeader FileHead;
    RawSubbandIn::BlockHeader BlockHead;

    FILE *fd = fopen("/mnt/lofar/kalpana/raw1.out","r");

    Bool ok;

    ok = newRawSubbandIn.readFileHeader(fd,FileHead);
    cout << "readFileHeader successfull: "<<ok<<endl;
    cout << "FileHeadlen: "<< hex << sizeof(RawSubbandIn::FileHeader) << endl;
    cout << "FileHead.magic: " << FileHead.magic << endl;
    cout << "FileHead.bitsPerSample: " << dec << (int)FileHead.bitsPerSample << endl;
    cout << "FileHead.nrPolarizations: " << (int)FileHead.nrPolarizations << endl;
    cout << "FileHead.nrBeamlets: " << FileHead.nrBeamlets << endl;
    cout << "FileHead.nrSamplesPerBeamlet: " << FileHead.nrSamplesPerBeamlet << endl;
    cout << "FileHead.station: " << FileHead.station << endl;
    cout << "FileHead.sampleRate: " << hex << FileHead.sampleRate << endl;
    cout << "FileHead.subbandFrequencies: " << FileHead.subbandFrequencies[0] << ","
	 << FileHead.subbandFrequencies[1] << ","<< FileHead.subbandFrequencies[2] << ","
	 << FileHead.subbandFrequencies[3] << ","<< FileHead.subbandFrequencies[4] << ","
	 << FileHead.subbandFrequencies[17] << ","<< FileHead.subbandFrequencies[18] << ","
	 << FileHead.subbandFrequencies[52] << ","<< FileHead.subbandFrequencies[53] << ","<< endl;
    cout << "FileHead.beamDirections: " << FileHead.beamDirections[0][0] << ","
	 << FileHead.beamDirections[0][1] << "; " << FileHead.beamDirections[1][0] << ","
	 << FileHead.beamDirections[1][1] << "; " << FileHead.beamDirections[2][0] << ","
	 << FileHead.beamDirections[2][1] << ";" << endl;
    cout << "FileHead.beamlet2beams: " << dec << FileHead.beamlet2beams[0] << "-" 
	 << FileHead.beamlet2beams[17] << "-" << FileHead.beamlet2beams[18] << "-" 
	 << FileHead.beamlet2beams[53] << endl;

    ok = newRawSubbandIn.readBlockHeader(fd,BlockHead);
    cout << endl << "readBlockHeader successfull: "<<ok<<endl;
    cout << "BlockHead.magic: " << hex << BlockHead.magic << endl;
    cout << "BlockHead.coarseDelayApplied: " << dec<< BlockHead.coarseDelayApplied[0] << ","
	 << BlockHead.coarseDelayApplied[1] << "," << BlockHead.coarseDelayApplied[2] << ","
	 << BlockHead.coarseDelayApplied[3] << endl;
    cout << "BlockHead.fineDelayRemainingAtBegin: " << BlockHead.fineDelayRemainingAtBegin[0] << ","
	 << BlockHead.fineDelayRemainingAtBegin[1] << ","<< BlockHead.fineDelayRemainingAtBegin[2] << ","
	 << BlockHead.fineDelayRemainingAtBegin[3] << endl;
    cout << "BlockHead.fineDelayRemainingAfterEnd: " << BlockHead.fineDelayRemainingAfterEnd[0] << ","
	 << BlockHead.fineDelayRemainingAfterEnd[1] << ","<< BlockHead.fineDelayRemainingAfterEnd[2] << ","
	 << BlockHead.fineDelayRemainingAfterEnd[3] << endl;    
    cout << "BlockHead.time: " << hex <<BlockHead.time[0] << endl 
	 << "Seq-No: " << dec << BlockHead.time[0] *(1./195312.5) << endl;
    printf("Time: %20.12e \n",BlockHead.time[0]/FileHead.sampleRate);
    //    cout << "BlockHead.: " << BlockHead. << endl;
    //    cout << "BlockHead.: " << BlockHead. << endl;
    */

  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int main ()
{
  int nofFailedTests (0);

  // Test for the constructor(s)
  {
    nofFailedTests += test_RawSubbandIn ();
  }

  return nofFailedTests;
}
