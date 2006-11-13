#ifndef MASTDFILTER_H
#define MASTDFILTER_H
/*******************************************************************************
* E.S.O. - VLT project
*
* "@(#) $Id: MAStdFilter.h,v 1.2 2004/05/10 15:41:32 loose Exp $"
*
* who       when      what
* --------  --------  ----------------------------------------------
* efedrigo  04/07/02  created
*/

/************************************************************************
 *
 *----------------------------------------------------------------------
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <aomath/MAFilter.h>
#include <math.h>

/*!
	\class MAStdFilter
 
	\brief Filter based on standard deviation of the data
 */
template<class T> 
class MAStdFilter : public MAFilter<T> {
  
 public:
  
  /*!
    \brief Argumented constructor

    \param size -- Size/length of the filter
  */
  MAStdFilter(int size);
  /*!
    \brief Destructor
  */
  ~MAStdFilter();
  
  /*!
    \brief Add data

    \param data -- The data to be added

    \return ok -- Status of the operation; returns <tt>false</tt> if an
                  error occured.
  */
  virtual bool Add (T data);
  /*!
    \brief Get the standard deviation of the data

    \return std -- The standard deviation of the data
  */
  double Std();
  /*!
    \brief Get the variance of the data

    \return var -- The variance of the data
  */
  double Var();
  
 protected:
  
  //! Square of the data
  T             mSquare;
  
};

// ------------------------------------------------------------------ MAStdFilter

template<class T>
inline MAStdFilter<T>::MAStdFilter(int size)
  : MAFilter<T>(size)
{
	  mSquare = 0;
}

// ----------------------------------------------------------------- ~MAStdFilter

template<class T>
inline MAStdFilter<T>::~MAStdFilter()
{
}

// -------------------------------------------------------------------------- Add

template<class T>
inline bool MAStdFilter<T>::Add(T data)
{
    if (MAFilter<T>::mFilterLength>0) {
      MAFilter<T>::mSum = MAFilter<T>::mSum+data-MAFilter<T>::mFilterData[MAFilter<T>::mCurrent];
      mSquare = mSquare+data*data-MAFilter<T>::mFilterData[MAFilter<T>::mCurrent]*MAFilter<T>::mFilterData[MAFilter<T>::mCurrent];
      MAFilter<T>::mFilterData[MAFilter<T>::mCurrent]=data;
      MAFilter<T>::mCurrent = (MAFilter<T>::mCurrent + 1) % MAFilter<T>::mFilterLength;
      return true;
    } else {
      return false;
    }
}

// -------------------------------------------------------------------------- Var

template<class T>
double MAStdFilter<T>::Var()
{
    if (MAFilter<T>::mFilterLength>0) {
      return mSquare - (MAFilter<T>::mSum*MAFilter<T>::mSum / (double)MAFilter<T>::mFilterLength);
    } else {
       return 0;
    }

}

// -------------------------------------------------------------------------- Std

template<class T>
double MAStdFilter<T>::Std()
{
    if (MAFilter<T>::mFilterLength>0) {
      double var = mSquare - (MAFilter<T>::mSum*MAFilter<T>::mSum / (double)MAFilter<T>::mFilterLength);
      if (var>=0) {
         return sqrt(var / (double)MAFilter<T>::mFilterLength);
      } else {
         return 0;
      }
    } else {
       return 0;
    }

}

typedef MAStdFilter<unsigned int>   MAStdFilterIUnt;
typedef MAStdFilter<int>            MAStdFilterInt;
typedef MAStdFilter<short>          MAStdFilterShort;
typedef MAStdFilter<unsigned short> MAStdFilterUShort;
typedef MAStdFilter<float>          MAStdFilterFloat;
typedef MAStdFilter<double>         MAStdFilterDouble;

#endif

