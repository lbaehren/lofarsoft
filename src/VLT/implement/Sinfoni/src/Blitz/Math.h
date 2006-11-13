//#  Math.h: Mathematical functions operating on Blitz arrays.
//#
//#  Copyright (C) 2002-2006
//#  ASTRON (Netherlands Foundation for Research in Astronomy)
//#  P.O.Box 2, 7990 AA Dwingeloo, The Netherlands, seg@astron.nl
//#
//#  This program is free software; you can redistribute it and/or modify
//#  it under the terms of the GNU General Public License as published by
//#  the Free Software Foundation; either version 2 of the License, or
//#  (at your option) any later version.
//#
//#  This program is distributed in the hope that it will be useful,
//#  but WITHOUT ANY WARRANTY; without even the implied warranty of
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//#  GNU General Public License for more details.
//#
//#  You should have received a copy of the GNU General Public License
//#  along with this program; if not, write to the Free Software
//#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//#
//#  $Id: Math.h,v 1.11 2006/05/19 16:03:23 loose Exp $

#ifndef SINFONI_BLITZ_MATH_H
#define SINFONI_BLITZ_MATH_H

#include <Sinfoni/Config.h>

#if !defined(HAVE_BLITZ)
#error Blitz++ package is required
#endif

//# Includes
#include <Sinfoni/Blitz/Types.h>
#include <Sinfoni/Math.h>
#include <cmath>
#include <vector>

namespace ASTRON
{
  namespace SINFONI
  {

    // @ingroup mathematics
    // @defgroup arrays_and_matrices Arrays and Matrices

    // @ingroup mathematics
    // @defgroup data_analysis Data Analysis


    // @ingroup arrays_and_matrices
    // @defgroup isempty isEmpty
    //
    // isEmpty(x) returns true if x is an empty vector or matrix (either the
    // number of rows, or the number of columns, or both are zero); otherwise,
    // returns false.

    //@{

    inline bool isEmpty(const VectorReal& x)
    {
      return x.size() == 0;
    }


    inline bool isEmpty(const VectorInteger& x)
    {
      return x.size() == 0;
    }


    inline bool isEmpty(const MatrixReal& x)
    {
      return x.size() == 0;
    }


    inline bool isEmpty(const MatrixInteger& x)
    {
      return x.size() == 0;
    }


    //@}


    // @ingroup arrays_and_matrices
    // @defgroup isequal isEqual
    //
    // isEqual(a,b) returns true if the two arrays are the same size
    // and contain the same values; otherwise, returns false.

    //@{

    inline bool isEqual(Real a, Real b)
    {
      return (a == b);
    }

    inline bool isEqual(const VectorComplex& a, const VectorComplex& b)
    {
      return (a.rows() == b.rows() && all(a == b));
    }

    inline bool isEqual(const VectorReal& a, const VectorReal& b)
    {
      return (a.rows() == b.rows() && all(a == b));
    }

    inline bool isEqual(const VectorInteger& a, const VectorInteger& b)
    {
      return (a.rows() == b.rows() && all(a == b));
    }

    inline bool isEqual(const MatrixComplex& a, const MatrixComplex& b)
    {
      return (a.rows() == b.rows() && all(a == b));
    }

    inline bool isEqual(const MatrixReal& a, const MatrixReal& b)
    {
      return (a.rows() == b.rows() && a.cols() == b.cols() && all(a == b));
    }

    inline bool isEqual(const MatrixInteger& a, const MatrixInteger& b)
    {
      return (a.rows() == b.rows() && a.cols() == b.cols() && all(a == b));
    }

    //@}


    inline bool compare(const VectorReal& a, const VectorReal& b, 
                        Real eps = std::numeric_limits<Real>::epsilon())
    {
      //# Initialize \a result with test for equal size of \a a and \a b.
      bool result(a.rows() == b.rows());
      if (result)
      {
        for (int r = 0; r < a.rows() && result; r++) {
          result = result && compare(a(r), b(r), eps);
        }
      }
      return result;
    }

    inline bool compare(const VectorComplex& a, const VectorComplex& b, 
                        Real eps = std::numeric_limits<Real>::epsilon())
    {
      //# Initialize \a result with test for equal size of \a a and \a b.
      bool result(a.rows() == b.rows());
      if (result)
      {
        for (int r = 0; r < a.rows() && result; r++) {
          result = result && compare(a(r), b(r), eps);
        }
      }
      return result;
    }

    inline bool compare(const MatrixReal& a, const MatrixReal& b, 
                        Real eps = std::numeric_limits<Real>::epsilon())
    {
      //# Initialize \a result with test for equal size of \a a and \a b.
      bool result(a.rows() == b.rows() && a.cols() == b.cols());
      if (result)
      {
        for (int r = 0; r < a.rows() && result; r++) {
          for (int c = 0; c < a.cols() && result; c++) {
            result = result && compare(a(r,c), b(r,c), eps);
          }
        }
      }
      return result;
    }

