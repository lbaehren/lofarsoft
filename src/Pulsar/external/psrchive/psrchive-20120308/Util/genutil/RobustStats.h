/***************************************************************************
 *
 *   Copyright (C) 2011 by Stefan Oslowski
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// This set of methods aims at providing robust, resistant and efficient statistics
// Resistance means that the methods provided are insensitive to small changes of sample; i.e. are dependent on the locust of the data and ignore outliers
// Robustness means that the methods provided are insensitive to the assumptions about the intrinsic population
// Efficient means that the methods provided yield maximum information that can be derived from provided data.
//
// For more information, see Understanding Robust and Exploratory Data Analysis by Hoaglin et al, or Robust estimators of scale, Lax 1985

#ifndef __RobustStats_h
#define __RobustStats_h

#include <cmath>
#include <vector>
#include <algorithm>
#include <iostream>

using namespace std;

// Measures of scale:

// Median absolute deviation
template<typename T>
T MAD ( T * data, unsigned size );


// f-spread
template<typename T>
T f_spread ( T * data, unsigned size );

// f-pseudosigma
template<typename T>
T f_pseudosigma ( T * data, unsigned size );

template<typename T>
T f_pseudosigma ( T f_spread );

// robust std deviation

template<typename T>
T robust_stddev ( T * data, unsigned size, T initial_guess, T initial_guess_scale = 1.4826, T Tukey_tune = 6.0 );

template<typename T>
T robust_stddev ( T * data, unsigned size);
//double robust_stddev ( double * data, unsigned size );
//


// Measures of scale:

/*!
 * @brief Calculate the median absolute deviation (MAD). The efficiency varies from 40% (for Gaussian distributions) to 90% (heavy-tailed distributions)
 * @param[in] data The input sample
 * @param[in] size Size of the input sample
 */
template<typename T>
T MAD ( T * data, unsigned size )
{
  vector<T> input;
  input.resize ( size );

  //copy ( data, data + size, back_inserter ( input ) );
  std::copy ( data, data + size, input.begin() );

  sort ( input.begin(), input.end() );

  // median of the input data
  T median = input.at ( size%2==0 ? (unsigned)( (float)size / 2 + 0.5 ) : size / 2 ); 

  // vector of the absolute deviations from the median
  vector<T> AD;

  for (unsigned element = 0 ; element < size ; element ++ )
    AD.push_back ( abs ( input.at( element ) - median ) );

  sort ( AD.begin(), AD.end() );

  // return the median absolute deviation
  return AD.at ( size%2==0 ? (unsigned)( (float)size / 2 + 0.5 ) : size / 2 );
}

/*!
 * @brief the f-spread. This provides the difference between the upper and lower fourths of a dataset. The standard application is to easily test for non-gaussianity of the dataset. For Gaussian distribution this measure is not very efficient
 *
 */

template<typename T>
T f_spread ( T * data, unsigned size )
{
  cerr << "RobustStats::f_spread WARNING f_spread not implemented yet" << endl;
  return (T)0.0;
}

/*!
 * @brief the f-pseudosigma. This is the convenient way of analysing the f-spread. F-pseudosigma is defined as the f-spread expressed in units of the f-spread for Gaussian distribution
 *
 */
template<typename T>
T f_pseudosigma ( T * data, unsigned size )
{
  // 1.349 is the value of f_spread for Gaussian distribution
  return f_spread ( data, size )  / 1.349;
}

template<typename T>
T f_pseudosigma ( T f_spread )
{
  // 1.349 is the value of f_spread for Gaussian distribution
  return f_spread / (T)1.349;
}


//TODO some sources say that the most efficient choice is 9 for the tuning constant. I think this is because some sources use c * MAD, while others use c * MAD * 1.4826
//TODO IDL implementation has a slighly different estimator. I think they are attempting to get the correct assymptotic value but they are doing it wrong :) Hmm, gives the same result, I guess it's the same estimator just expressed differently.
/*! 
 * @brief calculate the A-estimator of the standard deviation using modified Tukey's biweighting. This estimator is superior to any of the above. E.g. for Gaussian distribution the efficiency reaches almost 90%. This estimator is suitable for samples size > 10.
 * @param[in] data The input sample
 * @param[in] size Size of the input sample
 * @param[in] initial_guess The initial guess of standard deviation. The best choice is to use MAD
 * @param[in] initial_guess_scale This converts the MAD initial guess to a consistent estimator of the standard deviation. In the case of MAD and normally distributed data the conversion value equals 1.4826. In general, this corresponds to the inverse of of the quantile function value at 0.75 for the given probability distribution.
 * @param[in] Tukey_tune is the tuning constant for the influence function (Tukey's biweight in this case). This affects the efficiency of the estimator. The optimal value is 6.0 (i.e. include data up to 4 sigma away from the mean (Data analysis and regression, Mosteller and Tukey 1977)
*/
template<typename T>
T robust_stddev ( T * data, unsigned size, T initial_guess, T initial_guess_scale = 1.4826, T Tukey_tune = 6.0 )
{
  vector<T> input;
  input.resize ( size );
  copy ( data, data + size, input.begin()  );

  sort ( input.begin(), input.end() );

  T median = input.at ( size%2==0 ? (unsigned)( (float)size / 2 + 0.5 ) : size / 2 );

  vector<T> biweighted_sq;
  vector<unsigned> indices;
  T tmp;
  // calculate the biweighted values:
  for ( unsigned i_el = 0; i_el < size ; ++ i_el )
  {
    tmp = ( input.at ( i_el ) - median ) / ( Tukey_tune * initial_guess_scale * initial_guess ) ;
    tmp *= tmp ;

    //cout << i_el << " " << Tukey_tune << " " << initial_guess << " " << initial_guess_scale << endl;
    if ( tmp < 1.0 )
    {
      indices.push_back ( i_el );
      biweighted_sq.push_back ( tmp );
    }
  }

  tmp = 0.0;
  T std_dev = (T)0.0;
  for ( unsigned i_el = 0; i_el < biweighted_sq.size() ; ++ i_el )
  {
    std_dev += pow ( input.at ( indices.at ( i_el ) ) - median, 2 ) * pow ( 1.0 - biweighted_sq.at( i_el ), 4 ) ;
    tmp += ( 1.0 - biweighted_sq.at ( i_el ) ) * ( 1.0 - 5.0 * biweighted_sq.at ( i_el ) );
  }

  tmp = abs ( tmp );

  // eq 4.15 Lax 1985 (robust scale estimators)
  //cout << " LAX version " << endl;
  return std_dev = sqrt ( std_dev / ( (T)size - 1.0 ) ) / tmp * (T)size ;
  // IDL implementation
  //cout << " IDL version " << endl;
  //return std_dev = sqrt ( std_dev / tmp / (tmp - 1) * (T)size) ;
}

/*
 * @brief convienience wrapper for robust_stddev which assumes that MAD is to be used as the initial guess for scale
 */
template<typename T>
T robust_stddev ( T * data, unsigned size )
{
  return robust_stddev ( data, size, MAD ( data, size ) );
}

#endif
