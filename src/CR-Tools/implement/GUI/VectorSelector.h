#ifndef VECTORSELECTOR_H
#define VECTORSELECTOR_H

#include <GUI/hfcast.h>

enum SELECTORTYPE {
  SEL_NONE,
  SEL_ALL,
  SEL_ONE,
  SEL_BLOCK,
  SEL_RANGE,
  SEL_LIST,
  SEL_ABOVE,
  SEL_BELOW,
  SEL_BETWEEN,
  SEL_QUERY
};

//========================================================================
// Vector_Selector
//========================================================================

/*!
  \class Vector_Selector

  \ingroup CR_GUI
*/


const bool noSelection(const Vector_Selector *vs);
const bool isSelection(const Vector_Selector *vs);

template <class T> 
void select_vector_elements(vector<T>* ip, vector<T>* op, vector<address>* idx);

class Vector_Selector {

 public:
  
  template <class T>
    vector<T> get(const vector<T> &v);

template <class T>
  void select(vector<T>* vp);

  template <class T>
    vector<address>* getList(const vector<T>* v);
  
  void setRange(address start, address end, address inc=1);

  void setAll();
  void setAbove(HNumber x);
  void setBelow(HNumber x);
  void setBetween(HNumber x, HNumber y);

  void setList(const vector<address> &vptr);
  void setOne(address start);

  bool isIndexable() const;
  SELECTORTYPE SelectorType() const;

  template <class T>
    T getOne(const vector<T> &v);

  Vector_Selector();
  ~Vector_Selector();

private:  
  template <class T>
  void calcAllList(const vector<T> &v);
  template <class T>
  void calcRangeList(const vector<T> &v);
  template <class T>
  void calcAboveList(const vector<T> &v);
  template <class T>
  void calcBelowList(const vector<T> &v);
  template <class T>
  void calcBetweenList(const vector<T> &v);

  struct sel_range {address start,end,inc;} s_range;

  //This needs to be made generic, so that all types can be dealt with.
  struct sel_limit {HNumber above, below;} s_limit;

  bool isindex;

  vector<address> s_list;
  SELECTORTYPE type;
};

//End Vector Selector
//========================================================================


#endif
