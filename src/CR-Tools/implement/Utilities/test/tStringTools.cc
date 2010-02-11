/***************************************************************************
 *   Copyright (C) 2004,2005                                               *
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

/* $Id$ */

#include <Utilities/StringTools.h>

/*!
  \file tStringTools.cc
  
  \ingroup CR_Utilities
*/

// ------------------------------------------------------------------ string2char

int string2char ()
{
  int nofFailedTests (0);
  int maxlen (30);
  string teststring ("");

  for (int i=0; i<maxlen; i++) {
    teststring += "a";
    //
    char* tmp = CR::string2char(teststring);
    //
    cout << i+1 << "\t[" << teststring << "] -> ["
	 << tmp << "]" << endl;
    //
    delete tmp;
  }

  return nofFailedTests;
}

// ------------------------------------------------------------------------- main

/*!
  \file tStringTools.cc

  \brief A collection of tests for StringTools.

  \author Lars B&auml;hren

  \date 2005/03/02
*/

int main ()
{
  int nofFailedTests (0);
  String filepath = "/home/user/bin/myfile.h";

  String filename = CR::fileFromPath (filepath);
  String dirname = CR::dirFromPath (filepath);
  Vector<String> substrings = CR::getSubstrings (filepath,"/");
  int nofSubstrings = CR::nofSubstrings (filepath,"/");

  cout << "\n[tStringTools]\n" << endl;
  cout << " - Complete path to file : " << filepath << endl;
  cout << " - Name of file ........ : " << filename << endl;
  cout << " - Directory name ...... : " << dirname << endl;
  cout << " - Substrings .......... : " << substrings << endl;
  cout << " - Number of substrings  : " << nofSubstrings << endl;

  cout << endl;

  // Test conversion of string to aray of characters
  nofFailedTests += string2char ();

  return nofFailedTests;

}
