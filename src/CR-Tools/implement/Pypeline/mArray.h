/**************************************************************************
 *  Header file of Array module for CR Pipeline Python bindings.          *
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

#ifndef CR_PIPELINE_ARRAY_H
#define CR_PIPELINE_ARRAY_H

#include "core.h"
#include "mModule.h"


using namespace std;
using namespace casa;

//========================================================================
//                             Array Class
//========================================================================

template <class T>
  class hArray {

  public:

  struct unit_container {
    HNumber scale_factor;
    HString prefix;
    HString name;
    map<HString,HNumber> prefix_to_factor;
  };

  struct storage_container {
    HInteger *ndims_p;
    HInteger *size_p;
    vector<HInteger> * dimensions_p;
    vector<HInteger> * slice_sizes_p;
    vector<T>* vec_p;
    hArray* parent;
    bool vector_is_shared;
    unit_container unit;
    bool trackHistory;
    vector<HString> history;
    map<HString,HString> keywords;
    map<HString,HPyObjectPtr> keywords_py;
  };

    void init();
    void initialize_storage();
    hArray();
    hArray(const std::vector<T> & vec);
    hArray(storage_container * sptr);
    ~hArray();
    void delVector();
    void delete_storage();
    void new_storage();
    hArray<T> & shared_copy();
    hArray<T> & resize(HInteger newsize);
    hArray<T> & setVector(std::vector<T> & vec);
    std::vector<T> & getVector();
    std::vector<HInteger> & getDimensions();
    std::vector<HInteger> & getSizes();
    void calcSizes();
    void setDimensions1(HInteger dim0);
    void setDimensions2(HInteger dim0, HInteger dim1);
    void setDimensions3(HInteger dim0, HInteger dim1, HInteger dim2);
    void setDimensions4(HInteger dim0, HInteger dim1, HInteger dim2, HInteger dim3);
    void setDimensions5(HInteger dim0, HInteger dim1, HInteger dim2, HInteger dim3, HInteger dim4);
    HInteger getNumberOfDimensions();
    hArray<T> & setSlice(HInteger beg, HInteger end=-1);
    hArray<T> & setSliceVector(vector<HInteger> & index_vector);
    HInteger getSubSliceEnd();
    HInteger getSubSliceStart();
    hArray<T> & setSubSlice(HInteger start, HInteger end, HInteger level);
    hArray<T> & setSubSliceVec(const vector<HInteger> start, const vector<HInteger> & end, const HInteger level);
    typename std::vector<T>::iterator begin();
    typename std::vector<T>::iterator end();
    HInteger getLoop_i();
    HInteger getLoop_nslice();
    HInteger getLoop_start();
    HInteger getLoop_end();
    HInteger loop_slice_start_offset;
    HInteger loop_slice_end_offset;
    HInteger getLoop_increment();
    HInteger getBegin();
    HInteger getEnd();
    HInteger getSize();
    HInteger length();
    bool loopingMode();
    bool doLoopAgain();
    hArray<T> & loop(vector<HInteger> & start_element_index, HInteger start=0, HInteger end=-1, HInteger increment=1);
    hArray<T> & loopVector(vector<HInteger> & start_element_index, vector<HInteger> & vec);
    HInteger setLoopSlice(vector<HInteger> & start_element_index);
    hArray<T> &  next();
    hArray<T> &  loopOn();
    hArray<T> &  loopOff();
    hArray<T> &  resetLoop();
    hArray<T> &  all();
    hArray<T> &  setUnit(HString prefix, HString name);
    hArray<T> &  clearUnit();
    HString  getUnit();
    HString  getUnitName();
    HString  getUnitPrefix();
    hArray<T> &  setHistory(bool on_or_off);
    bool isTrackingHistory();
    hArray<T> &  clearHistory();
    hArray<T> &  addHistory(HString name, HString text);
    vector<HString> & getHistory();
    void printHistory();
    HString getKey(HString key);
    hArray<T> & setKey(HString key, HString contents);

    //hArray<T> & setKeyPy(HString key, HPyObjectPtr pyobj);
    //HPyObject & getKeyPy(HString key);

    //These are the basic parameters describing the data
    //structure. They can be shared

    storage_container * storage_p;
    bool array_is_shared;

    HInteger copycount;

  private:
    HInteger slice_begin, slice_end, slice_size;
    HInteger loop_slice_begin, loop_slice_end, loop_slice_size, loop_lower_level_size;
    HInteger loop_start, loop_end, loop_increment, loop_i, loop_nslice, loop_maxn;
    HInteger subslice_level,subslice_start,subslice_end;
    vector<HInteger> subslice_vec_start,subslice_vec_end;
    vector<HInteger>::iterator subslice_start_it,subslice_end_it;
    vector<HInteger> index_vector;
    bool loop_over_indexvector;
    bool loop_next;
    bool doiterate;
  };


//! Creating a rudimentary Array class, that allows contiguous slicing


template <class T> void hArray<T>::init(){
  //  cout << "Creating hArray ptr=" << reinterpret_cast<void*>(this) << endl;
  storage_p = NULL;
  slice_begin=0;
  slice_end=0;
  slice_size=0;
  array_is_shared=false;
  doiterate=false;
  loop_slice_begin=0; loop_slice_end=0; loop_slice_size=0; loop_lower_level_size=0;
  loop_slice_start_offset=0; loop_slice_end_offset=-1;
  loop_i=0; loop_start=0; loop_end=0; loop_increment=1; loop_maxn=0;
  loop_next=false;
  loop_nslice=0;
  loop_over_indexvector=false;
  setSubSlice(0,-1,-1);
}

extern bool hArray_trackHistory_value;

template <class T> void hArray<T>::new_storage(){
  storage_p=new storage_container;
  storage_p->parent=this;
  storage_p->ndims_p=NULL;
  storage_p->size_p=NULL;
  storage_p->dimensions_p=NULL;
  storage_p->slice_sizes_p=NULL;
  storage_p->vec_p=NULL;
  storage_p->vector_is_shared=false;
  storage_p->unit.prefix_to_factor["f"]=1e-15;
  storage_p->unit.prefix_to_factor["p"]=1e-12;
  storage_p->unit.prefix_to_factor["n"]=1e-9;
  storage_p->unit.prefix_to_factor["micro"]=1e-6;
  storage_p->unit.prefix_to_factor["mu"]=1e-6;
  storage_p->unit.prefix_to_factor["\\mu"]=1e-6;
  storage_p->unit.prefix_to_factor["µ"]=1e-6;
  storage_p->unit.prefix_to_factor["m"]=1e-3;
  storage_p->unit.prefix_to_factor["c"]=1e-2;
  storage_p->unit.prefix_to_factor["d"]=1e-1;
  storage_p->unit.prefix_to_factor[""]=1.0;
  storage_p->unit.prefix_to_factor["D"]=1e1;
  storage_p->unit.prefix_to_factor["h"]=1e2;
  storage_p->unit.prefix_to_factor["k"]=1e3;
  storage_p->unit.prefix_to_factor["M"]=1e6;
  storage_p->unit.prefix_to_factor["G"]=1e9;
  storage_p->unit.prefix_to_factor["T"]=1e12;
  storage_p->unit.prefix_to_factor["P"]=1e15;
  storage_p->unit.prefix_to_factor["E"]=1e18;
  storage_p->unit.prefix_to_factor["Z"]=1e21;
  storage_p->unit.name="";
  storage_p->unit.prefix="";
  storage_p->unit.scale_factor=1.0;
  storage_p->trackHistory=hArray_trackHistory_value;
}

template <class T> void hArray<T>::initialize_storage(){
  if (storage_p==NULL) new_storage();
  if (storage_p->ndims_p==NULL) storage_p->ndims_p=new HInteger;
  if (storage_p->size_p==NULL) storage_p->size_p=new HInteger;
  if (storage_p->dimensions_p==NULL) storage_p->dimensions_p=new std::vector<HInteger>;
  if (storage_p->slice_sizes_p==NULL) storage_p->slice_sizes_p=new std::vector<HInteger>;
  if (storage_p->vec_p==NULL) {
    storage_p->vec_p=new std::vector<T>;
    addHistory((HString)"initialize_storage",(HString)"Vector of type "+typeid(*storage_p->vec_p).name()+" created.");
  };
}

template <class T> void hArray<T>::delete_storage(){
  if (!array_is_shared) {
    if (storage_p->ndims_p!=NULL) {delete storage_p->ndims_p; storage_p->ndims_p=NULL;}
    if (storage_p->size_p!=NULL) {delete storage_p->dimensions_p; storage_p->dimensions_p=NULL;}
    if (storage_p->dimensions_p!=NULL) {delete storage_p->dimensions_p; storage_p->dimensions_p=NULL;}
    if (storage_p->slice_sizes_p!=NULL) {delete storage_p->slice_sizes_p; storage_p->slice_sizes_p=NULL;}
    delVector();
    delete storage_p;
  }
  storage_p=NULL;
}

template <class T> hArray<T>::hArray(){
  copycount=0;
  init();
  initialize_storage();
}

template <class T> hArray<T>::hArray(const std::vector<T> & vec){
  copycount=0;
  init();
  storage_p=new storage_container;
  setVector(vec);
  storage_p->vector_is_shared=true;
  initialize_storage();
}


// ________________________________________________________________________
//                                    Add declarations of wrapper functions

// Tell the wrapper preprocessor that this is a c++ header file
#undef HFPP_FILETYPE
//-----------------------
#define HFPP_FILETYPE hFILE
//-----------------------
#include "../../../../build/cr/implement/Pypeline/mArray.def.h"



// ________________________________________________________________________
//                                         Casting and conversion functions

/* template <class T> */
/* HString pretty_vec(hArray<T> & v,const HInteger maxlen=8); */


