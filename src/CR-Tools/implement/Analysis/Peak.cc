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

#include <iostream>
#include <string.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <Analysis/Peak.h>

#include <fstream>

using namespace std;

namespace CR { // Namespace CR -- begin

Peak::Peak() {
	p_time         = 0;
	p_duration     = 0;
	p_height       = 0;
	p_significance = 0;
}

Peak::Peak(uint t, int h, uint s) {
	p_time         = t;
	p_duration     = 0;
	p_height       = h;
	p_significance = s;
}

Peak::Peak(char* str) {
	memcpy(str,   &this->p_time,         4);
	memcpy(str,   &this->p_duration,     4);
	memcpy(str+4, &this->p_height,       4);
	memcpy(str+4, &this->p_significance, 4);
}

void Peak::print() {
	if (this->height() != 0 && this->significance() != 0)
		cout << "[t=" << this->time() << " (-" << this->duration() << "),\th="
		     << this->height() << ",\ts=" << this->significance() << "]\t" << endl;
}

Peak* Peak::mergePeaks(Peak* peaks, uint num) {
	if (num == 1) return this;
	Peak* p = peaks;
	//cout << "To be merged: ";
	//p->print();
	uint t0 = p->time() - p->duration();
	int  h  = p->height();
	uint s  = p->significance();
	for (int i = 1; i < (int)num; i ++) {
		p = &peaks[i];
		//cout << "To be merged: ";
		//p->print();
		if (abs(h) < abs(p->height())) h = p->height();
		s += p->significance();
	}

	this->p_time         = p->time();
	this->p_duration     = p->time() - t0;
	this->p_height       = h;
	this->p_significance = s;
	//cout << "Result:       ";
	//this->print();
	return this;
}

} // Namespace CR -- end
