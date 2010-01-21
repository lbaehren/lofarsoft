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

#include <Data/UpSampledDR.h>

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  UpSampledDR::UpSampledDR ()  
    : DataReader (1){
    init();
  }
  
  void UpSampledDR::init(){
    iterator_p = NULL;
    inpDR_p = NULL;
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  UpSampledDR::~UpSampledDR ()
  {
    destroy();
  }
  
  void UpSampledDR::destroy() {
    if (iterator_p != NULL) { 
      delete [] iterator_p; 
      iterator_p=NULL; 
    };
    inpDR_p = NULL;
  }
  
  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  void UpSampledDR::summary (std::ostream &os)
  {;}
  
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================
  
  Bool UpSampledDR::setup(DataReader *inputDR, double newSampleFrequency, 
			  Bool UseCalFFT, SecondStagePipeline *inpPipeline){
    try {
      if (inputDR == NULL){
	cerr << "UpSampledDR::setup: " << "Need a valid DataReader pointer!" << endl;
	return False;
      };
      Double SampleFreqRatio = newSampleFrequency/inputDR->sampleFrequency();
      if (SampleFreqRatio < 1.) {
	cerr << "UpSampledDR::setup: " << "newSampleFrequency smaller than original!" << endl;
	return False;
      };
      if (abs(SampleFreqRatio-1.) < 1e-9) {
	cout << "UpSampledDR::setup: " << "New sample frequency identical to old one," << endl;
	cout << "                    " << "disabling upsampling." << endl;
	noUpsample = True;
	DataReader::setNyquistZone(inputDR->nyquistZone());
      } else {
	noUpsample = False;
	DataReader::setNyquistZone(1);
      };
      inpDR_p = inputDR;

      // --------------------------------------------
      // Functionality from the "setStreams" function
      // --------------------------------------------
      DataReader::setSampleFrequency(newSampleFrequency);
      int i,nAntennas,blocksize;
      nAntennas = inpDR_p->nofAntennas();
      blocksize = (int)floor(inpDR_p->blocksize()*SampleFreqRatio+0.5);
      // Setup the Iterators
      if (iterator_p != NULL) { delete [] iterator_p; iterator_p=NULL; };
      iterator_p = new DataIterator[nAntennas];
      for (i=0; i<nAntennas; i++){
        iterator_p[i].setDataStart(0);
        iterator_p[i].setStride(0);
        iterator_p[i].setShift(0);
	//Set the width of a step to 1, so we count in samples:
 	iterator_p[i].setStepWidthToSample();
 	iterator_p[i].setBlocksize(blocksize);
      }
      // Setup the blocksize and "Calibration" arrays
      uint fftLength(blocksize/2+1);      
      Matrix<DComplex> fft2calfft(fftLength,nAntennas,1.0);
      Vector<Double> adc2voltage(nAntennas,1.);      
      DataReader::init(blocksize,
		       adc2voltage,
		       fft2calfft);   
      setReferenceTime(inpDR_p->referenceTime());

      // --------------------------------------------
      // Generate the header record
      // --------------------------------------------
      setHeaderRecord();

      // Set "UseCalFFT_p"
      UseCalFFT_p = UseCalFFT;
      // Set "inpPipeline_p"
      // (Will automatically set inpPipeline_p to NULL if inpPipeline is NULL...)
      inpPipeline_p = inpPipeline;
      
    } catch (AipsError x) {
      cerr << "UpSampledDR::setup: " << x.getMesg() << endl;
      return False;
    }; 
    return True;
  }

  // ------------------------------------------------------------ setHeaderRecord
  
  bool UpSampledDR::setHeaderRecord(){
    try {
      header_p.mergeField(inpDR_p->headerRecord(),"Date", RecordInterface::OverwriteDuplicates);
      header_p.mergeField(inpDR_p->headerRecord(),"AntennaIDs", RecordInterface::OverwriteDuplicates);
      header_p.mergeField(inpDR_p->headerRecord(),"Observatory", RecordInterface::OverwriteDuplicates);
      header_p.mergeField(inpDR_p->headerRecord(),"dDate", RecordInterface::OverwriteDuplicates);
      int filesize = inpDR_p->headerRecord().asInt("Filesize");
      filesize = (int)floor(filesize*(DataReader::sampleFrequency()/inpDR_p->sampleFrequency())+0.5);
      header_p.define("Filesize",filesize);
      header_p.define("SampleFreq",DataReader::sampleFrequency());
    } catch (AipsError x) {
      cerr << "[UpSampledDR::setHeaderRecord] " << x.getMesg() << endl;
      return false;
    }; 
    return true;
  };

  //_____________________________________________________________________________
  //                                                                           fx
  
  /*!
    \return Matrix with the data
  */
  Matrix<Double> UpSampledDR::fx()
  {
    Matrix<Double> data;

    UpSampledDR::fx(data);

    return data;
  }

  //_____________________________________________________________________________
  //                                                                           fx

  /*!
    \retval Matrix with the data
  */
  void UpSampledDR::fx (Matrix<Double> &fx)
  {
    try {
      uint nofSelectedAntennas(DataReader::nofSelectedAntennas());
      if (nofSelectedAntennas >0) {
	uint antenna (0);
	uint sourceAnt (0);
	uint startsample (0);
	uint origSourceBlockNo (0);
	uint sourceBlockNo (0);
	uint sourceBlockSize=inpDR_p->blocksize(), sendBlocksize=DataReader::blocksize();
	uint restoreBlocksize, restoreFFTlen, sendoffset;
	Double SampleFreqRatio = DataReader::sampleFrequency()/inpDR_p->sampleFrequency();
	Vector<Bool> origSourceSelection, SSelection;
	Vector<Double> sourceFreqs, restoreFreqs;
	Matrix<DComplex> sourceFFT;
	Vector<DComplex> sourceFFTvec, restoreFFT;
	Vector<Double> restoreFX;
	startsample = iterator_p[selectedAntennas_p(0)].position();
	origSourceSelection  = inpDR_p->antennaSelection();
	SSelection.resize(origSourceSelection.shape());
	SSelection = True;
#ifdef DEBUGGING_MESSAGES      
	cout << "UpSampledDR::fx:" << " SSelection.shape: " << SSelection.shape() 
	     << ";" << inpDR_p->antennaSelection() << endl;
#endif
	inpDR_p->setSelectedAntennas(SSelection);

	origSourceBlockNo = inpDR_p->block();
	sourceBlockNo = (uint)floor((startsample/SampleFreqRatio)/sourceBlockSize);
	// Currently the DataReader and DataIterator start to count at 1
	inpDR_p->setBlock(sourceBlockNo);
	restoreBlocksize  = (uint)floor(sourceBlockSize*SampleFreqRatio+0.5);
	
	sourceFreqs = inpDR_p->frequencyValues();
	restoreFFTlen = restoreBlocksize/2+1;
	restoreFreqs.resize(restoreFFTlen);
	indgen(restoreFreqs); restoreFreqs *= DataReader::sampleFrequency()/restoreBlocksize;
	sendoffset = startsample - (uint)floor((sourceBlockNo*sourceBlockSize*SampleFreqRatio));
	if (sendoffset+sendBlocksize > restoreBlocksize) {
	  cerr << "UpSampledDR::fx: " << "Inconsistent parameters!" <<endl;
	  cerr << "                 " << "Requested block on source block boundary." << endl;
	  fx = Matrix<Double>();	  
	};
#ifdef DEBUGGING_MESSAGES      
	cout << "UpSampledDR::fx:" << " SampleFreqRatio: " << SampleFreqRatio 
	     << " sourceBlockSize: " << sourceBlockSize 
	     << " restoreBlocksize: " << restoreBlocksize << endl;
	cout << "                " << " startsample: " << startsample
	     << " sourceBlockNo: " << sourceBlockNo 
	     << " sendoffset: " << sendoffset << endl;
#endif
	fx.resize(sendBlocksize,nofSelectedAntennas);
	if ( inpPipeline_p != NULL) {
#ifdef DEBUGGING_MESSAGES      
	  cout << "UpSampledDR::fx:" << "Getting data from pipeline. " << endl;	  
#endif
	  sourceFFT = inpPipeline_p->GetData(inpDR_p);
	} else if (UseCalFFT_p) {
#ifdef DEBUGGING_MESSAGES      
	cout << "UpSampledDR::fx:" << "Getting data from calfft(). " << endl;
#endif
	  sourceFFT = inpDR_p->calfft();
	} else {
#ifdef DEBUGGING_MESSAGES      
	  cout << "UpSampledDR::fx:" << "Getting data from fft(). " << endl;
#endif
	  sourceFFT = inpDR_p->fft();
	}
	FFTServer<Double,DComplex> fftserv(IPosition(1,restoreBlocksize), FFTEnums::REALTOCOMPLEX);
	restoreFX.resize(restoreBlocksize);

	Double minSourceFreq=min(sourceFreqs),maxSourceFreq=max(sourceFreqs);
	for (antenna=0; antenna<nofSelectedAntennas; antenna++) {
	  sourceAnt = selectedAntennas_p(antenna);
	  if (	startsample != iterator_p[sourceAnt].position()){
	    cerr << "UpSampledDR::fx: Different startsample for Ant:" << sourceAnt
		 << " and Ant:" << selectedAntennas_p(0) << "!" << endl;
	    cerr << "                 Not implemented yet!!!" << endl;
	  }
	  sourceFFTvec.reference(sourceFFT.column(sourceAnt));
	  if (noUpsample) {
	    restoreFX =  inpDR_p->invfft(sourceFFTvec);
	  } else {
	    //sourceFFTvec(0) = 0.; sourceFFTvec(sourceFFTvec.nelements()-1) = 0.;
            sourceFFTvec(0) = sourceFFTvec(0)/2.;
            sourceFFTvec(sourceFFTvec.nelements()-1) /= 2.;
            //sourceFFTvec(sourceFFTvec.nelements()-1) = sourceFFTvec(sourceFFTvec.nelements()-1)/2.;
	    InterpolateArray1D<Double, DComplex>::interpolate(restoreFFT,restoreFreqs,
							      sourceFreqs,sourceFFTvec,
				    InterpolateArray1D<Double, DComplex>::nearestNeighbour);
	    restoreFFT(restoreFreqs<minSourceFreq) = 0.;
	    restoreFFT(restoreFreqs>maxSourceFreq) = 0.;
            restoreFFT(0) = restoreFFT(0)*2.;
            restoreFFT(restoreFFTlen-1) = restoreFFT(restoreFFTlen-1)*2.;
	    restoreFFT = restoreFFT*SampleFreqRatio;
	    fftserv.fft(restoreFX,restoreFFT);
	  };
	  fx.column(antenna) = restoreFX(Slice(sendoffset,sendBlocksize));
	};
	
	inpDR_p->setBlock(origSourceBlockNo);
	inpDR_p->setSelectedAntennas(origSourceSelection);
      };
    } catch (AipsError x) {
      cerr << "UpSampledDR::fx: " << x.getMesg() << endl;
    }; 
  }
  

} // Namespace CR -- end