template <class T>
HString pretty_vec(hArray<T> & a,const HInteger maxlen){
  vector<T>* v = &a.getVector();
  if (v->size()==0) return "[]";
  int i=1;
  typename vector<T>::iterator it=v->begin();
  HString s=("[");
  s+=hf2string(*it); ++it;  ++i;
  while (it!=v->end() && i< maxlen) {
    s+=","+hf2string(*it); ++it; ++i;
  };
  if ((HInteger)v->size() > maxlen) s+=",...]";
  else s+="]";
  return s;
}


/*!
\brief Creates a new array as a shared copy, i.e. it shares the data vector and dimensions of its parent.

*/

template <class T> hArray<T>::hArray(storage_container * sptr){
  if (sptr==NULL) return;
  copycount=sptr->parent->copycount+1;
  init();
  storage_p=sptr;
  array_is_shared=true;
  initialize_storage();
  setSlice(0,storage_p->vec_p->size());
}

/*! \brief Returns a new hArray object, which shares the same vector and storage description (i.e. dimensions).

The storage will be deleted when the original object is deleted (not
when the shared copy is deleted) - even if the shared copied object is
still around.

 */
template <class T> hArray<T> & hArray<T>::shared_copy(){hArray<T> * ary_p=new hArray<T>(storage_p); return *ary_p;}

template <class T> void hArray<T>::delVector(){
  if (storage_p==NULL) return; //Check if vector was deleted elsewhere
  if ((storage_p->vec_p != NULL) && !storage_p->vector_is_shared) {
    addHistory((HString)"delVector",(HString)"Vector of type"+typeid(*storage_p->vec_p).name()+" and size="+hf2string((HInteger)storage_p->vec_p->size())+" deleted.");
    delete storage_p->vec_p;
  }
  storage_p->vec_p=NULL;
  addHistory((HString)"delVector",(HString)"Reference to Vector of type"+typeid(*storage_p->vec_p).name()+" deleted.");
}

