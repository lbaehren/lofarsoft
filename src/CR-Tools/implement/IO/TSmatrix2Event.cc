/*-------------------------------------------------------------------------*
 | $Id                                                                   $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                  *
 *   Andreas Horneffer (<mail>)                                                     *
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

#include <IO/TSmatrix2Event.h>

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  TSmatrix2Event::TSmatrix2Event() {
    eventheader_p = (lopesevent_v1*)malloc(LOPESEV_HEADERSIZE);
    if (eventheader_p == NULL) {
      cerr << " TSmatrix2Event::TSmatrix2Event() : Failed to allocate memory!!!"
	   << endl;
      exit(EXIT_FAILURE);
    };
    reset();
  }
  
  Bool TSmatrix2Event::reset(void){
    eventheader_p->length      = 0;
    eventheader_p->version     = VERSION;
    eventheader_p->JDR         = 0;
    eventheader_p->TL          = 0;
    eventheader_p->type        = TIM40;
    eventheader_p->evclass     = simulation;
    eventheader_p->blocksize   = 0;
    eventheader_p->presync     = 0;
    eventheader_p->LTL         = 0;
    eventheader_p->observatory = 0; // meaning LOPES
    hasData_p = False;
    hasDate_p = False;
    return True;
  };

  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  TSmatrix2Event::~TSmatrix2Event ()
  {
    destroy();
  }
  
  void TSmatrix2Event::destroy(){
    if (eventheader_p != NULL) { free(eventheader_p);}
  }

  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  void TSmatrix2Event::summary (std::ostream &os)
  {;}
  
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================
  
  Bool TSmatrix2Event::SetData(Matrix<double> data,
			       Vector<int> AntIDs,
			       int presync){
    try {
      if (data.ncolumn() != AntIDs.nelements()){
	cerr << "TSmatrix2Event::SetData : " << "Inconsistent data: (data.ncolumn() != AntIDs.nelements()) !" << endl;
	return False;
      };
      if (data.nrow() > MAXSAMPLESIZE){
	cerr << "TSmatrix2Event::SetData : " << "Data blocksize too large!" << endl;
	return False;
      };
      data_p.resize(data.shape());
      //multiply with sqrt(R)*ADCSteps_per_Volt to get to ADC-Values
      //convert to Integer (short) for quantisation
      convertArray(data_p,(data*sqrt(50.)*2048.));
      AntIDs_p.resize(AntIDs.shape());
      AntIDs_p = AntIDs;
      eventheader_p->blocksize = data.nrow();
      eventheader_p->presync = presync;
      hasData_p = True;

    } catch (AipsError x) {
      cerr << "TSmatrix2Event::SetData: " << x.getMesg() << endl;
      return False;
    }; 
    return True; 
  };

  Bool TSmatrix2Event::SetDate (double date){
    try {
      eventheader_p->JDR = uInt(date);
      eventheader_p->TL  = uInt((date-eventheader_p->JDR)*5e6);
      eventheader_p->LTL = uInt((date-eventheader_p->JDR)*40e6);
      hasDate_p = True;
    } catch (AipsError x) {
      cerr << "TSmatrix2Event::SetDate: " << x.getMesg() << endl;
      return False;
    }; 
    return True; 
  };
  

  Bool TSmatrix2Event::WriteEvent(String filename){
    try {
      if (!hasData_p || !hasDate_p){
	cerr << "TSmatrix2Event::WriteEvent: " << "Data or date not set!" << endl;
	return False;	
      };
      int blocksize,nAnt;
      blocksize = data_p.nrow();
      nAnt = data_p.ncolumn();
      if (blocksize != eventheader_p->blocksize ) {
	cerr << "TSmatrix2Event::WriteEvent: " << "Inconsitent data: (blocksize != eventheader_p->blocksize)!" << endl;
	return False;	
      };
      const char *filename_c = filename.c_str();
      FILE *fd = fopen(filename_c,"w");
      if (fd == NULL) {
	cerr << "TSmatrix2Event::WriteEvent: " << "Failed to open output file!" << endl;
	return False;	
      };
      cout << "TSmatrix2Event::WriteEvent: mark1"<<endl;
      lopesevent *tmpevent;
      tmpevent = (lopesevent *)malloc(sizeof(lopesevent));
      if (tmpevent == NULL){
	cerr << "TSmatrix2Event::WriteEvent: " << "Failed to allocate temporary memory!" << endl;
	fclose(fd);
	return False;	
      };
      cout << "TSmatrix2Event::WriteEvent: mark2"<<endl;
      // copy that header into the temporary storage
      tmpevent->length      = eventheader_p->length;
      tmpevent->version     = eventheader_p->version;
      tmpevent->JDR         = eventheader_p->JDR;
      tmpevent->TL          = eventheader_p->TL;
      tmpevent->type        = eventheader_p->type;
      tmpevent->evclass     = eventheader_p->evclass;
      tmpevent->blocksize   = eventheader_p->blocksize;
      tmpevent->presync     = eventheader_p->presync;
      tmpevent->LTL         = eventheader_p->LTL;
      tmpevent->observatory = eventheader_p->observatory;

      cout << "TSmatrix2Event::WriteEvent: mark3"<<endl;      
      cout << "Space: " << sizeof(lopesevent) << " Header: " << LOPESEV_HEADERSIZE 
	   << " Data: " << ((blocksize*2+8)*nAnt) << " Shape: " << data_p.shape() << endl;
      unsigned int chan,i;
      short int *datapoint;
      datapoint = tmpevent->data;
      cout << "Start: used bytes:" << ((char *)datapoint)-((char *)tmpevent) << " " << sizeof(short int) <<endl; 
      // copy that data into the temporary storage
      for (chan=0; chan < nAnt; chan++) {
	// Some dirty typecasting and pointer calculations! 
	// If you know a better way -> tell me!
	*((unsigned int *)datapoint) =  AntIDs_p(chan);
	datapoint += sizeof(int)/sizeof(short);
	*((unsigned int *)datapoint) = blocksize;
	datapoint += sizeof(int)/sizeof(short);
	for (i=0; i<blocksize; i++) {
	  //	  *datapoint = data_p.column(chan)(i);
	  *datapoint = data_p(i,chan);
	  datapoint++;
	};
	cout << "Chann:"<<chan<<" ID:"<<AntIDs_p(chan) 
	     << " used bytes:" << ((char *)datapoint)-((char *)tmpevent) << endl; 
      }; //for (chan=0;...)
      tmpevent->length = sizeof(lopesevent)-MAXDATASIZE+(datapoint-tmpevent->data)*sizeof(short);
      cout << "TSmatrix2Event::WriteEvent: mark4"<<endl;
      
      // write to the file
      i = fwrite(tmpevent, 1, tmpevent->length, fd);
      fclose(fd); // close the file

      // free temporary space
      free(tmpevent);

    } catch (AipsError x) {
      cerr << "TSmatrix2Event::WriteEvent: " << x.getMesg() << endl;
      return False;
    }; 
    return True; 

  };




} // Namespace CR -- end
