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
class Vector_Selector {

 public:
  
  template <class T>
    vector<T> get(const vector<T> &v);

  template <class T>
    vector<address>* getList(const vector<T> &v);
  
  void setRange(address start, address end, address inc=1);

  void setAll();
  void setAbove(Number x);
  void setBelow(Number x);
  void setBetween(Number x, Number y);

  void setList(const vector<address> &vptr);
  void setOne(address start);

  SELECTORTYPE SelectorType();

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
  struct sel_limit {Number above, below;} s_limit;

  vector<address> s_list;
  SELECTORTYPE type;
};

//End Vector Selector
//========================================================================


#endif