template <class T> hArray<T>::~hArray(){
  //  cout << "Deleting hArray ptr=" << reinterpret_cast<void*>(this) << endl;
  delete_storage();
}

/*!

\brief Set the vector to be stored (as reference, hence no copy is
made). Creation and destruction of this vector has to be done outside
this class!!

*/
template <class T> hArray<T> &   hArray<T>::setVector(std::vector<T> & vec){
  delVector();
  initialize_storage();
  storage_p->vec_p=&vec;
  (*storage_p->size_p)=vec.size();
  setDimensions1((*storage_p->size_p));
  setSlice(0,(*storage_p->size_p));
  storage_p->vector_is_shared=true;
  return *this;
}

/*!
\brief Retrieve the stored vector (returned as reference, hence no copy is made).
 */
template <class T> std::vector<T> & hArray<T>::getVector(){
  if (storage_p==NULL) { static vector<T> v; return v;} //Check if vector was deleted elsewhere
  return *(storage_p->vec_p);
}

/*!
\brief Retrieve the stored vector (returned as reference, hence no copy is made).
 */
template <class T> std::vector<HInteger> & hArray<T>::getDimensions(){
  if (storage_p==NULL) {static vector<HInteger> v; return v;} //Check if vector was deleted elsewhere
  return *(storage_p->dimensions_p);
}

/*!
\brief Calculate the size of one slice in each dimension, will be called whenever dimensions change.
 */
template <class T> void hArray<T>::calcSizes(){
  if (storage_p==NULL) return; //Check if vector was deleted elsewhere
  (*storage_p->slice_sizes_p).resize((*storage_p->dimensions_p).size());
  (*storage_p->slice_sizes_p)[(*storage_p->dimensions_p).size()-1]=1;
  for (int i=(*storage_p->dimensions_p).size()-1; i>0; --i) (*storage_p->slice_sizes_p)[i-1]=(*storage_p->slice_sizes_p)[i]*(*storage_p->dimensions_p)[i];
  setSubSlice(0,-1,-1);
}


/*!
\brief Calculate the size of one slice in each dimension, will be called whenever dimensions change.
 */
template <class T> std::vector<HInteger> & hArray<T>::getSizes(){
  if (storage_p==NULL) {static vector<HInteger> v; return v;} //Check if vector was deleted elsewhere
  return *(storage_p->slice_sizes_p);
}

/*!
\brief Sets the dimensions of the array. Last index runs fastest, i.e., from left to right one goes from large to small chunks in memory.
 */
template <class T> void  hArray<T>::setDimensions1(HInteger dim0){
  if (storage_p==NULL) return ; //Check if vector was deleted elsewhere
  if (storage_p->vec_p==NULL) return ; //Check if vector was deleted elsewhere
  addHistory((HString)"setDimension",(HString)"Dimensions set to ["+hf2string(dim0)+"]");
  (*storage_p->ndims_p)=1;
  if ((*storage_p->dimensions_p).size()!=(uint)(*storage_p->ndims_p)) (*storage_p->dimensions_p).resize((*storage_p->ndims_p));
  (*storage_p->dimensions_p)[0]=dim0;
  (*storage_p->size_p)=hProduct<HInteger>((*storage_p->dimensions_p));
  if (storage_p->vec_p->size() != (uint)(*storage_p->size_p)) storage_p->vec_p->resize((*storage_p->size_p));
  setSlice(0,(*storage_p->size_p));
  calcSizes();
  return ;
}

/*!
\brief Sets the dimensions of the array. Last index runs fastest, i.e., from left to right one goes from large to small chunks in memory.
 */
template <class T> void  hArray<T>::setDimensions2(HInteger dim0, HInteger dim1){
  if (storage_p==NULL) return ; //Check if vector was deleted elsewhere
  if (storage_p->vec_p==NULL) return ; //Check if vector was deleted elsewhere
  addHistory((HString)"setDimension",(HString)"Dimensions set to ["+hf2string(dim0)+","+hf2string(dim1)+"]");
  (*storage_p->ndims_p)=2;
  if ((*storage_p->dimensions_p).size()!=(uint)(*storage_p->ndims_p)) (*storage_p->dimensions_p).resize((*storage_p->ndims_p));
  (*storage_p->dimensions_p)[0]=dim0;
  (*storage_p->dimensions_p)[1]=dim1;
  (*storage_p->size_p)=hProduct<HInteger>((*storage_p->dimensions_p));
  if ((uint)(*storage_p->size_p) != storage_p->vec_p->size()) storage_p->vec_p->resize((*storage_p->size_p));
  setSlice(0,(*storage_p->size_p));
  calcSizes();
  return ;
}

/*!
\brief Sets the dimensions of the array. Last index runs fastest, i.e., from left to right one goes from large to small chunks in memory.
 */
template <class T> void  hArray<T>::setDimensions3(HInteger dim0, HInteger dim1, HInteger dim2){
  if (storage_p==NULL) return ; //Check if vector was deleted elsewhere
  if (storage_p->vec_p==NULL) return ; //Check if vector was deleted elsewhere
  (*storage_p->ndims_p)=3;
  addHistory((HString)"setDimension",(HString)"Dimensions set to ["+hf2string(dim0)+","+hf2string(dim1)+","+hf2string(dim2)+"]");
  if ((*storage_p->dimensions_p).size()!=(uint)(*storage_p->ndims_p)) (*storage_p->dimensions_p).resize((*storage_p->ndims_p));
  (*storage_p->dimensions_p)[0]=dim0;
  (*storage_p->dimensions_p)[1]=dim1;
  (*storage_p->dimensions_p)[2]=dim2;
  (*storage_p->size_p)=hProduct<HInteger>((*storage_p->dimensions_p));
  if ((uint)(*storage_p->size_p) != storage_p->vec_p->size()) storage_p->vec_p->resize((*storage_p->size_p));
  setSlice(0,(*storage_p->size_p));
  calcSizes();
  return ;
}

