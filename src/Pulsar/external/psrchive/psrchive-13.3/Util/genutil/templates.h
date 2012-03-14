/***************************************************************************
 *
 *   Copyright (C) 2000 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef _Util_genutil_templates_h
#define _Util_genutil_templates_h

#include <algorithm>
#include <iterator>
#include <vector>

#include <assert.h>

//! For each element of container C, call method M
template<typename C, typename M>
void foreach (C& container, M method)
{
  std::for_each (container.begin(), container.end(), 
		 std::mem_fun(method));
}

//! For each element of container C, call method M with argument A
template<typename C, typename M, typename A>
void foreach (C& container, M method, const A& a)
{
  std::for_each (container.begin(), container.end(),
		 std::bind2nd( std::mem_fun(method), a ));
}

template <class T>
void scrunch (std::vector<T>& vals, unsigned factor, bool mean = true)
{
  typename std::vector<T>::iterator into = vals.begin();
  typename std::vector<T>::iterator val;

  unsigned fi = 0;

  for (val = vals.begin(); val != vals.end(); into++) {
    *into = *val; val++;
    for (fi=1; fi<factor && val != vals.end(); (val++, fi++))
      *into += *val;
    if (mean)
      *into /= fi;
  }
  vals.resize (vals.size()/factor);
}

//! inplace shift
template<typename T>
void shift (unsigned size, unsigned shift, T* array)
{
  T* temp = new T[shift];
  memcpy (temp, array, shift*sizeof(T));
  memmove (array, array+shift, (size-shift)*sizeof(T));
  memcpy (array+size-shift, temp, shift*sizeof(T));
  delete [] temp;
}

//! outofplace shift
template<typename T>
void shift (unsigned size, unsigned shift, T* result, const T* input)
{
  memcpy (result, input+shift, (size-shift)*sizeof(T));
  memcpy (result+size-shift, input, shift*sizeof(T));
}

// returns the mean "bin" of a histogram
template <class T>
T histomean (const std::vector<T>& vals)
{
  T valcount = 0.0;
  T totcount = 0.0;

  T total = vals.size();
  T bin = 0.0;

  typename std::vector<T>::const_iterator val;

  for (val = vals.begin(); val != vals.end(); val++) {
    valcount += *val * bin;
    totcount += *val * total;

    bin += 1.0;
  }
  return valcount/totcount;
}

template <class T>
T sqr (const T& x)
{
  return x * x;
}

//! Return the sum of all values on [i1, i2)
/*! Use this version when you want a different return type than
  iterator value_type */
template <class I, class T>
T sum (const I& it1, const I& it2, T& the_sum)
{
  the_sum = 0.0;

  for (I it=it1; it != it2; it++)
    the_sum += T(*it);

  return the_sum;
}

//! Return the sum of all values on [i1, i2)
template <class I> typename std::iterator_traits<I>::value_type
sum (const I& it1, const I& it2)
{
  typename std::iterator_traits<I>::value_type t;
  return sum (it1, it2, t);
}

// return the sum of all elements in an STL container
template <class C> typename C::value_type
sum (const C& x)
{
  return sum (x.begin(), x.end());
}

//! Return the mean of all values on [i1, i2)
/*! Use this version when you want a different return type than
  iterator value_type */
template <class I, class T> T
mean (const I& it1, const I& it2, T& the_mean)
{
  the_mean = sum(it1, it2, the_mean) / (it2 - it1);
  return the_mean;
}

//! Return the mean of all values on [i1, i2)
template <class I> typename std::iterator_traits<I>::value_type
mean (const I& it1, const I& it2)
{
  typename std::iterator_traits<I>::value_type t;
  return mean (it1, it2, t);
}

// return the mean of all elements in an STL container
template <class C> typename C::value_type
mean (const C& x)
{
  return mean (x.begin(), x.end());
}

//! Return the variance of all values on [i1, i2)
/*! Use this version when you want a different return type than
  iterator value_type */
template <class I, class T> T
variance (const I& it1, const I& it2, T& the_variance)
{
  T the_mean = mean (it1, it2, the_mean);
  the_variance = 0.0;
  
  for (I it=it1; it != it2; it++)
    the_variance += sqr( T(*it) - the_mean );

  the_variance /= (it2 - it1 - 1);

  return the_variance;
}

//! Return the variance of all values on [i1, i2)
template <class I> typename std::iterator_traits<I>::value_type 
variance (const I& it1, const I& it2)
{
  typename std::iterator_traits<I>::value_type t;
  return variance (it1, it2, t);
}

// return the variance of all elements in an STL container
template <class C> typename C::value_type 
variance (const C& x)
{
  return variance (x.begin(), x.end());
}

// normalize each element of a vector by the sum of all elements in it
template <class T>
void normalize (std::vector<T>& x)
{
  T the_sum = sum (x);
  assert( the_sum != 0 );
  for (unsigned i=0; i<x.size(); i++)
    x[i] /= the_sum;
}

//! Returns the maximum and minimum values on [i1, i2)
template <class T, class I>
void minmax (const I& it1, const I& it2, T& min, T& max, bool follow = false)
{
  if (!follow)
    max = min = *it1;

  for (I it=it1; it != it2; it++) {
    if (*it > max)
      max = *it;
    if (*it < min)
      min = *it;
  }
}

//! Returns the maximum and minimum values found in container c
template <class T, class C>
void minmax (const C& c, T& min, T& max, bool follow = false)
{
  minmax (c.begin(), c.end(), min, max, follow);
}

//! Returns the maximum and minimum values found in container c
template <class T, class C>
  void cyclic_minmax (const C& c, unsigned i1, unsigned i2, T& min, T& max,
		      bool follow = false)
{
  if (i2 < c.size())
    minmax (c.begin()+i1, c.begin()+i2, min, max, follow);
  else {
    i2 -= c.size();
    minmax (c.begin()+i1, c.end(), min, max, follow);
    minmax (c.begin(), c.begin()+i2, min, max, true);
  }
}

//! Returns true if element x is inside container c
template <class T, class C>
bool found (const T& x, const C& c)
{
  return std::find (c.begin(), c.end(), x) != c.end();
}

//! Returns the index of element x inside container c
template<class T, class C>
int index (const T& x, const C& c)
{
  typename C::const_iterator f = std::find (c.begin(), c.end(), x);
  if (f == c.end())
    return -1;

  return f - c.begin();
}

//! Removes the first instance of element x from container c
template<class T, class C>
bool remove (const T& x, C& c)
{
  typename C::iterator f = std::find (c.begin(), c.end(), x);
  if (f != c.end()) {
    c.erase(f);
    return true;
  }
  return false;
}

//! Removes an inclusive range of elements from a container
/*! Note that STL ranges are not inclusive:

"This example uses a range, just like the example of reversing a
vector: the first argument to reverse is a pointer to the beginning of
the range, and the second argument points one element past the end of
the range. This range is denoted [A, A + 6); the asymmetrical notation
is a reminder that the two endpoints are different, that the first is
the beginning of the range and the second is one past the end of the
range."

*/
template<typename C>
void remove (C& c, unsigned first, unsigned last)
{
  c.erase (c.begin()+first, c.begin()+last+1);
}

//! Return the number nearest to and larger than big and divisible by small
template<typename Big, typename Small>
inline Big multiple_greater (Big big, Small small)
{
  Big divides = big / small;
  if (big % small)
    divides ++;

  return divides * small;
}

#endif

