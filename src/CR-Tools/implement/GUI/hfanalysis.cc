//#define DBG_MODE 0
//#define DBG_MODE 1

// CASA/casacore header files

//using casa::Bool;
//using casa::Double;
//using casa::Matrix;
//using casa::String;
//using casa::Vector;
//using casa::Array;

using namespace std;


#include <GUI/hfanalysis.h>

//Some definitions needed for the preprosessor programming
//$FILENAME: HFILE=hfwrappers-$SELF.h  // Copy source code (where specified with COPY_TO HFILE START) to the file hwrappers-hfanalysis.cc.h

#undef HF_PP_FILETYPE
#define HF_PP_FILETYPE() (CC)  // Tell the preprocessor (for generating wrappers) that this is a c++ source code file (brackets are crucial)

/* Some explanations...

See hfanalysis.h for actual definition

#define STLVectorIteratorT typename vector<T>::iterator
#define STLVectorIteratorI typename vector<HInteger>::iterator
#define STLVectorIteratorN typename vector<HNumber>::iterator
#define STLVectorIteratorC typename vector<HComplex>::iterator
#define IterValueType typename Iter::value_type

//Example on how to create a casa Vector with data storage identical to the stl vector:

HInteger * storage = &(Offsets[0]);
casa::IPosition shape(1,Offsets.size()); //tell casa the size of the vector
CasaVector<HInteger> OffsetsCasa(shape,storage,casa::SHARE); 

*/

//Needed to create a Python wrapper for a function of a templated STL
//vector of Integer, Number, Complex, and String type containing 0
//extra parameters


