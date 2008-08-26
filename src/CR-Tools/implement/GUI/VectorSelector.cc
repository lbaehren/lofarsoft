#include <boost/python/class.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>

#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/implicit.hpp>
#include <boost/python/enum.hpp>
#include <boost/python/overloads.hpp>
#include <boost/python/args.hpp>
#include <boost/python/tuple.hpp>
#include <boost/python/class.hpp>
#include <boost/python/return_internal_reference.hpp>
#include <boost/python/operators.hpp>
#include <boost/python/object_operators.hpp>

using namespace std;

#include <GUI/hfdefs.h>
#include <GUI/hfcast.h>
#include <GUI/VectorSelector.h>

//------------------------------------------------------------------------
// Vector_Selector
//------------------------------------------------------------------------

Vector_Selector::Vector_Selector(){type=SEL_ALL;}
Vector_Selector::~Vector_Selector(){}

SELECTORTYPE Vector_Selector::SelectorType(){
  return type;
}

void Vector_Selector::setAbove(Number x){
  type=SEL_ABOVE;
  s_limit.above=x;
}

void Vector_Selector::setBelow(Number x){
  type=SEL_BELOW;
  s_limit.below=x;
}

void Vector_Selector::setBetween(Number x, Number y){
  type=SEL_BETWEEN;
  s_limit.above=x;
  s_limit.below=y;
}

void Vector_Selector::setRange(address start, address end, address inc) {
  type=SEL_RANGE;
  s_range.start=start; s_range.end=end; s_range.inc=inc; 
}

void Vector_Selector::setAll() {
  type=SEL_ALL;
}

void Vector_Selector::setList(const vector<address> &vptr){
  type=SEL_LIST;
  s_list = vptr;
}

void Vector_Selector::setOne(address i) {
  type=SEL_ONE;
  s_range.start=i; s_range.end=i; s_range.inc=1; 
}

template <class T>
T Vector_Selector::getOne(const vector<T> &v){
  if (s_range.start < v.size) {return v[s_range.start];}
  else {return 0;};
}

template <class T>
vector<T> Vector_Selector::get(const vector<T> &v){
  address i;
  vector<T> o;
  switch (type) {
  case SEL_ALL:
    return v;
  case SEL_ONE:
    if (s_range.start < v.size()) {o.push_back(v[s_range.start]);};
    return o;
  default:
    getList(v);
    address s1,s2;
    s1=s_list.size();
    s2=v.size();
    if (o.capacity()<s1) {o.reserve(s1);};
    for (i=0;i<s1;i++){if (s_list[i]<s2) {o.push_back(v[s_list[i]]);};};
    return o;
  };
}

template <class T>
vector<address>* Vector_Selector::getList(const vector<T> &v){
  address i;
  switch (type) {
  case SEL_ONE:  //this makes use of the fall-through property of switch ...
  case SEL_RANGE:
    calcRangeList(v);
    return &s_list;
  case SEL_ABOVE:
    calcAboveList(v);
    return &s_list;
  case SEL_BELOW:
    calcBelowList(v);
    return &s_list;
  case SEL_BETWEEN:
    calcBetweenList(v);
    return &s_list;
  case SEL_LIST:
    return &s_list;
  case SEL_ALL:  //uses fall-through
  default:
    type=SEL_ALL;
    calcAllList(v);
    return &s_list;
  };
}

template <class T>
void Vector_Selector::calcAllList(const vector<T> &v){
  address i,size;
  type=SEL_LIST;
  size=v.size();
  s_list.clear(); if (s_list.capacity()<size) {s_list.reserve(size);};
  for (i=0;i<size;i++){s_list.push_back(i);};
}

template <class T>
void Vector_Selector::calcRangeList(const vector<T> &v){
  address i,size;
  type=SEL_LIST;
  size=(s_range.end-s_range.start)/s_range.inc+1;
  s_list.clear(); if (s_list.capacity()<size) {s_list.reserve(size);};
  for (i=s_range.start;i<=s_range.end;i=i+s_range.inc){s_list.push_back(i);};
}

template <class T>
void Vector_Selector::calcAboveList(const vector<T> &v){
  address i,size;
  type=SEL_LIST;
  size=v.size();
  s_list.clear(); if (s_list.capacity()<size) {s_list.reserve(size);};
  for (i=0;i<size;i++){if (mycast<Number>(v[i])>s_limit.above) {s_list.push_back(i);};
  }
}

template <class T>
void Vector_Selector::calcBelowList(const vector<T> &v){
  address i,size;
  type=SEL_LIST;
  size=v.size();
  s_list.clear(); if (s_list.capacity()<size) {s_list.reserve(size);};
  for (i=0;i<size;i++){if (mycast<Number>(v[i])<s_limit.below) {s_list.push_back(i);};
  }
}

template <class T>
void Vector_Selector::calcBetweenList(const vector<T> &v){
  address i,size;
  type=SEL_LIST;
  size=v.size();
  s_list.clear(); if (s_list.capacity()<size) {s_list.reserve(size);};
  for (i=0;i<size;i++){if (mycast<Number>(v[i])>s_limit.above && mycast<Number>(v[i])<s_limit.below) {s_list.push_back(i);};
  }
}

//------------------------------------------------------------------------
//End class Vector_Selector 
//------------------------------------------------------------------------

//Make sure all instances are created, never call this function ...
void instantiate_VectorSelector(DATATYPE type){
  
  Vector_Selector vs;
  DEF_D_PTR(Integer);
  DEF_VAL(Integer);

  switch (type) {

#define SW_TYPE_COMM(EXT,TYPE)  \
 vs.get(*d_ptr_##EXT);\
 vs.getList(*d_ptr_##EXT)
#include "switch-type.cc"      
    }
}
