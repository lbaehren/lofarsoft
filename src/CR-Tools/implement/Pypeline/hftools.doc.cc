//HFDOCSTART: Marks begin of file hftools.doc.cc which is being read by doxygen--------------
//HFDOCSTART: Marks begin of file hftools.doc.cc which is being read by doxygen--------------
/*!
 HFTOOLS Version 0.2 - Basic algorithms for the CR pipeline with
  automatic wrapper generation for python.  Includes various tools to
  manipulate stl vectors in python and wrapper function for CR Tool classes.

Here is an alphabetic list of some of the functions that are available.

funcGaussian(x, sigma, mu)          - Implementation of the Gauss function
hAdd(vec1, val, vec2)               - Performs a Add between the vector and a scalar, where the result is returned in the second vector.
hAdd(vec1, vec2, vec3)              - Performs a Add between the two vectors, which is returned in the third vector.
hAddAdd(vec1, vec2, vec3)           - Performs a Add between the two vectors, and adds the result to the output (third) vector.
hAddAddConv(vec1, vec2, vec3)       - Performs a Add between the two vectors, and adds the result to the output (third) vector - automatic casting is done.
hCalTable(filename, keyword, date, pyob) - Return a list of antenna positions from the CalTables - this is a test
hConvert(vec1, vec2)                - Copies and converts a vector to a vector of another type.
hCopy(vec, outvec)                  - Copies a vector to another one.
hDiv(vec1, val, vec2)               - Performs a Div between the vector and a scalar, where the result is returned in the second vector.
hDiv(vec1, vec2, vec3)              - Performs a Div between the two vectors, which is returned in the third vector.
hDivAdd(vec1, vec2, vec3)           - Performs a Div between the two vectors, and adds the result to the output (third) vector.
hDivAddConv(vec1, vec2, vec3)       - Performs a Div between the two vectors, and adds the result to the output (third) vector - automatic casting is done.
hDownsample(vec, downsample_factor) - Downsample the input vector by a cetain factor and return a new vector
hDownsample(vec1, vec2)             - Downsample the input vector to a smaller output vector.
hFileClose(iptr)                    - Function to close a file with a datareader object providing the pointer to the object as an integer.
hFileGetParameter(iptr, keyword)    - Return information from a data file as a Python object
hFileOpen(Filename)                 - Function to open a file based on a filename and returning a pointer to a datareader object as an integer
hFileRead(iptr, Datatype, vec)      - Read data from a Datareader object (pointer in iptr) into a vector, where the size should be pre-allocated.
hFileSetParameter(iptr, keyword, pyob) - Set parameters in a data file with a Python object as input
hFill(vec, fill_value)              - Fills a vector with a constant value.
hFindGreaterEqual(vec, threshold, vecout) - Find the samples that are GreaterEqual a certain threshold value and returns the number of samples found and the positions of the samples in a second vector
hFindGreaterEqualAbs(vec, threshold, vecout) - Find the samples whose absolute values are GreaterEqual a certain threshold value and returns the number of samples found and the positions of the samples in a second vector
hFindGreaterThan(vec, threshold, vecout) - Find the samples that are GreaterThan a certain threshold value and returns the number of samples found and the positions of the samples in a second vector
hFindGreaterThanAbs(vec, threshold, vecout) - Find the samples whose absolute values are GreaterThan a certain threshold value and returns the number of samples found and the positions of the samples in a second vector
hFindLessEqual(vec, threshold, vecout) - Find the samples that are LessEqual a certain threshold value and returns the number of samples found and the positions of the samples in a second vector
hFindLessEqualAbs(vec, threshold, vecout) - Find the samples whose absolute values are LessEqual a certain threshold value and returns the number of samples found and the positions of the samples in a second vector
hFindLessThan(vec, threshold, vecout) - Find the samples that are LessThan a certain threshold value and returns the number of samples found and the positions of the samples in a second vector
hFindLessThanAbs(vec, threshold, vecout) - Find the samples whose absolute values are LessThan a certain threshold value and returns the number of samples found and the positions of the samples in a second vector
hFindLowerBound(vec, value)         - Finds the location (i.e., returns integer) in a monotonically increasing vector, where the input search value is just above or equal to the value in the vector.
hFlatWeights(wlen)                  - Returns vector of weights of length len with constant weights normalized to give a sum of unity. Can be used by hRunningAverageT.
hGaussianWeights(wlen)              - Returns vector of weights of length wlen with Gaussian distribution centered at len/2 and sigma=len/4 (i.e. the Gaussian extends over 2 sigma in both directions).
hGeometricDelayFarField(antPosition, skyDirection, length) - Calculates the time delay in seconds for a signal received at an antenna position relative to a phase center from a source located in a certain direction in farfield (based on L. Bahren)
hGeometricDelayNearField(antPosition, skyPosition, distance) - Calculates the time delay in seconds for a signal received at an antenna position relative to a phase center from a source located at a certain 3D space coordinate in nearfield (based on L. Bahren)
hGeometricDelays(antPositions, skyPositions, delays, farfield) - Calculates the time delay in seconds for signals received at various antenna positions relative to a phase center from sources located at certain 3D space coordinates in near or far field
hGeometricPhases(frequencies, antPositions, skyPositions, phases, farfield) - Calculates the phase gradients for signals received at various antenna positions relative to a phase center from sources located at certain 3D space coordinates in near or far field and for different frequencies.
hGeometricWeights(frequencies, antPositions, skyPositions, weights, farfield) - Calculates the phase gradients as complex weights for signals received at various antenna positions relative to a phase center from sources located at certain 3D space coordinates in near or far field and for different frequencies.
hLinearWeights(wlen)                - Returns vector of weights of length wlen with linearly rising and decreasing weights centered at len/2.
hMean(vec)                          - Returns the mean value of all elements in a vector
hMedian(vec)                        - Returns the median value of the elements
hMul(vec1, val, vec2)               - Performs a Mul between the vector and a scalar, where the result is returned in the second vector.
hMul(vec1, vec2, vec3)              - Performs a Mul between the two vectors, which is returned in the third vector.
hMulAdd(vec1, vec2, vec3)           - Performs a Mul between the two vectors, and adds the result to the output (third) vector.
hMulAddConv(vec1, vec2, vec3)       - Performs a Mul between the two vectors, and adds the result to the output (third) vector - automatic casting is done.
hNegate(vec)                        - Multiplies each element in the vector with -1 in place, i.e. the input vector is also the output vector.
hNew(vec)                           - Make and return a new vector of the same size and type as the input vector
hNorm(vec)                          - Returns the lengths or norm of a vector (i.e. Sqrt(Sum_i(xi*+2)))
hNormalize(vec)                     - Normalizes a vector to length unity
hPhase(frequency, time)             - Returns the interferometer phase in radians for a given frequency and time
hReadFileOld(vec, iptr, Datatype, Antenna, Blocksize, Block, Stride, Shift) - Read data from a Datareader object (pointer in iptr) into a vector.
hRunningAverage(idata, odata, weights) - Calculate the running average of an input vector using a weight vector.
hRunningAverage(idata, odata, wlen, wtype) - Overloaded function to automatically calculate weights.
hSort(vec)                          - Sorts a vector in place
hSortMedian(vec)                    - Sorts a vector in place and returns the median value of the elements
hSpectralPower(vec, outvec)         - Calculates the power of a complex spectrum and add it to an output vector.
hStdDev(vec)                        - Calculates the standard deviation of a vector of values
hStdDev(vec, mean)                  - Calculates the standard deviation around a mean value.
hSub(vec1, val, vec2)               - Performs a Sub between the vector and a scalar, where the result is returned in the second vector.
hSub(vec1, vec2, vec3)              - Performs a Sub between the two vectors, which is returned in the third vector.
hSubAdd(vec1, vec2, vec3)           - Performs a Sub between the two vectors, and adds the result to the output (third) vector.
hSubAddConv(vec1, vec2, vec3)       - Performs a Sub between the two vectors, and adds the result to the output (third) vector - automatic casting is done.
hSum(vec)                           - Performs a sum over the values in a vector and returns the value
hWeights(wlen, wtype)               - Create a normalized weight vector.
hiAdd(vec1, val)                    - Performs a Add between the vector and a scalar (applied to each element), which is returned in the first vector.
hiAdd(vec1, vec2)                   - Performs a Add between the two vectors, which is returned in the first vector. If the second vector is shorter it will be applied multiple times.
hiDiv(vec1, val)                    - Performs a Div between the vector and a scalar (applied to each element), which is returned in the first vector.
hiDiv(vec1, vec2)                   - Performs a Div between the two vectors, which is returned in the first vector. If the second vector is shorter it will be applied multiple times.
hiMul(vec1, val)                    - Performs a Mul between the vector and a scalar (applied to each element), which is returned in the first vector.
hiMul(vec1, vec2)                   - Performs a Mul between the two vectors, which is returned in the first vector. If the second vector is shorter it will be applied multiple times.
hiSub(vec1, val)                    - Performs a Sub between the vector and a scalar (applied to each element), which is returned in the first vector.
hiSub(vec1, vec2)                   - Performs a Sub between the two vectors, which is returned in the first vector. If the second vector is shorter it will be applied multiple times.
square(val)                         - Returns the squared value of the parameter

*/
//========================================================================
//                    Casting & Conversion Functions
//========================================================================
template<class T> T hfnull(){};
template<> inline HString hfnull<HString>(){HString null=""; return null;}
template<> inline HPointer hfnull<HPointer>(){return __null;}
template<> inline HInteger hfnull<HInteger>(){return 0;}
template<> inline HNumber hfnull<HNumber>(){return 0.0;}
template<> inline HComplex hfnull<HComplex>(){return 0.0;}
//Identity
template<class T> inline T hfcast(/*const*/ T v){return v;}
//Convert to arbitrary class T if not specified otherwise
template<class T> inline T hfcast(uint v){return static_cast<T>(v);}
template<class T> inline T hfcast(HInteger v){return static_cast<T>(v);}
template<class T> inline T hfcast(HNumber v){return static_cast<T>(v);}
template<class T> inline T hfcast(HComplex v){return static_cast<T>(v);}
//Convert Numbers to Numbers and loose information (round float, absolute of complex)
template<> inline HInteger hfcast<HInteger>(HNumber v){return static_cast<HInteger>(floor(v+0.5));}
template<> inline HInteger hfcast<HInteger>(HComplex v){return static_cast<HInteger>(floor(real(v)+0.5));}
template<> inline HNumber hfcast<HNumber>(HComplex v){return real(v);}
inline HInteger ptr2int(HPointer v){return reinterpret_cast<HInteger>(v);}
inline HPointer int2ptr(HInteger v){return reinterpret_cast<HPointer>(v);}
inline HComplex operator*(HInteger i, HComplex c) {return hfcast<HComplex>(i)*c;}
inline HComplex operator*(HComplex c, HInteger i) {return hfcast<HComplex>(i)*c;}
inline HComplex operator+(HInteger i, HComplex c) {return hfcast<HComplex>(i)+c;}
inline HComplex operator+(HComplex c, HInteger i) {return hfcast<HComplex>(i)+c;}
inline HComplex operator-(HInteger i, HComplex c) {return hfcast<HComplex>(i)-c;}
inline HComplex operator-(HComplex c, HInteger i) {return c-hfcast<HComplex>(i);}
inline HComplex operator/(HInteger i, HComplex c) {return hfcast<HComplex>(i)/c;}
inline HComplex operator/(HComplex c, HInteger i) {return c/hfcast<HComplex>(i);}
template <class T>
casa::Vector<T> stl2casa(std::vector<T>& stlvec)
{
  T * storage = &(stlvec[0]);
  casa::IPosition shape(1,stlvec.size()); //tell casa the size of the vector
  casa::Vector<T> casavec(shape,storage,casa::SHARE);
  return casavec;
}
vector<HNumber> PyList2STLFloatVec(PyObject* pyob){
  std::vector<HNumber> vec;
  if (((((((PyObject*)(pyob))->ob_type))->tp_flags & ((1L<<25))) != 0)){
    HInteger i,size=PyList_Size(pyob);
    vec.reserve(size);
    for (i=0;i<size;++i) vec.push_back(PyFloat_AsDouble(PyList_GetItem(pyob,i)));
  }
  return vec;
}
vector<HInteger> PyList2STLIntVec(PyObject* pyob){
  std::vector<HInteger> vec;
  if (((((((PyObject*)(pyob))->ob_type))->tp_flags & ((1L<<25))) != 0)){
    HInteger i,size=PyList_Size(pyob);
    vec.reserve(size);
    for (i=0;i<size;++i) vec.push_back(PyInt_AsLong(PyList_GetItem(pyob,i)));
  }
  return vec;
}
vector<uint> PyList2STLuIntVec(PyObject* pyob){
  std::vector<uint> vec;
  if (((((((PyObject*)(pyob))->ob_type))->tp_flags & ((1L<<25))) != 0)){
    HInteger i,size=PyList_Size(pyob);
    vec.reserve(size);
    for (i=0;i<size;++i) vec.push_back(PyInt_AsLong(PyList_GetItem(pyob,i)));
  }
  return vec;
}
template <class S, class T>
void aipscol2stlvec(casa::Matrix<S> &data, std::vector<T>& stlvec, const HInteger col){
    HInteger i,nrow,ncol;
    nrow=data.nrow();
    ncol=data.ncolumn();
    //    if (ncol>1) {MSG("aipscol2stlvec: ncol="<<ncol <<" (nrow="<<nrow<<")");};
    if (col>=ncol) {
 ( cout << endl << "ERROR in file " << "hftools.tmp.cc" << " line " << 223 << ": " << "aipscol2stlvec: column number col=" << col << " is larger than total number of columns (" << ncol << ") in matrix." << endl );
 stlvec.clear();
 return;
    }
    stlvec.resize(nrow);
    casa::Vector<S> CASAVec = data.column(col);
//    p=stlvec.begin();
    for (i=0;i<nrow;i++) {
//	*p=hfcast<T>(CASAVec[i]);
 stlvec[i]=hfcast<T>(CASAVec[i]);
//	p++;
    };
}
/*!
  \brief The function converts an aips++ vector to an stl vector
 */
template <class S, class T>
void aipsvec2stlvec(casa::Vector<S>& data, std::vector<T>& stlvec){
    HInteger i,n;
//    std::vector<R>::iterator p;
    n=data.size();
    stlvec.resize(n);
//    p=stlvec.begin();
    for (i=0;i<n;i++) {
//	*p=hfcast<T>(data[i]);
 stlvec[i]=hfcast<T>(data[i]);
//	++p;
    };
}
//This function is copied from hfget.cc since I don't know a better
//way to do a proper instantiation of a templated function across two
//cpp files ..
template <class T, class S>
void copycast_vec(std::vector<T> &vi, std::vector<S> & vo) {
  typedef typename std::vector<T>::iterator Tit;
  typedef typename std::vector<S>::iterator Sit;
  Tit it1=vi.begin();
  Tit end=vi.end();
  if (it1==end) {vo.clear();}
  else {
    vo.assign(vi.size(),hfnull<S>()); //make the new vector equal in size and initialize with proper Null values
    Sit it2=vo.begin();
    while (it1!=end) {
      *it2=hfcast<S>(*it1);
      it1++; it2++;
    };
  };
}
//========================================================================
//                        Matrix Class !!!!!IGNORE!!!!!
//========================================================================
// Testing a simple Matrix class that is built upon std::vectors
// we do actually not really want to do this ....
namespace std {
  template <class T, class Alloc>
  class hmatrix : public vector<T,Alloc> {
  public:
    template <class Iter> hmatrix(Iter it1, Iter it2);
    hmatrix(const char* buf);
    hmatrix();
    ~hmatrix();
    void setNumberOfDimensions(HInteger newdim);
    HInteger getNumberOfDimensions();
    void setDimensionN(HInteger n, HInteger size);
    void setDimensions(HInteger dim0);
    void setDimensions(HInteger dim0, HInteger dim1);
    void setDimensions(HInteger dim0, HInteger dim1, HInteger dim3);
  private:
    HInteger ndim;
    vector<HInteger>* dimensions_p;
  };
  template <class T, class Alloc> template <class Iter> hmatrix<T,Alloc>::hmatrix(Iter it1, Iter it2) : vector<T,Alloc>(it1,it2) {};
  template <class T, class Alloc> hmatrix<T,Alloc>::hmatrix(const char* buf) : vector<T,Alloc>(buf){};
  template <class T, class Alloc> hmatrix<T,Alloc>::hmatrix(){
    ndim=0;
    dimensions_p=__null;
    //    setDimension(2);
  };
  template <class T, class Alloc> hmatrix<T,Alloc>::~hmatrix(){};
  template <class T, class Alloc>
  void hmatrix<T,Alloc>::setNumberOfDimensions(HInteger newdim){
    if (newdim==ndim) return;
    if (dimensions_p != __null) delete dimensions_p;
    ndim=newdim;
    dimensions_p = new vector<HString>(ndim);
  };
  template <class T, class Alloc>
  HInteger hmatrix<T,Alloc>::getNumberOfDimensions(){return ndim;};
  template <class T, class Alloc>
  void hmatrix<T,Alloc>::setDimensionN(HInteger n, HInteger size){
  }
  template <class T, class Alloc>
  void setDimensions(HInteger dim0){}
  template <class T, class Alloc>
  void setDimensions(HInteger dim0, HInteger dim1){}
  template <class T, class Alloc>
  void setDimensions(HInteger dim0, HInteger dim1, HInteger dim3){}
};
//========================================================================
//                        Helper Functions
//========================================================================
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
//========================================================================
//                    Administrative Vector Functions
//========================================================================
/*!

  \brief Fills a vector with a constant value.

  \param vec: STL Iterator pointing to the first element of an array with
         input values.
  \param vec_end: STL Iterator pointing to the end of the input vector

  \param fill_value: Value to fill vector with
*/
template <class Iter>
void hFill(const Iter vec,const Iter vec_end, const typename Iter::value_type fill_value)
{
  Iter it=vec;
  while (it!=vec_end) {
    *it=fill_value;
    ++it;
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T > inline void hFill( std::vector<T> & vec , T fill_value) {
hFill ( vec.begin(),vec.end() , fill_value);
}
template < class T > inline void hFill( casa::Vector<T> & vec , T fill_value) {
hFill ( vec.cbegin(),vec.cend() , fill_value);
}

void (*fptr_hFill_HString11STDIT)( std::vector<HString> & vec , HString fill_value) = &hFill;
void (*fptr_hFill_HBool11STDIT)( std::vector<HBool> & vec , HBool fill_value) = &hFill;
void (*fptr_hFill_HComplex11STDIT)( std::vector<HComplex> & vec , HComplex fill_value) = &hFill;
void (*fptr_hFill_HNumber11STDIT)( std::vector<HNumber> & vec , HNumber fill_value) = &hFill;
void (*fptr_hFill_HInteger11STDIT)( std::vector<HInteger> & vec , HInteger fill_value) = &hFill;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//-----------------------------------------------------------------------
/*!
  \brief Make and return a new vector of the same size and type as the input vector

    \param vec: Input vector

*/
template <class T>
std::vector<T> hNew(std::vector<T> & vec)
{
  std::vector<T> vnew(vec.size());
  return vnew;
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers

std::vector<HString> (*fptr_hNew_HString1STL)( std::vector<HString> & vec) = &hNew;
std::vector<HBool> (*fptr_hNew_HBool1STL)( std::vector<HBool> & vec) = &hNew;
std::vector<HComplex> (*fptr_hNew_HComplex1STL)( std::vector<HComplex> & vec) = &hNew;
std::vector<HNumber> (*fptr_hNew_HNumber1STL)( std::vector<HNumber> & vec) = &hNew;
std::vector<HInteger> (*fptr_hNew_HInteger1STL)( std::vector<HInteger> & vec) = &hNew;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
/*!

  \brief Resize a vector to a new length.

    \param vec: Input vector

    \param newsize: New size of vector


 */
template <class T>
void hResize(std::vector<T> & vec, HInteger newsize)
{
  vec.resize(newsize);
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers

void (*fptr_hResize_HString1HIntegerSTL)( std::vector<HString> & vec , HInteger newsize) = &hResize;
void (*fptr_hResize_HBool1HIntegerSTL)( std::vector<HBool> & vec , HInteger newsize) = &hResize;
void (*fptr_hResize_HComplex1HIntegerSTL)( std::vector<HComplex> & vec , HInteger newsize) = &hResize;
void (*fptr_hResize_HNumber1HIntegerSTL)( std::vector<HNumber> & vec , HInteger newsize) = &hResize;
void (*fptr_hResize_HInteger1HIntegerSTL)( std::vector<HInteger> & vec , HInteger newsize) = &hResize;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
/*!

  \brief Resize a vector to a new length and fill new elements in vector with a specific value.

    \param vec: Input vector

    \param newsize: New size

    \param fill: Value to fill new vector elements with


*/
template <class T>
void hResize(std::vector<T> & vec, HInteger newsize, T fill)
{
  vec.resize(newsize,fill);
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers

void (*fptr_hResize_HString1HInteger1STL)( std::vector<HString> & vec , HInteger newsize , HString fill) = &hResize;
void (*fptr_hResize_HBool1HInteger1STL)( std::vector<HBool> & vec , HInteger newsize , HBool fill) = &hResize;
void (*fptr_hResize_HComplex1HInteger1STL)( std::vector<HComplex> & vec , HInteger newsize , HComplex fill) = &hResize;
void (*fptr_hResize_HNumber1HInteger1STL)( std::vector<HNumber> & vec , HInteger newsize , HNumber fill) = &hResize;
void (*fptr_hResize_HInteger1HInteger1STL)( std::vector<HInteger> & vec , HInteger newsize , HInteger fill) = &hResize;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
/*!

  \brief Resize an STL vector to the same length as a second vector

    \param vec1: Input vector to be resized

    \param vec2: Reference vector


*/
template <class T, class S>
void hResize(std::vector<T> & vec1,std::vector<S> & vec2)
{
  vec1.resize(vec2.size());
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers

void (*fptr_hResize_HStringHString12STLSTL)( std::vector<HString> & vec1 , std::vector<HString> & vec2) = &hResize;
void (*fptr_hResize_HStringHBool12STLSTL)( std::vector<HString> & vec1 , std::vector<HBool> & vec2) = &hResize;
void (*fptr_hResize_HStringHComplex12STLSTL)( std::vector<HString> & vec1 , std::vector<HComplex> & vec2) = &hResize;
void (*fptr_hResize_HStringHNumber12STLSTL)( std::vector<HString> & vec1 , std::vector<HNumber> & vec2) = &hResize;
void (*fptr_hResize_HStringHInteger12STLSTL)( std::vector<HString> & vec1 , std::vector<HInteger> & vec2) = &hResize;
void (*fptr_hResize_HBoolHString12STLSTL)( std::vector<HBool> & vec1 , std::vector<HString> & vec2) = &hResize;
void (*fptr_hResize_HBoolHBool12STLSTL)( std::vector<HBool> & vec1 , std::vector<HBool> & vec2) = &hResize;
void (*fptr_hResize_HBoolHComplex12STLSTL)( std::vector<HBool> & vec1 , std::vector<HComplex> & vec2) = &hResize;
void (*fptr_hResize_HBoolHNumber12STLSTL)( std::vector<HBool> & vec1 , std::vector<HNumber> & vec2) = &hResize;
void (*fptr_hResize_HBoolHInteger12STLSTL)( std::vector<HBool> & vec1 , std::vector<HInteger> & vec2) = &hResize;
void (*fptr_hResize_HComplexHString12STLSTL)( std::vector<HComplex> & vec1 , std::vector<HString> & vec2) = &hResize;
void (*fptr_hResize_HComplexHBool12STLSTL)( std::vector<HComplex> & vec1 , std::vector<HBool> & vec2) = &hResize;
void (*fptr_hResize_HComplexHComplex12STLSTL)( std::vector<HComplex> & vec1 , std::vector<HComplex> & vec2) = &hResize;
void (*fptr_hResize_HComplexHNumber12STLSTL)( std::vector<HComplex> & vec1 , std::vector<HNumber> & vec2) = &hResize;
void (*fptr_hResize_HComplexHInteger12STLSTL)( std::vector<HComplex> & vec1 , std::vector<HInteger> & vec2) = &hResize;
void (*fptr_hResize_HNumberHString12STLSTL)( std::vector<HNumber> & vec1 , std::vector<HString> & vec2) = &hResize;
void (*fptr_hResize_HNumberHBool12STLSTL)( std::vector<HNumber> & vec1 , std::vector<HBool> & vec2) = &hResize;
void (*fptr_hResize_HNumberHComplex12STLSTL)( std::vector<HNumber> & vec1 , std::vector<HComplex> & vec2) = &hResize;
void (*fptr_hResize_HNumberHNumber12STLSTL)( std::vector<HNumber> & vec1 , std::vector<HNumber> & vec2) = &hResize;
void (*fptr_hResize_HNumberHInteger12STLSTL)( std::vector<HNumber> & vec1 , std::vector<HInteger> & vec2) = &hResize;
void (*fptr_hResize_HIntegerHString12STLSTL)( std::vector<HInteger> & vec1 , std::vector<HString> & vec2) = &hResize;
void (*fptr_hResize_HIntegerHBool12STLSTL)( std::vector<HInteger> & vec1 , std::vector<HBool> & vec2) = &hResize;
void (*fptr_hResize_HIntegerHComplex12STLSTL)( std::vector<HInteger> & vec1 , std::vector<HComplex> & vec2) = &hResize;
void (*fptr_hResize_HIntegerHNumber12STLSTL)( std::vector<HInteger> & vec1 , std::vector<HNumber> & vec2) = &hResize;
void (*fptr_hResize_HIntegerHInteger12STLSTL)( std::vector<HInteger> & vec1 , std::vector<HInteger> & vec2) = &hResize;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
/*!

  \brief Resize a casa vector to the same length as a second vector

    \param vec1: Input vector to be resized

    \param vec2: Reference vector


*/
template <class T, class S>
void hResize(casa::Vector<T> & vec1,casa::Vector<S> & vec2)
{
  vec1.resize(*(vec2.shape().begin()));
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers

void (*fptr_hResize_HStringHString12CASACASA)( std::vector<HString> & vec1 , std::vector<HString> & vec2) = &hResize;
void (*fptr_hResize_HStringHBool12CASACASA)( std::vector<HString> & vec1 , std::vector<HBool> & vec2) = &hResize;
void (*fptr_hResize_HStringHComplex12CASACASA)( std::vector<HString> & vec1 , std::vector<HComplex> & vec2) = &hResize;
void (*fptr_hResize_HStringHNumber12CASACASA)( std::vector<HString> & vec1 , std::vector<HNumber> & vec2) = &hResize;
void (*fptr_hResize_HStringHInteger12CASACASA)( std::vector<HString> & vec1 , std::vector<HInteger> & vec2) = &hResize;
void (*fptr_hResize_HBoolHString12CASACASA)( std::vector<HBool> & vec1 , std::vector<HString> & vec2) = &hResize;
void (*fptr_hResize_HBoolHBool12CASACASA)( std::vector<HBool> & vec1 , std::vector<HBool> & vec2) = &hResize;
void (*fptr_hResize_HBoolHComplex12CASACASA)( std::vector<HBool> & vec1 , std::vector<HComplex> & vec2) = &hResize;
void (*fptr_hResize_HBoolHNumber12CASACASA)( std::vector<HBool> & vec1 , std::vector<HNumber> & vec2) = &hResize;
void (*fptr_hResize_HBoolHInteger12CASACASA)( std::vector<HBool> & vec1 , std::vector<HInteger> & vec2) = &hResize;
void (*fptr_hResize_HComplexHString12CASACASA)( std::vector<HComplex> & vec1 , std::vector<HString> & vec2) = &hResize;
void (*fptr_hResize_HComplexHBool12CASACASA)( std::vector<HComplex> & vec1 , std::vector<HBool> & vec2) = &hResize;
void (*fptr_hResize_HComplexHComplex12CASACASA)( std::vector<HComplex> & vec1 , std::vector<HComplex> & vec2) = &hResize;
void (*fptr_hResize_HComplexHNumber12CASACASA)( std::vector<HComplex> & vec1 , std::vector<HNumber> & vec2) = &hResize;
void (*fptr_hResize_HComplexHInteger12CASACASA)( std::vector<HComplex> & vec1 , std::vector<HInteger> & vec2) = &hResize;
void (*fptr_hResize_HNumberHString12CASACASA)( std::vector<HNumber> & vec1 , std::vector<HString> & vec2) = &hResize;
void (*fptr_hResize_HNumberHBool12CASACASA)( std::vector<HNumber> & vec1 , std::vector<HBool> & vec2) = &hResize;
void (*fptr_hResize_HNumberHComplex12CASACASA)( std::vector<HNumber> & vec1 , std::vector<HComplex> & vec2) = &hResize;
void (*fptr_hResize_HNumberHNumber12CASACASA)( std::vector<HNumber> & vec1 , std::vector<HNumber> & vec2) = &hResize;
void (*fptr_hResize_HNumberHInteger12CASACASA)( std::vector<HNumber> & vec1 , std::vector<HInteger> & vec2) = &hResize;
void (*fptr_hResize_HIntegerHString12CASACASA)( std::vector<HInteger> & vec1 , std::vector<HString> & vec2) = &hResize;
void (*fptr_hResize_HIntegerHBool12CASACASA)( std::vector<HInteger> & vec1 , std::vector<HBool> & vec2) = &hResize;
void (*fptr_hResize_HIntegerHComplex12CASACASA)( std::vector<HInteger> & vec1 , std::vector<HComplex> & vec2) = &hResize;
void (*fptr_hResize_HIntegerHNumber12CASACASA)( std::vector<HInteger> & vec1 , std::vector<HNumber> & vec2) = &hResize;
void (*fptr_hResize_HIntegerHInteger12CASACASA)( std::vector<HInteger> & vec1 , std::vector<HInteger> & vec2) = &hResize;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//-----------------------------------------------------------------------
/*!

  \brief Copies and converts a vector to a vector of another type.

    \param vec1: Numeric input vector

    \param vec2: Vector containing a copy of the input values converted to a new type

*/
template <class Iterin, class Iter>
void hConvert(const Iterin vec1,const Iterin vec1_end, const Iter vec2,const Iter vec2_end)
{
  typedef typename Iter::value_type T;
  Iterin it=vec1;
  Iter itout=vec2;
  while ((it!=vec1_end) && (itout !=vec2_end)) {
    *itout=hfcast<T>(*it);
    ++it; ++itout;
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T , class S > inline void hConvert( std::vector<T> & vec1 , std::vector<S> & vec2) {
hResize(vec2,vec1); hConvert ( vec1.begin(),vec1.end() , vec2.begin(),vec2.end());
}
template < class T , class S > inline void hConvert( casa::Vector<T> & vec1 , casa::Vector<S> & vec2) {
hResize(vec2,vec1); hConvert ( vec1.cbegin(),vec1.cend() , vec2.cbegin(),vec2.cend());
}

void (*fptr_hConvert_HComplexHComplex12STDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HComplex> & vec2) = &hConvert;
void (*fptr_hConvert_HComplexHNumber12STDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HNumber> & vec2) = &hConvert;
void (*fptr_hConvert_HComplexHInteger12STDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HInteger> & vec2) = &hConvert;
void (*fptr_hConvert_HNumberHComplex12STDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HComplex> & vec2) = &hConvert;
void (*fptr_hConvert_HNumberHNumber12STDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HNumber> & vec2) = &hConvert;
void (*fptr_hConvert_HNumberHInteger12STDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HInteger> & vec2) = &hConvert;
void (*fptr_hConvert_HIntegerHComplex12STDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HComplex> & vec2) = &hConvert;
void (*fptr_hConvert_HIntegerHNumber12STDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HNumber> & vec2) = &hConvert;
void (*fptr_hConvert_HIntegerHInteger12STDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HInteger> & vec2) = &hConvert;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//-----------------------------------------------------------------------
/*!

  \brief Copies a vector to another one.

  \param vec: STL Iterator pointing to the first element of an array with
         input values.
  \param vec_end: STL Iterator pointing to the end of the input vector

  \param out: STL Iterator pointing to the first element of an array with
         output values.
  \param out_end: STL Iterator pointing to the end of the output vector
*/
template <class Iterin, class Iter>
void hCopy(const Iterin vec,const Iterin vec_end, const Iter out,const Iter out_end)
{
  typedef typename Iter::value_type T;
  Iterin it=vec;
  Iter itout=out;
  while ((it!=vec_end) && (itout !=out_end)) {
    *itout=*it;
    ++it; ++itout;
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T > inline void hCopy( std::vector<T> & vec , std::vector<T> & outvec) {
hCopy ( vec.begin(),vec.end() , outvec.begin(),outvec.end());
}
template < class T > inline void hCopy( casa::Vector<T> & vec , casa::Vector<T> & outvec) {
hCopy ( vec.cbegin(),vec.cend() , outvec.cbegin(),outvec.cend());
}

void (*fptr_hCopy_HComplex11STDITSTDIT)( std::vector<HComplex> & vec , std::vector<HComplex> & outvec) = &hCopy;
void (*fptr_hCopy_HNumber11STDITSTDIT)( std::vector<HNumber> & vec , std::vector<HNumber> & outvec) = &hCopy;
void (*fptr_hCopy_HInteger11STDITSTDIT)( std::vector<HInteger> & vec , std::vector<HInteger> & outvec) = &hCopy;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//========================================================================
//                           Math Functions
//========================================================================
//------------------------------------------------------------------------
/*!
 \brief Returns the squared value of the parameter

    \param val: Value to be squared

*/
template <class T>
inline T square(T val)
{
  return val*val;
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers

HComplex (*fptr_square_HComplex1)( HComplex val) = &square;
HNumber (*fptr_square_HNumber1)( HNumber val) = &square;
HInteger (*fptr_square_HInteger1)( HInteger val) = &square;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//------------------------------------------------------------------------
/*!
 \brief Returns the interferometer phase in radians for a given frequency and time

    \param frequency: Frequency in Hz

    \param time: Time in seconds

*/
inline HNumber hPhase(HNumber frequency, HNumber time)
{
  return CR::_2pi*frequency*time;
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers

HNumber (*fptr_hPhase_HIntegerHNumberHNumber)( HNumber frequency , HNumber time) = &hPhase;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//------------------------------------------------------------------------
/*!
 \brief Implementation of the Gauss function

    \param x: Position at which the Gaussian is evaluated

    \param sigma: Width of the Gaussian

    \param mu: Mean value of the Gaussian

*/
inline HNumber funcGaussian (HNumber x,
        HNumber sigma,
        HNumber mu)
{
  return exp(-(x-mu)*(x-mu)/(2*sigma*sigma))/(sigma*sqrt(2*casa::C::pi));
};
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers

HNumber (*fptr_funcGaussian_HIntegerHNumberHNumberHNumber)( HNumber x , HNumber sigma , HNumber mu) = &funcGaussian;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//========================================================================
//$ITERATE MFUNC abs,cos,cosh,exp,log,log10,sin,sinh,sqrt,square,tan,tanh
//========================================================================
/*!

  \brief Take the exp of all the elements in the vector.

  \param vec: STL Iterator pointing to the first element of an array with
         input values.
  \param vec_end: STL Iterator pointing to the end of the input vector

*/
template <class Iter>
void hExp1(const Iter vec,const Iter vec_end)
{
  Iter it=vec;
  while (it!=vec_end) {
    *it=exp(*it);
    ++it;
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T > inline void hExp( std::vector<T> & vec) {
hExp1 ( vec.begin(),vec.end());
}
template < class T > inline void hExp( casa::Vector<T> & vec) {
hExp1 ( vec.cbegin(),vec.cend());
}

void (*fptr_hExp1_HComplex1STDIT)( std::vector<HComplex> & vec) = &hExp;
void (*fptr_hExp1_HNumber1STDIT)( std::vector<HNumber> & vec) = &hExp;
void (*fptr_hExp1_HInteger1STDIT)( std::vector<HInteger> & vec) = &hExp;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
/*!

  \brief Take the exp of all the elements in the vector and return results in a second vector.

  \param vec: STL Iterator pointing to the first element of an array with
         input values.
  \param vec_end: STL Iterator pointing to the end of the input vector

  \param out: STL Iterator pointing to the first element of an array with
         output values.
  \param out_end: STL Iterator pointing to the end of the output vector

*/
template <class Iter>
void hExp2(const Iter vec,const Iter vec_end, const Iter out,const Iter out_end)
{
  Iter it=vec;
  Iter itout=out;
  while ((it!=vec_end) && (itout !=out_end)) {
    *itout=exp(*it);
    ++it; ++itout;
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T > inline void hExp( std::vector<T> & vec , std::vector<T> & vecout) {
hExp2 ( vec.begin(),vec.end() , vecout.begin(),vecout.end());
}
template < class T > inline void hExp( casa::Vector<T> & vec , casa::Vector<T> & vecout) {
hExp2 ( vec.cbegin(),vec.cend() , vecout.cbegin(),vecout.cend());
}

void (*fptr_hExp2_HComplex11STDITSTDIT)( std::vector<HComplex> & vec , std::vector<HComplex> & vecout) = &hExp;
void (*fptr_hExp2_HNumber11STDITSTDIT)( std::vector<HNumber> & vec , std::vector<HNumber> & vecout) = &hExp;
void (*fptr_hExp2_HInteger11STDITSTDIT)( std::vector<HInteger> & vec , std::vector<HInteger> & vecout) = &hExp;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//========================================================================
/*!

  \brief Take the log of all the elements in the vector.

  \param vec: STL Iterator pointing to the first element of an array with
         input values.
  \param vec_end: STL Iterator pointing to the end of the input vector

*/
template <class Iter>
void hLog1(const Iter vec,const Iter vec_end)
{
  Iter it=vec;
  while (it!=vec_end) {
    *it=log(*it);
    ++it;
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T > inline void hLog( std::vector<T> & vec) {
hLog1 ( vec.begin(),vec.end());
}
template < class T > inline void hLog( casa::Vector<T> & vec) {
hLog1 ( vec.cbegin(),vec.cend());
}

void (*fptr_hLog1_HComplex1STDIT)( std::vector<HComplex> & vec) = &hLog;
void (*fptr_hLog1_HNumber1STDIT)( std::vector<HNumber> & vec) = &hLog;
void (*fptr_hLog1_HInteger1STDIT)( std::vector<HInteger> & vec) = &hLog;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
/*!

  \brief Take the log of all the elements in the vector and return results in a second vector.

  \param vec: STL Iterator pointing to the first element of an array with
         input values.
  \param vec_end: STL Iterator pointing to the end of the input vector

  \param out: STL Iterator pointing to the first element of an array with
         output values.
  \param out_end: STL Iterator pointing to the end of the output vector

*/
template <class Iter>
void hLog2(const Iter vec,const Iter vec_end, const Iter out,const Iter out_end)
{
  Iter it=vec;
  Iter itout=out;
  while ((it!=vec_end) && (itout !=out_end)) {
    *itout=log(*it);
    ++it; ++itout;
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T > inline void hLog( std::vector<T> & vec , std::vector<T> & vecout) {
hLog2 ( vec.begin(),vec.end() , vecout.begin(),vecout.end());
}
template < class T > inline void hLog( casa::Vector<T> & vec , casa::Vector<T> & vecout) {
hLog2 ( vec.cbegin(),vec.cend() , vecout.cbegin(),vecout.cend());
}

void (*fptr_hLog2_HComplex11STDITSTDIT)( std::vector<HComplex> & vec , std::vector<HComplex> & vecout) = &hLog;
void (*fptr_hLog2_HNumber11STDITSTDIT)( std::vector<HNumber> & vec , std::vector<HNumber> & vecout) = &hLog;
void (*fptr_hLog2_HInteger11STDITSTDIT)( std::vector<HInteger> & vec , std::vector<HInteger> & vecout) = &hLog;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//========================================================================
/*!

  \brief Take the log10 of all the elements in the vector.

  \param vec: STL Iterator pointing to the first element of an array with
         input values.
  \param vec_end: STL Iterator pointing to the end of the input vector

*/
template <class Iter>
void hLog101(const Iter vec,const Iter vec_end)
{
  Iter it=vec;
  while (it!=vec_end) {
    *it=log10(*it);
    ++it;
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T > inline void hLog10( std::vector<T> & vec) {
hLog101 ( vec.begin(),vec.end());
}
template < class T > inline void hLog10( casa::Vector<T> & vec) {
hLog101 ( vec.cbegin(),vec.cend());
}

void (*fptr_hLog101_HComplex1STDIT)( std::vector<HComplex> & vec) = &hLog10;
void (*fptr_hLog101_HNumber1STDIT)( std::vector<HNumber> & vec) = &hLog10;
void (*fptr_hLog101_HInteger1STDIT)( std::vector<HInteger> & vec) = &hLog10;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
/*!

  \brief Take the log10 of all the elements in the vector and return results in a second vector.

  \param vec: STL Iterator pointing to the first element of an array with
         input values.
  \param vec_end: STL Iterator pointing to the end of the input vector

  \param out: STL Iterator pointing to the first element of an array with
         output values.
  \param out_end: STL Iterator pointing to the end of the output vector

*/
template <class Iter>
void hLog102(const Iter vec,const Iter vec_end, const Iter out,const Iter out_end)
{
  Iter it=vec;
  Iter itout=out;
  while ((it!=vec_end) && (itout !=out_end)) {
    *itout=log10(*it);
    ++it; ++itout;
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T > inline void hLog10( std::vector<T> & vec , std::vector<T> & vecout) {
hLog102 ( vec.begin(),vec.end() , vecout.begin(),vecout.end());
}
template < class T > inline void hLog10( casa::Vector<T> & vec , casa::Vector<T> & vecout) {
hLog102 ( vec.cbegin(),vec.cend() , vecout.cbegin(),vecout.cend());
}

void (*fptr_hLog102_HComplex11STDITSTDIT)( std::vector<HComplex> & vec , std::vector<HComplex> & vecout) = &hLog10;
void (*fptr_hLog102_HNumber11STDITSTDIT)( std::vector<HNumber> & vec , std::vector<HNumber> & vecout) = &hLog10;
void (*fptr_hLog102_HInteger11STDITSTDIT)( std::vector<HInteger> & vec , std::vector<HInteger> & vecout) = &hLog10;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//========================================================================
/*!

  \brief Take the sin of all the elements in the vector.

  \param vec: STL Iterator pointing to the first element of an array with
         input values.
  \param vec_end: STL Iterator pointing to the end of the input vector

*/
template <class Iter>
void hSin1(const Iter vec,const Iter vec_end)
{
  Iter it=vec;
  while (it!=vec_end) {
    *it=sin(*it);
    ++it;
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T > inline void hSin( std::vector<T> & vec) {
hSin1 ( vec.begin(),vec.end());
}
template < class T > inline void hSin( casa::Vector<T> & vec) {
hSin1 ( vec.cbegin(),vec.cend());
}

void (*fptr_hSin1_HComplex1STDIT)( std::vector<HComplex> & vec) = &hSin;
void (*fptr_hSin1_HNumber1STDIT)( std::vector<HNumber> & vec) = &hSin;
void (*fptr_hSin1_HInteger1STDIT)( std::vector<HInteger> & vec) = &hSin;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
/*!

  \brief Take the sin of all the elements in the vector and return results in a second vector.

  \param vec: STL Iterator pointing to the first element of an array with
         input values.
  \param vec_end: STL Iterator pointing to the end of the input vector

  \param out: STL Iterator pointing to the first element of an array with
         output values.
  \param out_end: STL Iterator pointing to the end of the output vector

*/
template <class Iter>
void hSin2(const Iter vec,const Iter vec_end, const Iter out,const Iter out_end)
{
  Iter it=vec;
  Iter itout=out;
  while ((it!=vec_end) && (itout !=out_end)) {
    *itout=sin(*it);
    ++it; ++itout;
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T > inline void hSin( std::vector<T> & vec , std::vector<T> & vecout) {
hSin2 ( vec.begin(),vec.end() , vecout.begin(),vecout.end());
}
template < class T > inline void hSin( casa::Vector<T> & vec , casa::Vector<T> & vecout) {
hSin2 ( vec.cbegin(),vec.cend() , vecout.cbegin(),vecout.cend());
}

void (*fptr_hSin2_HComplex11STDITSTDIT)( std::vector<HComplex> & vec , std::vector<HComplex> & vecout) = &hSin;
void (*fptr_hSin2_HNumber11STDITSTDIT)( std::vector<HNumber> & vec , std::vector<HNumber> & vecout) = &hSin;
void (*fptr_hSin2_HInteger11STDITSTDIT)( std::vector<HInteger> & vec , std::vector<HInteger> & vecout) = &hSin;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//========================================================================
/*!

  \brief Take the sinh of all the elements in the vector.

  \param vec: STL Iterator pointing to the first element of an array with
         input values.
  \param vec_end: STL Iterator pointing to the end of the input vector

*/
template <class Iter>
void hSinh1(const Iter vec,const Iter vec_end)
{
  Iter it=vec;
  while (it!=vec_end) {
    *it=sinh(*it);
    ++it;
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T > inline void hSinh( std::vector<T> & vec) {
hSinh1 ( vec.begin(),vec.end());
}
template < class T > inline void hSinh( casa::Vector<T> & vec) {
hSinh1 ( vec.cbegin(),vec.cend());
}

void (*fptr_hSinh1_HComplex1STDIT)( std::vector<HComplex> & vec) = &hSinh;
void (*fptr_hSinh1_HNumber1STDIT)( std::vector<HNumber> & vec) = &hSinh;
void (*fptr_hSinh1_HInteger1STDIT)( std::vector<HInteger> & vec) = &hSinh;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
/*!

  \brief Take the sinh of all the elements in the vector and return results in a second vector.

  \param vec: STL Iterator pointing to the first element of an array with
         input values.
  \param vec_end: STL Iterator pointing to the end of the input vector

  \param out: STL Iterator pointing to the first element of an array with
         output values.
  \param out_end: STL Iterator pointing to the end of the output vector

*/
template <class Iter>
void hSinh2(const Iter vec,const Iter vec_end, const Iter out,const Iter out_end)
{
  Iter it=vec;
  Iter itout=out;
  while ((it!=vec_end) && (itout !=out_end)) {
    *itout=sinh(*it);
    ++it; ++itout;
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T > inline void hSinh( std::vector<T> & vec , std::vector<T> & vecout) {
hSinh2 ( vec.begin(),vec.end() , vecout.begin(),vecout.end());
}
template < class T > inline void hSinh( casa::Vector<T> & vec , casa::Vector<T> & vecout) {
hSinh2 ( vec.cbegin(),vec.cend() , vecout.cbegin(),vecout.cend());
}

void (*fptr_hSinh2_HComplex11STDITSTDIT)( std::vector<HComplex> & vec , std::vector<HComplex> & vecout) = &hSinh;
void (*fptr_hSinh2_HNumber11STDITSTDIT)( std::vector<HNumber> & vec , std::vector<HNumber> & vecout) = &hSinh;
void (*fptr_hSinh2_HInteger11STDITSTDIT)( std::vector<HInteger> & vec , std::vector<HInteger> & vecout) = &hSinh;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//========================================================================
/*!

  \brief Take the sqrt of all the elements in the vector.

  \param vec: STL Iterator pointing to the first element of an array with
         input values.
  \param vec_end: STL Iterator pointing to the end of the input vector

*/
template <class Iter>
void hSqrt1(const Iter vec,const Iter vec_end)
{
  Iter it=vec;
  while (it!=vec_end) {
    *it=sqrt(*it);
    ++it;
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T > inline void hSqrt( std::vector<T> & vec) {
hSqrt1 ( vec.begin(),vec.end());
}
template < class T > inline void hSqrt( casa::Vector<T> & vec) {
hSqrt1 ( vec.cbegin(),vec.cend());
}

void (*fptr_hSqrt1_HComplex1STDIT)( std::vector<HComplex> & vec) = &hSqrt;
void (*fptr_hSqrt1_HNumber1STDIT)( std::vector<HNumber> & vec) = &hSqrt;
void (*fptr_hSqrt1_HInteger1STDIT)( std::vector<HInteger> & vec) = &hSqrt;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
/*!

  \brief Take the sqrt of all the elements in the vector and return results in a second vector.

  \param vec: STL Iterator pointing to the first element of an array with
         input values.
  \param vec_end: STL Iterator pointing to the end of the input vector

  \param out: STL Iterator pointing to the first element of an array with
         output values.
  \param out_end: STL Iterator pointing to the end of the output vector

*/
template <class Iter>
void hSqrt2(const Iter vec,const Iter vec_end, const Iter out,const Iter out_end)
{
  Iter it=vec;
  Iter itout=out;
  while ((it!=vec_end) && (itout !=out_end)) {
    *itout=sqrt(*it);
    ++it; ++itout;
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T > inline void hSqrt( std::vector<T> & vec , std::vector<T> & vecout) {
hSqrt2 ( vec.begin(),vec.end() , vecout.begin(),vecout.end());
}
template < class T > inline void hSqrt( casa::Vector<T> & vec , casa::Vector<T> & vecout) {
hSqrt2 ( vec.cbegin(),vec.cend() , vecout.cbegin(),vecout.cend());
}

void (*fptr_hSqrt2_HComplex11STDITSTDIT)( std::vector<HComplex> & vec , std::vector<HComplex> & vecout) = &hSqrt;
void (*fptr_hSqrt2_HNumber11STDITSTDIT)( std::vector<HNumber> & vec , std::vector<HNumber> & vecout) = &hSqrt;
void (*fptr_hSqrt2_HInteger11STDITSTDIT)( std::vector<HInteger> & vec , std::vector<HInteger> & vecout) = &hSqrt;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//========================================================================
/*!

  \brief Take the square of all the elements in the vector.

  \param vec: STL Iterator pointing to the first element of an array with
         input values.
  \param vec_end: STL Iterator pointing to the end of the input vector

*/
template <class Iter>
void hSquare1(const Iter vec,const Iter vec_end)
{
  Iter it=vec;
  while (it!=vec_end) {
    *it=square(*it);
    ++it;
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T > inline void hSquare( std::vector<T> & vec) {
hSquare1 ( vec.begin(),vec.end());
}
template < class T > inline void hSquare( casa::Vector<T> & vec) {
hSquare1 ( vec.cbegin(),vec.cend());
}

void (*fptr_hSquare1_HComplex1STDIT)( std::vector<HComplex> & vec) = &hSquare;
void (*fptr_hSquare1_HNumber1STDIT)( std::vector<HNumber> & vec) = &hSquare;
void (*fptr_hSquare1_HInteger1STDIT)( std::vector<HInteger> & vec) = &hSquare;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
/*!

  \brief Take the square of all the elements in the vector and return results in a second vector.

  \param vec: STL Iterator pointing to the first element of an array with
         input values.
  \param vec_end: STL Iterator pointing to the end of the input vector

  \param out: STL Iterator pointing to the first element of an array with
         output values.
  \param out_end: STL Iterator pointing to the end of the output vector

*/
template <class Iter>
void hSquare2(const Iter vec,const Iter vec_end, const Iter out,const Iter out_end)
{
  Iter it=vec;
  Iter itout=out;
  while ((it!=vec_end) && (itout !=out_end)) {
    *itout=square(*it);
    ++it; ++itout;
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T > inline void hSquare( std::vector<T> & vec , std::vector<T> & vecout) {
hSquare2 ( vec.begin(),vec.end() , vecout.begin(),vecout.end());
}
template < class T > inline void hSquare( casa::Vector<T> & vec , casa::Vector<T> & vecout) {
hSquare2 ( vec.cbegin(),vec.cend() , vecout.cbegin(),vecout.cend());
}

void (*fptr_hSquare2_HComplex11STDITSTDIT)( std::vector<HComplex> & vec , std::vector<HComplex> & vecout) = &hSquare;
void (*fptr_hSquare2_HNumber11STDITSTDIT)( std::vector<HNumber> & vec , std::vector<HNumber> & vecout) = &hSquare;
void (*fptr_hSquare2_HInteger11STDITSTDIT)( std::vector<HInteger> & vec , std::vector<HInteger> & vecout) = &hSquare;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//========================================================================
/*!

  \brief Take the tan of all the elements in the vector.

  \param vec: STL Iterator pointing to the first element of an array with
         input values.
  \param vec_end: STL Iterator pointing to the end of the input vector

*/
template <class Iter>
void hTan1(const Iter vec,const Iter vec_end)
{
  Iter it=vec;
  while (it!=vec_end) {
    *it=tan(*it);
    ++it;
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T > inline void hTan( std::vector<T> & vec) {
hTan1 ( vec.begin(),vec.end());
}
template < class T > inline void hTan( casa::Vector<T> & vec) {
hTan1 ( vec.cbegin(),vec.cend());
}

void (*fptr_hTan1_HComplex1STDIT)( std::vector<HComplex> & vec) = &hTan;
void (*fptr_hTan1_HNumber1STDIT)( std::vector<HNumber> & vec) = &hTan;
void (*fptr_hTan1_HInteger1STDIT)( std::vector<HInteger> & vec) = &hTan;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
/*!

  \brief Take the tan of all the elements in the vector and return results in a second vector.

  \param vec: STL Iterator pointing to the first element of an array with
         input values.
  \param vec_end: STL Iterator pointing to the end of the input vector

  \param out: STL Iterator pointing to the first element of an array with
         output values.
  \param out_end: STL Iterator pointing to the end of the output vector

*/
template <class Iter>
void hTan2(const Iter vec,const Iter vec_end, const Iter out,const Iter out_end)
{
  Iter it=vec;
  Iter itout=out;
  while ((it!=vec_end) && (itout !=out_end)) {
    *itout=tan(*it);
    ++it; ++itout;
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T > inline void hTan( std::vector<T> & vec , std::vector<T> & vecout) {
hTan2 ( vec.begin(),vec.end() , vecout.begin(),vecout.end());
}
template < class T > inline void hTan( casa::Vector<T> & vec , casa::Vector<T> & vecout) {
hTan2 ( vec.cbegin(),vec.cend() , vecout.cbegin(),vecout.cend());
}

void (*fptr_hTan2_HComplex11STDITSTDIT)( std::vector<HComplex> & vec , std::vector<HComplex> & vecout) = &hTan;
void (*fptr_hTan2_HNumber11STDITSTDIT)( std::vector<HNumber> & vec , std::vector<HNumber> & vecout) = &hTan;
void (*fptr_hTan2_HInteger11STDITSTDIT)( std::vector<HInteger> & vec , std::vector<HInteger> & vecout) = &hTan;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//========================================================================
/*!

  \brief Take the tanh of all the elements in the vector.

  \param vec: STL Iterator pointing to the first element of an array with
         input values.
  \param vec_end: STL Iterator pointing to the end of the input vector

*/
template <class Iter>
void hTanh1(const Iter vec,const Iter vec_end)
{
  Iter it=vec;
  while (it!=vec_end) {
    *it=tanh(*it);
    ++it;
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T > inline void hTanh( std::vector<T> & vec) {
hTanh1 ( vec.begin(),vec.end());
}
template < class T > inline void hTanh( casa::Vector<T> & vec) {
hTanh1 ( vec.cbegin(),vec.cend());
}

void (*fptr_hTanh1_HComplex1STDIT)( std::vector<HComplex> & vec) = &hTanh;
void (*fptr_hTanh1_HNumber1STDIT)( std::vector<HNumber> & vec) = &hTanh;
void (*fptr_hTanh1_HInteger1STDIT)( std::vector<HInteger> & vec) = &hTanh;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
/*!

  \brief Take the tanh of all the elements in the vector and return results in a second vector.

  \param vec: STL Iterator pointing to the first element of an array with
         input values.
  \param vec_end: STL Iterator pointing to the end of the input vector

  \param out: STL Iterator pointing to the first element of an array with
         output values.
  \param out_end: STL Iterator pointing to the end of the output vector

*/
template <class Iter>
void hTanh2(const Iter vec,const Iter vec_end, const Iter out,const Iter out_end)
{
  Iter it=vec;
  Iter itout=out;
  while ((it!=vec_end) && (itout !=out_end)) {
    *itout=tanh(*it);
    ++it; ++itout;
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T > inline void hTanh( std::vector<T> & vec , std::vector<T> & vecout) {
hTanh2 ( vec.begin(),vec.end() , vecout.begin(),vecout.end());
}
template < class T > inline void hTanh( casa::Vector<T> & vec , casa::Vector<T> & vecout) {
hTanh2 ( vec.cbegin(),vec.cend() , vecout.cbegin(),vecout.cend());
}

void (*fptr_hTanh2_HComplex11STDITSTDIT)( std::vector<HComplex> & vec , std::vector<HComplex> & vecout) = &hTanh;
void (*fptr_hTanh2_HNumber11STDITSTDIT)( std::vector<HNumber> & vec , std::vector<HNumber> & vecout) = &hTanh;
void (*fptr_hTanh2_HInteger11STDITSTDIT)( std::vector<HInteger> & vec , std::vector<HInteger> & vecout) = &hTanh;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//========================================================================
/*!

  \brief Take the abs of all the elements in the vector.

  \param vec: STL Iterator pointing to the first element of an array with
         input values.
  \param vec_end: STL Iterator pointing to the end of the input vector

*/
template <class Iter>
void hAbs1(const Iter vec,const Iter vec_end)
{
  Iter it=vec;
  while (it!=vec_end) {
    *it=abs(*it);
    ++it;
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T > inline void hAbs( std::vector<T> & vec) {
hAbs1 ( vec.begin(),vec.end());
}
template < class T > inline void hAbs( casa::Vector<T> & vec) {
hAbs1 ( vec.cbegin(),vec.cend());
}

void (*fptr_hAbs1_HComplex1STDIT)( std::vector<HComplex> & vec) = &hAbs;
void (*fptr_hAbs1_HNumber1STDIT)( std::vector<HNumber> & vec) = &hAbs;
void (*fptr_hAbs1_HInteger1STDIT)( std::vector<HInteger> & vec) = &hAbs;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
/*!

  \brief Take the abs of all the elements in the vector and return results in a second vector.

  \param vec: STL Iterator pointing to the first element of an array with
         input values.
  \param vec_end: STL Iterator pointing to the end of the input vector

  \param out: STL Iterator pointing to the first element of an array with
         output values.
  \param out_end: STL Iterator pointing to the end of the output vector

*/
template <class Iter>
void hAbs2(const Iter vec,const Iter vec_end, const Iter out,const Iter out_end)
{
  Iter it=vec;
  Iter itout=out;
  while ((it!=vec_end) && (itout !=out_end)) {
    *itout=abs(*it);
    ++it; ++itout;
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T > inline void hAbs( std::vector<T> & vec , std::vector<T> & vecout) {
hAbs2 ( vec.begin(),vec.end() , vecout.begin(),vecout.end());
}
template < class T > inline void hAbs( casa::Vector<T> & vec , casa::Vector<T> & vecout) {
hAbs2 ( vec.cbegin(),vec.cend() , vecout.cbegin(),vecout.cend());
}

void (*fptr_hAbs2_HComplex11STDITSTDIT)( std::vector<HComplex> & vec , std::vector<HComplex> & vecout) = &hAbs;
void (*fptr_hAbs2_HNumber11STDITSTDIT)( std::vector<HNumber> & vec , std::vector<HNumber> & vecout) = &hAbs;
void (*fptr_hAbs2_HInteger11STDITSTDIT)( std::vector<HInteger> & vec , std::vector<HInteger> & vecout) = &hAbs;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//========================================================================
/*!

  \brief Take the cos of all the elements in the vector.

  \param vec: STL Iterator pointing to the first element of an array with
         input values.
  \param vec_end: STL Iterator pointing to the end of the input vector

*/
template <class Iter>
void hCos1(const Iter vec,const Iter vec_end)
{
  Iter it=vec;
  while (it!=vec_end) {
    *it=cos(*it);
    ++it;
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T > inline void hCos( std::vector<T> & vec) {
hCos1 ( vec.begin(),vec.end());
}
template < class T > inline void hCos( casa::Vector<T> & vec) {
hCos1 ( vec.cbegin(),vec.cend());
}

void (*fptr_hCos1_HComplex1STDIT)( std::vector<HComplex> & vec) = &hCos;
void (*fptr_hCos1_HNumber1STDIT)( std::vector<HNumber> & vec) = &hCos;
void (*fptr_hCos1_HInteger1STDIT)( std::vector<HInteger> & vec) = &hCos;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
/*!

  \brief Take the cos of all the elements in the vector and return results in a second vector.

  \param vec: STL Iterator pointing to the first element of an array with
         input values.
  \param vec_end: STL Iterator pointing to the end of the input vector

  \param out: STL Iterator pointing to the first element of an array with
         output values.
  \param out_end: STL Iterator pointing to the end of the output vector

*/
template <class Iter>
void hCos2(const Iter vec,const Iter vec_end, const Iter out,const Iter out_end)
{
  Iter it=vec;
  Iter itout=out;
  while ((it!=vec_end) && (itout !=out_end)) {
    *itout=cos(*it);
    ++it; ++itout;
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T > inline void hCos( std::vector<T> & vec , std::vector<T> & vecout) {
hCos2 ( vec.begin(),vec.end() , vecout.begin(),vecout.end());
}
template < class T > inline void hCos( casa::Vector<T> & vec , casa::Vector<T> & vecout) {
hCos2 ( vec.cbegin(),vec.cend() , vecout.cbegin(),vecout.cend());
}

void (*fptr_hCos2_HComplex11STDITSTDIT)( std::vector<HComplex> & vec , std::vector<HComplex> & vecout) = &hCos;
void (*fptr_hCos2_HNumber11STDITSTDIT)( std::vector<HNumber> & vec , std::vector<HNumber> & vecout) = &hCos;
void (*fptr_hCos2_HInteger11STDITSTDIT)( std::vector<HInteger> & vec , std::vector<HInteger> & vecout) = &hCos;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//========================================================================
/*!

  \brief Take the cosh of all the elements in the vector.

  \param vec: STL Iterator pointing to the first element of an array with
         input values.
  \param vec_end: STL Iterator pointing to the end of the input vector

*/
template <class Iter>
void hCosh1(const Iter vec,const Iter vec_end)
{
  Iter it=vec;
  while (it!=vec_end) {
    *it=cosh(*it);
    ++it;
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T > inline void hCosh( std::vector<T> & vec) {
hCosh1 ( vec.begin(),vec.end());
}
template < class T > inline void hCosh( casa::Vector<T> & vec) {
hCosh1 ( vec.cbegin(),vec.cend());
}

void (*fptr_hCosh1_HComplex1STDIT)( std::vector<HComplex> & vec) = &hCosh;
void (*fptr_hCosh1_HNumber1STDIT)( std::vector<HNumber> & vec) = &hCosh;
void (*fptr_hCosh1_HInteger1STDIT)( std::vector<HInteger> & vec) = &hCosh;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
/*!

  \brief Take the cosh of all the elements in the vector and return results in a second vector.

  \param vec: STL Iterator pointing to the first element of an array with
         input values.
  \param vec_end: STL Iterator pointing to the end of the input vector

  \param out: STL Iterator pointing to the first element of an array with
         output values.
  \param out_end: STL Iterator pointing to the end of the output vector

*/
template <class Iter>
void hCosh2(const Iter vec,const Iter vec_end, const Iter out,const Iter out_end)
{
  Iter it=vec;
  Iter itout=out;
  while ((it!=vec_end) && (itout !=out_end)) {
    *itout=cosh(*it);
    ++it; ++itout;
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T > inline void hCosh( std::vector<T> & vec , std::vector<T> & vecout) {
hCosh2 ( vec.begin(),vec.end() , vecout.begin(),vecout.end());
}
template < class T > inline void hCosh( casa::Vector<T> & vec , casa::Vector<T> & vecout) {
hCosh2 ( vec.cbegin(),vec.cend() , vecout.cbegin(),vecout.cend());
}

void (*fptr_hCosh2_HComplex11STDITSTDIT)( std::vector<HComplex> & vec , std::vector<HComplex> & vecout) = &hCosh;
void (*fptr_hCosh2_HNumber11STDITSTDIT)( std::vector<HNumber> & vec , std::vector<HNumber> & vecout) = &hCosh;
void (*fptr_hCosh2_HInteger11STDITSTDIT)( std::vector<HInteger> & vec , std::vector<HInteger> & vecout) = &hCosh;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//$ENDITERATE
//========================================================================
//$ITERATE MFUNC acos,asin,atan,ceil,floor
//========================================================================
/*!

  \brief Take the ceil of all the elements in the vector.

  \param vec: STL Iterator pointing to the first element of an array with
         input values.
  \param vec_end: STL Iterator pointing to the end of the input vector

*/
template <class Iter>
void hCeil1(const Iter vec,const Iter vec_end)
{
  Iter it=vec;
  while (it!=vec_end) {
    *it=ceil(*it);
    ++it;
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T > inline void hCeil( std::vector<T> & vec) {
hCeil1 ( vec.begin(),vec.end());
}
template < class T > inline void hCeil( casa::Vector<T> & vec) {
hCeil1 ( vec.cbegin(),vec.cend());
}

void (*fptr_hCeil1_HNumber1STDIT)( std::vector<HNumber> & vec) = &hCeil;
void (*fptr_hCeil1_HInteger1STDIT)( std::vector<HInteger> & vec) = &hCeil;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
/*!

  \brief Take the ceil of all the elements in the vector and return results in a second vector.

  \param vec: STL Iterator pointing to the first element of an array with
         input values.
  \param vec_end: STL Iterator pointing to the end of the input vector

  \param data: STL Iterator pointing to the first element of an array with
         output values.
  \param vec_end: STL Iterator pointing to the end of the output vector

*/
template <class Iter1,class Iter2>
void hCeil2(const Iter1 vec,const Iter1 vec_end, const Iter2 out,const Iter2 out_end)
{
  Iter1 it=vec;
  Iter2 itout=out;
  while ((it!=vec_end) && (itout !=out_end)) {
    *itout=ceil(*it);
    ++it; ++itout;
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T , class S > inline void hCeil( std::vector<T> & vec , std::vector<S> & vecout) {
hCeil2 ( vec.begin(),vec.end() , vecout.begin(),vecout.end());
}
template < class T , class S > inline void hCeil( casa::Vector<T> & vec , casa::Vector<S> & vecout) {
hCeil2 ( vec.cbegin(),vec.cend() , vecout.cbegin(),vecout.cend());
}

void (*fptr_hCeil2_HNumberHNumber12STDITSTDIT)( std::vector<HNumber> & vec , std::vector<HNumber> & vecout) = &hCeil;
void (*fptr_hCeil2_HNumberHInteger12STDITSTDIT)( std::vector<HNumber> & vec , std::vector<HInteger> & vecout) = &hCeil;
void (*fptr_hCeil2_HIntegerHNumber12STDITSTDIT)( std::vector<HInteger> & vec , std::vector<HNumber> & vecout) = &hCeil;
void (*fptr_hCeil2_HIntegerHInteger12STDITSTDIT)( std::vector<HInteger> & vec , std::vector<HInteger> & vecout) = &hCeil;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//========================================================================
/*!

  \brief Take the floor of all the elements in the vector.

  \param vec: STL Iterator pointing to the first element of an array with
         input values.
  \param vec_end: STL Iterator pointing to the end of the input vector

*/
template <class Iter>
void hFloor1(const Iter vec,const Iter vec_end)
{
  Iter it=vec;
  while (it!=vec_end) {
    *it=floor(*it);
    ++it;
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T > inline void hFloor( std::vector<T> & vec) {
hFloor1 ( vec.begin(),vec.end());
}
template < class T > inline void hFloor( casa::Vector<T> & vec) {
hFloor1 ( vec.cbegin(),vec.cend());
}

void (*fptr_hFloor1_HNumber1STDIT)( std::vector<HNumber> & vec) = &hFloor;
void (*fptr_hFloor1_HInteger1STDIT)( std::vector<HInteger> & vec) = &hFloor;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
/*!

  \brief Take the floor of all the elements in the vector and return results in a second vector.

  \param vec: STL Iterator pointing to the first element of an array with
         input values.
  \param vec_end: STL Iterator pointing to the end of the input vector

  \param data: STL Iterator pointing to the first element of an array with
         output values.
  \param vec_end: STL Iterator pointing to the end of the output vector

*/
template <class Iter1,class Iter2>
void hFloor2(const Iter1 vec,const Iter1 vec_end, const Iter2 out,const Iter2 out_end)
{
  Iter1 it=vec;
  Iter2 itout=out;
  while ((it!=vec_end) && (itout !=out_end)) {
    *itout=floor(*it);
    ++it; ++itout;
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T , class S > inline void hFloor( std::vector<T> & vec , std::vector<S> & vecout) {
hFloor2 ( vec.begin(),vec.end() , vecout.begin(),vecout.end());
}
template < class T , class S > inline void hFloor( casa::Vector<T> & vec , casa::Vector<S> & vecout) {
hFloor2 ( vec.cbegin(),vec.cend() , vecout.cbegin(),vecout.cend());
}

void (*fptr_hFloor2_HNumberHNumber12STDITSTDIT)( std::vector<HNumber> & vec , std::vector<HNumber> & vecout) = &hFloor;
void (*fptr_hFloor2_HNumberHInteger12STDITSTDIT)( std::vector<HNumber> & vec , std::vector<HInteger> & vecout) = &hFloor;
void (*fptr_hFloor2_HIntegerHNumber12STDITSTDIT)( std::vector<HInteger> & vec , std::vector<HNumber> & vecout) = &hFloor;
void (*fptr_hFloor2_HIntegerHInteger12STDITSTDIT)( std::vector<HInteger> & vec , std::vector<HInteger> & vecout) = &hFloor;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//========================================================================
/*!

  \brief Take the acos of all the elements in the vector.

  \param vec: STL Iterator pointing to the first element of an array with
         input values.
  \param vec_end: STL Iterator pointing to the end of the input vector

*/
template <class Iter>
void hAcos1(const Iter vec,const Iter vec_end)
{
  Iter it=vec;
  while (it!=vec_end) {
    *it=acos(*it);
    ++it;
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T > inline void hAcos( std::vector<T> & vec) {
hAcos1 ( vec.begin(),vec.end());
}
template < class T > inline void hAcos( casa::Vector<T> & vec) {
hAcos1 ( vec.cbegin(),vec.cend());
}

void (*fptr_hAcos1_HNumber1STDIT)( std::vector<HNumber> & vec) = &hAcos;
void (*fptr_hAcos1_HInteger1STDIT)( std::vector<HInteger> & vec) = &hAcos;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
/*!

  \brief Take the acos of all the elements in the vector and return results in a second vector.

  \param vec: STL Iterator pointing to the first element of an array with
         input values.
  \param vec_end: STL Iterator pointing to the end of the input vector

  \param data: STL Iterator pointing to the first element of an array with
         output values.
  \param vec_end: STL Iterator pointing to the end of the output vector

*/
template <class Iter1,class Iter2>
void hAcos2(const Iter1 vec,const Iter1 vec_end, const Iter2 out,const Iter2 out_end)
{
  Iter1 it=vec;
  Iter2 itout=out;
  while ((it!=vec_end) && (itout !=out_end)) {
    *itout=acos(*it);
    ++it; ++itout;
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T , class S > inline void hAcos( std::vector<T> & vec , std::vector<S> & vecout) {
hAcos2 ( vec.begin(),vec.end() , vecout.begin(),vecout.end());
}
template < class T , class S > inline void hAcos( casa::Vector<T> & vec , casa::Vector<S> & vecout) {
hAcos2 ( vec.cbegin(),vec.cend() , vecout.cbegin(),vecout.cend());
}

void (*fptr_hAcos2_HNumberHNumber12STDITSTDIT)( std::vector<HNumber> & vec , std::vector<HNumber> & vecout) = &hAcos;
void (*fptr_hAcos2_HNumberHInteger12STDITSTDIT)( std::vector<HNumber> & vec , std::vector<HInteger> & vecout) = &hAcos;
void (*fptr_hAcos2_HIntegerHNumber12STDITSTDIT)( std::vector<HInteger> & vec , std::vector<HNumber> & vecout) = &hAcos;
void (*fptr_hAcos2_HIntegerHInteger12STDITSTDIT)( std::vector<HInteger> & vec , std::vector<HInteger> & vecout) = &hAcos;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//========================================================================
/*!

  \brief Take the asin of all the elements in the vector.

  \param vec: STL Iterator pointing to the first element of an array with
         input values.
  \param vec_end: STL Iterator pointing to the end of the input vector

*/
template <class Iter>
void hAsin1(const Iter vec,const Iter vec_end)
{
  Iter it=vec;
  while (it!=vec_end) {
    *it=asin(*it);
    ++it;
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T > inline void hAsin( std::vector<T> & vec) {
hAsin1 ( vec.begin(),vec.end());
}
template < class T > inline void hAsin( casa::Vector<T> & vec) {
hAsin1 ( vec.cbegin(),vec.cend());
}

void (*fptr_hAsin1_HNumber1STDIT)( std::vector<HNumber> & vec) = &hAsin;
void (*fptr_hAsin1_HInteger1STDIT)( std::vector<HInteger> & vec) = &hAsin;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
/*!

  \brief Take the asin of all the elements in the vector and return results in a second vector.

  \param vec: STL Iterator pointing to the first element of an array with
         input values.
  \param vec_end: STL Iterator pointing to the end of the input vector

  \param data: STL Iterator pointing to the first element of an array with
         output values.
  \param vec_end: STL Iterator pointing to the end of the output vector

*/
template <class Iter1,class Iter2>
void hAsin2(const Iter1 vec,const Iter1 vec_end, const Iter2 out,const Iter2 out_end)
{
  Iter1 it=vec;
  Iter2 itout=out;
  while ((it!=vec_end) && (itout !=out_end)) {
    *itout=asin(*it);
    ++it; ++itout;
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T , class S > inline void hAsin( std::vector<T> & vec , std::vector<S> & vecout) {
hAsin2 ( vec.begin(),vec.end() , vecout.begin(),vecout.end());
}
template < class T , class S > inline void hAsin( casa::Vector<T> & vec , casa::Vector<S> & vecout) {
hAsin2 ( vec.cbegin(),vec.cend() , vecout.cbegin(),vecout.cend());
}

void (*fptr_hAsin2_HNumberHNumber12STDITSTDIT)( std::vector<HNumber> & vec , std::vector<HNumber> & vecout) = &hAsin;
void (*fptr_hAsin2_HNumberHInteger12STDITSTDIT)( std::vector<HNumber> & vec , std::vector<HInteger> & vecout) = &hAsin;
void (*fptr_hAsin2_HIntegerHNumber12STDITSTDIT)( std::vector<HInteger> & vec , std::vector<HNumber> & vecout) = &hAsin;
void (*fptr_hAsin2_HIntegerHInteger12STDITSTDIT)( std::vector<HInteger> & vec , std::vector<HInteger> & vecout) = &hAsin;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//========================================================================
/*!

  \brief Take the atan of all the elements in the vector.

  \param vec: STL Iterator pointing to the first element of an array with
         input values.
  \param vec_end: STL Iterator pointing to the end of the input vector

*/
template <class Iter>
void hAtan1(const Iter vec,const Iter vec_end)
{
  Iter it=vec;
  while (it!=vec_end) {
    *it=atan(*it);
    ++it;
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T > inline void hAtan( std::vector<T> & vec) {
hAtan1 ( vec.begin(),vec.end());
}
template < class T > inline void hAtan( casa::Vector<T> & vec) {
hAtan1 ( vec.cbegin(),vec.cend());
}

void (*fptr_hAtan1_HNumber1STDIT)( std::vector<HNumber> & vec) = &hAtan;
void (*fptr_hAtan1_HInteger1STDIT)( std::vector<HInteger> & vec) = &hAtan;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
/*!

  \brief Take the atan of all the elements in the vector and return results in a second vector.

  \param vec: STL Iterator pointing to the first element of an array with
         input values.
  \param vec_end: STL Iterator pointing to the end of the input vector

  \param data: STL Iterator pointing to the first element of an array with
         output values.
  \param vec_end: STL Iterator pointing to the end of the output vector

*/
template <class Iter1,class Iter2>
void hAtan2(const Iter1 vec,const Iter1 vec_end, const Iter2 out,const Iter2 out_end)
{
  Iter1 it=vec;
  Iter2 itout=out;
  while ((it!=vec_end) && (itout !=out_end)) {
    *itout=atan(*it);
    ++it; ++itout;
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T , class S > inline void hAtan( std::vector<T> & vec , std::vector<S> & vecout) {
hAtan2 ( vec.begin(),vec.end() , vecout.begin(),vecout.end());
}
template < class T , class S > inline void hAtan( casa::Vector<T> & vec , casa::Vector<S> & vecout) {
hAtan2 ( vec.cbegin(),vec.cend() , vecout.cbegin(),vecout.cend());
}

void (*fptr_hAtan2_HNumberHNumber12STDITSTDIT)( std::vector<HNumber> & vec , std::vector<HNumber> & vecout) = &hAtan;
void (*fptr_hAtan2_HNumberHInteger12STDITSTDIT)( std::vector<HNumber> & vec , std::vector<HInteger> & vecout) = &hAtan;
void (*fptr_hAtan2_HIntegerHNumber12STDITSTDIT)( std::vector<HInteger> & vec , std::vector<HNumber> & vecout) = &hAtan;
void (*fptr_hAtan2_HIntegerHInteger12STDITSTDIT)( std::vector<HInteger> & vec , std::vector<HInteger> & vecout) = &hAtan;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//$ENDITERATE
//========================================================================
//$ITERATE MFUNC Mul,Add,Div,Sub
//========================================================================
//-----------------------------------------------------------------------
/*!

  \brief Performs a Sub between the two vectors, which is returned in the first vector. If the second vector is shorter it will be applied multiple times.

  \param vec1: STL Iterator pointing to the first element of an array with
         input and output values.
  \param vec1_end: STL Iterator pointing to the end of the input vector

  \param vec2: STL Iterator pointing to the first element of an array with
         input values.
  \param vec2_end: STL Iterator pointing to the end of the input vector
*/
template <class Iter, class Iterin>
void hiSub(const Iter vec1,const Iter vec1_end, const Iterin vec2,const Iterin vec2_end)
{
  typedef typename Iter::value_type T;
  Iter it1=vec1;
  Iterin it2=vec2;
  while (it1!=vec1_end) {
    *it1 -= hfcast<T>(*it2);
    ++it1; ++it2;
    if (it2==vec2_end) it2=vec2;
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T , class S > inline void hiSub( std::vector<T> & vec1 , std::vector<S> & vec2) {
hiSub ( vec1.begin(),vec1.end() , vec2.begin(),vec2.end());
}
template < class T , class S > inline void hiSub( casa::Vector<T> & vec1 , casa::Vector<S> & vec2) {
hiSub ( vec1.cbegin(),vec1.cend() , vec2.cbegin(),vec2.cend());
}

void (*fptr_hiSub_HComplexHComplex12STDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HComplex> & vec2) = &hiSub;
void (*fptr_hiSub_HComplexHNumber12STDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HNumber> & vec2) = &hiSub;
void (*fptr_hiSub_HComplexHInteger12STDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HInteger> & vec2) = &hiSub;
void (*fptr_hiSub_HNumberHComplex12STDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HComplex> & vec2) = &hiSub;
void (*fptr_hiSub_HNumberHNumber12STDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HNumber> & vec2) = &hiSub;
void (*fptr_hiSub_HNumberHInteger12STDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HInteger> & vec2) = &hiSub;
void (*fptr_hiSub_HIntegerHComplex12STDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HComplex> & vec2) = &hiSub;
void (*fptr_hiSub_HIntegerHNumber12STDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HNumber> & vec2) = &hiSub;
void (*fptr_hiSub_HIntegerHInteger12STDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HInteger> & vec2) = &hiSub;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//-----------------------------------------------------------------------
/*!

  \brief Performs a Sub between the vector and a scalar (applied to each element), which is returned in the first vector.

  \param vec1: STL Iterator pointing to the first element of an array with
         input and output values.
  \param vec1_end: STL Iterator pointing to the end of the input vector

  \param val: Value containing the second operand
*/
template <class Iter, class S>
void hiSub2(const Iter vec1,const Iter vec1_end, S val)
{
  typedef typename Iter::value_type T;
  Iter it=vec1;
  T val_t = hfcast<T>(val);
  while (it!=vec1_end) {
    *it -= val_t;
    ++it;
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T , class S > inline void hiSub( std::vector<T> & vec1 , S val) {
hiSub2 ( vec1.begin(),vec1.end() , val);
}
template < class T , class S > inline void hiSub( casa::Vector<T> & vec1 , S val) {
hiSub2 ( vec1.cbegin(),vec1.cend() , val);
}

void (*fptr_hiSub2_HComplexHComplex12STDIT)( std::vector<HComplex> & vec1 , HComplex val) = &hiSub;
void (*fptr_hiSub2_HComplexHNumber12STDIT)( std::vector<HComplex> & vec1 , HNumber val) = &hiSub;
void (*fptr_hiSub2_HComplexHInteger12STDIT)( std::vector<HComplex> & vec1 , HInteger val) = &hiSub;
void (*fptr_hiSub2_HNumberHComplex12STDIT)( std::vector<HNumber> & vec1 , HComplex val) = &hiSub;
void (*fptr_hiSub2_HNumberHNumber12STDIT)( std::vector<HNumber> & vec1 , HNumber val) = &hiSub;
void (*fptr_hiSub2_HNumberHInteger12STDIT)( std::vector<HNumber> & vec1 , HInteger val) = &hiSub;
void (*fptr_hiSub2_HIntegerHComplex12STDIT)( std::vector<HInteger> & vec1 , HComplex val) = &hiSub;
void (*fptr_hiSub2_HIntegerHNumber12STDIT)( std::vector<HInteger> & vec1 , HNumber val) = &hiSub;
void (*fptr_hiSub2_HIntegerHInteger12STDIT)( std::vector<HInteger> & vec1 , HInteger val) = &hiSub;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//-----------------------------------------------------------------------
/*!

  \brief Performs a Sub between the two vectors, which is returned in the third vector.

  \param vec1: STL Iterator pointing to the first element of an array with
         input  values.
  \param vec1_end: STL Iterator pointing to the end of the input vector

  \param vec2: STL Iterator pointing to the first element of an array with
         input values.
  \param vec2_end: STL Iterator pointing to the end of the input vector

  \param vec3: STL Iterator pointing to the first element of an array with
         output values.
  \param vec3_end: STL Iterator pointing to the end of the output vector
*/
template <class Iterin1, class Iterin2, class Iter>
void hSub(const Iterin1 vec1,const Iterin1 vec1_end, const Iterin2 vec2,const Iterin2 vec2_end, const Iter vec3,const Iter vec3_end)
{
  typedef typename Iter::value_type T;
  Iterin1 it1=vec1;
  Iterin2 it2=vec2;
  Iter itout=vec3;
  while ((it1!=vec1_end) && (itout !=vec3_end)) {
    *itout = hfcast<T>((*it1) - (*it2));
    ++it1; ++it2; ++itout;
    if (it2==vec2_end) it2=vec2;
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T , class S , class U > inline void hSub( std::vector<T> & vec1 , std::vector<S> & vec2 , std::vector<U> & vec3) {
hSub ( vec1.begin(),vec1.end() , vec2.begin(),vec2.end() , vec3.begin(),vec3.end());
}
template < class T , class S , class U > inline void hSub( casa::Vector<T> & vec1 , casa::Vector<S> & vec2 , casa::Vector<U> & vec3) {
hSub ( vec1.cbegin(),vec1.cend() , vec2.cbegin(),vec2.cend() , vec3.cbegin(),vec3.cend());
}

void (*fptr_hSub_HComplexHComplexHComplex123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HComplex> & vec2 , std::vector<HComplex> & vec3) = &hSub;
void (*fptr_hSub_HComplexHComplexHNumber123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HComplex> & vec2 , std::vector<HNumber> & vec3) = &hSub;
void (*fptr_hSub_HComplexHComplexHInteger123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HComplex> & vec2 , std::vector<HInteger> & vec3) = &hSub;
void (*fptr_hSub_HComplexHNumberHComplex123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HNumber> & vec2 , std::vector<HComplex> & vec3) = &hSub;
void (*fptr_hSub_HComplexHNumberHNumber123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HNumber> & vec2 , std::vector<HNumber> & vec3) = &hSub;
void (*fptr_hSub_HComplexHNumberHInteger123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HNumber> & vec2 , std::vector<HInteger> & vec3) = &hSub;
void (*fptr_hSub_HComplexHIntegerHComplex123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HInteger> & vec2 , std::vector<HComplex> & vec3) = &hSub;
void (*fptr_hSub_HComplexHIntegerHNumber123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HInteger> & vec2 , std::vector<HNumber> & vec3) = &hSub;
void (*fptr_hSub_HComplexHIntegerHInteger123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HInteger> & vec2 , std::vector<HInteger> & vec3) = &hSub;
void (*fptr_hSub_HNumberHComplexHComplex123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HComplex> & vec2 , std::vector<HComplex> & vec3) = &hSub;
void (*fptr_hSub_HNumberHComplexHNumber123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HComplex> & vec2 , std::vector<HNumber> & vec3) = &hSub;
void (*fptr_hSub_HNumberHComplexHInteger123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HComplex> & vec2 , std::vector<HInteger> & vec3) = &hSub;
void (*fptr_hSub_HNumberHNumberHComplex123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HNumber> & vec2 , std::vector<HComplex> & vec3) = &hSub;
void (*fptr_hSub_HNumberHNumberHNumber123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HNumber> & vec2 , std::vector<HNumber> & vec3) = &hSub;
void (*fptr_hSub_HNumberHNumberHInteger123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HNumber> & vec2 , std::vector<HInteger> & vec3) = &hSub;
void (*fptr_hSub_HNumberHIntegerHComplex123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HInteger> & vec2 , std::vector<HComplex> & vec3) = &hSub;
void (*fptr_hSub_HNumberHIntegerHNumber123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HInteger> & vec2 , std::vector<HNumber> & vec3) = &hSub;
void (*fptr_hSub_HNumberHIntegerHInteger123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HInteger> & vec2 , std::vector<HInteger> & vec3) = &hSub;
void (*fptr_hSub_HIntegerHComplexHComplex123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HComplex> & vec2 , std::vector<HComplex> & vec3) = &hSub;
void (*fptr_hSub_HIntegerHComplexHNumber123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HComplex> & vec2 , std::vector<HNumber> & vec3) = &hSub;
void (*fptr_hSub_HIntegerHComplexHInteger123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HComplex> & vec2 , std::vector<HInteger> & vec3) = &hSub;
void (*fptr_hSub_HIntegerHNumberHComplex123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HNumber> & vec2 , std::vector<HComplex> & vec3) = &hSub;
void (*fptr_hSub_HIntegerHNumberHNumber123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HNumber> & vec2 , std::vector<HNumber> & vec3) = &hSub;
void (*fptr_hSub_HIntegerHNumberHInteger123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HNumber> & vec2 , std::vector<HInteger> & vec3) = &hSub;
void (*fptr_hSub_HIntegerHIntegerHComplex123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HInteger> & vec2 , std::vector<HComplex> & vec3) = &hSub;
void (*fptr_hSub_HIntegerHIntegerHNumber123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HInteger> & vec2 , std::vector<HNumber> & vec3) = &hSub;
void (*fptr_hSub_HIntegerHIntegerHInteger123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HInteger> & vec2 , std::vector<HInteger> & vec3) = &hSub;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//-----------------------------------------------------------------------
/*!
  \brief Performs a Sub between the two vectors, and adds the result to the output (third) vector.

    \param vec1: Numeric input vector

    \param vec2: Vector containing the second operands

    \param vec3: Vector containing the results

*/
template <class Iter>
void hSubAdd(const Iter vec1,const Iter vec1_end, const Iter vec2,const Iter vec2_end, const Iter vec3,const Iter vec3_end)
{
  typedef typename Iter::value_type T;
  Iter it1(vec1), it2(vec2), itout(vec3);
  while ((it1!=vec1_end) && (itout !=vec3_end)) {
    *itout += (*it1) - (*it2);
    ++it1; ++it2; ++itout;
    if (it2==vec2_end) it2=vec2;
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T > inline void hSubAdd( std::vector<T> & vec1 , std::vector<T> & vec2 , std::vector<T> & vec3) {
hSubAdd ( vec1.begin(),vec1.end() , vec2.begin(),vec2.end() , vec3.begin(),vec3.end());
}
template < class T > inline void hSubAdd( casa::Vector<T> & vec1 , casa::Vector<T> & vec2 , casa::Vector<T> & vec3) {
hSubAdd ( vec1.cbegin(),vec1.cend() , vec2.cbegin(),vec2.cend() , vec3.cbegin(),vec3.cend());
}

void (*fptr_hSubAdd_HComplex111STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HComplex> & vec2 , std::vector<HComplex> & vec3) = &hSubAdd;
void (*fptr_hSubAdd_HNumber111STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HNumber> & vec2 , std::vector<HNumber> & vec3) = &hSubAdd;
void (*fptr_hSubAdd_HInteger111STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HInteger> & vec2 , std::vector<HInteger> & vec3) = &hSubAdd;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//-----------------------------------------------------------------------
/*!
  \brief Performs a Sub between the two vectors, and adds the result to the output (third) vector - automatic casting is done.

    \param vec1: Numeric input vector

    \param vec2: Vector containing the second operands

    \param vec3: Vector containing the results

*/
template <class Iterin1, class Iterin2, class Iter>
void hSubAddConv(const Iterin1 vec1,const Iterin1 vec1_end, const Iterin2 vec2,const Iterin2 vec2_end, const Iter vec3,const Iter vec3_end)
{
  typedef typename Iter::value_type T;
  Iterin1 it1=vec1;
  Iterin2 it2=vec2;
  Iter itout=vec3;
  while ((it1!=vec1_end) && (itout !=vec3_end)) {
    *itout += hfcast<T>((*it1) - (*it2));
    ++it1; ++it2; ++itout;
    if (it2==vec2_end) it2=vec2;
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T , class S , class U > inline void hSubAddConv( std::vector<T> & vec1 , std::vector<S> & vec2 , std::vector<U> & vec3) {
hSubAddConv ( vec1.begin(),vec1.end() , vec2.begin(),vec2.end() , vec3.begin(),vec3.end());
}
template < class T , class S , class U > inline void hSubAddConv( casa::Vector<T> & vec1 , casa::Vector<S> & vec2 , casa::Vector<U> & vec3) {
hSubAddConv ( vec1.cbegin(),vec1.cend() , vec2.cbegin(),vec2.cend() , vec3.cbegin(),vec3.cend());
}

void (*fptr_hSubAddConv_HComplexHComplexHComplex123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HComplex> & vec2 , std::vector<HComplex> & vec3) = &hSubAddConv;
void (*fptr_hSubAddConv_HComplexHComplexHNumber123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HComplex> & vec2 , std::vector<HNumber> & vec3) = &hSubAddConv;
void (*fptr_hSubAddConv_HComplexHComplexHInteger123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HComplex> & vec2 , std::vector<HInteger> & vec3) = &hSubAddConv;
void (*fptr_hSubAddConv_HComplexHNumberHComplex123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HNumber> & vec2 , std::vector<HComplex> & vec3) = &hSubAddConv;
void (*fptr_hSubAddConv_HComplexHNumberHNumber123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HNumber> & vec2 , std::vector<HNumber> & vec3) = &hSubAddConv;
void (*fptr_hSubAddConv_HComplexHNumberHInteger123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HNumber> & vec2 , std::vector<HInteger> & vec3) = &hSubAddConv;
void (*fptr_hSubAddConv_HComplexHIntegerHComplex123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HInteger> & vec2 , std::vector<HComplex> & vec3) = &hSubAddConv;
void (*fptr_hSubAddConv_HComplexHIntegerHNumber123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HInteger> & vec2 , std::vector<HNumber> & vec3) = &hSubAddConv;
void (*fptr_hSubAddConv_HComplexHIntegerHInteger123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HInteger> & vec2 , std::vector<HInteger> & vec3) = &hSubAddConv;
void (*fptr_hSubAddConv_HNumberHComplexHComplex123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HComplex> & vec2 , std::vector<HComplex> & vec3) = &hSubAddConv;
void (*fptr_hSubAddConv_HNumberHComplexHNumber123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HComplex> & vec2 , std::vector<HNumber> & vec3) = &hSubAddConv;
void (*fptr_hSubAddConv_HNumberHComplexHInteger123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HComplex> & vec2 , std::vector<HInteger> & vec3) = &hSubAddConv;
void (*fptr_hSubAddConv_HNumberHNumberHComplex123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HNumber> & vec2 , std::vector<HComplex> & vec3) = &hSubAddConv;
void (*fptr_hSubAddConv_HNumberHNumberHNumber123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HNumber> & vec2 , std::vector<HNumber> & vec3) = &hSubAddConv;
void (*fptr_hSubAddConv_HNumberHNumberHInteger123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HNumber> & vec2 , std::vector<HInteger> & vec3) = &hSubAddConv;
void (*fptr_hSubAddConv_HNumberHIntegerHComplex123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HInteger> & vec2 , std::vector<HComplex> & vec3) = &hSubAddConv;
void (*fptr_hSubAddConv_HNumberHIntegerHNumber123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HInteger> & vec2 , std::vector<HNumber> & vec3) = &hSubAddConv;
void (*fptr_hSubAddConv_HNumberHIntegerHInteger123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HInteger> & vec2 , std::vector<HInteger> & vec3) = &hSubAddConv;
void (*fptr_hSubAddConv_HIntegerHComplexHComplex123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HComplex> & vec2 , std::vector<HComplex> & vec3) = &hSubAddConv;
void (*fptr_hSubAddConv_HIntegerHComplexHNumber123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HComplex> & vec2 , std::vector<HNumber> & vec3) = &hSubAddConv;
void (*fptr_hSubAddConv_HIntegerHComplexHInteger123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HComplex> & vec2 , std::vector<HInteger> & vec3) = &hSubAddConv;
void (*fptr_hSubAddConv_HIntegerHNumberHComplex123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HNumber> & vec2 , std::vector<HComplex> & vec3) = &hSubAddConv;
void (*fptr_hSubAddConv_HIntegerHNumberHNumber123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HNumber> & vec2 , std::vector<HNumber> & vec3) = &hSubAddConv;
void (*fptr_hSubAddConv_HIntegerHNumberHInteger123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HNumber> & vec2 , std::vector<HInteger> & vec3) = &hSubAddConv;
void (*fptr_hSubAddConv_HIntegerHIntegerHComplex123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HInteger> & vec2 , std::vector<HComplex> & vec3) = &hSubAddConv;
void (*fptr_hSubAddConv_HIntegerHIntegerHNumber123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HInteger> & vec2 , std::vector<HNumber> & vec3) = &hSubAddConv;
void (*fptr_hSubAddConv_HIntegerHIntegerHInteger123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HInteger> & vec2 , std::vector<HInteger> & vec3) = &hSubAddConv;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//-----------------------------------------------------------------------
/*!

  \brief Performs a Sub between the vector and a scalar, where the result is returned in the second vector.

  \param vec1: STL Iterator pointing to the first element of an array with
         input  values.
  \param vec1_end: STL Iterator pointing to the end of the input vector

  \param val parameter containing the second operand

  \param vec2: STL Iterator pointing to the first element of an array with
         output values.
  \param vec2_end: STL Iterator pointing to the end of the output vector
*/
template <class Iterin1, class S, class Iter>
void hSub2(const Iterin1 vec1,const Iterin1 vec1_end, S val, const Iter vec2,const Iter vec2_end)
{
  typedef typename Iter::value_type T;
  Iterin1 itin=vec1;
  Iter itout=vec2;
  T val_t=hfcast<T>(val);
  while ((itin!=vec1_end) && (itout !=vec2_end)) {
    *itout = hfcast<T>(*itin) - val_t;
    ++itin; ++itout;
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T , class S , class U > inline void hSub( std::vector<T> & vec1 , S val , std::vector<U> & vec2) {
hSub2 ( vec1.begin(),vec1.end() , val , vec2.begin(),vec2.end());
}
template < class T , class S , class U > inline void hSub( casa::Vector<T> & vec1 , S val , casa::Vector<U> & vec2) {
hSub2 ( vec1.cbegin(),vec1.cend() , val , vec2.cbegin(),vec2.cend());
}

void (*fptr_hSub2_HComplexHComplexHComplex123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , HComplex val , std::vector<HComplex> & vec2) = &hSub;
void (*fptr_hSub2_HComplexHComplexHNumber123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , HComplex val , std::vector<HNumber> & vec2) = &hSub;
void (*fptr_hSub2_HComplexHComplexHInteger123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , HComplex val , std::vector<HInteger> & vec2) = &hSub;
void (*fptr_hSub2_HComplexHNumberHComplex123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , HNumber val , std::vector<HComplex> & vec2) = &hSub;
void (*fptr_hSub2_HComplexHNumberHNumber123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , HNumber val , std::vector<HNumber> & vec2) = &hSub;
void (*fptr_hSub2_HComplexHNumberHInteger123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , HNumber val , std::vector<HInteger> & vec2) = &hSub;
void (*fptr_hSub2_HComplexHIntegerHComplex123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , HInteger val , std::vector<HComplex> & vec2) = &hSub;
void (*fptr_hSub2_HComplexHIntegerHNumber123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , HInteger val , std::vector<HNumber> & vec2) = &hSub;
void (*fptr_hSub2_HComplexHIntegerHInteger123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , HInteger val , std::vector<HInteger> & vec2) = &hSub;
void (*fptr_hSub2_HNumberHComplexHComplex123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , HComplex val , std::vector<HComplex> & vec2) = &hSub;
void (*fptr_hSub2_HNumberHComplexHNumber123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , HComplex val , std::vector<HNumber> & vec2) = &hSub;
void (*fptr_hSub2_HNumberHComplexHInteger123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , HComplex val , std::vector<HInteger> & vec2) = &hSub;
void (*fptr_hSub2_HNumberHNumberHComplex123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , HNumber val , std::vector<HComplex> & vec2) = &hSub;
void (*fptr_hSub2_HNumberHNumberHNumber123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , HNumber val , std::vector<HNumber> & vec2) = &hSub;
void (*fptr_hSub2_HNumberHNumberHInteger123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , HNumber val , std::vector<HInteger> & vec2) = &hSub;
void (*fptr_hSub2_HNumberHIntegerHComplex123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , HInteger val , std::vector<HComplex> & vec2) = &hSub;
void (*fptr_hSub2_HNumberHIntegerHNumber123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , HInteger val , std::vector<HNumber> & vec2) = &hSub;
void (*fptr_hSub2_HNumberHIntegerHInteger123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , HInteger val , std::vector<HInteger> & vec2) = &hSub;
void (*fptr_hSub2_HIntegerHComplexHComplex123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , HComplex val , std::vector<HComplex> & vec2) = &hSub;
void (*fptr_hSub2_HIntegerHComplexHNumber123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , HComplex val , std::vector<HNumber> & vec2) = &hSub;
void (*fptr_hSub2_HIntegerHComplexHInteger123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , HComplex val , std::vector<HInteger> & vec2) = &hSub;
void (*fptr_hSub2_HIntegerHNumberHComplex123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , HNumber val , std::vector<HComplex> & vec2) = &hSub;
void (*fptr_hSub2_HIntegerHNumberHNumber123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , HNumber val , std::vector<HNumber> & vec2) = &hSub;
void (*fptr_hSub2_HIntegerHNumberHInteger123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , HNumber val , std::vector<HInteger> & vec2) = &hSub;
void (*fptr_hSub2_HIntegerHIntegerHComplex123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , HInteger val , std::vector<HComplex> & vec2) = &hSub;
void (*fptr_hSub2_HIntegerHIntegerHNumber123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , HInteger val , std::vector<HNumber> & vec2) = &hSub;
void (*fptr_hSub2_HIntegerHIntegerHInteger123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , HInteger val , std::vector<HInteger> & vec2) = &hSub;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//========================================================================
//-----------------------------------------------------------------------
/*!

  \brief Performs a Mul between the two vectors, which is returned in the first vector. If the second vector is shorter it will be applied multiple times.

  \param vec1: STL Iterator pointing to the first element of an array with
         input and output values.
  \param vec1_end: STL Iterator pointing to the end of the input vector

  \param vec2: STL Iterator pointing to the first element of an array with
         input values.
  \param vec2_end: STL Iterator pointing to the end of the input vector
*/
template <class Iter, class Iterin>
void hiMul(const Iter vec1,const Iter vec1_end, const Iterin vec2,const Iterin vec2_end)
{
  typedef typename Iter::value_type T;
  Iter it1=vec1;
  Iterin it2=vec2;
  while (it1!=vec1_end) {
    *it1 *= hfcast<T>(*it2);
    ++it1; ++it2;
    if (it2==vec2_end) it2=vec2;
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T , class S > inline void hiMul( std::vector<T> & vec1 , std::vector<S> & vec2) {
hiMul ( vec1.begin(),vec1.end() , vec2.begin(),vec2.end());
}
template < class T , class S > inline void hiMul( casa::Vector<T> & vec1 , casa::Vector<S> & vec2) {
hiMul ( vec1.cbegin(),vec1.cend() , vec2.cbegin(),vec2.cend());
}

void (*fptr_hiMul_HComplexHComplex12STDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HComplex> & vec2) = &hiMul;
void (*fptr_hiMul_HComplexHNumber12STDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HNumber> & vec2) = &hiMul;
void (*fptr_hiMul_HComplexHInteger12STDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HInteger> & vec2) = &hiMul;
void (*fptr_hiMul_HNumberHComplex12STDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HComplex> & vec2) = &hiMul;
void (*fptr_hiMul_HNumberHNumber12STDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HNumber> & vec2) = &hiMul;
void (*fptr_hiMul_HNumberHInteger12STDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HInteger> & vec2) = &hiMul;
void (*fptr_hiMul_HIntegerHComplex12STDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HComplex> & vec2) = &hiMul;
void (*fptr_hiMul_HIntegerHNumber12STDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HNumber> & vec2) = &hiMul;
void (*fptr_hiMul_HIntegerHInteger12STDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HInteger> & vec2) = &hiMul;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//-----------------------------------------------------------------------
/*!

  \brief Performs a Mul between the vector and a scalar (applied to each element), which is returned in the first vector.

  \param vec1: STL Iterator pointing to the first element of an array with
         input and output values.
  \param vec1_end: STL Iterator pointing to the end of the input vector

  \param val: Value containing the second operand
*/
template <class Iter, class S>
void hiMul2(const Iter vec1,const Iter vec1_end, S val)
{
  typedef typename Iter::value_type T;
  Iter it=vec1;
  T val_t = hfcast<T>(val);
  while (it!=vec1_end) {
    *it *= val_t;
    ++it;
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T , class S > inline void hiMul( std::vector<T> & vec1 , S val) {
hiMul2 ( vec1.begin(),vec1.end() , val);
}
template < class T , class S > inline void hiMul( casa::Vector<T> & vec1 , S val) {
hiMul2 ( vec1.cbegin(),vec1.cend() , val);
}

void (*fptr_hiMul2_HComplexHComplex12STDIT)( std::vector<HComplex> & vec1 , HComplex val) = &hiMul;
void (*fptr_hiMul2_HComplexHNumber12STDIT)( std::vector<HComplex> & vec1 , HNumber val) = &hiMul;
void (*fptr_hiMul2_HComplexHInteger12STDIT)( std::vector<HComplex> & vec1 , HInteger val) = &hiMul;
void (*fptr_hiMul2_HNumberHComplex12STDIT)( std::vector<HNumber> & vec1 , HComplex val) = &hiMul;
void (*fptr_hiMul2_HNumberHNumber12STDIT)( std::vector<HNumber> & vec1 , HNumber val) = &hiMul;
void (*fptr_hiMul2_HNumberHInteger12STDIT)( std::vector<HNumber> & vec1 , HInteger val) = &hiMul;
void (*fptr_hiMul2_HIntegerHComplex12STDIT)( std::vector<HInteger> & vec1 , HComplex val) = &hiMul;
void (*fptr_hiMul2_HIntegerHNumber12STDIT)( std::vector<HInteger> & vec1 , HNumber val) = &hiMul;
void (*fptr_hiMul2_HIntegerHInteger12STDIT)( std::vector<HInteger> & vec1 , HInteger val) = &hiMul;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//-----------------------------------------------------------------------
/*!

  \brief Performs a Mul between the two vectors, which is returned in the third vector.

  \param vec1: STL Iterator pointing to the first element of an array with
         input  values.
  \param vec1_end: STL Iterator pointing to the end of the input vector

  \param vec2: STL Iterator pointing to the first element of an array with
         input values.
  \param vec2_end: STL Iterator pointing to the end of the input vector

  \param vec3: STL Iterator pointing to the first element of an array with
         output values.
  \param vec3_end: STL Iterator pointing to the end of the output vector
*/
template <class Iterin1, class Iterin2, class Iter>
void hMul(const Iterin1 vec1,const Iterin1 vec1_end, const Iterin2 vec2,const Iterin2 vec2_end, const Iter vec3,const Iter vec3_end)
{
  typedef typename Iter::value_type T;
  Iterin1 it1=vec1;
  Iterin2 it2=vec2;
  Iter itout=vec3;
  while ((it1!=vec1_end) && (itout !=vec3_end)) {
    *itout = hfcast<T>((*it1) * (*it2));
    ++it1; ++it2; ++itout;
    if (it2==vec2_end) it2=vec2;
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T , class S , class U > inline void hMul( std::vector<T> & vec1 , std::vector<S> & vec2 , std::vector<U> & vec3) {
hMul ( vec1.begin(),vec1.end() , vec2.begin(),vec2.end() , vec3.begin(),vec3.end());
}
template < class T , class S , class U > inline void hMul( casa::Vector<T> & vec1 , casa::Vector<S> & vec2 , casa::Vector<U> & vec3) {
hMul ( vec1.cbegin(),vec1.cend() , vec2.cbegin(),vec2.cend() , vec3.cbegin(),vec3.cend());
}

void (*fptr_hMul_HComplexHComplexHComplex123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HComplex> & vec2 , std::vector<HComplex> & vec3) = &hMul;
void (*fptr_hMul_HComplexHComplexHNumber123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HComplex> & vec2 , std::vector<HNumber> & vec3) = &hMul;
void (*fptr_hMul_HComplexHComplexHInteger123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HComplex> & vec2 , std::vector<HInteger> & vec3) = &hMul;
void (*fptr_hMul_HComplexHNumberHComplex123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HNumber> & vec2 , std::vector<HComplex> & vec3) = &hMul;
void (*fptr_hMul_HComplexHNumberHNumber123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HNumber> & vec2 , std::vector<HNumber> & vec3) = &hMul;
void (*fptr_hMul_HComplexHNumberHInteger123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HNumber> & vec2 , std::vector<HInteger> & vec3) = &hMul;
void (*fptr_hMul_HComplexHIntegerHComplex123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HInteger> & vec2 , std::vector<HComplex> & vec3) = &hMul;
void (*fptr_hMul_HComplexHIntegerHNumber123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HInteger> & vec2 , std::vector<HNumber> & vec3) = &hMul;
void (*fptr_hMul_HComplexHIntegerHInteger123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HInteger> & vec2 , std::vector<HInteger> & vec3) = &hMul;
void (*fptr_hMul_HNumberHComplexHComplex123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HComplex> & vec2 , std::vector<HComplex> & vec3) = &hMul;
void (*fptr_hMul_HNumberHComplexHNumber123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HComplex> & vec2 , std::vector<HNumber> & vec3) = &hMul;
void (*fptr_hMul_HNumberHComplexHInteger123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HComplex> & vec2 , std::vector<HInteger> & vec3) = &hMul;
void (*fptr_hMul_HNumberHNumberHComplex123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HNumber> & vec2 , std::vector<HComplex> & vec3) = &hMul;
void (*fptr_hMul_HNumberHNumberHNumber123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HNumber> & vec2 , std::vector<HNumber> & vec3) = &hMul;
void (*fptr_hMul_HNumberHNumberHInteger123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HNumber> & vec2 , std::vector<HInteger> & vec3) = &hMul;
void (*fptr_hMul_HNumberHIntegerHComplex123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HInteger> & vec2 , std::vector<HComplex> & vec3) = &hMul;
void (*fptr_hMul_HNumberHIntegerHNumber123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HInteger> & vec2 , std::vector<HNumber> & vec3) = &hMul;
void (*fptr_hMul_HNumberHIntegerHInteger123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HInteger> & vec2 , std::vector<HInteger> & vec3) = &hMul;
void (*fptr_hMul_HIntegerHComplexHComplex123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HComplex> & vec2 , std::vector<HComplex> & vec3) = &hMul;
void (*fptr_hMul_HIntegerHComplexHNumber123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HComplex> & vec2 , std::vector<HNumber> & vec3) = &hMul;
void (*fptr_hMul_HIntegerHComplexHInteger123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HComplex> & vec2 , std::vector<HInteger> & vec3) = &hMul;
void (*fptr_hMul_HIntegerHNumberHComplex123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HNumber> & vec2 , std::vector<HComplex> & vec3) = &hMul;
void (*fptr_hMul_HIntegerHNumberHNumber123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HNumber> & vec2 , std::vector<HNumber> & vec3) = &hMul;
void (*fptr_hMul_HIntegerHNumberHInteger123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HNumber> & vec2 , std::vector<HInteger> & vec3) = &hMul;
void (*fptr_hMul_HIntegerHIntegerHComplex123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HInteger> & vec2 , std::vector<HComplex> & vec3) = &hMul;
void (*fptr_hMul_HIntegerHIntegerHNumber123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HInteger> & vec2 , std::vector<HNumber> & vec3) = &hMul;
void (*fptr_hMul_HIntegerHIntegerHInteger123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HInteger> & vec2 , std::vector<HInteger> & vec3) = &hMul;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//-----------------------------------------------------------------------
/*!
  \brief Performs a Mul between the two vectors, and adds the result to the output (third) vector.

    \param vec1: Numeric input vector

    \param vec2: Vector containing the second operands

    \param vec3: Vector containing the results

*/
template <class Iter>
void hMulAdd(const Iter vec1,const Iter vec1_end, const Iter vec2,const Iter vec2_end, const Iter vec3,const Iter vec3_end)
{
  typedef typename Iter::value_type T;
  Iter it1(vec1), it2(vec2), itout(vec3);
  while ((it1!=vec1_end) && (itout !=vec3_end)) {
    *itout += (*it1) * (*it2);
    ++it1; ++it2; ++itout;
    if (it2==vec2_end) it2=vec2;
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T > inline void hMulAdd( std::vector<T> & vec1 , std::vector<T> & vec2 , std::vector<T> & vec3) {
hMulAdd ( vec1.begin(),vec1.end() , vec2.begin(),vec2.end() , vec3.begin(),vec3.end());
}
template < class T > inline void hMulAdd( casa::Vector<T> & vec1 , casa::Vector<T> & vec2 , casa::Vector<T> & vec3) {
hMulAdd ( vec1.cbegin(),vec1.cend() , vec2.cbegin(),vec2.cend() , vec3.cbegin(),vec3.cend());
}

void (*fptr_hMulAdd_HComplex111STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HComplex> & vec2 , std::vector<HComplex> & vec3) = &hMulAdd;
void (*fptr_hMulAdd_HNumber111STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HNumber> & vec2 , std::vector<HNumber> & vec3) = &hMulAdd;
void (*fptr_hMulAdd_HInteger111STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HInteger> & vec2 , std::vector<HInteger> & vec3) = &hMulAdd;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//-----------------------------------------------------------------------
/*!
  \brief Performs a Mul between the two vectors, and adds the result to the output (third) vector - automatic casting is done.

    \param vec1: Numeric input vector

    \param vec2: Vector containing the second operands

    \param vec3: Vector containing the results

*/
template <class Iterin1, class Iterin2, class Iter>
void hMulAddConv(const Iterin1 vec1,const Iterin1 vec1_end, const Iterin2 vec2,const Iterin2 vec2_end, const Iter vec3,const Iter vec3_end)
{
  typedef typename Iter::value_type T;
  Iterin1 it1=vec1;
  Iterin2 it2=vec2;
  Iter itout=vec3;
  while ((it1!=vec1_end) && (itout !=vec3_end)) {
    *itout += hfcast<T>((*it1) * (*it2));
    ++it1; ++it2; ++itout;
    if (it2==vec2_end) it2=vec2;
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T , class S , class U > inline void hMulAddConv( std::vector<T> & vec1 , std::vector<S> & vec2 , std::vector<U> & vec3) {
hMulAddConv ( vec1.begin(),vec1.end() , vec2.begin(),vec2.end() , vec3.begin(),vec3.end());
}
template < class T , class S , class U > inline void hMulAddConv( casa::Vector<T> & vec1 , casa::Vector<S> & vec2 , casa::Vector<U> & vec3) {
hMulAddConv ( vec1.cbegin(),vec1.cend() , vec2.cbegin(),vec2.cend() , vec3.cbegin(),vec3.cend());
}

void (*fptr_hMulAddConv_HComplexHComplexHComplex123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HComplex> & vec2 , std::vector<HComplex> & vec3) = &hMulAddConv;
void (*fptr_hMulAddConv_HComplexHComplexHNumber123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HComplex> & vec2 , std::vector<HNumber> & vec3) = &hMulAddConv;
void (*fptr_hMulAddConv_HComplexHComplexHInteger123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HComplex> & vec2 , std::vector<HInteger> & vec3) = &hMulAddConv;
void (*fptr_hMulAddConv_HComplexHNumberHComplex123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HNumber> & vec2 , std::vector<HComplex> & vec3) = &hMulAddConv;
void (*fptr_hMulAddConv_HComplexHNumberHNumber123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HNumber> & vec2 , std::vector<HNumber> & vec3) = &hMulAddConv;
void (*fptr_hMulAddConv_HComplexHNumberHInteger123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HNumber> & vec2 , std::vector<HInteger> & vec3) = &hMulAddConv;
void (*fptr_hMulAddConv_HComplexHIntegerHComplex123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HInteger> & vec2 , std::vector<HComplex> & vec3) = &hMulAddConv;
void (*fptr_hMulAddConv_HComplexHIntegerHNumber123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HInteger> & vec2 , std::vector<HNumber> & vec3) = &hMulAddConv;
void (*fptr_hMulAddConv_HComplexHIntegerHInteger123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HInteger> & vec2 , std::vector<HInteger> & vec3) = &hMulAddConv;
void (*fptr_hMulAddConv_HNumberHComplexHComplex123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HComplex> & vec2 , std::vector<HComplex> & vec3) = &hMulAddConv;
void (*fptr_hMulAddConv_HNumberHComplexHNumber123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HComplex> & vec2 , std::vector<HNumber> & vec3) = &hMulAddConv;
void (*fptr_hMulAddConv_HNumberHComplexHInteger123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HComplex> & vec2 , std::vector<HInteger> & vec3) = &hMulAddConv;
void (*fptr_hMulAddConv_HNumberHNumberHComplex123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HNumber> & vec2 , std::vector<HComplex> & vec3) = &hMulAddConv;
void (*fptr_hMulAddConv_HNumberHNumberHNumber123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HNumber> & vec2 , std::vector<HNumber> & vec3) = &hMulAddConv;
void (*fptr_hMulAddConv_HNumberHNumberHInteger123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HNumber> & vec2 , std::vector<HInteger> & vec3) = &hMulAddConv;
void (*fptr_hMulAddConv_HNumberHIntegerHComplex123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HInteger> & vec2 , std::vector<HComplex> & vec3) = &hMulAddConv;
void (*fptr_hMulAddConv_HNumberHIntegerHNumber123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HInteger> & vec2 , std::vector<HNumber> & vec3) = &hMulAddConv;
void (*fptr_hMulAddConv_HNumberHIntegerHInteger123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HInteger> & vec2 , std::vector<HInteger> & vec3) = &hMulAddConv;
void (*fptr_hMulAddConv_HIntegerHComplexHComplex123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HComplex> & vec2 , std::vector<HComplex> & vec3) = &hMulAddConv;
void (*fptr_hMulAddConv_HIntegerHComplexHNumber123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HComplex> & vec2 , std::vector<HNumber> & vec3) = &hMulAddConv;
void (*fptr_hMulAddConv_HIntegerHComplexHInteger123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HComplex> & vec2 , std::vector<HInteger> & vec3) = &hMulAddConv;
void (*fptr_hMulAddConv_HIntegerHNumberHComplex123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HNumber> & vec2 , std::vector<HComplex> & vec3) = &hMulAddConv;
void (*fptr_hMulAddConv_HIntegerHNumberHNumber123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HNumber> & vec2 , std::vector<HNumber> & vec3) = &hMulAddConv;
void (*fptr_hMulAddConv_HIntegerHNumberHInteger123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HNumber> & vec2 , std::vector<HInteger> & vec3) = &hMulAddConv;
void (*fptr_hMulAddConv_HIntegerHIntegerHComplex123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HInteger> & vec2 , std::vector<HComplex> & vec3) = &hMulAddConv;
void (*fptr_hMulAddConv_HIntegerHIntegerHNumber123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HInteger> & vec2 , std::vector<HNumber> & vec3) = &hMulAddConv;
void (*fptr_hMulAddConv_HIntegerHIntegerHInteger123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HInteger> & vec2 , std::vector<HInteger> & vec3) = &hMulAddConv;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//-----------------------------------------------------------------------
/*!

  \brief Performs a Mul between the vector and a scalar, where the result is returned in the second vector.

  \param vec1: STL Iterator pointing to the first element of an array with
         input  values.
  \param vec1_end: STL Iterator pointing to the end of the input vector

  \param val parameter containing the second operand

  \param vec2: STL Iterator pointing to the first element of an array with
         output values.
  \param vec2_end: STL Iterator pointing to the end of the output vector
*/
template <class Iterin1, class S, class Iter>
void hMul2(const Iterin1 vec1,const Iterin1 vec1_end, S val, const Iter vec2,const Iter vec2_end)
{
  typedef typename Iter::value_type T;
  Iterin1 itin=vec1;
  Iter itout=vec2;
  T val_t=hfcast<T>(val);
  while ((itin!=vec1_end) && (itout !=vec2_end)) {
    *itout = hfcast<T>(*itin) * val_t;
    ++itin; ++itout;
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T , class S , class U > inline void hMul( std::vector<T> & vec1 , S val , std::vector<U> & vec2) {
hMul2 ( vec1.begin(),vec1.end() , val , vec2.begin(),vec2.end());
}
template < class T , class S , class U > inline void hMul( casa::Vector<T> & vec1 , S val , casa::Vector<U> & vec2) {
hMul2 ( vec1.cbegin(),vec1.cend() , val , vec2.cbegin(),vec2.cend());
}

void (*fptr_hMul2_HComplexHComplexHComplex123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , HComplex val , std::vector<HComplex> & vec2) = &hMul;
void (*fptr_hMul2_HComplexHComplexHNumber123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , HComplex val , std::vector<HNumber> & vec2) = &hMul;
void (*fptr_hMul2_HComplexHComplexHInteger123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , HComplex val , std::vector<HInteger> & vec2) = &hMul;
void (*fptr_hMul2_HComplexHNumberHComplex123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , HNumber val , std::vector<HComplex> & vec2) = &hMul;
void (*fptr_hMul2_HComplexHNumberHNumber123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , HNumber val , std::vector<HNumber> & vec2) = &hMul;
void (*fptr_hMul2_HComplexHNumberHInteger123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , HNumber val , std::vector<HInteger> & vec2) = &hMul;
void (*fptr_hMul2_HComplexHIntegerHComplex123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , HInteger val , std::vector<HComplex> & vec2) = &hMul;
void (*fptr_hMul2_HComplexHIntegerHNumber123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , HInteger val , std::vector<HNumber> & vec2) = &hMul;
void (*fptr_hMul2_HComplexHIntegerHInteger123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , HInteger val , std::vector<HInteger> & vec2) = &hMul;
void (*fptr_hMul2_HNumberHComplexHComplex123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , HComplex val , std::vector<HComplex> & vec2) = &hMul;
void (*fptr_hMul2_HNumberHComplexHNumber123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , HComplex val , std::vector<HNumber> & vec2) = &hMul;
void (*fptr_hMul2_HNumberHComplexHInteger123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , HComplex val , std::vector<HInteger> & vec2) = &hMul;
void (*fptr_hMul2_HNumberHNumberHComplex123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , HNumber val , std::vector<HComplex> & vec2) = &hMul;
void (*fptr_hMul2_HNumberHNumberHNumber123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , HNumber val , std::vector<HNumber> & vec2) = &hMul;
void (*fptr_hMul2_HNumberHNumberHInteger123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , HNumber val , std::vector<HInteger> & vec2) = &hMul;
void (*fptr_hMul2_HNumberHIntegerHComplex123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , HInteger val , std::vector<HComplex> & vec2) = &hMul;
void (*fptr_hMul2_HNumberHIntegerHNumber123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , HInteger val , std::vector<HNumber> & vec2) = &hMul;
void (*fptr_hMul2_HNumberHIntegerHInteger123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , HInteger val , std::vector<HInteger> & vec2) = &hMul;
void (*fptr_hMul2_HIntegerHComplexHComplex123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , HComplex val , std::vector<HComplex> & vec2) = &hMul;
void (*fptr_hMul2_HIntegerHComplexHNumber123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , HComplex val , std::vector<HNumber> & vec2) = &hMul;
void (*fptr_hMul2_HIntegerHComplexHInteger123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , HComplex val , std::vector<HInteger> & vec2) = &hMul;
void (*fptr_hMul2_HIntegerHNumberHComplex123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , HNumber val , std::vector<HComplex> & vec2) = &hMul;
void (*fptr_hMul2_HIntegerHNumberHNumber123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , HNumber val , std::vector<HNumber> & vec2) = &hMul;
void (*fptr_hMul2_HIntegerHNumberHInteger123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , HNumber val , std::vector<HInteger> & vec2) = &hMul;
void (*fptr_hMul2_HIntegerHIntegerHComplex123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , HInteger val , std::vector<HComplex> & vec2) = &hMul;
void (*fptr_hMul2_HIntegerHIntegerHNumber123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , HInteger val , std::vector<HNumber> & vec2) = &hMul;
void (*fptr_hMul2_HIntegerHIntegerHInteger123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , HInteger val , std::vector<HInteger> & vec2) = &hMul;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//========================================================================
//-----------------------------------------------------------------------
/*!

  \brief Performs a Add between the two vectors, which is returned in the first vector. If the second vector is shorter it will be applied multiple times.

  \param vec1: STL Iterator pointing to the first element of an array with
         input and output values.
  \param vec1_end: STL Iterator pointing to the end of the input vector

  \param vec2: STL Iterator pointing to the first element of an array with
         input values.
  \param vec2_end: STL Iterator pointing to the end of the input vector
*/
template <class Iter, class Iterin>
void hiAdd(const Iter vec1,const Iter vec1_end, const Iterin vec2,const Iterin vec2_end)
{
  typedef typename Iter::value_type T;
  Iter it1=vec1;
  Iterin it2=vec2;
  while (it1!=vec1_end) {
    *it1 += hfcast<T>(*it2);
    ++it1; ++it2;
    if (it2==vec2_end) it2=vec2;
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T , class S > inline void hiAdd( std::vector<T> & vec1 , std::vector<S> & vec2) {
hiAdd ( vec1.begin(),vec1.end() , vec2.begin(),vec2.end());
}
template < class T , class S > inline void hiAdd( casa::Vector<T> & vec1 , casa::Vector<S> & vec2) {
hiAdd ( vec1.cbegin(),vec1.cend() , vec2.cbegin(),vec2.cend());
}

void (*fptr_hiAdd_HComplexHComplex12STDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HComplex> & vec2) = &hiAdd;
void (*fptr_hiAdd_HComplexHNumber12STDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HNumber> & vec2) = &hiAdd;
void (*fptr_hiAdd_HComplexHInteger12STDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HInteger> & vec2) = &hiAdd;
void (*fptr_hiAdd_HNumberHComplex12STDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HComplex> & vec2) = &hiAdd;
void (*fptr_hiAdd_HNumberHNumber12STDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HNumber> & vec2) = &hiAdd;
void (*fptr_hiAdd_HNumberHInteger12STDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HInteger> & vec2) = &hiAdd;
void (*fptr_hiAdd_HIntegerHComplex12STDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HComplex> & vec2) = &hiAdd;
void (*fptr_hiAdd_HIntegerHNumber12STDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HNumber> & vec2) = &hiAdd;
void (*fptr_hiAdd_HIntegerHInteger12STDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HInteger> & vec2) = &hiAdd;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//-----------------------------------------------------------------------
/*!

  \brief Performs a Add between the vector and a scalar (applied to each element), which is returned in the first vector.

  \param vec1: STL Iterator pointing to the first element of an array with
         input and output values.
  \param vec1_end: STL Iterator pointing to the end of the input vector

  \param val: Value containing the second operand
*/
template <class Iter, class S>
void hiAdd2(const Iter vec1,const Iter vec1_end, S val)
{
  typedef typename Iter::value_type T;
  Iter it=vec1;
  T val_t = hfcast<T>(val);
  while (it!=vec1_end) {
    *it += val_t;
    ++it;
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T , class S > inline void hiAdd( std::vector<T> & vec1 , S val) {
hiAdd2 ( vec1.begin(),vec1.end() , val);
}
template < class T , class S > inline void hiAdd( casa::Vector<T> & vec1 , S val) {
hiAdd2 ( vec1.cbegin(),vec1.cend() , val);
}

void (*fptr_hiAdd2_HComplexHComplex12STDIT)( std::vector<HComplex> & vec1 , HComplex val) = &hiAdd;
void (*fptr_hiAdd2_HComplexHNumber12STDIT)( std::vector<HComplex> & vec1 , HNumber val) = &hiAdd;
void (*fptr_hiAdd2_HComplexHInteger12STDIT)( std::vector<HComplex> & vec1 , HInteger val) = &hiAdd;
void (*fptr_hiAdd2_HNumberHComplex12STDIT)( std::vector<HNumber> & vec1 , HComplex val) = &hiAdd;
void (*fptr_hiAdd2_HNumberHNumber12STDIT)( std::vector<HNumber> & vec1 , HNumber val) = &hiAdd;
void (*fptr_hiAdd2_HNumberHInteger12STDIT)( std::vector<HNumber> & vec1 , HInteger val) = &hiAdd;
void (*fptr_hiAdd2_HIntegerHComplex12STDIT)( std::vector<HInteger> & vec1 , HComplex val) = &hiAdd;
void (*fptr_hiAdd2_HIntegerHNumber12STDIT)( std::vector<HInteger> & vec1 , HNumber val) = &hiAdd;
void (*fptr_hiAdd2_HIntegerHInteger12STDIT)( std::vector<HInteger> & vec1 , HInteger val) = &hiAdd;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//-----------------------------------------------------------------------
/*!

  \brief Performs a Add between the two vectors, which is returned in the third vector.

  \param vec1: STL Iterator pointing to the first element of an array with
         input  values.
  \param vec1_end: STL Iterator pointing to the end of the input vector

  \param vec2: STL Iterator pointing to the first element of an array with
         input values.
  \param vec2_end: STL Iterator pointing to the end of the input vector

  \param vec3: STL Iterator pointing to the first element of an array with
         output values.
  \param vec3_end: STL Iterator pointing to the end of the output vector
*/
template <class Iterin1, class Iterin2, class Iter>
void hAdd(const Iterin1 vec1,const Iterin1 vec1_end, const Iterin2 vec2,const Iterin2 vec2_end, const Iter vec3,const Iter vec3_end)
{
  typedef typename Iter::value_type T;
  Iterin1 it1=vec1;
  Iterin2 it2=vec2;
  Iter itout=vec3;
  while ((it1!=vec1_end) && (itout !=vec3_end)) {
    *itout = hfcast<T>((*it1) + (*it2));
    ++it1; ++it2; ++itout;
    if (it2==vec2_end) it2=vec2;
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T , class S , class U > inline void hAdd( std::vector<T> & vec1 , std::vector<S> & vec2 , std::vector<U> & vec3) {
hAdd ( vec1.begin(),vec1.end() , vec2.begin(),vec2.end() , vec3.begin(),vec3.end());
}
template < class T , class S , class U > inline void hAdd( casa::Vector<T> & vec1 , casa::Vector<S> & vec2 , casa::Vector<U> & vec3) {
hAdd ( vec1.cbegin(),vec1.cend() , vec2.cbegin(),vec2.cend() , vec3.cbegin(),vec3.cend());
}

void (*fptr_hAdd_HComplexHComplexHComplex123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HComplex> & vec2 , std::vector<HComplex> & vec3) = &hAdd;
void (*fptr_hAdd_HComplexHComplexHNumber123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HComplex> & vec2 , std::vector<HNumber> & vec3) = &hAdd;
void (*fptr_hAdd_HComplexHComplexHInteger123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HComplex> & vec2 , std::vector<HInteger> & vec3) = &hAdd;
void (*fptr_hAdd_HComplexHNumberHComplex123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HNumber> & vec2 , std::vector<HComplex> & vec3) = &hAdd;
void (*fptr_hAdd_HComplexHNumberHNumber123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HNumber> & vec2 , std::vector<HNumber> & vec3) = &hAdd;
void (*fptr_hAdd_HComplexHNumberHInteger123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HNumber> & vec2 , std::vector<HInteger> & vec3) = &hAdd;
void (*fptr_hAdd_HComplexHIntegerHComplex123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HInteger> & vec2 , std::vector<HComplex> & vec3) = &hAdd;
void (*fptr_hAdd_HComplexHIntegerHNumber123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HInteger> & vec2 , std::vector<HNumber> & vec3) = &hAdd;
void (*fptr_hAdd_HComplexHIntegerHInteger123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HInteger> & vec2 , std::vector<HInteger> & vec3) = &hAdd;
void (*fptr_hAdd_HNumberHComplexHComplex123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HComplex> & vec2 , std::vector<HComplex> & vec3) = &hAdd;
void (*fptr_hAdd_HNumberHComplexHNumber123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HComplex> & vec2 , std::vector<HNumber> & vec3) = &hAdd;
void (*fptr_hAdd_HNumberHComplexHInteger123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HComplex> & vec2 , std::vector<HInteger> & vec3) = &hAdd;
void (*fptr_hAdd_HNumberHNumberHComplex123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HNumber> & vec2 , std::vector<HComplex> & vec3) = &hAdd;
void (*fptr_hAdd_HNumberHNumberHNumber123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HNumber> & vec2 , std::vector<HNumber> & vec3) = &hAdd;
void (*fptr_hAdd_HNumberHNumberHInteger123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HNumber> & vec2 , std::vector<HInteger> & vec3) = &hAdd;
void (*fptr_hAdd_HNumberHIntegerHComplex123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HInteger> & vec2 , std::vector<HComplex> & vec3) = &hAdd;
void (*fptr_hAdd_HNumberHIntegerHNumber123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HInteger> & vec2 , std::vector<HNumber> & vec3) = &hAdd;
void (*fptr_hAdd_HNumberHIntegerHInteger123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HInteger> & vec2 , std::vector<HInteger> & vec3) = &hAdd;
void (*fptr_hAdd_HIntegerHComplexHComplex123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HComplex> & vec2 , std::vector<HComplex> & vec3) = &hAdd;
void (*fptr_hAdd_HIntegerHComplexHNumber123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HComplex> & vec2 , std::vector<HNumber> & vec3) = &hAdd;
void (*fptr_hAdd_HIntegerHComplexHInteger123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HComplex> & vec2 , std::vector<HInteger> & vec3) = &hAdd;
void (*fptr_hAdd_HIntegerHNumberHComplex123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HNumber> & vec2 , std::vector<HComplex> & vec3) = &hAdd;
void (*fptr_hAdd_HIntegerHNumberHNumber123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HNumber> & vec2 , std::vector<HNumber> & vec3) = &hAdd;
void (*fptr_hAdd_HIntegerHNumberHInteger123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HNumber> & vec2 , std::vector<HInteger> & vec3) = &hAdd;
void (*fptr_hAdd_HIntegerHIntegerHComplex123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HInteger> & vec2 , std::vector<HComplex> & vec3) = &hAdd;
void (*fptr_hAdd_HIntegerHIntegerHNumber123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HInteger> & vec2 , std::vector<HNumber> & vec3) = &hAdd;
void (*fptr_hAdd_HIntegerHIntegerHInteger123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HInteger> & vec2 , std::vector<HInteger> & vec3) = &hAdd;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//-----------------------------------------------------------------------
/*!
  \brief Performs a Add between the two vectors, and adds the result to the output (third) vector.

    \param vec1: Numeric input vector

    \param vec2: Vector containing the second operands

    \param vec3: Vector containing the results

*/
template <class Iter>
void hAddAdd(const Iter vec1,const Iter vec1_end, const Iter vec2,const Iter vec2_end, const Iter vec3,const Iter vec3_end)
{
  typedef typename Iter::value_type T;
  Iter it1(vec1), it2(vec2), itout(vec3);
  while ((it1!=vec1_end) && (itout !=vec3_end)) {
    *itout += (*it1) + (*it2);
    ++it1; ++it2; ++itout;
    if (it2==vec2_end) it2=vec2;
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T > inline void hAddAdd( std::vector<T> & vec1 , std::vector<T> & vec2 , std::vector<T> & vec3) {
hAddAdd ( vec1.begin(),vec1.end() , vec2.begin(),vec2.end() , vec3.begin(),vec3.end());
}
template < class T > inline void hAddAdd( casa::Vector<T> & vec1 , casa::Vector<T> & vec2 , casa::Vector<T> & vec3) {
hAddAdd ( vec1.cbegin(),vec1.cend() , vec2.cbegin(),vec2.cend() , vec3.cbegin(),vec3.cend());
}

void (*fptr_hAddAdd_HComplex111STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HComplex> & vec2 , std::vector<HComplex> & vec3) = &hAddAdd;
void (*fptr_hAddAdd_HNumber111STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HNumber> & vec2 , std::vector<HNumber> & vec3) = &hAddAdd;
void (*fptr_hAddAdd_HInteger111STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HInteger> & vec2 , std::vector<HInteger> & vec3) = &hAddAdd;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//-----------------------------------------------------------------------
/*!
  \brief Performs a Add between the two vectors, and adds the result to the output (third) vector - automatic casting is done.

    \param vec1: Numeric input vector

    \param vec2: Vector containing the second operands

    \param vec3: Vector containing the results

*/
template <class Iterin1, class Iterin2, class Iter>
void hAddAddConv(const Iterin1 vec1,const Iterin1 vec1_end, const Iterin2 vec2,const Iterin2 vec2_end, const Iter vec3,const Iter vec3_end)
{
  typedef typename Iter::value_type T;
  Iterin1 it1=vec1;
  Iterin2 it2=vec2;
  Iter itout=vec3;
  while ((it1!=vec1_end) && (itout !=vec3_end)) {
    *itout += hfcast<T>((*it1) + (*it2));
    ++it1; ++it2; ++itout;
    if (it2==vec2_end) it2=vec2;
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T , class S , class U > inline void hAddAddConv( std::vector<T> & vec1 , std::vector<S> & vec2 , std::vector<U> & vec3) {
hAddAddConv ( vec1.begin(),vec1.end() , vec2.begin(),vec2.end() , vec3.begin(),vec3.end());
}
template < class T , class S , class U > inline void hAddAddConv( casa::Vector<T> & vec1 , casa::Vector<S> & vec2 , casa::Vector<U> & vec3) {
hAddAddConv ( vec1.cbegin(),vec1.cend() , vec2.cbegin(),vec2.cend() , vec3.cbegin(),vec3.cend());
}

void (*fptr_hAddAddConv_HComplexHComplexHComplex123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HComplex> & vec2 , std::vector<HComplex> & vec3) = &hAddAddConv;
void (*fptr_hAddAddConv_HComplexHComplexHNumber123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HComplex> & vec2 , std::vector<HNumber> & vec3) = &hAddAddConv;
void (*fptr_hAddAddConv_HComplexHComplexHInteger123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HComplex> & vec2 , std::vector<HInteger> & vec3) = &hAddAddConv;
void (*fptr_hAddAddConv_HComplexHNumberHComplex123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HNumber> & vec2 , std::vector<HComplex> & vec3) = &hAddAddConv;
void (*fptr_hAddAddConv_HComplexHNumberHNumber123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HNumber> & vec2 , std::vector<HNumber> & vec3) = &hAddAddConv;
void (*fptr_hAddAddConv_HComplexHNumberHInteger123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HNumber> & vec2 , std::vector<HInteger> & vec3) = &hAddAddConv;
void (*fptr_hAddAddConv_HComplexHIntegerHComplex123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HInteger> & vec2 , std::vector<HComplex> & vec3) = &hAddAddConv;
void (*fptr_hAddAddConv_HComplexHIntegerHNumber123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HInteger> & vec2 , std::vector<HNumber> & vec3) = &hAddAddConv;
void (*fptr_hAddAddConv_HComplexHIntegerHInteger123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HInteger> & vec2 , std::vector<HInteger> & vec3) = &hAddAddConv;
void (*fptr_hAddAddConv_HNumberHComplexHComplex123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HComplex> & vec2 , std::vector<HComplex> & vec3) = &hAddAddConv;
void (*fptr_hAddAddConv_HNumberHComplexHNumber123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HComplex> & vec2 , std::vector<HNumber> & vec3) = &hAddAddConv;
void (*fptr_hAddAddConv_HNumberHComplexHInteger123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HComplex> & vec2 , std::vector<HInteger> & vec3) = &hAddAddConv;
void (*fptr_hAddAddConv_HNumberHNumberHComplex123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HNumber> & vec2 , std::vector<HComplex> & vec3) = &hAddAddConv;
void (*fptr_hAddAddConv_HNumberHNumberHNumber123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HNumber> & vec2 , std::vector<HNumber> & vec3) = &hAddAddConv;
void (*fptr_hAddAddConv_HNumberHNumberHInteger123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HNumber> & vec2 , std::vector<HInteger> & vec3) = &hAddAddConv;
void (*fptr_hAddAddConv_HNumberHIntegerHComplex123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HInteger> & vec2 , std::vector<HComplex> & vec3) = &hAddAddConv;
void (*fptr_hAddAddConv_HNumberHIntegerHNumber123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HInteger> & vec2 , std::vector<HNumber> & vec3) = &hAddAddConv;
void (*fptr_hAddAddConv_HNumberHIntegerHInteger123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HInteger> & vec2 , std::vector<HInteger> & vec3) = &hAddAddConv;
void (*fptr_hAddAddConv_HIntegerHComplexHComplex123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HComplex> & vec2 , std::vector<HComplex> & vec3) = &hAddAddConv;
void (*fptr_hAddAddConv_HIntegerHComplexHNumber123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HComplex> & vec2 , std::vector<HNumber> & vec3) = &hAddAddConv;
void (*fptr_hAddAddConv_HIntegerHComplexHInteger123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HComplex> & vec2 , std::vector<HInteger> & vec3) = &hAddAddConv;
void (*fptr_hAddAddConv_HIntegerHNumberHComplex123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HNumber> & vec2 , std::vector<HComplex> & vec3) = &hAddAddConv;
void (*fptr_hAddAddConv_HIntegerHNumberHNumber123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HNumber> & vec2 , std::vector<HNumber> & vec3) = &hAddAddConv;
void (*fptr_hAddAddConv_HIntegerHNumberHInteger123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HNumber> & vec2 , std::vector<HInteger> & vec3) = &hAddAddConv;
void (*fptr_hAddAddConv_HIntegerHIntegerHComplex123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HInteger> & vec2 , std::vector<HComplex> & vec3) = &hAddAddConv;
void (*fptr_hAddAddConv_HIntegerHIntegerHNumber123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HInteger> & vec2 , std::vector<HNumber> & vec3) = &hAddAddConv;
void (*fptr_hAddAddConv_HIntegerHIntegerHInteger123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HInteger> & vec2 , std::vector<HInteger> & vec3) = &hAddAddConv;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//-----------------------------------------------------------------------
/*!

  \brief Performs a Add between the vector and a scalar, where the result is returned in the second vector.

  \param vec1: STL Iterator pointing to the first element of an array with
         input  values.
  \param vec1_end: STL Iterator pointing to the end of the input vector

  \param val parameter containing the second operand

  \param vec2: STL Iterator pointing to the first element of an array with
         output values.
  \param vec2_end: STL Iterator pointing to the end of the output vector
*/
template <class Iterin1, class S, class Iter>
void hAdd2(const Iterin1 vec1,const Iterin1 vec1_end, S val, const Iter vec2,const Iter vec2_end)
{
  typedef typename Iter::value_type T;
  Iterin1 itin=vec1;
  Iter itout=vec2;
  T val_t=hfcast<T>(val);
  while ((itin!=vec1_end) && (itout !=vec2_end)) {
    *itout = hfcast<T>(*itin) + val_t;
    ++itin; ++itout;
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T , class S , class U > inline void hAdd( std::vector<T> & vec1 , S val , std::vector<U> & vec2) {
hAdd2 ( vec1.begin(),vec1.end() , val , vec2.begin(),vec2.end());
}
template < class T , class S , class U > inline void hAdd( casa::Vector<T> & vec1 , S val , casa::Vector<U> & vec2) {
hAdd2 ( vec1.cbegin(),vec1.cend() , val , vec2.cbegin(),vec2.cend());
}

void (*fptr_hAdd2_HComplexHComplexHComplex123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , HComplex val , std::vector<HComplex> & vec2) = &hAdd;
void (*fptr_hAdd2_HComplexHComplexHNumber123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , HComplex val , std::vector<HNumber> & vec2) = &hAdd;
void (*fptr_hAdd2_HComplexHComplexHInteger123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , HComplex val , std::vector<HInteger> & vec2) = &hAdd;
void (*fptr_hAdd2_HComplexHNumberHComplex123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , HNumber val , std::vector<HComplex> & vec2) = &hAdd;
void (*fptr_hAdd2_HComplexHNumberHNumber123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , HNumber val , std::vector<HNumber> & vec2) = &hAdd;
void (*fptr_hAdd2_HComplexHNumberHInteger123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , HNumber val , std::vector<HInteger> & vec2) = &hAdd;
void (*fptr_hAdd2_HComplexHIntegerHComplex123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , HInteger val , std::vector<HComplex> & vec2) = &hAdd;
void (*fptr_hAdd2_HComplexHIntegerHNumber123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , HInteger val , std::vector<HNumber> & vec2) = &hAdd;
void (*fptr_hAdd2_HComplexHIntegerHInteger123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , HInteger val , std::vector<HInteger> & vec2) = &hAdd;
void (*fptr_hAdd2_HNumberHComplexHComplex123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , HComplex val , std::vector<HComplex> & vec2) = &hAdd;
void (*fptr_hAdd2_HNumberHComplexHNumber123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , HComplex val , std::vector<HNumber> & vec2) = &hAdd;
void (*fptr_hAdd2_HNumberHComplexHInteger123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , HComplex val , std::vector<HInteger> & vec2) = &hAdd;
void (*fptr_hAdd2_HNumberHNumberHComplex123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , HNumber val , std::vector<HComplex> & vec2) = &hAdd;
void (*fptr_hAdd2_HNumberHNumberHNumber123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , HNumber val , std::vector<HNumber> & vec2) = &hAdd;
void (*fptr_hAdd2_HNumberHNumberHInteger123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , HNumber val , std::vector<HInteger> & vec2) = &hAdd;
void (*fptr_hAdd2_HNumberHIntegerHComplex123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , HInteger val , std::vector<HComplex> & vec2) = &hAdd;
void (*fptr_hAdd2_HNumberHIntegerHNumber123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , HInteger val , std::vector<HNumber> & vec2) = &hAdd;
void (*fptr_hAdd2_HNumberHIntegerHInteger123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , HInteger val , std::vector<HInteger> & vec2) = &hAdd;
void (*fptr_hAdd2_HIntegerHComplexHComplex123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , HComplex val , std::vector<HComplex> & vec2) = &hAdd;
void (*fptr_hAdd2_HIntegerHComplexHNumber123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , HComplex val , std::vector<HNumber> & vec2) = &hAdd;
void (*fptr_hAdd2_HIntegerHComplexHInteger123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , HComplex val , std::vector<HInteger> & vec2) = &hAdd;
void (*fptr_hAdd2_HIntegerHNumberHComplex123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , HNumber val , std::vector<HComplex> & vec2) = &hAdd;
void (*fptr_hAdd2_HIntegerHNumberHNumber123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , HNumber val , std::vector<HNumber> & vec2) = &hAdd;
void (*fptr_hAdd2_HIntegerHNumberHInteger123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , HNumber val , std::vector<HInteger> & vec2) = &hAdd;
void (*fptr_hAdd2_HIntegerHIntegerHComplex123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , HInteger val , std::vector<HComplex> & vec2) = &hAdd;
void (*fptr_hAdd2_HIntegerHIntegerHNumber123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , HInteger val , std::vector<HNumber> & vec2) = &hAdd;
void (*fptr_hAdd2_HIntegerHIntegerHInteger123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , HInteger val , std::vector<HInteger> & vec2) = &hAdd;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//========================================================================
//-----------------------------------------------------------------------
/*!

  \brief Performs a Div between the two vectors, which is returned in the first vector. If the second vector is shorter it will be applied multiple times.

  \param vec1: STL Iterator pointing to the first element of an array with
         input and output values.
  \param vec1_end: STL Iterator pointing to the end of the input vector

  \param vec2: STL Iterator pointing to the first element of an array with
         input values.
  \param vec2_end: STL Iterator pointing to the end of the input vector
*/
template <class Iter, class Iterin>
void hiDiv(const Iter vec1,const Iter vec1_end, const Iterin vec2,const Iterin vec2_end)
{
  typedef typename Iter::value_type T;
  Iter it1=vec1;
  Iterin it2=vec2;
  while (it1!=vec1_end) {
    *it1 /= hfcast<T>(*it2);
    ++it1; ++it2;
    if (it2==vec2_end) it2=vec2;
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T , class S > inline void hiDiv( std::vector<T> & vec1 , std::vector<S> & vec2) {
hiDiv ( vec1.begin(),vec1.end() , vec2.begin(),vec2.end());
}
template < class T , class S > inline void hiDiv( casa::Vector<T> & vec1 , casa::Vector<S> & vec2) {
hiDiv ( vec1.cbegin(),vec1.cend() , vec2.cbegin(),vec2.cend());
}

void (*fptr_hiDiv_HComplexHComplex12STDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HComplex> & vec2) = &hiDiv;
void (*fptr_hiDiv_HComplexHNumber12STDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HNumber> & vec2) = &hiDiv;
void (*fptr_hiDiv_HComplexHInteger12STDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HInteger> & vec2) = &hiDiv;
void (*fptr_hiDiv_HNumberHComplex12STDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HComplex> & vec2) = &hiDiv;
void (*fptr_hiDiv_HNumberHNumber12STDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HNumber> & vec2) = &hiDiv;
void (*fptr_hiDiv_HNumberHInteger12STDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HInteger> & vec2) = &hiDiv;
void (*fptr_hiDiv_HIntegerHComplex12STDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HComplex> & vec2) = &hiDiv;
void (*fptr_hiDiv_HIntegerHNumber12STDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HNumber> & vec2) = &hiDiv;
void (*fptr_hiDiv_HIntegerHInteger12STDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HInteger> & vec2) = &hiDiv;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//-----------------------------------------------------------------------
/*!

  \brief Performs a Div between the vector and a scalar (applied to each element), which is returned in the first vector.

  \param vec1: STL Iterator pointing to the first element of an array with
         input and output values.
  \param vec1_end: STL Iterator pointing to the end of the input vector

  \param val: Value containing the second operand
*/
template <class Iter, class S>
void hiDiv2(const Iter vec1,const Iter vec1_end, S val)
{
  typedef typename Iter::value_type T;
  Iter it=vec1;
  T val_t = hfcast<T>(val);
  while (it!=vec1_end) {
    *it /= val_t;
    ++it;
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T , class S > inline void hiDiv( std::vector<T> & vec1 , S val) {
hiDiv2 ( vec1.begin(),vec1.end() , val);
}
template < class T , class S > inline void hiDiv( casa::Vector<T> & vec1 , S val) {
hiDiv2 ( vec1.cbegin(),vec1.cend() , val);
}

void (*fptr_hiDiv2_HComplexHComplex12STDIT)( std::vector<HComplex> & vec1 , HComplex val) = &hiDiv;
void (*fptr_hiDiv2_HComplexHNumber12STDIT)( std::vector<HComplex> & vec1 , HNumber val) = &hiDiv;
void (*fptr_hiDiv2_HComplexHInteger12STDIT)( std::vector<HComplex> & vec1 , HInteger val) = &hiDiv;
void (*fptr_hiDiv2_HNumberHComplex12STDIT)( std::vector<HNumber> & vec1 , HComplex val) = &hiDiv;
void (*fptr_hiDiv2_HNumberHNumber12STDIT)( std::vector<HNumber> & vec1 , HNumber val) = &hiDiv;
void (*fptr_hiDiv2_HNumberHInteger12STDIT)( std::vector<HNumber> & vec1 , HInteger val) = &hiDiv;
void (*fptr_hiDiv2_HIntegerHComplex12STDIT)( std::vector<HInteger> & vec1 , HComplex val) = &hiDiv;
void (*fptr_hiDiv2_HIntegerHNumber12STDIT)( std::vector<HInteger> & vec1 , HNumber val) = &hiDiv;
void (*fptr_hiDiv2_HIntegerHInteger12STDIT)( std::vector<HInteger> & vec1 , HInteger val) = &hiDiv;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//-----------------------------------------------------------------------
/*!

  \brief Performs a Div between the two vectors, which is returned in the third vector.

  \param vec1: STL Iterator pointing to the first element of an array with
         input  values.
  \param vec1_end: STL Iterator pointing to the end of the input vector

  \param vec2: STL Iterator pointing to the first element of an array with
         input values.
  \param vec2_end: STL Iterator pointing to the end of the input vector

  \param vec3: STL Iterator pointing to the first element of an array with
         output values.
  \param vec3_end: STL Iterator pointing to the end of the output vector
*/
template <class Iterin1, class Iterin2, class Iter>
void hDiv(const Iterin1 vec1,const Iterin1 vec1_end, const Iterin2 vec2,const Iterin2 vec2_end, const Iter vec3,const Iter vec3_end)
{
  typedef typename Iter::value_type T;
  Iterin1 it1=vec1;
  Iterin2 it2=vec2;
  Iter itout=vec3;
  while ((it1!=vec1_end) && (itout !=vec3_end)) {
    *itout = hfcast<T>((*it1) / (*it2));
    ++it1; ++it2; ++itout;
    if (it2==vec2_end) it2=vec2;
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T , class S , class U > inline void hDiv( std::vector<T> & vec1 , std::vector<S> & vec2 , std::vector<U> & vec3) {
hDiv ( vec1.begin(),vec1.end() , vec2.begin(),vec2.end() , vec3.begin(),vec3.end());
}
template < class T , class S , class U > inline void hDiv( casa::Vector<T> & vec1 , casa::Vector<S> & vec2 , casa::Vector<U> & vec3) {
hDiv ( vec1.cbegin(),vec1.cend() , vec2.cbegin(),vec2.cend() , vec3.cbegin(),vec3.cend());
}

void (*fptr_hDiv_HComplexHComplexHComplex123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HComplex> & vec2 , std::vector<HComplex> & vec3) = &hDiv;
void (*fptr_hDiv_HComplexHComplexHNumber123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HComplex> & vec2 , std::vector<HNumber> & vec3) = &hDiv;
void (*fptr_hDiv_HComplexHComplexHInteger123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HComplex> & vec2 , std::vector<HInteger> & vec3) = &hDiv;
void (*fptr_hDiv_HComplexHNumberHComplex123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HNumber> & vec2 , std::vector<HComplex> & vec3) = &hDiv;
void (*fptr_hDiv_HComplexHNumberHNumber123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HNumber> & vec2 , std::vector<HNumber> & vec3) = &hDiv;
void (*fptr_hDiv_HComplexHNumberHInteger123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HNumber> & vec2 , std::vector<HInteger> & vec3) = &hDiv;
void (*fptr_hDiv_HComplexHIntegerHComplex123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HInteger> & vec2 , std::vector<HComplex> & vec3) = &hDiv;
void (*fptr_hDiv_HComplexHIntegerHNumber123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HInteger> & vec2 , std::vector<HNumber> & vec3) = &hDiv;
void (*fptr_hDiv_HComplexHIntegerHInteger123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HInteger> & vec2 , std::vector<HInteger> & vec3) = &hDiv;
void (*fptr_hDiv_HNumberHComplexHComplex123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HComplex> & vec2 , std::vector<HComplex> & vec3) = &hDiv;
void (*fptr_hDiv_HNumberHComplexHNumber123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HComplex> & vec2 , std::vector<HNumber> & vec3) = &hDiv;
void (*fptr_hDiv_HNumberHComplexHInteger123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HComplex> & vec2 , std::vector<HInteger> & vec3) = &hDiv;
void (*fptr_hDiv_HNumberHNumberHComplex123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HNumber> & vec2 , std::vector<HComplex> & vec3) = &hDiv;
void (*fptr_hDiv_HNumberHNumberHNumber123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HNumber> & vec2 , std::vector<HNumber> & vec3) = &hDiv;
void (*fptr_hDiv_HNumberHNumberHInteger123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HNumber> & vec2 , std::vector<HInteger> & vec3) = &hDiv;
void (*fptr_hDiv_HNumberHIntegerHComplex123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HInteger> & vec2 , std::vector<HComplex> & vec3) = &hDiv;
void (*fptr_hDiv_HNumberHIntegerHNumber123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HInteger> & vec2 , std::vector<HNumber> & vec3) = &hDiv;
void (*fptr_hDiv_HNumberHIntegerHInteger123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HInteger> & vec2 , std::vector<HInteger> & vec3) = &hDiv;
void (*fptr_hDiv_HIntegerHComplexHComplex123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HComplex> & vec2 , std::vector<HComplex> & vec3) = &hDiv;
void (*fptr_hDiv_HIntegerHComplexHNumber123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HComplex> & vec2 , std::vector<HNumber> & vec3) = &hDiv;
void (*fptr_hDiv_HIntegerHComplexHInteger123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HComplex> & vec2 , std::vector<HInteger> & vec3) = &hDiv;
void (*fptr_hDiv_HIntegerHNumberHComplex123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HNumber> & vec2 , std::vector<HComplex> & vec3) = &hDiv;
void (*fptr_hDiv_HIntegerHNumberHNumber123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HNumber> & vec2 , std::vector<HNumber> & vec3) = &hDiv;
void (*fptr_hDiv_HIntegerHNumberHInteger123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HNumber> & vec2 , std::vector<HInteger> & vec3) = &hDiv;
void (*fptr_hDiv_HIntegerHIntegerHComplex123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HInteger> & vec2 , std::vector<HComplex> & vec3) = &hDiv;
void (*fptr_hDiv_HIntegerHIntegerHNumber123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HInteger> & vec2 , std::vector<HNumber> & vec3) = &hDiv;
void (*fptr_hDiv_HIntegerHIntegerHInteger123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HInteger> & vec2 , std::vector<HInteger> & vec3) = &hDiv;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//-----------------------------------------------------------------------
/*!
  \brief Performs a Div between the two vectors, and adds the result to the output (third) vector.

    \param vec1: Numeric input vector

    \param vec2: Vector containing the second operands

    \param vec3: Vector containing the results

*/
template <class Iter>
void hDivAdd(const Iter vec1,const Iter vec1_end, const Iter vec2,const Iter vec2_end, const Iter vec3,const Iter vec3_end)
{
  typedef typename Iter::value_type T;
  Iter it1(vec1), it2(vec2), itout(vec3);
  while ((it1!=vec1_end) && (itout !=vec3_end)) {
    *itout += (*it1) / (*it2);
    ++it1; ++it2; ++itout;
    if (it2==vec2_end) it2=vec2;
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T > inline void hDivAdd( std::vector<T> & vec1 , std::vector<T> & vec2 , std::vector<T> & vec3) {
hDivAdd ( vec1.begin(),vec1.end() , vec2.begin(),vec2.end() , vec3.begin(),vec3.end());
}
template < class T > inline void hDivAdd( casa::Vector<T> & vec1 , casa::Vector<T> & vec2 , casa::Vector<T> & vec3) {
hDivAdd ( vec1.cbegin(),vec1.cend() , vec2.cbegin(),vec2.cend() , vec3.cbegin(),vec3.cend());
}

void (*fptr_hDivAdd_HComplex111STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HComplex> & vec2 , std::vector<HComplex> & vec3) = &hDivAdd;
void (*fptr_hDivAdd_HNumber111STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HNumber> & vec2 , std::vector<HNumber> & vec3) = &hDivAdd;
void (*fptr_hDivAdd_HInteger111STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HInteger> & vec2 , std::vector<HInteger> & vec3) = &hDivAdd;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//-----------------------------------------------------------------------
/*!
  \brief Performs a Div between the two vectors, and adds the result to the output (third) vector - automatic casting is done.

    \param vec1: Numeric input vector

    \param vec2: Vector containing the second operands

    \param vec3: Vector containing the results

*/
template <class Iterin1, class Iterin2, class Iter>
void hDivAddConv(const Iterin1 vec1,const Iterin1 vec1_end, const Iterin2 vec2,const Iterin2 vec2_end, const Iter vec3,const Iter vec3_end)
{
  typedef typename Iter::value_type T;
  Iterin1 it1=vec1;
  Iterin2 it2=vec2;
  Iter itout=vec3;
  while ((it1!=vec1_end) && (itout !=vec3_end)) {
    *itout += hfcast<T>((*it1) / (*it2));
    ++it1; ++it2; ++itout;
    if (it2==vec2_end) it2=vec2;
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T , class S , class U > inline void hDivAddConv( std::vector<T> & vec1 , std::vector<S> & vec2 , std::vector<U> & vec3) {
hDivAddConv ( vec1.begin(),vec1.end() , vec2.begin(),vec2.end() , vec3.begin(),vec3.end());
}
template < class T , class S , class U > inline void hDivAddConv( casa::Vector<T> & vec1 , casa::Vector<S> & vec2 , casa::Vector<U> & vec3) {
hDivAddConv ( vec1.cbegin(),vec1.cend() , vec2.cbegin(),vec2.cend() , vec3.cbegin(),vec3.cend());
}

void (*fptr_hDivAddConv_HComplexHComplexHComplex123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HComplex> & vec2 , std::vector<HComplex> & vec3) = &hDivAddConv;
void (*fptr_hDivAddConv_HComplexHComplexHNumber123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HComplex> & vec2 , std::vector<HNumber> & vec3) = &hDivAddConv;
void (*fptr_hDivAddConv_HComplexHComplexHInteger123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HComplex> & vec2 , std::vector<HInteger> & vec3) = &hDivAddConv;
void (*fptr_hDivAddConv_HComplexHNumberHComplex123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HNumber> & vec2 , std::vector<HComplex> & vec3) = &hDivAddConv;
void (*fptr_hDivAddConv_HComplexHNumberHNumber123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HNumber> & vec2 , std::vector<HNumber> & vec3) = &hDivAddConv;
void (*fptr_hDivAddConv_HComplexHNumberHInteger123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HNumber> & vec2 , std::vector<HInteger> & vec3) = &hDivAddConv;
void (*fptr_hDivAddConv_HComplexHIntegerHComplex123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HInteger> & vec2 , std::vector<HComplex> & vec3) = &hDivAddConv;
void (*fptr_hDivAddConv_HComplexHIntegerHNumber123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HInteger> & vec2 , std::vector<HNumber> & vec3) = &hDivAddConv;
void (*fptr_hDivAddConv_HComplexHIntegerHInteger123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , std::vector<HInteger> & vec2 , std::vector<HInteger> & vec3) = &hDivAddConv;
void (*fptr_hDivAddConv_HNumberHComplexHComplex123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HComplex> & vec2 , std::vector<HComplex> & vec3) = &hDivAddConv;
void (*fptr_hDivAddConv_HNumberHComplexHNumber123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HComplex> & vec2 , std::vector<HNumber> & vec3) = &hDivAddConv;
void (*fptr_hDivAddConv_HNumberHComplexHInteger123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HComplex> & vec2 , std::vector<HInteger> & vec3) = &hDivAddConv;
void (*fptr_hDivAddConv_HNumberHNumberHComplex123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HNumber> & vec2 , std::vector<HComplex> & vec3) = &hDivAddConv;
void (*fptr_hDivAddConv_HNumberHNumberHNumber123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HNumber> & vec2 , std::vector<HNumber> & vec3) = &hDivAddConv;
void (*fptr_hDivAddConv_HNumberHNumberHInteger123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HNumber> & vec2 , std::vector<HInteger> & vec3) = &hDivAddConv;
void (*fptr_hDivAddConv_HNumberHIntegerHComplex123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HInteger> & vec2 , std::vector<HComplex> & vec3) = &hDivAddConv;
void (*fptr_hDivAddConv_HNumberHIntegerHNumber123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HInteger> & vec2 , std::vector<HNumber> & vec3) = &hDivAddConv;
void (*fptr_hDivAddConv_HNumberHIntegerHInteger123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HInteger> & vec2 , std::vector<HInteger> & vec3) = &hDivAddConv;
void (*fptr_hDivAddConv_HIntegerHComplexHComplex123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HComplex> & vec2 , std::vector<HComplex> & vec3) = &hDivAddConv;
void (*fptr_hDivAddConv_HIntegerHComplexHNumber123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HComplex> & vec2 , std::vector<HNumber> & vec3) = &hDivAddConv;
void (*fptr_hDivAddConv_HIntegerHComplexHInteger123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HComplex> & vec2 , std::vector<HInteger> & vec3) = &hDivAddConv;
void (*fptr_hDivAddConv_HIntegerHNumberHComplex123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HNumber> & vec2 , std::vector<HComplex> & vec3) = &hDivAddConv;
void (*fptr_hDivAddConv_HIntegerHNumberHNumber123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HNumber> & vec2 , std::vector<HNumber> & vec3) = &hDivAddConv;
void (*fptr_hDivAddConv_HIntegerHNumberHInteger123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HNumber> & vec2 , std::vector<HInteger> & vec3) = &hDivAddConv;
void (*fptr_hDivAddConv_HIntegerHIntegerHComplex123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HInteger> & vec2 , std::vector<HComplex> & vec3) = &hDivAddConv;
void (*fptr_hDivAddConv_HIntegerHIntegerHNumber123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HInteger> & vec2 , std::vector<HNumber> & vec3) = &hDivAddConv;
void (*fptr_hDivAddConv_HIntegerHIntegerHInteger123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HInteger> & vec2 , std::vector<HInteger> & vec3) = &hDivAddConv;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//-----------------------------------------------------------------------
/*!

  \brief Performs a Div between the vector and a scalar, where the result is returned in the second vector.

  \param vec1: STL Iterator pointing to the first element of an array with
         input  values.
  \param vec1_end: STL Iterator pointing to the end of the input vector

  \param val parameter containing the second operand

  \param vec2: STL Iterator pointing to the first element of an array with
         output values.
  \param vec2_end: STL Iterator pointing to the end of the output vector
*/
template <class Iterin1, class S, class Iter>
void hDiv2(const Iterin1 vec1,const Iterin1 vec1_end, S val, const Iter vec2,const Iter vec2_end)
{
  typedef typename Iter::value_type T;
  Iterin1 itin=vec1;
  Iter itout=vec2;
  T val_t=hfcast<T>(val);
  while ((itin!=vec1_end) && (itout !=vec2_end)) {
    *itout = hfcast<T>(*itin) / val_t;
    ++itin; ++itout;
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T , class S , class U > inline void hDiv( std::vector<T> & vec1 , S val , std::vector<U> & vec2) {
hDiv2 ( vec1.begin(),vec1.end() , val , vec2.begin(),vec2.end());
}
template < class T , class S , class U > inline void hDiv( casa::Vector<T> & vec1 , S val , casa::Vector<U> & vec2) {
hDiv2 ( vec1.cbegin(),vec1.cend() , val , vec2.cbegin(),vec2.cend());
}

void (*fptr_hDiv2_HComplexHComplexHComplex123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , HComplex val , std::vector<HComplex> & vec2) = &hDiv;
void (*fptr_hDiv2_HComplexHComplexHNumber123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , HComplex val , std::vector<HNumber> & vec2) = &hDiv;
void (*fptr_hDiv2_HComplexHComplexHInteger123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , HComplex val , std::vector<HInteger> & vec2) = &hDiv;
void (*fptr_hDiv2_HComplexHNumberHComplex123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , HNumber val , std::vector<HComplex> & vec2) = &hDiv;
void (*fptr_hDiv2_HComplexHNumberHNumber123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , HNumber val , std::vector<HNumber> & vec2) = &hDiv;
void (*fptr_hDiv2_HComplexHNumberHInteger123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , HNumber val , std::vector<HInteger> & vec2) = &hDiv;
void (*fptr_hDiv2_HComplexHIntegerHComplex123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , HInteger val , std::vector<HComplex> & vec2) = &hDiv;
void (*fptr_hDiv2_HComplexHIntegerHNumber123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , HInteger val , std::vector<HNumber> & vec2) = &hDiv;
void (*fptr_hDiv2_HComplexHIntegerHInteger123STDITSTDITSTDIT)( std::vector<HComplex> & vec1 , HInteger val , std::vector<HInteger> & vec2) = &hDiv;
void (*fptr_hDiv2_HNumberHComplexHComplex123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , HComplex val , std::vector<HComplex> & vec2) = &hDiv;
void (*fptr_hDiv2_HNumberHComplexHNumber123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , HComplex val , std::vector<HNumber> & vec2) = &hDiv;
void (*fptr_hDiv2_HNumberHComplexHInteger123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , HComplex val , std::vector<HInteger> & vec2) = &hDiv;
void (*fptr_hDiv2_HNumberHNumberHComplex123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , HNumber val , std::vector<HComplex> & vec2) = &hDiv;
void (*fptr_hDiv2_HNumberHNumberHNumber123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , HNumber val , std::vector<HNumber> & vec2) = &hDiv;
void (*fptr_hDiv2_HNumberHNumberHInteger123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , HNumber val , std::vector<HInteger> & vec2) = &hDiv;
void (*fptr_hDiv2_HNumberHIntegerHComplex123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , HInteger val , std::vector<HComplex> & vec2) = &hDiv;
void (*fptr_hDiv2_HNumberHIntegerHNumber123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , HInteger val , std::vector<HNumber> & vec2) = &hDiv;
void (*fptr_hDiv2_HNumberHIntegerHInteger123STDITSTDITSTDIT)( std::vector<HNumber> & vec1 , HInteger val , std::vector<HInteger> & vec2) = &hDiv;
void (*fptr_hDiv2_HIntegerHComplexHComplex123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , HComplex val , std::vector<HComplex> & vec2) = &hDiv;
void (*fptr_hDiv2_HIntegerHComplexHNumber123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , HComplex val , std::vector<HNumber> & vec2) = &hDiv;
void (*fptr_hDiv2_HIntegerHComplexHInteger123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , HComplex val , std::vector<HInteger> & vec2) = &hDiv;
void (*fptr_hDiv2_HIntegerHNumberHComplex123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , HNumber val , std::vector<HComplex> & vec2) = &hDiv;
void (*fptr_hDiv2_HIntegerHNumberHNumber123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , HNumber val , std::vector<HNumber> & vec2) = &hDiv;
void (*fptr_hDiv2_HIntegerHNumberHInteger123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , HNumber val , std::vector<HInteger> & vec2) = &hDiv;
void (*fptr_hDiv2_HIntegerHIntegerHComplex123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , HInteger val , std::vector<HComplex> & vec2) = &hDiv;
void (*fptr_hDiv2_HIntegerHIntegerHNumber123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , HInteger val , std::vector<HNumber> & vec2) = &hDiv;
void (*fptr_hDiv2_HIntegerHIntegerHInteger123STDITSTDITSTDIT)( std::vector<HInteger> & vec1 , HInteger val , std::vector<HInteger> & vec2) = &hDiv;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//$ENDITERATE
//-----------------------------------------------------------------------
/*!

  \brief Multiplies each element in the vector with -1 in place, i.e. the input vector is also the output vector.

  \param vec: STL Iterator pointing to the first element of an array with
         input values.
  \param vec_end: STL Iterator pointing to the end of the input vector
*/
template <class Iter>
void hNegate(const Iter vec,const Iter vec_end)
{
  Iter it=vec;
  typename Iter::value_type fac = -1;
  while (it!=vec_end) {
    *it=(*it) * fac;
    ++it;
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T > inline void hNegate( std::vector<T> & vec) {
hNegate ( vec.begin(),vec.end());
}
template < class T > inline void hNegate( casa::Vector<T> & vec) {
hNegate ( vec.cbegin(),vec.cend());
}

void (*fptr_hNegate_HComplex1STDIT)( std::vector<HComplex> & vec) = &hNegate;
void (*fptr_hNegate_HNumber1STDIT)( std::vector<HNumber> & vec) = &hNegate;
void (*fptr_hNegate_HInteger1STDIT)( std::vector<HInteger> & vec) = &hNegate;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//-----------------------------------------------------------------------
/*!
  \brief Performs a sum over the values in a vector and returns the value

    \param vec: Numeric input vector

*/
template <class Iter>
typename Iter::value_type hSum (const Iter vec,const Iter vec_end)
{
  typedef typename Iter::value_type T;
  T sum=hfnull<T>();
  Iter it=vec;
  while (it!=vec_end) {sum+=*it; ++it;};
  return sum;
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T > inline T hSum( std::vector<T> & vec) {
return hSum ( vec.begin(),vec.end());
}
template < class T > inline T hSum( casa::Vector<T> & vec) {
return hSum ( vec.cbegin(),vec.cend());
}

HComplex (*fptr_hSum_HComplex1STDIT)( std::vector<HComplex> & vec) = &hSum;
HNumber (*fptr_hSum_HNumber1STDIT)( std::vector<HNumber> & vec) = &hSum;
HInteger (*fptr_hSum_HInteger1STDIT)( std::vector<HInteger> & vec) = &hSum;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//-----------------------------------------------------------------------
/*!
  \brief Returns the lengths or norm of a vector (i.e. Sqrt(Sum_i(xi*+2)))

    \param vec: Numeric input vector

*/
template <class Iter>
HNumber hNorm (const Iter vec,const Iter vec_end)
{
  HNumber sum=0;
  Iter it=vec;
  while (it!=vec_end) {sum += (*it) * (*it); ++it;};
  return sqrt(sum);
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T > inline HNumber hNorm( std::vector<T> & vec) {
return hNorm ( vec.begin(),vec.end());
}
template < class T > inline HNumber hNorm( casa::Vector<T> & vec) {
return hNorm ( vec.cbegin(),vec.cend());
}

HNumber (*fptr_hNorm_HNumber1STDIT)( std::vector<HNumber> & vec) = &hNorm;
HNumber (*fptr_hNorm_HInteger1STDIT)( std::vector<HInteger> & vec) = &hNorm;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//-----------------------------------------------------------------------
/*!
  \brief Normalizes a vector to length unity

    \param vec: Numeric input and output vector

*/
template <class Iter>
void hNormalize (const Iter vec,const Iter vec_end)
{
  HNumber norm=hNorm(vec,vec_end);
  Iter it=vec;
  while (it!=vec_end) {*it=(*it)/norm; ++it;};
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T > inline void hNormalize( std::vector<T> & vec) {
hNormalize ( vec.begin(),vec.end());
}
template < class T > inline void hNormalize( casa::Vector<T> & vec) {
hNormalize ( vec.cbegin(),vec.cend());
}

void (*fptr_hNormalize_HNumber1STDIT)( std::vector<HNumber> & vec) = &hNormalize;
void (*fptr_hNormalize_HInteger1STDIT)( std::vector<HInteger> & vec) = &hNormalize;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//-----------------------------------------------------------------------
/*!
  \brief Returns the mean value of all elements in a vector

    \param vec: Numeric input vector

*/
template <class Iter>
HNumber hMean (const Iter vec,const Iter vec_end)
{
  HNumber mean=hSum(vec,vec_end);
  if (vec_end>vec) mean/=(vec_end-vec);
  return mean;
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T > inline HNumber hMean( std::vector<T> & vec) {
return hMean ( vec.begin(),vec.end());
}
template < class T > inline HNumber hMean( casa::Vector<T> & vec) {
return hMean ( vec.cbegin(),vec.cend());
}

HNumber (*fptr_hMean_HNumber1STDIT)( std::vector<HNumber> & vec) = &hMean;
HNumber (*fptr_hMean_HInteger1STDIT)( std::vector<HInteger> & vec) = &hMean;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//-----------------------------------------------------------------------
/*!
  \brief Sorts a vector in place

    \param vec: Numeric input vector


  Attention!!! The vector will be sorted in place. Hence, if you want to
  keep the data in its original order, you need to copy the data first
  to a scratch vector and then call this function with the scratch vector!
*/
template <class Iter>
void hSort(const Iter vec, const Iter vec_end)
{
  sort(vec,vec_end);
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T > inline void hSort( std::vector<T> & vec) {
hSort ( vec.begin(),vec.end());
}
template < class T > inline void hSort( casa::Vector<T> & vec) {
hSort ( vec.cbegin(),vec.cend());
}

void (*fptr_hSort_HComplex1STDIT)( std::vector<HComplex> & vec) = &hSort;
void (*fptr_hSort_HNumber1STDIT)( std::vector<HNumber> & vec) = &hSort;
void (*fptr_hSort_HInteger1STDIT)( std::vector<HInteger> & vec) = &hSort;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//-----------------------------------------------------------------------
/*!
  \brief Sorts a vector in place and returns the median value of the elements

    \param vec: Numeric input vector


  Attention!!! The vector will be sorted first. Hence, if you want to
  keep the data in its original order, you need to copy the data first
  to a scratch vector and then call this function with the scratch vector!
*/
template <class Iter>
typename Iter::value_type hSortMedian(const Iter vec, const Iter vec_end)
{
  sort(vec,vec_end);
  if (vec_end!=vec) return *(vec+(vec_end-vec)/2);
  else return hfnull<typename Iter::value_type>();
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T > inline T hSortMedian( std::vector<T> & vec) {
return hSortMedian ( vec.begin(),vec.end());
}
template < class T > inline T hSortMedian( casa::Vector<T> & vec) {
return hSortMedian ( vec.cbegin(),vec.cend());
}

HNumber (*fptr_hSortMedian_HNumber1STDIT)( std::vector<HNumber> & vec) = &hSortMedian;
HInteger (*fptr_hSortMedian_HInteger1STDIT)( std::vector<HInteger> & vec) = &hSortMedian;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//-----------------------------------------------------------------------
/*!
  \brief Returns the median value of the elements

    \param vec: Numeric input vector

*/
template <class T>
T hMedian(std::vector<T> & vec)
{
  std::vector<T> scratch(vec);
  return hSortMedian(scratch);
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers

HNumber (*fptr_hMedian_HNumber1STL)( std::vector<HNumber> & vec) = &hMedian;
HInteger (*fptr_hMedian_HInteger1STL)( std::vector<HInteger> & vec) = &hMedian;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//-----------------------------------------------------------------------
/*!
  \brief Calculates the standard deviation around a mean value.

    \param vec: Numeric input vector

    \param mean: The mean value of the vector caluclated beforehand

*/
template <class Iter>
HNumber hStdDev (const Iter vec,const Iter vec_end, const typename Iter::value_type mean)
{
  typedef typename Iter::value_type T;
  HNumber scrt,sum=0.0;
  HInteger len=vec_end-vec;
  Iter it=vec;
  while (it<vec_end) {
    scrt = *it - mean;
    sum += scrt * scrt;
    ++it;
  };
  if (len>1) return sqrt(sum / (len-1));
  else return sqrt(sum);
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T > inline HNumber hStdDev( std::vector<T> & vec , HNumber mean) {
return hStdDev ( vec.begin(),vec.end() , mean);
}
template < class T > inline HNumber hStdDev( casa::Vector<T> & vec , HNumber mean) {
return hStdDev ( vec.cbegin(),vec.cend() , mean);
}

HNumber (*fptr_hStdDev_HNumber1HNumberSTDIT)( std::vector<HNumber> & vec , HNumber mean) = &hStdDev;
HNumber (*fptr_hStdDev_HInteger1HNumberSTDIT)( std::vector<HInteger> & vec , HNumber mean) = &hStdDev;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//-----------------------------------------------------------------------
/*!
  \brief Calculates the standard deviation of a vector of values

    \param vec: Numeric input vector

*/
template <class Iter>
HNumber hStdDev (const Iter vec,const Iter vec_end)
{
  return hStdDev(vec,vec_end,hMean(vec,vec_end));
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T > inline HNumber hStdDev( std::vector<T> & vec) {
return hStdDev ( vec.begin(),vec.end());
}
template < class T > inline HNumber hStdDev( casa::Vector<T> & vec) {
return hStdDev ( vec.cbegin(),vec.cend());
}

HNumber (*fptr_hStdDev_HNumber1STDIT)( std::vector<HNumber> & vec) = &hStdDev;
HNumber (*fptr_hStdDev_HInteger1STDIT)( std::vector<HInteger> & vec) = &hStdDev;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//========================================================================
//$ITERATE MFUNC GreaterThan,GreaterEqual,LessThan,LessEqual
//========================================================================
//-----------------------------------------------------------------------
/*!
  \brief Find the samples that are LessEqual a certain threshold value and returns the number of samples found and the positions of the samples in a second vector

    \param vec: Numeric input vector to search through

    \param threshold: The threshold value

    \param vecout: Output vector - contains a list of positions in input vector which are above threshold

*/
template <class Iter>
HInteger hFindLessEqual (const Iter vec , const Iter vec_end, const typename Iter::value_type threshold, const typename vector<HInteger>::iterator vecout, const typename vector<HInteger>::iterator vecout_end)
{
  Iter it=vec;
  typename vector<HInteger>::iterator itout=vecout;
  while (it<vec_end) {
    if (*it <= threshold) {
      if (itout < vecout_end) {
 *itout=(it-vec);
 ++itout;
      };
    };
    ++it;
  };
  return (itout-vecout);
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T > inline HInteger hFindLessEqual( std::vector<T> & vec , T threshold , std::vector<HInteger> & vecout) {
return hFindLessEqual ( vec.begin(),vec.end() , threshold , vecout.begin(),vecout.end());
}
template < class T > inline HInteger hFindLessEqual( casa::Vector<T> & vec , T threshold , casa::Vector<HInteger> & vecout) {
return hFindLessEqual ( vec.cbegin(),vec.cend() , threshold , vecout.cbegin(),vecout.cend());
}

HInteger (*fptr_hFindLessEqual_HNumber11HIntegerSTDITSTDIT)( std::vector<HNumber> & vec , HNumber threshold , std::vector<HInteger> & vecout) = &hFindLessEqual;
HInteger (*fptr_hFindLessEqual_HInteger11HIntegerSTDITSTDIT)( std::vector<HInteger> & vec , HInteger threshold , std::vector<HInteger> & vecout) = &hFindLessEqual;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//-----------------------------------------------------------------------
/*!
  \brief Find the samples whose absolute values are LessEqual a certain threshold value and returns the number of samples found and the positions of the samples in a second vector

    \param vec: Numeric input vector to search through

    \param threshold: The threshold value

    \param vecout: Output vector - contains a list of positions in input vector which are above threshold

*/
template <class Iter>
HInteger hFindLessEqualAbs (const Iter vec , const Iter vec_end, const typename Iter::value_type threshold, const typename vector<HInteger>::iterator vecout, const typename vector<HInteger>::iterator vecout_end)
{
  Iter it=vec;
  typename vector<HInteger>::iterator itout=vecout;
  while (it<vec_end) {
    if (abs(*it) <= threshold) {
      if (itout < vecout_end) {
 *itout=(it-vec);
 ++itout;
      };
    };
    ++it;
  };
  return (itout-vecout);
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T > inline HInteger hFindLessEqualAbs( std::vector<T> & vec , T threshold , std::vector<HInteger> & vecout) {
return hFindLessEqualAbs ( vec.begin(),vec.end() , threshold , vecout.begin(),vecout.end());
}
template < class T > inline HInteger hFindLessEqualAbs( casa::Vector<T> & vec , T threshold , casa::Vector<HInteger> & vecout) {
return hFindLessEqualAbs ( vec.cbegin(),vec.cend() , threshold , vecout.cbegin(),vecout.cend());
}

HInteger (*fptr_hFindLessEqualAbs_HNumber11HIntegerSTDITSTDIT)( std::vector<HNumber> & vec , HNumber threshold , std::vector<HInteger> & vecout) = &hFindLessEqualAbs;
HInteger (*fptr_hFindLessEqualAbs_HInteger11HIntegerSTDITSTDIT)( std::vector<HInteger> & vec , HInteger threshold , std::vector<HInteger> & vecout) = &hFindLessEqualAbs;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//========================================================================
//-----------------------------------------------------------------------
/*!
  \brief Find the samples that are GreaterThan a certain threshold value and returns the number of samples found and the positions of the samples in a second vector

    \param vec: Numeric input vector to search through

    \param threshold: The threshold value

    \param vecout: Output vector - contains a list of positions in input vector which are above threshold

*/
template <class Iter>
HInteger hFindGreaterThan (const Iter vec , const Iter vec_end, const typename Iter::value_type threshold, const typename vector<HInteger>::iterator vecout, const typename vector<HInteger>::iterator vecout_end)
{
  Iter it=vec;
  typename vector<HInteger>::iterator itout=vecout;
  while (it<vec_end) {
    if (*it > threshold) {
      if (itout < vecout_end) {
 *itout=(it-vec);
 ++itout;
      };
    };
    ++it;
  };
  return (itout-vecout);
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T > inline HInteger hFindGreaterThan( std::vector<T> & vec , T threshold , std::vector<HInteger> & vecout) {
return hFindGreaterThan ( vec.begin(),vec.end() , threshold , vecout.begin(),vecout.end());
}
template < class T > inline HInteger hFindGreaterThan( casa::Vector<T> & vec , T threshold , casa::Vector<HInteger> & vecout) {
return hFindGreaterThan ( vec.cbegin(),vec.cend() , threshold , vecout.cbegin(),vecout.cend());
}

HInteger (*fptr_hFindGreaterThan_HNumber11HIntegerSTDITSTDIT)( std::vector<HNumber> & vec , HNumber threshold , std::vector<HInteger> & vecout) = &hFindGreaterThan;
HInteger (*fptr_hFindGreaterThan_HInteger11HIntegerSTDITSTDIT)( std::vector<HInteger> & vec , HInteger threshold , std::vector<HInteger> & vecout) = &hFindGreaterThan;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//-----------------------------------------------------------------------
/*!
  \brief Find the samples whose absolute values are GreaterThan a certain threshold value and returns the number of samples found and the positions of the samples in a second vector

    \param vec: Numeric input vector to search through

    \param threshold: The threshold value

    \param vecout: Output vector - contains a list of positions in input vector which are above threshold

*/
template <class Iter>
HInteger hFindGreaterThanAbs (const Iter vec , const Iter vec_end, const typename Iter::value_type threshold, const typename vector<HInteger>::iterator vecout, const typename vector<HInteger>::iterator vecout_end)
{
  Iter it=vec;
  typename vector<HInteger>::iterator itout=vecout;
  while (it<vec_end) {
    if (abs(*it) > threshold) {
      if (itout < vecout_end) {
 *itout=(it-vec);
 ++itout;
      };
    };
    ++it;
  };
  return (itout-vecout);
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T > inline HInteger hFindGreaterThanAbs( std::vector<T> & vec , T threshold , std::vector<HInteger> & vecout) {
return hFindGreaterThanAbs ( vec.begin(),vec.end() , threshold , vecout.begin(),vecout.end());
}
template < class T > inline HInteger hFindGreaterThanAbs( casa::Vector<T> & vec , T threshold , casa::Vector<HInteger> & vecout) {
return hFindGreaterThanAbs ( vec.cbegin(),vec.cend() , threshold , vecout.cbegin(),vecout.cend());
}

HInteger (*fptr_hFindGreaterThanAbs_HNumber11HIntegerSTDITSTDIT)( std::vector<HNumber> & vec , HNumber threshold , std::vector<HInteger> & vecout) = &hFindGreaterThanAbs;
HInteger (*fptr_hFindGreaterThanAbs_HInteger11HIntegerSTDITSTDIT)( std::vector<HInteger> & vec , HInteger threshold , std::vector<HInteger> & vecout) = &hFindGreaterThanAbs;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//========================================================================
//-----------------------------------------------------------------------
/*!
  \brief Find the samples that are GreaterEqual a certain threshold value and returns the number of samples found and the positions of the samples in a second vector

    \param vec: Numeric input vector to search through

    \param threshold: The threshold value

    \param vecout: Output vector - contains a list of positions in input vector which are above threshold

*/
template <class Iter>
HInteger hFindGreaterEqual (const Iter vec , const Iter vec_end, const typename Iter::value_type threshold, const typename vector<HInteger>::iterator vecout, const typename vector<HInteger>::iterator vecout_end)
{
  Iter it=vec;
  typename vector<HInteger>::iterator itout=vecout;
  while (it<vec_end) {
    if (*it >= threshold) {
      if (itout < vecout_end) {
 *itout=(it-vec);
 ++itout;
      };
    };
    ++it;
  };
  return (itout-vecout);
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T > inline HInteger hFindGreaterEqual( std::vector<T> & vec , T threshold , std::vector<HInteger> & vecout) {
return hFindGreaterEqual ( vec.begin(),vec.end() , threshold , vecout.begin(),vecout.end());
}
template < class T > inline HInteger hFindGreaterEqual( casa::Vector<T> & vec , T threshold , casa::Vector<HInteger> & vecout) {
return hFindGreaterEqual ( vec.cbegin(),vec.cend() , threshold , vecout.cbegin(),vecout.cend());
}

HInteger (*fptr_hFindGreaterEqual_HNumber11HIntegerSTDITSTDIT)( std::vector<HNumber> & vec , HNumber threshold , std::vector<HInteger> & vecout) = &hFindGreaterEqual;
HInteger (*fptr_hFindGreaterEqual_HInteger11HIntegerSTDITSTDIT)( std::vector<HInteger> & vec , HInteger threshold , std::vector<HInteger> & vecout) = &hFindGreaterEqual;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//-----------------------------------------------------------------------
/*!
  \brief Find the samples whose absolute values are GreaterEqual a certain threshold value and returns the number of samples found and the positions of the samples in a second vector

    \param vec: Numeric input vector to search through

    \param threshold: The threshold value

    \param vecout: Output vector - contains a list of positions in input vector which are above threshold

*/
template <class Iter>
HInteger hFindGreaterEqualAbs (const Iter vec , const Iter vec_end, const typename Iter::value_type threshold, const typename vector<HInteger>::iterator vecout, const typename vector<HInteger>::iterator vecout_end)
{
  Iter it=vec;
  typename vector<HInteger>::iterator itout=vecout;
  while (it<vec_end) {
    if (abs(*it) >= threshold) {
      if (itout < vecout_end) {
 *itout=(it-vec);
 ++itout;
      };
    };
    ++it;
  };
  return (itout-vecout);
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T > inline HInteger hFindGreaterEqualAbs( std::vector<T> & vec , T threshold , std::vector<HInteger> & vecout) {
return hFindGreaterEqualAbs ( vec.begin(),vec.end() , threshold , vecout.begin(),vecout.end());
}
template < class T > inline HInteger hFindGreaterEqualAbs( casa::Vector<T> & vec , T threshold , casa::Vector<HInteger> & vecout) {
return hFindGreaterEqualAbs ( vec.cbegin(),vec.cend() , threshold , vecout.cbegin(),vecout.cend());
}

HInteger (*fptr_hFindGreaterEqualAbs_HNumber11HIntegerSTDITSTDIT)( std::vector<HNumber> & vec , HNumber threshold , std::vector<HInteger> & vecout) = &hFindGreaterEqualAbs;
HInteger (*fptr_hFindGreaterEqualAbs_HInteger11HIntegerSTDITSTDIT)( std::vector<HInteger> & vec , HInteger threshold , std::vector<HInteger> & vecout) = &hFindGreaterEqualAbs;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//========================================================================
//-----------------------------------------------------------------------
/*!
  \brief Find the samples that are LessThan a certain threshold value and returns the number of samples found and the positions of the samples in a second vector

    \param vec: Numeric input vector to search through

    \param threshold: The threshold value

    \param vecout: Output vector - contains a list of positions in input vector which are above threshold

*/
template <class Iter>
HInteger hFindLessThan (const Iter vec , const Iter vec_end, const typename Iter::value_type threshold, const typename vector<HInteger>::iterator vecout, const typename vector<HInteger>::iterator vecout_end)
{
  Iter it=vec;
  typename vector<HInteger>::iterator itout=vecout;
  while (it<vec_end) {
    if (*it < threshold) {
      if (itout < vecout_end) {
 *itout=(it-vec);
 ++itout;
      };
    };
    ++it;
  };
  return (itout-vecout);
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T > inline HInteger hFindLessThan( std::vector<T> & vec , T threshold , std::vector<HInteger> & vecout) {
return hFindLessThan ( vec.begin(),vec.end() , threshold , vecout.begin(),vecout.end());
}
template < class T > inline HInteger hFindLessThan( casa::Vector<T> & vec , T threshold , casa::Vector<HInteger> & vecout) {
return hFindLessThan ( vec.cbegin(),vec.cend() , threshold , vecout.cbegin(),vecout.cend());
}

HInteger (*fptr_hFindLessThan_HNumber11HIntegerSTDITSTDIT)( std::vector<HNumber> & vec , HNumber threshold , std::vector<HInteger> & vecout) = &hFindLessThan;
HInteger (*fptr_hFindLessThan_HInteger11HIntegerSTDITSTDIT)( std::vector<HInteger> & vec , HInteger threshold , std::vector<HInteger> & vecout) = &hFindLessThan;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//-----------------------------------------------------------------------
/*!
  \brief Find the samples whose absolute values are LessThan a certain threshold value and returns the number of samples found and the positions of the samples in a second vector

    \param vec: Numeric input vector to search through

    \param threshold: The threshold value

    \param vecout: Output vector - contains a list of positions in input vector which are above threshold

*/
template <class Iter>
HInteger hFindLessThanAbs (const Iter vec , const Iter vec_end, const typename Iter::value_type threshold, const typename vector<HInteger>::iterator vecout, const typename vector<HInteger>::iterator vecout_end)
{
  Iter it=vec;
  typename vector<HInteger>::iterator itout=vecout;
  while (it<vec_end) {
    if (abs(*it) < threshold) {
      if (itout < vecout_end) {
 *itout=(it-vec);
 ++itout;
      };
    };
    ++it;
  };
  return (itout-vecout);
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T > inline HInteger hFindLessThanAbs( std::vector<T> & vec , T threshold , std::vector<HInteger> & vecout) {
return hFindLessThanAbs ( vec.begin(),vec.end() , threshold , vecout.begin(),vecout.end());
}
template < class T > inline HInteger hFindLessThanAbs( casa::Vector<T> & vec , T threshold , casa::Vector<HInteger> & vecout) {
return hFindLessThanAbs ( vec.cbegin(),vec.cend() , threshold , vecout.cbegin(),vecout.cend());
}

HInteger (*fptr_hFindLessThanAbs_HNumber11HIntegerSTDITSTDIT)( std::vector<HNumber> & vec , HNumber threshold , std::vector<HInteger> & vecout) = &hFindLessThanAbs;
HInteger (*fptr_hFindLessThanAbs_HInteger11HIntegerSTDITSTDIT)( std::vector<HInteger> & vec , HInteger threshold , std::vector<HInteger> & vecout) = &hFindLessThanAbs;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//$ENDITERATE
//-----------------------------------------------------------------------
/*!
  \brief Downsample the input vector to a smaller output vector.

    \param vec1: Numeric input vector

    \param vec2: Numeric output vector


 Downsample the input vector to a smaller output vector, by replacing
     subsequent blocks of values by their mean value. The block size
     is automatically chosen such that the input vector fits exactly
     into the output vector. All blocks have the same length with a
     possible exception of the last block.
*/
template <class Iter>
void hDownsample (const Iter vec1,
    const Iter vec1_end,
    const Iter vec2,
    const Iter vec2_end)
{
  if (vec2>=vec2_end) return; //If size 0 do nothing
  if (vec1>=vec1_end) return; //If size 0 do nothing
  HInteger ilen=(vec1_end-vec1);
  HInteger olen=(vec2_end-vec2);
  HInteger blen=max(ilen/(olen-1),1);
  //use max to avoid infinite loops if output vector is too large
  Iter it2,it1=vec1;
  Iter ito=vec2,ito_end=vec2_end-1;
  //only produce the first N-1 blocks in the output vector
  while ((it1<vec1_end) && (ito<ito_end)) {
    it2=min(it1+blen,vec1_end);
    *ito=hMean(it1,it2);
    it1=it2;
    ++ito;
    }
  *ito=hMean(it2,vec1_end);
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T > inline void hDownsample( std::vector<T> & vec1 , std::vector<T> & vec2) {
hDownsample ( vec1.begin(),vec1.end() , vec2.begin(),vec2.end());
}
template < class T > inline void hDownsample( casa::Vector<T> & vec1 , casa::Vector<T> & vec2) {
hDownsample ( vec1.cbegin(),vec1.cend() , vec2.cbegin(),vec2.cend());
}

void (*fptr_hDownsample_HNumber11STDITSTDIT)( std::vector<HNumber> & vec1 , std::vector<HNumber> & vec2) = &hDownsample;
void (*fptr_hDownsample_HInteger11STDITSTDIT)( std::vector<HInteger> & vec1 , std::vector<HInteger> & vec2) = &hDownsample;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//-----------------------------------------------------------------------
/*!
  \brief Downsample the input vector by a cetain factor and return a new vector

    \param vec: Numeric input vector

    \param downsample_factor: Factor by which to reduce original size


 Downsample the input vector to a new smaller output vector, by replacing
     subsequent blocks of values by their mean value. The block size
     is automatically chosen such that the input vector fits exactly
     into the output vector. All blocks have the same length with a
     possible exception of the last block.
*/
template <class T>
std::vector<T> hDownsample (
    std::vector<T> & vec,
    HNumber downsample_factor
    )
{
  std::vector<T> newvec(floor(vec.size()/downsample_factor+0.5));
  hDownsample(vec,newvec);
  return newvec;
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers

std::vector<HNumber> (*fptr_hDownsample_HNumber1HNumberSTL)( std::vector<HNumber> & vec , HNumber downsample_factor) = &hDownsample;
std::vector<HInteger> (*fptr_hDownsample_HInteger1HNumberSTL)( std::vector<HInteger> & vec , HNumber downsample_factor) = &hDownsample;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//-----------------------------------------------------------------------
/*!

  \brief Finds the location (i.e., returns integer) in a monotonically increasing vector, where the input search value is just above or equal to the value in the vector.

    \param vec: Sorted numeric input vector

    \param value: value to search for


Finds -- through a binary search and interpolation -- the location in
  a monotonically increasing vector, where the search value is just
  above or equal to the value in the vector.

This requires random access iterators, in order to have an optimal search result.

*/
template <class Iter>
HInteger hFindLowerBound (const Iter vec,
    const Iter vec_end,
    const typename Iter::value_type value)
//iterator_traits<Iter>::value_type value)
{
  HNumber value_n=hfcast<HNumber>(value); //This also works for Complex then
  HInteger niter=0;
  if (vec==vec_end) return 0;
  HInteger maxpos=vec_end-vec-1,posguess;
  Iter it1=vec,it2=vec_end-1,it0;
  if (value_n<=hfcast<HNumber>(*it1)) return 0;
  if (value_n>=hfcast<HNumber>(*it2)) return maxpos;
  posguess=(value_n-hfcast<HNumber>(*it1))/(hfcast<HNumber>(*it2)-hfcast<HNumber>(*it1))*maxpos;
  it0=vec+posguess;
  if (it0<it1) return hfcast<HInteger>(it1-vec); //Error, Non monotonic
  if (it0>=it2) return hfcast<HInteger>(it2-vec); //Error, Non monotonic
  //  cout << "hFindLowerBound(" << value_n << "): niter=" << niter << ", posguess=" << posguess << ", val=" << *it0 << " vals=(" << hfcast<HNumber>(*(it0)) << ", " << hfcast<HNumber>(*(it0+1)) << "), bracket=(" << it1-vec << ", " << it2-vec <<")" <<endl;
  while (!((value_n < hfcast<HNumber>(*(it0+1))) && (value_n >= hfcast<HNumber>(*it0)))) {
    if (value_n > hfcast<HNumber>(*it0)) {
      it1=it0;
    } else {
      it2=it0;
    };
    it0=it1+(it2-it1)/2;
    if (*it0>value_n) it2=it0; //Binary search step
    else it1=it0;
    posguess=(value_n-hfcast<HNumber>(*it1))/(hfcast<HNumber>(*it2)-hfcast<HNumber>(*it1))*(it2-it1)+(it1-vec);
    it0=vec+posguess;
    ++niter;
    //cout << "hFindLowerBound(" << value_n << "): niter=" << niter << ", posguess=" << posguess << ", val=" << *it0 << " vals=(" << hfcast<HNumber>(*(it0)) << ", " << hfcast<HNumber>(*(it0+1)) << "), bracket=(" << it1-vec << ", " << it2-vec <<")" <<endl;
    if (it0<it1) return it1-vec; //Error, Non monotonic
    if (it0>it2) return it2-vec; //Error, Non monotonic
  };
  return posguess;
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T > inline HInteger hFindLowerBound( std::vector<T> & vec , T value) {
return hFindLowerBound ( vec.begin(),vec.end() , value);
}
template < class T > inline HInteger hFindLowerBound( casa::Vector<T> & vec , T value) {
return hFindLowerBound ( vec.cbegin(),vec.cend() , value);
}

HInteger (*fptr_hFindLowerBound_HComplex11STDIT)( std::vector<HComplex> & vec , HComplex value) = &hFindLowerBound;
HInteger (*fptr_hFindLowerBound_HNumber11STDIT)( std::vector<HNumber> & vec , HNumber value) = &hFindLowerBound;
HInteger (*fptr_hFindLowerBound_HInteger11STDIT)( std::vector<HInteger> & vec , HInteger value) = &hFindLowerBound;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
/*
//Wrapper for c-style arrays
HInteger hFindLowerBound(const HNumber* vec,
			 const HInteger len,
			 const HNumber value)
{
  return hFindLowerBound(vec,vec+len,value);
}
*/
//-----------------------------------------------------------------------
/*!
  \brief Returns vector of weights of length len with constant weights normalized to give a sum of unity. Can be used by hRunningAverageT.

    \param wlen: Lengths of weights vector

*/
 //template <class T>
std::vector<HNumber> hFlatWeights (HInteger wlen) {
  std::vector<HNumber> weights(wlen,1.0/wlen);
  return weights;
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers

std::vector<HNumber> (*fptr_hFlatWeights_HIntegerHInteger)( HInteger wlen) = &hFlatWeights;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//-----------------------------------------------------------------------
/*!
  \brief Returns vector of weights of length wlen with linearly rising and decreasing weights centered at len/2.

    \param wlen: Lengths of weights vector


The vector is normalized to give a sum of unity. Can be used by
hRunningAverage.
*/
 //template <class T>
std::vector<HNumber> hLinearWeights (HInteger wlen) {
  std::vector<HNumber> weights(wlen,0.0);
  HInteger i,middle=wlen/2;
  HNumber f,sum=0.0;
  for (i=0; i<wlen; i++) {
    f=1.0-abs(middle-i)/(middle+1.0);
    weights[i]=f;
    sum+=f;
  };
  //Normalize to unity
  for (i=0; i<wlen; i++) weights[i]=weights[i]/sum;
  return weights;
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers

std::vector<HNumber> (*fptr_hLinearWeights_HIntegerHInteger)( HInteger wlen) = &hLinearWeights;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//-----------------------------------------------------------------------
/*!
  \brief Returns vector of weights of length wlen with Gaussian distribution centered at len/2 and sigma=len/4 (i.e. the Gaussian extends over 2 sigma in both directions).

    \param wlen: Lengths of weights vector

*/
 //template <class T>
std::vector<HNumber> hGaussianWeights (HInteger wlen) {
  vector<HNumber> weights(wlen,0.0);
  HInteger i,middle=wlen/2;
  HNumber f,sum=0.0;
  for (i=0; i<wlen; i++) {
    f=funcGaussian(i,max(wlen/4.0,1.0),middle);
    weights[i]=f;
    sum+=f;
  };
  //Normalize to unity
  for (i=0; i<wlen; i++) weights[i]=weights[i]/sum;
  return weights;
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers

std::vector<HNumber> (*fptr_hGaussianWeights_HIntegerHInteger)( HInteger wlen) = &hGaussianWeights;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
/*!
  \brief Create a normalized weight vector.

    \param wlen: Length of weight vector

    \param wtype: Type of weight vector

*/
vector<HNumber> hWeights(HInteger wlen, hWEIGHTS wtype){
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
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers

std::vector<HNumber> (*fptr_hWeights_HIntegerHIntegerhWEIGHTS)( HInteger wlen , hWEIGHTS wtype) = &hWeights;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
/*!
  \brief Calculate the running average of an input vector using a weight vector.

    \param idata: Input vector

    \param odata: Output vector

    \param weights: Weight vector

*/
template <class DataIter, class NumVecIter>
void hRunningAverage (const DataIter idata,
       const DataIter idata_end,
       const DataIter odata,
       const DataIter odata_end,
       const NumVecIter weights,
       const NumVecIter weights_end)
{
  HInteger l=(weights_end-weights);
  /* Index of the central element of the weights vector (i.e., where it
     typically would peak) */
  HInteger middle=l/2;
  /* To avoid too many rounding errors with Integers */
  typename DataIter::value_type fac = l*10;
  DataIter dit;
  DataIter dit2;
  DataIter din(idata);
  DataIter dout(odata);
  NumVecIter wit;
  while (din<idata_end && dout<odata_end) {
    dit=din-middle; //data iterator set to the first element to be taken into account (left from current element)
    wit=weights; // weight iterators set to beginning of weights
    while (wit<weights_end) {
      if (dit<idata) dit2=idata;
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
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
 inline void hRunningAverage( std::vector<HNumber> & idata , std::vector<HNumber> & odata , std::vector<HNumber> & weights) {
hRunningAverage ( idata.begin(),idata.end() , odata.begin(),odata.end() , weights.begin(),weights.end());
}

void (*fptr_hRunningAverage_HIntegerHNumberHNumberHNumberSTDITSTDITSTDIT)( std::vector<HNumber> & idata , std::vector<HNumber> & odata , std::vector<HNumber> & weights) = &hRunningAverage;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//-----------------------------------------------------------------------
/*!
  \brief Overloaded function to automatically calculate weights.

    \param idata: Input vector

    \param odata: Output vector

    \param wlen: Length of weight vector

    \param wtype: Type of weight vector

*/
template <class DataIter>
void hRunningAverage (const DataIter idata,
       const DataIter idata_end,
       const DataIter odata,
       const DataIter odata_end,
       HInteger wlen,
       hWEIGHTS wtype)
{
  vector<HNumber> weights = hWeights(wlen, wtype);
  hRunningAverage<DataIter, vector<HNumber>::iterator> (idata,
       idata_end,
       odata,
       odata_end,
       weights.begin(),
       weights.end());
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
 inline void hRunningAverage( std::vector<HNumber> & idata , std::vector<HNumber> & odata , HInteger wlen , hWEIGHTS wtype) {
hRunningAverage ( idata.begin(),idata.end() , odata.begin(),odata.end() , wlen , wtype);
}

void (*fptr_hRunningAverage_HIntegerHNumberHNumberHIntegerhWEIGHTSSTDITSTDIT)( std::vector<HNumber> & idata , std::vector<HNumber> & odata , HInteger wlen , hWEIGHTS wtype) = &hRunningAverage;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//========================================================================
//                     RF (Radio Frequency) Functions
//========================================================================
//-----------------------------------------------------------------------
/*!
  \brief Calculates the time delay in seconds for a signal received at an antenna position relative to a phase center from a source located in a certain direction in farfield (based on L. Bahren)

    \param antPosition: Cartesian antenna positions 

    \param skyDirection: Vector in Cartesian coordinates pointing towards a sky position from the antenna - vector of length 3

    \param length: Length of the skyDirection vector, used for normalization - provided to speed up calculation

*/
template <class Iter>
HNumber hGeometricDelayFarField (
   const Iter antPosition,
   const Iter skyDirection,
   HNumber length
   )
{
  return - (*skyDirection * *antPosition
     + *(skyDirection+1) * *(antPosition+1)
     + *(skyDirection+2) * *(antPosition+2))/length/CR::lightspeed;
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
 inline HNumber hGeometricDelayFarField( std::vector<HNumber> & antPosition , std::vector<HNumber> & skyDirection , HNumber length) {
return hGeometricDelayFarField ( antPosition.begin() , skyDirection.begin() , length);
}
inline HNumber hGeometricDelayFarField( casa::Vector<HNumber> & antPosition , casa::Vector<HNumber> & skyDirection , HNumber length) {
return hGeometricDelayFarField ( antPosition.cbegin() , skyDirection.cbegin() , length);
}

HNumber (*fptr_hGeometricDelayFarField_HIntegerHNumberHNumberHNumberSTDITFIXEDSTDITFIXED)( std::vector<HNumber> & antPosition , std::vector<HNumber> & skyDirection , HNumber length) = &hGeometricDelayFarField;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//-----------------------------------------------------------------------
/*!
  \brief Calculates the time delay in seconds for a signal received at an antenna position relative to a phase center from a source located at a certain 3D space coordinate in nearfield (based on L. Bahren)

    \param antPosition: Cartesian antenna positions 

    \param skyPosition: Vector in Cartesian coordinates 

    \param distance: Distance of source, i.e. the length of skyPosition - provided to speed up calculation

*/
template <class Iter>
HNumber hGeometricDelayNearField (
   const Iter antPosition,
   const Iter skyPosition,
   const HNumber distance)
{
  return (
   sqrt(
        square(*skyPosition - *antPosition)
        +square(*(skyPosition+1) - *(antPosition+1))
        +square(*(skyPosition+2) - *(antPosition+2))
        ) - distance
   )/CR::lightspeed;
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
 inline HNumber hGeometricDelayNearField( std::vector<HNumber> & antPosition , std::vector<HNumber> & skyPosition , HNumber distance) {
return hGeometricDelayNearField ( antPosition.begin() , skyPosition.begin() , distance);
}
inline HNumber hGeometricDelayNearField( casa::Vector<HNumber> & antPosition , casa::Vector<HNumber> & skyPosition , HNumber distance) {
return hGeometricDelayNearField ( antPosition.cbegin() , skyPosition.cbegin() , distance);
}

HNumber (*fptr_hGeometricDelayNearField_HIntegerHNumberHNumberHNumberSTDITFIXEDSTDITFIXED)( std::vector<HNumber> & antPosition , std::vector<HNumber> & skyPosition , HNumber distance) = &hGeometricDelayNearField;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//-----------------------------------------------------------------------
/*!
  \brief Calculates the time delay in seconds for signals received at various antenna positions relative to a phase center from sources located at certain 3D space coordinates in near or far field

    \param antPositions: Cartesian antenna positions 

    \param skyPositions: Vector in Cartesian coordinates 

    \param delays: Output vector containing the delays in seconds for all antennas and positions [antenna index runs fastest: 

    \param farfield: Calculate in farfield approximation if true, otherwise do near field calculation

*/
template <class Iter>
void hGeometricDelays (
   const Iter antPositions,
   const Iter antPositions_end,
   const Iter skyPositions,
   const Iter skyPositions_end,
   const Iter delays,
   const Iter delays_end,
   const bool farfield
   )
{
  HNumber distance;
  Iter
    ant,
    sky=skyPositions,
    del=delays,
    ant_end=antPositions_end-2,
    sky_end=skyPositions_end-2;
  if (farfield) {
    while (sky < sky_end && del < delays_end) {
      distance = hNorm(sky,sky+3);
      ant=antPositions;
      while (ant < ant_end && del < delays_end) {
 *del=hGeometricDelayFarField(ant,sky,distance);
 ant+=3; ++del;
      };
      sky+=3;
    };
  } else {
    while (sky < sky_end && del < delays_end) {
      distance = hNorm(sky,sky+3); //distance from phase center
      ant=antPositions;
      while (ant < ant_end && del < delays_end) {
 *del=hGeometricDelayNearField(ant,sky,distance);
 ant+=3; ++del;
      };
      sky+=3;
    };
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
 inline void hGeometricDelays( std::vector<HNumber> & antPositions , std::vector<HNumber> & skyPositions , std::vector<HNumber> & delays , bool farfield) {
hGeometricDelays ( antPositions.begin(),antPositions.end() , skyPositions.begin(),skyPositions.end() , delays.begin(),delays.end() , farfield);
}
inline void hGeometricDelays( casa::Vector<HNumber> & antPositions , casa::Vector<HNumber> & skyPositions , casa::Vector<HNumber> & delays , bool farfield) {
hGeometricDelays ( antPositions.cbegin(),antPositions.cend() , skyPositions.cbegin(),skyPositions.cend() , delays.cbegin(),delays.cend() , farfield);
}

void (*fptr_hGeometricDelays_HIntegerHNumberHNumberHNumberboolSTDITSTDITSTDIT)( std::vector<HNumber> & antPositions , std::vector<HNumber> & skyPositions , std::vector<HNumber> & delays , bool farfield) = &hGeometricDelays;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//-----------------------------------------------------------------------
/*!
  \brief Calculates the phase gradients for signals received at various antenna positions relative to a phase center from sources located at certain 3D space coordinates in near or far field and for different frequencies.

    \param frequencies: Vector of frequencies 

    \param antPositions: Cartesian antenna positions 

    \param skyPositions: Vector in Cartesian coordinates 

    \param phases: Output vector containing the phases in radians for all frequencies, antennas and positions [frequency index, runs fastest, then antenna index: 

    \param farfield: Calculate in farfield approximation if true, otherwise do near field calculation

*/
template <class Iter>
void hGeometricPhases (
   const Iter frequencies,
   const Iter frequencies_end,
   const Iter antPositions,
   const Iter antPositions_end,
   const Iter skyPositions,
   const Iter skyPositions_end,
   const Iter phases,
   const Iter phases_end,
   const bool farfield
   )
{
  HNumber distance;
  Iter
    ant,
    freq,
    sky=skyPositions,
    phase=phases,
    ant_end=antPositions_end-2,
    sky_end=skyPositions_end-2;
  if (farfield) {
    while (sky < sky_end && phase < phases_end) {
      distance = hNorm(sky,sky+3);
      ant=antPositions;
      while (ant < ant_end && phase < phases_end) {
 freq=frequencies;
 while (freq < frequencies_end && phase < phases_end) {
   *phase=hPhase(*freq,hGeometricDelayFarField(ant,sky,distance));
   ++phase; ++freq;
 };
 ant+=3;
      };
      sky+=3;
    };
  } else {
    while (sky < sky_end && phase < phases_end) {
      distance = hNorm(sky,sky+3);
      ant=antPositions;
      while (ant < ant_end && phase < phases_end) {
 freq=frequencies;
 while (freq < frequencies_end && phase < phases_end) {
   *phase=hPhase(*freq,hGeometricDelayNearField(ant,sky,distance));
   ++phase; ++freq;
 };
 ant+=3;
      };
      sky+=3;
    };
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
 inline void hGeometricPhases( std::vector<HNumber> & frequencies , std::vector<HNumber> & antPositions , std::vector<HNumber> & skyPositions , std::vector<HNumber> & phases , bool farfield) {
hGeometricPhases ( frequencies.begin(),frequencies.end() , antPositions.begin(),antPositions.end() , skyPositions.begin(),skyPositions.end() , phases.begin(),phases.end() , farfield);
}
inline void hGeometricPhases( casa::Vector<HNumber> & frequencies , casa::Vector<HNumber> & antPositions , casa::Vector<HNumber> & skyPositions , casa::Vector<HNumber> & phases , bool farfield) {
hGeometricPhases ( frequencies.cbegin(),frequencies.cend() , antPositions.cbegin(),antPositions.cend() , skyPositions.cbegin(),skyPositions.cend() , phases.cbegin(),phases.cend() , farfield);
}

void (*fptr_hGeometricPhases_HIntegerHNumberHNumberHNumberHNumberboolSTDITSTDITSTDITSTDIT)( std::vector<HNumber> & frequencies , std::vector<HNumber> & antPositions , std::vector<HNumber> & skyPositions , std::vector<HNumber> & phases , bool farfield) = &hGeometricPhases;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//-----------------------------------------------------------------------
/*!
  \brief Calculates the phase gradients as complex weights for signals received at various antenna positions relative to a phase center from sources located at certain 3D space coordinates in near or far field and for different frequencies.

    \param frequencies: Vector of frequencies 

    \param antPositions: Cartesian antenna positions 

    \param skyPositions: Vector in Cartesian coordinates 

    \param weights: Output vector containing the phases in radians for all frequencies, antennas and positions [frequency index, runs fastest, then antenna index: 

    \param farfield: Calculate in farfield approximation if true, otherwise do near field calculation

*/
template <class Iter, class CIter>
void hGeometricWeights (
   const Iter frequencies,
   const Iter frequencies_end,
   const Iter antPositions,
   const Iter antPositions_end,
   const Iter skyPositions,
   const Iter skyPositions_end,
   const CIter weights,
   const CIter weights_end,
   const bool farfield
   )
{
  HNumber distance;
  Iter
    ant,
    freq,
    sky=skyPositions,
    ant_end=antPositions_end-2,
    sky_end=skyPositions_end-2;
  CIter weight=weights;
  if (farfield) {
    while (sky < sky_end && weight < weights_end) {
      distance = hNorm(sky,sky+3);
      ant=antPositions;
      while (ant < ant_end && weight < weights_end) {
 freq=frequencies;
 while (freq < frequencies_end && weight < weights_end) {
   *weight=exp(HComplex(0.0,hPhase(*freq,hGeometricDelayFarField(ant,sky,distance))));
   ++weight; ++freq;
 };
 ant+=3;
      };
      sky+=3;
    };
  } else {
    while (sky < sky_end && weight < weights_end) {
      distance = hNorm(sky,sky+3);
      ant=antPositions;
      while (ant < ant_end && weight < weights_end) {
 freq=frequencies;
 while (freq < frequencies_end && weight < weights_end) {
   *weight=exp(HComplex(0.0,hPhase(*freq,hGeometricDelayNearField(ant,sky,distance))));
   ++weight; ++freq;
 };
 ant+=3;
      };
      sky+=3;
    };
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
 inline void hGeometricWeights( std::vector<HNumber> & frequencies , std::vector<HNumber> & antPositions , std::vector<HNumber> & skyPositions , std::vector<HComplex> & weights , bool farfield) {
hGeometricWeights ( frequencies.begin(),frequencies.end() , antPositions.begin(),antPositions.end() , skyPositions.begin(),skyPositions.end() , weights.begin(),weights.end() , farfield);
}
inline void hGeometricWeights( casa::Vector<HNumber> & frequencies , casa::Vector<HNumber> & antPositions , casa::Vector<HNumber> & skyPositions , casa::Vector<HComplex> & weights , bool farfield) {
hGeometricWeights ( frequencies.cbegin(),frequencies.cend() , antPositions.cbegin(),antPositions.cend() , skyPositions.cbegin(),skyPositions.cend() , weights.cbegin(),weights.cend() , farfield);
}

void (*fptr_hGeometricWeights_HIntegerHNumberHNumberHNumberHComplexboolSTDITSTDITSTDITSTDIT)( std::vector<HNumber> & frequencies , std::vector<HNumber> & antPositions , std::vector<HNumber> & skyPositions , std::vector<HComplex> & weights , bool farfield) = &hGeometricWeights;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//-----------------------------------------------------------------------
/*!

  \brief Calculates the power of a complex spectrum and add it to an output vector.

  The fact that the result is added to the output vector allows one to
  call the function multiple times and get a summed spectrum. If you
  need it only once, just fill the vector with zeros.

  \param vec: STL Iterator pointing to the first element of an array with
         input values.
  \param vec_end: STL Iterator pointing to the end of the input vector

  \param out: STL Iterator pointing to the first element of an array with
         output values.
  \param out_end: STL Iterator pointing to the end of the output vector
*/
template <class Iterin, class Iter>
void hSpectralPower(const Iterin vec,const Iterin vec_end, const Iter out,const Iter out_end)
{
  typedef typename Iter::value_type T;
  Iterin it(vec);
  Iter itout(out);
  while ((it!=vec_end) && (itout !=out_end)) {
    *itout+=real((*it)*conj(*it));
    ++it; ++itout;
  };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
 inline void hSpectralPower( std::vector<HComplex> & vec , std::vector<HNumber> & outvec) {
hSpectralPower ( vec.begin(),vec.end() , outvec.begin(),outvec.end());
}

void (*fptr_hSpectralPower_HIntegerHComplexHNumberSTDITSTDIT)( std::vector<HComplex> & vec , std::vector<HNumber> & outvec) = &hSpectralPower;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
/*
  \brief Convert the ADC value to a voltage

    \param vec: Numeric input and output vector

    \param adc2voltage: Scaling factor if the gain

*/
template <class Iter>
void hADC2Voltage(const Iter vec, const Iter vec_end, const HNumber adc2voltage) {
  Iter it = vec;
  while(it != vec_end) {
    *it *= adc2voltage;
    it++;
  }
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T > inline void hADC2Voltage( std::vector<T> & vec , HNumber adc2voltage) {
hADC2Voltage ( vec.begin(),vec.end() , adc2voltage);
}

void (*fptr_hADC2Voltage_HComplex1HNumberSTDIT)( std::vector<HComplex> & vec , HNumber adc2voltage) = &hADC2Voltage;
void (*fptr_hADC2Voltage_HNumber1HNumberSTDIT)( std::vector<HNumber> & vec , HNumber adc2voltage) = &hADC2Voltage;
void (*fptr_hADC2Voltage_HInteger1HNumberSTDIT)( std::vector<HInteger> & vec , HNumber adc2voltage) = &hADC2Voltage;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
/*
  \brief Create a Hanning filter.

    \param vec: Return vector containing Hanning filter

    \param Alpha: Height parameter of Hanning function

    \param Beta: Width parameter of Hanning function

    \param BetaRise: Rising slope parameter of Hanning function

    \param BetaFall: Falling slope parameter of Hanning function

*/
template <class Iter>
void hGetHanningFilter(const Iter vec, const Iter vec_end,
         const double Alpha,
         const uint Beta,
         const uint BetaRise,
         const uint BetaFall) {
  uint blocksize = vec_end - vec;
  CR::HanningFilter<HNumber> hanning_filter(blocksize, Alpha, Beta, BetaRise, BetaFall);
  Iter it_v = vec;
  casa::Vector<HNumber> filter = hanning_filter.weights();
  casa::Vector<HNumber>::iterator it_f = filter.begin();
  while ((it_v != vec_end) && (it_f != filter.end())) {
    *it_v = (typename Iter::value_type) *it_f;
    it_v++; it_f++;
  }
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T > inline void hGetHanningFilter( std::vector<T> & vec , HNumber Alpha , uint Beta , uint BetaRise , uint BetaFall) {
hGetHanningFilter ( vec.begin(),vec.end() , Alpha , Beta , BetaRise , BetaFall);
}
template < class T > inline void hGetHanningFilter( casa::Vector<T> & vec , HNumber Alpha , uint Beta , uint BetaRise , uint BetaFall) {
hGetHanningFilter ( vec.cbegin(),vec.cend() , Alpha , Beta , BetaRise , BetaFall);
}

void (*fptr_hGetHanningFilter_HComplex1HNumberuintuintuintSTDIT)( std::vector<HComplex> & vec , HNumber Alpha , uint Beta , uint BetaRise , uint BetaFall) = &hGetHanningFilter;
void (*fptr_hGetHanningFilter_HNumber1HNumberuintuintuintSTDIT)( std::vector<HNumber> & vec , HNumber Alpha , uint Beta , uint BetaRise , uint BetaFall) = &hGetHanningFilter;
void (*fptr_hGetHanningFilter_HInteger1HNumberuintuintuintSTDIT)( std::vector<HInteger> & vec , HNumber Alpha , uint Beta , uint BetaRise , uint BetaFall) = &hGetHanningFilter;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
/*
  \brief Create a Hanning filter.

    \param vec: Return vector containing Hanning filter

    \param Alpha: Height parameter of Hanning function

    \param Beta: Width parameter of Hanning function

*/
template <class Iter>
void hGetHanningFilter(const Iter vec, const Iter vec_end,
         const double Alpha,
         const uint Beta) {
  uint blocksize = vec_end - vec;
  uint BetaRise = (blocksize - Beta)/2;
  uint BetaFall = (blocksize - Beta)/2;
  hGetHanningFilter(vec, vec_end, Alpha, Beta, BetaRise, BetaFall);
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T > inline void hGetHanningFilter( std::vector<T> & vec , HNumber Alpha , uint Beta) {
hGetHanningFilter ( vec.begin(),vec.end() , Alpha , Beta);
}
template < class T > inline void hGetHanningFilter( casa::Vector<T> & vec , HNumber Alpha , uint Beta) {
hGetHanningFilter ( vec.cbegin(),vec.cend() , Alpha , Beta);
}

void (*fptr_hGetHanningFilter_HComplex1HNumberuintSTDIT)( std::vector<HComplex> & vec , HNumber Alpha , uint Beta) = &hGetHanningFilter;
void (*fptr_hGetHanningFilter_HNumber1HNumberuintSTDIT)( std::vector<HNumber> & vec , HNumber Alpha , uint Beta) = &hGetHanningFilter;
void (*fptr_hGetHanningFilter_HInteger1HNumberuintSTDIT)( std::vector<HInteger> & vec , HNumber Alpha , uint Beta) = &hGetHanningFilter;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
/*
  \brief Create a Hanning filter.

    \param vec: Return vector containing Hanning filter

    \param Alpha: Height parameter of Hanning function

*/
template <class Iter>
void hGetHanningFilter(const Iter vec, const Iter vec_end,
         const double Alpha) {
  uint Beta = 0;
  hGetHanningFilter(vec, vec_end, Alpha, Beta);
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T > inline void hGetHanningFilter( std::vector<T> & vec , HNumber Alpha) {
hGetHanningFilter ( vec.begin(),vec.end() , Alpha);
}
template < class T > inline void hGetHanningFilter( casa::Vector<T> & vec , HNumber Alpha) {
hGetHanningFilter ( vec.cbegin(),vec.cend() , Alpha);
}

void (*fptr_hGetHanningFilter_HComplex1HNumberSTDIT)( std::vector<HComplex> & vec , HNumber Alpha) = &hGetHanningFilter;
void (*fptr_hGetHanningFilter_HNumber1HNumberSTDIT)( std::vector<HNumber> & vec , HNumber Alpha) = &hGetHanningFilter;
void (*fptr_hGetHanningFilter_HInteger1HNumberSTDIT)( std::vector<HInteger> & vec , HNumber Alpha) = &hGetHanningFilter;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
/*
  \brief Create a Hanning filter.

    \param vec: Return vector containing Hanning filter

*/
template <class Iter>
void hGetHanningFilter(const Iter vec, const Iter vec_end){
  double Alpha = 0.5;
  hGetHanningFilter(vec, vec_end, Alpha);
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T > inline void hGetHanningFilter( std::vector<T> & vec) {
hGetHanningFilter ( vec.begin(),vec.end());
}
template < class T > inline void hGetHanningFilter( casa::Vector<T> & vec) {
hGetHanningFilter ( vec.cbegin(),vec.cend());
}

void (*fptr_hGetHanningFilter_HComplex1STDIT)( std::vector<HComplex> & vec) = &hGetHanningFilter;
void (*fptr_hGetHanningFilter_HNumber1STDIT)( std::vector<HNumber> & vec) = &hGetHanningFilter;
void (*fptr_hGetHanningFilter_HInteger1STDIT)( std::vector<HInteger> & vec) = &hGetHanningFilter;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
/*
  \brief Apply a predefined filter on a vector.

    \param data: Vector containing the data on which the filter will be applied.

    \param filter: Vector containing the filter.

*/
template <class Iter, class IterFilter>
void hApplyFilter(const Iter data, const Iter data_end, const IterFilter filter, IterFilter filter_end){
  Iter it_d = data;
  IterFilter it_f = filter;
  while ((it_d != data_end) && (it_f != filter_end)) {
    *it_d *= (typename Iter::value_type) *it_f;
    it_d++; it_f++;
  }
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T > inline void hApplyFilter( std::vector<T> & data , std::vector<T> & filter) {
hApplyFilter ( data.begin(),data.end() , filter.begin(),filter.end());
}
template < class T > inline void hApplyFilter( casa::Vector<T> & data , casa::Vector<T> & filter) {
hApplyFilter ( data.cbegin(),data.cend() , filter.cbegin(),filter.cend());
}

void (*fptr_hApplyFilter_HComplex11STDITSTDIT)( std::vector<HComplex> & data , std::vector<HComplex> & filter) = &hApplyFilter;
void (*fptr_hApplyFilter_HNumber11STDITSTDIT)( std::vector<HNumber> & data , std::vector<HNumber> & filter) = &hApplyFilter;
void (*fptr_hApplyFilter_HInteger11STDITSTDIT)( std::vector<HInteger> & data , std::vector<HInteger> & filter) = &hApplyFilter;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
/*
  \brief Apply a Hanning filter on a vector.

    \param data: Input and return vector containing the data on which the Hanning filter will be applied.

*/
template <class Iter>
void hApplyHanningFilter(const Iter data, const Iter data_end){
  uint blocksize = data_end - data;
  vector<HNumber> filter(blocksize);
  hGetHanningFilter(filter.begin(), filter.end());
  hApplyFilter(data, data_end, filter.begin(), filter.end());
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T > inline void hApplyHanningFilter( std::vector<T> & data) {
hApplyHanningFilter ( data.begin(),data.end());
}
template < class T > inline void hApplyHanningFilter( casa::Vector<T> & data) {
hApplyHanningFilter ( data.cbegin(),data.cend());
}

void (*fptr_hApplyHanningFilter_HComplex1STDIT)( std::vector<HComplex> & data) = &hApplyHanningFilter;
void (*fptr_hApplyHanningFilter_HNumber1STDIT)( std::vector<HNumber> & data) = &hApplyHanningFilter;
void (*fptr_hApplyHanningFilter_HInteger1STDIT)( std::vector<HInteger> & data) = &hApplyHanningFilter;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
/*
  \brief Apply an FFT on a vector

    \param data_in: Vector containing the data on which the FFT will be applied.

    \param data_out: Return vector in which the FFT transformed data is stored.

    \param nyquistZone: Nyquist zone

*/
template <class IterIn, class IterOut>
void hFFT(const IterIn data_in, const IterIn data_in_end,
      const IterOut data_out, const IterOut data_out_end,
      const HInteger nyquistZone) {
  uint channel;
  uint blocksize(data_in_end - data_in);
  uint fftLength(blocksize/2+1);
  uint nofChannels(fftLength);
  IPosition shape_in(1,blocksize);
  IPosition shape_out(1,fftLength);
  FFTServer<Double,DComplex> fftserver(shape_in, FFTEnums::REALTOCOMPLEX);
  Vector<Double> cvec_in(shape_in, reinterpret_cast<Double*>(&(*data_in)), casa::SHARE);
  Vector<DComplex> cvec_out(shape_out, 0.);
  IterOut it_out;
  // Apply FFT
  fftserver.fft(cvec_out, cvec_in);
  // Is there some aftercare needed (checking/setting the size of the in/output vector)
  switch (nyquistZone) {
  case 1:
  case 3:
  case 5:
    {
      it_out = data_out;
      channel = 0;
      while ((it_out != data_out_end) && (channel < nofChannels)) {
 *it_out = cvec_out(channel);
 it_out++; channel++;
      }
    }
    break;
  case 2:
  case 4:
  case 6:
    {
      /// See datareader for implementation.
      it_out = data_out;
      channel = 0;
      while ((it_out != data_out_end) && (channel < nofChannels)) {
 *it_out = conj(cvec_out(fftLength - channel - 1));
 it_out++; channel++;
      }
    }
    break;
  }
  /// TODO: Check if output is correct.
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
 inline void hFFT( std::vector<HNumber> & data_in , std::vector<HComplex> & data_out , HInteger nyquistZone) {
hFFT ( data_in.begin(),data_in.end() , data_out.begin(),data_out.end() , nyquistZone);
}
inline void hFFT( casa::Vector<HNumber> & data_in , casa::Vector<HComplex> & data_out , HInteger nyquistZone) {
hFFT ( data_in.cbegin(),data_in.cend() , data_out.cbegin(),data_out.cend() , nyquistZone);
}

void (*fptr_hFFT_HIntegerHNumberHComplexHIntegerSTDITSTDIT)( std::vector<HNumber> & data_in , std::vector<HComplex> & data_out , HInteger nyquistZone) = &hFFT;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
/*
  \brief Apply an Inverse FFT on a vector

    \param data_in: Vector containing the input data on which the inverse FFT will be applied.

    \param data_out: Return vector in which the inverse FFT transformed data is stored.

    \param nyquistZone: Nyquist zone

*/
template <class IterIn, class IterOut>
void hInvFFT(const IterIn data_in, const IterIn data_in_end,
      const IterOut data_out, const IterOut data_out_end,
      const uint nyquistZone) {
  // uint channel;
  // uint blocksize = data_out_end - data_out;
  // uint fftLength = blocksize/2 + 1;
  // uint nofChannels = fftLength;
  // IPosition shape_in = (1,fftLength);
  // IPosition shape_out = (1, blocksize);
  // Vector<DComplex> cvec_f(shape_in, reinterpret_cast<DComplex*>(data_in), casa::SHARE);
  // Vector<Double> cvec_out(shape_out, 0.);
  // if ((data_in_end - data_in) != fftLength) {
  //   cerr << "[invfft] Bad input: len(data_in) != fftLength" << endl;
  // };
  // try {
  //   Vector<DComplex> cvec_in(fftLength);
  //   FFTServer<Double,DComplex> server(shape_out,
  // 				      FFTEnums::REALTOCOMPLEX);
  //   switch (nyquistZone) {
  //   case 1:
  //   case 3:
  //     for (channel=0; channel<nofChannels; channel++) {
  // 	cvec_in(channel) = cvec_f(channel);
  //     }
  //     break;
  //   case 2:
  //     for (channel=0; channel<nofChannels; channel++) {
  // 	cvec_in(channel) = conj(cvec_f(fftLength - channel - 1));
  //     }
  //     break;
  //   }
  //   server.fft(cvec_out,cvec_in);
  // } catch (AipsError x) {
  //   cerr << "[invfft]" << x.getMesg() << endl;
  // }
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
template < class T > inline void hInvFFT( std::vector<T> & data_in , std::vector<T> & data_out , uint nyquistZone) {
hInvFFT ( data_in.begin(),data_in.end() , data_out.begin(),data_out.end() , nyquistZone);
}
template < class T > inline void hInvFFT( casa::Vector<T> & data_in , casa::Vector<T> & data_out , uint nyquistZone) {
hInvFFT ( data_in.cbegin(),data_in.cend() , data_out.cbegin(),data_out.cend() , nyquistZone);
}

void (*fptr_hInvFFT_HComplex11uintSTDITSTDIT)( std::vector<HComplex> & data_in , std::vector<HComplex> & data_out , uint nyquistZone) = &hInvFFT;
void (*fptr_hInvFFT_HNumber11uintSTDITSTDIT)( std::vector<HNumber> & data_in , std::vector<HNumber> & data_out , uint nyquistZone) = &hInvFFT;
void (*fptr_hInvFFT_HInteger11uintSTDITSTDIT)( std::vector<HInteger> & data_in , std::vector<HInteger> & data_out , uint nyquistZone) = &hInvFFT;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//========================================================================
//                     I/O Functions (DataReader)
//========================================================================
//------------------------------------------------------------------------
/*!
 \brief Function to close a file with a datareader object providing the pointer to the object as an integer.

    \param iptr: Pointer to a DataReader object, stored as an integer.

*/
void hFileClose(HIntPointer iptr) {
  union{void* ptr; CR::DataReader* drp;};
  ptr=reinterpret_cast<HPointer>(iptr);
  if (ptr!=reinterpret_cast<HPointer>(__null)) delete drp;
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers

void (*fptr_hFileClose_HIntegerHIntPointer)( HIntPointer iptr) = &hFileClose;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//------------------------------------------------------------------------
/*!
 \brief Function to open a file based on a filename and returning a pointer to a datareader object as an integer

    \param Filename: Filename of file to opwn including full directory name

*/
HIntPointer hFileOpen(HString Filename) {
  bool opened;
  union{HIntPointer iptr; void* ptr; CR::DataReader* drp; CR::LOFAR_TBB* tbb; CR::LopesEventIn* lep;};
  //Create the a pointer to the DataReader object and store the pointer
  HString Filetype = hgetFiletype(Filename);
  if (Filetype=="LOPESEvent") {
    lep = new CR::LopesEventIn;
    opened=lep->attachFile(Filename);
    ( cout << "[" << "hftools.tmp.cc" << "," << 3970 << "]: " << "Opening LOPES File="<<Filename << endl ); lep->summary();
  } else if (Filetype=="LOFAR_TBB") {
    tbb = new CR::LOFAR_TBB(Filename,1024);
    opened=tbb!=__null;
    ( cout << "[" << "hftools.tmp.cc" << "," << 3974 << "]: " << "Opening LOFAR File="<<Filename << endl );tbb->summary();
  } else {
    ( cout << endl << "ERROR in file " << "hftools.tmp.cc" << " line " << 3976 << ": " << "hFileOpen" << ": Unknown Filetype = " << Filetype << ", Filename=" << Filename << endl );
    opened=false;
  }
  if (!opened){
    ( cout << endl << "ERROR in file " << "hftools.tmp.cc" << " line " << 3981 << ": " << "hFileOpen" << ": Opening file " << Filename << " failed." << endl );
    return reinterpret_cast<HInteger>(reinterpret_cast<HPointer>(__null));
  };
  return iptr;
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers

HIntPointer (*fptr_hFileOpen_HIntegerHString)( HString Filename) = &hFileOpen;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//------------------------------------------------------------------------
//-----------------------------------------------------------------------
/*!
 \brief Return information from a data file as a Python object

    \param iptr: Integer containing pointer to the datareader object

    \param keyword: Keyword ro be read out from the file metadata

*/
HPyObject hFileGetParameter(HIntPointer iptr, HString key)
{
  DataReader *drp(reinterpret_cast<DataReader*>(iptr));
  if (key== "nofAntennas") {uint result(drp->nofAntennas ()); HPyObject pyob((uint)result); return pyob;} else
    if (key== "nofSelectedChannels") {uint result(drp->nofSelectedChannels ()); HPyObject pyob((uint)result); return pyob;} else
    if (key== "nofSelectedAntennas") {uint result(drp->nofSelectedAntennas ()); HPyObject pyob((uint)result); return pyob;} else
    if (key== "nofBaselines") {uint result(drp->nofBaselines ()); HPyObject pyob((uint)result); return pyob;} else
    if (key== "block") {uint result(drp->block ()); HPyObject pyob((uint)result); return pyob;} else
    if (key== "blocksize") {uint result(drp->blocksize ()); HPyObject pyob((uint)result); return pyob;} else
    if (key== "stride") {uint result(drp->stride ()); HPyObject pyob((uint)result); return pyob;} else
    if (key== "fftLength") {uint result(drp->fftLength ()); HPyObject pyob((uint)result); return pyob;} else
    if (key== "nyquistZone") {uint result(drp->nyquistZone ()); HPyObject pyob((uint)result); return pyob;} else
    if (key== "sampleInterval") {double result(drp->sampleInterval ()); HPyObject pyob((double)result); return pyob;} else
    if (key== "referenceTime") {double result(drp->referenceTime ()); HPyObject pyob((double)result); return pyob;} else
    if (key== "sampleFrequency") {double result(drp->sampleFrequency ()); HPyObject pyob((double)result); return pyob;} else
    if (key== "antennas") {casa::Vector<uint> casavec(drp->antennas ()); std::vector<HInteger> result; aipsvec2stlvec(casavec,result); HPyObject pyob(result); return pyob;} else
    if (key== "selectedAntennas") {casa::Vector<uint> casavec(drp->selectedAntennas ()); std::vector<HInteger> result; aipsvec2stlvec(casavec,result); HPyObject pyob(result); return pyob;} else
    if (key== "selectedChannels") {casa::Vector<uint> casavec(drp->selectedChannels ()); std::vector<HInteger> result; aipsvec2stlvec(casavec,result); HPyObject pyob(result); return pyob;} else
    if (key== "positions") {casa::Vector<uint> casavec(drp->positions ()); std::vector<HInteger> result; aipsvec2stlvec(casavec,result); HPyObject pyob(result); return pyob;} else
    if (key== "increment") {casa::Vector<double> casavec(drp->increment ()); std::vector<HNumber> result; aipsvec2stlvec(casavec,result); HPyObject pyob(result); return pyob;} else
    if (key== "frequencyValues") {casa::Vector<double> casavec(drp->frequencyValues ()); std::vector<HNumber> result; aipsvec2stlvec(casavec,result); HPyObject pyob(result); return pyob;} else
    if (key== "frequencyRange") {casa::Vector<double> casavec(drp->frequencyRange ()); std::vector<HNumber> result; aipsvec2stlvec(casavec,result); HPyObject pyob(result); return pyob;} else
      if (key== "Date") {uint result; drp->headerRecord().get(key,result); HPyObject pyob((uint)result); return pyob;} else
 if (key== "Observatory") {casa::String result; drp->headerRecord().get(key,result); HPyObject pyob((HString)result); return pyob;} else
 if (key== "Filesize") {int result; drp->headerRecord().get(key,result); HPyObject pyob((int)result); return pyob;} else
 if (key== "dDate") {double result; drp->headerRecord().get(key,result); HPyObject pyob((double)result); return pyob;} else
 if (key== "presync") {int result; drp->headerRecord().get(key,result); HPyObject pyob((int)result); return pyob;} else
 if (key== "TL") {int result; drp->headerRecord().get(key,result); HPyObject pyob((int)result); return pyob;} else
 if (key== "LTL") {int result; drp->headerRecord().get(key,result); HPyObject pyob((int)result); return pyob;} else
 if (key== "EventClass") {int result; drp->headerRecord().get(key,result); HPyObject pyob((int)result); return pyob;} else
 if (key== "SampleFreq") {casa::uChar result; drp->headerRecord().get(key,result); HPyObject pyob((uint)result); return pyob;} else
 if (key== "StartSample") {uint result; drp->headerRecord().get(key,result); HPyObject pyob((uint)result); return pyob;} else
 if (key== "AntennaIDs") {casa::Vector<int> casavec; drp->headerRecord().get(key,casavec); std::vector<int> result; aipsvec2stlvec(casavec,result); HPyObject pyob(result); return pyob;} else
    { HString result; result = result
  + "nofAntennas" + ", "
    + "nofSelectedChannels" + ", "
    + "nofSelectedAntennas" + ", "
    + "nofBaselines" + ", "
    + "block" + ", "
    + "blocksize" + ", "
    + "stride" + ", "
    + "fftLength" + ", "
    + "nyquistZone" + ", "
    + "sampleInterval" + ", "
    + "referenceTime" + ", "
    + "sampleFrequency" + ", "
    + "antennas" + ", "
    + "selectedAntennas" + ", "
    + "selectedChannels" + ", "
    + "positions" + ", "
    + "increment" + ", "
    + "frequencyValues" + ", "
    + "frequencyRange" + ", "
      + "Date" + ", "
 + "Observatory" + ", "
 + "Filesize" + ", "
 + "dDate" + ", "
 + "presync" + ", "
 + "TL" + ", "
 + "LTL" + ", "
 + "EventClass" + ", "
 + "SampleFreq" + ", "
 + "StartSample" + ", "
//------------------------------------------------------------------------
 + "AntennaIDs" + ", "
       + "help";
      if (key!="help") cout << "Unknown keyword " << key <<"!"<<endl;
      cout << "hFileGetParameter" << " - available keywords: "<< result <<endl;
      HPyObject pyob(result);
      return pyob;
    };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers

HPyObject (*fptr_hFileGetParameter_HIntegerHIntPointerHString)( HIntPointer iptr , HString keyword) = &hFileGetParameter;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//-----------------------------------------------------------------------
/*!
 \brief Set parameters in a data file with a Python object as input

    \param iptr: Integer containing pointer to the datareader object

    \param keyword: Keyword to be set in the file

    \param pyob: Input paramter

*/
void hFileSetParameter(HIntPointer iptr, HString key, HPyObjectPtr pyob)
{
  DataReader *drp(reinterpret_cast<DataReader*>(iptr));
  if (key== "Blocksize") {uint input(PyInt_AsLong (pyob)); drp->setBlocksize (input);} else
    if (key== "StartBlock") {uint input(PyInt_AsLong (pyob)); drp->setStartBlock (input);} else
    if (key== "Block") {uint input(PyInt_AsLong (pyob)); drp->setBlock (input);} else
    if (key== "Stride") {uint input(PyInt_AsLong (pyob)); drp->setStride (input);} else
    if (key== "SampleOffset") {uint input(PyInt_AsLong (pyob)); drp->setSampleOffset (input);} else
    if (key== "NyquistZone") {uint input(PyInt_AsLong (pyob)); drp->setNyquistZone (input);} else
    if (key== "ReferenceTime") {double input(PyFloat_AsDouble (pyob)); drp->setReferenceTime (input);} else
    if (key== "SampleFrequency") {double input(PyFloat_AsDouble (pyob)); drp->setSampleFrequency (input);} else
    if (key== "Shift") {int input(PyInt_AsLong (pyob)); drp->setShift (input);} else
    if (key=="SelectedAntennas") {
      vector<uint> stlvec(PyList2STLuIntVec(pyob));
      uint * storage = &(stlvec[0]);
      casa::IPosition shape(1,stlvec.size()); //tell casa the size of the vector
      casa::Vector<uint> casavec(shape,storage,casa::SHARE);
      drp->setSelectedAntennas(casavec);
    } else
    { HString txt; txt = txt
  + "Blocksize" + ", "
    + "StartBlock" + ", "
    + "Block" + ", "
    + "Stride" + ", "
    + "SampleOffset" + ", "
    + "NyquistZone" + ", "
    + "ReferenceTime" + ", "
    + "SampleFrequency" + ", "
    + "Shift" + ", "
    + "SelectedAntennas" + ", "
       + "help";
      if (key!="help") cout << "Unknown keyword " << key <<"!"<<endl;
      cout << "hFileSetParameter" << " - available keywords: "<< txt <<endl;
    };
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers

void (*fptr_hFileSetParameter_HIntegerHIntPointerHStringHPyObjectPtr)( HIntPointer iptr , HString keyword , HPyObjectPtr pyob) = &hFileSetParameter;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//-----------------------------------------------------------------------
/*!
 \brief Read data from a Datareader object (pointer in iptr) into a vector, where the size should be pre-allocated.

    \param iptr: Integer containing pointer to the datareader object

    \param Datatype: Name of the data column to be retrieved 

    \param vec: Data 


Example on how to use this with the Python wrapper

file=hFileOpen("data/lofar/RS307C-readfullsecond.h5")
file=hFileOpen("/Users/falcke/LOFAR/usg/data/lopes/test.event")

#offsets=IntVec()
idata=IntVec()
hReadFile(file,"Fx",idata)
hCloseFile(file)

The data will then be in the vector idata. You can covert that to a
Python list with [].extend(idata)
*/
template <class T>
void hFileRead(
      HIntPointer iptr,
      HString Datatype,
      std::vector<T> & vec
      )
{
  //Create a DataReader Pointer from an interger variable
  DataReader *drp=reinterpret_cast<DataReader*>(iptr);
  //Check whether it is non-NULL.
  if (drp==reinterpret_cast<HPointer>(__null)){
    ( cout << endl << "ERROR in file " << "hftools.tmp.cc" << " line " << 4194 << ": " << "hFileRead" << ": pointer to FileObject is NULL, DataReader not found." << endl );
    return;
  };
  //------TIME------------------------------
  if (Datatype=="Time") {
    if (typeid(vec) == typeid(vector<double>)) {
      std::vector<double> * vec_p;
      vec_p=reinterpret_cast<vector<double>*>(&vec); //That is just a trick to fool the compiler
      drp->timeValues(*vec_p);
    } else {
      cout << "hFileRead" << ": Datatype " << typeid(vec).name() << " not supported for data field = " << Datatype << "." <<endl;
    };
  //------FREQUENCY------------------------------
  } else if (Datatype=="Frequency") {
    if (typeid(vec) == typeid(vector<double>)) {
    casa::Vector<double> val = drp->frequencyValues();
    aipsvec2stlvec(val,vec);
    } else {
      cout << "hFileRead" << ": Datatype " << typeid(vec).name() << " not supported for data field = " << Datatype << "." <<endl;
    };
  }
//..........................................................................................
//Conversion from aips to stl using shared memory space
//..........................................................................................
//..........................................................................................
  //------FX------------------------------
  else if (Datatype=="Fx") {if (typeid(vec)==typeid(std::vector<HNumber>)) { casa::IPosition shape(2); shape(0)=drp->blocksize(); shape(1)=drp->nofSelectedAntennas(); casa::Matrix<double> casamtrx(shape,reinterpret_cast<double*>(&(vec[0])),casa::SHARE); drp->fx (casamtrx); } else { cout << "hFileRead" << ": Datatype " << typeid(vec).name() << " not supported for data field = " << Datatype << "." <<endl; };}
  //------VOLTAGE------------------------------
  else if (Datatype=="Voltage") {if (typeid(vec)==typeid(std::vector<HNumber>)) { casa::IPosition shape(2); shape(0)=drp->blocksize(); shape(1)=drp->nofSelectedAntennas(); casa::Matrix<double> casamtrx(shape,reinterpret_cast<double*>(&(vec[0])),casa::SHARE); drp->voltage (casamtrx); } else { cout << "hFileRead" << ": Datatype " << typeid(vec).name() << " not supported for data field = " << Datatype << "." <<endl; };}
  //------FFT------------------------------
  else if (Datatype=="FFT") {if (typeid(vec)==typeid(std::vector<HComplex>)) { casa::IPosition shape(2); shape(0)=drp->blocksize(); shape(1)=drp->nofSelectedAntennas(); casa::Matrix<CasaComplex> casamtrx(shape,reinterpret_cast<CasaComplex*>(&(vec[0])),casa::SHARE); drp->fft (casamtrx); } else { cout << "hFileRead" << ": Datatype " << typeid(vec).name() << " not supported for data field = " << Datatype << "." <<endl; };}
  //------CALFFT------------------------------
  else if (Datatype=="CalFFT") {if (typeid(vec)==typeid(std::vector<HComplex>)) { casa::IPosition shape(2); shape(0)=drp->blocksize(); shape(1)=drp->nofSelectedAntennas(); casa::Matrix<CasaComplex> casamtrx(shape,reinterpret_cast<CasaComplex*>(&(vec[0])),casa::SHARE); drp->calfft (casamtrx); } else { cout << "hFileRead" << ": Datatype " << typeid(vec).name() << " not supported for data field = " << Datatype << "." <<endl; };}
  else {
    ( cout << endl << "ERROR in file " << "hftools.tmp.cc" << " line " << 4239 << ": " << "hFileRead" << ": Datatype=" << Datatype << " is unknown." << endl );
    vec.clear();
  };
  return;
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers

void (*fptr_hFileRead_HComplexHIntPointerHString1STL)( HIntPointer iptr , HString Datatype , std::vector<HComplex> & vec) = &hFileRead;
void (*fptr_hFileRead_HNumberHIntPointerHString1STL)( HIntPointer iptr , HString Datatype , std::vector<HNumber> & vec) = &hFileRead;
void (*fptr_hFileRead_HIntegerHIntPointerHString1STL)( HIntPointer iptr , HString Datatype , std::vector<HInteger> & vec) = &hFileRead;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//------------------------------------------------------------------------
//-----------------------------------------------------------------------
/*!
 \brief Return a list of antenna positions from the CalTables - this is a test

    \param filename: Filename of the caltable

    \param keyword: Keyword to be read out from the file metadata 

    \param date: Date for which the information is requested

    \param pyob: 


Example:
antennaIDs=hFileGetParameter(file,"AntennaIDs")
x=hCalTable("~/LOFAR/usg/data/lopes/LOPES-CalTable",obsdate,list(antennaIDs))

*/
HPyObjectPtr hCalTable(HString filename, HString keyword, HInteger date, HPyObjectPtr pyob) {
  CR::CalTableReader* CTRead = new CR::CalTableReader(filename);
  HInteger i,ant,size;
  casa::Vector<Double> tmpvec;
  HPyObjectPtr list=PyList_New(0),tuple;
  if (CTRead != __null && ((((((PyObject*)(pyob))->ob_type))->tp_flags & ((1L<<25))) != 0)) { //Check if CalTable was opened ... and Python object is a list
    size=PyList_Size(pyob);
    for (i=0;i<size;++i){ //loop over all antennas
      ant=PyInt_AsLong(PyList_GetItem(pyob,i)); //Get the ith element of the list, i.e. the antenna ID
      CTRead->GetData((uint)date, ant, keyword, &tmpvec);
      tuple=PyTuple_Pack(3,PyFloat_FromDouble(tmpvec[0]),PyFloat_FromDouble(tmpvec[1]),PyFloat_FromDouble(tmpvec[2]));
      PyList_Append(list,tuple);
    };
  };
  return list;
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers

HPyObjectPtr (*fptr_hCalTable_HIntegerHStringHStringHIntegerHPyObjectPtr)( HString filename , HString keyword , HInteger date , HPyObjectPtr pyob) = &hCalTable;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//========================================================================
//              Coordinate Conversion (VectorConversion.cc)
//========================================================================
//$ORIGIN: Math/VectorConversion.cc
//-----------------------------------------------------------------------
/*!
  \brief Converts a 3D spatial vector into a different Coordinate type (e.g. Spherical to Cartesian)

    \param source: Coordinates of the source to be converted - vector of length 3

    \param sourceCoordinate: Type of the coordinates for the source

    \param target: Coordinates of the source to be converted - vector of length 3

    \param targetCoordinate: Type of the coordinates for the target 

    \param anglesInDegrees: True if the angles are in degree, otherwise in radians


Available Coordinate Types are:

      -  Azimuth-Elevation-Height, \f$ \vec x = (Az,El,H) \f$
      AzElHeight,
      -  Azimuth-Elevation-Radius, \f$ \vec x = (Az,El,R) \f$
      AzElRadius,
      -  Cartesian coordinates, \f$ \vec x = (x,y,z) \f$
      Cartesian,
      -  Cylindrical coordinates, \f$ \vec x = (r,\phi,h) \f$
      Cylindrical,
      -  Direction on the sky, \f$ \vec x = (Lon,Lat) \f$
      Direction,
      -  Direction on the sky with radial distance, \f$ \vec x = (Lon,Lat,R) \f$
      DirectionRadius,
      -  Frquency
      Frequency,
      -  Longitude-Latitude-Radius
      LongLatRadius,
      -  North-East-Height
      NorthEastHeight,
      -  Spherical coordinates, \f$ \vec x = (r,\phi,\theta) \f$
      Spherical,
      - Time
      Time

*/
template <class Iter>
bool hCoordinateConvert (Iter source,
      CR::CoordinateType::Types const &sourceCoordinate,
      Iter target,
      CR::CoordinateType::Types const &targetCoordinate,
      bool anglesInDegrees
      )
{
  return CR::convertVector(
      *target,
      *(target+1),
      *(target+2),
      targetCoordinate,
      *source,
      *(source+1),
      *(source+2),
      sourceCoordinate,
      anglesInDegrees
          );
    }
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
 inline bool hCoordinateConvert( std::vector<HNumber> & source , CRCoordinateType sourceCoordinate , std::vector<HNumber> & target , CRCoordinateType targetCoordinate , bool anglesInDegrees) {
return hCoordinateConvert ( source.begin() , sourceCoordinate , target.begin() , targetCoordinate , anglesInDegrees);
}
inline bool hCoordinateConvert( casa::Vector<HNumber> & source , CRCoordinateType sourceCoordinate , casa::Vector<HNumber> & target , CRCoordinateType targetCoordinate , bool anglesInDegrees) {
return hCoordinateConvert ( source.cbegin() , sourceCoordinate , target.cbegin() , targetCoordinate , anglesInDegrees);
}

bool (*fptr_hCoordinateConvert_HIntegerHNumberCRCoordinateTypeHNumberCRCoordinateTypeboolSTDITFIXEDSTDITFIXED)( std::vector<HNumber> & source , CRCoordinateType sourceCoordinate , std::vector<HNumber> & target , CRCoordinateType targetCoordinate , bool anglesInDegrees) = &hCoordinateConvert;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//========================================================================================
//OUTDATED!!!OUTDATED!!!OUTDATED!!!OUTDATED!!!OUTDATED!!!OUTDATED!!!OUTDATED!!!OUTDATED!!!
//OUTDATED!!!OUTDATED!!!OUTDATED!!!OUTDATED!!!OUTDATED!!!OUTDATED!!!OUTDATED!!!OUTDATED!!!
//========================================================================================
//-----------------------------------------------------------------------
//#define HFPP_PARDEF_8 (HInteger)(Offsets)()("Offsets per antenna")(HFPP_PAR_IS_VECTOR)(STL)(HFPP_PASS_AS_REFERENCE)
/*!
 \brief Read data from a Datareader object (pointer in iptr) into a vector.

    \param vec: Data 

    \param iptr: Integer containing pointer to the datareader object

    \param Datatype: Name of the data column to be retrieved 

    \param Antenna: Antenna number

    \param Blocksize: Length of the data block to read

    \param Block: Block number to read

    \param Stride: Stride between blocks

    \param Shift: Shift start of first block by n samples


Example on how to use this with the Python wrapper

file=hOpenFile("data/lofar/RS307C-readfullsecond.h5")
file=hOpenFile("/Users/falcke/LOFAR/usg/data/lopes/test.event")

#offsets=IntVec()
data=FloatVec()
idata=IntVec()
cdata=ComplexVec()
sdata=StringVec()
Datatype="Fx"
Antenna=1
Blocksize=1024
Block=10
Stride=0
Shift=0
hReadFile(idata,file,Datatype,Antenna,Blocksize,Block,Stride,Shift,offsets)
hCloseFile(file)

The data will then be in the vector idata. You can covert that to a
Python list with [].extend(idata)
*/
template <class T>
void hReadFileOld(std::vector<T> & vec,
      HIntPointer iptr,
      HString Datatype,
      HInteger Antenna,
      HInteger Blocksize,
      HInteger Block,
      HInteger Stride,
      HInteger Shift)
//	       std::vector<HInteger> & Offsets)
{
  DataReader *drp=reinterpret_cast<DataReader*>(iptr);
  //First retrieve the pointer to the pointer to the dataRead and check whether it is non-NULL.
  if (drp==reinterpret_cast<HPointer>(__null)){
    ( cout << endl << "ERROR in file " << "hftools.tmp.cc" << " line " << 4447 << ": " << "dataRead: pointer to FileObject is NULL, DataReader not found." << endl );
    return;
  };
//!!!One Needs to verify somehow that the parameters make sense !!!
  if (Antenna > static_cast<HInteger>(drp->nofAntennas()-1)) {
    ( cout << endl << "ERROR in file " << "hftools.tmp.cc" << " line " << 4453 << ": " << "Requested Antenna number too large!" << endl );
    return;
  };
  drp->setBlocksize(Blocksize);
  drp->setBlock(Block);
  drp->setStride(Stride);
  drp->setShift(Shift);
  casa::Vector<uint> antennas(1,Antenna);
  drp->setSelectedAntennas(antennas);
  address ncol;
  if (Datatype=="Time") {
    if (typeid(vec) == typeid(vector<double>)) {
      std::vector<double> * vec_p;
      vec_p=reinterpret_cast<vector<double>*>(&vec); //That is just a tr
      drp->timeValues(*vec_p);
    } else {
      std::vector<double> tmpvec;
      drp->timeValues(tmpvec);
      hConvert(tmpvec,vec);
    };
  }
  else if (Datatype=="Frequency") {
    //vector<HNumber>* vp2; *vp2 = drp->frequencyValues().tovec();
    //    std::vector<double> vals;
    //copycast_vec(vals,vec);
    casa::Vector<double> val = drp->frequencyValues();
    aipsvec2stlvec(val,vec);
  }
  else if (Datatype=="Fx") {
//hf #define MAKE_UNION_CASA_MATRIX_AND_STLVEC(TYPECASA,STLVEC,CASAVEC) //     casa::IPosition shape(2,STLVEC.size()); shape(1)=1;	//     casa::Matrix<TYPECASA> CASAVEC(shape,reinterpret_cast<TYPECASA*>(&(STLVEC[0])),casa::SHARE)
//     if (typeid(vec)==typeid(std::vector<double>)) {
//       vec.resize(Blocksize);
//       MAKE_UNION_CASA_MATRIX_AND_STLVEC(double,vec,mtrx);
//       drp->fx(mtrx);
//     } else {
//       std::vector<double> nvec(Blocksize);
//       MAKE_UNION_CASA_MATRIX_AND_STLVEC(double,nvec,mtrx);
//       drp->fx(mtrx);
//       copycast_vec(nvec,vec); //Copy back to output vector and convert type
//     };
    casa::Matrix<CasaNumber> ary=drp->fx();
    ncol=ary.ncolumn(); if (ncol>1 && Antenna<ncol) aipscol2stlvec(ary,vec,Antenna); else aipscol2stlvec(ary,vec,0);;
  }
  else if (Datatype=="Voltage") {
    //    std::vector<HNumber>* vp2 = new std::vector<HNumber>;
    casa::Matrix<CasaNumber> ary=drp->voltage();
    ncol=ary.ncolumn(); if (ncol>1 && Antenna<ncol) aipscol2stlvec(ary,vec,Antenna); else aipscol2stlvec(ary,vec,0);;
  }
  else if (Datatype=="invFFT") {
    //    std::vector<HNumber>* vp2 = new std::vector<HNumber>;
    casa::Matrix<CasaNumber> ary=drp->invfft();
    ncol=ary.ncolumn(); if (ncol>1 && Antenna<ncol) aipscol2stlvec(ary,vec,Antenna); else aipscol2stlvec(ary,vec,0);;
  }
  else if (Datatype=="FFT") {
    //    std::vector<HComplex>* vp2 = new std::vector<HComplex>;
    casa::Matrix<CasaComplex> ary=drp->fft();
    ncol=ary.ncolumn(); if (ncol>1 && Antenna<ncol) aipscol2stlvec(ary,vec,Antenna); else aipscol2stlvec(ary,vec,0);;
  }
  else if (Datatype=="CalFFT") {
    //    std::vector<HComplex>* vp2 = new std::vector<HComplex>;
    casa::Matrix<CasaComplex> ary=drp->calfft();
    ncol=ary.ncolumn(); if (ncol>1 && Antenna<ncol) aipscol2stlvec(ary,vec,Antenna); else aipscol2stlvec(ary,vec,0);;
  }
  else {
    ( cout << endl << "ERROR in file " << "hftools.tmp.cc" << " line " << 4525 << ": " << "DataFunc_CR_dataRead: Datatype=" << Datatype << " is unknown." << endl );
    vec.clear();
    return;
  };
  return;
}
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers

void (*fptr_hReadFileOld_HComplex1HIntPointerHStringHIntegerHIntegerHIntegerHIntegerHIntegerSTL)( std::vector<HComplex> & vec , HIntPointer iptr , HString Datatype , HInteger Antenna , HInteger Blocksize , HInteger Block , HInteger Stride , HInteger Shift) = &hReadFileOld;
void (*fptr_hReadFileOld_HNumber1HIntPointerHStringHIntegerHIntegerHIntegerHIntegerHIntegerSTL)( std::vector<HNumber> & vec , HIntPointer iptr , HString Datatype , HInteger Antenna , HInteger Blocksize , HInteger Block , HInteger Stride , HInteger Shift) = &hReadFileOld;
void (*fptr_hReadFileOld_HInteger1HIntPointerHStringHIntegerHIntegerHIntegerHIntegerHIntegerSTL)( std::vector<HInteger> & vec , HIntPointer iptr , HString Datatype , HInteger Antenna , HInteger Blocksize , HInteger Block , HInteger Stride , HInteger Shift) = &hReadFileOld;

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
/*
  else if (Datatype=="Fx") {aipscol2stlvec(drp->fx(),*vp,0);}
  else if (Datatype=="Voltage") {aipscol2stlvec(drp->voltage(),*vp,0);}
  else if (Datatype=="invFFT") {aipscol2stlvec(drp->invfft(),*vp,0);}
  else if (Datatype=="FFT") {aipscol2stlvec(drp->fft(),*vp,0);}
  else if (Datatype=="CalFFT") {aipscol2stlvec(drp->calfft(),*vp,0);}
*/
//       std::vector<double> tmpvec;
//       STL2CASA_SHARED(double,tmpvec,casavec); //Create casa vector sharing memory with the stl vector
//       casa::Vector<double> * vec_p;  //Get pointer to casa vector
//       vec_p=reinterpret_cast<casa::Vector<double>*>(&casavec); //That is just a trick to fool the compiler to
//                                                               //compile this section for T!=double (even though it is then never used)
//       *vec_p=drp->frequencyValues(); //read data into the casa and hence also into the stl vector
//       hConvert(tmpvec,vec);
//     }  else {   //Input vector is not of the right format
//       std::vector<double> tmpvec;  //Create temporary stl vector
//       STL2CASA_SHARED(double,tmpvec,casavec);  //Create casa vector sharing memory with the tmp stl vector
//       casavec=drp->frequencyValues(); //read data into the casa vector (hence als tmp stl vector)
//       hConvert(tmpvec,vec); // Copy and convert data from tmp stl (=casa) vector to the output vector.
//     };
//========================================================================================
//OUTDATED!!!OUTDATED!!!OUTDATED!!!OUTDATED!!!OUTDATED!!!OUTDATED!!!OUTDATED!!!OUTDATED!!!
//OUTDATED!!!OUTDATED!!!OUTDATED!!!OUTDATED!!!OUTDATED!!!OUTDATED!!!OUTDATED!!!OUTDATED!!!
//========================================================================================
////////////////////////////////////////////////////////////////////////
//Definition of Python bindings for hftools ...
////////////////////////////////////////////////////////////////////////
// Tell the preprocessor (for generating wrappers) that this is a c++
// header file for Python exposure
void init_module_hftools(); extern "C" __attribute__ ((visibility("default"))) void inithftools() { boost::python::detail::init_module("hftools", &init_module_hftools); } void init_module_hftools()
{
    using namespace boost::python;
    class_<std::hmatrix<HInteger,allocator<HInteger> > >("IntMatrix")
      .def(vector_indexing_suite<std::hmatrix<HInteger,allocator<HInteger> > >())
      //      .def("setDimension",&std::hmatrix<HInteger,allocator<HInteger> >::setDimension)
      //.def("getDimension",&std::hmatrix<HInteger,allocator<HInteger> >::getDimension)
      ;
    /*
  class_<casa::Vector<HInteger> >("CasaIntVec")
    .def(vector_indexing_suite<casa::Vector<HInteger> >())
    ;
    */
  class_<std::vector<HInteger> >("IntVec")
    .def(vector_indexing_suite<std::vector<HInteger> >())
      /*.def("thiAdd",fptr_hiAdd_HComplexHComplex12)
	.def("thiAdd",fptr_hiAdd_HComplexHNumber12)
	.def("thiAdd",fptr_hiAdd_HComplexHInteger12)
	.def("thiAdd",fptr_hiAdd_HNumberHComplex12)
	.def("thiAdd",fptr_hiAdd_HNumberHNumber12)
	.def("thiAdd",fptr_hiAdd_HNumberHInteger12)
	.def("thiAdd",fptr_hiAdd_HIntegerHComplex12)
	.def("thiAdd",fptr_hiAdd_HIntegerHNumber12)
	.def("thiAdd",fptr_hiAdd_HIntegerHInteger12)
      */
      ;
    class_<std::vector<HNumber> >("FloatVec")
        .def(vector_indexing_suite<std::vector<HNumber> >())
      ;
    class_<std::vector<bool> >("BoolVec")
        .def(vector_indexing_suite<std::vector<bool> >())
      ;
    class_<std::vector<HComplex> >("ComplexVec")
        .def(vector_indexing_suite<std::vector<HComplex> >())
      ;
    class_<std::vector<HString> >("StringVec")
        .def(vector_indexing_suite<std::vector<HString> >())
      ;
    //boost::python::converter::registry::insert(&extract_swig_wrapped_pointer, type_id<mglData>());
    // def("pytointptr",getPointerFromPythonObject);
    //def("getptr", PyGetPtr);
    def("hgetFiletype",hgetFiletype);
    def("hgetFileExtension",hgetFileExtension);
    enum_<hWEIGHTS>("hWEIGHTS")
      .value("FLAT",WEIGHTS_FLAT)
      .value("LINEAR",WEIGHTS_LINEAR)
      .value("GAUSSIAN", WEIGHTS_GAUSSIAN);
    enum_<CR::CoordinateType::Types>("CoordinateTypes")
      //! Azimuth-Elevation-Height, \f$ \vec x = (Az,El,H) \f$
      .value("AzElHeight",CR::CoordinateType::AzElHeight)
      //! Azimuth-Elevation-Radius, \f$ \vec x = (Az,El,R) \f$
      .value("AzElRadius",CR::CoordinateType::AzElRadius)
      //! Cartesian coordinates, \f$ \vec x = (x,y,z) \f$
      .value("Cartesian",CR::CoordinateType::Cartesian)
      //! Cylindrical coordinates, \f$ \vec x = (r,\phi,h) \f$
      .value("Cylindrical",CR::CoordinateType::Cylindrical)
      //! Direction on the sky, \f$ \vec x = (Lon,Lat) \f$
      .value("Direction",CR::CoordinateType::Direction)
      //! Direction on the sky with radial distance, \f$ \vec x = (Lon,Lat,R) \f$
      .value("DirectionRadius",CR::CoordinateType::DirectionRadius)
      //! Frquency
      .value("Frequency",CR::CoordinateType::Frequency)
      //! Longitude-Latitude-Radius
      .value("LongLatRadius",CR::CoordinateType::LongLatRadius)
      //! North-East-Height
      .value("NorthEastHeight",CR::CoordinateType::NorthEastHeight)
      //! Spherical coordinates, \f$ \vec x = (r,\phi,\theta) \f$
      .value("Spherical",CR::CoordinateType::Spherical)
      //! Time
      .value("Time",CR::CoordinateType::Time);
    //The following will include the automatically generated python wrappers
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hFill",fptr_hFill_HString11STDIT );
def("hFill",fptr_hFill_HBool11STDIT );
def("hFill",fptr_hFill_HComplex11STDIT );
def("hFill",fptr_hFill_HNumber11STDIT );
def("hFill",fptr_hFill_HInteger11STDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers

def("hNew",fptr_hNew_HString1STL );
def("hNew",fptr_hNew_HBool1STL );
def("hNew",fptr_hNew_HComplex1STL );
def("hNew",fptr_hNew_HNumber1STL );
def("hNew",fptr_hNew_HInteger1STL );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers

def("hResize",fptr_hResize_HString1HIntegerSTL );
def("hResize",fptr_hResize_HBool1HIntegerSTL );
def("hResize",fptr_hResize_HComplex1HIntegerSTL );
def("hResize",fptr_hResize_HNumber1HIntegerSTL );
def("hResize",fptr_hResize_HInteger1HIntegerSTL );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers

def("hResize",fptr_hResize_HString1HInteger1STL );
def("hResize",fptr_hResize_HBool1HInteger1STL );
def("hResize",fptr_hResize_HComplex1HInteger1STL );
def("hResize",fptr_hResize_HNumber1HInteger1STL );
def("hResize",fptr_hResize_HInteger1HInteger1STL );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers

def("hResize",fptr_hResize_HStringHString12STLSTL );
def("hResize",fptr_hResize_HStringHBool12STLSTL );
def("hResize",fptr_hResize_HStringHComplex12STLSTL );
def("hResize",fptr_hResize_HStringHNumber12STLSTL );
def("hResize",fptr_hResize_HStringHInteger12STLSTL );
def("hResize",fptr_hResize_HBoolHString12STLSTL );
def("hResize",fptr_hResize_HBoolHBool12STLSTL );
def("hResize",fptr_hResize_HBoolHComplex12STLSTL );
def("hResize",fptr_hResize_HBoolHNumber12STLSTL );
def("hResize",fptr_hResize_HBoolHInteger12STLSTL );
def("hResize",fptr_hResize_HComplexHString12STLSTL );
def("hResize",fptr_hResize_HComplexHBool12STLSTL );
def("hResize",fptr_hResize_HComplexHComplex12STLSTL );
def("hResize",fptr_hResize_HComplexHNumber12STLSTL );
def("hResize",fptr_hResize_HComplexHInteger12STLSTL );
def("hResize",fptr_hResize_HNumberHString12STLSTL );
def("hResize",fptr_hResize_HNumberHBool12STLSTL );
def("hResize",fptr_hResize_HNumberHComplex12STLSTL );
def("hResize",fptr_hResize_HNumberHNumber12STLSTL );
def("hResize",fptr_hResize_HNumberHInteger12STLSTL );
def("hResize",fptr_hResize_HIntegerHString12STLSTL );
def("hResize",fptr_hResize_HIntegerHBool12STLSTL );
def("hResize",fptr_hResize_HIntegerHComplex12STLSTL );
def("hResize",fptr_hResize_HIntegerHNumber12STLSTL );
def("hResize",fptr_hResize_HIntegerHInteger12STLSTL );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers

def("hResize",fptr_hResize_HStringHString12CASACASA );
def("hResize",fptr_hResize_HStringHBool12CASACASA );
def("hResize",fptr_hResize_HStringHComplex12CASACASA );
def("hResize",fptr_hResize_HStringHNumber12CASACASA );
def("hResize",fptr_hResize_HStringHInteger12CASACASA );
def("hResize",fptr_hResize_HBoolHString12CASACASA );
def("hResize",fptr_hResize_HBoolHBool12CASACASA );
def("hResize",fptr_hResize_HBoolHComplex12CASACASA );
def("hResize",fptr_hResize_HBoolHNumber12CASACASA );
def("hResize",fptr_hResize_HBoolHInteger12CASACASA );
def("hResize",fptr_hResize_HComplexHString12CASACASA );
def("hResize",fptr_hResize_HComplexHBool12CASACASA );
def("hResize",fptr_hResize_HComplexHComplex12CASACASA );
def("hResize",fptr_hResize_HComplexHNumber12CASACASA );
def("hResize",fptr_hResize_HComplexHInteger12CASACASA );
def("hResize",fptr_hResize_HNumberHString12CASACASA );
def("hResize",fptr_hResize_HNumberHBool12CASACASA );
def("hResize",fptr_hResize_HNumberHComplex12CASACASA );
def("hResize",fptr_hResize_HNumberHNumber12CASACASA );
def("hResize",fptr_hResize_HNumberHInteger12CASACASA );
def("hResize",fptr_hResize_HIntegerHString12CASACASA );
def("hResize",fptr_hResize_HIntegerHBool12CASACASA );
def("hResize",fptr_hResize_HIntegerHComplex12CASACASA );
def("hResize",fptr_hResize_HIntegerHNumber12CASACASA );
def("hResize",fptr_hResize_HIntegerHInteger12CASACASA );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hConvert",fptr_hConvert_HComplexHComplex12STDITSTDIT );
def("hConvert",fptr_hConvert_HComplexHNumber12STDITSTDIT );
def("hConvert",fptr_hConvert_HComplexHInteger12STDITSTDIT );
def("hConvert",fptr_hConvert_HNumberHComplex12STDITSTDIT );
def("hConvert",fptr_hConvert_HNumberHNumber12STDITSTDIT );
def("hConvert",fptr_hConvert_HNumberHInteger12STDITSTDIT );
def("hConvert",fptr_hConvert_HIntegerHComplex12STDITSTDIT );
def("hConvert",fptr_hConvert_HIntegerHNumber12STDITSTDIT );
def("hConvert",fptr_hConvert_HIntegerHInteger12STDITSTDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hCopy",fptr_hCopy_HComplex11STDITSTDIT );
def("hCopy",fptr_hCopy_HNumber11STDITSTDIT );
def("hCopy",fptr_hCopy_HInteger11STDITSTDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//------------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers

def("square",fptr_square_HComplex1 );
def("square",fptr_square_HNumber1 );
def("square",fptr_square_HInteger1 );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//------------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers

def("hPhase",fptr_hPhase_HIntegerHNumberHNumber );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//------------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers

def("funcGaussian",fptr_funcGaussian_HIntegerHNumberHNumberHNumber );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hExp",fptr_hExp1_HComplex1STDIT );
def("hExp",fptr_hExp1_HNumber1STDIT );
def("hExp",fptr_hExp1_HInteger1STDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hExp",fptr_hExp2_HComplex11STDITSTDIT );
def("hExp",fptr_hExp2_HNumber11STDITSTDIT );
def("hExp",fptr_hExp2_HInteger11STDITSTDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hLog",fptr_hLog1_HComplex1STDIT );
def("hLog",fptr_hLog1_HNumber1STDIT );
def("hLog",fptr_hLog1_HInteger1STDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hLog",fptr_hLog2_HComplex11STDITSTDIT );
def("hLog",fptr_hLog2_HNumber11STDITSTDIT );
def("hLog",fptr_hLog2_HInteger11STDITSTDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hLog10",fptr_hLog101_HComplex1STDIT );
def("hLog10",fptr_hLog101_HNumber1STDIT );
def("hLog10",fptr_hLog101_HInteger1STDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hLog10",fptr_hLog102_HComplex11STDITSTDIT );
def("hLog10",fptr_hLog102_HNumber11STDITSTDIT );
def("hLog10",fptr_hLog102_HInteger11STDITSTDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hSin",fptr_hSin1_HComplex1STDIT );
def("hSin",fptr_hSin1_HNumber1STDIT );
def("hSin",fptr_hSin1_HInteger1STDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hSin",fptr_hSin2_HComplex11STDITSTDIT );
def("hSin",fptr_hSin2_HNumber11STDITSTDIT );
def("hSin",fptr_hSin2_HInteger11STDITSTDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hSinh",fptr_hSinh1_HComplex1STDIT );
def("hSinh",fptr_hSinh1_HNumber1STDIT );
def("hSinh",fptr_hSinh1_HInteger1STDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hSinh",fptr_hSinh2_HComplex11STDITSTDIT );
def("hSinh",fptr_hSinh2_HNumber11STDITSTDIT );
def("hSinh",fptr_hSinh2_HInteger11STDITSTDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hSqrt",fptr_hSqrt1_HComplex1STDIT );
def("hSqrt",fptr_hSqrt1_HNumber1STDIT );
def("hSqrt",fptr_hSqrt1_HInteger1STDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hSqrt",fptr_hSqrt2_HComplex11STDITSTDIT );
def("hSqrt",fptr_hSqrt2_HNumber11STDITSTDIT );
def("hSqrt",fptr_hSqrt2_HInteger11STDITSTDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hSquare",fptr_hSquare1_HComplex1STDIT );
def("hSquare",fptr_hSquare1_HNumber1STDIT );
def("hSquare",fptr_hSquare1_HInteger1STDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hSquare",fptr_hSquare2_HComplex11STDITSTDIT );
def("hSquare",fptr_hSquare2_HNumber11STDITSTDIT );
def("hSquare",fptr_hSquare2_HInteger11STDITSTDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hTan",fptr_hTan1_HComplex1STDIT );
def("hTan",fptr_hTan1_HNumber1STDIT );
def("hTan",fptr_hTan1_HInteger1STDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hTan",fptr_hTan2_HComplex11STDITSTDIT );
def("hTan",fptr_hTan2_HNumber11STDITSTDIT );
def("hTan",fptr_hTan2_HInteger11STDITSTDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hTanh",fptr_hTanh1_HComplex1STDIT );
def("hTanh",fptr_hTanh1_HNumber1STDIT );
def("hTanh",fptr_hTanh1_HInteger1STDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hTanh",fptr_hTanh2_HComplex11STDITSTDIT );
def("hTanh",fptr_hTanh2_HNumber11STDITSTDIT );
def("hTanh",fptr_hTanh2_HInteger11STDITSTDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hAbs",fptr_hAbs1_HComplex1STDIT );
def("hAbs",fptr_hAbs1_HNumber1STDIT );
def("hAbs",fptr_hAbs1_HInteger1STDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hAbs",fptr_hAbs2_HComplex11STDITSTDIT );
def("hAbs",fptr_hAbs2_HNumber11STDITSTDIT );
def("hAbs",fptr_hAbs2_HInteger11STDITSTDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hCos",fptr_hCos1_HComplex1STDIT );
def("hCos",fptr_hCos1_HNumber1STDIT );
def("hCos",fptr_hCos1_HInteger1STDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hCos",fptr_hCos2_HComplex11STDITSTDIT );
def("hCos",fptr_hCos2_HNumber11STDITSTDIT );
def("hCos",fptr_hCos2_HInteger11STDITSTDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hCosh",fptr_hCosh1_HComplex1STDIT );
def("hCosh",fptr_hCosh1_HNumber1STDIT );
def("hCosh",fptr_hCosh1_HInteger1STDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hCosh",fptr_hCosh2_HComplex11STDITSTDIT );
def("hCosh",fptr_hCosh2_HNumber11STDITSTDIT );
def("hCosh",fptr_hCosh2_HInteger11STDITSTDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hCeil",fptr_hCeil1_HNumber1STDIT );
def("hCeil",fptr_hCeil1_HInteger1STDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hCeil",fptr_hCeil2_HNumberHNumber12STDITSTDIT );
def("hCeil",fptr_hCeil2_HNumberHInteger12STDITSTDIT );
def("hCeil",fptr_hCeil2_HIntegerHNumber12STDITSTDIT );
def("hCeil",fptr_hCeil2_HIntegerHInteger12STDITSTDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hFloor",fptr_hFloor1_HNumber1STDIT );
def("hFloor",fptr_hFloor1_HInteger1STDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hFloor",fptr_hFloor2_HNumberHNumber12STDITSTDIT );
def("hFloor",fptr_hFloor2_HNumberHInteger12STDITSTDIT );
def("hFloor",fptr_hFloor2_HIntegerHNumber12STDITSTDIT );
def("hFloor",fptr_hFloor2_HIntegerHInteger12STDITSTDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hAcos",fptr_hAcos1_HNumber1STDIT );
def("hAcos",fptr_hAcos1_HInteger1STDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hAcos",fptr_hAcos2_HNumberHNumber12STDITSTDIT );
def("hAcos",fptr_hAcos2_HNumberHInteger12STDITSTDIT );
def("hAcos",fptr_hAcos2_HIntegerHNumber12STDITSTDIT );
def("hAcos",fptr_hAcos2_HIntegerHInteger12STDITSTDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hAsin",fptr_hAsin1_HNumber1STDIT );
def("hAsin",fptr_hAsin1_HInteger1STDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hAsin",fptr_hAsin2_HNumberHNumber12STDITSTDIT );
def("hAsin",fptr_hAsin2_HNumberHInteger12STDITSTDIT );
def("hAsin",fptr_hAsin2_HIntegerHNumber12STDITSTDIT );
def("hAsin",fptr_hAsin2_HIntegerHInteger12STDITSTDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hAtan",fptr_hAtan1_HNumber1STDIT );
def("hAtan",fptr_hAtan1_HInteger1STDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hAtan",fptr_hAtan2_HNumberHNumber12STDITSTDIT );
def("hAtan",fptr_hAtan2_HNumberHInteger12STDITSTDIT );
def("hAtan",fptr_hAtan2_HIntegerHNumber12STDITSTDIT );
def("hAtan",fptr_hAtan2_HIntegerHInteger12STDITSTDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hiSub",fptr_hiSub_HComplexHComplex12STDITSTDIT );
def("hiSub",fptr_hiSub_HComplexHNumber12STDITSTDIT );
def("hiSub",fptr_hiSub_HComplexHInteger12STDITSTDIT );
def("hiSub",fptr_hiSub_HNumberHComplex12STDITSTDIT );
def("hiSub",fptr_hiSub_HNumberHNumber12STDITSTDIT );
def("hiSub",fptr_hiSub_HNumberHInteger12STDITSTDIT );
def("hiSub",fptr_hiSub_HIntegerHComplex12STDITSTDIT );
def("hiSub",fptr_hiSub_HIntegerHNumber12STDITSTDIT );
def("hiSub",fptr_hiSub_HIntegerHInteger12STDITSTDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hiSub",fptr_hiSub2_HComplexHComplex12STDIT );
def("hiSub",fptr_hiSub2_HComplexHNumber12STDIT );
def("hiSub",fptr_hiSub2_HComplexHInteger12STDIT );
def("hiSub",fptr_hiSub2_HNumberHComplex12STDIT );
def("hiSub",fptr_hiSub2_HNumberHNumber12STDIT );
def("hiSub",fptr_hiSub2_HNumberHInteger12STDIT );
def("hiSub",fptr_hiSub2_HIntegerHComplex12STDIT );
def("hiSub",fptr_hiSub2_HIntegerHNumber12STDIT );
def("hiSub",fptr_hiSub2_HIntegerHInteger12STDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hSub",fptr_hSub_HComplexHComplexHComplex123STDITSTDITSTDIT );
def("hSub",fptr_hSub_HComplexHComplexHNumber123STDITSTDITSTDIT );
def("hSub",fptr_hSub_HComplexHComplexHInteger123STDITSTDITSTDIT );
def("hSub",fptr_hSub_HComplexHNumberHComplex123STDITSTDITSTDIT );
def("hSub",fptr_hSub_HComplexHNumberHNumber123STDITSTDITSTDIT );
def("hSub",fptr_hSub_HComplexHNumberHInteger123STDITSTDITSTDIT );
def("hSub",fptr_hSub_HComplexHIntegerHComplex123STDITSTDITSTDIT );
def("hSub",fptr_hSub_HComplexHIntegerHNumber123STDITSTDITSTDIT );
def("hSub",fptr_hSub_HComplexHIntegerHInteger123STDITSTDITSTDIT );
def("hSub",fptr_hSub_HNumberHComplexHComplex123STDITSTDITSTDIT );
def("hSub",fptr_hSub_HNumberHComplexHNumber123STDITSTDITSTDIT );
def("hSub",fptr_hSub_HNumberHComplexHInteger123STDITSTDITSTDIT );
def("hSub",fptr_hSub_HNumberHNumberHComplex123STDITSTDITSTDIT );
def("hSub",fptr_hSub_HNumberHNumberHNumber123STDITSTDITSTDIT );
def("hSub",fptr_hSub_HNumberHNumberHInteger123STDITSTDITSTDIT );
def("hSub",fptr_hSub_HNumberHIntegerHComplex123STDITSTDITSTDIT );
def("hSub",fptr_hSub_HNumberHIntegerHNumber123STDITSTDITSTDIT );
def("hSub",fptr_hSub_HNumberHIntegerHInteger123STDITSTDITSTDIT );
def("hSub",fptr_hSub_HIntegerHComplexHComplex123STDITSTDITSTDIT );
def("hSub",fptr_hSub_HIntegerHComplexHNumber123STDITSTDITSTDIT );
def("hSub",fptr_hSub_HIntegerHComplexHInteger123STDITSTDITSTDIT );
def("hSub",fptr_hSub_HIntegerHNumberHComplex123STDITSTDITSTDIT );
def("hSub",fptr_hSub_HIntegerHNumberHNumber123STDITSTDITSTDIT );
def("hSub",fptr_hSub_HIntegerHNumberHInteger123STDITSTDITSTDIT );
def("hSub",fptr_hSub_HIntegerHIntegerHComplex123STDITSTDITSTDIT );
def("hSub",fptr_hSub_HIntegerHIntegerHNumber123STDITSTDITSTDIT );
def("hSub",fptr_hSub_HIntegerHIntegerHInteger123STDITSTDITSTDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hSubAdd",fptr_hSubAdd_HComplex111STDITSTDITSTDIT );
def("hSubAdd",fptr_hSubAdd_HNumber111STDITSTDITSTDIT );
def("hSubAdd",fptr_hSubAdd_HInteger111STDITSTDITSTDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hSubAddConv",fptr_hSubAddConv_HComplexHComplexHComplex123STDITSTDITSTDIT );
def("hSubAddConv",fptr_hSubAddConv_HComplexHComplexHNumber123STDITSTDITSTDIT );
def("hSubAddConv",fptr_hSubAddConv_HComplexHComplexHInteger123STDITSTDITSTDIT );
def("hSubAddConv",fptr_hSubAddConv_HComplexHNumberHComplex123STDITSTDITSTDIT );
def("hSubAddConv",fptr_hSubAddConv_HComplexHNumberHNumber123STDITSTDITSTDIT );
def("hSubAddConv",fptr_hSubAddConv_HComplexHNumberHInteger123STDITSTDITSTDIT );
def("hSubAddConv",fptr_hSubAddConv_HComplexHIntegerHComplex123STDITSTDITSTDIT );
def("hSubAddConv",fptr_hSubAddConv_HComplexHIntegerHNumber123STDITSTDITSTDIT );
def("hSubAddConv",fptr_hSubAddConv_HComplexHIntegerHInteger123STDITSTDITSTDIT );
def("hSubAddConv",fptr_hSubAddConv_HNumberHComplexHComplex123STDITSTDITSTDIT );
def("hSubAddConv",fptr_hSubAddConv_HNumberHComplexHNumber123STDITSTDITSTDIT );
def("hSubAddConv",fptr_hSubAddConv_HNumberHComplexHInteger123STDITSTDITSTDIT );
def("hSubAddConv",fptr_hSubAddConv_HNumberHNumberHComplex123STDITSTDITSTDIT );
def("hSubAddConv",fptr_hSubAddConv_HNumberHNumberHNumber123STDITSTDITSTDIT );
def("hSubAddConv",fptr_hSubAddConv_HNumberHNumberHInteger123STDITSTDITSTDIT );
def("hSubAddConv",fptr_hSubAddConv_HNumberHIntegerHComplex123STDITSTDITSTDIT );
def("hSubAddConv",fptr_hSubAddConv_HNumberHIntegerHNumber123STDITSTDITSTDIT );
def("hSubAddConv",fptr_hSubAddConv_HNumberHIntegerHInteger123STDITSTDITSTDIT );
def("hSubAddConv",fptr_hSubAddConv_HIntegerHComplexHComplex123STDITSTDITSTDIT );
def("hSubAddConv",fptr_hSubAddConv_HIntegerHComplexHNumber123STDITSTDITSTDIT );
def("hSubAddConv",fptr_hSubAddConv_HIntegerHComplexHInteger123STDITSTDITSTDIT );
def("hSubAddConv",fptr_hSubAddConv_HIntegerHNumberHComplex123STDITSTDITSTDIT );
def("hSubAddConv",fptr_hSubAddConv_HIntegerHNumberHNumber123STDITSTDITSTDIT );
def("hSubAddConv",fptr_hSubAddConv_HIntegerHNumberHInteger123STDITSTDITSTDIT );
def("hSubAddConv",fptr_hSubAddConv_HIntegerHIntegerHComplex123STDITSTDITSTDIT );
def("hSubAddConv",fptr_hSubAddConv_HIntegerHIntegerHNumber123STDITSTDITSTDIT );
def("hSubAddConv",fptr_hSubAddConv_HIntegerHIntegerHInteger123STDITSTDITSTDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hSub",fptr_hSub2_HComplexHComplexHComplex123STDITSTDITSTDIT );
def("hSub",fptr_hSub2_HComplexHComplexHNumber123STDITSTDITSTDIT );
def("hSub",fptr_hSub2_HComplexHComplexHInteger123STDITSTDITSTDIT );
def("hSub",fptr_hSub2_HComplexHNumberHComplex123STDITSTDITSTDIT );
def("hSub",fptr_hSub2_HComplexHNumberHNumber123STDITSTDITSTDIT );
def("hSub",fptr_hSub2_HComplexHNumberHInteger123STDITSTDITSTDIT );
def("hSub",fptr_hSub2_HComplexHIntegerHComplex123STDITSTDITSTDIT );
def("hSub",fptr_hSub2_HComplexHIntegerHNumber123STDITSTDITSTDIT );
def("hSub",fptr_hSub2_HComplexHIntegerHInteger123STDITSTDITSTDIT );
def("hSub",fptr_hSub2_HNumberHComplexHComplex123STDITSTDITSTDIT );
def("hSub",fptr_hSub2_HNumberHComplexHNumber123STDITSTDITSTDIT );
def("hSub",fptr_hSub2_HNumberHComplexHInteger123STDITSTDITSTDIT );
def("hSub",fptr_hSub2_HNumberHNumberHComplex123STDITSTDITSTDIT );
def("hSub",fptr_hSub2_HNumberHNumberHNumber123STDITSTDITSTDIT );
def("hSub",fptr_hSub2_HNumberHNumberHInteger123STDITSTDITSTDIT );
def("hSub",fptr_hSub2_HNumberHIntegerHComplex123STDITSTDITSTDIT );
def("hSub",fptr_hSub2_HNumberHIntegerHNumber123STDITSTDITSTDIT );
def("hSub",fptr_hSub2_HNumberHIntegerHInteger123STDITSTDITSTDIT );
def("hSub",fptr_hSub2_HIntegerHComplexHComplex123STDITSTDITSTDIT );
def("hSub",fptr_hSub2_HIntegerHComplexHNumber123STDITSTDITSTDIT );
def("hSub",fptr_hSub2_HIntegerHComplexHInteger123STDITSTDITSTDIT );
def("hSub",fptr_hSub2_HIntegerHNumberHComplex123STDITSTDITSTDIT );
def("hSub",fptr_hSub2_HIntegerHNumberHNumber123STDITSTDITSTDIT );
def("hSub",fptr_hSub2_HIntegerHNumberHInteger123STDITSTDITSTDIT );
def("hSub",fptr_hSub2_HIntegerHIntegerHComplex123STDITSTDITSTDIT );
def("hSub",fptr_hSub2_HIntegerHIntegerHNumber123STDITSTDITSTDIT );
def("hSub",fptr_hSub2_HIntegerHIntegerHInteger123STDITSTDITSTDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hiMul",fptr_hiMul_HComplexHComplex12STDITSTDIT );
def("hiMul",fptr_hiMul_HComplexHNumber12STDITSTDIT );
def("hiMul",fptr_hiMul_HComplexHInteger12STDITSTDIT );
def("hiMul",fptr_hiMul_HNumberHComplex12STDITSTDIT );
def("hiMul",fptr_hiMul_HNumberHNumber12STDITSTDIT );
def("hiMul",fptr_hiMul_HNumberHInteger12STDITSTDIT );
def("hiMul",fptr_hiMul_HIntegerHComplex12STDITSTDIT );
def("hiMul",fptr_hiMul_HIntegerHNumber12STDITSTDIT );
def("hiMul",fptr_hiMul_HIntegerHInteger12STDITSTDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hiMul",fptr_hiMul2_HComplexHComplex12STDIT );
def("hiMul",fptr_hiMul2_HComplexHNumber12STDIT );
def("hiMul",fptr_hiMul2_HComplexHInteger12STDIT );
def("hiMul",fptr_hiMul2_HNumberHComplex12STDIT );
def("hiMul",fptr_hiMul2_HNumberHNumber12STDIT );
def("hiMul",fptr_hiMul2_HNumberHInteger12STDIT );
def("hiMul",fptr_hiMul2_HIntegerHComplex12STDIT );
def("hiMul",fptr_hiMul2_HIntegerHNumber12STDIT );
def("hiMul",fptr_hiMul2_HIntegerHInteger12STDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hMul",fptr_hMul_HComplexHComplexHComplex123STDITSTDITSTDIT );
def("hMul",fptr_hMul_HComplexHComplexHNumber123STDITSTDITSTDIT );
def("hMul",fptr_hMul_HComplexHComplexHInteger123STDITSTDITSTDIT );
def("hMul",fptr_hMul_HComplexHNumberHComplex123STDITSTDITSTDIT );
def("hMul",fptr_hMul_HComplexHNumberHNumber123STDITSTDITSTDIT );
def("hMul",fptr_hMul_HComplexHNumberHInteger123STDITSTDITSTDIT );
def("hMul",fptr_hMul_HComplexHIntegerHComplex123STDITSTDITSTDIT );
def("hMul",fptr_hMul_HComplexHIntegerHNumber123STDITSTDITSTDIT );
def("hMul",fptr_hMul_HComplexHIntegerHInteger123STDITSTDITSTDIT );
def("hMul",fptr_hMul_HNumberHComplexHComplex123STDITSTDITSTDIT );
def("hMul",fptr_hMul_HNumberHComplexHNumber123STDITSTDITSTDIT );
def("hMul",fptr_hMul_HNumberHComplexHInteger123STDITSTDITSTDIT );
def("hMul",fptr_hMul_HNumberHNumberHComplex123STDITSTDITSTDIT );
def("hMul",fptr_hMul_HNumberHNumberHNumber123STDITSTDITSTDIT );
def("hMul",fptr_hMul_HNumberHNumberHInteger123STDITSTDITSTDIT );
def("hMul",fptr_hMul_HNumberHIntegerHComplex123STDITSTDITSTDIT );
def("hMul",fptr_hMul_HNumberHIntegerHNumber123STDITSTDITSTDIT );
def("hMul",fptr_hMul_HNumberHIntegerHInteger123STDITSTDITSTDIT );
def("hMul",fptr_hMul_HIntegerHComplexHComplex123STDITSTDITSTDIT );
def("hMul",fptr_hMul_HIntegerHComplexHNumber123STDITSTDITSTDIT );
def("hMul",fptr_hMul_HIntegerHComplexHInteger123STDITSTDITSTDIT );
def("hMul",fptr_hMul_HIntegerHNumberHComplex123STDITSTDITSTDIT );
def("hMul",fptr_hMul_HIntegerHNumberHNumber123STDITSTDITSTDIT );
def("hMul",fptr_hMul_HIntegerHNumberHInteger123STDITSTDITSTDIT );
def("hMul",fptr_hMul_HIntegerHIntegerHComplex123STDITSTDITSTDIT );
def("hMul",fptr_hMul_HIntegerHIntegerHNumber123STDITSTDITSTDIT );
def("hMul",fptr_hMul_HIntegerHIntegerHInteger123STDITSTDITSTDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hMulAdd",fptr_hMulAdd_HComplex111STDITSTDITSTDIT );
def("hMulAdd",fptr_hMulAdd_HNumber111STDITSTDITSTDIT );
def("hMulAdd",fptr_hMulAdd_HInteger111STDITSTDITSTDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hMulAddConv",fptr_hMulAddConv_HComplexHComplexHComplex123STDITSTDITSTDIT );
def("hMulAddConv",fptr_hMulAddConv_HComplexHComplexHNumber123STDITSTDITSTDIT );
def("hMulAddConv",fptr_hMulAddConv_HComplexHComplexHInteger123STDITSTDITSTDIT );
def("hMulAddConv",fptr_hMulAddConv_HComplexHNumberHComplex123STDITSTDITSTDIT );
def("hMulAddConv",fptr_hMulAddConv_HComplexHNumberHNumber123STDITSTDITSTDIT );
def("hMulAddConv",fptr_hMulAddConv_HComplexHNumberHInteger123STDITSTDITSTDIT );
def("hMulAddConv",fptr_hMulAddConv_HComplexHIntegerHComplex123STDITSTDITSTDIT );
def("hMulAddConv",fptr_hMulAddConv_HComplexHIntegerHNumber123STDITSTDITSTDIT );
def("hMulAddConv",fptr_hMulAddConv_HComplexHIntegerHInteger123STDITSTDITSTDIT );
def("hMulAddConv",fptr_hMulAddConv_HNumberHComplexHComplex123STDITSTDITSTDIT );
def("hMulAddConv",fptr_hMulAddConv_HNumberHComplexHNumber123STDITSTDITSTDIT );
def("hMulAddConv",fptr_hMulAddConv_HNumberHComplexHInteger123STDITSTDITSTDIT );
def("hMulAddConv",fptr_hMulAddConv_HNumberHNumberHComplex123STDITSTDITSTDIT );
def("hMulAddConv",fptr_hMulAddConv_HNumberHNumberHNumber123STDITSTDITSTDIT );
def("hMulAddConv",fptr_hMulAddConv_HNumberHNumberHInteger123STDITSTDITSTDIT );
def("hMulAddConv",fptr_hMulAddConv_HNumberHIntegerHComplex123STDITSTDITSTDIT );
def("hMulAddConv",fptr_hMulAddConv_HNumberHIntegerHNumber123STDITSTDITSTDIT );
def("hMulAddConv",fptr_hMulAddConv_HNumberHIntegerHInteger123STDITSTDITSTDIT );
def("hMulAddConv",fptr_hMulAddConv_HIntegerHComplexHComplex123STDITSTDITSTDIT );
def("hMulAddConv",fptr_hMulAddConv_HIntegerHComplexHNumber123STDITSTDITSTDIT );
def("hMulAddConv",fptr_hMulAddConv_HIntegerHComplexHInteger123STDITSTDITSTDIT );
def("hMulAddConv",fptr_hMulAddConv_HIntegerHNumberHComplex123STDITSTDITSTDIT );
def("hMulAddConv",fptr_hMulAddConv_HIntegerHNumberHNumber123STDITSTDITSTDIT );
def("hMulAddConv",fptr_hMulAddConv_HIntegerHNumberHInteger123STDITSTDITSTDIT );
def("hMulAddConv",fptr_hMulAddConv_HIntegerHIntegerHComplex123STDITSTDITSTDIT );
def("hMulAddConv",fptr_hMulAddConv_HIntegerHIntegerHNumber123STDITSTDITSTDIT );
def("hMulAddConv",fptr_hMulAddConv_HIntegerHIntegerHInteger123STDITSTDITSTDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hMul",fptr_hMul2_HComplexHComplexHComplex123STDITSTDITSTDIT );
def("hMul",fptr_hMul2_HComplexHComplexHNumber123STDITSTDITSTDIT );
def("hMul",fptr_hMul2_HComplexHComplexHInteger123STDITSTDITSTDIT );
def("hMul",fptr_hMul2_HComplexHNumberHComplex123STDITSTDITSTDIT );
def("hMul",fptr_hMul2_HComplexHNumberHNumber123STDITSTDITSTDIT );
def("hMul",fptr_hMul2_HComplexHNumberHInteger123STDITSTDITSTDIT );
def("hMul",fptr_hMul2_HComplexHIntegerHComplex123STDITSTDITSTDIT );
def("hMul",fptr_hMul2_HComplexHIntegerHNumber123STDITSTDITSTDIT );
def("hMul",fptr_hMul2_HComplexHIntegerHInteger123STDITSTDITSTDIT );
def("hMul",fptr_hMul2_HNumberHComplexHComplex123STDITSTDITSTDIT );
def("hMul",fptr_hMul2_HNumberHComplexHNumber123STDITSTDITSTDIT );
def("hMul",fptr_hMul2_HNumberHComplexHInteger123STDITSTDITSTDIT );
def("hMul",fptr_hMul2_HNumberHNumberHComplex123STDITSTDITSTDIT );
def("hMul",fptr_hMul2_HNumberHNumberHNumber123STDITSTDITSTDIT );
def("hMul",fptr_hMul2_HNumberHNumberHInteger123STDITSTDITSTDIT );
def("hMul",fptr_hMul2_HNumberHIntegerHComplex123STDITSTDITSTDIT );
def("hMul",fptr_hMul2_HNumberHIntegerHNumber123STDITSTDITSTDIT );
def("hMul",fptr_hMul2_HNumberHIntegerHInteger123STDITSTDITSTDIT );
def("hMul",fptr_hMul2_HIntegerHComplexHComplex123STDITSTDITSTDIT );
def("hMul",fptr_hMul2_HIntegerHComplexHNumber123STDITSTDITSTDIT );
def("hMul",fptr_hMul2_HIntegerHComplexHInteger123STDITSTDITSTDIT );
def("hMul",fptr_hMul2_HIntegerHNumberHComplex123STDITSTDITSTDIT );
def("hMul",fptr_hMul2_HIntegerHNumberHNumber123STDITSTDITSTDIT );
def("hMul",fptr_hMul2_HIntegerHNumberHInteger123STDITSTDITSTDIT );
def("hMul",fptr_hMul2_HIntegerHIntegerHComplex123STDITSTDITSTDIT );
def("hMul",fptr_hMul2_HIntegerHIntegerHNumber123STDITSTDITSTDIT );
def("hMul",fptr_hMul2_HIntegerHIntegerHInteger123STDITSTDITSTDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hiAdd",fptr_hiAdd_HComplexHComplex12STDITSTDIT );
def("hiAdd",fptr_hiAdd_HComplexHNumber12STDITSTDIT );
def("hiAdd",fptr_hiAdd_HComplexHInteger12STDITSTDIT );
def("hiAdd",fptr_hiAdd_HNumberHComplex12STDITSTDIT );
def("hiAdd",fptr_hiAdd_HNumberHNumber12STDITSTDIT );
def("hiAdd",fptr_hiAdd_HNumberHInteger12STDITSTDIT );
def("hiAdd",fptr_hiAdd_HIntegerHComplex12STDITSTDIT );
def("hiAdd",fptr_hiAdd_HIntegerHNumber12STDITSTDIT );
def("hiAdd",fptr_hiAdd_HIntegerHInteger12STDITSTDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hiAdd",fptr_hiAdd2_HComplexHComplex12STDIT );
def("hiAdd",fptr_hiAdd2_HComplexHNumber12STDIT );
def("hiAdd",fptr_hiAdd2_HComplexHInteger12STDIT );
def("hiAdd",fptr_hiAdd2_HNumberHComplex12STDIT );
def("hiAdd",fptr_hiAdd2_HNumberHNumber12STDIT );
def("hiAdd",fptr_hiAdd2_HNumberHInteger12STDIT );
def("hiAdd",fptr_hiAdd2_HIntegerHComplex12STDIT );
def("hiAdd",fptr_hiAdd2_HIntegerHNumber12STDIT );
def("hiAdd",fptr_hiAdd2_HIntegerHInteger12STDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hAdd",fptr_hAdd_HComplexHComplexHComplex123STDITSTDITSTDIT );
def("hAdd",fptr_hAdd_HComplexHComplexHNumber123STDITSTDITSTDIT );
def("hAdd",fptr_hAdd_HComplexHComplexHInteger123STDITSTDITSTDIT );
def("hAdd",fptr_hAdd_HComplexHNumberHComplex123STDITSTDITSTDIT );
def("hAdd",fptr_hAdd_HComplexHNumberHNumber123STDITSTDITSTDIT );
def("hAdd",fptr_hAdd_HComplexHNumberHInteger123STDITSTDITSTDIT );
def("hAdd",fptr_hAdd_HComplexHIntegerHComplex123STDITSTDITSTDIT );
def("hAdd",fptr_hAdd_HComplexHIntegerHNumber123STDITSTDITSTDIT );
def("hAdd",fptr_hAdd_HComplexHIntegerHInteger123STDITSTDITSTDIT );
def("hAdd",fptr_hAdd_HNumberHComplexHComplex123STDITSTDITSTDIT );
def("hAdd",fptr_hAdd_HNumberHComplexHNumber123STDITSTDITSTDIT );
def("hAdd",fptr_hAdd_HNumberHComplexHInteger123STDITSTDITSTDIT );
def("hAdd",fptr_hAdd_HNumberHNumberHComplex123STDITSTDITSTDIT );
def("hAdd",fptr_hAdd_HNumberHNumberHNumber123STDITSTDITSTDIT );
def("hAdd",fptr_hAdd_HNumberHNumberHInteger123STDITSTDITSTDIT );
def("hAdd",fptr_hAdd_HNumberHIntegerHComplex123STDITSTDITSTDIT );
def("hAdd",fptr_hAdd_HNumberHIntegerHNumber123STDITSTDITSTDIT );
def("hAdd",fptr_hAdd_HNumberHIntegerHInteger123STDITSTDITSTDIT );
def("hAdd",fptr_hAdd_HIntegerHComplexHComplex123STDITSTDITSTDIT );
def("hAdd",fptr_hAdd_HIntegerHComplexHNumber123STDITSTDITSTDIT );
def("hAdd",fptr_hAdd_HIntegerHComplexHInteger123STDITSTDITSTDIT );
def("hAdd",fptr_hAdd_HIntegerHNumberHComplex123STDITSTDITSTDIT );
def("hAdd",fptr_hAdd_HIntegerHNumberHNumber123STDITSTDITSTDIT );
def("hAdd",fptr_hAdd_HIntegerHNumberHInteger123STDITSTDITSTDIT );
def("hAdd",fptr_hAdd_HIntegerHIntegerHComplex123STDITSTDITSTDIT );
def("hAdd",fptr_hAdd_HIntegerHIntegerHNumber123STDITSTDITSTDIT );
def("hAdd",fptr_hAdd_HIntegerHIntegerHInteger123STDITSTDITSTDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hAddAdd",fptr_hAddAdd_HComplex111STDITSTDITSTDIT );
def("hAddAdd",fptr_hAddAdd_HNumber111STDITSTDITSTDIT );
def("hAddAdd",fptr_hAddAdd_HInteger111STDITSTDITSTDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hAddAddConv",fptr_hAddAddConv_HComplexHComplexHComplex123STDITSTDITSTDIT );
def("hAddAddConv",fptr_hAddAddConv_HComplexHComplexHNumber123STDITSTDITSTDIT );
def("hAddAddConv",fptr_hAddAddConv_HComplexHComplexHInteger123STDITSTDITSTDIT );
def("hAddAddConv",fptr_hAddAddConv_HComplexHNumberHComplex123STDITSTDITSTDIT );
def("hAddAddConv",fptr_hAddAddConv_HComplexHNumberHNumber123STDITSTDITSTDIT );
def("hAddAddConv",fptr_hAddAddConv_HComplexHNumberHInteger123STDITSTDITSTDIT );
def("hAddAddConv",fptr_hAddAddConv_HComplexHIntegerHComplex123STDITSTDITSTDIT );
def("hAddAddConv",fptr_hAddAddConv_HComplexHIntegerHNumber123STDITSTDITSTDIT );
def("hAddAddConv",fptr_hAddAddConv_HComplexHIntegerHInteger123STDITSTDITSTDIT );
def("hAddAddConv",fptr_hAddAddConv_HNumberHComplexHComplex123STDITSTDITSTDIT );
def("hAddAddConv",fptr_hAddAddConv_HNumberHComplexHNumber123STDITSTDITSTDIT );
def("hAddAddConv",fptr_hAddAddConv_HNumberHComplexHInteger123STDITSTDITSTDIT );
def("hAddAddConv",fptr_hAddAddConv_HNumberHNumberHComplex123STDITSTDITSTDIT );
def("hAddAddConv",fptr_hAddAddConv_HNumberHNumberHNumber123STDITSTDITSTDIT );
def("hAddAddConv",fptr_hAddAddConv_HNumberHNumberHInteger123STDITSTDITSTDIT );
def("hAddAddConv",fptr_hAddAddConv_HNumberHIntegerHComplex123STDITSTDITSTDIT );
def("hAddAddConv",fptr_hAddAddConv_HNumberHIntegerHNumber123STDITSTDITSTDIT );
def("hAddAddConv",fptr_hAddAddConv_HNumberHIntegerHInteger123STDITSTDITSTDIT );
def("hAddAddConv",fptr_hAddAddConv_HIntegerHComplexHComplex123STDITSTDITSTDIT );
def("hAddAddConv",fptr_hAddAddConv_HIntegerHComplexHNumber123STDITSTDITSTDIT );
def("hAddAddConv",fptr_hAddAddConv_HIntegerHComplexHInteger123STDITSTDITSTDIT );
def("hAddAddConv",fptr_hAddAddConv_HIntegerHNumberHComplex123STDITSTDITSTDIT );
def("hAddAddConv",fptr_hAddAddConv_HIntegerHNumberHNumber123STDITSTDITSTDIT );
def("hAddAddConv",fptr_hAddAddConv_HIntegerHNumberHInteger123STDITSTDITSTDIT );
def("hAddAddConv",fptr_hAddAddConv_HIntegerHIntegerHComplex123STDITSTDITSTDIT );
def("hAddAddConv",fptr_hAddAddConv_HIntegerHIntegerHNumber123STDITSTDITSTDIT );
def("hAddAddConv",fptr_hAddAddConv_HIntegerHIntegerHInteger123STDITSTDITSTDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hAdd",fptr_hAdd2_HComplexHComplexHComplex123STDITSTDITSTDIT );
def("hAdd",fptr_hAdd2_HComplexHComplexHNumber123STDITSTDITSTDIT );
def("hAdd",fptr_hAdd2_HComplexHComplexHInteger123STDITSTDITSTDIT );
def("hAdd",fptr_hAdd2_HComplexHNumberHComplex123STDITSTDITSTDIT );
def("hAdd",fptr_hAdd2_HComplexHNumberHNumber123STDITSTDITSTDIT );
def("hAdd",fptr_hAdd2_HComplexHNumberHInteger123STDITSTDITSTDIT );
def("hAdd",fptr_hAdd2_HComplexHIntegerHComplex123STDITSTDITSTDIT );
def("hAdd",fptr_hAdd2_HComplexHIntegerHNumber123STDITSTDITSTDIT );
def("hAdd",fptr_hAdd2_HComplexHIntegerHInteger123STDITSTDITSTDIT );
def("hAdd",fptr_hAdd2_HNumberHComplexHComplex123STDITSTDITSTDIT );
def("hAdd",fptr_hAdd2_HNumberHComplexHNumber123STDITSTDITSTDIT );
def("hAdd",fptr_hAdd2_HNumberHComplexHInteger123STDITSTDITSTDIT );
def("hAdd",fptr_hAdd2_HNumberHNumberHComplex123STDITSTDITSTDIT );
def("hAdd",fptr_hAdd2_HNumberHNumberHNumber123STDITSTDITSTDIT );
def("hAdd",fptr_hAdd2_HNumberHNumberHInteger123STDITSTDITSTDIT );
def("hAdd",fptr_hAdd2_HNumberHIntegerHComplex123STDITSTDITSTDIT );
def("hAdd",fptr_hAdd2_HNumberHIntegerHNumber123STDITSTDITSTDIT );
def("hAdd",fptr_hAdd2_HNumberHIntegerHInteger123STDITSTDITSTDIT );
def("hAdd",fptr_hAdd2_HIntegerHComplexHComplex123STDITSTDITSTDIT );
def("hAdd",fptr_hAdd2_HIntegerHComplexHNumber123STDITSTDITSTDIT );
def("hAdd",fptr_hAdd2_HIntegerHComplexHInteger123STDITSTDITSTDIT );
def("hAdd",fptr_hAdd2_HIntegerHNumberHComplex123STDITSTDITSTDIT );
def("hAdd",fptr_hAdd2_HIntegerHNumberHNumber123STDITSTDITSTDIT );
def("hAdd",fptr_hAdd2_HIntegerHNumberHInteger123STDITSTDITSTDIT );
def("hAdd",fptr_hAdd2_HIntegerHIntegerHComplex123STDITSTDITSTDIT );
def("hAdd",fptr_hAdd2_HIntegerHIntegerHNumber123STDITSTDITSTDIT );
def("hAdd",fptr_hAdd2_HIntegerHIntegerHInteger123STDITSTDITSTDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hiDiv",fptr_hiDiv_HComplexHComplex12STDITSTDIT );
def("hiDiv",fptr_hiDiv_HComplexHNumber12STDITSTDIT );
def("hiDiv",fptr_hiDiv_HComplexHInteger12STDITSTDIT );
def("hiDiv",fptr_hiDiv_HNumberHComplex12STDITSTDIT );
def("hiDiv",fptr_hiDiv_HNumberHNumber12STDITSTDIT );
def("hiDiv",fptr_hiDiv_HNumberHInteger12STDITSTDIT );
def("hiDiv",fptr_hiDiv_HIntegerHComplex12STDITSTDIT );
def("hiDiv",fptr_hiDiv_HIntegerHNumber12STDITSTDIT );
def("hiDiv",fptr_hiDiv_HIntegerHInteger12STDITSTDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hiDiv",fptr_hiDiv2_HComplexHComplex12STDIT );
def("hiDiv",fptr_hiDiv2_HComplexHNumber12STDIT );
def("hiDiv",fptr_hiDiv2_HComplexHInteger12STDIT );
def("hiDiv",fptr_hiDiv2_HNumberHComplex12STDIT );
def("hiDiv",fptr_hiDiv2_HNumberHNumber12STDIT );
def("hiDiv",fptr_hiDiv2_HNumberHInteger12STDIT );
def("hiDiv",fptr_hiDiv2_HIntegerHComplex12STDIT );
def("hiDiv",fptr_hiDiv2_HIntegerHNumber12STDIT );
def("hiDiv",fptr_hiDiv2_HIntegerHInteger12STDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hDiv",fptr_hDiv_HComplexHComplexHComplex123STDITSTDITSTDIT );
def("hDiv",fptr_hDiv_HComplexHComplexHNumber123STDITSTDITSTDIT );
def("hDiv",fptr_hDiv_HComplexHComplexHInteger123STDITSTDITSTDIT );
def("hDiv",fptr_hDiv_HComplexHNumberHComplex123STDITSTDITSTDIT );
def("hDiv",fptr_hDiv_HComplexHNumberHNumber123STDITSTDITSTDIT );
def("hDiv",fptr_hDiv_HComplexHNumberHInteger123STDITSTDITSTDIT );
def("hDiv",fptr_hDiv_HComplexHIntegerHComplex123STDITSTDITSTDIT );
def("hDiv",fptr_hDiv_HComplexHIntegerHNumber123STDITSTDITSTDIT );
def("hDiv",fptr_hDiv_HComplexHIntegerHInteger123STDITSTDITSTDIT );
def("hDiv",fptr_hDiv_HNumberHComplexHComplex123STDITSTDITSTDIT );
def("hDiv",fptr_hDiv_HNumberHComplexHNumber123STDITSTDITSTDIT );
def("hDiv",fptr_hDiv_HNumberHComplexHInteger123STDITSTDITSTDIT );
def("hDiv",fptr_hDiv_HNumberHNumberHComplex123STDITSTDITSTDIT );
def("hDiv",fptr_hDiv_HNumberHNumberHNumber123STDITSTDITSTDIT );
def("hDiv",fptr_hDiv_HNumberHNumberHInteger123STDITSTDITSTDIT );
def("hDiv",fptr_hDiv_HNumberHIntegerHComplex123STDITSTDITSTDIT );
def("hDiv",fptr_hDiv_HNumberHIntegerHNumber123STDITSTDITSTDIT );
def("hDiv",fptr_hDiv_HNumberHIntegerHInteger123STDITSTDITSTDIT );
def("hDiv",fptr_hDiv_HIntegerHComplexHComplex123STDITSTDITSTDIT );
def("hDiv",fptr_hDiv_HIntegerHComplexHNumber123STDITSTDITSTDIT );
def("hDiv",fptr_hDiv_HIntegerHComplexHInteger123STDITSTDITSTDIT );
def("hDiv",fptr_hDiv_HIntegerHNumberHComplex123STDITSTDITSTDIT );
def("hDiv",fptr_hDiv_HIntegerHNumberHNumber123STDITSTDITSTDIT );
def("hDiv",fptr_hDiv_HIntegerHNumberHInteger123STDITSTDITSTDIT );
def("hDiv",fptr_hDiv_HIntegerHIntegerHComplex123STDITSTDITSTDIT );
def("hDiv",fptr_hDiv_HIntegerHIntegerHNumber123STDITSTDITSTDIT );
def("hDiv",fptr_hDiv_HIntegerHIntegerHInteger123STDITSTDITSTDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hDivAdd",fptr_hDivAdd_HComplex111STDITSTDITSTDIT );
def("hDivAdd",fptr_hDivAdd_HNumber111STDITSTDITSTDIT );
def("hDivAdd",fptr_hDivAdd_HInteger111STDITSTDITSTDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hDivAddConv",fptr_hDivAddConv_HComplexHComplexHComplex123STDITSTDITSTDIT );
def("hDivAddConv",fptr_hDivAddConv_HComplexHComplexHNumber123STDITSTDITSTDIT );
def("hDivAddConv",fptr_hDivAddConv_HComplexHComplexHInteger123STDITSTDITSTDIT );
def("hDivAddConv",fptr_hDivAddConv_HComplexHNumberHComplex123STDITSTDITSTDIT );
def("hDivAddConv",fptr_hDivAddConv_HComplexHNumberHNumber123STDITSTDITSTDIT );
def("hDivAddConv",fptr_hDivAddConv_HComplexHNumberHInteger123STDITSTDITSTDIT );
def("hDivAddConv",fptr_hDivAddConv_HComplexHIntegerHComplex123STDITSTDITSTDIT );
def("hDivAddConv",fptr_hDivAddConv_HComplexHIntegerHNumber123STDITSTDITSTDIT );
def("hDivAddConv",fptr_hDivAddConv_HComplexHIntegerHInteger123STDITSTDITSTDIT );
def("hDivAddConv",fptr_hDivAddConv_HNumberHComplexHComplex123STDITSTDITSTDIT );
def("hDivAddConv",fptr_hDivAddConv_HNumberHComplexHNumber123STDITSTDITSTDIT );
def("hDivAddConv",fptr_hDivAddConv_HNumberHComplexHInteger123STDITSTDITSTDIT );
def("hDivAddConv",fptr_hDivAddConv_HNumberHNumberHComplex123STDITSTDITSTDIT );
def("hDivAddConv",fptr_hDivAddConv_HNumberHNumberHNumber123STDITSTDITSTDIT );
def("hDivAddConv",fptr_hDivAddConv_HNumberHNumberHInteger123STDITSTDITSTDIT );
def("hDivAddConv",fptr_hDivAddConv_HNumberHIntegerHComplex123STDITSTDITSTDIT );
def("hDivAddConv",fptr_hDivAddConv_HNumberHIntegerHNumber123STDITSTDITSTDIT );
def("hDivAddConv",fptr_hDivAddConv_HNumberHIntegerHInteger123STDITSTDITSTDIT );
def("hDivAddConv",fptr_hDivAddConv_HIntegerHComplexHComplex123STDITSTDITSTDIT );
def("hDivAddConv",fptr_hDivAddConv_HIntegerHComplexHNumber123STDITSTDITSTDIT );
def("hDivAddConv",fptr_hDivAddConv_HIntegerHComplexHInteger123STDITSTDITSTDIT );
def("hDivAddConv",fptr_hDivAddConv_HIntegerHNumberHComplex123STDITSTDITSTDIT );
def("hDivAddConv",fptr_hDivAddConv_HIntegerHNumberHNumber123STDITSTDITSTDIT );
def("hDivAddConv",fptr_hDivAddConv_HIntegerHNumberHInteger123STDITSTDITSTDIT );
def("hDivAddConv",fptr_hDivAddConv_HIntegerHIntegerHComplex123STDITSTDITSTDIT );
def("hDivAddConv",fptr_hDivAddConv_HIntegerHIntegerHNumber123STDITSTDITSTDIT );
def("hDivAddConv",fptr_hDivAddConv_HIntegerHIntegerHInteger123STDITSTDITSTDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hDiv",fptr_hDiv2_HComplexHComplexHComplex123STDITSTDITSTDIT );
def("hDiv",fptr_hDiv2_HComplexHComplexHNumber123STDITSTDITSTDIT );
def("hDiv",fptr_hDiv2_HComplexHComplexHInteger123STDITSTDITSTDIT );
def("hDiv",fptr_hDiv2_HComplexHNumberHComplex123STDITSTDITSTDIT );
def("hDiv",fptr_hDiv2_HComplexHNumberHNumber123STDITSTDITSTDIT );
def("hDiv",fptr_hDiv2_HComplexHNumberHInteger123STDITSTDITSTDIT );
def("hDiv",fptr_hDiv2_HComplexHIntegerHComplex123STDITSTDITSTDIT );
def("hDiv",fptr_hDiv2_HComplexHIntegerHNumber123STDITSTDITSTDIT );
def("hDiv",fptr_hDiv2_HComplexHIntegerHInteger123STDITSTDITSTDIT );
def("hDiv",fptr_hDiv2_HNumberHComplexHComplex123STDITSTDITSTDIT );
def("hDiv",fptr_hDiv2_HNumberHComplexHNumber123STDITSTDITSTDIT );
def("hDiv",fptr_hDiv2_HNumberHComplexHInteger123STDITSTDITSTDIT );
def("hDiv",fptr_hDiv2_HNumberHNumberHComplex123STDITSTDITSTDIT );
def("hDiv",fptr_hDiv2_HNumberHNumberHNumber123STDITSTDITSTDIT );
def("hDiv",fptr_hDiv2_HNumberHNumberHInteger123STDITSTDITSTDIT );
def("hDiv",fptr_hDiv2_HNumberHIntegerHComplex123STDITSTDITSTDIT );
def("hDiv",fptr_hDiv2_HNumberHIntegerHNumber123STDITSTDITSTDIT );
def("hDiv",fptr_hDiv2_HNumberHIntegerHInteger123STDITSTDITSTDIT );
def("hDiv",fptr_hDiv2_HIntegerHComplexHComplex123STDITSTDITSTDIT );
def("hDiv",fptr_hDiv2_HIntegerHComplexHNumber123STDITSTDITSTDIT );
def("hDiv",fptr_hDiv2_HIntegerHComplexHInteger123STDITSTDITSTDIT );
def("hDiv",fptr_hDiv2_HIntegerHNumberHComplex123STDITSTDITSTDIT );
def("hDiv",fptr_hDiv2_HIntegerHNumberHNumber123STDITSTDITSTDIT );
def("hDiv",fptr_hDiv2_HIntegerHNumberHInteger123STDITSTDITSTDIT );
def("hDiv",fptr_hDiv2_HIntegerHIntegerHComplex123STDITSTDITSTDIT );
def("hDiv",fptr_hDiv2_HIntegerHIntegerHNumber123STDITSTDITSTDIT );
def("hDiv",fptr_hDiv2_HIntegerHIntegerHInteger123STDITSTDITSTDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hNegate",fptr_hNegate_HComplex1STDIT );
def("hNegate",fptr_hNegate_HNumber1STDIT );
def("hNegate",fptr_hNegate_HInteger1STDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hSum",fptr_hSum_HComplex1STDIT );
def("hSum",fptr_hSum_HNumber1STDIT );
def("hSum",fptr_hSum_HInteger1STDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hNorm",fptr_hNorm_HNumber1STDIT );
def("hNorm",fptr_hNorm_HInteger1STDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hNormalize",fptr_hNormalize_HNumber1STDIT );
def("hNormalize",fptr_hNormalize_HInteger1STDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hMean",fptr_hMean_HNumber1STDIT );
def("hMean",fptr_hMean_HInteger1STDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hSort",fptr_hSort_HComplex1STDIT );
def("hSort",fptr_hSort_HNumber1STDIT );
def("hSort",fptr_hSort_HInteger1STDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hSortMedian",fptr_hSortMedian_HNumber1STDIT );
def("hSortMedian",fptr_hSortMedian_HInteger1STDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers

def("hMedian",fptr_hMedian_HNumber1STL );
def("hMedian",fptr_hMedian_HInteger1STL );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hStdDev",fptr_hStdDev_HNumber1HNumberSTDIT );
def("hStdDev",fptr_hStdDev_HInteger1HNumberSTDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hStdDev",fptr_hStdDev_HNumber1STDIT );
def("hStdDev",fptr_hStdDev_HInteger1STDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hFindLessEqual",fptr_hFindLessEqual_HNumber11HIntegerSTDITSTDIT );
def("hFindLessEqual",fptr_hFindLessEqual_HInteger11HIntegerSTDITSTDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hFindLessEqualAbs",fptr_hFindLessEqualAbs_HNumber11HIntegerSTDITSTDIT );
def("hFindLessEqualAbs",fptr_hFindLessEqualAbs_HInteger11HIntegerSTDITSTDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hFindGreaterThan",fptr_hFindGreaterThan_HNumber11HIntegerSTDITSTDIT );
def("hFindGreaterThan",fptr_hFindGreaterThan_HInteger11HIntegerSTDITSTDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hFindGreaterThanAbs",fptr_hFindGreaterThanAbs_HNumber11HIntegerSTDITSTDIT );
def("hFindGreaterThanAbs",fptr_hFindGreaterThanAbs_HInteger11HIntegerSTDITSTDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hFindGreaterEqual",fptr_hFindGreaterEqual_HNumber11HIntegerSTDITSTDIT );
def("hFindGreaterEqual",fptr_hFindGreaterEqual_HInteger11HIntegerSTDITSTDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hFindGreaterEqualAbs",fptr_hFindGreaterEqualAbs_HNumber11HIntegerSTDITSTDIT );
def("hFindGreaterEqualAbs",fptr_hFindGreaterEqualAbs_HInteger11HIntegerSTDITSTDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hFindLessThan",fptr_hFindLessThan_HNumber11HIntegerSTDITSTDIT );
def("hFindLessThan",fptr_hFindLessThan_HInteger11HIntegerSTDITSTDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hFindLessThanAbs",fptr_hFindLessThanAbs_HNumber11HIntegerSTDITSTDIT );
def("hFindLessThanAbs",fptr_hFindLessThanAbs_HInteger11HIntegerSTDITSTDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hDownsample",fptr_hDownsample_HNumber11STDITSTDIT );
def("hDownsample",fptr_hDownsample_HInteger11STDITSTDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers

def("hDownsample",fptr_hDownsample_HNumber1HNumberSTL );
def("hDownsample",fptr_hDownsample_HInteger1HNumberSTL );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hFindLowerBound",fptr_hFindLowerBound_HComplex11STDIT );
def("hFindLowerBound",fptr_hFindLowerBound_HNumber11STDIT );
def("hFindLowerBound",fptr_hFindLowerBound_HInteger11STDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers

def("hFlatWeights",fptr_hFlatWeights_HIntegerHInteger );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers

def("hLinearWeights",fptr_hLinearWeights_HIntegerHInteger );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers

def("hGaussianWeights",fptr_hGaussianWeights_HIntegerHInteger );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers

def("hWeights",fptr_hWeights_HIntegerHIntegerhWEIGHTS );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers


def("hRunningAverage",fptr_hRunningAverage_HIntegerHNumberHNumberHNumberSTDITSTDITSTDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers


def("hRunningAverage",fptr_hRunningAverage_HIntegerHNumberHNumberHIntegerhWEIGHTSSTDITSTDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hGeometricDelayFarField",fptr_hGeometricDelayFarField_HIntegerHNumberHNumberHNumberSTDITFIXEDSTDITFIXED );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hGeometricDelayNearField",fptr_hGeometricDelayNearField_HIntegerHNumberHNumberHNumberSTDITFIXEDSTDITFIXED );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hGeometricDelays",fptr_hGeometricDelays_HIntegerHNumberHNumberHNumberboolSTDITSTDITSTDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hGeometricPhases",fptr_hGeometricPhases_HIntegerHNumberHNumberHNumberHNumberboolSTDITSTDITSTDITSTDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hGeometricWeights",fptr_hGeometricWeights_HIntegerHNumberHNumberHNumberHComplexboolSTDITSTDITSTDITSTDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers


def("hSpectralPower",fptr_hSpectralPower_HIntegerHComplexHNumberSTDITSTDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers


def("hADC2Voltage",fptr_hADC2Voltage_HComplex1HNumberSTDIT );
def("hADC2Voltage",fptr_hADC2Voltage_HNumber1HNumberSTDIT );
def("hADC2Voltage",fptr_hADC2Voltage_HInteger1HNumberSTDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hGetHanningFilter",fptr_hGetHanningFilter_HComplex1HNumberuintuintuintSTDIT );
def("hGetHanningFilter",fptr_hGetHanningFilter_HNumber1HNumberuintuintuintSTDIT );
def("hGetHanningFilter",fptr_hGetHanningFilter_HInteger1HNumberuintuintuintSTDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hGetHanningFilter",fptr_hGetHanningFilter_HComplex1HNumberuintSTDIT );
def("hGetHanningFilter",fptr_hGetHanningFilter_HNumber1HNumberuintSTDIT );
def("hGetHanningFilter",fptr_hGetHanningFilter_HInteger1HNumberuintSTDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hGetHanningFilter",fptr_hGetHanningFilter_HComplex1HNumberSTDIT );
def("hGetHanningFilter",fptr_hGetHanningFilter_HNumber1HNumberSTDIT );
def("hGetHanningFilter",fptr_hGetHanningFilter_HInteger1HNumberSTDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hGetHanningFilter",fptr_hGetHanningFilter_HComplex1STDIT );
def("hGetHanningFilter",fptr_hGetHanningFilter_HNumber1STDIT );
def("hGetHanningFilter",fptr_hGetHanningFilter_HInteger1STDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hApplyFilter",fptr_hApplyFilter_HComplex11STDITSTDIT );
def("hApplyFilter",fptr_hApplyFilter_HNumber11STDITSTDIT );
def("hApplyFilter",fptr_hApplyFilter_HInteger11STDITSTDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hApplyHanningFilter",fptr_hApplyHanningFilter_HComplex1STDIT );
def("hApplyHanningFilter",fptr_hApplyHanningFilter_HNumber1STDIT );
def("hApplyHanningFilter",fptr_hApplyHanningFilter_HInteger1STDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hFFT",fptr_hFFT_HIntegerHNumberHComplexHIntegerSTDITSTDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hInvFFT",fptr_hInvFFT_HComplex11uintSTDITSTDIT );
def("hInvFFT",fptr_hInvFFT_HNumber11uintSTDITSTDIT );
def("hInvFFT",fptr_hInvFFT_HInteger11uintSTDITSTDIT );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//------------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers

def("hFileClose",fptr_hFileClose_HIntegerHIntPointer );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//------------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers

def("hFileOpen",fptr_hFileOpen_HIntegerHString );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers

def("hFileGetParameter",fptr_hFileGetParameter_HIntegerHIntPointerHString );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers

def("hFileSetParameter",fptr_hFileSetParameter_HIntegerHIntPointerHStringHPyObjectPtr );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers

def("hFileRead",fptr_hFileRead_HComplexHIntPointerHString1STL );
def("hFileRead",fptr_hFileRead_HNumberHIntPointerHString1STL );
def("hFileRead",fptr_hFileRead_HIntegerHIntPointerHString1STL );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers

def("hCalTable",fptr_hCalTable_HIntegerHStringHStringHIntegerHPyObjectPtr );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers



def("hCoordinateConvert",fptr_hCoordinateConvert_HIntegerHNumberCRCoordinateTypeHNumberCRCoordinateTypeboolSTDITFIXEDSTDITFIXED );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
//==================================================================================
// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk
//==================================================================================
//     File was generated from hftools.iter.cc on Sat Mar 06 01:47:42 CET 2010
//----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
//#define HFPP_PARDEF_8 (HInteger)(Offsets)()("Offsets per antenna")(HFPP_PAR_IS_VECTOR)(STL)(HFPP_PASS_AS_REFERENCE)
/* hfppnew-generatewrappers.def - START ..........................................*/
//
//GENERATING WRAPPERS
//
//First set all the values to default values, if not present
//------------------------------------------------------------------------------
//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
//Unless, explicitly set, yields a "return" statement if the function
//type is not void (actually, HFPP_VOID, which is = -1)
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers

def("hReadFileOld",fptr_hReadFileOld_HComplex1HIntPointerHStringHIntegerHIntegerHIntegerHIntegerHIntegerSTL );
def("hReadFileOld",fptr_hReadFileOld_HNumber1HIntPointerHStringHIntegerHIntegerHIntegerHIntegerHIntegerSTL );
def("hReadFileOld",fptr_hReadFileOld_HInteger1HIntPointerHStringHIntegerHIntegerHIntegerHIntegerHIntegerSTL );

////////////////////////////////////////////////////////////////////////////////
//
//========================================================================
//--Undefine parameters
//========================================================================
//--Undefine user-defined parameters from cc file
/* hfppnew-generatewrappers.def - END ............................................*/
}
