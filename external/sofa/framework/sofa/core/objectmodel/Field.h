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
//
// C++ Interface: Field
//
// Description:
//
//
// Author: The SOFA team </www.sofa-framework.org>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SOFA_CORE_OBJECTMODEL_FIELD_H
#define SOFA_CORE_OBJECTMODEL_FIELD_H

#if !defined(__GNUC__) || (__GNUC__ > 3 || (_GNUC__ == 3 && __GNUC_MINOR__ > 3))
#pragma once
#endif

#include <sofa/core/objectmodel/FieldBase.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <map>


namespace sofa
{

namespace core
{

namespace objectmodel
{

typedef std::istringstream istrstream;
typedef std::ostringstream ostrstream;

/**
Pointer to data, readable and writable from/to a string.*/
template < class T = void* >
class Field : public FieldBase
{
public:
    /** Constructor
    \param t a pointer to the value
    \param h help on the field
    */
    Field( T* t, const char* h )
            : FieldBase(h)
            , ptr(t)
    {}

    virtual ~Field()
    {}

    inline void printValue(std::ostream& out) const ;
    inline std::string getValueString() const ;
    inline T* beginEdit()
    {
        m_isSet = true;
        return ptr;
    }
    inline void endEdit()
    {}
    inline void setValue(const T& value )
    {
        *beginEdit()=value;
        endEdit();
    }
    inline const T& getValue() const
    {
        return *ptr;
    }

protected:
    /// Pointer to the parameter
    T* ptr;


    /** Try to read argument value from an input stream.
        Return false if failed
    */
    inline bool read( std::string& s )
    {
        if (s.empty())
            return false;
        //std::cerr<<"Field::read "<<s.c_str()<<std::endl;
        istrstream istr( s.c_str() );
        istr >> *ptr;
        if( istr.fail() )
        {
            //std::cerr<<"field "<<getName<<" could not read value: "<<s<<std::endl;
            return false;
        }
        else
        {
            m_isSet = true;
            return true;
        }
    }


}
;

/// Specialization for reading strings
template<>
inline
bool Field<std::string>::read( std::string& str )
{
    *ptr = str;
    m_isSet = true;
    return true;
}

/// General case for printing default value
template<class T>
inline
void Field<T>::printValue( std::ostream& out=std::cout ) const
{
    out << *ptr << " ";
}

/// General case for printing default value
template<class T>
inline
std::string Field<T>::getValueString() const
{
    ostrstream out;
    out << *ptr;
    return out.str();
}


} // namespace objectmodel

} // namespace core

} // namespace sofa

#endif
