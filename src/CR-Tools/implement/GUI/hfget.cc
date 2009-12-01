/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2008                                                    *
 *   Heino Falcke (www.astro.ru.nl/~falcke)                                *
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

/*! 
  HFGET Version 0.1 - The basic data get/put mechanism for the Hyper-Flexible
  Plotting Tool for LOFAR Data
*/


using namespace std;

//#include "mainwindow.h" 
#include "hfdefs.h" 
#include "hfcast.h" 
#include "VectorSelector.h"
#include "hfget.h"
#include "hffuncs.awk.h"  
#include "hfanalysis.h"  
#include <time.h>

#define DOSILENT( DOSOMETHING ) bool SILENTx=Silent(true); DOSOMETHING; Silent(SILENTx)
#define DOSILENT2( DOSOMETHING ) SILENTx=Silent(true); DOSOMETHING; Silent(SILENTx)

//------------------------------------------------------------------------
// Globale Definitions
//------------------------------------------------------------------------

Data NullObject("NULL");

int global_debuglevel = 0;

void setDebug(int level) {global_debuglevel = level;}


//========================================================================
// object logic operators
//========================================================================

/*
  These are function allowing one to compare data objects - eventually that
  should be used to overload operators like +,-,=,<,> etc ...
*/

//Checks if a value equals the first data element of the object
//This is mainly useful for objects containing a single data point (e.g., an parameter)
//The value the object data is  compared with will be converted to the object data type.
// object == value

template <class T>
bool object_logic_cmpr(const Data* o_ptr, const T val){
  bool result;
  switch (o_ptr->getType()) {
#define SW_TYPE_COMM(X,TYPE) result = ( (mycast<TYPE>(val)) == (o_ptr->getOne<TYPE>()))
#include "switch-type.cc"
    result=false;
  };
  return result;
}

//Checks if the first data element of the object is contained in a set (vector) of values.
//This is mainly useful for objects containing a single data point (e.g., an parameter)
//The value the object data is compared with (set members) will be converted to the object data type.
// object == value1 || object == value2 || object == value3 ....