#define PythonWrapper_VecINCS_0_Parameters(FUNC) \
  void FUNC (vector<HPointer > &vec) {} \
  void (*FUNC##_I)(vector<HInteger> &vec) = &FUNC; \
  void (*FUNC##_N)(vector<HNumber > &vec) = &FUNC; \
  void (*FUNC##_C)(vector<HComplex > &vec) = &FUNC; \
  void (*FUNC##_S)(vector<HString > &vec) = &FUNC
#define PythonWrapper_VecINC_VecOut_0_Parameters(FUNC) \
  void FUNC (vector<HPointer > &vec,vector<HPointer > &vec) {}	       \
  void (*FUNC##_I)(vector<HInteger> &vec,vector<HInteger> &vec) = &FUNC;		       \
  void (*FUNC##_N)(vector<HNumber > &vec) = &FUNC; \
  void (*FUNC##_C)(vector<HComplex > &vec) = &FUNC; \
  void (*FUNC##_S)(vector<HString > &vec) = &FUNC
#define PythonWrapper_TVecINC_0_Parameters(FUNC)     \
  HPointer FUNC (vector<HPointer > &vec) {return Null_p;}\
  HString FUNC (vector<HString > &vec) {return "";}\
  HString (*FUNC##_S)(vector<HString > &vec) = &FUNC;\
  HInteger (*FUNC##_I)(vector<HInteger> &vec) = &FUNC;	\
  HNumber (*FUNC##_N) (vector<HNumber > &vec) = &FUNC;	\
  HComplex (*FUNC##_C)(vector<HComplex > &vec) = &FUNC
#define PythonWrapper_VecINCS_1_TParameters(FUNC)  \
  void (*FUNC##_I)(vector<HInteger> &vec,HInteger) = &FUNC;    \
  void (*FUNC##_N)(vector<HNumber > &vec,HNumber) = &FUNC;	   \
  void (*FUNC##_C)(vector<HComplex > &vec,HComplex) = &FUNC; \
  void (*FUNC##_S)(vector<HString > &vec,HString) = &FUNC
#define PythonWrapper_VecINCS_8_Parameters(FUNC,VAL1,VAL2,VAL3,VAL4,VAL5,VAL6,VAL7,VAL8)   \
  void (*FUNC##_I)(vector<HInteger> &vec,VAL1,VAL2,VAL3,VAL4,VAL5,VAL6,VAL7,VAL8) = &FUNC; \
  void (*FUNC##_N)(vector<HNumber > &vec,VAL1,VAL2,VAL3,VAL4,VAL5,VAL6,VAL7,VAL8) = &FUNC; \
  void (*FUNC##_C)(vector<HComplex > &vec,VAL1,VAL2,VAL3,VAL4,VAL5,VAL6,VAL7,VAL8) = &FUNC; \
  void (*FUNC##_S)(vector<HString > &vec,VAL1,VAL2,VAL3,VAL4,VAL5,VAL6,VAL7,VAL8) = &FUNC
#define PythonWrapper_VecINCSP_8_Parameters(FUNC,VAL1,VAL2,VAL3,VAL4,VAL5,VAL6,VAL7,VAL8)   \
  void (*FUNC##_I)(vector<HInteger> &vec,VAL1,VAL2,VAL3,VAL4,VAL5,VAL6,VAL7,VAL8) = &FUNC; \
  void (*FUNC##_N)(vector<HNumber > &vec,VAL1,VAL2,VAL3,VAL4,VAL5,VAL6,VAL7,VAL8) = &FUNC; \
  void (*FUNC##_C)(vector<HComplex > &vec,VAL1,VAL2,VAL3,VAL4,VAL5,VAL6,VAL7,VAL8) = &FUNC; \
  void (*FUNC##_P)(vector<HPointer > &vec,VAL1,VAL2,VAL3,VAL4,VAL5,VAL6,VAL7,VAL8) = &FUNC; \
  void (*FUNC##_S)(vector<HString > &vec,VAL1,VAL2,VAL3,VAL4,VAL5,VAL6,VAL7,VAL8) = &FUNC

#define VecWrappers_TFunc_VecCopy_0_Parameters(FUNC)			\
  template <class T> inline T FUNC(vector<T> &vec) {vector<T> vec_copy(vec.begin(),vec.end()); return FUNC(vec_copy.begin(),vec_copy.end());} \
  template <class T> inline T FUNC(casa::Vector<T> &vec) {vector<T> vec_copy(vec.cbegin(),vec.cend()); return FUNC(vec_copy.begin(),vec_copy.end());} 

#define VecWrappers_TFunc_Vec_0_Parameters(FUNC)			\
  template <class T> inline T FUNC(vector<T> &vec) {return FUNC(vec.begin(),vec.end());} \
  template <class T> inline T FUNC(casa::Vector<T> &vec) {return FUNC(vec.begin(),vec.end());} 

// template <class T> inline T FUNC(casa::Vector<T> &vec) {return FUNC<STLVectorIteratorT,T>(static_cast<STLVectorIteratorT>(vec.cbegin()),static_cast<STLVectorIteratorT>(vec.cend()));} 

//------------------------------------------------------------------------
// Conversion Rountines
//------------------------------------------------------------------------

/*!
  \brief The function converts a column in an aips++ matrix to an stl vector
 */

template <class S, class T>
void aipscol2stlvec(casa::Matrix<S> &data, vector<T>& stlvec, const HInteger col){
    HInteger i,nrow,ncol;
//    vector<HNumber>::iterator p;
    
    nrow=data.nrow();
    ncol=data.ncolumn();
    //    if (ncol>1) {MSG("aipscol2stlvec: ncol="<<ncol <<" (nrow="<<nrow<<")");};
    if (col>=ncol) {
	ERROR("aipscol2stlvec: column number col=" << col << " is larger than total number of columns (" << ncol << ") in matrix.");
	stlvec.clear();
	return;
    }

    stlvec.resize(nrow);
    CasaVector<S> CASAVec = data.column(col);
    
//    p=stlvec.begin();
    
    for (i=0;i<nrow;i++) {
//	*p=mycast<T>(CASAVec[i]); 
	stlvec[i]=mycast<T>(CASAVec[i]); 
//	p++;
    };
}

/*!
  \brief The function converts an aips++ vector to an stl vector
 */

template <class S, class T>
void aipsvec2stlvec(CasaVector<S>& data, vector<T>& stlvec){
    HInteger i,n;
//    vector<R>::iterator p;
    
    n=data.size();
    stlvec.resize(n);
//    p=stlvec.begin();
    for (i=0;i<n;i++) {
//	*p=mycast<T>(data[i]); 
	stlvec[i]=mycast<T>(data[i]); 
//	p++;
    };
}



//------------------------------------------------------------------------
// I/O DataReader Access functions
//------------------------------------------------------------------------


/*! 
Function to close a file with a datareader object providing the pointer to the object as an integer
*/
void hCloseFile(HIntPointer iptr) {
  union{void* ptr; CR::DataReader* drp;};
  ptr=AsPtr(iptr);
  if (ptr!=Null_p) delete drp;
}

/*! 
  Function to open a file with a datareader object and returning the pointer to the object as an integer
 */

HIntPointer hOpenFile(HString Filename, vector<HInteger> & Offsets) {
  
  bool opened;
  union{HIntPointer iptr; void* ptr; CR::DataReader* drp; CR::LOFAR_TBB* tbb; CR::LopesEventIn* lep;};

  //Create the a pointer to the DataReader object and store the pointer
      
  DBG("DataFunc_CR_dataReaderObject: Opening File, Filename=" << Filename);
  HString Filetype = determine_filetype(Filename);
  if (Filetype=="LOPESEvent") {
    lep = new CR::LopesEventIn;
    DBG("DataFunc_CR_dataReaderObject: lep=" << ptr << " = " << reinterpret_cast<HInteger>(ptr));
    opened=lep->attachFile(Filename);
    cout << "File="<<Filename;lep->summary();
  } else if (Filetype=="LOFAR_TBB") {
    tbb = new CR::LOFAR_TBB(Filename,1024);
    DBG("DataFunc_CR_dataReaderObject: tbb=" << ptr << " = " << reinterpret_cast<HInteger>(ptr));
    opened=tbb!=NULL;
    MSG("Opening File="<<Filename);tbb->summary();
    if (opened) {
      CasaVector<int> OffsetsCasa = tbb->sample_offset();
      aipsvec2stlvec(OffsetsCasa, Offsets);
      };
  } else {
    ERROR("DataFunc_CR_dataReaderObject: Unknown Filetype = " << Filetype  << ", Filename=" << Filename);
    opened=false;
  }
  
  if (!opened){
    ERROR("DataFunc_CR_dataReaderObject: Opening file " << Filename << " failed.");
    return AsIPtr(Null_p);
  };

  
  //Read the data Header, containing important information about the file (e.g. size)
  CasaRecord hdr=drp->headerRecord();
  uint i,nfields=hdr.nfields();
  //  for (i=0; i<nfields; i++) MSG("hdr name="<<hdr.name(i) << " type="<<hdr.dataType(i));

  //return value
  return iptr;
}

//------------------------------------------------------------------------
/*! Read data from a Datareader object (pointer in iptr) into an stl vector 


Example on how to use this with the PYthon wrapper

offsets=IntVec()
datareader_ptr=hOpenFile("/Users/falcke/LOFAR/usg/data/lofar/RS307C-readfullsecond.h5", offsets)

fdata=FloatVec()
idata=IntVec()
cdata=ComplexVec()
sdata=StringVec()
Datatype="Fx"
Antenna=1
Blocksize=1024
Block=10
Stride=0
Shift=0
hReadFile(idata,datareader_ptr,Datatype,Antenna,Blocksize,Block,Stride,Shift,offsets)
hCloseFile(datareader_ptr)

The data will then be in the vector idata. You can covert that to a
Python list with l=vec2list(idata) (defined in hfinit.py)

 */
template <class T> 
void hReadFile(vector<T> & vec,
	       const HIntPointer iptr,
	       const HString Datatype,
	       const HInteger Antenna,
	       const HInteger Blocksize,
	       const HInteger Block,
	       const HInteger Stride,
	       const HInteger Shift,
	       vector<HInteger> & Offsets) 
{
  
  DataReader *drp=reinterpret_cast<DataReader*>(iptr); 
  
  //First retrieve the pointer to the pointer to the dataRead and check whether it is non-NULL.
  if (drp==Null_p){
    ERROR("dataRead: pointer to FileObject is NULL, DataReader not found."); 
    return;
  };

//!!!One Needs to verify somehow that the parameters make sense !!!
  if (Antenna > drp->nofAntennas()-1) {
    ERROR("Requested Antenna number too large!");
    return;
  };
  drp->setBlocksize(Blocksize);
  drp->setBlock(Block);
  drp->setStride(Stride);
  if (Offsets.size()>0) {
    drp->setShift(Shift-Offsets[Antenna]);
  } else {
    drp->setShift(Shift);
  };

  CasaVector<uint> antennas(1,Antenna);
  //  drp->setSelectedAntennas(antennas);

//    Vector<uint> selantennas=drp->selectedAntennas();
//    MSG("No of Selected Antennas" << drp->nofSelectedAntennas ()<< " SelectedAntennas[0]=" <<selantennas[0]);

  address ncol;

  #define copy_ary2vp  ncol=ary.ncolumn(); /* MSG("ncol="<<ncol<<", Antenna="<<Antenna); */ if (ncol>1 && Antenna<ncol) aipscol2stlvec(ary,vec,Antenna); else aipscol2stlvec(ary,vec,0); 

  //#define copy_ary2vp  ncol=ary.ncolumn(); /* MSG("ncol="<<ncol<<", Antenna="<<Antenna); */ if (ncol>1 && Antenna<ncol) *vp2=ary.column(Antenna).tovec(); else *vp2=ary.column(0).tovec(); dp->noMod(); dp->put(*vp2)

  if (Datatype=="Time") {
    //    vector<HNumber>* vp2; *vp2 = drp->timeValues().tovec();
    CasaVector<double> val = drp->timeValues();
    aipsvec2stlvec(val,vec);
  }
  else if (Datatype=="Frequency") {
    //vector<HNumber>* vp2; *vp2 = drp->frequencyValues().tovec();
    CasaVector<double> val = drp->frequencyValues();
    aipsvec2stlvec(val,vec);
  }
  else if (Datatype=="Fx") {
    //    vector<HNumber>* vp2 = new vector<HNumber>;
    CasaMatrix<CasaNumber> ary=drp->fx();
    copy_ary2vp;
  }
  else if (Datatype=="Voltage") {
    //    vector<HNumber>* vp2 = new vector<HNumber>;
    CasaMatrix<CasaNumber> ary=drp->voltage();
    copy_ary2vp;
  }
  else if (Datatype=="invFFT") {
    //    vector<HNumber>* vp2 = new vector<HNumber>;
    CasaMatrix<CasaNumber> ary=drp->invfft();
    copy_ary2vp;
  }
  else if (Datatype=="FFT") {
    //    vector<HComplex>* vp2 = new vector<HComplex>;
    CasaMatrix<CasaComplex> ary=drp->fft();
    copy_ary2vp;
  }
  else if (Datatype=="CalFFT") {
    //    vector<HComplex>* vp2 = new vector<HComplex>;
    CasaMatrix<CasaComplex> ary=drp->calfft();
    copy_ary2vp;
  }
  else {
    ERROR("DataFunc_CR_dataRead: Datatype=" << Datatype << " is unknown.");
    vec.clear();
    return;
  };
  return;
}

/*
  else if (Datatype=="Fx") {aipscol2stlvec(drp->fx(),*vp,0);}
  else if (Datatype=="Voltage") {aipscol2stlvec(drp->voltage(),*vp,0);}
  else if (Datatype=="invFFT") {aipscol2stlvec(drp->invfft(),*vp,0);}
  else if (Datatype=="FFT") {aipscol2stlvec(drp->fft(),*vp,0);}
  else if (Datatype=="CalFFT") {aipscol2stlvec(drp->calfft(),*vp,0);}
*/

PythonWrapper_VecINCSP_8_Parameters(hReadFile,\
				   const HIntPointer iptr,\
				   const HString Datatype,\
				   const HInteger Antenna,\
				   const HInteger Blocksize,\
				   const HInteger Block,\
				   const HInteger Stride,\
				   const HInteger Shift,\
				   vector<HInteger> & Offsets);

/*========================================================================
  hfanalysis
  ========================================================================
  Collection of basic algorithms to operate on vectors
*/


/*!
  Implementation of the Gaussfunction
  \param x     - Position at which the Gaussian is evaluated
  \param sigma - Spread of the Gaussian
  \param mu    - Median value of the Gaussian
*/
inline HNumber funcGaussian (HNumber x,
			     HNumber sigma,
			     HNumber mu)
{
  return exp(-(x-mu)*(x-mu)/(2*sigma*sigma))/(sigma*sqrt(2*casa::C::pi));
};

/*!
  \brief Returns vector of weights of length len with constant weights normalized to give a sum of unity. Can be used by hRunningAverageT.
*/
vector<HNumber> hFlatWeights (address wlen) {
  vector<HNumber> weights(wlen,1.0/wlen);
  return weights;
}

/*!  \brief Returns vector of weights of length wlen with linearly
rising and decreasing weights centered at len/2.

 The vector is normalized to give a sum of unity. Can be used by
hRunningAverageT.
 */
vector<HNumber> hLinearWeights(address wlen){
  vector<HNumber> weights(wlen,0.0);
  address i,middle=wlen/2;
  HNumber f,sum=0.0;

  //This is not the most effective way of doing things for linear
  //weights but that allows one to adapt this easily to other functions
  for (i=0; i<wlen; i++) {
    f=1.0-abs(middle-i)/(middle+1.0);
    weights[i]=f;
    sum+=f;
  };
  //Normalize to unity
  for (i=0; i<wlen; i++) weights[i]=weights[i]/sum;
  return weights;
}

/*!  \brief Returns vector of weights of length wlen with Gaussian
     distribution centered at len/2 and width sigma.

 The vector is normalized to give a sum of unity. Can be used by hRunningAverageT.
 */
vector<HNumber> hGaussianWeights(address wlen){
  vector<HNumber> weights(wlen,0.0);
  address i,middle=wlen/2;
  HNumber f,sum=0.0;

  //This is not the most effective way of doing things for linear
  //weights but that allows one to adapt this easily to other functions
  for (i=0; i<wlen; i++) {
    f=funcGaussian(i,max(middle/3.0,1.0),middle);
    weights[i]=f;
    sum+=f;
  };
  //Normalize to unity
  for (i=0; i<wlen; i++) weights[i]=weights[i]/sum;
  return weights;
}

vector<HNumber> hWeights(address wlen, hWEIGHTS wtype){
  vector<HNumber> weights;
  if (wlen<1) wlen=1;
  switch (wtype) {
  case WEIGHTS_LINEAR:
    weights=hLinearWeights(wlen);
    break;
  case WEIGHTS_GAUSSIAN:
    weights=hGaussianWeights(wlen);
    break;
  default: //  WEIGHTS_FLAT:
    weights=hFlatWeights(wlen);
  };
  return weights;
}

//_______________________________________________________________________________
//                                                               hRunningAverageT

/*!
  \param idata_start: STL Iterator pointing to the first element of an array with
         input values.
  \param idata_end: STL Iterator pointing to the end of the input vector
  \param odata_start: STL Iterator pointing to the first element of an array
         which will contain output values. The array must have the same size as
	 *start_data_in and be initialized to zero.
  \param weights_start: STL Iterator pointing to the first element of an array
         with weights values that allows one to specify weights to be applied
	 to the neighbouring samples before averaging. The weights should be
	 normalized to unity.
  \param  weights_end: STL Iterator pointing to the end (last plus one element!)
          of an array with weights

  Attention: Note that it is assumed that the output array is initialized, has
  the same length as the input array, and is initialized with zeros!! If the
  array values are nonzero, they will simply be added to the input vector. This
  is done so that the memory allocation of a vector/array is handled by the user,
  hence different types of arrays/vectors can be used.
*/

template <class T>
void hRunningAverageT (const STLVectorIteratorT idata_start,
		       const STLVectorIteratorT idata_end,
		       const STLVectorIteratorT odata_start,
		       const STLVectorIteratorN weights_start,
		       const STLVectorIteratorN weights_end)
{
  address l=(weights_end-weights_start);
  /* Index of the central element of the weights vector (i.e., where it
     typically would peak) */
  address middle=l/2;
  /* To avoid too many rounding errors with Integers */
  T fac = l*10;

  STLVectorIteratorT dit;
  STLVectorIteratorT dit2;
  STLVectorIteratorT din=idata_start;
  STLVectorIteratorT dout=odata_start;
  vector<HNumber>::iterator wit;

  while (din!=idata_end) {
    dit=din-middle; //data iterator set to the first element to be taken into account (left from current element)
    wit=weights_start; // weight iterators set to beginning of weights
    while (wit!=weights_end) {
      if (dit<idata_start) dit2=idata_start;
      else if (dit>=idata_end) dit2=idata_end-1;
      else dit2=dit;
      *dout=*dout+((*dit2)*(*wit)*fac);
      ++dit; ++wit;
    };
    *dout/=fac;
    ++dout; ++din; //point to the next element in data input and output vector
  };
  return;
} 

//_______________________________________________________________________________
//                                                               hRunningAverageT

/*!
  \brief Overloaded function to automatically calculate weights
*/
template <class T> 
void hRunningAverageT (const STLVectorIteratorT idata_start,
		       const STLVectorIteratorT idata_end,
		       const STLVectorIteratorT odata_start,
		       address wlen,
		       hWEIGHTS wtype)
{
  vector<HNumber> weights=hWeights(wlen,wtype);
  hRunningAverageT<T> (idata_start,
		       idata_end,
		       odata_start,
		       weights.begin(),
		       weights.end());
}

//_______________________________________________________________________________
//                                                         hRunningAverageCasaVec

/*!
  \brief Wrapper to hRunningAverageT for CASA Vectors
*/
template <class T>
void hRunningAverageCasaVec (casa::Vector<T> &vec_in,
			     casa::Vector<T> &vec_out,
			     address wlen,
			     hWEIGHTS wtype)
{
  //  vec_out.assign(vec_in.size(),mycast<T>(0.0));
  typedef STLVectorIteratorT Tit;
  hRunningAverageT<T> (static_cast<Tit>(vec_in.cbegin()),
		       static_cast<Tit>(vec_in.cend()),
		       static_cast<Tit>(vec_in.cend()),
		       wlen,
		       wtype);
  return;
} 

//_______________________________________________________________________________
//                                                             hRunningAverageVec

/*!
  \brief Wrapper to hRunningAverageT for STL Vectors
*/
template <class T>
void hRunningAverageVec (vector<T> &vec_in,
			 vector<T> &vec_out,
			 address wlen,
			 hWEIGHTS wtype)
{
  vec_out.assign(vec_in.size(),mycast<T>(0.0));
  hRunningAverageT<T> (vec_in.begin(),
		       vec_in.end(),
		       vec_out.begin(),
		       wlen,
		       wtype);
  return;
} 

//_______________________________________________________________________________
//                                                             hRunningAverageVec

/*!
  \brief Dummy function used for formal reasons by the GUI
*/
void hRunningAverageVec (vector<HString> &vec_in,
			 vector<HString> &vec_out,
			 address wlen,
			 hWEIGHTS wtype)
{
  vec_out=vec_in;
  MSG("hRunningAverageVec<HString>: not yet implemented");
}

//_______________________________________________________________________________
//                                                             hRunningAverageVec

/*!
  \brief Dummy function used for formal reasons by the GUI
*/
void hRunningAverageVec (vector<HPointer> &vec_in,
			 vector<HPointer> &vec_out,
			 address wlen,
			 hWEIGHTS wtype)
{
  vec_out=vec_in;
  MSG("hRunningAverageVec<HPointer>: not implemented");
}

/*!
  
  \brief Calculate the running average of an array of Numbers. Wrapper
  for hRunningAverageT using pointers instead of iterators.
  
  start_data_in: Pointer to the first element of an array with input values.
  
  start_data_out: Pointer to the first element of an array which will contain input values. The array must have the same size as *start_data_in and be initialized to zero.
  
  datalen: length of input and output array
  
  start_weights: Pointer to the first element of an array with weights
  values that allows one to specify weights to be applied to the
  neighbouring samples before averaging. The weights should be
  normalized to unity.
  
  weightslen: number of weights
  
  Attention: Note that it is assumed that the output array is
  initialized, has the same length as the input array, and is
  initialized with zeros!! If the array values are nonzero, they will
  simply be added to the input vector. This is done so that the memory
  allocation of a vector/array is handled by the user, hence different
  types of arrays/vectors can be used.
*/

template <class T>
void hRunningAverageN (T * start_data_in,
		       T * start_data_out,
		       address datalen,
		       HNumber * start_weights,
		       address weightslen)
{
  /* Set up the iterators which are passed to the underlying function */
  typedef STLVectorIteratorT Tit;
  typedef STLVectorIteratorN Nit;
  Tit idata_start=static_cast<Tit>(start_data_in);
  Tit odata_start=static_cast<Tit>(start_data_out);
  Nit weights_start=static_cast<Nit>(start_weights);
  /* Compute running average */
  hRunningAverageT (idata_start,
		    idata_start+datalen,
		    odata_start,
		    weights_start,
		    weights_start+weightslen);
}

/*!
  \brief This is needed to produce a Boost Python wrapper to the templated function hRunningAverage for a specific type. 
  
  Those definitions should actually better go into the .hpp file
*/

void (*hRunningAverageVec_N)(vector<HNumber > &vec_in,vector<HNumber > &vec_out, address wlen, hWEIGHTS wtype) = &hRunningAverageVec;
void (*hRunningAverageVec_I)(vector<HInteger> &vec_in,vector<HInteger> &vec_out, address wlen, hWEIGHTS wtype) = &hRunningAverageVec;
void (*hRunningAverageVec_C)(vector<HComplex> &vec_in,vector<HComplex> &vec_out, address wlen, hWEIGHTS wtype) = &hRunningAverageVec;

/*
  Actually you need to define new functions that take 2 or 3 parameters only to makes use of parameter defaulting in Python...
  
  void (*hRunningAverageVec_N_3)(vector<HNumber > &vec_in,vector<HNumber > &vec_out, address len) = &hRunningAverageVec;
  void (*hRunningAverageVec_I_3)(vector<HInteger> &vec_in,vector<HInteger> &vec_out, address len) = &hRunningAverageVec;
  void (*hRunningAverageVec_C_3)(vector<HComplex> &vec_in,vector<HComplex> &vec_out, address len) = &hRunningAverageVec;
  void (*hRunningAverageVec_N_2)(vector<HNumber > &vec_in,vector<HNumber > &vec_out) = &hRunningAverageVec;
  void (*hRunningAverageVec_I_2)(vector<HInteger> &vec_in,vector<HInteger> &vec_out) = &hRunningAverageVec;
  void (*hRunningAverageVec_C_2)(vector<HComplex> &vec_in,vector<HComplex> &vec_out) = &hRunningAverageVec;
*/

//------------------------------------------------------------------------
//Collection of simple functions operating on Vectors
//------------------------------------------------------------------------


//_______________________________________________________________________________
//                                                               hNegate

//$COPY_TO HFILE START --------------------------------------------------
#include "hfpp-undef.cc"
#define HF_PP_FUNCNAME hNegate  //The Name of the function
//.......................................................................
#define HF_PP_FUNCBRIEF "Multiplies each element in the vector with -1."
#define HF_PP_FUNCTYPE void     //Return value type of function
#define HF_PP_NVECS 1 //Number of (input/output) vectors
#define HF_PP_NPAR 0  //Number of other parameters
#define HF_PP_GUI_LIBRARY Math
#include "hfdefaultwrappercode.h"
//$COPY_TO END --------------------------------------------------
/*!

  \brief The function will multiply each element in the vector with
  -1. The input vector is also the output vector

  \param data_start: STL Iterator pointing to the first element of an array with
         input values.
  \param data_end: STL Iterator pointing to the end of the input vector
*/
template <class Iter> 
void hNegate(const Iter data_start,const Iter data_end)
{
  typedef IterValueType T;
  Iter it=data_start;
  T fac = mycast<T>(-1);
  while (it!=data_end) {
    *it=hf_mul(*it,fac);
    ++it;
  };
}

/*!

  \brief Fills a vector with a constant number

  \param data_start: STL Iterator pointing to the first element of an array with
         input values.
  \param data_end: STL Iterator pointing to the end of the input vector
*/
//$COPY_TO HFILE START --------------------------------------------------
#include "hfpp-undef.cc"
#define HF_PP_FUNCNAME hFill  //The Name of the function
//.......................................................................
#define HF_PP_FUNCBRIEF "Fills a vector with a constant value."
#define HF_PP_FUNCTYPE void  //Return value type of function
#define HF_PP_GUI_LIBRARY Math
#define HF_PP_NVECS 1 //Number of (input/output) vectors
#define HF_PP_NPAR 1  //Number of other parameters
#define HF_PP_PAR0 (fill_value,HInteger,0,"Value to fill vector with.",HF_PP_TEMPLATED_TYPE,HF_PP_PASS_AS_VALUE)  //Definition of input parameter
#define HF_PP_NO_GUI 1 //Don't generate a wrapper for the GUI (use overloaded function) 
#define HF_PP_GUI_LIBRARY Math
#include "hfdefaultwrappercode.h"
//$COPY_TO END --------------------------------------------------
/*!

  \brief Fills a vector with a constant value.

  \param data_start: STL Iterator pointing to the first element of an array with
         input values.
  \param data_end: STL Iterator pointing to the end of the input vector

  \param fill_value: Value to fill vector with
*/
template <class Iter> 
void hFill(const Iter data_start,const Iter data_end, const IterValueType fill_value)
{
  Iter it=data_start;
  while (it!=data_end) {
    *it=fill_value;
    ++it;
  };
}


//$COPY_TO HFILE START --------------------------------------------------
#include "hfpp-undef.cc"
#define HF_PP_FUNCNAME hSum  //The Name of the function
//.......................................................................
#define HF_PP_FUNCBRIEF "Performs a sum over the values in a vector and returns the value"
#define HF_PP_GUI_LIBRARY Math
#define HF_PP_FUNCTYPE_T 1     //Return value type of function is templated with T
#define HF_PP_NVECS 1 //Number of (input/output) vectors
#define HF_PP_NPAR 0  //Number of other parameters
#define HF_PP_GUI_LIBRARY Math
#define HF_PP_GUI_RETURN_POLICY  HF_PP_GUI_RETURN_SCALAR
#include "hfdefaultwrappercode.h"
//$COPY_TO END --------------------------------------------------

/*!
  \brief Performs a sum over the values in a vector and returns the value

The functions contains no check for an overrun!

  \param data_start: STL Iterator pointing to the first element of an array with input
  values.  

  \param data_end: STL Iterator pointing to the end of the
  input vector
*/
template <class Iter> 
IterValueType hSum(const Iter data_start,const Iter data_end)
{
  typedef IterValueType T;
  T sum=mycast<T>(0);
  Iter it=data_start;
  while (it<=data_end) {sum+=*it; ++it;};
  return sum;
} 

//$COPY_TO HFILE START --------------------------------------------------
#include "hfpp-undef.cc"
#define HF_PP_FUNCNAME hMean  //The Name of the function
//.......................................................................
#define HF_PP_FUNCBRIEF "Returns the mean value of all elements in a vector"
#define HF_PP_GUI_LIBRARY Math
#define HF_PP_FUNCTYPE_T 1     //Return value type of function is templated with T
#define HF_PP_NVECS 1 //Number of (input/output) vectors
#define HF_PP_NPAR 0  //Number of other parameters
#define HF_PP_GUI_LIBRARY Math
#define HF_PP_GUI_RETURN_POLICY  HF_PP_GUI_RETURN_SCALAR
#include "hfdefaultwrappercode.h"
//$COPY_TO END --------------------------------------------------

/*!
  \brief Returns the mean value of all elements in a vector

  \param data_start: STL Iterator pointing to the first element of an array with
         input values.
  \param data_end: STL Iterator pointing to the end of the input vector

*/
template <class Iter> 
IterValueType hMean (const Iter data_start,const Iter data_end)
{
  typedef IterValueType T;
  T mean=hSum(data_start,data_end);
  if (data_end!=data_start) mean/=mycast<T>(data_end-data_start); 
  //The cast is neccesary since my compiler refuses to divide a
  //complex number (actually Casa?) by an integer - weird!
  return mean;
} 


//$COPY_TO HFILE START --------------------------------------------------
#include "hfpp-undef.cc"
#define HF_PP_FUNCNAME hMedian  //The Name of the function
//.......................................................................
#define HF_PP_FUNCBRIEF "Returns the median value of the elements in a vector."
#define HF_PP_FUNCTYPE_T 1     //Return value type of function is templated with T
#define HF_PP_NVECS 1 //Number of (input/output) vectors
#define HF_PP_NPAR 0  //Number of other parameters
#define HF_PP_GUI_LIBRARY Math
#define HF_PP_GUI_RETURN_POLICY  HF_PP_GUI_RETURN_SCALAR
#define HF_PP_VEC_WRAPPER_CODE_STL \
  vector<T> vec_copy(HF_PP_VECTORITERATOR_STL(N,0,N));\
  return hMedian(vec_copy.begin(),vec_copy.end())	   
#define HF_PP_VEC_WRAPPER_CODE_CASA \
  vector<T> vec_copy(HF_PP_VECTORITERATOR_CASA(N,0,N));\
  return hMedian(vec_copy.begin(),vec_copy.end())			
HF_PP_GENERATE_WRAPPERS

#undef HF_PP_FUNCNAME
#undef HF_PP_FUNCBRIEF
//Define a 2nd convenience wrapper function - needs a different name 
#define HF_PP_ALT_FUNCNAME hMedian
#define HF_PP_FUNCNAME hMedianSort  //The Name of the function
#define HF_PP_FUNCBRIEF "Returns the median value of the elements in a vector and sorts the element of the vector (slightly faster that hMedian)."
#include "hfdefaultwrappercode.h"
//$COPY_TO END --------------------------------------------------


/*!  \brief Returns the median value of the elements in a
  (scratch) vector. 

  Attention!!! The vector will be sorted first. Hence, if you want to
  keep the data in its original order, you need to copy the data first
  to a scratch vector and then call this function with the scratch vector!

  \param data_start: STL Iterator pointing to the first element of an array with
         input values.
  \param data_end: STL Iterator pointing to the end of the input vector

*/
template <class Iter> 
IterValueType hMedian(const Iter data_start, const Iter data_end)
{
  address len2=mycast<address>(data_end-data_start)/2;
  sort(data_start,data_end);
  if (data_end!=data_start) return *(data_start+len2);
  else return mycast<IterValueType>(0);
} 


//$COPY_TO HFILE START --------------------------------------------------
#include "hfpp-undef.cc"
#define HF_PP_FUNCNAME hStdDev  //The Name of the function
//.......................................................................
#define HF_PP_FUNCBRIEF "Calculates the standard deviation around a mean value."
#define HF_PP_NO_GUI 1 //Don't generate a wrapper for the GUI (use overloaded function) 
#define HF_PP_FUNCTYPE_T 1     //Return value type of function is templated with T
#define HF_PP_NVECS 1 //Number of (input/output) vectors
#define HF_PP_NPAR 1  //Number of other parameters
#define HF_PP_PAR0 (mean,T,0,"Mean value of vector.",HF_PP_TEMPLATED_TYPE,HF_PP_PASS_AS_VALUE)  //Definition of input parameter
#include "hfdefaultwrappercode.h"
//$COPY_TO END --------------------------------------------------

/*!
  \brief Calculates the standard deviation around a mean value.

  \param data_start: STL Iterator pointing to the first element of an array with input
  values.  

  \param data_end: STL Iterator pointing to the end of the
  input vector

  \param mean: The mean value of the vector, which needs to be provided
*/
template <class Iter> 
IterValueType hStdDev(const Iter data_start,const Iter data_end, const IterValueType mean)
{
  typedef IterValueType T;
  T scrt,sum=mycast<T>(0);
  address len=mycast<address>(data_end-data_start);
  Iter it=data_start;
  while (it!=data_end) {
    scrt=hf_sub(*it,mean);
    sum+=hf_mul(scrt,scrt);
    ++it;
  };
  if (len>1) return hf_sqrt(hf_div(sum,mycast<T>(len-1)));
  else return hf_sqrt(sum);
} 

//$COPY_TO HFILE START --------------------------------------------------
#include "hfpp-undef.cc"
#define HF_PP_FUNCNAME hStdDev  //The Name of the function
#define HF_PP_VARIANT 1 //Indicating that this is an overloaded function
//.......................................................................
#define HF_PP_FUNCBRIEF "Calculates the standard deviation around a mean value."
#define HF_PP_GUI_LIBRARY Math
#define HF_PP_GUI_RETURN_POLICY  HF_PP_GUI_RETURN_SCALAR
#define HF_PP_FUNCTYPE_T 1     //Return value type of function is templated with T
#define HF_PP_NVECS 1 //Number of (input/output) vectors
#define HF_PP_NPAR 0  //Number of other parameters
#include "hfdefaultwrappercode.h"
//$COPY_TO END --------------------------------------------------

/*!
  \brief Calculates the standard deviation around the mean value.

  This is a convenience overload function that calculates the mean value automatically.

  \param data_start: STL Iterator pointing to the first element of an array with input
  values.  

  \param data_end: STL Iterator pointing to the end of the
  input vector

*/
template <class Iter> 
IterValueType hStdDev(const Iter data_start,const Iter data_end)
{
  return hStdDev(data_start,data_end,hMean(data_start,data_end));
} 

//$COPY_TO HFILE START --------------------------------------------------
#include "hfpp-undef.cc"
#define HF_PP_FUNCNAME hDownsample  //The Name of the function
//.......................................................................
#define HF_PP_FUNCBRIEF "Resamples (by averaging) an input vector to fit the size of the output vector."
#define HF_PP_GUI_LIBRARY Math
#define HF_PP_GUI_RETURN_POLICY  HF_PP_GUI_RETURN_NEW_VECTOR
#define HF_PP_NO_GUI 1
#define HF_PP_FUNCTYPE void   //Return value type of function is templated with T
#define HF_PP_NVECS 2 //Number of (input/output) vectors
#define HF_PP_NPAR 0  //Number of other parameters
#include "hfdefaultwrappercode.h"

//Define a 2nd convenience wrapper function where the length is specified and vector is resized
#define HF_PP_VARIANT 1
#undef HF_PP_NPAR
#undef HF_PP_NO_GUI //Now use this variant for making a GUI wrapper
#define HF_PP_NPAR 1  //Number of other parameters
#define HF_PP_PAR0 (downsample_factor,address,8,"Factor by which input vector is to be reduced.",HF_PP_NON_TEMPLATED_TYPE,HF_PP_PASS_AS_VALUE)  //Definition of input parameter
//Now define your own wrapper code
#undef HF_PP_VEC_WRAPPER_CODE_STL
#undef HF_PP_VEC_WRAPPER_CODE_CASA
#define HF_PP_VEC_WRAPPER_CODE_STL \
  HF_PP_VECTORNAME(1).resize(HF_PP_VECTORNAME(0).size()/downsample_factor); \
  HF_PP_FUNCNAME (HF_PP_PAR_VECTORLIST(HF_PP_NVECS))
#define HF_PP_VEC_WRAPPER_CODE_CASA \
  HF_PP_VECTORNAME(1).resize(*(HF_PP_VECTORNAME(0).shape().begin())/downsample_factor); \
  HF_PP_FUNCNAME (HF_PP_PAR_VECTORLIST(HF_PP_NVECS))
HF_PP_GENERATE_WRAPPERS

//$COPY_TO END --------------------------------------------------
/*
#undef HF_PP_VARIANT
#undef HF_PP_NVEC
#define HF_PP_VARIANT 2
#undef HF_PP_NPAR
#undef HF_PP_NVEC 1
--->#define HF_PP_FUNCTYPE void   //Return value type of function is templated with T
#define HF_PP_VEC_WRAPPER_CODE_STL \
  HF_PP_VECTORNAME(1).resize(len,mycast<T>(0));\
  return HF_PP_FUNCNAME (HF_PP_PAR_VECTORLIST(HF_PP_NVECS))
#define HF_PP_VEC_WRAPPER_CODE_CASA \
  HF_PP_VECTORNAME(1).resize(len);\
  return HF_PP_FUNCNAME (HF_PP_PAR_VECTORLIST(HF_PP_NVECS))
HF_PP_GENERATE_WRAPPERS
*/

/*!  \brief Downsample the input vector to a smaller output vector, by
     replacing subsequent blocks of values by their mean value. The
     block size is automatically chosen such that the input vector
     fits exactly into the output vector. All blocks have the same
     length with a possible exception of the last block.


  \param idata_start: Iterator pointing to the first element of an array with
         input values.

  \param idata_end: Iterator pointing to the end of the input vector

  \param odata_start: Iterator pointing to the first element of an array
         which will contain output values.

  \param odata_end: Iterator pointing to the last element of an array
         which will contain output values. The length of the output
         vector should be smaller or equal to the input vector.


*/


template <class Iter> 
void hDownsample (const Iter idata_start,
		  const Iter idata_end,
		  const Iter odata_start,
		  const Iter odata_end)
{
  if (odata_start>=odata_end) return; //If size 0 do nothing 
  if (idata_start>=idata_end) return; //If size 0 do nothing 
  address ilen=(idata_end-idata_start);
  address olen=(odata_end-odata_start);
  address blen=max(ilen/(olen-1),1); 
  //use max to avoid infinite loops if output vector is too large
  Iter it2,it1=idata_start;
  Iter ito=odata_start,ito_end=odata_end-1; 
  //only produce the first N-1 blocks in the output vector
  while (it1<idata_end && ito<ito_end) {
    it2=min(it1+blen,idata_end);
    //    cout << "it1="<< &(*it1) << ", it2=" << &(*it2) <<endl; 
    //    if (ito<odata_end & ito>=odata_start) *ito=hMean(it1,it2);
    *ito=hMean(it1,it2);
    it1=it2; 
    ++ito; 
    }
  *ito=hMean(it2,idata_end);
}

void dummy_instantitate_templates(){
  casa::Vector<HNumber> v;
  vector<HNumber> vn;
  vector<HInteger> vi;
  vector<HComplex> vc;
  hRunningAverageCasaVec(v, v,5,WEIGHTS_GAUSSIAN);
  /*  hNegate(vn);
  hNegate(vi);
  hNegate(vi.begin(),vi.end());
  hNegate(vc);
  */
}


#undef HF_PP_FILETYPE