    inline bool compare(const MatrixComplex& a, const MatrixComplex& b, 
                        Real eps = std::numeric_limits<Real>::epsilon())
    {
      //# Initialize \a result with test for equal size of \a a and \a b.
      bool result(a.rows() == b.rows() && a.cols() == b.cols());
      if (result)
      {
        for (int r = 0; r < a.rows() && result; r++) {
          for (int c = 0; c < a.cols() && result; c++) {
            result = result && compare(a(r,c), b(r,c), eps);
          }
        }
      }
      return result;
    }


    // @ingroup arrays_and_matrices
    // @defgroup diag diag
    //
    // Diagonal matrices and diagonals of a matrix.
    //
    // diag(v,k) when v is a vector with n components is a square matrix of
    // order n+abs(k) with the elements of v on the k-th diagonal. k = 0 is
    // the main diagonal, k > 0 is above the main diagonal and k < 0 is below
    // the main diagonal.
    //
    // diag(v) is the same as diag(v,0) and puts v on the main diagonal.
    //
    // diag(x,k) when x is a matrix is a column vector formed from
    // the elements of the k-th diagonal of x.

    //@{

    inline VectorReal diag(const MatrixReal& x, int k = 0)
    {
      //# Calculate the number of elements on the k-th diagonal.
      int n;
      if (k < 0) 
        n = std::max(0, std::min(x.cols(), x.rows()+k));
      else 
        n = std::max(0, std::min(x.rows(), x.cols()-k));

      //# Allocate a vector that will hold the elements of the k-th diagonal.
      VectorReal v(n);

      //# Assign the elements of the k-th diagonal.
      for (int i=0; i<n; i++) {
        if (k < 0) 
          v(i) = x(i-k, i);
        else 
          v(i) = x(i, i+k);
      }
      return v;
    }


    inline VectorReal diag(const MatrixInteger& m, int k = 0)
    {
      MatrixReal mm(blitz::cast<Real>(m));
      return diag(mm, k);
    }


    inline MatrixReal diag(const VectorReal& v, int k = 0)
    {
      //# Calculate the size of the square matrix to be formed.
      int n = v.rows() + abs(k);

      //# Allocate a matrix that will hold the elements of v.
      MatrixReal m(n, n);

      //# Set all elements equal to zero.
      m = 0;
      
      //# Assign the elements of v to the k-th diagonal.
      for (int i=0; i<v.size(); i++) {
        if (k < 0) 
          m(i-k, i) = v(i);
        else 
          m(i, i+k) = v(i);
      }
      return m;
    }


    inline MatrixReal diag(const VectorInteger& v, int k = 0)
    {
      VectorReal vv(blitz::cast<Real>(v));
      return diag(vv, k);
    }

    //@}


    // @ingroup arrays_and_matrices
    // @defgroup trans trans
    //
    // Matrix transpose.
    //
    // trans(x) is the linear algebraic transpose of x.

    //@{

    inline MatrixReal trans(const MatrixReal& x)
    {
      MatrixReal xt(x);
      xt.transposeSelf(blitz::secondDim, blitz::firstDim);
      return xt;
    }


    inline MatrixInteger trans(const MatrixInteger& x)
    {
      MatrixInteger xt(x);
      xt.transposeSelf(blitz::secondDim, blitz::firstDim);
      return xt;
    }


    //@}


    // @ingroup data_analysis
    // @defgroup mean mean
    //
    // Average or mean value.
    //
    // For vectors, mean(x) is the mean value of the elements in x. For
    // matrices, mean(x) is a vector containing the mean value of each column.

    //@{

    inline Real mean(const VectorReal& v)
    {
      return blitz::mean(v);
    }


    inline Real mean(const VectorInteger& v)
    {
      VectorReal vv(blitz::cast<Real>(v));
      return mean(vv);
    }


    inline VectorReal mean(const MatrixReal& m)
    {
      blitz::firstIndex i;
      blitz::secondIndex j;
      //# We must calculate the column-wise mean, hence we should "reduce"
      //# using the second index.
      VectorReal v(blitz::mean(m(j,i),j));
      return v;
    }


    inline VectorReal mean(const MatrixInteger& m)
    {
      MatrixReal mm(blitz::cast<Real>(m));
      return mean(mm);
    }

    //@}


    // @ingroup data_analysis
    // @defgroup var var
    //
    // Variance. 
    //
    // For vectors, var(x) returns the variance of x. For matrices, var(x)
    // is a vector containing the variance of each column of x.
    // 
    // var(x) normalizes by N-1 where N is the sequence length. This makes
    // var(x) the best unbiased estimate of the variance if x is a sample
    // from a normal distribution.

    //@{

    inline Real var(const VectorReal& x)
    {
      return sum(sqr(x - mean(x))) / (std::max(1, x.rows()) - 1);
    }


