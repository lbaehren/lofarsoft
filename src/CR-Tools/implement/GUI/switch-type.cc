/*-------------------------------------------------------------------------*
 | $Id:: IO.h 393 2007-06-13 10:49:08Z baehren                           $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2008                                                    *
 *   Heino Falcke                                                          *
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

// switch (DATATYPE) {
 case POINTER:
   SW_TYPE_COMM(P,HPointer);
   break;
 case INTEGER:
   SW_TYPE_COMM(I,HInteger);
   break;
 case NUMBER:
   SW_TYPE_COMM(N,HNumber);
   break;
 case COMPLEX:
   SW_TYPE_COMM(C,HComplex);
   break;
 case STRING:
   SW_TYPE_COMM(S,HString);
   break;
 default:
   ERROR("Error: unknown type."); 
//  }
#undef SW_TYPE_COMM
   
