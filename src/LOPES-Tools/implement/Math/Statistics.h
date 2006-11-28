/***************************************************************************
 *   Copyright (C) 2005                                                    *
 *   Lars B"ahren (bahren@astron.nl)                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

/* $Id: Statistics.h,v 1.5 2006/10/31 18:24:08 bahren Exp $*/

#ifndef STATISTICS_H
#define STATISTICS_H

// AIPS++ wrapper classes
#include <casa/aips.h>
#include <casa/string.h>
#include <casa/Arrays.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Vector.h>
#include <scimath/Mathematics.h>
#include <casa/BasicSL/Complex.h>
#include <casa/BasicMath/Math.h>

using std::cout;
using std::cerr;
using std::endl;

using casa::IPosition;
using casa::Matrix;
using casa::Vector;

/*!
  \class Statistics.h

  \brief Brief description for class Statistics

  \author Lars B&auml;hren

  \date 2005/11/24

  \test tStatistics.cc

  <h3>Prerequisite</h3>

  <ul type="square">
    <li>[AIPS++] <a href="http://aips2.nrao.edu/docs/aips/implement/Arrays/Array.html">Array</a>
    A templated N-D Array class with zero origin.
    <a href="http://aips2.nrao.edu/docs/aips/implement/Arrays/Vector.html">Vector</a>,
    <a href="http://aips2.nrao.edu/docs/aips/implement/Arrays/Matrix.html">Matrix</a>,
    and Cube are one, two, and three dimensional specializations of Array.
    <li>[AIPS++] <a href="http://aips2.nrao.edu/docs/aips/implement/Arrays/ArrayMath.html">Mathematical
    operations for Arrays</a>.
    <li>[AIPS++] <a href="http://aips2.nrao.edu/docs/aips/implement/Mathematics/FFTServer.html">FFTServer</a>
    -- Methods for Fast Fourier Transforms. The FFTServer
    class provides methods for performing n-dimensional Fast Fourier Transforms
    with real and complex Array's of arbitrary size and dimensionality. It can
    do either real to complex, complex to real, or complex to complex transforms
    with the "origin" of the transform either at the centre of the Array or at 
    the first element.<br>
    Also see the description of the
    <a href="http://aips2.nrao.edu/docs/user/Utility/node287.html">fftserver--Tool</a>
  </ul>

  <h3>Synopsis</h3>

  <h3>Example(s)</h3>

  <h3>References</h3>

  <ul>
    <li>Mathworld (Wolfram Research), <i>Cross-Correlation</i>
    (http://mathworld.wolfram.com/Cross-Correlation.html)
    <li>Numerical Recipes in C (http://www.library.cornell.edu/nr/bookcpdf.html)
    <li>P.J. Lewis, <i>Fast Normalized Cross-Correlation</i>
    (http://www.idiom.com/~zilla/Papers/nvisionInterface/nip.html)
    <li>Chatfield, C. The analysis of time series. Chapman and Hall, 1984.
    <li>Julius O. Smith, <i>Mathematics of the Discrete Fourier Transform (DFT)</i>
    (http://ccrma-www.stanford.edu/~jos/mdft/Cross_Correlation.html)
  </ul>

*/

namespace LOPES {  // NAMESPACE LOPES -- BEGIN

  /*!
    \brief Cross-Correlation of data sets

    \param data         -- The input data
    \param positiveLags -- Compute only for positive lags?
    \param circular     -- Do a circular cross-correlation?

    \return cc -- Sum of the cross-correlations of unique combinations of data
                  sets (e.g. from different antennas).
  */
  template <class T>
    Vector<T> cc (const Matrix<T>& data,
		  const bool& positiveLags=true,
		  const bool& circular=false);
  