    inline Real var(const VectorInteger& x)
    {
      VectorReal v(blitz::cast<Real>(x));
      return var(v);
    }


    inline VectorReal var(const MatrixReal& x)
    {
      //# Allocate a vector that will hold the variances.
      VectorReal v(x.cols());

      for (int i = 0; i < x.cols(); i++) {
        v(i) = var(x(blitz::Range::all(),i));
      }
      return v;
    }


    inline VectorReal var(const MatrixInteger& x)
    {
      MatrixReal m(blitz::cast<Real>(x));
      return var(m);
    }

    //@}


    // @ingroup data_analysis
    // @defgroup cov cov
    //
    // Covariance matrix.
    //
    // cov(x), if x is a vector, returns the variance. For matrices, where
    // each row is an observation, and each column a variable, cov(x) is the
    // covariance matrix. diag(cov(x)) is a vector of variances for each
    // column.
    //
    // cov(x) normalizes by N-1 where N is the number of observations. This
    // makes cov(x) the best unbiased estimate of the covariance matrix if the
    // observations are from a normal distribution.

    //@{

    inline Real cov(const VectorReal& x)
    {
      return var(x);
    }


    inline Real cov(const VectorInteger& x)
    {
      VectorReal v(blitz::cast<Real>(x));
      return cov(v);
    }


    inline MatrixReal cov(const MatrixReal& x)
    {
      //# Allocate a matrix that will hold the covariances.
      MatrixReal cm( x.cols(), x.cols() );

      //# First, we'll remove the mean for each variable.
      for (int i = 0; i < x.cols(); i++) {
        VectorReal v( x(blitz::Range::all(),i) );
        v = v - mean(v);
      }

      //# Calculate the covariance for each pair of variables. Note that the
      //# covariance matrix is symmetrical.
      for (int i = 0; i < x.cols(); i++) {
        for (int j = i; j < x.cols(); j++) {
          cm(i,j) = blitz::sum( x(blitz::Range::all(),i) * 
                                x(blitz::Range::all(),j) ) / (x.rows() - 1);
          if (i != j) cm(j,i) = cm(i,j);
        }
      }

      //# Return the result
      return cm;
    }


    inline MatrixReal cov(const MatrixInteger& x)
    {
      MatrixReal m(blitz::cast<Real>(x));
      return cov(m);
    }

    //@}


    // @ingroup data_analysis
    // @defgroup median median
    //
    // Median value.
    //
    // For vectors, median(x) is the median value of the elements in x. For
    // matrices, median(x) is a vector containing the median value of each
    // column.
    //
    // \note median() is currently implemented using the STL algorithm
    // nth_element(). To use this algorithm, we first need to copy the
    // contents of \a x into an STL vector. This is not terribly efficient,
    // but I didn't have the time to reimplement the nth_element() method for
    // VectorReal. On the other hand, nth_element() modifies the container, so
    // we would have to make a copy anyway, because we don't want to modify
    // the original Vector \a x.

    //@{

    inline Real median(const VectorReal& x)
    {
      //# Variable that will hold the median of \a x. Initialize it to NaN.
      Real result(nan(""));

      //# If vector \a x contains no elements, return immediately.
      if (x.rows() == 0) return result;

      //# Copy the contents of \a x into the STL vector \a v.
      std::vector<Real> v(x.rows());
      for (int i = 0; i < x.rows(); i++) {
        v[i] = x(i);
      }

      std::vector<Real>::iterator begin(v.begin());
      std::vector<Real>::iterator end(v.end());

      //# Iterator \a pos will be pointing to the middle of \a v when v.size()
      //# is odd, and to the first element past the middle when v.size() is
      //# even.
      std::vector<Real>::iterator pos(begin + v.size()/2);

      //# Partially sort \a v so that the element at position \a pos is the one
      //# that would be there if \a v had been fully sorted.
      nth_element(begin, pos, end);

      //# Assign the value at \a pos to \a result. If v.size() is odd we're
      //# done.
      result = *pos;

      //# If v.size() is even, we need to repeat the partial sort to get the
      //# element that would have been at position \a pos-1 if \a v had been
      //# fully sorted. Then we need to average these values.
      if (x.rows() % 2 == 0) {
        nth_element(begin, --pos, end);
        result += *pos;
        result /= 2;
      }

      return result;
    }


    inline Real median(const VectorInteger& x)
    {
      VectorReal v(blitz::cast<Real>(x));
      return median(v);
    }
    

    inline VectorReal median(const MatrixReal& x)
    {
      //# Allocate a vector that will hold the median values.
      VectorReal v(x.cols());

      for (int i = 0; i < x.cols(); i++) {
        v(i) = median(x(blitz::Range::all(),i));
      }
      return v;
    }


    inline VectorReal median(const MatrixInteger& x)
    {
      MatrixReal m(blitz::cast<Real>(x));
      return median(m);
    }

    //@}


  } // namespace SINFONI

} // namespace ASTRON

#endif