/*!
\brief Sets the dimensions of the array. Last index runs fastest, i.e., from left to right one goes from large to small chunks in memory.
 */
template <class T> void  hArray<T>::setDimensions4(HInteger dim0, HInteger dim1, HInteger dim2, HInteger dim3){
  if (storage_p==NULL) return ; //Check if vector was deleted elsewhere
  if (storage_p->vec_p==NULL) return ; //Check if vector was deleted elsewhere
  (*storage_p->ndims_p)=4;
  addHistory((HString)"setDimension",(HString)"Dimensions set to ["+hf2string(dim0)+","+hf2string(dim1)+","+hf2string(dim2)+","+hf2string(dim3)+"]");
  if ((*storage_p->dimensions_p).size()!=(uint)(*storage_p->ndims_p)) (*storage_p->dimensions_p).resize((*storage_p->ndims_p));
  (*storage_p->dimensions_p)[0]=dim0;
  (*storage_p->dimensions_p)[1]=dim1;
  (*storage_p->dimensions_p)[2]=dim2;
  (*storage_p->dimensions_p)[3]=dim3;
  (*storage_p->size_p)=hProduct<HInteger>((*storage_p->dimensions_p));
  if ((uint)(*storage_p->size_p) != storage_p->vec_p->size()) storage_p->vec_p->resize((*storage_p->size_p));
  setSlice(0,(*storage_p->size_p));
  calcSizes();
  return ;
}

/*!
\brief Sets the dimensions of the array. Last index runs fastest, i.e., from left to right one goes from large to small chunks in memory.
 */
template <class T> void  hArray<T>::setDimensions5(HInteger dim0, HInteger dim1, HInteger dim2, HInteger dim3, HInteger dim4){
  if (storage_p==NULL) return ; //Check if vector was deleted elsewhere
  if (storage_p->vec_p==NULL) return ; //Check if vector was deleted elsewhere
  (*storage_p->ndims_p)=5;
  addHistory((HString)"setDimension",(HString)"Dimensions set to ["+hf2string(dim0)+","+hf2string(dim1)+","+hf2string(dim2)+","+hf2string(dim3)+","+hf2string(dim4)+"]");
  if ((*storage_p->dimensions_p).size()!=(uint)(*storage_p->ndims_p)) (*storage_p->dimensions_p).resize((*storage_p->ndims_p));
  (*storage_p->dimensions_p)[0]=dim0;
  (*storage_p->dimensions_p)[1]=dim1;
  (*storage_p->dimensions_p)[2]=dim2;
  (*storage_p->dimensions_p)[3]=dim3;
  (*storage_p->dimensions_p)[4]=dim4;
  (*storage_p->size_p)=hProduct<HInteger>((*storage_p->dimensions_p));
  if ((uint)(*storage_p->size_p) != storage_p->vec_p->size()) storage_p->vec_p->resize((*storage_p->size_p));
  setSlice(0,(*storage_p->size_p));
  calcSizes();
  return ;
}

/*!
\brief Sets begin and end the currently active slice using integer offsets from the begin of the stored vector. Use (0,size(vector)) to get the full vector
 */
template <class T> hArray<T> &  hArray<T>::setSlice(HInteger beg, HInteger end){
  if (storage_p==NULL) return *this; //Check if vector was deleted elsewhere
  if (storage_p->vec_p==NULL) return *this; //Check if vector was deleted elsewhere
  slice_begin=hfmax(beg,0);
  if (end==-1) end=storage_p->vec_p->size();
  if (end>=0) slice_end=std::min(end,(*storage_p->size_p));
  else slice_end=(*storage_p->size_p);
  slice_size=slice_end-slice_begin;
  return *this;
}

/*!  \brief Sets begin and end of the currently active slice sing an integer array of array indices.

If the length of the index vector is shorter than the actual number of
dimensions, this will provide a slice over the remanining dimensions
(belonging to the element indicated by the index vector)

 */
template <class T> hArray<T> &  hArray<T>::setSliceVector(vector<HInteger> & index_vector){
  if (storage_p==NULL) return *this; //Check if vector was deleted elsewhere
  if (storage_p->vec_p==NULL) return *this; //Check if vector was deleted elsewhere
  HInteger level=index_vector.size();
  if ((level > *storage_p->ndims_p) || (level < 0)) {ERROR("setSliceVector: Dimension wrong"); return *this;};
  if (subslice_level==-1) subslice_level=level;
  slice_begin=hfmax(0,std::min(hMulSum(index_vector,*storage_p->slice_sizes_p)+getSubSliceStart(),(HInteger)storage_p->vec_p->size()));
  slice_end=hfmax(0,std::min(hMulSum(index_vector,*storage_p->slice_sizes_p)+getSubSliceEnd(),(HInteger)storage_p->vec_p->size()));
  slice_size=slice_end-slice_begin;
  return *this;
}

//!Calculate the begin of the current subslice (the last slice) as an offset from the begin of the 2nd to last index. If *subslice_end_it == -1 then calculate from end
template <class T> HInteger hArray<T>::getSubSliceStart(){
  subslice_start=*subslice_start_it;
  //If negative (i.e. -N), then take full slice minus (N-1) number of elements
  if (subslice_start<0) subslice_start=storage_p->dimensions_p->at(subslice_level)+subslice_start;
  subslice_start *= (storage_p->slice_sizes_p->at(subslice_level));
  return subslice_start;
}

//!Calculate the end of the current subslice (the last slice) as an offset from the begin of the 2nd to last index. If *subslice_end_it == -1 then take full slice.
template <class T> HInteger hArray<T>::getSubSliceEnd(){
  subslice_end=*subslice_end_it;
  //If negative (i.e. -N), then take full slice minus (N-1) number of elements
  if (subslice_end<0) subslice_end=storage_p->dimensions_p->at(subslice_level)+subslice_end+1;
  subslice_end *= (storage_p->slice_sizes_p->at(subslice_level));
  return subslice_end;
}

