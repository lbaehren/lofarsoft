/**************************************************************************
 *  IO module for the CR Pipeline Python bindings.                        *
 *                                                                        *
 *  Copyright (c) 2010                                                    *
 *                                                                        *
 *  Martin van den Akker <martinva@astro.ru.nl>                           *
 *                                                                        *
 *  This library is free software: you can redistribute it and/or modify  *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation, either version 3 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        *
 *  This library is distributed in the hope that it will be useful,       *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *  GNU General Public License for more details.                          *
 *                                                                        *
 *  You should have received a copy of the GNU General Public License     *
 *  along with this library. If not, see <http://www.gnu.org/licenses/>.  *
 **************************************************************************/


// ========================================================================
//
//  Wrapper Preprocessor Definitions
//
// ========================================================================

//-----------------------------------------------------------------------
//Some definitions needed for the wrapper preprosessor:
//-----------------------------------------------------------------------
//$FILENAME: HFILE=mIO.def.h
//-----------------------------------------------------------------------

// ========================================================================
//
//  Header files
//
// ========================================================================

#include "core.h"
#include "mVector.h"
#include "mIO.h"


// ========================================================================
//
//  Implementation
//
// ========================================================================

#undef HFPP_FILETYPE
//--------------------
#define HFPP_FILETYPE CC
//--------------------

// ========================================================================
//$SECTION: IO functions
// ========================================================================

/*!
\brief Determine the filetype based on the extension of the filename
 */
HString hgetFiletype(HString filename){
  HString ext=hgetFileExtension(filename);
  HString typ="";
  if (ext=="event") typ="LOPESEvent";
  else if (ext=="h5") typ="LOFAR_TBB";
  return typ;
}

/*!
\brief Get the extension of the filename, i.e. the text after the last dot.
 */
HString hgetFileExtension(HString filename){
  HInteger size=filename.size();
  HInteger pos=filename.rfind('.',size);
  pos++;
  if (pos>=size) {return "";}
  else {return filename.substr(pos,size-pos);};
}


//$DOCSTRING: Print a brief summary of the file contents and current settings.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFileSummary
//------------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (CRDataReader)(dr)()("DataReader object")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
 \brief $DOCSTRING
 $PARDOCSTRING
*/

