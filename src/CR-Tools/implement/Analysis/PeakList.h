/***************************************************************************
 *   Copyright (C) 2005 by Sven Lafebre                                    *
 *   s.lafebre@astro.ru.nl                                                 *
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

#ifndef _PEAKLIST_H_
#define _PEAKLIST_H_

#include <sstream>
#include <fstream>
#include <Analysis/Peak.h>

namespace CR { // Namespace CR -- begin

/*!
	\class PeakList

  \ingroup Analysis
  
	\brief Class for peaks in a data stream

	\author Sven Lafebre

*/
class PeakList {
	uint length_;
	uint space_;
	Peak* peak_;

 public:
	PeakList();
	PeakList(uint l);
	~PeakList();

	uint length() {return length_;}

	Peak* peak(uint i);
	Peak* addPeak(Peak p);
	Peak* addPeak();
	Peak* nextPeak();
	Peak* operator[] (uint i) {return peak(i);}
};

} // Namespace CR -- end

#endif /* _PEAKLIST_H_ */
