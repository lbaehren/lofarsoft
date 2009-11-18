#ifndef HFCAST_H
#define HFCAST_H


//========================================================================
//Casting Operations
//========================================================================
//Convert the various data types into each other

template<class T> T hfnull();
template<> inline HString  hfnull<HString>();
template<> inline HPointer hfnull<HPointer>();
template<> inline HInteger hfnull<HInteger>();
template<> inline HNumber  hfnull<HNumber>();
template<> inline HComplex hfnull<HComplex>();

//forward definition

class Vector_Selector;
template<class S> inline HString mytostring(S v);

//Identity
template<class T> inline T mycast(const T v);

//--Numbers ------------------------------------------------------------

template<class T> inline T mycast(const unsigned int v);

//Convert to arbitrary class T if no specified otherwise
template<class T> inline T mycast(const HPointer v);
template<class T> inline T mycast(const HInteger v);
template<class T> inline T mycast(const HNumber v);
template<class T> inline T mycast(const HComplex v);
template<class T> inline T mycast(const HString v);

//Convert Numbers to Numbers and loose information (round float, absolute of complex)
template<>  inline HInteger mycast<HInteger>(HNumber v);
template<>  inline HInteger mycast<HInteger>(HComplex v);

template<>  inline HNumber mycast<HNumber>(HComplex v);


//--Strings ------------------------------------------------------------

//Convert Numbers to Strings
template<> inline HString mycast<HString>(HPointer v);
template<> inline HString mycast<HString>(HInteger v);
template<> inline HString mycast<HString>(HNumber v);
template<> inline HString mycast<HString>(HComplex v);



//--Pointers ------------------------------------------------------------


//Convert Type T to HPointer:
template<> inline HPointer mycast(const HInteger v);
template<> inline HPointer mycast(const HNumber v);
template<> inline HPointer mycast(const HComplex v);

//Convenience definition for mycast
#define AsNum mycast<HNumber>
#define AsInt mycast<HInteger>
#define AsPtr mycast<HPointer>
#define AsIPtr mycast<HIntPointer>
#define AsStr mycast<HString>
#define AsCpx mycast<HComplex>

/* 
This function allows one to set the value of a variable stored in the
heap of known "type", using a void pointer "ptr" using an input type
T.  Conversion is only possible between the basic data types known to
mycast.
*/
template <class T>
void set_ptr_to_value(void * ptr, DATATYPE type, T value);

//This allows one to cast a void ptr to a certain value
template <class T>
T cast_ptr_to_value(void * ptr, DATATYPE type);

//--End Casting------------------------------------------------------------


template <class T,class S>
  void copycast_vec(void *ptr, vector<S> *sp);

template <class T, class S>
  void copycast_vec(void *ptr, vector<S> *sp, Vector_Selector *vs);

#define INSTANTIATE_FUNC_T \
  DEF_FUNC(HInteger,I);\
  DEF_FUNC(HNumber,N);\
  DEF_FUNC(HComplex,C);\
  DEF_FUNC(HString,S);

#define INSTANTIATE_FUNC_S_T \
  DEF_FUNC(HString,HInteger,S,I);\
  DEF_FUNC(HString,HNumber,S,N);\
  DEF_FUNC(HString,HComplex,S,C);\
  DEF_FUNC(HString,HString,S,S);\
  DEF_FUNC(HInteger,HInteger,I,I);\
  DEF_FUNC(HInteger,HNumber,I,N);\
  DEF_FUNC(HInteger,HComplex,I,C);\
  DEF_FUNC(HInteger,HString,I,S);\
  DEF_FUNC(HNumber,HInteger,N,I);\
  DEF_FUNC(HNumber,HNumber,N,N);\
  DEF_FUNC(HNumber,HComplex,N,C);\
  DEF_FUNC(HNumber,HString,N,S);\
  DEF_FUNC(HComplex,HInteger,C,I);\
  DEF_FUNC(HComplex,HNumber,C,N);\
  DEF_FUNC(HComplex,HComplex,C,C);\
  DEF_FUNC(HComplex,HString,C,S);


//Casting Utilities
//------------------------------------------------------------------------

template <class T>
vector<T> vec_combine(const vector<T> v1, const vector<T> v2);

template <class T>
void vec_append(vector<T> &v1,const vector<T> &v2);

template <class T>
HString vectostring(const vector<T> v,const address maxlen=8);

template <class T>
void printvec_noendl(const vector<T> v,const address maxlen=8);

template <class T>
void printvec(const vector<T> v,const address maxlen=8);

template <class T, class S>
void printvec_txt(const vector<T> v, const char* (*f)(S));

template <class T>
DATATYPE WhichType();

#endif
