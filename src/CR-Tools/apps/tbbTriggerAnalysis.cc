/*-------------------------------------------------------------------------*
 | $Id:: tbbTriggerAnalysis.cc 2605 2009-05-20 11:42:54Z acorstanje      $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2009                                                    *
 *   Arthur Corstanje (a.corstanje@astro.ru.nl)                            *
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
#include <Utilities/StringTools.h>
#include <Analysis/tbbTools.h>

#include <casa/Arrays/Matrix.h>

/* Namespace usage */
using std::cout;
using std::endl;
using CR::LOFAR_TBB;

/*!
 \file tbbTriggerAnalysis
 
 \ingroup CR_Apps
 
 \brief Application to test a given HDF5 file from LOFAR TBBs for triggers, following the FPGA online trigger.
 
 \author Arthur Corstanje
 
 \date 2009/10/29
 
 <h3>Usage</h3>
 
 \verbatim
 tbbTriggerAnalysis <filename>
 \endverbatim
 where <tt>filename</tt> points to a existing LOFAR TBB data set in HDF5
 format.
 */

typedef struct 
  {
    uint	itsRcuNr;
    uint	itsSeqNr;
    uint	itsTime;
    uint	itsSampleNr;
    uint	itsSum;
    uint	itsNrSamples;
    uint	itsPeakValue;
    uint        itsValueBefore;
    uint        itsValueAfter;
    uint	itsFlags;
  } TBBTrigger;

template <class T> casa::Vector<T> appendedVector(casa::Vector<T> first, casa::Vector<T> last)
{
  uint firstsize = first.shape()(0);
  uint lastsize = last.shape()(0);
  uint totalsize = firstsize + lastsize;
  
  casa::Vector<T> outVec = first; // copied or pointer-assigned ???
  
  outVec.resize(totalsize, True); // copy-values = true, otherwise vector is scratched...
  for(uint i=0; i<lastsize; i++)
  {
    outVec(i + firstsize) = last(i);
  }
  return outVec;
}

template <class T> void appendToVector(casa::Vector<T>& first, casa::Vector<T> last)
{ // appends 'last' to 'first' and result is in 'first'.
  uint firstsize = first.shape()(0);
  uint lastsize = last.shape()(0);
  uint totalsize = firstsize + lastsize;
  
  first.resize(totalsize, True);
  for(uint i=0; i<lastsize; i++)
  {
    first(i + firstsize) = last(i);
  }
}

void addFPGATriggersToList(uint rcu, Vector<Int> &index, Vector<Int> &sum, Vector<Int> &width, Vector<Int> &peak, 
                           Vector<Int> &meanval, Vector<Int> &afterval, Vector<TBBTrigger> &listToAppend, FILE * triggerOutputFile)
{
  Vector<TBBTrigger> newList;
  uint len = index.nelements();
  newList.resize(len, True);
  for(uint i = 0; i < len; i++)
  {
    TBBTrigger newTBBTrigger;
    newTBBTrigger.itsRcuNr = rcu;
    newTBBTrigger.itsSeqNr = index(i) / 1024;
    newTBBTrigger.itsTime = index(i) / 200000000;
    newTBBTrigger.itsSampleNr = index(i) % 200000000; // hardcoded sampling rate, get it out of here...
    newTBBTrigger.itsSum = sum(i);
    newTBBTrigger.itsNrSamples = width(i);
    newTBBTrigger.itsPeakValue = peak(i);
    newTBBTrigger.itsValueBefore = meanval(i);
    newTBBTrigger.itsValueAfter = afterval(i);
    
    newList(i) = newTBBTrigger;
  }
  if (triggerOutputFile != NULL)
  {
    for(uint i=0; i< newList.nelements(); i++)
    {
      TBBTrigger thisTrigger = newList(i);
//      triggerCount[thisTrigger.itsRcuNr]++;
      if (thisTrigger.itsRcuNr == 0)
      {
        //      cout << thisTrigger.itsRcuNr << ", " << thisTrigger.itsSampleNr << ", " << thisTrigger.itsSum << ", " << thisTrigger.itsNrSamples << ", "
        //     << thisTrigger.itsPeakValue << ", " << thisTrigger.itsValueBefore << endl;
      }
      printf("%d %u %u %u %u %u %u %u %u\n", thisTrigger.itsRcuNr, thisTrigger.itsSeqNr, thisTrigger.itsTime, thisTrigger.itsSampleNr, thisTrigger.itsSum, 
             thisTrigger.itsNrSamples, thisTrigger.itsPeakValue, thisTrigger.itsValueBefore, thisTrigger.itsValueAfter);
      
      fprintf(triggerOutputFile, "%d %u %u %u %u %u %u %u %u\n", thisTrigger.itsRcuNr, thisTrigger.itsSeqNr, thisTrigger.itsTime, thisTrigger.itsSampleNr, thisTrigger.itsSum, 
              thisTrigger.itsNrSamples, thisTrigger.itsPeakValue, thisTrigger.itsValueBefore, thisTrigger.itsValueAfter);
  
//      for (uint i=0; i<nofAntennas; i++)
//      {
//        cout << "RCU " << i << " had " << triggerCount[i] << " triggers." << endl;
//      }
    }
  } // end for
  
  
  appendToVector(listToAppend, newList);
}

// -----------------------------------------------------------------------------

/*!
 \brief Perform trigger analysis on HDF5 file. 
 
 \param filename -- Name of the HDF5 dataset to work with
 \param triggerList -- A list in which we output the trigger info
 */