/*
template <class T> hArray::storage_container * hArray<T>::getStorage(){
  return storage_p;
}
*/



//!Sets a second slice at the end (last index) of an already sliced array (e.g. used for array[0:2,...,0:4] -loops over first index and takes only the first 4 elements of each)
template <class T> hArray<T> &  hArray<T>::setSubSlice(HInteger start, HInteger end, HInteger level)
{
  subslice_vec_start.resize(1);
  subslice_vec_end.resize(1);
  subslice_start_it=subslice_vec_start.begin();
  subslice_end_it=subslice_vec_end.begin();
  *subslice_start_it=start;
  *subslice_end_it=end;
  subslice_level=level;
  return *this;
}

//!Sets a second slice at the end (last index) of an already sliced array (e.g. used for array[0:2,...,0:4] -loops over first index and takes only the first 4 elements of each). With each call of next the next slice is taken from the vectors.
template <class T> hArray<T> &  hArray<T>::setSubSliceVec(const vector<HInteger> start, const vector<HInteger> & end, const HInteger level){
  subslice_vec_start=start;
  subslice_vec_end=end;
  subslice_start_it=subslice_vec_start.begin();
  subslice_end_it=subslice_vec_end.begin();
  subslice_level=level;
  return *this;
}

/*!
\brief Returns the number of dimensions that have been associated with the current array
 */
template <class T> HInteger hArray<T>::getNumberOfDimensions(){
  if (storage_p==NULL) return 0; //Check if vector was deleted elsewhere
  return (*storage_p->ndims_p);
}

/*!
\brief Returns the begin iterator of the current slice in the stored vector
 */
template <class T> typename std::vector<T>::iterator hArray<T>::begin(){
  return storage_p->vec_p->begin()+getBegin();
}
/*!
\brief Returns the end iterator of the current slice in the stored vector
 */
template <class T> typename std::vector<T>::iterator hArray<T>::end(){
  return storage_p->vec_p->begin()+getEnd();
}

/*!
\brief Returns the offset of the current slice from the begin iterator of the stored vector
 */
template <class T> HInteger hArray<T>::getBegin(){
  if (doiterate) return std::min(loop_slice_begin+loop_nslice*loop_slice_size+getSubSliceStart(),loop_slice_end);
  else return slice_begin;
}

/*!
\brief Returns the offset of the end of the current slice from the begin iterator of the stored vector
 */
template <class T> HInteger hArray<T>::getEnd(){
  if (doiterate) return std::max(loop_slice_begin+loop_nslice*loop_slice_size+getSubSliceEnd(),loop_slice_begin);
  else return slice_end;
}

/*!
\brief Returns the size (length) of the current slice
 */
template <class T> HInteger hArray<T>::getSize(){
  if (doiterate) return loop_slice_size;
  else return slice_size;
}

/*!
\brief Returns the length of the underlying vector
 */
template <class T> HInteger hArray<T>::length(){
  if (storage_p==NULL) return 0; //Check if vector was deleted elsewhere
  if (storage_p->vec_p==NULL) return 0; //Check if vector was deleted elsewhere
  return storage_p->vec_p->size();
}


/*!  \brief Resizes the vector to the length given as input. If the
new size differs, then the vector will be resized and slices and
dimensions be reset to a flat (one dimensional) vector array.
 */
template <class T> hArray<T> & hArray<T>::resize(HInteger newsize){
  if (storage_p==NULL) return *this; //Check if vector was deleted elsewhere
  if (storage_p->vec_p==NULL) return *this; //Check if vector was deleted elsewhere
  if (newsize==(HInteger)storage_p->vec_p->size()) return *this;
  storage_p->vec_p->resize(newsize);
  *storage_p->size_p=newsize;
  storage_p->dimensions_p->resize(1);
  storage_p->dimensions_p->at(0)=newsize;
  setSlice(0,newsize);
  calcSizes();
  return *this;
}

/*!
\brief Returns whether or not to iterate over all slices in the vector
 */
template <class T> bool hArray<T>::doLoopAgain(){return doiterate && loop_next;}

/*!
\brief Returns whether or not the  iterate over all slices in the vector
 */
template <class T> bool hArray<T>::loopingMode(){return doiterate;}

/*!  \brief Returns the current loop index (which may be different
from loop_nslice if one loops over an array of indices)
 */
template <class T> HInteger hArray<T>::getLoop_i(){return loop_i;}

/*!
\brief Returns the current loop start
 */
template <class T> HInteger hArray<T>::getLoop_start(){return loop_start;}

/*!
\brief Returns the current loop end
 */
template <class T> HInteger hArray<T>::getLoop_end(){return loop_end;}

/*!
\brief Returns the current loop index increment
 */
template <class T> HInteger hArray<T>::getLoop_increment(){return loop_increment;}

/*!  \brief Returns the number of the currently used slice in the loop
(which may be different from loop_i, if one loops over an array of
indices)
 */
template <class T> HInteger hArray<T>::getLoop_nslice(){return loop_nslice;}

/*!
\brief Sets the array to looping mode (i.e. the next function will loop over all slices in the vector).

 */
template <class T> hArray<T> & hArray<T>::loopOn(){
  loop_next=true;
  doiterate=true;
  return *this;
}

/*!
\brief Sets the array to looping mode (i.e. the next function will loop over all slices in the vector).

 */
template <class T> hArray<T> & hArray<T>::all(){setSlice(0,-1); return *this;}

/*!
\brief Sets the array to looping mode (i.e. the next function will loop over all slices in the vector).

\param "start_element_index" indicates the array element to start with. Looping
will be done over the next higher dimension.

\param start (starting at 0), end, increment indicate over how
many slices the iteration should proceed.

 */
