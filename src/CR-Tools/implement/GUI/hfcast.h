#ifndef HFCAST_H
#define HFCAST_H


//========================================================================
//Casting Operations
//========================================================================
//Convert the various data types into each other

//forward definition

class Vector_Selector;
template<class S> inline String mytostring(S v);

//Identity
template<class T> inline T mycast(const T v);

//--Numbers ------------------------------------------------------------

//Convert to arbitrary class T if no specified otherwise
template<class T> inline T mycast(const Pointer v);
template<class T> inline T mycast(const Integer v);
template<class T> inline T mycast(const Number v);
template<class T> inline T mycast(const Complex v);
template<class T> inline T mycast(const String v);

//Convert Numbers to Numbers and loose information (round float, absolute of complex)
template<>  inline Integer mycast<Integer>(Number v);
template<>  inline Integer mycast<Integer>(Complex v);

template<>  inline Number mycast<Number>(Complex v);


//--Strings ------------------------------------------------------------

//Convert Numbers to Strings
template<> inline String mycast<String>(Pointer v);
template<> inline String mycast<String>(Integer v);
template<> inline String mycast<String>(Number v);
template<> inline String mycast<String>(Complex v);



//--Pointers ------------------------------------------------------------


//Convert Type T to Pointer:
template<> inline Pointer mycast(const Integer v);
template<> inline Pointer mycast(const Number v);
template<> inline Pointer mycast(const Complex v);

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
  DEF_FUNC(Integer,I);\
  DEF_FUNC(Number,N);\
  DEF_FUNC(Complex,C);\
  DEF_FUNC(String,S);

#define INSTANTIATE_FUNC_S_T \
  DEF_FUNC(String,Integer,S,I);\
  DEF_FUNC(String,Number,S,N);\
  DEF_FUNC(String,Complex,S,C);\
  DEF_FUNC(String,String,S,S);\
  DEF_FUNC(Integer,Integer,I,I);\
  DEF_FUNC(Integer,Number,I,N);\
  DEF_FUNC(Integer,Complex,I,C);\
  DEF_FUNC(Integer,String,I,S);\
  DEF_FUNC(Number,Integer,N,I);\
  DEF_FUNC(Number,Number,N,N);\
  DEF_FUNC(Number,Complex,N,C);\
  DEF_FUNC(Number,String,N,S);\
  DEF_FUNC(Complex,Integer,C,I);\
  DEF_FUNC(Complex,Number,C,N);\
  DEF_FUNC(Complex,Complex,C,C);\
  DEF_FUNC(Complex,String,C,S);


//Casting Utilities
//------------------------------------------------------------------------

template <class T>
vector<T> vec_combine(vector<T> v1,vector<T> v2);

template <class T>
void vec_append(vector<T> &v1,const vector<T> &v2);

template <class T>
String vectostring(vector<T> v,address maxlen=8);

template <class T>
void printvec_noendl(vector<T> v,address maxlen=8);

template <class T>
void printvec(vector<T> v,address maxlen=8);

template <class T, class S>
void printvec_txt(vector<T> v, char* (*f)(S));

template <class T>
DATATYPE WhichType();

#endif
