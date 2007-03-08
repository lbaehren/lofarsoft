/*******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 1       *
*                (c) 2006-2007 MGH, INRIA, USTL, UJF, CNRS                     *
*                                                                              *
* This library is free software; you can redistribute it and/or modify it      *
* under the terms of the GNU Lesser General Public License as published by the *
* Free Software Foundation; either version 2.1 of the License, or (at your     *
* option) any later version.                                                   *
*                                                                              *
* This library is distributed in the hope that it will be useful, but WITHOUT  *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or        *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License  *
* for more details.                                                            *
*                                                                              *
* You should have received a copy of the GNU Lesser General Public License     *
* along with this library; if not, write to the Free Software Foundation,      *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.           *
*                                                                              *
* Contact information: contact@sofa-framework.org                              *
*                                                                              *
* Authors: J. Allard, P-J. Bensoussan, S. Cotin, C. Duriez, H. Delingette,     *
* F. Faure, S. Fonteneau, L. Heigeas, C. Mendoza, M. Nesme, P. Neumann,        *
* and F. Poyer                                                                 *
*******************************************************************************/
//  (C) Copyright John Maddock 2000.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/static_assert for documentation.
#ifndef SOFA_HELPER_STATIC_ASSERT_H
#define SOFA_HELPER_STATIC_ASSERT_H

#include <sofa/helper/system/config.h>

namespace sofa
{

namespace helper
{

#ifndef BOOST_STATIC_ASSERT

template <bool x> struct STATIC_ASSERTION_FAILURE{};
template <> struct STATIC_ASSERTION_FAILURE<true> { enum { value = 1 }; };
template<int x> struct static_assert_test{};

#if defined(_MSC_VER)
#define BOOST_STATIC_ASSERT( B ) \
	typedef ::sofa::helper::static_assert_test<\
      sizeof(::sofa::helper::STATIC_ASSERTION_FAILURE< (bool)( B ) >)>\
         sofa_concat(boost_static_assert_typedef_,__LINE__)
#elif (defined(__INTEL_COMPILER) || defined(__ICL) || defined(__ICC) || defined(__ECC)) || (defined(__GNUC__) && (__GNUC__ == 3) && ((__GNUC_MINOR__ == 3) || (__GNUC_MINOR__ == 4)))
// agurt 15/sep/02: a special care is needed to force Intel C++ and GCC 3.3/3.4 to issue an error 
// instead of warning in case of failure
# define BOOST_STATIC_ASSERT( B ) \
    typedef char sofa_concat(boost_static_assert_typedef_, __LINE__) \
        [ ::sofa::helper::STATIC_ASSERTION_FAILURE< (bool)( B ) >::value ]
#elif defined(__sgi)
// special version for SGI MIPSpro compiler
#define BOOST_STATIC_ASSERT( B ) \
   BOOST_STATIC_CONSTANT(bool, \
     sofa_concat(boost_static_assert_test_, __LINE__) = ( B )); \
   typedef ::sofa::helper::static_assert_test<\
     sizeof(::sofa::helper::STATIC_ASSERTION_FAILURE< \
       sofa_concat(boost_static_assert_test_, __LINE__) >)>\
         sofa_concat(boost_static_assert_typedef_, __LINE__)
#else
// generic version
#define BOOST_STATIC_ASSERT( B ) \
   typedef ::sofa::helper::static_assert_test<\
      sizeof(::sofa::helper::STATIC_ASSERTION_FAILURE< (bool)( B ) >)>\
         sofa_concat(boost_static_assert_typedef_, __LINE__)
#endif

#endif
} // namespace helper

} // namespace sofa

#endif
