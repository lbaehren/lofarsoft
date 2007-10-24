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

#include <fstream>
#include <sstream>
#include <iostream>
#include <Data/Data.h>

using namespace std;

namespace CR {
  
  Data::Data() {
    data_   = NULL;
    length_ = 0;
    id_     = 0;
  }
  
  Data::Data(uint length) {
    init(length, 0);
  }
  
  Data::Data(uint length, uint id) {
    init(length, id);
  }
  
  Data::~Data() {
    //cout << "Clearing data set " << id_ << "..." << endl;
    if (data_ != NULL) {
      delete []data_;
    }
  }
  
  void Data::clear() {
    // If the data set has length zero, do nothing
    //cout << "Initializing..." << endl;
    if (length_ == 0)
      return;
    
    // Set elements to zero
     for (uint i = 0; i < length_; i ++) {
      data_[i] = 0;
    }
  }
  
  void Data::init(uint length,
		  uint id)
  {

    try {
      //cout << "Allocating memory (" << length*sizeof(short) << " bytes)..." << endl;
      data_   = new short[length];
      length_ = length;
      id_     = id;
      //cout << "Init done..." << endl;
    } catch (std::bad_alloc) {
      cerr << "  FATAL: Error allocating memory. Exiting." << endl;
      exit (1);
    }
  }
  
  /* Experimental stuff.
     Data& Data::operator+ (Data& a) { 
     if (length_ != a.length_) {
     cerr << "WARNING: Length mismatch in data addition. Skipping." << endl;
     return *this;
     }
     Data* t = new Data(length_);
     for (uint i = 0; i < length_; i ++) {
     t->data_[i] = this->data_[i] + a.data_[i];
     }
     return *t;
     } 
     
     Data Data::operator= (Data& a) { 
     this->Data::~Data();
     
     cerr << "WARNING: Direct data assignment is buggy and may cause errors later on."
     << endl;
     Data* t= new Data(a.length());
     t = (Data*)memcpy(t, &a, a.objectSize());
     t->id() = 5;
     return *t;
     }*/
  
}