void HFPP_FUNC_NAME(CRDataReader & dr) {
  dr.summary();
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Function to open a file based on a filename and returning a datareader object.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFileOpen
//------------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
#define HFPP_FUNCDEF  (CRDataReader)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_0 (HString)(Filename)()("Filename of file to open including full directory name")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
 \brief $DOCSTRING
 $PARDOCSTRING
*/

CRDataReader & HFPP_FUNC_NAME(HString Filename) {

  bool opened;

  //Create the a pointer to the DataReader object and store the pointer
  CR::DataReader* drp;
  HString Filetype = hgetFiletype(Filename);

  if (Filetype=="LOPESEvent") {
    CR::LopesEventIn* lep = new CR::LopesEventIn;
    opened=lep->attachFile(Filename);
    drp=lep;
    cout << "Opening LOPES File="<<Filename<<endl; drp->summary();
  } else if (Filetype=="LOFAR_TBB") {
    drp = new CR::LOFAR_TBB(Filename,1024);
    opened=drp!=NULL;
    cout << "Opening LOFAR File="<<Filename<<endl; drp->summary();
  } else {
    ERROR(BOOST_PP_STRINGIZE(HFPP_FUNC_NAME) << ": Unknown Filetype = " << Filetype  << ", Filename=" << Filename);
    opened=false;
  }
  if (!opened){
    ERROR(BOOST_PP_STRINGIZE(HFPP_FUNC_NAME) << ": Opening file " << Filename << " failed.");
    CR::LopesEventIn* lep = new CR::LopesEventIn; //Make a dummy data reader ....
    drp=lep;
  };
  return *drp;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//------------------------------------------------------------------------
//$DOCSTRING: Return information from a data file as a Python object.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFileGetParameter
//-----------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
#define HFPP_FUNCDEF  (HPyObject)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (CRDataReader)(dr)()("Datareader object openen, e.g. with hFileOpen or crfile.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HString)(keyword)()("Keyword to be read out from the file metadata")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
 \brief $DOCSTRING
 $PARDOCSTRING
*/
HPyObject HFPP_FUNC_NAME(CRDataReader &dr, HString key)
{
  HString key2(key);
  key2[0]=(unsigned char)toupper((int)key2[0]);
  key[0]=(unsigned char)tolower((int)key[0]);
  DataReader *drp=&dr;
#define HFPP_REPEAT(TYPE,TYPE2,KEY)  if ((key== #KEY) || (key2== #KEY)) {_H_NL_ TYPE result(drp->KEY ()); _H_NL_ HPyObject pyob((TYPE2)result); _H_NL_ return pyob;} else
    HFPP_REPEAT(uint,uint,nofAntennas)
    HFPP_REPEAT(uint,uint,nofSelectedChannels)
    HFPP_REPEAT(uint,uint,nofSelectedAntennas)
    HFPP_REPEAT(uint,uint,blocksize)
    HFPP_REPEAT(uint,uint,fftLength)
    HFPP_REPEAT(uint,uint,block)
    HFPP_REPEAT(uint,uint,stride)
    HFPP_REPEAT(uint,uint,nyquistZone)
    HFPP_REPEAT(double,double,sampleInterval)
    HFPP_REPEAT(double,double,referenceTime)
    HFPP_REPEAT(double,double,sampleFrequency)
    HFPP_REPEAT(uint,uint,nofBaselines)
#undef HFPP_REPEAT
#define HFPP_REPEAT(TYPE,TYPE2,KEY) if ((key== #KEY) || (key2== #KEY)) {_H_NL_ CasaVector<TYPE> casavec(drp->KEY ()); _H_NL_ std::vector<TYPE2> result; _H_NL_ aipsvec2stlvec(casavec,result); _H_NL_ HPyObject pyob(result); _H_NL_ return pyob;} else
    HFPP_REPEAT(uint,HInteger,antennas)
    HFPP_REPEAT(uint,HInteger,selectedAntennas)
    HFPP_REPEAT(uint,HInteger,selectedChannels)
    HFPP_REPEAT(uint,HInteger,positions)
    HFPP_REPEAT(double,HNumber,increment)
    HFPP_REPEAT(double,HNumber,frequencyValues)
    HFPP_REPEAT(double,HNumber,frequencyRange)
    HFPP_REPEAT(int, HInteger, shift)
#undef HFPP_REPEAT

// --- Reading data from the headerrecord in a scalar---
#define HFPP_REPEAT(TYPE,TYPE2,KEY)  \
      if ((key== #KEY) || (key2== #KEY)) {_H_NL_\
	if (drp->headerRecord().isDefined(#KEY)) {	_H_NL_	\
	TYPE result;  _H_NL_\
	drp->headerRecord().get(#KEY,result); _H_NL_\
        HPyObject pyob((TYPE2)result);return pyob;} _H_NL_	\
	else {HPyObject pyob("NONE");return pyob;}; _H_NL_ \
       	} else

    HFPP_REPEAT(uint,uint,Date)
    HFPP_REPEAT(casa::String,HString,Observatory)
    HFPP_REPEAT(int,int,Filesize)
    //	HFPP_REPEAT(double,double,dDate)
    HFPP_REPEAT(int,int,presync)
    HFPP_REPEAT(int,int,TL)
    HFPP_REPEAT(int,int,LTL)
    HFPP_REPEAT(int,int,EventClass)
    HFPP_REPEAT(casa::uChar,uint,SampleFreq)
    HFPP_REPEAT(uint,uint,StartSample)
#undef HFPP_REPEAT

// --- Reading data from the headerrecord in a vector ---
#define HFPP_REPEAT(TYPE,TYPE2,KEY)  if ((key== #KEY) || (key2== #KEY)) {_H_NL_	\
      if (drp->headerRecord().isDefined(#KEY)) {  _H_NL_		\
      CasaVector<TYPE> casavec;  _H_NL_\
      drp->headerRecord().get(#KEY,casavec); _H_NL_\
      std::vector<TYPE2> result; _H_NL_\
      aipsvec2stlvec(casavec,result); _H_NL_ \
      HPyObject pyob(result);return pyob;} _H_NL_ \
      else {HPyObject pyob("NONE");return pyob;}; _H_NL_ \
      } else

      HFPP_REPEAT(int,HInteger,AntennaIDs)
      HFPP_REPEAT(int,HInteger,SAMPLE_OFFSET)
      HFPP_REPEAT(uint,HInteger,SAMPLE_NUMBER)
      HFPP_REPEAT(uint,HInteger,TIME)
    { HString result; result = result
#undef HFPP_REPEAT
#define HFPP_REPEAT(TYPE,TYPE2,KEY)  + #KEY + ", "
    HFPP_REPEAT(uint,uint,nofAntennas)
    HFPP_REPEAT(uint,uint,nofSelectedChannels)
    HFPP_REPEAT(uint,uint,nofSelectedAntennas)
    HFPP_REPEAT(uint,uint,nofBaselines)
    HFPP_REPEAT(uint,uint,block)
    HFPP_REPEAT(uint,uint,blocksize)
    HFPP_REPEAT(uint,uint,stride)
    HFPP_REPEAT(uint,uint,fftLength)
    HFPP_REPEAT(uint,uint,nyquistZone)
    HFPP_REPEAT(double,double,sampleInterval)
    HFPP_REPEAT(double,double,referenceTime)
    HFPP_REPEAT(double,double,sampleFrequency)
    HFPP_REPEAT(uint,HInteger,antennas)
    HFPP_REPEAT(uint,HInteger,selectedAntennas)
    HFPP_REPEAT(uint,HInteger,selectedChannels)
    HFPP_REPEAT(uint,HInteger,positions)
    HFPP_REPEAT(int,HInteger,shift)
    HFPP_REPEAT(double,HNumber,increment)
    HFPP_REPEAT(double,HNumber,frequencyValues)
    HFPP_REPEAT(double,HNumber,frequencyRange)
    HFPP_REPEAT(uint,uint,Date)
    HFPP_REPEAT(casa::String,HString,Observatory)
    HFPP_REPEAT(int,int,Filesize)
    // HFPP_REPEAT(double,double,dDate)
    HFPP_REPEAT(int,int,presync)
    HFPP_REPEAT(int,int,TL)
    HFPP_REPEAT(int,int,LTL)
    HFPP_REPEAT(int,int,EventClass)
    HFPP_REPEAT(casa::uChar,uint,SampleFreq)
    HFPP_REPEAT(uint,uint,StartSample)
    HFPP_REPEAT(int,HInteger,AntennaIDs)
			HFPP_REPEAT(int,HInteger,SAMPLE_OFFSET)
			HFPP_REPEAT(uint,HInteger,SAMPLE_NUMBER)
			HFPP_REPEAT(uint,HInteger,TIME)
//------------------------------------------------------------------------
#undef HFPP_REPEAT
		     + "keywords, help";
      if ((key!="help") && (key!="keywords")) cout << "Unknown keyword " << key <<"!"<<endl;
      if (key!="keywords") cout  << BOOST_PP_STRINGIZE(HFPP_FUNC_NAME) << " - available keywords: "<< result <<endl;
      HPyObject pyob(result);
      return pyob;
    };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Set parameters in a data file with a Python object as input.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFileSetParameter
//-----------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
#define HFPP_FUNCDEF  (bool)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (CRDataReader)(dr)()("Datareader object openen, e.g. with hFileOpen or crfile.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HString)(keyword)()("Keyword to be set in the file")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (HPyObjectPtr)(pyob)()("Input parameter")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
////$COPY_TO END --------------------------------------------------
/*!
 \brief $DOCSTRING
 $PARDOCSTRING
*/
bool HFPP_FUNC_NAME(CRDataReader &dr, HString key, HPyObjectPtr pyob)
{
  DataReader *drp=&dr;
  HString key2(key);
  key2[0]=(unsigned char)toupper((int)key2[0]);
  key[0]=(unsigned char)tolower((int)key[0]);
#define HFPP_REPEAT(TYPE,TYPE2,KEY) if ((key== #KEY) || (key2==#KEY)) {TYPE input(TYPE2 (pyob)); drp->set##KEY (input);} else
  HFPP_REPEAT(uint,PyInt_AsLong,Block)
    HFPP_REPEAT(uint,PyInt_AsLong,Blocksize)
    HFPP_REPEAT(uint,PyInt_AsLong,StartBlock)
    HFPP_REPEAT(uint,PyInt_AsLong,Stride)
    HFPP_REPEAT(uint,PyInt_AsLong,SampleOffset)
    HFPP_REPEAT(uint,PyInt_AsLong,NyquistZone)
    HFPP_REPEAT(double,PyFloat_AsDouble,ReferenceTime)
    HFPP_REPEAT(double,PyFloat_AsDouble,SampleFrequency)
    HFPP_REPEAT(int,PyInt_AsLong,Shift)
    if ((key=="shift") || (key2=="Shift")) {
      vector<int> stlvec(PyList2STLInt32Vec(pyob));
      drp->setShift(stlvec);
    } else  if ((key=="selectedAntennas") || (key2=="SelectedAntennas")) {
      vector<uint> stlvec(PyList2STLuIntVec(pyob));
      uint * storage = &(stlvec[0]);
      casa::IPosition shape(1,stlvec.size()); //tell casa the size of the vector
      CasaVector<uint> casavec(shape,storage,casa::SHARE);
      drp->setSelectedAntennas(casavec,false);
    } else if ((key=="selectedAntennasID") || (key2=="SelectedAntennasID")) {
      vector<uint> stlvec(PyList2STLuIntVec(pyob));
      uint * storage = &(stlvec[0]);
      casa::IPosition shape(1,stlvec.size()); //tell casa the size of the vector
      CasaVector<uint> casavec(shape,storage,casa::SHARE);
      drp->setSelectedAntennas(casavec);
    } else if ((key=="shiftVector") || (key=="ShiftVector")) {
      vector<int> stlvec(PyList2STLInt32Vec(pyob));
      drp->setShift(stlvec);
    } else
#undef HFPP_REPEAT
      // -- For documentation purposes --
#define HFPP_REPEAT(TYPE,TYPE2,KEY)  + #KEY + ", "
    { HString txt; txt = txt
    HFPP_REPEAT(uint,PyInt_AsLong,Blocksize)
    HFPP_REPEAT(uint,PyInt_AsLong,StartBlock)
    HFPP_REPEAT(uint,PyInt_AsLong,Block)
    HFPP_REPEAT(uint,PyInt_AsLong,Stride)
    HFPP_REPEAT(uint,PyInt_AsLong,SampleOffset)
    HFPP_REPEAT(uint,PyInt_AsLong,NyquistZone)
    HFPP_REPEAT(double,PyFloat_AsDouble,ReferenceTime)
    HFPP_REPEAT(double,PyFloat_AsDouble,SampleFrequency)
    HFPP_REPEAT(int,PyInt_AsLong,Shift)
    HFPP_REPEAT(int,XX,ShiftVector)
    HFPP_REPEAT(uint,XX,SelectedAntennas)
    HFPP_REPEAT(uint,XX,SelectedAntennasID)
#undef HFPP_REPEAT
		     + "help";
      if (key!="help") {
	cout << "Unknown keyword " << key <<"!"<<endl;
	return false;
      };
      cout  << BOOST_PP_STRINGIZE(HFPP_FUNC_NAME) << " - available keywords: "<< txt <<endl;
    };
  return true;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Read a specified block of data from a Datareader object into a vector, where the size should be pre-allocated.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFileRead
//-----------------------------------------------------------------------
#define HFPP_FUNC_MASTER_ARRAY_PARAMETER 2 // Use the third parameter as the master array for looping and history informations
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (CRDataReader)(dr)()("Datareader object, opened e.g. with hFileOpen or crfile.")(HFPP_PAR_IS_UNMUTABLE_SCALAR)()(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HString)(Datatype)()("Name of the data column to be retrieved (e.g., FFT, Fx,Time, Frequency...)")(HFPP_PAR_IS_UNMUTABLE_SCALAR)()(HFPP_PASS_AS_VALUE) //UNMUTABLE_SCALAR: will not be converted to a vector when used in looping mode.
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_TYPE)(vec)()("Data (output) vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (HInteger)(block)()("block number to read - read current block if negative")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_4 (HInteger)(antenna)()("antenna number to read - read all if negative")(HFPP_PAR_IS_UNMUTABLE_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
 \brief $DOCSTRING
 $PARDOCSTRING

Example:

filename=LOFARSOFT+"/data/lofar/rw_20080701_162002_0109.h5"
datafile=crfile(filename)
datafile["blocksize"]=10
f=hArray(float,[10])
f.read(datafile,"Fx")
f.pprint(-1)

>> [29,29,35,27,9,-8,-45,-74,-92,-109]

datafile["blocksize"]=2
x=hArray(float,[5,2])
hFileRead(datafile,"Fx",x[...],Vector(range(2)))
x.mprint()
>> [29,29]   # blck 0
   [35,27]   # blck 1
   [29,29]   # blck 0
   [35,27]   # blck 1
   [29,29]   # blck 0

Note, that here block 0,1 and one are read in 2-3 times, since the
array was longer then the block list specified! This is the default
behaviour of looping arrays.

You may only read the data into a section of your array:

x[1].read(datafile,"Fx",3)
x.mprint()

>> [29,29]
   [-45,-74]
   [29,29]
   [35,27]
   [29,29]

Now, we sneaked the 4th block (block 3) into the second row (index 1).

Finally, you can also read the data into an array of different type
(but that is slightly slower as scratch arrays are created for every
row that is being read)

c=hArray(complex,[5,2])
c[...].read(datafile,"Fx",Vector([0,1,2,3]))
c.mprint()
>> [(29,0),(29,0)]
   [(35,0),(27,0)]
   [(9,0),(-8,0)]
   [(-45,0),(-74,0)]
   [(29,0),(29,0)]   # here we have block 0 again ....
*/

template <class Iter>
void HFPP_FUNC_NAME(
		    CRDataReader &dr,
		    HString Datatype,
		    const Iter vec, const Iter vec_end,
		    HInteger block,
		    HInteger antenna
		    )
{

  //Create a DataReader Pointer from an integer variable
  DataReader *drp=&dr;
  typedef IterValueType T;

  //Check whether it is non-NULL.
  if (drp==Null_p){
    ERROR(BOOST_PP_STRINGIZE(HFPP_FUNC_NAME) << ": pointer to FileObject is NULL, DataReader not found.");
    return;
  };
  if (block>=0) {drp->setBlock((uint)block);};
  //------TIME------------------------------
  if (Datatype=="Time") {
    casa::IPosition shape(1,vec_end-vec); //tell casa the size of the vector
    if (typeid(*vec) == typeid(double)) {
      double * storage = reinterpret_cast<double*>(&(*vec));
      CasaVector<double> casavec(shape,storage,casa::SHARE);
      drp->timeValues(casavec);
    }  else {
      CasaVector<double> casavec(shape);
      drp->timeValues(casavec);
      aipsvec2stdit(casavec,vec,vec_end);
    };
    //------FREQUENCY------------------------------
  } else if (Datatype=="Frequency") {
    CasaVector<double> val = drp->frequencyValues();
    aipsvec2stdit(val,vec,vec_end);
  }
//..........................................................................................
//Conversion from aips to stl using shared memory space
//..........................................................................................

//Note there is an ugly patch to select antennas since the DataReader still doesn't work
#define HFPP_REPEAT(TYPESTL,TYPECASA,FIELD,SIZE)				\
  if ((typeid(T)==typeid(TYPESTL)) && (antenna<0)) {	_H_NL_		\
    casa::IPosition shape(2);				_H_NL_		\
    shape(0)=drp->SIZE (); shape(1)=drp->nofSelectedAntennas();	_H_NL_\
    if (shape(0)*shape(1) != vec_end-vec) {ERROR( BOOST_PP_STRINGIZE(HFPP_FUNC_NAME) << ": Input vector size " << vec_end-vec << " does not match expected size of " << shape(1) << " antennas times " << shape(0) << " data points (= " << shape(0)*shape(1) <<")!"); throw PyCR::ValueError("Incorrect size of read input vector."); return;}; _H_NL_ \
    casa::Matrix<TYPECASA> casamtrx(shape,reinterpret_cast<TYPECASA*>(&(*vec)),casa::SHARE); _H_NL_\
    drp->FIELD (casamtrx);						_H_NL_\
  } else {								_H_NL_\
    casa::IPosition shape(2);				_H_NL_		\
    HInteger selantennas(drp->nofSelectedAntennas());_H_NL_\
    shape(0)=drp->SIZE (); shape(1)=selantennas;	_H_NL_\
    vector<TYPESTL> tmpvec(shape(0)*shape(1)); _H_NL_\
    casa::Matrix<TYPECASA> casamtrx(shape,reinterpret_cast<TYPECASA*>(&(tmpvec[0])),casa::SHARE); _H_NL_\
    if (antenna>=0) {shape(1)=1; if (antenna>=selantennas) antenna=(selantennas-1);};_H_NL_\
    if (shape(0)*shape(1) != vec_end-vec) {ERROR( BOOST_PP_STRINGIZE(HFPP_FUNC_NAME) << ": Input vector size " << vec_end-vec << " does not match expected size of " << shape(1) << " antennas times " << shape(0) << " data points (= " << shape(0)*shape(1) <<")!");throw PyCR::ValueError("Incorrect size of read input vector."); return;}; _H_NL_ \
    drp->FIELD (casamtrx);						_H_NL_\
  if (antenna>=0) {PyCR::Vector::hCopy(vec,vec_end,tmpvec.begin()+antenna*shape(0),tmpvec.begin()+(antenna+1)*shape(0));}_H_NL_\
  else {PyCR::Vector::hCopy(vec,vec_end,tmpvec.begin(),tmpvec.end());}_H_NL_\
  }

//..........................................................................................

  //------FX----------------------------TYPESTL,TYPECASA,FIELD,SIZE
  else if (Datatype=="Fx") {HFPP_REPEAT(HNumber,double,fx,blocksize);}
  //------VOLTAGE------------------------------
  else if (Datatype=="Voltage") {HFPP_REPEAT(HNumber,double,voltage,blocksize);}
  //------FFT------------------------------
  else if (Datatype=="FFT") {HFPP_REPEAT(HComplex,CasaComplex,fft,fftLength);}
  //------CALFFT------------------------------
  else if (Datatype=="CalFFT") {HFPP_REPEAT(HComplex,CasaComplex,calfft,fftLength);}
  else {
    ERROR(BOOST_PP_STRINGIZE(HFPP_FUNC_NAME) << ": Datatype=" << Datatype << " is unknown.");
  };
  return;
}
#undef HFPP_REPEAT
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Read data from a Datareader object (pointer in iptr) into a vector, where the size should be pre-allocated.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFileRead
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_CLASSES HFPP_CLASS_STL HFPP_CLASS_hARRAY  //HFPP_CLASS_hARRAYALL is missing due to a deficiency of hfppnew to deal with pass as reference
#define HFPP_PYTHON_WRAPPER_CLASSES HFPP_CLASS_STL HFPP_CLASS_hARRAY
#define HFPP_FUNC_KEEP_RETURN_TYPE_FIXED HFPP_TRUE //return a single DataReader object and not a vector thereof for array operations
#define HFPP_FUNC_MASTER_ARRAY_PARAMETER 2 // Use the third parameter as the master array for looping and history informations
#define HFPP_FUNCDEF  (CRDataReader)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_0 (CRDataReader)(dr)()("Datareader object, opened e.g. with hFileOpen or crfile.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HString)(Datatype)()("Name of the data column to be retrieved (e.g., FFT, Fx,Time, Frequency...)")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_TYPE)(vec)()("Data (output) vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
 \brief $DOCSTRING
 $PARDOCSTRING

*/

template <class Iter>
CRDataReader & HFPP_FUNC_NAME(
		    CRDataReader &dr,
		    HString Datatype,
		    const Iter vec, const Iter vec_end
		    )
{
  hFileRead(dr,Datatype,vec,vec_end,-1,-1);
  return dr;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Read data from a Datareader object into a vector, where the size should be pre-allocated.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFileRead
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_CLASSES HFPP_CLASS_STL HFPP_CLASS_hARRAY  //HFPP_CLASS_hARRAYALL is missing due to a deficiency of hfppnew to deal with pass as reference
#define HFPP_PYTHON_WRAPPER_CLASSES HFPP_CLASS_STL HFPP_CLASS_hARRAY
#define HFPP_FUNC_KEEP_RETURN_TYPE_FIXED HFPP_TRUE //return a single DataReader object and not a vector thereof for array operations
#define HFPP_FUNC_MASTER_ARRAY_PARAMETER 2 // Use the third parameter as the master array for looping and history informations
#define HFPP_FUNCDEF  (CRDataReader)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_0 (CRDataReader)(dr)()("Datareader object, opened e.g. with hFileOpen or crfile.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HString)(Datatype)()("Name of the data column to be retrieved (e.g., FFT, Fx,Time, Frequency...)")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_TYPE)(vec)()("Data (output) vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (HInteger)(block)()("block number to read - read current block if negative")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
 \brief $DOCSTRING
 $PARDOCSTRING

*/

template <class Iter>
CRDataReader & HFPP_FUNC_NAME(
		    CRDataReader &dr,
		    HString Datatype,
		    const Iter vec, const Iter vec_end,
		    const HInteger block
		    )
{
  hFileRead(dr,Datatype,vec,vec_end,block,-1);
  return dr;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//------------------------------------------------------------------------
//$DOCSTRING: Return a list of antenna positions from the CalTables - this is a test.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hCalTable
//-----------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
#define HFPP_FUNCDEF  (HPyObjectPtr)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HString)(filename)()("Filename of the caltable")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_1 (HString)(keyword)()("Keyword to be read out from the file metadata (currenly only Position is implemented)")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (HInteger)(date)()("Date for which the information is requested")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_3 (HPyObjectPtr)(pyob)()("(Python) List  with antenna IDs")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
 \brief $DOCSTRING
 $PARDOCSTRING

Example:
antennaIDs=hFileGetParameter(file,"AntennaIDs")
x=hCalTable("~/LOFAR/usg/data/lopes/LOPES-CalTable","Positions",obsdate,list(antennaIDs))

*/
HPyObjectPtr HFPP_FUNC_NAME(HString filename, HString keyword, HInteger date, HPyObjectPtr pyob) {
  CR::CalTableReader* CTRead = new CR::CalTableReader(filename);
  HInteger i,ant,size;
  casa::Vector<Double> tmpvec;
  Double tmpval;
  HPyObjectPtr list=PyList_New(0),tuple;
  if (CTRead != NULL && PyList_Check(pyob)) {  //Check if CalTable was opened ... and Python object is a list
    size=PyList_Size(pyob);
    for (i=0;i<size;++i){  //loop over all antennas
      ant=PyInt_AsLong(PyList_GetItem(pyob,i));  //Get the ith element of the list, i.e. the antenna ID
      CTRead->GetData((uint)date, ant, keyword, &tmpvec);
      if (keyword=="Position") {
	tuple=PyTuple_Pack(3,PyFloat_FromDouble(tmpvec[0]),PyFloat_FromDouble(tmpvec[1]),PyFloat_FromDouble(tmpvec[2]));
      } else if (keyword=="Delay") {
	tuple=PyTuple_Pack(1,PyFloat_FromDouble(tmpval));
      };
      PyList_Append(list,tuple);
    };
  };
  delete CTRead;
  return list;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//------------------------------------------------------------------------
//$DOCSTRING: Return a list of antenna calibration values from the CalTables as a float vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hCalTableVector
//-----------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
#define HFPP_FUNCDEF  (HNumber)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_VECTOR)(STL)(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HString)(filename)()("Filename of the caltable")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_1 (HString)(keyword)()("Keyword to be read out from the file metadata (currenly only Position is implemented)")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (HInteger)(date)()("Date for which the information is requested (I guess Julian days? As output from DataReader)")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_3 (HPyObjectPtr)(pyob)()("(Python) List  with antenna IDs")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
 \brief $DOCSTRING
 $PARDOCSTRING

 possible keywords: Positions, Delay


Example:
antennaIDs=hFileGetParameter(file,"AntennaIDs")
vec=hCalTablePositions("~/LOFAR/usg/data/lopes/LOPES-CalTable",obsdate,list(antennaIDs))

*/
vector<HNumber> HFPP_FUNC_NAME(HString filename, HString keyword, HInteger date, HPyObjectPtr pyob) {
  CR::CalTableReader* CTRead = new CR::CalTableReader(filename);
  /*  cout << "summary:" << CTRead.summary() << endl;
  uint t = time();
  cout << "current time:" << t << endl;
  */
  vector<HNumber> outvec;
  HInteger i,ant,size;
  casa::Vector<Double> tmpvec;
  Double tmpval;
  if (CTRead != NULL && PyList_Check(pyob)) {  //Check if CalTable was opened ... and Python object is a list
    size=PyList_Size(pyob);
    for (i=0;i<size;++i){  //loop over all antennas
      ant=PyInt_AsLong(PyList_GetItem(pyob,i));  //Get the ith element of the list, i.e. the antenna ID
      if (keyword=="Position") {
	CTRead->GetData((uint)date, ant, keyword, &tmpvec);
	outvec.push_back(tmpvec[1]); outvec.push_back(tmpvec[0]); outvec.push_back(tmpvec[2]);
      } else if (keyword=="Delay") {
	CTRead->GetData((uint)date, ant, keyword, &tmpval);
	outvec.push_back(tmpval);
      };
    };
  };
  delete CTRead;
  return outvec;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//-----------------------------------------------------------------------
//$DOCSTRING: Dump a single vector to a file in binary format (machine dependent)
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hWriteDump
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Output data vector.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HString)(filename)()("Filename (including path if necessary) to write file to.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  vec.writedump(filename) -> dumps vector to file

Related functions:
  hReadDump, hWriteDump

Python Example:
  >>> hWriteFileBinaryVector
  >>> v=Vector(float,10,fill=3.0)
  >>> hWriteFileBinaryVector(v,"test.dat")
  >>> x=Vector(float,10,fill=1.0)
  >>> hReadFileBinaryVector(x,"test.dat")
  >>> x
  Vec(float,10)=[3.0,3.0,3.0,3.0,3.0,...]


*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter vec,   const Iter vec_end, HString filename) {
  char * v1 = reinterpret_cast<char*>(&(*vec));
  char * v2 = reinterpret_cast<char*>(&(*vec_end));
  fstream outfile(filename.c_str(), ios::out | ios::binary);
  outfile.write(v1, (HInteger)(v2-v1));
  outfile.close();
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//-----------------------------------------------------------------------
//$DOCSTRING: Read a single vector from a file which was dumped in (machine-dependent) binary format
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hReadDump
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Input data vector.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HString)(filename)()("Filename (including path if necessary) to write File to. The vector needs to have the length corresponding to the file size.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END ----------------------------------------------------------
/*!
  vec.readdump(filename) -> reads dumped vector from file

  \brief $DOCSTRING
  $PARDOCSTRING

Related functions:
  hReadDump, hWriteDump

Python Example:
  >>> hWriteFileBinaryVector
  >>> v=Vector(float,10,fill=3.0)
  >>> hWriteFileBinaryVector(v,"test.dat")
  >>> x=Vector(float,10,fill=1.0)
  >>> hReadFileBinaryVector(x,"test.dat")
  >>> x
  Vec(float,10)=[3.0,3.0,3.0,3.0,3.0,...]

*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter vec,   const Iter vec_end, HString filename) {
  char * v1 = reinterpret_cast<char*>(&(*vec));
  char * v2 = reinterpret_cast<char*>(&(*vec_end));
  fstream outfile(filename.c_str(), ios::in | ios::binary);
  outfile.read(v1, (HInteger)(v2-v1));
  outfile.close();
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//-----------------------------------------------------------------------
//$DOCSTRING: Read a section (block) of a single vector from a file which was dumped in (machine-dependent) binary format
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hReadDump
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Input data vector.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HString)(filename)()("Filename (including path if necessary) to write File to. The vector needs to have the length corresponding to the file size.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (HInteger)(block)()("The block number to read (zero-based index), the block length is determined by the vector length.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END ----------------------------------------------------------
/*!
  vec.readdump(filename) -> reads dumped vector from file

  \brief $DOCSTRING
  $PARDOCSTRING

Related functions:
  hReadDump, hWriteDump

Example:
 x=hArray(range(10))
 x.writedump("test.dat")
 v=hArray(int,10)
 v.readdump("test.dat")
 v.pprint()
 >> [0,1,2,3,4,5,6,7,8,9]

#Now read the third block (of length 3) into the start of the vector
 v[0:3].readdump("test.dat",2)
 v.pprint()
 >> [6,7,8,3,4,5,6,7,8,9]
*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter vec,   const Iter vec_end, HString filename, HInteger block) {
  char * v1 = reinterpret_cast<char*>(&(*vec));
  char * v2 = reinterpret_cast<char*>(&(*vec_end));
  HInteger clen(v2-v1);
  ifstream outfile(filename.c_str(), ios::in | ios::binary);
  outfile.seekg(clen*block);
  outfile.read(v1, clen);
  outfile.close();
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