  /*!
    \brief Compute the cross-correlation between two data arrays

    We distinguish between to modes of cross-correlation (see J. O. Smith,
    CCRMA, Stanford):
    <ul>
      <li>circular cross-correlation of two signals \f$ f \f$ and \f$ g \f$ in
      \f$ \mathbf C^{N} \f$
      \f[
        \hat{r}_{fg} [l] \equiv \frac{1}{N} (f \star g) = \frac{1}{N}
	\sum_{n=0}^{N-1} \overline{f[n]}\, g[n+l]
	\quad , \qquad l = 0,1,2,...,N-1
      \f]
      <li>unbiased sample cross-correlation
      \f[
        \hat{r}_{fg}^{u} [l]
	\equiv \frac{1}{N-1} \sum_{n=0}^{N-1-l} \overline{f[n]}\, g[n+l]
	\quad , \qquad l = 0,1,2,...,L-1
      \f]
    </ul>
    
    \param f            -- Vector of data
    \param g            -- Vector of data
    \param positiveLags -- Compute only for positive lags?
    \param circular     -- Do a circular cross-correlation?
    
    \return cc -- Cross-correlation of the two input vectors.
  */
  template <class T>
    Vector<T> cc (const Vector<T>& f,
		  const Vector<T>& g,
		  const bool& positiveLags=true,
		  const bool& circular=false);
  
  /*!
    \brief Weighted center of a vector of values.
    
    \f[ \overline{x} = \frac{\sum_i i \cdot x_i}{\sum_i x_i} \f]
    
    \param f - Vector of values.
  */
  template <class T>
    T WeightedCenter (const Vector<T>& f);
  
  /*!
    \brief Weighted center of a vector of values.
    
    The weighted center, \f$ \overline{x} \f$, is considered as equivalent to 
    the center of mass -- thus
    \f[ \overline{x} = \frac{\sum_i x_i \, f(x_i)}{\sum_i f(x_i)} \f]
    
    \param w - Vector of weights.
    \param f - Vector of values.
  */
  template <class T>
    T WeightedCenter (const Vector<T>& w,
		      const Vector<T>& f);

  /*!
    \brief Product-moment correlation coefficient
    
    Given two arrays <i>x</i> and <i>y</i>, this routine computes their correlation
    coefficient \f$ r \f$ (returned as <i>r</i>), the significance level at which
    the null hypothesis of zero correlation is disproved (<i>prob</i> whose small
    value indicates a significant correlation), and Fisher's \f$ z \f$ (returned
    as <i>z</i>), whose value can be used in further statistical tests.
    
    \param x    - First vector of data.
    \param y    - Second vector of data.
    \param r    - Correlation coefficient
    \param prob - Significance level at which the null hypothesis of zero
                  correlation is disproved.
    \param z    - Fisher's \f$ z \f$.
  */
  template <class T>
    void LinearCorrelation (const Vector<T>& x,
			    const Vector<T>& y,
			    T *r,
			    T *prob,
			    T *z);
  
  /*!
    \brief Compute the auto-correlation function of a function \f$ f \f$.
    
    \param cc     - Vector containing the auto-correlated data.
    \param f      - Data vector to be autocorrelated.
  */
  void AutoCorrelation (Vector<double>& cc,
			const Vector<double>& f);
  
  void AutoCorrelation (Vector<Complex>&,
			const Vector<Complex>&);
  
  void AutoCorrelation (Matrix<double>&,
			const Matrix<double>&,
			int);
  
  void AutoCorrelation (Matrix<Complex>&,
			const Matrix<Complex>&,
			int);
  
  /*!
    \brief Compute the cross-correlation of two functions \f$ f \f$ and \f$ g \f$.
    
    Compute the cross-correlation of two functions \f$ f \f$ and \f$ g \f$, 
    \f[
    \hbox{CF}(\tau) = \frac{E \{ [f(t)-\overline{f}]\,[g(t+\tau)-\overline{g}]
    \}} {\sigma_f \, \sigma_g}
    \f]
    where \f$ E \{ f \} \f$ is the expectation value of the function \f$ f \f$,
    \f$ \overline{f} \f$ is its mean, and \f$ \sigma_f \f$ is its standard
    deviation [Oppenheim & Schafer 1975; Edelson & Krolik 1988].
    
    \param cc     - Vector containing the auto-correlated data.
    \param f      - Data vector.
    \param g      - Data vector.
  */
  void CrossCorrelation (Vector<double>& cc,
			 const Vector<double>& f,
			 const Vector<double>& g);
  
