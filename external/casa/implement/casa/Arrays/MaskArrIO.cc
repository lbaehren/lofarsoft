//# MaskArrIO.cc: Write out an ascii reporesentation of a MaskedArray.
//# Copyright (C) 1993,1994,1995,2001
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU Library General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
//# License for more details.
//#
//# You should have received a copy of the GNU Library General Public License
//# along with this library; if not, write to the Free Software Foundation,
//# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id: MaskArrIO.cc,v 19.3 2004/11/30 17:50:14 ddebonis Exp $

#include <casa/iostream.h>

#include <casa/aips.h>
#include <casa/Arrays/MaskArrIO.h>
#include <casa/Arrays/MaskedArray.h>
#include <casa/Arrays/ArrayIO.h>

namespace casa { //# NAMESPACE CASA - BEGIN

template<class T>
ostream & operator<< (ostream &s, const MaskedArray<T> &a)
{
    // Print the Array.
    s << "Array: " << a.getArray();

    s << "\n";

    // Print the Mask.
    s << "Mask:  " << a.getMask();

    return s;
}


} //# NAMESPACE CASA - END

