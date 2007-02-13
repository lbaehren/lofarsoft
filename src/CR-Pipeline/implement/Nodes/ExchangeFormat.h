/***************************************************************************
 *   Copyright (C) 2005                                                    *
 *   Sven Lafebre (<mail>)                                                 *
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

/* $Id: ExchangeFormat.h,v 1.6 2006/08/01 15:03:25 bahren Exp $*/

#ifndef EXCHANGEFORMAT_H
#define EXCHANGEFORMAT_H

// AIPS++/CASA includes
#include <casa/aips.h>

namespace CR {  // Namespace CR -- begin
  
  /*!
    \class ExchangeFormat
    
    \ingroup Nodes
    
    \brief Description class for a data exchange format
    
    \author Sven Lafebre
    
    \date 2005/07/21
    
    \test tExchangeFormat.cc
    
    \todo Develop flags to check for to distinguish between data exchange formats.
    
    <h3>Prerequisite</h3>
    None.
    
    <h3>Synopsis</h3>
    This class gives a description of the format of the data exchange between two
    pipeline objects. It contains flags specifying properties of the data. Flags
    inplemented so far are:

  <table border="0">
    <tr>
      <td class="indexkey">EF_isComplex</td>
      <td>Set to \c true when the data is of type complex, and set to \c false
      when the data is only real.</td>
    </tr>
    <tr>
      <td class="indexkey">EF_isScalar</td>
      <td>Set to \c true when the data is inheritantly zero-dimensional, set to
      \c false when the data can be of greater dimensionality.</td>
    </tr>
    <tr>
      <td class="indexkey">EF_isNumeric</td>
      <td>Set to \c true when the data is of a numeric type, such as Double,
      Real or Int, and set to \c false when the data is of non-numeric type,
      such as String.</td>
    </tr>
  </table>

  More flags will be added as appropriate.
*/
static const int EF_isNumeric   =  1;
static const int EF_isScalar    =  2;
static const int EF_isString    =  4;
static const int EF_isComplex   =  8;

class ExchangeFormat {

  unsigned int format_p;

 public:

  // --- Construction ----------------------------------------------------------

  /*!
    \brief Empty constructor

    Creates an empty ExchangeFormat object; i.e. one in which no format is set.
  */
  ExchangeFormat ();

  /*!
    \brief Copy constructor

    \param f -- The exchange format object the new object should be set equal to.

    Creates an ExchangeFormat object and sets the format flags to those of <b>f</b>.
  */
  ExchangeFormat (const unsigned int &f);

  /*!
    \brief Argumented constructor

    \param f -- The exchange format specification the new object should be set to.

    Creates an ExchangeFormat object and sets the format flags to <b>f</b>.
  */
  ExchangeFormat (ExchangeFormat &f);

  // --- Destruction -----------------------------------------------------------

  /*!
    \brief Destructor
  */
  ~ExchangeFormat ();

  // --- Parameters ------------------------------------------------------------

  /*!
    \brief Get or set the entire exchange format specification
  */
  unsigned int& format () {
    return format_p;
  }

  /*!
    \brief Get an exchange format flag

    \param fl -- The name of the flag to query

    \return   -- The state of the queried flag

    This function returns the state of a data exchange format flag <b>fl</b>.
    Mutliple flags can be checked for by using the | operator:
    \code
      f.flag(EF_isScalar | EF_isComplex)
    \endcode
    will only return true when both the EF_isScalar and EF_isComplex flags are set.
  */
  bool flag(const unsigned int fl) {
    return ((format_p & fl) == fl);
  }

  /*!
    \brief Set or clear an exchange format flag

    \param fl -- The name of the flag to set or clear

    \param on -- Should the value be set (true) or cleared (false)?

	 This function sets or clears the state of a data exchange format flag <b>fl</b>.
    Mutliple flags can be set or cleared by using the | operator. For example,
    \code
      f.clearFlag(EF_isScalar | EF_isComplex, true)
    \endcode
    will set both the EF_isScalar and EF_isComplex flags.
  */
  void setFlag(const unsigned int fl,
	       bool on);

  /*!
    \brief Set an exchange format flag

    \param fl -- The name of the flag to set

	 This function sets the state of a data exchange format flag <b>fl</b>.
    Mutliple flags can be set by using the | operator. For example,
    \code
      f.setFlag(EF_isScalar | EF_isComplex)
    \endcode
    will set both the EF_isScalar and EF_isComplex flags.
  */
  void setFlag(const unsigned int fl) {
    setFlag(fl, true);
  }

  /*!
    \brief Clear an exchange format flag

    \param fl -- The name of the flag to clear

	 This function clears the state of a data exchange format flag <b>fl</b>.
    Mutliple flags can be cleared by using the | operator. For example,
    \code
      f.clearFlag(EF_isScalar | EF_isComplex)
    \endcode
    will clear both the EF_isScalar and EF_isComplex flags.
  */
  void clearFlag(const unsigned int fl) {
    setFlag(fl, false);
  }

  // --- Functions -------------------------------------------------------------

  /*!
    \brief Compare two ExchangeFormat objects

    \param f       -- The ExchangeFormat object that should be compared to this
                      one

    \return result -- Are the two object similar or not?

    This functions checks whether two ExchangeFormats match or not, by comparing
    the different flags.
  */
  bool compare(const ExchangeFormat &f);

  // --- Operators -------------------------------------------------------------

  /*!
    \brief Compare two ExchangeFormat objects

    \param f       -- The ExchangeFormat object that should be comapred to this
                      one

    \return result -- Are the two object similar or not?

    This functions checks whether two ExchangeFormats match or not, by comparing
    the different flags.
  */
  bool operator==(const ExchangeFormat &f) {
    return compare(f);
  }

};

}  // Namespace CR -- end

#endif
