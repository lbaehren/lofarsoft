#ifndef MAFILTER_H
#define MAFILTER_H

/*******************************************************************************
* E.S.O. - VLT project
*
* "@(#) $Id: MAFilter.h,v 1.2 2004/05/10 15:41:32 loose Exp $"
*
* who       when      what
* --------  --------  ----------------------------------------------
* efedrigo  04/07/02  created
* efedrigo  05/02/04  added the possibility to pre-allocate memory and then
*                     declare a shorter filter without new allocations
*/

/************************************************************************
 *
 *----------------------------------------------------------------------
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif
#include <math.h>

#ifdef HAVE_VXWORKS
# include <vxWorks.h>
# include <logLib.h>
#endif

/*!
  \class MAFilter

  \ingroup VLT

  \brief Base class for filters
 
 \test tMAFilter.cc
*/
template<class T> 
class MAFilter {
  
 public:
  
  /*!
    \brief Argumented constructor

    \param size -- Size/length of the filter
  */
  MAFilter(int size);
  /*!
    \brief Destructor
  */
  ~MAFilter();

  /*!
    \brief Add data

    \param data -- The data to be added

    \return ok -- Status of the operation; returns <tt>false</tt> if an
                  error occured.
  */
  virtual bool Add(T data);
  /*!
    \brief Get the currently stored value

    \return value -- The currently stored value
  */
  virtual double Value() { return Mean(); }
  /*!
    \brief Get the mean value

    \return mean -- The mean value
  */
  virtual double Mean();
  /*!
    \brief Get the sum of the values

    \return sum -- The sum of the values
  */
  virtual double Sum();
  /*!
    \brief Get the current length of the filter

    \return length -- The current length of the filter
  */
  unsigned int GetLength() { return mFilterLength; }
  /*!
    \brief Get the maximum length of the filter

    \return maxFilterLength -- The maximum length of the filter
  */
  unsigned int GetMaxLength() { return mMaxFilterLength; }
  /*!
    \brief Set the length of the filter

    \return len -- The length of the filter
  */
  bool SetLength(unsigned int len);
  /*!
    \brief Initialize the filter for a certain length

    \return len -- The length of the filter
  */
  bool Init(unsigned int len);
  
 protected:
  
  //! Maximum length of the filter, [channels]
  unsigned int  mMaxFilterLength;
  //! Current length of the filter, [channels]
  unsigned int  mFilterLength;
  T            *mFilterData;
  T             mSum;
  unsigned int  mCurrent;
  
};

// ==============================================================================
//
//  Inline implementation of the functions
//
// ==============================================================================

template<class T>
inline MAFilter<T>::MAFilter(int size)
{
  mMaxFilterLength = size;
  mFilterLength = size;

    //==================================================================
    // Prepare Moving Average filter
    //==================================================================
    mFilterData = new T[mMaxFilterLength];
    if (mFilterData == 0) {
#ifdef HAVE_VXWORKS
      logMsg("error setting up the MA filter\n",0,0,0,0,0,0);
#else
      fprintf(stderr,"error setting up the MA filter\n");
#endif
    } else {
      memset(mFilterData,0,sizeof(T)*mFilterLength);
      mCurrent = mFilterLength-1;
      mSum = 0;
    }
}

template<class T>
bool MAFilter<T>::Init(unsigned int size)
{

    if (size > mMaxFilterLength ) return false;

    mFilterLength = size;

    //==================================================================
    // Prepare Moving Average filter
    //==================================================================

    if (mFilterData == 0) {
#ifdef HAVE_VXWORKS
      logMsg("error setting up the MA filter\n",0,0,0,0,0,0);
#else
      printf("error setting up the MA filter\n");
#endif
    } else {
      memset(mFilterData,0,sizeof(T)*mFilterLength);
      mCurrent = mFilterLength-1;
      mSum = 0;
    }

    return true;
}

template<class T>
inline MAFilter<T>::~MAFilter()
{
    delete[] mFilterData;
}

template<class T>
inline bool MAFilter<T>::Add(T data)
{
    if (mFilterLength>0) {
      mSum = mSum+data-mFilterData[mCurrent];
      mFilterData[mCurrent]=data;
      mCurrent = (mCurrent + 1) % mFilterLength;

      return true;
    } else {
      return false;
    }
}

template<class T>
double MAFilter<T>::Mean()
{
    if (mFilterLength>0) {
       return (double)((double) mSum / (double)mFilterLength);
    } else {
       return 0;
    }
}

template<class T>
double MAFilter<T>::Sum()
{
    if (mFilterLength>0) {
       return (double) mSum;
    } else {
       return 0;
    }
}

template<class T>
bool MAFilter<T>::SetLength(unsigned int len)
{
    if (len <= mMaxFilterLength ) {
       return Init(len);
    }

    if (mFilterData) {
      delete[] mFilterData;
      mFilterData = 0;
    }

    mMaxFilterLength = len;
    mFilterLength = len;
    mFilterData = new T[mMaxFilterLength];
    if (mFilterData == 0) {
#ifdef HAVE_VXWORKS
      logMsg("error setting up the MA filter\n",0,0,0,0,0,0);
#else
      printf("error setting up the MA filter\n");
#endif
      return false;
    } 

    memset(mFilterData,0,sizeof(T)*mFilterLength);
    mCurrent = mFilterLength-1;
    mSum = 0;
    return true;
}

typedef MAFilter<int>             MAFilterInt;
typedef MAFilter<unsigned int>    MAFilterUInt;
typedef MAFilter<short>           MAFilterShort;
typedef MAFilter<unsigned short>  MAFilterUShort;
typedef MAFilter<float>           MAFilterFloat;
typedef MAFilter<double>          MAFilterDouble;

#endif