template <class T> hArray<T> &   hArray<T>::loop(vector<HInteger> & start_element_index, HInteger start, HInteger end, HInteger increment){
  if (storage_p==NULL) return *this; //Check if vector was deleted elsewhere
  if (storage_p->vec_p==NULL) return *this; //Check if vector was deleted elsewhere
  HInteger level=setLoopSlice(start_element_index);
  if ( level <= -1 )  {ERROR("loop: dimensions are wrong!"); return *this;};
  if (end<0) end=storage_p->dimensions_p->at(level);
  start=std::min(hfmax(0,start),loop_maxn);
  end=std::min(hfmax(start,end),loop_maxn);
  increment = hfmax(increment,1);
  loopOn();
  loop_over_indexvector=false;
  loop_i=start; loop_start=start; loop_end=end; loop_increment=increment;
  loop_slice_start_offset=*subslice_start_it; loop_slice_end_offset=*subslice_end_it;
  loop_nslice = loop_i;
  return *this;
}

/*!

 \brief Sets the array to looping mode (i.e. the next function will
loop over all slices in the vector) using and index vector to indicate
over which slices to loop.

 */
template <class T> hArray<T> & hArray<T>::loopVector(vector<HInteger> & start_element_index, vector<HInteger> & vec){
  if (storage_p==NULL) return *this; //Check if vector was deleted elsewhere
  if (storage_p->vec_p==NULL) return *this; //Check if vector was deleted elsewhere
  HInteger level=setLoopSlice(start_element_index);
  if ( level <= -1 ) {ERROR("loopVector: dimensions are wrong!"); return *this;};
  loopOn();
  loop_over_indexvector=true;
  loop_i=0; loop_start=0; loop_end=vec.size(); loop_increment=1;
  loop_slice_start_offset=*subslice_start_it; loop_slice_end_offset=*subslice_end_it;
  index_vector=vec;
  loop_nslice=index_vector[loop_i];
  return *this;
}

/*!
\brief Sets the array to looping mode and resets the loop parameters to its start values

 */
template <class T> hArray<T> & hArray<T>::resetLoop(){
  loop_i=loop_start;
  subslice_start_it=subslice_vec_start.begin();
  subslice_end_it=subslice_vec_end.begin();
  if (loop_over_indexvector) loop_nslice=index_vector[loop_i];
  else loop_nslice = loop_i;
  loopOn();
  return *this;
}


/*!
\brief Sets the slice parameters used by the looping algorithm to calculate the currently worked on slice.

\param start_element_index: a vector of n indices which remain
constant during the looping. Looping will be done over the npipil+1st
index.

 */
template <class T> HInteger hArray<T>::setLoopSlice(vector<HInteger> & start_element_index){
  if (storage_p==NULL) return -1; //Check if vector was deleted elsewhere
  if (storage_p->vec_p==NULL) return -1; //Check if vector was deleted elsewhere
  HInteger level=start_element_index.size();
  if (level >= *storage_p->ndims_p)  {ERROR("setLoopSlice: dimensions are wrong!"); return -1;};
  loop_maxn = storage_p->dimensions_p->at(level);
  loop_slice_begin=hMulSum(start_element_index,*storage_p->slice_sizes_p); // multiplies the start element indices with the (cummulaitve) sizes of slices per dimension - giving the total offset
  loop_slice_size=storage_p->slice_sizes_p->at(level);
  if (level+1 >= *storage_p->ndims_p) loop_lower_level_size=0;
  else loop_lower_level_size=storage_p->slice_sizes_p->at(level+1);
  loop_slice_end=loop_slice_begin+loop_slice_size*loop_maxn;
  return level;
}

/*!
\brief Switches the looping mode off (i.e. the next function will not loop over all slices in the vector)
 */
template <class T> hArray<T> & hArray<T>::loopOff(){doiterate=false; return *this;}

/*!
\brief Increase the current slice by one, if array is in looping mode.

If the end of the vector is reached, switch looping mode off and reset array to first slice.
 */
template <class T> hArray<T> &  hArray<T>::next(){
  if (!doiterate) return *this;
  if (loop_next==false) loop_next=true;  // start all over again
  loop_i+=loop_increment;
  subslice_start_it+=loop_increment; if (subslice_start_it>=subslice_vec_start.end()) subslice_start_it=subslice_vec_start.begin();
  subslice_end_it+=loop_increment; if (subslice_end_it>=subslice_vec_end.end()) subslice_end_it=subslice_vec_end.begin();
  if (loop_i>=loop_end) { // the end is near, stop looping ...
    resetLoop();
    loop_next=false;
  } else {
    if (loop_over_indexvector) loop_nslice=index_vector[loop_i];
    else loop_nslice = loop_i;
    loop_nslice = std::min(loop_nslice,loop_maxn);
  };
  return *this;
}

/*!
\brief Get the unit name and prefix of the data array
 */
template <class T> HString hArray<T>::getUnit(){
  if (storage_p==NULL) return ""; //Check if vector was deleted elsewhere
  if (storage_p->vec_p==NULL) return ""; //Check if vector was deleted elsewhere
  return storage_p->unit.prefix + storage_p->unit.name;
};

/*!
\brief Get the unit name of the data array
 */
template <class T> HString hArray<T>::getUnitName(){
  if (storage_p==NULL) return ""; //Check if vector was deleted elsewhere
  if (storage_p->vec_p==NULL) return ""; //Check if vector was deleted elsewhere
  return storage_p->unit.name;
};

/*!
\brief Get the unit prefix of the data array
 */
template <class T> HString hArray<T>::getUnitPrefix(){
  if (storage_p==NULL) return ""; //Check if vector was deleted elsewhere
  if (storage_p->vec_p==NULL) return ""; //Check if vector was deleted elsewhere
  return storage_p->unit.prefix;
};

/*!
\brief Take out the unit information
 */
template <class T> hArray<T> &  hArray<T>::clearUnit(){
  if (storage_p==NULL) return *this; //Check if vector was deleted elsewhere
  if (storage_p->vec_p==NULL) return *this; //Check if vector was deleted elsewhere
  setUnit("","");
  storage_p->unit.name="";
  addHistory((HString)"clearUnit",(HString)"");
  return *this;
}