  void CrossCorrelation (Vector<Complex>&,
			 const Vector<Complex>&,
			 const Vector<Complex>&);
  
  /*!
    For the 2-dimensional cross-correlation we have
    \f[
    \hbox{corr}(f,g)[i,j] = \sum_{m=0}^{M_f-1} \sum_{n=0}^{N_f-1} f[m,n] \,
    \overline{g}[m+i,n+j]
    \f]
    [http://www.mathworks.com/access/helpdesk/help/toolbox/vipblks/2dcorrelation.html]
    
    \param cc   - Matrix with the cross-correlated data.
    \param f    - Matrix of data.
    \param g    - Matrix of data.
    \param axis - Index of the axis along which to cross-correlate: \f$x\f$-axis
    (axis=0), \f$y\f$-axis (axis=1) or 2-dim cross-correlation 
    (axis=-1).
  */
  void CrossCorrelation (Matrix<double>& cc,
			 const Matrix<double>& f,
			 const Matrix<double>& g,
			 int axis);
  
  
  void CrossCorrelation (Matrix<Complex>& cc,
			 const Matrix<Complex>& f,
			 const Matrix<Complex>& g,
			 int axis);
  
  /*!
      \brief Calculate the running average over a set of data
      
      Origin: this function implements the Glish function <i>runningavg</i> of
      <i>data-func.g</i>.
      
      \param ary - Vector with the data to be averaged; this vector on return
      is overwritten with the result of the computation.
      \param blocksize - Block size over which to average.
    */
    void RunningAverage (Vector<double>& ary,
				const int& blocksize);
    
    /*!
      \fn void NormalizeData (Matrix<double>&,Vector<double>&)
      
      \brief Normalization of a dataset.
      
      Given a number of \f$ n \f$ datasets of length \f$ m \f$, 
      \f$ f_n(\mathbf x) \f$, normalize by the values provided through the 
      data vector \f$ a (\mathbf x) \f$:
      
      \f[ f_{n, \rm norm} (\mathbf x) = \frac{f_n(\mathbf x)}{a(\mathbf x)} \f]
      
      \param data - \f$ m \times n \f$ matrix, containing \f$ n \f$ datasets of
      length \f$ m \f$; overwritten on output with the normalized
      data values.
      \param norm - Vector of length \f$ m \f$, containing normalization values
      for the input data.
    */
    void NormalizeData (Matrix<double>& data,
			Vector<double>& norm);
    
    /*!
      \brief Weighted center of function values
      
      The weighted center, \f$ \overline{\mathbf x} \f$, of a scalar function
      \f$ f(\mathbf x) \f$,
      \f[ \overline{\mathbf x} = \frac{\sum_i \mathbf x_i \, f(\mathbf x_i)}
      {\sum_i f(\mathbf x_i)} \f]
      
      \param x -- (nelem,ndim) Array of function argumentents, holding <i>nelem</i>
      vector element of length <i>ndim</i>.
      \param f -- Vector of function values.
    */
    Vector<double> WeightedCenter (const Matrix<double>& x,
				   const Vector<double>& f);
    
    /*!
      \brief Regridding of a data vector
      
      Regrid data from a vector \f$ vec a_1 \f$ of length \f$ n_1 \f$ to a vector
      \f$ \vec a_2 \f$ of length \f n_2 \f$, where \f$ n_1 \geq n_2 \f$.
    */
    Vector<double> averageData (const Vector<double>& dataIN,
				const int lenOUT);
    
    Matrix<int> binningIndices (const int lenData,
				const int nofBins);
    
  
}  // NAMESPACE LOPES -- END

#endif /* _STATISTICS_H_ */
