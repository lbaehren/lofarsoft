/***************************************************************************
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


// CGuipipeline includes
#include <Data/tbbctlIn.h>

// Glish includes
#include <casa/aips.h>
#include <casa/string.h>
#include <casa/Arrays.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Utilities/Sort.h>

// general includes
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

/*!
  \file tbbctlResort.cc
  
  \ingroup CR_Applications

  \brief An application to sort the blocks in a tbbctl-dump accordint to time.
  
  \author Andreas Horneffer
  
  \date 2008/07/09
  
  <h3>Prerequisite</h3>
    
  
  <h3>Synopsis</h3>
  
  <h3>Example(s)</h3>
  
*/

using namespace CR;


Bool resortTbbctlFile(string infile, string outfile, Bool allowHole){
  try {
    uint ui,start,noSamples;
    uint block,maxblock=0,startdate=0,stopdate=0,startsample=0,stopsample=0;
    uint olddate,oldsample;
    uint numblocks, firstblock;
    double tmpdouble,sampleFreq;
    size_t outsize;
    tbbctl_head *headerpoint;
    struct stat filestat;
    unsigned char *tmppoint=NULL;
    FILE *fd,*of; 
    
    //Allocate memory
    tmppoint = (unsigned char*)malloc(TBBCTL_BLOCK_SIZE);
    if (tmppoint == NULL) {
      cerr << "tbbctlResort:resortTbbctlFile: Error while allocating temporary memory " << endl;
      return False;	
    };
    headerpoint = (tbbctl_head*)tmppoint;
    stat(infile.c_str(), &filestat);
    numblocks = filestat.st_size / TBBCTL_BLOCK_SIZE;
    if (numblocks < 1) {
      cerr << "tbbctlResort:resortTbbctlFile: File " << infile << "too small (smaller than one blocksize)." <<endl;
      return False;
    };
    //	cout << " working on file: " << infile << endl;
    fd = fopen(infile.c_str(),"r");
    if (fd == NULL) {
      cerr << "tbbctlResort:resortTbbctlFile: Can't open file: " << infile << endl;
      return False;
    };
    ui = fread(headerpoint, 1, sizeof(tbbctl_head), fd);
    if ( (ui != sizeof(tbbctl_head) )  ) {
      cerr << "tbbctlResort:resortTbbctlFile: Inconsitent file: " << infile << endl;
      fclose(fd);
      return False;
    };
    startdate = headerpoint->Date;
    startsample = headerpoint->SampleNr;
    stopdate = headerpoint->Date;
    stopsample = headerpoint->SampleNr + headerpoint->NoSamples;
    maxblock = headerpoint->NoSamples;
    sampleFreq = headerpoint->sampleFreq*1e6;
    firstblock =0;
    olddate = headerpoint->Date;
    oldsample = headerpoint->SampleNr;
    for (block=1; block<numblocks; block++){
      fseek(fd, (block*(long)TBBCTL_BLOCK_SIZE), SEEK_SET); 
      ui = fread(headerpoint, 1, sizeof(tbbctl_head), fd);
      if ( (ui != sizeof(tbbctl_head) )  ) {
	cerr << "tbbctlResort:resortTbbctlFile: Inconsitent file: " << infile << endl;
	fclose(fd);
	return False;
      };
      noSamples = (headerpoint->Date-olddate)*200000000+headerpoint->SampleNr-oldsample;
      if (noSamples != 1024) {
	cout << "olddate: " << olddate << " oldsample: " << oldsample
	     << "  newdate: " << headerpoint->Date << " newsample: " << headerpoint->SampleNr 
	     << " diff: " << noSamples << endl;
      };
      if (((headerpoint->Date-olddate)==1)&&((headerpoint->SampleNr-oldsample)==1024)) {
	headerpoint->Date--;
      };
      olddate = headerpoint->Date;
      oldsample = headerpoint->SampleNr;
      if (headerpoint->Date < startdate) {
	startdate = headerpoint->Date;
	startsample = headerpoint->SampleNr;
	firstblock = block;
      } else if ((headerpoint->Date==startdate) && (headerpoint->SampleNr<startsample)){
	startsample = headerpoint->SampleNr;
	firstblock = block;
      };
      if (headerpoint->Date > stopdate) {
	stopdate = headerpoint->Date;
	stopsample = headerpoint->SampleNr + headerpoint->NoSamples;
      } else if ((headerpoint->Date==stopdate) &&
		 ( (headerpoint->SampleNr+ headerpoint->NoSamples) > stopsample)){
	stopsample = headerpoint->SampleNr + headerpoint->NoSamples;
      }
      //      if ((stopsample+1024)>sampleFreq) { stopdate--; };
      if (headerpoint->NoSamples != maxblock) { 
	cout << "tbbctlResort:resortTbbctlFile: " << "inconsistent block-size: " 
	     << headerpoint->NoSamples << " != " << maxblock << endl;
      };
      //cout << "file:" << infile <<" Date:" << headerpoint->Date << " start:" << headerpoint->SampleNr << endl;
    };
    fclose(fd);
    // end of first loop through the file
    tmpdouble = (stopdate-startdate)*sampleFreq+stopsample-startsample;
    noSamples = (uint)tmpdouble;
    cout << "startdate: " << startdate << " startsample: " << startsample
	 << " stopdate: " << stopdate << " stopsample: " << stopsample << endl;
    cout << " noSamples: " << noSamples << " noSamples/maxblock: " << noSamples/maxblock 
	 << endl;
    if (noSamples/maxblock != numblocks ){
      cout << "tbbctlResort:resortTbbctlFile: " << " Discontinous data: noSamples/maxblock(" 
	   << noSamples/maxblock << ") != numblocks(" << numblocks << ") !" << endl;
      if (! allowHole) {
	return False;
      };
    };
    outsize = (noSamples/maxblock)*TBBCTL_BLOCK_SIZE;
    // open outfile
    of = fopen(outfile.c_str(),"w");
    if (of == NULL) {
      cout << "tbbctlResort:resortTbbctlFile: " << " Failed to open output file!" << endl;
      return False;
    };
    if (ftruncate(fileno(of),outsize) != 0){
      cout << "tbbctlResort:resortTbbctlFile: " << " Failed to set size of output file!" << endl;
      fclose(of);
      return False;      
    };
    fd = fopen(infile.c_str(),"r");
    for (block=0; block<numblocks; block++){
      fseek(fd, (block*(long)TBBCTL_BLOCK_SIZE), SEEK_SET); 
      fread(tmppoint, 1, TBBCTL_BLOCK_SIZE, fd);
      headerpoint = (tbbctl_head*)tmppoint;
      if (((headerpoint->Date-olddate)==1)&&((headerpoint->SampleNr-oldsample)==1024)) {
	cout << "tbbctlResort:resortTbbctlFile: Adjusting broken date-header!" <<endl;
	headerpoint->Date--;
      };
      olddate = headerpoint->Date;
      oldsample = headerpoint->SampleNr;
      start = (uint)((headerpoint->Date-startdate)*sampleFreq + 
		     headerpoint->SampleNr-startsample+
		     ((headerpoint->Date%2)*512))/maxblock;
      if ( (start>=0) && (start*TBBCTL_BLOCK_SIZE < outsize)){
	cout << " Date:" << headerpoint->Date << " start:" << headerpoint->SampleNr 
	     << " -> Block " << start
	     << endl;
	fseek(of, (start*(long)TBBCTL_BLOCK_SIZE), SEEK_SET);
	ui = fwrite(tmppoint, 1, TBBCTL_BLOCK_SIZE, of);
      } else {
	cerr << "tbbctlResort:resortTbbctlFile: Error, strange block!" << endl;
      };
    };
    fclose(of);
    fclose(fd);
    free(tmppoint);  
  } catch (AipsError x) {
    cerr << "tbbctlResort:resortTbbctlFile: " << x.getMesg() << endl;
    return False;
  }; 
  return True;
  
};



int main (int argc, char *argv[]) {
  string infilename, outfilename;	// Files to be read in
  
  try {
    Record confRec;
    Vector<String> files;

    std::cout << "tbbctlResort execution started!" << endl;

    // Check correct number of arguments (2 + program name =  3)
    if ((argc < 3) || (argc > 3)) {
      std::cerr << "Wrong number of arguments in call of \"tbbctlResort\". The correct format is:\n";
      std::cerr << "tbbctlResort infile outfile\n" << std::endl;
      return 1;				// Exit the program
    };
    
    // First argument is the file to be read
    infilename.assign(argv[1]);

    // Second argument is the file to write
    outfilename.assign(argv[2]);

    cout << "Resorting: " <<  resortTbbctlFile(infilename,outfilename,True) << endl;

  } catch (std::string message) {
  std::cerr << "tbbctlResort: " << message << std::endl;
  }
  
  return 0;
}