/*!

\brief Set the value of a particular key word.

Inputs are always given as strings (also numerical values). As a
convention, the contents should be formatted as python input style.

 */
template <class T> hArray<T> &  hArray<T>::setKey(HString key, HString contents){
  if (storage_p==NULL) return *this; //Check if vector was deleted elsewhere
  storage_p->keywords[key]=contents;
  addHistory("setKey",key + " = " + contents);
  return *this;
}

/*!

\brief Set the value of a particular key word.

Inputs are always given as strings (also numerical values). As a
convention, the contents should be formatted as python input style.

 */
template <class T> HString hArray<T>::getKey(HString key){
  if (storage_p==NULL) return ""; //Check if vector was deleted elsewhere
  map<HString,HString>::iterator it=storage_p->keywords.find(key);
  if (it!=storage_p->keywords.end()) return it->second;
  return "";
}

/*!

\brief Set the value of a particular key word as a Python object.


template <class T> hArray<T> &  hArray<T>::setKeyPy(HString key, HPyObjectPtr pyobj){
  if (storage_p==NULL) return *this; //Check if vector was deleted elsewhere
  storage_p->keywords_py[key]=pyobj;
  addHistory("setKey",key + " = ..." );
  return *this;
}
 */

/*!

\brief Set the value of a particular key word from a python object.

template <class T> HPyObject & hArray<T>::getKeyPy(HString key){
  if (storage_p==NULL) return HPyObject(false); //Check if vector was deleted elsewhere
  map<HString,HPyObjectPtr>::iterator it=storage_p->keywords_py.find(key);
  if (it!=storage_p->keywords_py.end()) return *(it->second);
  return HPyObject(false);
}
 */


/*!
\brief Set the unit of the data array and multiply all data with the
apropriate scale factor. Prefix is the
 */
template <class T> hArray<T> &  hArray<T>::setUnit(HString prefix, HString name){
  if (storage_p==NULL) return *this; //Check if vector was deleted elsewhere
  if (storage_p->vec_p==NULL) return *this; //Check if vector was deleted elsewhere
  map<HString,HNumber>::iterator factor_p=storage_p->unit.prefix_to_factor.find(prefix);
  if (factor_p==storage_p->unit.prefix_to_factor.end()) {
    ERROR("setUnit: unit prefix " << prefix << " unknown.");
    return *this;
  };
  HNumber factor = factor_p->second;
  if ((name=="") || (storage_p->unit.name==name)) {
    if (typeid(T)==typeid(HInteger)) {
      hMul(*reinterpret_cast<vector<HInteger>*>(storage_p->vec_p),storage_p->unit.scale_factor/factor);
    } else if (typeid(T)==typeid(HNumber)) {
      hMul(*reinterpret_cast<vector<HNumber>*>(storage_p->vec_p),storage_p->unit.scale_factor/factor);
    } else if (typeid(T)==typeid(HComplex)) {
      hMul(*reinterpret_cast<vector<HComplex>*>(storage_p->vec_p),storage_p->unit.scale_factor/factor);
    } else {
      ERROR("setUnit: unit setting not available for type  " << typeid(T).name() << ".");
    }
  } else storage_p->unit.name=name;
  storage_p->unit.scale_factor=factor;
  storage_p->unit.prefix=prefix;
  addHistory((HString)"setUnit",(HString)"Unit set to " + getUnit()+".");
  return *this;
}

/*!
\brief Add a line of history information to the history vector
 */
template <class T> hArray<T> & hArray<T>::addHistory(HString name, HString text){
  if (storage_p==NULL) return *this; //Check if vector was deleted elsewhere
  if (storage_p->vec_p==NULL) return *this; //Check if vector was deleted elsewhere
  if (!storage_p->trackHistory) return *this;
  time_t rawtime;
  struct tm * timeinfo;
  char buffer [20];

  time ( &rawtime );
  timeinfo = localtime ( &rawtime );

  strftime (buffer,80,"%Y-%m-%d %H:%M:%S",timeinfo);
  HString out(buffer);
  out += " [" + name + "]: " + text;
  storage_p->history.push_back(out);
  return *this;
}

/*!
\brief Return the history information
 */
template <class T> vector<HString> & hArray<T>::getHistory(){
  if (storage_p==NULL) {static vector<HString> h; return h;}; //Check if vector was deleted elsewhere
  if (storage_p->vec_p==NULL) {static vector<HString> h; return h;}; //Check if vector was deleted elsewhere
  if (!storage_p->trackHistory) {static vector<HString> h; return h;};
  return storage_p->history;
}

/*!
\brief Print the history information
 */
template <class T> void hArray<T>::printHistory(){
  if (storage_p==NULL)  return; //Check if vector was deleted elsewhere
  if (storage_p->vec_p==NULL)  return; //Check if vector was deleted elsewhere
  if (!storage_p->trackHistory) return;
  vector<HString>::iterator it=storage_p->history.begin();
  while (it!=storage_p->history.end()) {
    cout << "  " << *it << endl;
    ++it;
  };
  return;
}

/*!
\brief Clear the history information
 */
template <class T> hArray<T> &  hArray<T>::clearHistory(){
  if (storage_p==NULL) return *this; //Check if vector was deleted elsewhere
  if (storage_p->vec_p==NULL) return *this; //Check if vector was deleted elsewhere
  storage_p->history.clear();
  return *this;
}

/*!
\brief Set history tracking on or off
 */
template <class T> hArray<T> &  hArray<T>::setHistory(bool on_or_off){
  if (storage_p==NULL) return *this; //Check if vector was deleted elsewhere
  if (storage_p->vec_p==NULL) return *this; //Check if vector was deleted elsewhere
  if (!on_or_off) addHistory((HString)"setHistory",(HString)"History tracking switched off.");
  storage_p->trackHistory=on_or_off;
  if (on_or_off) addHistory((HString)"setHistory",(HString)"History tracking switched on.");

  return *this;
}