template <class T>
bool object_logic_in_set(const Data* o_ptr, const vector<T> &set){
  bool found=false;
  objectid i,s=set.size();
  DEF_VAL( T );
  
  switch (o_ptr->getType()) {
#define SW_TYPE_COMM(EXT,TYPE)			\
    val_##EXT = new TYPE;			\
    (*val_##EXT)=o_ptr->getOne<TYPE>();		\
    D2BG("VALUE=" << (*val_##EXT));		\
    for (i=0; i<s && !found; ++i) {		\
      D2BG("set[" <<i<<"]=" << set[i]);			    \
      found = (mycast<TYPE>(set[i]) == *val_##EXT);	    \
      D2BG("FOUND=" << tf_txt(found));			    \
    };							    \
    delete val_##EXT
#include "switch-type.cc"
    found=false;
  };
  D2BG("Return FOUND=" << tf_txt(found));				    
  return found;
}

// End object logic operators
//------------------------------------------------------------------------


//========================================================================
//Utilities
//========================================================================


HInteger getPointerFromPythonObject(PyObject* pyobj) {
  return reinterpret_cast<HInteger>(reinterpret_cast<HPointer>(pyobj));
}

/*
Here we allow all types to be casted into others (even if that measn
to loose information). mycast is the basic function which allows one to do that.

*/

template<class T> T hfnull(){};
template<> inline HString  hfnull<HString>(){HString null=""; return null;} 
template<> inline HPointer hfnull<HPointer>(){return NULL;} 
template<> inline HInteger hfnull<HInteger>(){return 0;} 
template<> inline HNumber  hfnull<HNumber>(){return 0.0;} 
template<> inline HComplex hfnull<HComplex>(){return 0.0;} 

//Convert the various data types into each other:
template<class S> inline HString mytostring(S v){std::ostringstream os; os << v; return os.str();}

//Identity
template<class T> inline T mycast(const T v){return v;}

//--Numbers ------------------------------------------------------------

//Some Special cases to avoid ambiguity, hence first convert to common basic type
template<class T> inline T mycast(const unsigned int v){return mycast<T>(static_cast<HInteger>(v));}


//Convert to arbitrary class T if not specified otherwise
template<class T> inline T mycast(const HPointer v){return mycast<T>(reinterpret_cast<HInteger>(v));}
template<class T> inline T mycast(const HInteger v){return static_cast<T>(v);}
template<class T> inline T mycast(const HNumber v){return static_cast<T>(v);}
template<class T> inline T mycast(const HComplex v){return static_cast<T>(v);}

//Convert Numbers to Numbers and loose information (round float, absolute of complex)
template<>  inline HInteger mycast<HInteger>(HNumber v){return static_cast<HInteger>(floor(v+0.5));}
template<>  inline HInteger mycast<HInteger>(HComplex v){return static_cast<HInteger>(floor(real(v)+0.5));}

template<>  inline HNumber mycast<HNumber>(HComplex v){return real(v);}

//--Strings ------------------------------------------------------------

//Convert Numbers to Strings
template<> inline HString mycast<HString>(HPointer v){std::ostringstream os; os << v; return os.str();}
template<> inline HString mycast<HString>(HInteger v){std::ostringstream os; os << v; return os.str();}
template<> inline HString mycast<HString>(HNumber v){std::ostringstream os; os << v; return os.str();}
template<> inline HString mycast<HString>(HComplex v){std::ostringstream os; os << v; return os.str();}
template<> inline HPointer mycast(const HString v){HPointer t=NULL; std::istringstream is(v); is >> t; return t;}
template<> inline HInteger mycast(const HString v){HInteger t=0; std::istringstream is(v); is >> t; return t;}
template<> inline HNumber mycast(const HString v){HNumber t=0.0; std::istringstream is(v); is >> t; return t;}
template<> inline HComplex mycast(const HString v){HComplex t=0.0; std::istringstream is(v); is >> t; return t;}
template<class T> inline T mycast(const HString v){T t; std::istringstream is(v); is >> t; return t;}


//--Pointers ------------------------------------------------------------


//Convert Type T to HPointer:
template<> inline HPointer mycast(const HPointer v){return v;}
template<> inline HPointer mycast(const HInteger v){return reinterpret_cast<HPointer>(v);}
template<> inline HPointer mycast(const HNumber v){return reinterpret_cast<HPointer>(mycast<HInteger>(v));}
template<> inline HPointer mycast(const HComplex v){return reinterpret_cast<HPointer>(mycast<HInteger>(v));}



/*!  

\brief Returns a vector of IDs belonging to the Data object pointers in the DataList dl

 */

vector<address> DataListtoIDs(const DataList dl){
  DataList::iterator it;
  vector<address> vec;
  vec.reserve(dl.size());
  it=dl.begin();
  while (it!=dl.end()){
    SaveCall2(*it){vec.push_back((*it)->getOid());}
    it++;
  };
  return vec;
}


/*! \brief Copy and convert a vector of type S to a vector of type T. 

ptr is a void pointer to a vector of type T which will receive the
copied data from vector<S> *sp.

*/

template <class T, class S>
void copycast_vec(void *ptr, vector<S> *op) {
  vector<T> *ip =  reinterpret_cast<vector<T>*>(ptr);
  typedef typename vector<T>::iterator Tit; 
  typedef typename vector<S>::iterator Sit; 
  Tit it1=ip->begin();
  Tit end=ip->end();
  if (it1==end) {op->clear();}
  else {
    op->assign(ip->size(),hfnull<S>()); //make the new vector equal in size and initialize with proper Null values
    Sit it2=op->begin();
    while (it1!=end) {
      *it2=mycast<S>(*it1);
      it1++; it2++;
    };
  };
}


template <class T, class S>
void copycast_vec(void *ptr, vector<S> *op, Vector_Selector *vs) {
  typedef typename vector<S>::iterator Sit; 

  if (noSelection(vs)) {copycast_vec<T,S>(ptr,op); return;};

  vector<T> *ip =  reinterpret_cast<vector<T>*>(ptr);

  vector<address> *idx = vs->getList(ip); //Obtain a list of the elements to be selected
  vector<address>::iterator it1 = idx->begin();
  vector<address>::iterator itend = idx->end();
    
  if (it1==itend) {op->clear();return;};
    
  address size=ip->size();
  op->resize(idx->size(),hfnull<S>()); 
  Sit it2=op->begin();
  while (it1!=itend) {
    if (*it1<size) {
      *it2=mycast<S>((*ip)[*it1]);
      it2++;
    };
    it1++;
  };
  if (it2!=op->end()){
    ERROR("copycast_vec: inconsistent selection index");
    it2--;
    op->erase(it2,op->end());
  };
}

/* Functions that provide textual output for some of the enum types we use here */

const char* datapointer_txt(Data* d){
  if (isDataObject(d)) {return (d->getName(true)).c_str();}
  else {return mycast<HString>(reinterpret_cast<HPointer>(d)).c_str();};
}

const char* direction_txt(DIRECTION dir){
  char* DIRECTION_TXT[DIR_NONE+1]={"DIR_FROM", "DIR_TO", "DIR_BOTH","DIR_NONE"};
  if (dir <= DIR_NONE && dir >=0) {return DIRECTION_TXT[dir];} else {return "UNKNOWN_DIRECTION";};
}

char* dataclass_txt(DATACLASS x){
  char* DATACLASS_TXT[DAT_NONE+1]={ "DAT_CONTAINER", "DAT_PARAMETER", "DAT_FILENAME","DAT_VECTOR", "DAT_ARRAY", "DAT_NONE"};
  if (x <= DAT_NONE && x >=0) {return DATACLASS_TXT[x];}  else {return "UNKNOWN_DATACLASS";};
}

char* reftype_txt(REFTYPE x){
  char* REFTYPE_TXT[REF_NONE+1]={"REF_MEMORY", "REF_FUNCTION", "REF_FILE", "REF_NODE", "REF_WEB","REF_NONE"};  
  if (x <= REF_NONE && x >=0) {return REFTYPE_TXT[x];}  else {return "UNKNOWN_REF";};
}

char* modaction_txt(MOD_ACTION x){
  char* MODACTION_TXT[MOD_UNDEF+1]={"MOD_CLEARED",  "MOD_UPDATED",  "MOD_LINKCHANGED", "MOD_DELETED", "MOD_UNDEF"};
  if (x <= MOD_UNDEF && x >=0) {return MODACTION_TXT[x];}  else {return "UNKNOWN_MODACTION";};
}

char* datatype_txt(DATATYPE x){
  char* DATATYPE_TXT[6]={"HPointer","HInteger", "HNumber", "HComplex", "HString", "UNDEF"};
//  DBG("datatype_txt: x=" << x);
  if (x <= UNDEF && x >=0) {return DATATYPE_TXT[x];} else {return "UNKNOWN_TYPE";};
}

char* tf_txt(bool x){
  if (x) {return "TRUE";} else {return "FALSE";};
}

char* pf_txt(bool x){
  if (x) {return "PASS";} else {return "FAIL";};
}

DIRECTION inv_dir(DIRECTION dir) {
  if (dir==DIR_FROM) {return DIR_TO;} 
  else if (dir==DIR_TO) {return DIR_FROM;} 
  else {return dir;};
}

/* 
This function allows one to retreive the value stored in the heap of known "type", using a void pointer "ptr" and cast it to the type T. Conversion is only possible between the basic data types known to mycast.
*/


template <class T>
T cast_ptr_to_value(void * ptr, DATATYPE type){
  switch (type) {
#define SW_TYPE_COMM(EXT,TYPE) return mycast<T>(*(static_cast<TYPE*>(ptr)))
#include "switch-type.cc"
    return mycast<T>(0);
  };
}

/* 
This function allows one to set the value of a variable stored in the
heap of known "type", using a void pointer "ptr" using an input type
T.  Conversion is only possible between the basic data types known to
mycast.
*/

template <class T>
void set_ptr_to_value(void * ptr, DATATYPE type, T value){
  switch (type) {
#define SW_TYPE_COMM(EXT,TYPE) *(static_cast<TYPE*>(ptr))=mycast<TYPE>(value);
#include "switch-type.cc"
  };
}

//End Casting Operations
//------------------------------------------------------------------------


//Casting Utilities
//------------------------------------------------------------------------
 
/* join two vectors: (1,2,3) + (4,5,6) = (1,2,3,4,5,6) */
template <class T>
vector<T> vec_combine(const vector<T> v1,const vector<T> v2){
  vector<T> v;
  v.reserve(v1.size() + v2.size());
  v.insert(v.end(), v1.begin(), v1.end());
  v.insert(v.end(), v2.begin(), v2.end());
  return v;
}

/*!\brief append the second vector to the end of the first */
template <class T>
void vec_append(vector<T> &v1,const vector<T> &v2){
  v1.reserve(v1.size() + v2.size());
  v1.insert(v1.end(), v2.begin(), v2.end());
  return;
}

/*!\brief Copying the first vector into the second, just retaining
   unique elements nut not sorting them (each element exists only
   once)

   ATTENTION!! That function should be slightly faster than
   vec_unique, but due to a bug doesn't work yet!!!*/

template <typename T>
void vec_unique_copy(const vector<T> &v1,vector<T> &v2){
  typename vector<T>::const_iterator it1a,it_end,it_beg,it1b;
  typename vector<T>::iterator it2;
  bool found;
  longint l=0;
  MSG("This function is buggy, please check");
  v2.reserve(v1.size());
  it_beg=v1.begin();
  it_end=v1.end();
  it1a=it_beg;
  it2=v2.begin();
  while (it1a!=it_end){
    //    D2BG("vec_unique_copy: it1a=" << reinterpret_cast<void*>(it1a));
    it1b=it1a;
    found=false;
    while (it1b!=it_beg){
      --it1b;
      //D2BG("vec_unique_copy: it1b=" << it1b << ",it1a=" << it1a);
      if (*it1a==*it1b) found=true;
    };
    if (!found) {
      //D2BG("Duplicate Found! it1b="<<it1b);
      *it2=*it1a;
      ++l;
    }
    ++it1a;
  };
  //D2BG("vec_unique_copy: l=" << l << ", v2.size=" << v2.size());
  v2.resize(l);
}

/*!\brief Copying the first vector into the second, just retaining
   unique elements and sorting them (each element exists only once) 

   This is more efficient then vec_uniqe_copy(v1,v2).
*/

template <class T>
void vec_unique_copy_sorted(const vector<T> &v1,vector<T> &v2){
  sort(v1.begin(),v1.end());
  v2.assign(v1.begin(),unique(v1.begin(),v1.end()));
}

/*!\brief Return true if element elem is already in the vector v.*/
template <class T>
bool in_vector(const T elem, const vector<T> &v){
  bool ret=false;
  int i;
  for (i=0; i<v.size() && !ret; ++i) {
    ret = ret || (v[i]==elem);
  };
  return ret;
}

/*!\brief Return a version of vector v, which contains each element only once.

Note this is better implemented using the stl  algorithms.!!!!
*/
template <class T>
vector<T> vec_unique(const vector<T> &v){
  vector<T> v_out;
  int i;
  for (i=0; i<v.size(); ++i) {
    if (!in_vector(v[i],v_out)) v_out.push_back(v[i]);
  };
  return v_out;
}

template <class T>
HString vectostring(const vector<T> v,const address maxlen=8){
  int i;
  int s;
  HString out="";
  s=v.size(); 
  if (s==0) {return out;};
  if (s<=maxlen+1) {
    for (i=0;i<s-1;++i) {out += mycast<HString>(v[i])+", ";};
    out += mycast<HString>(v[i]);
  } else {
    for (i=0;i<maxlen/2;++i) {out += mycast<HString>(v[i])+", ";};
    out += "...";
    for (i=s-maxlen/2;i<s;++i) {out += ", " + mycast<HString>(v[i]);};
  };
  return out;
}

template <class T>
void printvec_noendl(const vector<T> v,const address maxlen=8){
  int i;  int s;
  s=v.size(); 
  if (s==0) {return;};
  if (s<=maxlen+1) {
    for (i=0;i<s-1;++i) {cout << v[i] << ", ";};
    cout << v[i];
  } else {
    for (i=0;i<maxlen/2;++i) {cout << v[i] << ", ";};
    cout << "...";
    for (i=s-maxlen/2;i<s;++i) {cout << ", " << v[i];};
  };
}

template <class T>
void printvec(const vector<T> v,const address maxlen=8){
  printvec_noendl(v,maxlen);
  cout << endl;
}


//!!Check - this doesn't work properly for direction_txt
template <class T, class S>
void printvec_txt(const vector<T> v, const char* (*f)(S)){
  int i;
  int s;
  s=v.size();
  if (s==0) {cout <<endl; return;};
  for (i=0;i<s-1;++i) {cout << *f(v[i]) << ", ";};
  cout << f(v[i]) << endl;
  return;
}

template <class T>
DATATYPE WhichType() {
  DATATYPE t;
       if (typeid(T)==typeid(HPointer)) t = POINTER;
  else if (typeid(T)==typeid(HInteger)) t = INTEGER;
  else if (typeid(T)==typeid(HNumber )) t = NUMBER;
  else if (typeid(T)==typeid(HComplex)) t = COMPLEX;
  else if (typeid(T)==typeid(HString )) t = STRING;
  else t = UNDEF;
  return t;
}


//End Casting Functions
//------------------------------------------------------------------------

void * new_vector(DATATYPE type, address len){
  void*ptr; 
  switch (type) {
#define SW_TYPE_COMM(EXT,TYPE) ptr = new vector<TYPE>(len)
#include "switch-type.cc"
    return NULL;
  }
  return ptr;
}

bool del_vector(void *ptr, DATATYPE type) {
  DEF_D_PTR( HInteger );
  d_ptr_v=ptr;
  switch (type) {
#define SW_TYPE_COMM(EXT,TYPE) \
    DBG("Delete " << #TYPE << " ptr=" << d_ptr_v);\
    delete d_ptr_##EXT;\
    DBG("Deleted: " << #TYPE << " ptr=" << d_ptr_v << ", type=" << type)
#include "switch-type.cc"
    ERROR("del_vector: While attemptting to delete a vector.");
  }
}

bool del_value(void *ptr, DATATYPE type) {
  switch (type) {
#define SW_TYPE_COMM(EXT,TYPE) \
    DBG("Delete value " << #TYPE << " ptr=" << ptr << endl);\
    delete static_cast<TYPE*>(ptr);\
    DBG("Deleted: " << #TYPE << " ptr=" << ptr << ", type=" << type << endl)
#include "switch-type.cc"
    ERROR("del_value: Undefined type encountered while attemptting to delete a value.");
  }
}

/*! \brief Splits a string of type name1:name2:name3 (for c=":") into a vector of strings.*/
vector<HString> split_str_into_vector(HString str, char c)
{
  vector<HString> out;
  int pos,oldpos=0;
  for (pos=str.find(c);pos>-1;pos=str.find(c,pos+1)) {
    out.push_back(str.substr(oldpos,pos-oldpos));
    oldpos=pos+1;
  };
  out.push_back(str.substr(oldpos));
  return out;
}

//finds the first location of certain characters, n contains the position
int string_find_chars (const HString s, const HString c, int &n, const int npos)
{
  int i, slen=s.size();
  int nchrs=c.size();
  bool found=false;
  for (i=npos; i<slen && !found; ++i) {
    for (n=0;n<nchrs && !found;n++) {found = c[n]==s[i];};
  };
  n=n-1;
  if (i>=slen) {i=0;};
  return i-1;
}

/*
finds the last location of certain characters (c) in string s, n contains the character number that was found, 
npos gives a starting position relative to the end
*/

int string_rfind_chars (const HString s, const HString c, int &n, const int npos)
{
  int i, slen=s.size();
  int nchrs=c.size();
  bool found=false;
  for (i=slen-1-npos; i>=0 && !found; i--) {
    for (n=0;n<nchrs && !found;n++) {found = c[n]==s[i];};
  };
  n=n-1;
  if (found) {return i+2;} else {return i+1;};
}

/*
finds the last location of certain characters (c) in string s, npos gives a starting position,
and the returns the string from the end up to (and excluding) the (punctuation) character 
*/

HString string_rfind_chars_substr (const HString s, const HString c, const int npos)
{
  int n,i=string_rfind_chars (s, c, n, npos);
  return s.substr(i,s.size()-i);
}


/*
Takes a string and splits it into a vector of strings (names) at the punctuation characters
contained in string c =":'". If a ":" is encountered the corresponding element in the 
direction vector dirs is set to DIR_TO otherwise it is set to DIR_FROM.
 */
void parse_record_name_to_vector(HString name, vector<HString> &names, vector<DIRECTION> &dirs, HString c)
{
  DIRECTION dir = DIR_TO;
  int nc,pos,oldpos=0;
  names.clear(); dirs.clear();
  for (pos=string_find_chars(name,c,nc);pos>-1;pos=string_find_chars(name,c,nc,pos+1)) {
    if (pos>oldpos) {
      names.push_back(name.substr(oldpos,pos-oldpos));
      dirs.push_back(dir);
    };
    if (nc==0) {dir=DIR_TO;} else {dir=DIR_FROM;};
    oldpos=pos+1;
  };
  names.push_back(name.substr(oldpos));
  dirs.push_back(dir);
}
/*!  

Searches for a comparison operator (currently only '=' is implemented)
in the string describing the object searched for (used in square
bracktets). Returns false if not present, otherwise returns the
elements after the comparison operator. Multiple elements are possible
if separated by a ','.

*/
bool parse_record_selection_to_vector(HString str, HString &name, vector<HString> &elems)
{
  int pos;
  pos=str.find('=');
  if (pos==-1) {name=str; elems.clear(); return false;};
  name=str.substr(0,pos);
  //  D2BG(endl <<"parse_record_selection_to_vector" << "Input=" << str);
  //D2BG("name=" << name);
  elems=split_str_into_vector(str.substr(pos+1),',');
  //D2BG2("elems="); DBG3(printvec(elems));
  return true;
}

vector<objectid> DPtrToOid(DataList objects) 
{
  vector<objectid> object_oids;
  DataList::const_iterator it;
  for (it=objects.begin(); it<objects.end(); ++it) {
    object_oids.push_back((*it)->getOid());
  };
  return object_oids;
}

//========================================================================
//class Data 
//========================================================================

/*    
  Things still to be done (very incomplete ....):
  
  - add unit support - this could, however, be done by special objects 
  attached to each real data vector containing the unit name (e.g. "m"), 
  scale letter (e.g. "k" for km) and a scale factor (here 1000). In fact per
  unit there needs to be only on single unit object that all data objects point 
  to if needed.
  
  - do more operator overloading, such that data["Station=1:Data"]=vector, 
  replaces data["Station=1:Data"].put(vector).
  
  - use operators like -> or so for setting Links (also in Python).
  
  - There is also a class Vector_Selector which is intended to allow you retrieving
  sub-parts of the data vector (e.g. only every 2nd value, or just values 
  above a certain threshold ...). That needs a bit more work still ...
  
*/

/*! 

\brief Checks whether the modification record comes from a valid object which has not been deleted.

*/

bool ModRecisValid(modification_record mod){
  return isDataObject(mod.ref) && mod.action<MOD_DELETED;
};

bool operator== (const modification_record mod1, const modification_record mod2) {return mod1.ref==mod2.ref && mod1.version==mod2.version && mod1.action==mod2.action;}

/*! \brief Tries to check whether an object is a valid data object. It checks whther the pointer is not NULL and not the NullObject and whether the magiccode is correct.

 */

bool isDataObject(const Data* obj, const bool notquiet) {
  if (obj!=NULL && obj!=&NullObject) {
    if (obj->magiccode==MAGICCODE) return true;
    else if (obj->magiccode==MAGICCODE_DELETED) {
      if (notquiet) {ERROR("Object ptr=" << obj << " was already deleted!");};
      return false;
    } else {
      if (notquiet) {ERROR("Object ptr=" << obj << " contains garbage!");};
      return false;
    };
  } else return false;
}

Vector_Selector * Data::sel(){return data.s_ptr;}
void Data::new_sel(){if (data.s_ptr==NULL) {data.s_ptr=new Vector_Selector;};}
void Data::del_sel(){delete data.s_ptr;data.s_ptr=NULL;}


HString Data::reference_descr_str(reference_descr* rd){
  std::ostringstream sout;
   if (true) { 
    sout << "name=" << rd->name;
    sout << ", oid=" << rd->oid;
    sout << ", ref=" << rd->ref;
    sout << ", port=" << rd->port; 
    sout << ", direction=" << direction_txt(rd->direction);
    sout << ", mod=" << strModFlag(rd->mod);
    sout << ", type=" << datatype_txt(rd->type);
  } else sout << "DELETED";
  return sout.str();
}

void Data::printAllStatus(bool short_output,longint maxlevel, bool listdeletedobjects){
  HInteger i;
  if (data.superior!=NULL) {
    for (i=0; i<data.superior->data_objects.size(); ++i) {
      if (data.superior->data_objects[i]!=&NullObject) {
	SaveCall(data.superior->data_objects[i]) {
	  if (data.superior->data_objects[i]->getNetLevel()<=maxlevel)
	    data.superior->data_objects[i]->printStatus(short_output);
	};
      } else {
	if (listdeletedobjects)  cout << i << " NULL" << endl;
      };
    };
  } else {
    ERROR("printAllStatus: No superior created yet, can't find any objects!");
  }
}

vector<objectid> Data::getAllIDs(){
  HInteger i;
  vector<objectid> vec;
  if (data.superior!=NULL) {
    for (i=0; i<data.superior->data_objects.size(); ++i) {
      if (data.superior->data_objects[i]!=&NullObject) {
	SaveCall(data.superior->data_objects[i]) vec.push_back(data.superior->data_objects[i]->getOid());
      };
    };
  } else {
    ERROR("getAllIDs: No superior created yet, can't find any objects!");
  }
  return vec;
}

/*!  \brief Print the content of the object, including the status of
all internal variables. If short_output = true, only a one-line
summary is printed.

 */
void Data::printStatus(bool short_output){cout << Status(short_output) <<endl;}

/*! \brief Return the number of modification flags which are set*/

address Data::getMod(){
  address i,result=0;
  for (i=0; i<data.from.size(); ++i) {
    if (ModRecisValid(data.from[i]->mod)) {result++;};
  };
  return result;
}

vector<HString> Data::getModFlagsStr(){
  address i;
  vector<HString> vec;
  for (i=0; i<data.from.size(); ++i) {
    if (ModRecisValid(data.from[i]->mod)) {vec.push_back(strModFlag(data.from[i]->mod));};
  };
  return vec;
}

/*! \brief Return a vector of modification flags of the object in
    direction dir. Flags are set by setModification.*/

vector<modification_record> Data::getModFlags(DIRECTION dir){
    objectid i;
    vector<modification_record> vec;
    if (dir==DIR_FROM || dir==DIR_BOTH) {for (i=0; i<data.from.size(); ++i) {vec.push_back(data.from[i]->mod);};} 
    else if (dir==DIR_TO || dir==DIR_BOTH) {for (i=0; i<data.to.size(); ++i) {vec.push_back(data.to[i]->mod);};} 
    return vec;
}

/*! \brief Return a string representation of a modification flag.*/

HString Data::strModFlag(modification_record mod){
    std::ostringstream sout;
    if (mod.ref==NULL) {sout << "NULL.0";} 
    else if (mod.action == MOD_DELETED) {sout << "DELETED." << mod.version;}
    else {sout << mod.ref->getName(true) << "." << mod.version<< "." << modaction_txt(mod.action);};
    return sout.str();
}

/*! 
\brief Returns true if the modification flag is set, otherwise False.
*/

bool Data::ModFlagSet(modification_record mod){
  return (mod.ref!=NULL);
}

/*! \brief Return a vector of strings containing the modification flags in direction dir. See also listNeighbourNames, listNeighbourIDs.*/

vector<HString> Data::listNeighbourModFlags(DIRECTION dir){
  address i;
  vector<HString> vec;
  vector<modification_record> mods=getModFlags(dir);
  for (i=0; i<mods.size(); ++i) {vec.push_back(strModFlag(mods[i]));};
  return vec;
}

/*!  Return a boolean flag indicating whether the modification flags
in an object are set. If false, this need not imply that the object is
up to date, since this call does not trigger going through the chain
for PULL connections. It juts looks at the from ports in the current
object.
*/

bool Data::isModified(){
    return data.modified;
}


HString Data::Status(bool short_output){
    std::ostringstream sout;
    objectid i;


  DEF_D_PTR(HInteger); 
  if (short_output) {
      sout << "#" << getName(true) << ": ";
    if (data.of_ptr != NULL) {sout << data.of_ptr->getName() <<"(";};
    sout << datatype_txt(data.type);
    if (data.of_ptr != NULL) {sout << ")";};
    if (data.len>0) {sout << "[" << data.len << "]";};
    if (isModified()) {sout << "*";};
    if (data.d_ptr!=NULL) {
      sout << "=";
      d_ptr_v=data.d_ptr;
      CALL_FUNC_D_PTR (sout << vectostring);
    };
    if (data.to.size()>0) {
      sout << " =>";
      for (i=0; i<data.to.size(); ++i) {
	SaveCall(data.to[i]->ref){
	  if (data.to[i]->direction==PULL || data.to[i]->direction==DIR_BOTH) {sout << " <<";};
	  if (data.to[i]->direction==PUSH || data.to[i]->direction==DIR_BOTH) {sout << " >>";};
	  sout << data.to[i]->ref->getName();
	  sout << "{" << data.to[i]->ref->getOid() << "}";
	if (i<data.to.size()-1) {sout << "|";};
	};
      };
    };
    if (data.from.size()>0) {
      sout << " <= ";
      for (i=0; i<data.from.size(); ++i) {
	SaveCall(data.from[i]->ref) {
	  sout << "{" << data.from[i]->ref->getOid() << "}";
	  sout << data.from[i]->ref->getName();
	  if (data.from[i]->direction==PULL || data.from[i]->direction==DIR_BOTH) {sout << "<<";};
	  if (data.from[i]->direction==PUSH || data.from[i]->direction==DIR_BOTH) {sout << ">>";};
	if (i<data.from.size()-1) {sout << "|";};
      };
      };
    };
  } else {
  sout << endl;
  sout << "------------------------------------------------------------------------"<<endl;
  sout << "Name: " << data.name <<endl;
  sout << "oid: " << data.oid <<endl;
  sout << "this:" << this << endl;
  sout << "------------------------------------------------------------------------"<<endl;
  sout << "origin: " << reftype_txt(data.origin) <<endl;
  sout << "buflen: " << data.buflen <<endl;
  sout << "len: " << data.len <<endl;
  address nmod=getMod();
  sout << "#mod: " << nmod <<endl;
  sout << "version: " << getVersion() << " - is modifed: " << tf_txt(isModified()) <<endl;
  sout << "To  : "<<endl; 
  for (  i=0; i<data.to.size(); ++i) {sout <<"["<<i<<"] "; sout << this->reference_descr_str(data.to[i])<<endl;};
  sout << "From: "<<endl;  
  for (  i=0; i<data.from.size(); ++i) {sout <<"["<<i<<"] "; sout << this->reference_descr_str(data.from[i])<<endl;};
  sout << "datatype = " << datatype_txt(data.type) << endl;
#define PRINT_DATACONTENT(VAR)   sout << #VAR << " = " << data.VAR << endl;
  sout << "of_ptr = " << data.of_ptr << endl;
  sout << "d_ptr = " << data.d_ptr << endl;
  sout << "pydbg = " << data.pydbg << endl;
  sout << "pyqt = " << data.pyqt << endl;
  sout << "py_func = " << data.py_func << endl;
  sout << "s_ptr = " << data.s_ptr << endl;
  PRINT_DATACONTENT(noMod);
  PRINT_DATACONTENT(noSignal);
  PRINT_DATACONTENT(modified);
  PRINT_DATACONTENT(beingmodified);
  PRINT_DATACONTENT(autoupdate);
  PRINT_DATACONTENT(updateable);
  PRINT_DATACONTENT(silent);
  PRINT_DATACONTENT(tmpsilent);
  PRINT_DATACONTENT(linkpathchanged);
  PRINT_DATACONTENT(verbose);
  PRINT_DATACONTENT(debuggui);
  PRINT_DATACONTENT(debug);
  PRINT_DATACONTENT(defdir);
  PRINT_DATACONTENT(netlevel);
  sout << "Worm = " << Worm <<endl;
  if (Worm!=NULL) {
    sout << "........................................................................";
    sout << strWorm();
  }
  
  if (data.d_ptr!=NULL) {
    sout << "========================================================================"<<endl;
    sout << "data = ";
    d_ptr_v=data.d_ptr;
    CALL_FUNC_D_PTR (sout << vectostring);
    sout << endl;
  };
  sout << "========================================================================";
  };
  return sout.str();
}

void Data::printDecendants(HString rootname){
 objectid i;
  HString name;
  if (rootname=="") {name=data.name;} else {name = rootname + ":" + data.name;};
  cout <<  name << "[" << data.len << "]";
  if (data.d_ptr != NULL || data.of_ptr != NULL) {
    cout << " = ";
    DEF_D_PTR(HNumber); 
    d_ptr_v=new_vector(data.type,data.len);
    CALL_FUNC_D_PTR(get);
    CALL_FUNC_D_PTR(printvec);
    del_vector(d_ptr_v,data.type);
  } else {cout << endl;};
  for (i=0; i<data.to.size(); ++i) {if (data.to[i]->ref!=NULL) {data.to[i]->ref->printDecendants(name);};};
}



/*!  \brief Provides a list of all names of objects linked to the
current one in the direction DIR (DIR.FROM, DIR.TO, or DIR.BOTH). See
also listNeighbourIDs, listNeighbourDirs, listNeighbourModFlags
*/

vector<HString> Data::listNeighbourNames(DIRECTION dir){
    vector<HString> l;
    objectid i;
    if (dir==DIR_FROM || dir==DIR_BOTH) {
      for (i=0; i<data.from.size(); ++i) {
	SaveCall(data.from[i]->ref) l.push_back(data.from[i]->ref->getName());
      };
    };
    if (dir==DIR_TO || dir==DIR_BOTH) {
      for (i=0; i<data.to.size(); ++i) {
	SaveCall(data.to[i]->ref) l.push_back(data.to[i]->ref->getName());
      };
    };
    return l;
}

/*!  

\brief Returns the direction of the link between the current object and its neighbour.

*/

DIRECTION Data::getLinkDirection(const Data & neighbour){
  objectid i;
  for (i=0; i<data.from.size(); ++i) {
    if (data.from[i]->ref==&neighbour) return DIR_FROM;
  };
  for (i=0; i<data.to.size(); ++i) {
    if (data.to[i]->ref==&neighbour) return DIR_TO;
  };
  return DIR_NONE;
}
/*!  

\brief Returns the direction of the link between the current object and its neighbour.

*/

DIRECTION Data::getLinkDirectionType(const Data & neighbour){
  objectid i;
  for (i=0; i<data.from.size(); ++i) {
    if (data.from[i]->ref==&neighbour) return data.from[i]->direction;
  };
  for (i=0; i<data.to.size(); ++i) {
    if (data.to[i]->ref==&neighbour) return data.to[i]->direction;
  };
  return DIR_NONE;
}

/*!
\brief Adds a reference_descr (i.e. a struct referring to another object) to a list (map), but only if the objected pointed to is not yet part of the list to preserve uniqueness, whereby the ref_descr with a set mod flag has precedence. See listNeighbours.
 */

void Data::addReferenceDescriptor(reference_descr rd, map<objectid,reference_descr> &l){
  map<objectid,reference_descr>::iterator p;
  p=l.find(rd.oid);
  if (p!=l.end()) {
    if (ModFlagSet(rd.mod)) l[rd.oid]=rd;
  } else {
    /*There might be a way to speed this up, since we know that this key needs to be added and need not be found again first*/
    l[rd.oid]=rd;
  };
}

/*!
\brief Combines two lists (maps) of reference_descr (i.e. a struct referring to another object). Preserves uniqueness concerning objects, whereby the ref_descr with a set mod flag has precedence. The result is returned in the first map given as argument. See listNeighbours.
 */

void Data::join_map_ReferenceDescriptors(map<objectid,reference_descr> &l1,map<objectid,reference_descr> l2){
  map<objectid,reference_descr>::iterator p;
  p=l2.begin();
  while (p!=l2.end()) {
    addReferenceDescriptor(p->second,l1);
    p++;
  };
}
/*!
\brief Provides a list (actually a map) of references to objects linked to the current one in the direction DIR. Will ignore objects with a netlevel larger than maxlevel. See also listNeighbourNames, listNeighbourDirs, listNeighbourModFlags.
 */

map<objectid,Data::reference_descr> Data::listNeighbours(DIRECTION dir,longint maxlevel){
  map<objectid,reference_descr> l;
  reference_descr rd;
    objectid i,oi,mi;
    mi=getOid();
    if (dir==DIR_FROM || dir==DIR_BOTH) {
      for (i=0; i<data.from.size(); ++i) {
	SaveCall(data.from[i]->ref) {
	  rd=*data.from[i];
	  oi=rd.oid;
	  if (oi!=mi) { // avoid infinite loops
	    if (data.from[i]->ref->getNetLevel()<=maxlevel)
	      {addReferenceDescriptor(rd,l);}
	    else 
	      {join_map_ReferenceDescriptors(l,data.from[i]->ref->listNeighbours(dir,maxlevel));};
	  };
	}
      };
    };
    if (dir==DIR_TO || dir==DIR_BOTH) {
      for (i=0; i<data.to.size(); ++i) {
	SaveCall(data.to[i]->ref) {
	  rd=*data.to[i];
	  oi=rd.oid;
	  if (oi!=mi) { // avoid infinite loops
	    if (data.to[i]->ref->getNetLevel()<=maxlevel)
	      {addReferenceDescriptor(rd,l);}
	    else 
	      {join_map_ReferenceDescriptors(l,data.to[i]->ref->listNeighbours(dir,maxlevel));};
	  };
	}
      };
    };
    return l;
}


/*!
\brief Returns (as arguments) vectors containing information about objects that are considered neighbours to the current object.

This takes maxlevel into account, meaning that objects with a netlevel higher than maxlevel are transparently ignored. The vectors returned contain the names, IDs, and Direction, as well as the modification flags. If multiple links lead to the same object only one is returned - if one link contains a non-zero modflag, that one has preference. See listNeighbours.
 */

 void Data::getNeighboursList(DIRECTION dir,longint maxlevel,  vector<objectid> &oids, vector<HString> &names, vector<DIRECTION> &dirs,  vector<HString> & flags){
  // DBG3("getNeighboursList: dir=" << dir <<", maxlevel=" << "," << maxlevel);
  map<objectid,reference_descr> l=listNeighbours(dir,maxlevel);
  longint size=l.size();
  //  DBG3("getNeighboursList: listNeighbours size=" << size);
  map<objectid,reference_descr>::iterator p=l.begin();
  names.clear(); names.reserve(size);
  oids.clear(); oids.reserve(size);
  dirs.clear(); dirs.reserve(size);
  flags.clear(); flags.reserve(size);
  while (p!=l.end()) {
    names.push_back(p->second.name);
    dirs.push_back(p->second.direction);
    oids.push_back(p->second.oid);
    flags.push_back(strModFlag(p->second.mod));
    p++;
  };
}


/*!  

\brief Provides a vector of Data object IDs with all objects
between the current object (in direction "dir") and the objects with
names in the "names" vector.

 */
vector<address> Data::FindChainID(const DIRECTION dir,const vector<HString> names, const bool include_endpoints, const bool monotonic){
  return DataListtoIDs(FindChain(dir,names,include_endpoints,monotonic));
}


/*!  

\brief Provides a vector of Data object pointers with all objects
between the current object (in direction "dir") and the objects with
names in the "names" vector.

 */


DataList Data::FindChain(const DIRECTION dir,const vector<HString> names, const bool include_endpoints, const bool monotonic){
  DataList newneighbours, neighbours, chain, endpoints;
  DataList::iterator it;
  DIRECTION dir2 = DIR_BOTH;
  if (monotonic) dir2=dir;
  HInteger oid;
  chain.push_back(this);
  neighbours=getNeighbours(dir);
  it=neighbours.begin();

  while (it!=neighbours.end()) {
    DBG2("Neighbour="<<(*it)->getName());
    if (!in_vector(*it,chain)) {
      SaveCall(*it){
	if (!(in_vector((*it)->getName(),names) || in_vector((*it)->getSearchName(),names))) {
	  newneighbours.push_back(*it);    
	  chain.push_back(*it);    
	} else {
	  endpoints.push_back(*it);    
	};
      };
    };
    it++;
  };

  while (newneighbours.size()>0) {
    //First collect all neighbours of the newly added objects to the chain (stored in newneighbours)
    neighbours.clear();
    it=newneighbours.begin();
    while (it!=newneighbours.end()) {
      D2BG("NewNeighbour="<<(*it)->getName());
      vec_append(neighbours,(*it)->getNeighbours(dir2));
      it++;
    };
    newneighbours.clear();
    //Now cycle over all these neighbours and determine which ones are new and not yet and endpoint object
    it=neighbours.begin();
    while (it!=neighbours.end()) {
      oid=(*it)->getOid();
      D2BG("Neighbour2="<< (*it)->getName(true));
      if (!in_vector(*it,chain)) {
	SaveCall(*it){
	  if (!(in_vector((*it)->getName(),names) || in_vector((*it)->getSearchName(),names))) {
	    //If it is really new and not an endpoint add to chain 
	    //and store it in the newneigbours list which will be used 
	    //as starting point in the next iteration
	    newneighbours.push_back(*it);    
	    chain.push_back(*it);    
	  } else {
	    endpoints.push_back(*it);    
	  };
	};
      };
      it++;
    };
  };
  if (include_endpoints) vec_append(chain,vec_unique(endpoints));
  return chain;
}


/*!
\brief Provides a list/vector of all object IDs of objects linked to the current one in the direction DIR. See also listNeighbourNames, listNeighbourDirs, listNeighbourModFlags.

 */
vector<objectid> Data::listNeighbourIDs(DIRECTION dir,longint maxlevel){
    vector<objectid> l;
    objectid i,oi,mi;
    mi=getOid();
    if (dir==DIR_FROM || dir==DIR_BOTH) {
      for (i=0; i<data.from.size(); ++i) {
	SaveCall(data.from[i]->ref) {
	  oi=data.from[i]->ref->getOid();
	  if (oi!=mi) { // avoid infinite loops
	    if (data.from[i]->ref->getNetLevel()<=maxlevel)
	      {l.push_back(oi);}
	    else 
	      {vec_append(l,data.from[i]->ref->listNeighbourIDs(dir,maxlevel));};
	  };
	}
      };
    };
    if (dir==DIR_TO || dir==DIR_BOTH) {
      for (i=0; i<data.to.size(); ++i) {
	SaveCall(data.to[i]->ref) {
	  oi=data.to[i]->ref->getOid();
	  if (oi!=mi) { // avoid infinite loops
	    if (data.to[i]->ref->getNetLevel()<=maxlevel)
	      {l.push_back(oi);}
	    else 
	      {vec_append(l,data.to[i]->ref->listNeighbourIDs(dir,maxlevel));};
	  };
	}
      };
    };
    return vec_unique(l);
}

/*!
\brief Provides a list of the PUSH/PULL direction of the all object links linked to the current one in the TO direction. See also listFromDirs, listToNames, listToIDs, listNeighbourModFlags.
 */
vector<DIRECTION> Data::listNeighbourDirs(DIRECTION dir){
    vector<DIRECTION> l;
    objectid i;
    if (dir==DIR_FROM || dir==DIR_BOTH) {
      for (i=0; i<data.from.size(); ++i) {
	l.push_back(data.from[i]->direction);
      };
    };
    if (dir==DIR_TO || dir==DIR_BOTH) {
      for (i=0; i<data.to.size(); ++i) {
	l.push_back(data.to[i]->direction);
      };
    };
    return l;
}

/*! \brief Increments the version number of the object.

The version is increased each time the object is modified and is used for creating unique modification flags and can be used to measure activity of an object.
 */
longint Data::incVersion(){
  if (data.version>=MAXVERSION) {
    MSG("incVersion(): Version Wrap - No Error - name=" << getName(true) << " data.version=" <<data.version << " MAXVERSION=" << MAXVERSION);
    data.version=0;
  } else { 
    data.version++; 
  };
  return data.version;
}

/*! \brief Returns the version number of the object.

The version is increased each time the object is modified and is used for creating unique modification flags and can be used to measure activity of an object.
 */
longint Data::getVersion(){return data.version;}

/*! \brief Set the version number of the object.

The version is increased each time the object is modified and is used for creating unique modification flags and can be used to measure activity of an object.
 */
void Data::setVersion(longint ver){data.version=ver;}

/*! \brief Get the Net level of the object.

The netlevel represents a criterion for displaying objects in a
network representation. Lower levels are more important. If the level
number is too high, the object will not be dsiplayed. Major objects
have level>=1, user parameters have level>=10, default paramteres have
level >=100, and system objects have level>=1000.
 */
longint Data::getNetLevel(){return data.netlevel;}

/*! \brief set the Net level of the object used for displaying the network.

The netlevel represents a criterion for displaying objects in a
network representation. Lower levels are more important. If the level
number is too high, the object will not be dsiplayed. Major objects
have level>=1, user parameters have level>=10, default paramteres have
level >=100, and system objects have level>=1000.
 */
Data& Data::setNetLevel(longint lev){
  data.netlevel=lev; 
  DBG("setNetLevel(" << lev << "): name=" << getName(true));
  return *this;
}

/*!

\brief Returns the name of the object. If longname=True also the ID is included in the name. 

*/
HString Data::getName(const bool longname){
  if (isDataObject(this)) {
    if (!longname) return data.name;
    ostringstream s;
    s << "{" << getOid() << "}" << data.name;
    return s.str();
  };
}

/*!

\brief Returns the name of the object together with the value of the first element in the data object vector, separated by "=".

If longname=True also the ID is included in the name (which cannot be
used in a search path anymore). This is the name used in a search path
(i.e., what one typcially specifies in square brackets).
*/

HString Data::getSearchName(const bool longname){
  HString name=getName(longname);
  if (len()>=1) name=name+"="+getOne<HString>();
  return name;
}

/*!
\brief Returns the length of the data vector in the object. 
*/
address Data::len(){return data.len;}

/*!
\brief Return the (default) type of the data vector stored in the object or being returned by a function. 

If the type is undefined and the object has no data and no function
then return the Type of the first attached FROM object as the type.
After all the data from this object will also be passed through when
doing a get.

 */
 DATATYPE Data::getType(){
   if (data.type==UNDEF) {   
     if (Empty()) {          
       if (data.from.size()>0) {
	 SaveCall(data.from[0]->ref){
	   return data.from[0]->ref->getType(); //Transparently pass type through from first connected object
	 };
       };
     };
   };
   return data.type;
 }

/*!
Set the data type of an object. This is only possible if the object does not yet contain a data vector.
 */
 void Data::setType(const DATATYPE typ){
   if (!hasData()) {data.type=typ;};
}
 objectid Data::getOid(){return data.oid;}
 ObjectFunctionClass* Data::getFunction(){return data.of_ptr;};

//  sendMessage(MSG_MODIFIED,DIR_TO);



/*!  \brief Make an object silent. 

That allows you to change the value of an object,
without triggering an instant update of the net. The modification flag will be
set however. To modify the object without setting the modifcation flag, use
noMod.
*/

bool Data::Silent(bool silent){
  if (this==&NullObject) {ERROR("Silent: Operation on NullObject not allowed."); return false;};
  bool old;
  old=data.silent;
  data.silent=silent;
  return old;
}

/*!  \brief Make an object temporarily silent. 

That allows you to change the value of an object,
without triggering an instant update of the net. The modification flag will be
set however. To modify the object without setting the modifcation flag, use
noMod.
*/

Data* Data::doSilent(){
  data.tmpsilent=true;
  return this;
}

Data& Data::doSilent_Ref(){return *doSilent();};


/*!
\brief checks whether an object needs updating
 */
bool Data::needsUpdate(){
  if (this==&NullObject) {ERROR("needsUpdate: Operation on NullObject not allowed."); return false;};
  return hasData() && hasFunc() && isModified();
}

/*!
\brief Returns the flag which determines whether an object is automatically updated when a modification message has been received.
 */
bool Data::doesAutoUpdate(){
  if (this==&NullObject) {ERROR("doesAutoUpdate: Operation on NullObject not allowed."); return false;};
  return data.autoupdate;
}

/*!
\brief Returns true if the object in principle is updateable (i.e. stores or displays data in some form).
 */

bool Data::Updateable(){
  if (this==&NullObject) {ERROR("Updateable: Operation on NullObject not allowed."); return false;};
  return data.updateable;
}

/*!
\brief Sets a flag which determines whether an object is automatically updated when a modification message has been received. up=true means that automatic updating is enabled.
 */
void Data::setAutoUpdate(bool up){
    if (this==&NullObject) {ERROR("setAutoUpdate: Operation on NullObject not allowed."); return;};
  data.autoupdate=up;
}

/*!
\brief Sets a flag which determines whether an object is in principle updateable. 
 */
void Data::setUpdateable(bool up){
  if (this==&NullObject) {ERROR("Updateable: Operation on NullObject not allowed."); return;};
  DBG("setUpdateable(" << tf_txt(up) << "): name=" << getName(true));
  data.updateable=up;
}


/*! If this returns true the object will produce a short message everytime the get method is called. Usefulf for tracing network operations.
 */

//template <class T>
//bool Data::doVerbose(const vector<T> &v,const bool checkmod)	{
bool Data::doVerbose()	{
  char in_c[256];
  HString in;
  cout << getName(true);
  //  if (v.size()>0) {cout << v[0];};
  //if (checkmod) {cout <<")* -> ";}
  //else {cout <<") => ";};
  cout <<" => ";
  if (data.pydbg!=NULL) {
    if (getDebugGUI()) {signalPyDBG("gui","");};
    if (getDebug()) {
      cout << "PyDBG>> "; cin.getline(in_c,256);
      in=string(in_c); 
      while (!(in=="" || in=="" || in=="end" || in=="quit" || in=="bye")) {
	signalPyDBG("execute",in);
	cout << "PyDBG>> "; cin.getline(in_c,256); in=string(in_c); 
      };
    };
  };
  return true;
};

/*! If this returns true the object will produce a short message everytime the get method is called. Usefulf for tracing network operations.
 */
bool Data::Verbose(){return data.verbose>data.netlevel;}

/*! Set the verbose level, the higher the more will be displayed. Goes
    in logarithmic steps (9,99,999,9999). Uses the NetLevel of each
    object as comparison. If Verbose() returns true the object will
    produce a short message everytime the get method is
    called. Usefulf for tracing network operations.
 */
longint Data::setVerbose(longint verbose){
  if (this==&NullObject) {ERROR("setVerbose: Operation on NullObject not allowed."); return false;};
  longint old;
  old=data.verbose;
  data.verbose=verbose;
  return old;
}

/*! Set the debug mode on (in connection with verbose). 
  
  This will call the python prompt whenever a doVerbose statement is
  encountered in the c++ code. This happens typically in the Data::get
  method, but only if the verbose level is set higher than the current
  netlevel of the object. Before doing this a python hfPyDBGObject()
  has to be created and assigned to all objects where this is
  desired. This hfPyDBGObject() is then called with the method
  execute(input_string). The result is printed. 

  The method will return the input state of the flag.
 */
bool Data::setDebug(bool tf){
  if (this==&NullObject) {ERROR("setDebug: Operation on NullObject not allowed."); return false;};
  data.debug=tf;
  return data.debug;
}

/*! Return whether the debug state of the object is on or off (in connection with verbose - see setDebug). 

 */

bool Data::getDebug(){
  if (this==&NullObject) {ERROR("setDebug: Operation on NullObject not allowed."); return false;};
  return data.debug;
}

/*! Set the GUI verbose mode on (in connection with verbose). 
  
  This will update the netview window whenever a doVerbose statement is
  encountered in the c++ code. This happens typically in the Data::get
  method, but only if the verbose level is set higher than the current
  netlevel of the object. Before doing this a python hfPyDBGObject()
  has to be created and assigned to all objects where this is
  desired. This hfPyDBGObject() is then called with the method
  gui(). 

  The method will return the input state of the flag.
 */

bool Data::setDebugGUI(bool tf){
  if (this==&NullObject) {ERROR("setDebug: Operation on NullObject not allowed."); return false;};
  data.debuggui=tf;
  return data.debuggui;
}

/*! Return whether the GUI verbose state of the object is on or off (in connection with verbose - see setDebugGUI). 

 */

bool Data::getDebugGUI(){
  if (this==&NullObject) {ERROR("setDebug: Operation on NullObject not allowed."); return false;};
  return data.debuggui;
}

/*!  
Sets the default direction (PUSH or PULL) when creating new parameter objects
automatically in a function. This needs to be set before the function is
executed the first time. In some cases before initialization.
*/

void Data::setDefaultDirection(DIRECTION dir){
    data.defdir=dir;
}

/*!  
Gets the default direction (PUSH or PULL) when creating new parameter objects
automatically in a function. This needs to be set before the function is
executed the first time. In some cases before initialization.
*/

DIRECTION Data::getDefaultDirection(){
    return data.defdir;
}


/*!
\brief Sets in all objects the verbose level. 

    The higher the verbose level, the more will be displayed. Goes
    in logarithmic steps (9,99,999,9999). Uses the NetLevel of each
    object as comparison. If Verbose() returns true the object will
    produce a short message everytime the get method is
    called. Usefulf for tracing network operations.
 */
longint Data::AllVerbose(longint verbose){
  if (this==&NullObject) {ERROR("AllVerbose: Operation on NullObject not allowed."); return false;};
  HInteger i;
  if (data.superior!=NULL) {
    for (i=0; i<data.superior->data_objects.size(); ++i) {
	data.superior->data_objects[i]->setVerbose(verbose);
    };
  } else {
      ERROR("AllVerbose: No superior created yet, can't find any objects!");
  };
}

/*!  
\brief Mark the current object as modified and propagate the
message through the network if necessary. 

This does not necessarily lead to an update of the object itself (I
still have to figure out why ... update seems to work better. I guess
this happens if the object higher up the chain, which should call this
object is set to silent and does not respond. ).

The object can be temporarily set to silent to suppress any update by
setting silent=true as argument.

Use .update() to execute this object. See also wakeUp() to not set
the modification (again) but run the update sequence.
 */

Data& Data::touch(const bool silent){
  if (silent) {
    DOSILENT(setModification()); 
  } else {
  setModification(); 
  };
  return *this;
}
Data& Data::touch_0(){return touch();}

/*!
\brief Starts the update worm of the current object. This is necessary if the object was set silent and one now wants to run the update worm. Also sets Silent(False).
 */
Data& Data::wakeUp(){Silent(false); if (Worm!=NULL) executeUpdateWorm();return *this;}

/*!

\brief Set all modification flags and propagate the message through the net
upwards (TO), if necessary.

Probably outdated description following: ....

If called without parameters then the object was directly modifed. As
a consequence set the modification for all TO ports, and propagate the
information further if a PUSH connections is made.  If called with a
port and version information the modification was signalled from a
another object which was modifed. In this case set all TO ports and
also the corresponding from port and propagate the information higher
up. Each object signals its current version. If a receiving object has
a version number smaller than the one it has received an update is
necessary.

Called without parameters: Tell the object that it was modified by a
user input. If necessary (PUSH connection) signal the modification to
the objects higher up the chain and cause update if necessary.

*/

void Data::setModification(const MOD_ACTION action){
    modification_record newmod;

    if (this==&NullObject) {ERROR("Operation on NullObject not allowed."); return;};

    if (action>=MOD_LINKCHANGED) setLinkpathChanged(true);

    if (data.noMod) {
      data.noMod=false;
      return;
    }    

    /*
    if (data.beingmodified && !data.silent) MSG("setModification: name=" << getName(true) << " data.version=" <<getVersion() << ". Warning the object is being modified again, while another modification chain is already in progress. Maybe there is an inconsistent state of the network.");
    */

    data.beingmodified=true;

    //Creating the modification message that will be passed on

    newmod.version=incVersion();
    newmod.ref=this;
    newmod.action=action;

    if (Worm == NULL) {Worm = new dataworm;};

    setModification(newmod);

    //Now the worm is filled with object references. Go sequentially
    //along the worm and update the objects.

    executeUpdateWorm();
    data.beingmodified=false;
    data.tmpsilent=false;
}

/*! \brief This is just a convenience subroutine to setModification() 

*/
void Data::setModification(modification_record newmod){

    objectid size,i;

    //Increase the version number to create a unique version number
    //which will be passed through the network. In case the maximum
    //version number was reached the counter is reset. 
        
    DBG("setModification: name=" << getName(true) << " data.version=" <<getVersion());
 
    //Now signal the modification to all objects in the forward direction.
    //For a PULL conncetion just store it locally, for a PUSH connection set
    //the modification flags of the TO objects (and make them update if necesssary.)

    data.modified=true;

    if (newmod.action>=MOD_LINKCHANGED) setLinkpathChanged(true);

    size=data.to.size();
    for (i=0; i<size; ++i) {
      //mark the port to the next higher object as modified - this
      //will be queried from higher up when needed.
      data.to[i]->mod=newmod;
      //In push mode notify any objects higher up in the food chain
      //about the modification. They will in turn notify objects higher
      //up. The notification message is stored in an "updateworm" 
      //The references of the objects to be updated will be added
      //to the update-worm of the object that started this (who's
      //reference is contained in newmod, which is passed on higher).
      if ((data.to[i]->direction == PUSH || data.to[i]->direction == DIR_BOTH)) {
	SaveCall(data.to[i]->ref) data.to[i]->ref->setModification(data.to[i]->port,newmod);
      };
    };   

    return;
}

/*!
\brief Print content of the update Worm, which contains a list of all objects that need updating. Here return output as string.
*/
HString Data::strWorm(){
  std::ostringstream sout;
  dataworm::const_iterator it;
  sout << endl << "UpdateWorm(" << getName(true) << "): ";
  for (it=Worm->begin(); it<Worm->end(); ++it) {
    sout << (*it)->getName(true) << " -> ";
  };
  sout << "END(Worm)." << endl;
  return sout.str();
}

/*!
\brief Print content of the update Worm, which contains a list of all objects that need updating. Here return output as string.
*/
void Data::printWorm(){cout << strWorm();}

/*!
\brief Execute the update worm which was created by setModification or checkModification

The worm is filled with object references. The function goes sequentially along the
worm, i.e. through the list of objects, and update the objects.

 */

void Data::executeUpdateWorm(){
  if (data.silent || data.tmpsilent) {return;};
  if (Verbose()) printWorm();
  if (Worm==NULL) {
    ERROR("executeUpdateWorm() name=" << getName(true) << ": Uninitialized UpdateWorm called ...");
    return;
  }
  DBG("executeUpdateWorm(): executed by object " << getName(true) << ", worm.size=" << Worm->size());
  dataworm::const_iterator it;
  for (it=Worm->begin(); it<Worm->end(); ++it) {
    DBG("executeUpdateWorm(): executed by " << getName(true) << " -> update object worm.name=" << (*it)->getName(true));
    if (isDataObject(*it,false)) (*it)->doAutoUpdate();
  };
  DBG("executeUpdateWorm(): clear Worm.");
  Worm -> clear();
}

/*!
Tell the object that an object it depends on was modified (e.g., by a user input). If necessary (PUSH connection) signal the modification to the objects higher up the chain. Add itself to the update worm of the object that caused the update chain.
 */

void Data::setModification(objectid port, modification_record newmod){
  if (this==&NullObject) {ERROR("Operation on NullObject not allowed."); return;};

  objectid size,i;
  bool circular=false;

  DBG("setModification(port=" <<port<<", mod=" << strModFlag(newmod) <<"): name=" << getName(true));

  /*  if (data.beingmodified) MSG("setModification(port=" <<port<<", mod=" << strModFlag(newmod) <<"): name=" << getName(true) << " data.version=" <<getVersion() << ". Warning the object is being modified again, while another modification chain is already in progress. Maybe there is an inconsistent state of the network.");
   */

  if (newmod.action>=MOD_LINKCHANGED) setLinkpathChanged(true);

  //store the modification in the respective FROM port, check for a circular network 
  if (port>=0 && port <data.from.size()) {
    if (data.from[port]->mod==newmod) {
      DBG("setModification(port=" <<port<<", mod=" << strModFlag(newmod) <<") name=" << getName(true) <<": Same modification flag received twice. Do you have a circular network?");
      circular=true;
    };
    data.from[port]->mod=newmod;
  } else {
    ERROR("setModification(port=" <<port<<", mod=" << strModFlag(newmod) <<") name=" << getName(true) << ": Port number out of range - network is inconsistent.");
  };

  bool sameflag=false;
  
  size=data.from.size();
  for (i=0; i<size && !sameflag; ++i) {
    //check whether the flag has aready been received at another
    //port. If yes, don't go further.
    if (i!=port) sameflag=data.from[i]->mod==newmod;
    if (sameflag) {
      DBG("setModification(port=" <<port<<", mod=" << strModFlag(newmod) <<") name=" << getName(true) << ": Same flag already present at port #" <<i);
      return;
    };
  };

  data.modified=true;

  if (!circular) {
    size=data.to.size();
    for (i=0; i<size; ++i) {
      //mark the port to the next higher object as modified - this
      //will be queried from higher up when needed.
      data.to[i]->mod=newmod;
      //In push mode notify any objects higher up in the food chain
      //about the modification. They will in turn notify objects higher
      //up. The notification messagis a worm 
      //The references of the to be updated objects will be added
      //to the update-worm of the object that started this (who's
      //reference is contained in newmod, which is passed on higher).
      if ((data.to[i]->direction == PUSH || data.to[i]->direction == DIR_BOTH)) {
	SaveCall(data.to[i]->ref) data.to[i]->ref->setModification(data.to[i]->port,newmod);
      };
    };   
    //Add yourself to the update worm ...
    if (ModRecisValid(newmod)) newmod.ref->Worm->push_back(this);
  };
}

/*!  Check whether an object is modified an needs to be updated. Also,
  check the modification in the respective FROM ports, check for a
  circular network, and loop through all from ports that have a PULL
  connection, check whether these objects are modified (and lower) and
  if so set the modification flag of the current object and add itself
  to the update worm.
  */
bool Data::checkModification(){
  if (this==&NullObject) {ERROR("Operation on NullObject not allowed."); return false;};

  objectid size,i;
  bool modified=data.modified;
  modification_record newmod;

  DBG("checkModification(): name=" << getName(true) << " data.modified=" << tf_txt(data.modified));
  if (modified) return true;

  //The following few lines are only used for PULL connections, so that
  //could me made smarter if one introduces a flag that tells whether
  //PULL connections exist.
  
  //Creating the modification message that will be passed on
  newmod.version=incVersion();
  newmod.ref=this;
  newmod.action=MOD_UPDATED;

  if (Worm == NULL) {Worm = new dataworm;};

  //check the modification in the respective FROM ports This will set
  //all modification flags in the FROM direction and create an update
  //worm, which needs to be executed separately.
  size=data.from.size();
  for (i=0; i<size; ++i) {
    if ((data.from[i]->direction == PULL || data.from[i]->direction == DIR_BOTH)) {
      SaveCall(data.from[i]->ref) modified = modified || data.from[i]->ref->checkModification(data.from[i]->port,newmod);
    };
  };   
  return modified;
}

/*!  Check whether an object is modified an needs to be updated. Also,
  check the modification in the respective FROM ports, check for a
  circular network, and loop through all from ports that have a PULL
  connection, check whether these objects are modified (and lower) and
  if so set the modification flag of the current object and add itself
  to the update worm.

  If objects in the FROM direction with a PULL connection are
  modified, then the current object is also modified
  (data.modified=true). If the TO flag is set, but the object itself
  is not modified (this can happen if the object has both: PUSH and
  PULL connections in the TO direction), then the routine returns
  modified=true (to signal the object higher up that it needs
  updating), but the object itself remains up-to-date
  (data.modified=false).
  */
bool Data::checkModification(objectid port, modification_record newmod){
  if (this==&NullObject) {ERROR("Operation on NullObject not allowed."); return -1;};

  objectid size,i;
  bool modified=data.modified;
  bool circular=false;

  DBG("checkModification(port=" <<port<<", newmod="<<strModFlag(newmod) <<"): name=" << getName(true) << " data.modified=" << tf_txt(data.modified));

  //If the TO flag for the port currently queried (from an object
  //higher up) is set then return modified=true. Also check if the new
  //mod message is unique. Otherwise return and do not go deeper,
  //since there must have been a circular network somehow.
  if (port>=0 && port <data.to.size()) {
    if (data.to[port]->mod.ref!=NULL ) {
      modified=true;
      if (data.to[port]->mod==newmod) {
	DBG("checkModification(toport=" <<port<<", mod=" << strModFlag(newmod) <<") name=" << getName(true) <<": Same modification flag received twice. Do you have a circular network?");
	circular=true;
      };
    };
  } else {
      ERROR("checkModification(port=" <<port<<", mod=" << strModFlag(newmod) <<") name=" << getName(true) << ": Port number out of range - network is inconsistent.");
      return true;
  };

  //Now go deeper through the network in the FROM direction (even if
  //this is already modified)
  size=data.from.size();
  if (!circular) {
    for (i=0; i<size; ++i) {
      if ((data.from[i]->direction == PULL || data.from[i]->direction == DIR_BOTH)) {
	SaveCall(data.from[i]->ref) {
	  if (data.from[i]->mod==newmod) {
	    MSG("checkModification(port=" <<port<<", mod=" << strModFlag(newmod) <<") name=" << getName(true) <<": Same modification flag received twice. Do you have a circular network?");
	    return false;
	  } else {
	    if (data.from[i]->ref->checkModification(data.from[i]->port,newmod)) {
	      data.from[i]->mod=newmod;
	      data.modified=true;
	      modified = true;
	      data.to[port]->mod=newmod;
	    };
	  };
	};  
      };
    };

    if (data.modified) {if (ModRecisValid(newmod)) newmod.ref->Worm->push_back(this);};
  };   
  
  //If the object is modified (i.e., if one object in FROM direction
  //was modified), then set the TO flag accordingly and add itself to
  //the update worm

  //Add itself to the update worm, if the object itself is modified.
  return modified;
}


/*!  Clears the modification flag and sets all FROM mod flags to
     NULL. This indicates that the object is up-to-date. For any
     connection in the FROM diretion it will also clear the TO flag in
     the respective parent object.
 */
void Data::clearModification(){
    objectid size,i;

    DBG("clearModification(): name=" << getName(true));

    data.modified = false;
    data.tmpsilent=false;

    size=data.from.size();
    for (i=0; i<size; ++i) {
      data.from[i]->mod.ref=NULL;
      data.from[i]->mod.action=MOD_CLEARED;
      SaveCall(data.from[i]->ref) data.from[i]->ref->clearModificationTO(data.from[i]->port);
    };
    if (Worm != NULL) Worm->clear();
    return;
}

/*!  Clears the modification flag in the TO direction of PORT. This is
     called exclusively by clearModification().
 */
void Data::clearModificationTO(objectid port){
    objectid size,i;

    DBG("clearModification(port="<<port<<"): name=" << getName(true));

    if (port>=0 && port<data.to.size()) {
      data.to[port]->mod.ref=NULL;
      data.to[port]->mod.action=MOD_CLEARED;
    } else {
      ERROR("clearModification(port=" <<port<<") name=" << getName(true) << ": Port number out of range - network is inconsistent.");
    };
    return;
}



/*!
Not yet really used, but probably useful at some point 
*/
void Data::sendMessage(MSG_CODE msg, DIRECTION dir, objectid count, void* ptr) {
  objectid i,s;
  if (dir == DIR_FROM || dir == DIR_BOTH) {
    s=data.from.size();
    for (i=0; i<s-1; ++i) {
      if (data.from[i]->direction == DIR_FROM || data.from[i]->direction == DIR_BOTH) {
	data.from[i]->ref->getMessage(msg,dir,data.from[i]->port,count,ptr);
      };
    };
  };
  
  if (dir == DIR_TO || dir == DIR_BOTH) {
    s=data.to.size();
    for (i=0; i<s-1; ++i) {
      if (data.to[i]->direction == DIR_TO || data.to[i]->direction == DIR_BOTH) {
	data.to[i]->ref->getMessage(msg,dir,data.to[i]->port,count,ptr);
      };
    };
  };
}

//if count is positive it indicates the number of levels a message is propagated
//with count=0 it only reaches the immediate neighbours.

void Data::getMessage(MSG_CODE msg, DIRECTION dir, objectid port, objectid count, void* ptr) {
  objectid i,s;
  switch (msg) {
  case MSG_MODIFIED:
    return;
    break;
  default:
    ERROR("Message " << msg << "is unknown.");
    break;
  };
  if (count > 0) {sendMessage(msg, dir, count-1, ptr);};
}

/*!
This function assigns the corresponding port number of a referencing
object, i.e. port "port" in the current object will point to port
report in the related object. dir specifies whether that is a "to" or a
"from" reference.
*/
void Data::setPort(objectid port, objectid refport, DIRECTION dir){
  if (this==&NullObject) {ERROR("Operation on NullObject not allowed."); return;};
  vector<reference_descr*> * tf;
  if (dir == DIR_FROM ) {tf=&data.from;} 
  else if (dir == DIR_TO) {tf=&data.to;} 
  else {
    ERROR("setPort: Improper direction specified- neiter to nor from.");
    return;
  };
  if ((*tf).size()>port) {(*tf)[port]->port=refport;}
  else {ERROR("setPort: Changing non-existing port.");};
}

/*!
\brief This function searches for a link to another object and deletes it.
*/

void Data::delLink(Data &d) {
  int i,l;
  l=data.to.size();
  DBG("delLink(" << &d << "=" << d.getName() << "): ptr=" << this << "=" << d.getName());
  for (i=0; i<l; ++i) {
    if (data.to[i]->ref == &d) {delLink(i,DIR_TO,true);
      DBG("  delLink TO:" << data.to[i]->ref << "->" << &d << " (deleted)");
    };
  };
  l=data.from.size();
  for (i=0; i<l; ++i) {
    if (data.from[i]->ref == &d) {
      delLink(i,DIR_FROM,true);
      DBG("  delLink FROM:" << data.from[i]->ref << "<-" << &d << " (deleted)");
    };
  };
}

/*!
This function deletes a link to another object, specified by the port number
*/

void Data::delLink(objectid oid, objectid port, DIRECTION dir, bool del_other) {
  Ptr(oid)->delLink(port,dir,del_other);
}
void Data::delLink_ID(objectid oid, objectid port, DIRECTION dir) {
  Ptr(oid)->delLink(port,dir);
}

void Data::delLink(objectid port, DIRECTION dir, bool del_other,
		bool delete_empty) {
  objectid corresponding_port;
  vector<reference_descr*> * tf;
  if (dir == DIR_FROM ) {tf=&data.from;} 
  else if (dir == DIR_TO) {tf=&data.to;} 
  else {
    ERROR("delLink(port=" << port <<", dir=" << direction_txt(dir) << ", del_other=" << tf_txt(del_other) <<") name=" << getName(false) <<": Improper direction specified: neither to nor from.");
    return;
  };
  DBG("delLink(port=" << port <<", dir=" << direction_txt(dir) << ", del_other=" << tf_txt(del_other) <<") name=" << getName(false) <<": link=" << this << " port=" << port << " to/from.size()=" << (*tf).size() << " delete_empty=" << tf_txt(delete_empty));
  DBG3(this->printStatus(false));
  if (port < (*tf).size()) {
    //If this was the first call to the function delete the corresponding entrance in the
    //other object
    if (del_other) {
      (*tf)[port]->ref->delLink((*tf)[port]->port,inv_dir(dir),false,delete_empty);
    };
//Delete the reference descriptor
    delete (*tf)[port];

    if (port<(*tf).size()-1) {
      (*tf)[port] = (*tf).back(); //(*tf)[(*tf).size()-1];
      if ((*tf)[port]->ref!=NULL) {
	(*tf)[port]->ref->setPort((*tf)[port]->port,port,inv_dir(dir));
      } else {ERROR("delLink(port=" << port <<", dir=" << direction_txt(dir) << ", del_other=" << tf_txt(del_other) <<") name=" << getName(false) <<": Deleting a reference to a non-existing object");};
    };
    (*tf).pop_back();
  } else {
    ERROR("delLink(port=" << port <<", dir=" << direction_txt(dir) << ", del_other=" << tf_txt(del_other) <<") name=" << getName(false) <<": Attempting to delete a non-existing Link. Port number too high.");
  };

  //If this was the last reference in the vector, just delete it
  //otherwise copy the last object to the current reference port and then delete the last
  if (!del_other && getNumberOfLinks()==0 && getOid()!=0 && delete_empty) {
    DBG("delLink(port=" << port <<", dir=" << direction_txt(dir) << ", del_other=" << tf_txt(del_other) <<") name=" << getName(false) <<": deleting myself.");
    delete this;
  }
  doSilent()->setModification(MOD_LINKCHANGED);
}

/*!

\brief Returns the number of links in direction dir. If dir=DIR_BOTH (default) return the total number of links.

Can, e.g., be used to kill an object if zero is returned.
 */
longint Data::getNumberOfLinks(DIRECTION dir){
  longint n=0;
  if (dir==DIR_FROM || dir==DIR_BOTH) n+=data.from.size();
  if (dir==DIR_TO   || dir==DIR_BOTH) n+=data.to.size();
  return n;
}

/*!make -k 
\brief Like setLink it will set a link to a another object, however, if the link already exists it will be overwritten.

 */
Data* Data::resetLink(Data* d, const DIRECTION dir_type, const DIRECTION dir) {
  if (this==&NullObject) {ERROR("Operation on NullObject not allowed."); return &NullObject;};
  DIRECTION dirtype=dir_type;
  if (dirtype==DIR_NONE) dirtype=data.defdir;
  if (in_vector(d,getNeighbours(DIR_BOTH))) {
    //Check if Link has the same properties, if not reset Link
    if (!(getLinkDirectionType(*d)==dirtype && getLinkDirection(*d)==dir)) {
      delLink(*d);
      setLink(d,dirtype,dir);
    };
  } else {
    setLink(d,dirtype,dir);
  };
  return d;
}

Data& Data::resetLink_Ref_3(Data& d, const DIRECTION dir_type, const DIRECTION dir) {
  Data* result=resetLink(&d,dir_type,dir);
  SaveCall(result) return *result;
}
Data& Data::resetLink_Ref_2(Data& d, const DIRECTION dir_type) {
  Data* result=resetLink(&d,dir_type);
  SaveCall(result) return *result;
}
Data& Data::resetLink_Ref_1(Data& d) {
  Data* result=resetLink(&d);
  SaveCall(result) return *result;
}


/*!

  \brief Set a link to another object

dir_type specifies whether the link is a push or a pull link. For a push link
(DIR_TO) the update flag will be propagated through the network whenever an
objected is updated. A pull link (DIR_FROM) means that the updated flag will
be set only locally when an update is made. Other objects will only check when
their get method is called whether an update of a dependend object (i.e. all
objects in the DIR_FROM direction) has happened.

Example you have a parameter and a graph on a screen. In the PUSH link mode
the screen will be updated whenever the parameter is changed. In the PULL link
mode the parameter can be updated frequently without anything happening. Only
when the user request a replot of the screen the parameter is checked for an
update.

All this only affects objects that have both: a data vector - acting as a local
buffer - and a function actually generating the data.

if thisport is >=0 then the link at port THISPORT will be
overwritten. This is used by insert() and can cause harm if used
improperly!

See newObject.
*/
objectid Data::setLink(Data *d,
			const DIRECTION dir_type,
			const DIRECTION dir,
		       const objectid otherport,
		       const objectid thisport)
{
  DBG("setLink( I am =" << getName(true) << ", link to name=" << d->getName(true) << ", dir_type=" << direction_txt(dir_type)  << ", dir=" << direction_txt(dir) << ", otherport=" << otherport << ", thisport=" << thisport <<": started.");
  if (this==&NullObject) {ERROR("Operation on NullObject not allowed."); return -1;};
  if (in_vector(d,getNeighbours(dir))) {
    ERROR("setLink( I am =" << getName(true) << ", link to name=" << d->getName(true) << ", dir_type=" << direction_txt(dir_type)  << ", dir=" << direction_txt(dir) << ", otherport=" << otherport << ", thisport=" << thisport << " -- Attempt to create a duplicate link!");
    return -1;
    };
  Data* p = this;
  DIRECTION dirtype=dir_type;
  objectid oport=otherport;
  reference_descr* rd=new reference_descr;
  modification_record newmod;
  if (dirtype==DIR_NONE) dirtype=data.defdir;
  newmod.action=MOD_LINKCHANGED;
  newmod.ref=this;
  newmod.version=0;
  objectid retport=-2;
  bool domod=oport<0;

  if (d == this) {
    ERROR("setLink: Object can't point to itself.");
    return -1;
  }
  if (d == &NullObject) {
    ERROR("setLink: Attempting to link Object to NullObject. It is likely that a wrong object name was specified.");
    return -1;
  }
  if (dir == DIR_FROM ) {
    if (oport<0) {
      if (thisport>=0  && thisport < data.from.size()) {
	oport=(d->setLink(this, dirtype,DIR_TO,thisport));
      } else {
	oport=(d->setLink(this, dirtype,DIR_TO,data.from.size()));
      };
    };
    rd->direction=dirtype; rd->ref=d; rd->port=oport; rd->type=d->getType();
    rd->name=d->getName(); rd->oid=d->getOid(); 
    rd->mod=newmod;
    if (thisport>=0 && thisport < data.from.size()) {
      D2BG("setLink: using thisport len(mapfrom)="<<data.mapfrom.size()<<" removing name=" <<data.from[thisport]->name);
      data.mapfrom.erase(data.mapfrom.find(data.from[thisport]->name));
      D2BG("setLink: using thisport len(map::from)="<<data.mapfrom.size()<<" adding name="<< rd->name);
      data.mapfrom[rd->name]=thisport;
      data.from[thisport]=rd;
      retport=thisport;
    }
    else {
      data.mapfrom[rd->name]=data.from.size();
      data.from.push_back(rd);
      retport= data.from.size()-1;
    };
    //D2BG("setLink: data.from[len]->mod=" << data.from[data.from.size()-1]->mod);
    
  } else if (dir == DIR_TO ) {
    if (oport<0) {
      if (thisport>=0  && thisport < data.to.size()) {
	oport=(d->setLink(this, dirtype,DIR_FROM,thisport));
      } else {
	oport=(d->setLink(this, dirtype,DIR_FROM,data.to.size()));
      };
    };
    rd->direction=dirtype; rd->ref=d; rd->port=oport; rd->mod=newmod; rd->type=d->getType();
    rd->name=d->getName(); rd->oid=d->getOid(); 
    rd->mod=newmod;
    if (thisport>=0 & thisport < data.to.size()) {
      D2BG("setLink: using thisport len(mapto)="<<data.mapto.size()<<" removing name=" <<data.to[thisport]->name);
      data.mapto.erase(data.mapto.find(data.to[thisport]->name));
      D2BG("setLink: using thisport len(mapto)="<<data.mapto.size()<<" adding name=" << rd->name);
      data.mapto[rd->name]=thisport;
      data.to[thisport]=rd;
      retport= thisport;
    }
    else {
      data.mapto[rd->name]=data.to.size();
      data.to.push_back(rd);
      retport=data.to.size()-1;
    };
    //D2BG("setLink: data.to[len]->mod=" << data.to[data.to.size()-1]->mod);
  } else {
    ERROR("setLink( I am =" << getName(true) << ", link to name=" << d->getName(true) << ", dir_type=" << direction_txt(dir_type)  << ", dir=" << direction_txt(dir) << ", otherport=" << otherport << ", thisport=" << thisport << " -- dir should be either TO or FROM!");
  }
  if (domod && dir_type==PUSH) {
    if (dir==DIR_TO) doSilent()->setModification(newmod.action);
    if (dir==DIR_FROM) d->doSilent()->setModification(newmod.action);
  }
  return retport;
};

//The next was used for python binding, creating _Name, _ID etc.
Data& Data::setLink_Ref_3(Data &d, const DIRECTION dir_type, const DIRECTION dir){
  setLink(&d, dir_type,dir,-1,-1);  return d;}
Data& Data::setLink_Ref_2(Data &d, const DIRECTION dir_type){
  setLink(&d, dir_type,DIR_FROM,-1,-1);  return d;}
Data& Data::setLink_Ref_1(Data &d){
  setLink(&d, DIR_NONE,DIR_FROM,-1,-1);  return d;}


/*!
\brief Returns the port number of the link to object d in direction dir.
*/
int Data::getPort(Data & d, DIRECTION dir) {
  map<HString,int>* maptofrom;
  map<HString,int>::iterator it;

  if (dir==DIR_FROM) {
    maptofrom = &data.mapfrom;
  } else {
    maptofrom = &data.mapto;
  };
    
  it=maptofrom->find(d.getName());
  if (it != maptofrom->end()){return it->second;}
  else return -1;
}

/*!\brief Returns a vector of strings containing the names of all immediate neighbours in direction DIR.

 */
vector<HString> Data::getNeighbourNames(DIRECTION dir) {
  vector<reference_descr*> *fromto;
  vector<HString> vec;
  if (dir==DIR_TO) {
    fromto = & data.to;
  } else {
    fromto = &data.from;
  };
  vec.reserve(fromto->size());
  for (longint i=0;i<(fromto->size());++i) vec.push_back((*fromto)[i]->name);
  return vec;
}

/*!\brief Returns a vector of pointers to the all immediate neighbours in direction DIR.

 */
DataList Data::getNeighbours(const DIRECTION dir) {
  DataList vec;
  vector<reference_descr*>::iterator it;
  vec.reserve(data.from.size()+data.to.size());
  if (dir==DIR_TO || dir==DIR_BOTH) {
    it=data.to.begin(); while(it!=data.to.end()) {vec.push_back((*it)->ref);it++;};
  };
  if (dir==DIR_FROM || dir==DIR_BOTH) {
    it=data.from.begin(); while(it!=data.from.end()) {vec.push_back((*it)->ref);it++;};
  };
  return vec;
}

/*!
\brief Returns the reference descriptor (i.e. the record describing a link to another object) for a given port number.
*/
Data::reference_descr Data::getLink(objectid port, DIRECTION dir) {
  vector<reference_descr*> *fromto;
  reference_descr rd;
  rd.ref=NULL;

  if (dir==DIR_TO) {
    fromto = & data.to;
  } else {
    fromto = &data.from;
  };
    
  if ((port>=0) && (port<((*fromto).size()))) {rd=*(fromto->at(port));};
  return rd;    
}

/*!
\brief Returns a pointer to the object hooked into port PORT in the FROM direction.
*/
Data* Data::from(objectid port) {
  if (port >=0 && port < data.from.size()) {return data.from[port]->ref;}
  else {ERROR("data.from(): unknown reference port=" << port); return NULL;};
}

/*!
\brief Returns a pointer to the object hooked into port PORT in the TO direction.
*/
Data* Data::to(objectid port) {
  if (port >=0 && port < data.to.size()) {return data.to[port]->ref;}
  else {ERROR("data.to(): unknown reference port=" << port); return NULL;};
}

/*!
\brief Returns a pointer to the object with name NAME in the FROM direction.
*/
Data* Data::from(HString name) {

  data.it=data.mapfrom.find(name);
  if (data.it != data.mapfrom.end()){return data.from[data.it->second]->ref;}
  else {ERROR("data.from(): unknown reference \"" << name << "\""); return NULL;};
}

/*!
\brief Returns a pointer to the object with name NAME in the TO direction.
*/
Data* Data::to(HString name) {
  data.it=data.mapto.find(name);
  if (data.it != data.mapto.end()){return data.to[data.it->second]->ref;}
  else {ERROR("data.to(): unknown reference \"" << name << "\""); return NULL;};
};

/*!
\brief Creates a new object between the current object (in TO direction) and the neighbouring object of name NAME. 

 */
Data* Data::create(HString name){
  Data * d = new Data(name,data.superior);
  return d;
}
/*!
\brief Creates a new object between the current object (in TO direction) and the neighbouring object of name NAME and return an Object (as reference) - for Python wrapping. 

 */
Data& Data::create_Ref(HString name){return *(create(name));}


/*!
\brief Inserts an object  in TO direction between this object and a neighbouring object.

 If the neighbouring object is an immediate neighbour, it will not be
 any more ofter this operation. If it is further away, simple links
 will be created between this->d->neighbour (and hence it may get a bit closer).
 
Python wrapper...
*/
Data& Data::insert_Ref(Data & d, Data & neighbour){return *insert(&d,&neighbour);}

/*!
\brief Inserts an object  in TO direction between this object and a neighbouring object.

 If the neighbouring object is an immediate neighbour, it will not be
 any more ofter this operation. If it is further away, simple links
 will be created between this->d->neighbour (and hence it may get a bit closer).
 */

Data* Data::insert(Data * d, Data * neighbour){
  DBG("insert(d=" << d->getName(true) << ", neighbour=" << neighbour->getName(true) <<"): Start");
  map<HString,int>::iterator it=data.mapto.find(neighbour->getName());
  if (it != data.mapto.end()){
    int port=it->second;
    reference_descr * rd=data.to[port];
    DBG("insert(d=" << d->getName(true) << ", neighbour=" << neighbour->getName(true) <<") port=" << port<<", rd->port=" << rd->port);
    setLink(d,rd->direction,DIR_TO,-1,port);
    neighbour->setLink(d,rd->direction,DIR_FROM,-1,rd->port);
    //    d.touch();
  } else {
    DBG("insert(non-neighbour: d=" << d->getName(true) << ", neighbour=" << neighbour->getName(true) <<")");
    setLink(d,data.defdir,DIR_TO);
    neighbour->setLink(d,data.defdir,DIR_FROM);
  };
  return d;
}



/*!
\brief Inserts a new object between the current object and the neighbour object (in TO direction)

 */
Data* Data::insertNew(HString newname, Data * neighbour){
      return insert(create(newname),neighbour);
}

/*!
\brief Inserts a new object between the current object and the neighbour object (in TO direction)
Python wrapper 
*/
Data& Data::insertNew_Ref(HString newname, Data & neighbour){
  return *insertNew(newname,&neighbour);
}

/*!
Currently the property list is not really used - delete it?
*/

void Data::setProperty(HString name,HString value){
  if (this==&NullObject) {ERROR("Operation on NullObject not allowed."); return;};
  map<HString,HString>::iterator it;
  if (data.prop_ptr==NULL) {data.prop_ptr=new map<HString,HString>;}; //create if not yet present
  (*data.prop_ptr)[name,value];
  return;
};

HString Data::getProperty(HString name){
  if (data.prop_ptr==NULL) {return "";};
  map<HString,HString>::iterator it;
  it=data.prop_ptr->find(name);
  if (it != data.prop_ptr->end()){return it->second;} else {return "";};
};


/*!
  Creates a new, dynamic data vector in the object (a "buffer") of a given type
*/
void Data::newVector(DATATYPE type){
  if (data.d_ptr!=NULL) {delData();};
  data.type=type; 
  data.d_ptr=new_vector(data.type,0); 
  data.len=0;
  DBG("new-vec data.d_ptr=" << data.d_ptr);
}

void Data::delData(){
  del_vector(data.d_ptr,data.type); 
  data.d_ptr=NULL; 
  data.type=UNDEF;
}

/*! \brief Send a signal to PyQt.

 Invokes the internally stored PyQt object and calls the method
 specified in the input string. This is used, for example, to send an
 updated() signal to the GUI.
*/


void Data::signalPyQt(HString signal){
  if (data.pyqt==NULL) return;
  const char *AttribStr = signal.c_str();
  //we need to check if the process attribute is present in the Python Object
  if (!PyObject_HasAttrString(data.pyqt, AttribStr)) {
    ERROR("signalPyQt: Object does not have Attribute " << AttribStr << "."  << ", name=" << getName(true)); return;};
  DBG("signalPyQt(signal=" << signal <<"): name=" << getName(true));
  int ret=boost::python::call_method<int>(data.pyqt,AttribStr,boost::ref(*this));
  if (ret!=0) {
    ERROR("signalPyQt(signal=" << signal <<") returned user-defined error code" << ret << ", name=" << getName(true));
  };
}

/*! \brief Store a reference to a python QObject reference in the object.

  The QObject is used to emit and receive Qt Signals to communicate with a GUI.
  See setPyQt under python

  #REF: setPyQt(Py), deletePyQt, retrievePyQt, retrievePyQtObject
 */
Data& Data::storePyQt(PyObject* pyobj) {
  data.pyqt=pyobj;
  return *this;
}

/*! \brief Delete the reference to the python QObject reference in the object.

  The QObject is used to emit and receive Qt Signals to communicate with a GUI.

  #REF: setPyQt(Py), retrievePyQt, storePyQt, retrievePyQtObject
 */
Data& Data::deletePyQt(PyObject* pyobj) {
  data.pyqt=NULL;
  return *this;
}

/*! \brief Retrieve a reference to the Qt Object.

  The QObject is used to emit and receive Qt Signals to communicate with a GUI.

  #REF: deletePyQt, storePyQt, retrievePyQtObject
 */
PyObject* Data::retrievePyQt() {return data.pyqt;}


/*! \brief Retrieve under python the Qt Object.

  The QObject is used to emit and receive Qt Signals to communicate with a GUI.

  #REF: deletePyQt, retrievePyQt, storePyQt, retrievePyQt
 */
boost::python::handle<> Data::retrievePyQtObject() {
    return boost::python::handle<>(boost::python::borrowed(data.pyqt));
}

/*!
\brief Stores the same DBG python object in all data objects. Usefulf for debugging.
 */

Data& Data::AllStorePyDBG(PyObject* pyobj) {
  if (this==&NullObject) {ERROR("AllStorePyDBG: Operation on NullObject not allowed."); return NullObject;};
  HInteger i;
  if (data.superior!=NULL) {
    for (i=0; i<data.superior->data_objects.size(); ++i) {
	data.superior->data_objects[i]->storePyDBG(pyobj);
    };
  } else {
      ERROR("AllVerbose: No superior created yet, can't find any objects!");
  };
  return *this;
}

/*! \brief Call an attribute of the PyDBG function.

 Invokes the internally stored PyDBG object and calls the method
 specified in the input string. This is used, for example, to send an
 execute() signal to Python.
*/

void Data::signalPyDBG(HString signal, HString input){
  if (data.pydbg==NULL) return;
  const char *AttribStr = signal.c_str();
  //we need to check if the process attribute is present in the Python Object
  if (!PyObject_HasAttrString(data.pydbg, AttribStr)) {
    ERROR("signalPyDBG: Object does not have Attribute " << AttribStr << "."  << ", name=" << getName(true)); return;};
  DBG("signalPyDBG(signal=" << signal <<"): name=" << getName(true) << ", input=" << input);
  int ret=boost::python::call_method<int>(data.pydbg,AttribStr,boost::ref(*this),input.c_str());
  if (ret!=0) {
    ERROR("signalPyDBG(signal=" << signal <<") returned user-defined error code" << ret << ", name=" << getName(true));
  };
}

/*! \brief Store a reference to a python QObject reference in the object.

  The QObject is used as a hook, which can be called whenever a Debug
  step is made. I.e. whenever a "verbose" line is encountered (in get)
  then the debug function is called.

  #REF: setPyDBG(Py), deletePyDBG, retrievePyDBG, retrievePyDBGObject
 */
Data& Data::storePyDBG(PyObject* pyobj) {
  data.pydbg=pyobj;
  return *this;
}

/*! \brief Delete the reference to the python QObject reference in the object.

  The QObject is used to emit and receive Qt Signals to communicate with a GUI.

  #REF: setPyDBG(Py), retrievePyDBG, storePyDBG, retrievePyDBGObject
 */
Data& Data::deletePyDBG(PyObject* pyobj) {
  data.pydbg=NULL;
  return *this;
}

/*! \brief Retrieve a reference to the Qt Object.

  The QObject is used to emit and receive Qt Signals to communicate with a GUI.

  #REF: deletePyDBG, storePyDBG, retrievePyDBGObject
 */
PyObject* Data::retrievePyDBG() {return data.pydbg;}


/*! \brief Retrieve under python the Qt Object.

  The QObject is used to emit and receive Qt Signals to communicate with a GUI.

  #REF: deletePyDBG, retrievePyDBG, storePyDBG, retrievePyDBG
 */
boost::python::handle<> Data::retrievePyDBGObject() {
    return boost::python::handle<>(boost::python::borrowed(data.pydbg));
}

/*! \brief Store a reference to a python function in the object.

  The python function is used to run user defined functions

  #REF: deletePyFunc, retrievePyFunc, retrievePyFuncObject
 */
Data& Data::storePyFunc(PyObject* pyobj) {
  data.py_func=pyobj;
  return *this;
}

/*! \brief Retrieve a reference to a python function in the object.

  The python function is used to run user defined functions

  #REF: deletePyFunc, storePyFunc, retrievePyFuncObject
 */
PyObject* Data::retrievePyFunc() {return data.py_func;}

/*! \brief Delete the reference to the python QObject reference in the object.

  The QObject is used to emit and receive Func Signals to communicate with a GUI.

  #REF: storePyFunc, retrievePyFunc, retrievePyFuncObject
 */
Data& Data::deletePyFunc(PyObject* pyobj) {
  data.py_func=NULL;
  return *this;
}

/*! \brief Retrieve under python the user-defined python function in the object.

  The python function is used to run user defined functions in the object.

  #REF: deletePyFunc, storePyFunc, retrievePyFunc
 */
boost::python::handle<> Data::retrievePyFuncObject() {
    return boost::python::handle<>(boost::python::borrowed(data.py_func));
}

/*! 

\brief Calls a simple method (i.e., no return value, parameter is the
current Data object) of a python object.

 */
bool Data::callSimplePyObjectMethod(PyObject* pyobj, HString method) {
  if (pyobj==NULL){
    ERROR("callSimplePyObjectMethod: pointer to PythonObject is NULL. Object PythonObject does not exist. Define PythonObject and use pytore before assigning PyFunc to this Object."); 
    return false;
  };
  //first we need to check if attribute is present in the Python Object
  if (!PyObject_HasAttrString(pyobj, method.c_str())) {
    ERROR("callSimplePyObjectMethod: Object does not have Attribute " << method); return false;};
  int ret=boost::python::call_method<int>(pyobj,method.c_str(),boost::ref(*this));
  if (ret!=0) {
    ERROR("callSimplePyObjectMethod - startup method returned user-defined error code" << ret);
    return false;
  };
  return true;
}



/*! \brief Store a list of python object reference in the  Data buffer.

Used to store the pointer to Python objects. The Python object can be retrieved with getPyList.
 */

Data& Data::putPyList(boost::python::object& obj) {
  boost::python::list lst = boost::python::extract<boost::python::list>(obj);
  address i,l=boost::python::extract<HInteger>(lst.attr("__len__")());
  vector<HPointer> vp;
  vp.reserve(l);
  PyObject* pyobj;
  boost::python::object oitem;
  for (i=0;i<l;++i) {
      oitem=lst[i];
      vp.push_back(reinterpret_cast<HPointer>(oitem.ptr()));
       }
  put(vp);
  return *this;
}

/*! \brief Retrieve a list of python object references from the  Data buffer.

Used to store the pointer to Python objects. The Python object can be stored with putPyList.
 */

boost::python::handle<> Data::getPyList() {
  boost::python::list lst;
  vector<HPointer> vp;
  get(vp);
  address i,l=vp.size();
  PyObject* pyobj;
  boost::python::object obj;
  for (i=0;i<l;++i) {
      pyobj=reinterpret_cast<PyObject*>(vp[i]);
      obj=boost::python::object(boost::python::handle<>(boost::python::borrowed(pyobj)));
      lst.append(obj);
    };
    return boost::python::handle<>(boost::python::borrowed(lst.ptr()));
}

/*! \brief Store a python object reference in the  Data buffer.

Used to store the pointer to a Python object. The Python object can be retrieved with getPy().
 */

Data& Data::putPy(PyObject* pyobj) {
  vector<HPointer> vp(1,reinterpret_cast<HPointer>(pyobj));
  put(vp);
  return *this;
}
/*! \brief Store a pyhton object reference in the  Data buffer and cause no update.
Used to store the pointer to a Python object. The Python object can be retrieved with pyretrieve.
 */
Data& Data::putPy_silent(PyObject* pyobj) {
  vector<HPointer> vp(1,reinterpret_cast<HPointer>(pyobj));
  DOSILENT(put(vp));
  return *this;
}

/*!  \brief Retrieve a pyhton object from Data buffer.

Used retrieve a python object which was stored with putPy. This will
create a new reference to the same python object stored with putPy.
 */
boost::python::handle<> Data::getPy() {
    return boost::python::handle<>(boost::python::borrowed(reinterpret_cast<PyObject*>(getOne<HPointer>())));
}

/*! \brief put a vector value into the object without updating the network
 */
template <class T>
void Data::put_silent(vector<T> &v) {
  bool sil=Silent(true);
  put(v);
  Silent(sil);
}


/*! \brief put a single value into the object without updating the network
 */
template <class T>
void Data::putOne_silent(T v) {
  bool sil=Silent(true);
  putOne(v);
  Silent(sil);
}


//return_value_policy<manage_new_object>()

template <class T>
void Data::putOne(T one) {
  vector<T> vp(1,one);
  DBG("putOne: Type=" << datatype_txt(WhichType<T>()) << " val=" << one);
  put(vp);
}

template <class T>
void Data::putOne(HString name, T one) {
  Ptr(name)->putOne(one); 
}

template <class T>
void Data::putOne(objectid oid, T one) {
  Ptr(oid)->putOne(one); 
}

/*
The follwing overloaded methods should be really removed and replaced by a call invoking data["name"]...
 */
template <class T>
void Data::put(HString name, vector<T> &vin) {
  Ptr(name)->put(vin); 
}

template <class T>
void Data::put(objectid oid, vector<T> &vin) {
  Ptr(oid)->put(vin); 
}

/*!

\brief Do not set modification flag during next put operation

This method allows one to modify an object without setting the modification
flag. This is valid only for the next operation. This should be used with
care, since it can lead to an inconsistent network.
 */

Data* Data::noMod(){
    data.noMod=true;
    return this;
}

Data& Data::noMod_Ref(){return *noMod();}

/*!  \brief A flag is set whenever the object received a modification
message that a link further down the net was changed.

 This is used by the object functions to determine whether they need
to go through the net again and find the parameter objects.
 */
Data* Data::setLinkpathChanged(bool change){data.linkpathchanged=change;return this;}

/*!  \brief Retrieve the flag which tells one whether the object had
received a modification message that a link further down the net was
changed.

This is used by the object functions to determine whether they need
to go through the net again and find the parameter objects.
 */
bool Data::getLinkpathChanged(){return data.linkpathchanged;}


/*!

\brief Do not send a PyQt signal during the next put operation

This method allows one to modify an object without sending a Signal
and is used by the GUI to change a parameter from the user without
creating an infinte loop between GUI and data object. This flag is
valid only for the next operation and will be qutomatically reset
after the put operation.
 */
Data & Data::noSignal(){
    data.noSignal=true;
    return *this;
}

/*! \brief Put a new value into the object data vector

This is the basic mechanism to assign new data values. One can only
assign whole vectors not individual elements. For convenience the
method putOne exists which has a scalar as input, but will then
convert it to a vector and call put. If the object previously had no
data vector, it will be created. If an existing data vector had a
different type than the new vector, the old vector will be delete and
the new one - of different type - will be created. After the data has
been assigned a "setModification" call will be made to signal the
network that a value has changed (which can cause other objects to
reevalute). Also, if a connection to a PyQT GUI slot has been created,
a "signalPyQT" call will be issued that allows the GUI to update the
contents of the respective input field connected to this object.

If, however the new vector is identical to the old vector, no action
is undertaken and not signal or setModification call will be
made. This avoids infinite loops, e.g. between GUI and data object and
spares unnessecary reevaluation of the network. If that is desire, you
need to call "touch".

 */
template <class T>
void Data::put(vector<T> &vin) {
  bool oldvec=true;
  if (this==&NullObject) {ERROR("Operation on NullObject not allowed."); return;};
  DEF_D_PTR(T);
  if (data.d_ptr==NULL) {
    oldvec=false;
    DBG("Put Vector: name=" << data.name);
    data.origin=REF_MEMORY; 
    newVector(WhichType<T>());
    DBG("Put Vector: d_ptr=" << data.d_ptr << " data.type=" << data.type);
  } else if (data.type!=WhichType<T>()) {
    oldvec=false;
    DBG("Delete Vector: name=" << data.name);
    DBG("Delete< Vector: d_ptr=" << data.d_ptr << " data.type=" << data.type);
    delData(); 
    newVector(WhichType<T>());
    DBG("Put Vector: name=" << data.name);
    DBG("Put Vector: d_ptr=" << data.d_ptr << " data.type=" << data.type);
  };
  d_ptr_v=data.d_ptr;
  if (oldvec && ((*d_ptr_T) == vin)) {
    /*Check whether the values are the same, if so, don't do anything - this avoids useless loops*/
    DBG("put: name=" << getName(true) << " == Identical value - Put ignored");
  } else {
    (*d_ptr_T) = vin;
    data.len=vin.size();
    DBG("put: d_ptr=" << data.d_ptr << ", &vin=" << &vin << ", len=" << data.len);
    setModification();  //Note that the data has changed
    if (data.noSignal) {data.noSignal=false;}
    else {signalPyQt("updated");}; //Tell a GUI, if present, that this has changed 
  };
}

/*!
  This method deletes an object function if present 
*/
void Data::delFunction(){
  ObjectFunctionClass * of = getFunction();
  if (of!=NULL) {
    //Calls the function and destruct the local static variables in the function
    delete of;
    data.of_ptr=NULL;
  };
}

/*!  
\brief This method adds a function to the object that will
  performed whenever an update is requested.

 If there is a previous function it will be deleted first, unless the
  new function is exactly the same. In this case no action is
  performed. You will have to call delFunction explicitly first.
*/
Data& Data::setFunction (const HString name,
			const HString library,
			const DATATYPE typ)
{
  if (this==&NullObject) {ERROR("Operation on NullObject not allowed."); return;};
  if (data.superior!=NULL && data.superior->library_ptr!=NULL) {
    if (data.superior->library_ptr->inLibrary(name,library)) {
      DBG("setFunction(" << name << "," << library << "," << datatype_txt(typ) << "): name=" << getName(true));
      if (data.of_ptr != NULL) {
	//Check if it is the same function as the existing one.
	if (data.of_ptr->getName()==name && (data.of_ptr->getLibrary()==library || library=="") && data.of_ptr->getType()==typ) return *this;
	else delFunction(); //Delete old function otherwise
      };
      DataFuncDescriptor fd = *(data.superior->library_ptr->FuncDescriptor(name,library));
      data.of_ptr = fd.newFunction(this,typ);
      data.origin=REF_FUNCTION;
      setUpdateable();  //Make the function updateable by default
      setModification();
    } else {
      ERROR("setFunction: Function " << name << " in library " << library << " not found.");
      return *this;
    };
  } else {
    ERROR("setFunction: superior object or Function library non-existent.");
    return *this;
  };
  return *this;
}

/*!
\brief Lists all possible object functions that are accessible in the functions library. 

Returns a vecor of strings with the relevant information. If
doprint=true the output will be printed to standard out.

If lib=="" list all functions and their docstring. 
If lib=="*" list only the names of the libraries.
If lib=="libname" list only the function names available within a certain library.

*/
vector<HString> Data::listFunctions (HString lib, bool doprint){
  if (data.superior->library_ptr!=NULL) {
    if (lib=="") {return data.superior->library_ptr->listFunctions(doprint);}
    else if (lib=="*") {return data.superior->library_ptr->getLibraries(doprint);}
    else  {return data.superior->library_ptr->getFuncnames(lib,doprint);}
  } else {
    ERROR("listFunctions: Function library non-existent.");
    vector<HString> nix;
    return nix;
  };
}


//Needed for Python Bindings
template <class T> T Data::getOne_0_(){return getOne<T>();}

template <class T>
T Data::getOne(const address i){
  Vector_Selector *vs;
  vs = new Vector_Selector;
  vs->setOne(i);
  vector<T> v(1);
  get(v,vs);
  delete vs;
  return v.at(0);
}


/*!  \brief Update contents of onbject and execute function.

Execute the internal function of the object and update the
stored vector if present. Also propagate an updated message through
the network, depending on the type of connection (PUSH/PULL). If you
want to just mark the object as modified but not update itself (only
the ones higher up) use .touch() instead.
*/

Data& Data::update(){
  DBG("Update: name=" << data.name << " Empty=" << tf_txt(Empty())<< " silent=" << tf_txt(data.silent));
  if (hasFunc()) {
    DEF_D_PTR(HNumber);
    d_ptr_v=new_vector(data.type,data.buflen);
    CALL_FUNC_D_PTR(get);
    del_vector(d_ptr_v, data.type);
  };
  return *this;
}

/*!  \brief Update all objects in the network

updateAll will start with the current object and then sequentially go
through all objetcs according to their object ID backwards. Can be
used to initialize the network.
*/

void Data::updateAll(){
  HInteger i;
  update();
  if (data.superior!=NULL) {
    for (i=data.superior->data_objects.size()-1; i>=0; i--) {
      SaveCall(data.superior->data_objects[i]) data.superior->data_objects[i]->update();
    };
  } else {
    ERROR("updateAll: No superior created yet, can't find any objects! (Should never happen...)");
  }
}

/*!  \brief If necessary updates the current content of the object and
calls its internal function. 

AutoUpdate does not perform a full modification check (i.e. does not call
checkModification), but relies on the modification flag to be properly
set. This is the case when the update worm has crawled through the
network. It also respects the AutoUpdate flag.
*/

void Data::doAutoUpdate(){
  if (Verbose()) doVerbose();
  if (isModified()) {
    if (hasFunc() && Updateable()) {
      if (doesAutoUpdate()) update();
    } else {
    //These are objects which can't be updated, so just clear the update flag.
      clearModification();
    };
  };
}


/*!
  Go backward or forward through the net until you find an object of the
  specified name returns NULL if not found and "this" (pointer to current object)
  for an empty string.
*/
Data* Data::find_name(const HString name, const DIRECTION dir) {
  Data *D_ptr=&NullObject; 
  if (name != data.name && name!="") {
    objectid i;
    vector<reference_descr*> *p_ref;
    if (dir==DIR_TO) {p_ref = &data.to;} else {p_ref = &data.from;};
    for (i=0;i<(*p_ref).size() && D_ptr==&NullObject;++i){
      D_ptr=(*p_ref)[i]->ref->find_name(name,dir);
    };
    return D_ptr;
  } else {
    return this;
  };
}


/*!
Check if any of the immediate relatives in direction dir has a given name and return
pointers to these objects. Returns multiple pointers in vec if multiple names are present.
The function returns true if the relative is found, and false if not.
*/

  DataList Data::find_immediate_relatives(const HString name, const DIRECTION dir) {
  objectid i;
  vector<reference_descr*> *p_ref;
  DataList vec;
  D2BG("find_immediate_relatives: name=" << name << " dir=" << direction_txt(dir) << " current object=" << data.name);
  if (dir==DIR_TO) {p_ref = &data.to;} else {p_ref = &data.from;};
  for (i=0;i<(*p_ref).size();++i){
    if ((*p_ref)[i]->name==name) {
      vec.push_back((*p_ref)[i]->ref);
    };
  };
  D2BG2("find_immediate_relatives: found=" << tf_txt(vec.size()>0) << " i=" << i); D2BG3(printvec(vec));
  return vec;
}

/*!  
\brief Leave a trail marker when searching the net, indicating
that this object has been visited. Hence, a circular vist can be
detected and stopped.
*/
void Data::dropVisitmarker(visit_marker vm){data.received_visit_marker=vm;}

/*!  
\brief Generate a new unique visitmarker, that can be dropped.
*/
visit_marker Data::getNewVisitmarker(longint v){
  visit_marker vm;
  if (v<0) {
    data.visitmarkers++;
    if (data.visitmarkers>=MAXVERSION) data.visitmarkers=0;
  } else {data.visitmarkers=v;};
  vm.action=MOD_VISITED;
  vm.ref=this;
  vm.version=data.visitmarkers;
  return vm;
}

/*!
\brief Check whether an object has already been visited, by looking
for an identical visit marker.
*/
bool Data::checkVisited(visit_marker vm){return data.received_visit_marker==vm;}

/*!Check if any of the immediate relatives in direction dir has a
given name and return pointers to these object. Returns multiple
pointers in vec if multiple names are present at the same level in the
network.  If not, seek the next generation of relatives. if vector
elems contains elements the additional requirement is that the data
vector (i.e., its first element) is among the elements.
*/
template <class T>
DataList Data::find_relatives(const HString name, const vector<T> &elems, const DIRECTION dir) {
  DataList neighbours,next_neighbours,result;
  address i,level=0,n_size,el_size=elems.size();
  bool found=false;
  visit_marker vm=getNewVisitmarker();
  D2BG2("find_relatives("<<name<<", dir="<<direction_txt(dir)<<"): getName=" << getName(true)<<" elems=");D2BG3(printvec(elems));
  neighbours=getNeighbours(dir);
  n_size=neighbours.size();
  while (result.size()==0 && n_size>0) {
    for (i=0;i<n_size; ++i) {
      found=false;
      SaveCall(neighbours[i]) {
	if (neighbours[i]->getName()==name) {
	  if (el_size==0) found = true;  //No elements given, hence no further content-based selection necessary
	  else found = object_logic_in_set(neighbours[i],elems); //Test whether contents of object is among those listed in elem
	};
      };
      if (found) {
	result.push_back(neighbours[i]);
      };
    };
    //Produce a vector with all neighbours on the next level if nothing was found
    if (result.size()==0) {
      next_neighbours.clear();
      for (i=0;i<n_size;++i) {
	SaveCall(neighbours[i]) {
	  if (neighbours[i]!=this && !neighbours[i]->checkVisited(vm)) {// stop at circular network
	    neighbours[i]->dropVisitmarker(vm);
	    vec_append(next_neighbours,neighbours[i]->getNeighbours(dir)); 
	  };
	};
      };
      //vec_unique_copy(next_neighbours,neighbours);
      neighbours=vec_unique(next_neighbours);
      n_size=neighbours.size();
    };
  };
  //result=vec_unique(result); - This is done in Find again ...
  return result;
}


//find objects of the given name (includes : and ' and selection specifications)
//if rpos>0 then only take rpos objects from the right side
//if rpos<0 then drops the last -npos objects from the right side
//rpos is defaulted to 0

vector<objectid> Data::IDs(HString s) {return DPtrToOid(Find(s));}
objectid Data::ID(HString s) {
    DataList vec = Find(s);
    if (vec.size()==0) {return -1;};
    return DPtrToOid(vec).front();
}

/*
Returns a reference  to the object with the given object ID or Name
 */

Data& Data::operator[] (HString name) {return Object_Name(name);}
Data& Data::operator[] (objectid oid) {return Object_ID(oid);}

Data& Data::Object(HString name) {return Object_Name(name);}
Data& Data::Object(objectid oid) {return Object_ID(oid);}
Data& Data::Object(Data &d) {return d;}


Data& Data::Object_Name(HString name) {
  Data* ptr=Ptr(name);
  if (ptr==&NullObject || ptr==NULL) {
    DBG("Object_Name: Object of name=" << name << " not found.");
    return NullObject;
  };
  return *ptr;
}

Data& Data::Object_ID(objectid oid) {
  Data* ptr=Ptr(oid);
  if (ptr==&NullObject || ptr==NULL) {
    DBG("Object_ID: Object of ID=" << oid << " not found.");
    return NullObject;
  };
  return *ptr;
}

Data& Data::Object_Ref(Data &d) {
  return d;
}


/*
Returns a pointer to the object with the given Object ID
 */
Data* Data::Ptr(objectid oid) {
  if (data.superior!=NULL) {
    if (oid<data.superior->data_objects.size()){
      return (data.superior->data_objects[oid]);
    } else {
      ERROR("Ptr: OID too large.");
    }
  } else {ERROR("Ptr: No superior exists ...!? (internal error).");};
  return &NullObject;
}

/*!

\brief Returns a pointer to the first object with the given name
 */
Data * Data::Ptr(HString name) {
  DataList vec = Find(name);
  if (vec.size()==0) {
    return &NullObject;
  } else {return vec.front();};
}

/*!

\brief Returns a list of pointers with objects who's names match the input string. This goes through all objects in the network

If the name is preced by a '*' the string should be contained in the
object name, otherwise, or if preceded by a '=', an exact match is
required. 

See Find() for more information.

 */

DataList Data::Search(const HString s) {
  DataList dl;
  HString ss;
  if (s.size()==0) return dl;
  if (data.superior!=NULL) {
    DataList::iterator it=data.superior->data_objects.begin();
    DataList::iterator end=data.superior->data_objects.end();
    if (s[0]=='*') {
      ss=s.substr(1);
      int size=ss.size();
      while (it!=end) {
	if (*it!=&NullObject) {
	  SaveCall(*it) {
	    if ((*it)->getName().find(ss)<size) {dl.push_back(*it);}
	  };
	};
	it++;
      };
    } else {
      if (s[0]=='=') ss=s.substr(1);
      else ss=s;
      while (it!=end) {
	if (*it!=&NullObject) {
	  SaveCall(*it) {
	    if ((*it)->getName()==ss) {dl.push_back(*it);}
	  };
	};
	it++;
      };
    };
  } else {
    ERROR("Search (" << s<<") Oid=" << getOid() << " - no Superior created yet - Something is wrong!");
  };
  return dl;
};
    
/*!
\brief Search for the objects of name s in the network and return a DataList with them (a vector fo pointers to the Data objects).

If s is preceded by '*' or '=' a global search is preformed, where the
entire network is searched for a partial or exact match,
respectively. 

If preceded by "'" or by ":" the search will go along the network
either backwards or forwards, respectively. 

Default search direction, if no special character is given as first
character, is forward (":"). 

Multiple search steps can be provided in a search path using "'" or
":" in s. 

A name followed by an equal sign ("=") means that the first element of
the data vector should contain the value following the equal sign. It
is envisaged that a more elaborate logic can be employed in the future.

Multiple values can be specified separated by a comma.

Hence, a search path specified as "NAME1'NAME2:NAME3=5" means that
first NAME1 is searched in the forward direction. Then the search
proceeds from NAME1 backwards through the net until one finds "NAME2".
Finally the search progresses forward again until an object is found
with the name NAME3 and which contains the value 5. That object is
returned (not NAME1 or NAME2!).

The search proceeds by levels. The object closest to the starting
point is returned. If several objects of the same name exist on the
same level, then they are all returned.

For a global search all objects matching the search criterion are
returned. The global search is mainly implemented for interactive use
and is depreciated for programming purposes.

 */
DataList Data::Find(const HString s, const int rpos) {
  DataList vec_in,vec_out;
  bool found=true;
  bool selection;
  HString name;
  vector<HString> names,elems;
  vector<DIRECTION> dirs;

  if (this==&NullObject || s.size()==0) return vec_in;
  if (s[0]=='*' || s[0]=='=') return Search(s);
  parse_record_name_to_vector(s, names, dirs);
  DBG("Find(name="<<getName(true)<<", s=\"" << s << "\", rpos=" << rpos << ")");

  objectid i,beg=0,end=names.size()-1;
  objectid j,n;
  vec_in.push_back(this); n=1;
  if (rpos>0) {beg=max(end-rpos+1,0);};
  if (rpos<0) {end=end+rpos;};
  for (i=beg;i<=end && n>0;++i) {
    //Check if the object names also have an additional condition,
    //e.g. "Antenna=0,1,2". selection=true if that is the case and the
    //elements after '=' are in elems
    parse_record_selection_to_vector(names[i],name,elems);
    vec_out.clear(); 
    for (j=0;j<n;j++) {
      vec_append(vec_out,vec_in[j]->find_relatives(name,elems,dirs[i]));
    };
    vec_in=vec_out; n=vec_in.size();
  };
  return vec_unique(vec_in);
}

//Sequentially go through the list  of names and find the corresponding name in the net, one after the other.
// This corresponds to finding fields in records, such as: name1.name2.name3
//returns the NullObject if not found and "this" if the vector is of length 0.


Data* Data::find_names(const vector<HString> names, const vector<DIRECTION> dir) {
  if (this==&NullObject) {ERROR("Operation on NullObject not allowed."); return &NullObject;};
  Data *D_ptr=this; 
  objectid i;
  for (i=0;i<names.size() && D_ptr!=&NullObject;++i){
    D_ptr=D_ptr->find_name(names[i],dir[i]);
  };
  return D_ptr;
}

/*
\brief Check if an object is empty, i.e. has neither function nor data vector 
*/

bool Data::Empty(){
  return (data.of_ptr==NULL && data.d_ptr==NULL);
}

bool Data::hasFunc(){
    return data.of_ptr!=NULL;
}

bool Data::hasData(){
    return data.d_ptr!=NULL;
}

template <class T>
void Data::inspect(vector<T> &vec){
    if (!hasData()) {return;};
    switch (data.type) {
#define SW_TYPE_COMM(EXT,TYPE)			\
	copycast_vec<TYPE,T>(data.d_ptr, &vec)
#include "switch-type.cc"
	ERROR("inspect: Undefined Datatype encountered while retrieving vector.");
    };
}

HString Data::getFuncName(){
    if (data.of_ptr == NULL) {return "";};
    return data.of_ptr->getName();
}


/*!
This looks for the first object that is available on port 0 and fill
the data vector. This is mainly used by DataFunc functions to get the 
vector to work on.
*/

template <class T>
void Data::getFirstFromVector(vector<T> &v, Vector_Selector* vs){
  for (address i=0;i<data.from.size();i++){
    if (data.from[i]->name!="Parameters") {
      SaveCall(data.from[i]->ref) {
	data.from[i]->ref->get(v,vs);
	i=data.from.size();
      }
    };
  };
  if (i==data.from.size()) {
    ERROR("getFirstFromVector: No related object. Object=" << getName());
  };
}


//Thin python wrapper for get
template <class T> 
void Data::get_1_(vector<T> &v){get(v);};


/*!
\brief The basic get function to retrieve an object value, which is described in general description more extensively
*/

template <class T> 
void Data::get(vector<T> &v, Vector_Selector *vs) {
  objectid checkmod=false;
  DATATYPE type=WhichType<T>();
  DEF_D_PTR(T);
  ObjectFunctionClass* f_ptr=getFunction();
  d_ptr_v = data.d_ptr;
  bool wormcreated=false;

  //Take the default Vector Selector of the object, if the parameter vs is NULL
  Vector_Selector* s_ptr=data.s_ptr;
  if (vs!=NULL) {s_ptr=vs;}  
  
  //Here the VectorSelector is split into an indexable part that can
  //be applied before calculation and one that is applied afterwards
  //since it is based on the vector's value.
  Vector_Selector *vs1=NULL, *vs2=NULL;
  if (isSelection(s_ptr)){
    if (s_ptr->isIndexable()) vs1=s_ptr;
    else vs2=s_ptr;
  };

  DBG("get: name=" << data.name << ", vs1=" << vs1 << ", vs2=" << vs2);

  if (f_ptr==NULL) {   //No function but data with different type
    if (d_ptr_v==NULL) {
      if (data.from.size()>0) { //No function and no data. This object is only used to organize the web in a nicer way.
	getFirstFromVector(v,vs1); //transparently pass through the values of the first attached object
      } else {
	DBG("No Data in Data Object!" << " name=" << getName(true)); 
      };
      return;
    } else {
      switch (data.type) {  //here we just have a data vector present, so just copy the data buffer to the vector
#define SW_TYPE_COMM(EXT,TYPE) \
	copycast_vec<TYPE,T>(d_ptr_v, &v, vs1)
#include "switch-type.cc"
	ERROR("get: Undefined Datatype encountered while retrieving vector." << " name=" << getName(true));
      };
    };
  } else { //f_ptr != NULL - that means we need to execute a function
      DBG("get: Function " << f_ptr->getName() << " f_ptr=" << f_ptr);
      if (d_ptr_v==NULL) {  //here we have just a function, so execute
			    //it, with output going to the vector to
			    //be returned
        incVersion();
	switch (type){
#define SW_TYPE_COMM(EXT,TYPE) \
	  DBG("get: Calling .process of function " << f_ptr->getName() << "<" << #TYPE << "> started. ");\
	  f_ptr->process_##EXT(static_cast<vector<TYPE>*>(static_cast<void*>(&v)),this, vs1);  //Note: only one of these is actually ever called
#include "switch-type.cc"
	  ERROR("Error (get): Unknown type encountered while processing an object function."  << " name=" << getName(true));
	};  //end switch types
	//	data.len=v.size();
      } else {  //function and vector (buffer) are present, so execute
		//function if network in the from-direction has been
	        //modified, with output to internal data vector and
		//copy that to the output vector
	checkmod=checkModification();
	wormcreated=checkmod && Worm!=NULL && Worm->size()>0;
	DBG("GET: name=" << data.name << " checkmod=" << tf_txt(checkmod) << " worm created=" << tf_txt(wormcreated));
	switch (data.type){
#define SW_TYPE_COMM(EXT,TYPE)					\
	  if (checkmod) {incVersion();f_ptr->process_##EXT(d_ptr_##EXT,this,NULL);};
#include "switch-type.cc"
	    ERROR("Error (get): unknown type." << " name=" << getName(true) );
	};
	switch (data.type){
#define SW_TYPE_COMM(EXT,TYPE)					\
	  copycast_vec<TYPE,T>(d_ptr_v, &v,vs1);
#include "switch-type.cc"
	    ERROR("Error (get): unknown type." << " name=" << getName(true) );
	};
	data.len=v.size();
      }; //end else d_ptr!=0 && f_ptr!=0
  }; // end else f_ptr!=0

  //  if (Verbose()) doVerbose(v,checkmod);

  if (vs2!=NULL) {vs2->select(&v);}; //Select on values if necessary ....
  clearModification();
  if (wormcreated) executeUpdateWorm();
}

Data::Data(HString name,superior_container * superior){

  magiccode = MAGICCODE; //allows one to verify the data object as such

  DBG("Data: name=" << name << " this=" << this);
  data.name=name;
  data.origin=REF_NONE;
  data.buflen=0;
  data.len=0;
  data.dimensions.push_back(0);
  data.dataclass=DAT_CONTAINER;
  data.type=UNDEF;
  data.unit_ptr=NULL;
  data.prop_ptr=NULL;
  data.s_ptr=NULL;
  data.of_ptr=NULL;
  data.d_ptr=NULL;
  data.pyqt=NULL;
  data.pydbg=NULL;
  data.py_func=NULL;
  data.noMod=false;
  data.noSignal=false;
  data.modified=true;
  data.beingmodified=false;
  data.autoupdate=true;
  data.updateable=false;
  data.silent=false;
  data.tmpsilent=false;
  data.linkpathchanged=true;
  data.verbose=0;
  data.debuggui=false;
  data.debug=false;
  data.defdir=PUSH;
  data.netlevel=1;
  data.visitmarkers=0;
  data.received_visit_marker=getNewVisitmarker(0);
  setVersion(1);
  Worm = NULL;

  //There is no superior, so create one
  if (superior==NULL){
    DBG("Data: Create new superior");
    data.superior = new superior_container;
    data.oid=0; 
    data.superior->root=this;
    data.superior->no_of_data_objects=1;
    data.superior->data_objects.push_back(this);
    data.superior->library_ptr=new DataFuncLibraryClass;
    //Initialize the data function libraries
    DataFunc_Library_publish(data.superior->library_ptr); 
  } else {
    data.superior=superior;
    //The superior vector is densely filled, so expand it
    if (data.superior->no_of_data_objects>=data.superior->data_objects.size()) {
      if (data.superior->no_of_data_objects > data.superior->data_objects.size()) {
	ERROR("Data Constructor: no_of_data_objects larger than vector size. Internal error.");
      }
      data.superior->data_objects.push_back(this);
      data.superior->no_of_data_objects=data.superior->data_objects.size();
      data.oid=data.superior->no_of_data_objects-1;
    } else {
      DBG("Data creator: no_of_data_objects=" << data.superior->no_of_data_objects << ", data_objects.size()="<< data.superior->data_objects.size() <<": The superior vector has gaps, so find them.");
      data.oid=0;
      while (data.oid<data.superior->data_objects.size()-1 && data.superior->data_objects[data.oid]!=&NullObject ) {
	data.oid++;
      };
      if (data.oid>=data.superior->data_objects.size() || data.superior->data_objects[data.oid]!=&NullObject) {
	ERROR("Data Constructor: Found no free OID for new object. no_of_data_objects and data_objects are inconsistent. Internal error.");
      } else {
	data.superior->no_of_data_objects++;
	data.superior->data_objects[data.oid]=this;
	DBG("Data Constructor: new object id=" << data.oid);
      };
    };
  };
  DBG("Data: Done!");
}

Data::~Data() {
  objectid i,size;
  magiccode=MAGICCODE_DELETED; //make sure that this is recognized as deleted, should a stray pointers still point at this object
  DBG("Data Destructor: Deleting name=" << getName() << " Oid=" << getOid());
  DBG3(printAllStatus());
  if (data.superior!=NULL) {
    if (getOid()==0) {
      DBG("Destructor: Deleting root object and all related objects");
      for (i=data.superior->data_objects.size()-1;i>0;i--) { //Delete all other objects in superior
	SaveCall(data.superior->data_objects[i]) {
	  data.superior->data_objects[i]->~Data();
	};
      };
    };
    data.superior->data_objects[data.oid]=&NullObject;
    data.superior->no_of_data_objects--;
    if (data.oid==data.superior->data_objects.size()-1) {
      data.superior->data_objects.pop_back();
    };
    //delete the superior if this was the last object in the network
    if (data.superior->no_of_data_objects==0) {
      DBG("Data Destructor: Delete superior");
      delete data.superior->library_ptr;
      data.superior->library_ptr=NULL;
      delete data.superior;
      data.superior=NULL;
    };
  };
  delFunction(); //Deletes the function pointer and local static variables, calls the destructor
  if (data.s_ptr!=NULL) {DBG("Delete Selector " << sel()); del_sel();};
  if (data.d_ptr!=NULL) {DBG("Delete Object: name=" << data.name << ", ptr=" << data.d_ptr); delData();};
  if (data.unit_ptr!=NULL) {delete data.unit_ptr;};
  if (data.prop_ptr!=NULL) {delete data.prop_ptr;};
  if (Worm!=NULL) {delete Worm;};
  size=data.to.size()-1; for (i=size;i>=0;i--) {delLink(i,DIR_TO);};
  size=data.from.size()-1; for (i=size;i>=0;i--) {delLink(i,DIR_FROM);};
}

//Create a new object with name. The name may contain record specification (:'=) to select the parents of the 
//new object otherwise it is the current object. Automatically adds a link to the parent


void Data::delObject_ID(objectid oid)
{
  Ptr(oid)->~Data();
}

void Data::delObject_Name(HString name)
{
  Ptr(name)->~Data();
}

/*! \brief Called by delObject to delete object itself.

 */
void Data::delOtherObject(Data *ptr){
  DBG("delOtherObject(" << ptr << ") name=" << getName(false) <<": Deleting other object -> " << ptr -> getName());
  delete ptr;
}

/*! \brief Erases a neighbouring object from the network and take over its links (for Python binding)
 */
Data& Data::erase_1(Data &d) {erase(d);};

/*! \brief Erases a neighbouring object in direction DIR from the network and take over its links
 */
Data& Data::erase(Data &d,DIRECTION dir)
{
  longint port;
  longint i;
  reference_descr rd;

  port = d.getPort(*this,inv_dir(dir));
  if (port<0) {
    ERROR("erase(neighbour name=" << d.getName(true) << ", dir=" << direction_txt(dir) << ") name=" <<getName(true) <<": Neighbour doesn't know me ...");
    return NullObject;
  };
  d.delLink(port,inv_dir(dir),false,false);

  port = getPort(d,dir);

  DBG("erase(neighbour name=" << d.getName(true) << ", dir=" << direction_txt(dir) << "name=" <<getName(true) <<": Start");

  if (port<0) {
    ERROR("erase(neighbour name=" << d.getName(true) << ", dir=" << direction_txt(dir) << ") name=" <<getName(true) <<": Neighbour not found in specified direction.");
    return NullObject;
  };
  //Now look at the Links of the object to be deleted

  longint n=d.getNumberOfLinks(dir);
  DBG("erase: NumberOfLinks in " << d.getName(true) <<" = " << n);
  for (i=0; i<n; ++i) {
    rd=d.getLink(i,dir);
    DBG("erase: i=" << i);
    if (rd.ref==NULL) {
      ERROR("erase(neighbour name=" << d.getName(true) << ", dir=" << direction_txt(dir) << "name=" <<getName(true) <<": Invalid link returned from Neighbour.");
      return NullObject;
    };
    //Replace the link in the object linked to the erased object with links to this object
    if (i==0) {
      DBG("erase: Special case i=0");
      setLink(rd.ref,rd.direction,dir,rd.port,port);
      SaveCall(rd.ref) rd.ref->setLink(this,rd.direction,inv_dir(dir),port,rd.port);
    } else {
      SaveCall(rd.ref) rd.ref->setLink(this,rd.direction,inv_dir(dir),-1,rd.port);
    }
  };
  for (i=n-1; i>=0; i--) {
    DBG("erase: Deleting Link in dying object port=" << i);
    d.delLink(i,dir,false,false);
  };
  d.delObject();
  //  if (!(data.silent || data.tmpsilent)) touch();
  return *this;
}

/*! \brief Delete an Object and remove it from the network

Will first cut all links, then perform an update, and finally, call
upon the root object to delete the current object from memory.
 */
void Data::delObject()
{
  DataList dvec;
  longint i;

  //First cut all Links in From direction
  longint size=data.from.size(); 
  DBG("delObject() name=" << getName(true) <<": Number of FROM links=" << size);
  DBG3(printStatus());
  for (i=size-1;i>=0;i--) {delLink(i,DIR_FROM);};

  //Cut Links in TO direction and store them locally
  
  size=data.to.size(); 
  DBG("delObject() name=" << getName(true) <<": Number of TO links=" << size);
  for (i=size-1;i>=0;i--) {
    DBG("delLink("<<getName(true)<<") To " <<data.to[i]->name << " " <<data.to[i]->oid);
    dvec.push_back(data.to[i]->ref);
    delLink(i,DIR_TO);
  };

  
  //Create a new modification message
  modification_record newmod;

  newmod.version=incVersion();
  newmod.ref=this;
  newmod.action=MOD_DELETED;

  if (Worm == NULL) {Worm = new dataworm;};
  DBG("delObject: version=" << newmod.version << ", size=" << size);
  //Now signal the modification higher up (but only if the object is
  //not isolated - in that case it was killed by delLink)
  for (i=0;i<size;++i) {
    SaveCall(dvec[i]) 
      {if (dvec[i]->getNumberOfLinks()>0) {
	  DBG2("setMod("<<getName(true)<<") To " <<dvec[i]->getName());
	  dvec[i]->setModification(newmod);
	};
      };
  };
  DBG("delObject: executeUpdateWorm");

  if (size>0) executeUpdateWorm();

  //Call Root object and delete itself...
  if (data.superior!=NULL) {
    if (data.superior->data_objects.size()>=1) {
      SaveCall(data.superior->data_objects[0]) data.superior->data_objects[0]->delOtherObject(this);
    } else {
      ERROR("delObject() name=" << getName() <<": No objects left in superior to delete, including the current object. Something is seriously wrong with the system...");
    };
  } else {
    ERROR("delObject() name=" << getName() <<": No superior created yet, will self-destruct delete myself!");
    delete this;
  };
}

/*!
Creates new objects and returns a vector of objectids. Mainly used for python bindings.
 */
vector<objectid> Data::newObjects_ID(HString name,DIRECTION dir_type) 
{
  DataList objects = newObjects(name,dir_type);
  return DPtrToOid(objects);
}

/*!  \brief Creates new objectx and returns a references to teh first data
  object. Mainly used for python bindings.  Calls newObjects.
 */

Data& Data::newObject_Ref(HString name, DIRECTION dir_type){
    DataList vec=newObjects(name, dir_type);
    if (vec.size()==0) {return NullObject;};
    return *vec[0];
} 

Data& Data::newObject_Ref_1(HString name){return newObject_Ref(name);}

Data* Data::newObject(HString name, DIRECTION dir_type){
    DataList vec=newObjects(name, dir_type);
    if (vec.size()==0) {return NULL;};
    return vec[0];
} 

/*!  Create new objects. The name specifies the name. If it is preceded by a '
then the object will be a predecessor of the current objects. With nothing or
a ":" preceding, the object will be a successor. The name can also have
existing object names, which means that the new object will be attached to
these and not the current object. If there are several objects with this
name on the same level, then multiple objects are created at once.  dir_type
specifies the kind of link (push or pull) - see setLink for a description

 */
DataList Data::newObjects(HString name, DIRECTION dir_type, objectid maxparents) 
{
  DataList parents;
  DataList children;
  DIRECTION dir;
  int n1,n2=0;
  objectid size;
  if (dir_type==DIR_NONE) dir_type=getDefaultDirection();
  //Determine the direction of the link. If name is preceded by ' then the new object links to the old one
  //and is not derived from it.
  n1=string_rfind_chars(name,RECORD_SEP,n2);
  if ((n1>0) && (n2==1)) {dir=DIR_TO;} else {dir=DIR_FROM;};
  DBG("newObjects: determine direction n1 = " << n1 << " n2=" << n2 << " dir=" << direction_txt(dir));
  HString newname=string_rfind_chars_substr(name);
  objectid i;
  if (name!=newname) {parents=Find(name,-1);} else {parents.push_back(this);};
  if (parents.size()==0) {
      ERROR("new(\"" << name << "\"): Parent not found!");
  } else {
      size=maxparents<=0 ? parents.size() : maxparents;
	  for (i=0; i<size; ++i) {
      if (parents[i]!=&NullObject) {
	DBG("Creating new object: " << newname << " i_parent=" << i);
	children.push_back(new Data(newname,data.superior));
	DBG("NewObjects: data.superior=" << data.superior);
	DBG("No of objects=" << data.superior->no_of_data_objects);
	children.back()->setLink(parents[i],dir_type,dir,-1);
      }
      else {ERROR("new(\"" << name << "\"[" << i <<"]): Parent pointer = &NullObject - that should not happend!");};
    };
  };
  return children;
}

template <class T>
DataList Data::newObjects(HString name, T val, DIRECTION dir_type, objectid maxparents) {
  DataList dobs;
  objectid i;
  dobs=newObjects(name,dir_type,maxparents);
  for (i=0;i<dobs.size();++i) {dobs[i]->putOne(val);};
  return dobs;
}

template <class T>
DataList Data::newObjects(HString name, vector<T> vec, DIRECTION dir_type, objectid maxparents) {
  DataList dobs;
  objectid i;
  dobs=newObjects(name,dir_type,maxparents);
  for (i=0;i<dobs.size();++i) {dobs[i]->put(vec);};
  return dobs;
}

//------------------------------------------------------------------------
//End Data class
//------------------------------------------------------------------------




/* ------------------------------------------------------------------------
Test Routines
------------------------------------------------------------------------ 
Currently no longer used - tests are done from Python now.

*/

bool test_result_verbose = true;
map<HString,bool> test_results;

template <class T>
bool test_result(HString routine, vector<T> v1, vector<T> v2){
  bool result=(v1==v2);
  test_results[routine]=result;
  if (test_result_verbose) {
    cout << "Test Result for " << routine <<endl;
    cout << "Expected: "; printvec(v1);
    cout << "Result  : "; printvec(v2);
  };
  cout << "TEST " << routine << " " << tf_txt(result) << endl;
  return result;
}

template <class T>
bool test_result(HString routine, T v1, T v2){
  bool result=(v1==v2);
  test_results[routine]=result;
  if (test_result_verbose) {
    cout << "Test Result for " << routine <<endl;
    cout << "Expected: " << v1 << endl;
    cout << "Result  : " << v2 << endl;
  };
  cout << "TEST " << routine << ": " << pf_txt(result) << endl;
  return result;
}

int test_parse() {
  vector<DIRECTION> v;
  vector<HString> s;
  HString name;
  name="LOFAR:Station:Ant'inc'dec:list"; parse_record_name_to_vector(name,s,v); cout <<name<<endl; printvec(s); printvec_txt(v,&direction_txt);
  name="LOFAR"; parse_record_name_to_vector(name,s,v); cout <<name<<endl; printvec(s); printvec_txt(v,&direction_txt);
  name=":list"; parse_record_name_to_vector(name,s,v); cout <<name<<endl; printvec(s); printvec_txt(v,&direction_txt);
  name="'inc'"; parse_record_name_to_vector(name,s,v); cout <<name<<endl; printvec(s); printvec_txt(v,&direction_txt);

  name="LOFAR:Station:Ant";
  MSG("string_rfind_chars_substr:");
  MSG("name=" << name << " result=" << string_rfind_chars_substr(name));

  MSG(" parse_record_selection_to_vector");
  parse_record_selection_to_vector("Field=1", name, s); cout <<"Field=1: " <<name<<": "; printvec(s);
  parse_record_selection_to_vector("Field=a,b,c", name, s); cout <<"Field=a,b,c: " <<name<<": "; printvec(s);

}

HString mytest(){cout << "Hello HF World" << endl; return "Hi!";}

float* mytest2(vector<HNumber>vec){
  vector<int> v; 
  float* ary;
  
  for (int i=0;i<10; ++i) {v.push_back(i);};
  cout << "mytest2" << endl; 
  return ary;}


mglData * mytest3(){
    mglData * dp;
    dp = new mglData(50);
    dp->Modify("0.7*sin(2*pi*x) + 0.5*cos(3*pi*x) + 0.2*sin(pi*x)");
    return dp;
}

mglData& mytest4(){
    cout << "mytest4" << endl; 
    mglData d(50);
    d.Modify("0.7*sin(2*pi*x) + 0.5*cos(3*pi*x) + 0.2*sin(pi*x)");
    return d;
}

float* mytest5(){
    float d[50];
    int i;
    for (i=0;++i;i<50) {d[i]=i*1.5;}
    return d;
}

vector<HNumber>& mytest6(){
  vector<HNumber> v; 
  for (int i=0;i<10; ++i) {v.push_back(i*1.5);};
  cout << "mytest6" << endl; 
  return v;
}

/*------------------------------------------------------------------------
Here are some functions that are needed to interact between boost python and
SWIG objects. Specifically for getting vector data into an mglData object
------------------------------------------------------------------------*/

#include <mgl/mgl_qt.h>

//The basic wrapper SWIG uses ...
struct PySwigObject {
    PyObject_HEAD 
    void * ptr;
    const char * desc;
};

/*
This function returns the pointer to an object exposed in SWIG 
 */
void* extract_swig_wrapped_pointer(PyObject* obj)
{
    char thisStr[] = "this";
    //first we need to get the this attribute from the Python Object
    if (!PyObject_HasAttrString(obj, thisStr))
        return NULL;
        
    PyObject* thisAttr = PyObject_GetAttrString(obj, thisStr);
    if (thisAttr == NULL)
        return NULL;
    //This Python Object is a SWIG Wrapper and contains our pointer
    return (((PySwigObject*)thisAttr)->ptr);
}

//just prints the pointer of a Python Object
void PyGetPtr(PyObject* obj){
    cout << reinterpret_cast<void*>(obj) << endl;
}
 
/*
in the definiton of our boost python module in the .hpp file, we then have to
add the follwoing line

boost::python::converter::registry::insert(&extract_swig_wrapped_pointer, type_id<mglData>());

which tells boost::python to recognize an object of type mglData and use our
extraction function above.
 */

//This just sets the data in an mgData wrapper to the STL vector vec.  The
//function is exposed to python (in hfget.hpp) and can be called interactively
void mglDataSetVecN(mglData* md, vector<HNumber> &vec){
    HNumber * a = &(vec[0]);
    md->Set(a,vec.size());
}

/*
void mglDataSetVecI(mglData* md, vector<HInteger> &vec){
    HInteger * a = &(vec[0]);
    md->Set(a,vec.size());
}
*/

//------------------------------------------------------------------------
// End Swig compatibility tools
//------------------------------------------------------------------------


//Just a trick to make sure all instances are created, it is
// never intended to actually call this function ...
void instantiate_hfget(DATATYPE type){

  Data d;
  unsigned int ui;

  DEF_D_PTR(HInteger);
  DEF_VAL(HInteger);

  switch (type){

#define SW_TYPE_COMM(EXT,TYPE) \
vectostring(*d_ptr_##EXT); \
 mycast<TYPE>(ui);\
d.getFirstFromVector(*d_ptr_##EXT, NULL);\
d.putOne(*val_##EXT);\
d.putOne_silent(*val_##EXT);\
d.put(*d_ptr_##EXT);\
d.put_silent(*d_ptr_##EXT);\
d.inspect(*d_ptr_##EXT);\
vec_append(*d_ptr_##EXT,*d_ptr_##EXT);			\
WhichType<TYPE>();\
printvec(*d_ptr_##EXT,8);\
printvec_noendl(*d_ptr_##EXT,8);\
set_ptr_to_value(Null_p, INTEGER, *val_##EXT);\
set_ptr_to_value<TYPE>(Null_p, INTEGER,*val_##EXT);\
cast_ptr_to_value<TYPE>(Null_p, INTEGER)
#include "switch-type.cc"
  }
}

/* specialize has_back_reference for Data
namespace boost { namespace python
{
  template <>
  struct has_back_reference<Data>
      : mpl::true_
  {};
}}
*/

#include <GUI/hfget.hpp>
