//#  Debug.h: Useful debugging stuff.
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
//#  $Id: Debug.h,v 1.6 2006/05/19 16:03:23 loose Exp $

#ifndef SINFONI_DEBUG_H
#define SINFONI_DEBUG_H

#include <Sinfoni/Config.h>


#if (SINFONI_DEBUG == 1)
#include <iostream>
#include <fstream>
#define DBG_DUMP(file, strm) \
{ \
  std::ofstream ofs(file); \
  ofs << std::setprecision(8) << strm << std::endl; \
}
#else
#define DBG_DUMP(file, strm)
#endif


#if (SINFONI_TRACE == 1)
#include <iostream>
#define DBG_TRACE(msg) \
{ \
  std::string file(__FILE__); \
  int line(__LINE__); \
  std::string::size_type idx(file.find_last_of('/')); \
  if (idx != std::string::npos) file = file.substr(idx+1); \
  std::cerr << "TRACE - " << file << ":" << line \
            << " - " << msg << std::endl; \
}
#else
#define DBG_TRACE(msg)
#endif


#endif
