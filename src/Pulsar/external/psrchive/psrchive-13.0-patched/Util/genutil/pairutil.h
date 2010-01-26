/***************************************************************************
 *
 *   Copyright (C) 2006-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include <utility>
#include <iostream>

namespace std
{
  template<typename T>
  class numeric_limits< std::pair<T,T> >
  {
    public:
    static const int digits10 = numeric_limits<T>::digits10;
  };
}

template<class T, class U>
std::ostream& operator << (std::ostream& os, std::pair<T,U> vals)
{
  return os << "(" << vals.first << "," << vals.second << ")";
}

template<class T, class U>
std::istream& operator >> (std::istream& is, std::pair<T,U>& vals)
{
  bool bracketed = is.peek() == '(';

  if (bracketed)
    is.get();

  char separator = 0;

  is >> vals.first >> separator >> vals.second;

  if ( (bracketed && separator != ',') || (!bracketed && separator != ':') )
  {
    is.setstate (std::istream::failbit);
    return is;
  }

  if (bracketed)
  {
    if (is.peek() != ')')
      is.setstate (std::istream::failbit);
    else
      is.get();
  }

  return is;
}

// Stretch min and max to normalized range
template<typename T>
void stretch (const std::pair<T,T>& norm, T& min, T& max)
{
  T diff = max - min;
  max = min + norm.second * diff;
  min = min + norm.first * diff;
}

// Stretch range to normalized range
template<typename T>
void stretch (const std::pair<T,T>& norm, std::pair<T,T>& range)
{
  stretch (norm, range.first, range.second);
}

template<typename T, typename U>
bool compair1 (const std::pair<T,U>& a, const std::pair<T,U>& b)
{
  return a.first < b.first;
}

template<typename T, typename U>
bool compair2 (const std::pair<T,U>& a, const std::pair<T,U>& b)
{
  return a.second < b.second;
}