void triggerAnalysisOnFile (std::string const &filename, Vector<TBBTrigger> &triggerList, FILE * triggerOutputFile)
{
  cout << "Starting trigger analysis on file: " << filename << endl;
  
  uint blocksize (1024 * 1);
  
  // create object to handle the data
  LOFAR_TBB data (filename,
		  blocksize);
  // cout << "Starting at block: " << data.block() << endl;

  uint nofBlocks = data.data_length()[0] / blocksize; // assuming same size for all antennas...
  
//  nofBlocks = 1000;
  cout << nofBlocks << " = blocks to be processed, blocksize = " << blocksize << endl;
  
  uint nofDipoles = data.nofDipoleDatasets();
//  data.toStartBlock(); // !! Reset file pointers to the start of the data !! Doesn't work properly !!
//  cout << "Reset to block: " << data.block() << endl;
  data.setBlock(0);
  cout << "Now set tot block: " << data.block() << endl;

  casa::Matrix<double> fx (data.blocksize(),
			   nofDipoles);
  Vector<double> tempArray;

  tbbTools * tbbToolsArray = new tbbTools[96];
  
  //  tbbTools * mySecondTbbTools = new tbbTools;
  //Vector<double> dataVector = data.column(0);
  Vector<int> index, sum, width, peak, meanval, afterval; // output for trigger function
  Vector<int> allindex, allsum, allwidth, allpeak, allmeanval, allafterval;
  uint nofAntennas = data.nofAntennas();
  
  for (uint block = 0; block < nofBlocks; block++) 
  {
    fx = data.fx();
    for (uint rcu = 0; rcu < nofAntennas; rcu++)
    {
      // get the data for the current block
      tempArray = fx.column(rcu); // overloaded operator: copies vector or pointer assignment?
      
//      for (uint k=0; k<10; k++)
//      {
//        cout << "RCU " << rcu << " sample " << k + block * blocksize << ": " << tempArray(k) << endl;
//      }
      
      bool reset = (block == 0) ? True : False;
      if (block % 10 == 0)
      {
        cout << "Block = " << block << endl;
        //    cout << tempArray[0] << " " << tempArray[1] << " " << tempArray[2] << "  " << tempArray[3] << endl;
        cout << "Triggering...";
      }
      bool ok = tbbToolsArray[rcu].meanFPGAtrigger(tempArray, 4, 8, 5, 4096, 0, index, sum, width, peak, meanval, afterval, reset);
      if (ok == False)
      {
        cerr << "Error doing TBBTools trigger algorithm!" << endl;
      }
      
      //bool ok2 = mySecondTbbTools->meanFPGAtrigger(tempArray, 3, 8, 5, 4096, 0, index, sum, width, peak, meanval, afterval, reset);
      if (block % 10 == 0)
      {
        cout << "appending stuff...";
      }
      if (index.shape()(0) > 0)
      {
        //  if (allindex.shape()(0) > 0)
        //    {
        //      index += allindex(allindex.shape()(0) - 1); // Wat zou dit moeten doen ???
        //    }
        for (uint j=0; j<index.nelements(); j++)
        {
          index(j) += block * blocksize;
        }
        appendToVector(allindex, index);
        appendToVector(allsum, sum);
        appendToVector(allwidth, width);
        appendToVector(allpeak, peak);
        appendToVector(allmeanval, meanval);
        appendToVector(allafterval, afterval);
        
        addFPGATriggersToList(rcu, index, sum, width, peak, meanval, afterval, triggerList, triggerOutputFile);
      } // end if
    } // end loop over RCUs   
    
    if (block % 10 == 0)
    {
      cout << "Done." << endl;
    }
    data.nextBlock();
  } // end loop over data blocks
  
  
  std::cout << "pulse count: " << allindex.shape() << std::endl;
  if (allindex.shape() >= 3)
  {
    std::cout << "pulse indices: " << allindex(0) << " " << allindex(1) << " " << allindex(2) << "..." << std::endl;
  }
  
  if (allindex.shape() > 0)
  {
    std::cout << "no.         Index       sum         width       peak        mean" << std::endl;
    uint sp=8; // spacing
    for(uint i=0; i<allindex.shape(); i++)
    { // show trigger results
      std::cout << setiosflags(ios::left);
      string empty = "    ";
      std::cout << setw(sp) << i << empty
      << setw(sp) << allindex(i) << empty
      << setw(sp) << allsum(i) << empty 
      << setw(sp) << allwidth(i) << empty
      << setw(sp) << allpeak(i) << empty
      << setw(sp) << allmeanval(i) << std::endl;
    }
  }	
  }

// -----------------------------------------------------------------------------

int main (int argc,
	  char *argv[])
{ 
  /*
   Check if filename of the dataset is provided on the command line; if not
   exit the program.
   */
  if (argc < 2) {
    std::cerr << "[tbbTriggerAnalysis] Too few parameters!" << endl;
    std::cerr << "" << endl;
    std::cerr << "  tbbTriggerAnalysis <filename>" << endl;
    std::cerr << "" << endl;
    return -1;
  }
  
  std::string filename = argv[1];
 
  Vector<TBBTrigger> triggerList; // the result of the analysis is stored here.
  char *outputFilename = "/Users/acorstanje/triggering/outputTriggers.dat";
  FILE * triggerFile = fopen(outputFilename, "w"); // overwrites existing file...
  
  triggerAnalysisOnFile(filename, triggerList, triggerFile);
  fclose(triggerFile);
  
  uint nofAntennas = 96;
  uint triggerCount[nofAntennas];
  for (uint i=0; i<nofAntennas; i++)
  { triggerCount[i]=0; }
  
 
  
    
  
  return 0;
}