/*!
\brief Check if history tracking is on or off
 */
template <class T> bool hArray<T>::isTrackingHistory(){
  if (storage_p==NULL) return false; //Check if vector was deleted elsewhere
  return(storage_p->trackHistory);
}

void hArray_trackHistory(HBool on);

// ========================================================================
//
//  Generic templates for wrapper functionality
//
// ========================================================================

namespace PyCR { // Namespace PyCR -- begin

  namespace Array { // Namespace Array -- begin

    template <class Iter> IterValueType
      hProduct (const Iter vec,const Iter vec_end)
      {
        typedef IterValueType T;
        T prod=1.0;
        Iter it=vec;
        while (it!=vec_end) {prod *= *it; ++it;};
        return prod;
      }

    template <class Iter>
      HNumber hVectorLength (const Iter vec, const Iter vec_end)
      {
        HNumber sum=0;
        Iter it=vec;
        while (it!=vec_end) {sum += (*it) * (*it); ++it;};
        return sqrt(sum);
      }

    template <class Iter>
      void hNormalize (const Iter vec, const Iter vec_end)
      {
        HNumber norm=hVectorLength(vec,vec_end);
        Iter it=vec;
        while (it!=vec_end) {*it=(*it)/norm; ++it;};
      }

    template <class Iter>
      IterValueType hMulSum (const Iter vec1,const Iter vec1_end,const Iter vec2,const Iter vec2_end)
      {
        typedef IterValueType T;
        T sum=hfnull<T>();
        Iter it1=vec1,it2=vec2;
        while (it1!=vec1_end && it2!=vec2_end) {
          sum+=(*it1) * (*it2);
          ++it1; ++it2;
        };
        return sum;
      }

  } // Namespace Array -- end

} // Namespace PyCR -- end


#define HFPP_hARRAY_BOOST_PYTHON_WRAPPER(TYPE,NAME)	\
    class_<hArray<TYPE> >(#NAME) \
    .def("getVector",&hArray<TYPE>::getVector,return_internal_reference<>()) \
    .def("shared_copy",&hArray<TYPE>::shared_copy,return_internal_reference<>()) \
    .def("getDimensions",&hArray<TYPE>::getDimensions,return_internal_reference<>()) \
    .def("getSizes",&hArray<TYPE>::getSizes,return_internal_reference<>()) \
    .def("setVector",&hArray<TYPE>::setVector,return_internal_reference<>())\
    .def("setDimensions",&hArray<TYPE>::setDimensions1)			\
    .def("setDimensions",&hArray<TYPE>::setDimensions2)			\
    .def("setDimensions",&hArray<TYPE>::setDimensions3)			\
    .def("setDimensions",&hArray<TYPE>::setDimensions4)			\
    .def("setSlice",&hArray<TYPE>::setSlice,return_internal_reference<>())				\
    .def("setSliceVector",&hArray<TYPE>::setSliceVector,return_internal_reference<>())				\
    .def("setSubSlice",&hArray<TYPE>::setSubSlice,return_internal_reference<>())				\
    .def("setSubSlice",&hArray<TYPE>::setSubSliceVec,return_internal_reference<>())				\
    .def("getNumberOfDimensions",&hArray<TYPE>::getNumberOfDimensions)	\
    .def("getBegin",&hArray<TYPE>::getBegin)				\
    .def("getEnd",&hArray<TYPE>::getEnd)				\
    .def("getSize",&hArray<TYPE>::getSize)				\
    .def("loopingMode",&hArray<TYPE>::loopingMode)			\
    .def("doLoopAgain",&hArray<TYPE>::doLoopAgain)			\
    .def("loop_i",&hArray<TYPE>::getLoop_i)				\
    .def("loop_nslice",&hArray<TYPE>::getLoop_nslice)			\
    .def("loop_start",&hArray<TYPE>::getLoop_start)			\
    .def("loop_end",&hArray<TYPE>::getLoop_end)				\
    .def("loop_increment",&hArray<TYPE>::getLoop_increment)		\
    .def("__len__",&hArray<TYPE>::length)				\
    .def("resize",&hArray<TYPE>::resize,return_internal_reference<>())					\
    .def("loop",&hArray<TYPE>::loop,return_internal_reference<>())					\
    .def("loop",&hArray<TYPE>::loopVector,return_internal_reference<>())					\
    .def("resetLoop",&hArray<TYPE>::resetLoop,return_internal_reference<>())				\
    .def("noOn",&hArray<TYPE>::loopOn,return_internal_reference<>())				\
    .def("noOff",&hArray<TYPE>::loopOff,return_internal_reference<>())				\
    .def("next",&hArray<TYPE>::next,return_internal_reference<>())       \
    .def("setUnit_",&hArray<TYPE>::setUnit,return_internal_reference<>())	\
    .def("getUnit",&hArray<TYPE>::getUnit)	\
    .def("getUnitName",&hArray<TYPE>::getUnitName)	\
    .def("getUnitPrefix",&hArray<TYPE>::getUnitPrefix)	\
    .def("clearUnit",&hArray<TYPE>::clearUnit,return_internal_reference<>())	\
    .def("addHistory",&hArray<TYPE>::addHistory,return_internal_reference<>())	\
    .def("clearHistory",&hArray<TYPE>::clearHistory,return_internal_reference<>())	\
    .def("setHistory",&hArray<TYPE>::setHistory,return_internal_reference<>())	\
    .def("getHistory",&hArray<TYPE>::getHistory,return_internal_reference<>())	\
    .def("isTrackingHistory",&hArray<TYPE>::isTrackingHistory)	\
    .def("history",&hArray<TYPE>::printHistory)	\
    .def("setKey",&hArray<TYPE>::setKey,return_internal_reference<>())	\
    .def("getKey",&hArray<TYPE>::getKey)	\


#undef HFPP_FILETYPE

#endif /* CR_PIPELINE_ARRAY_H */
