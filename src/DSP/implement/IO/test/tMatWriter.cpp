/*-------------------------------------------------------------------------*
 | $Id:: tBeamformer.cc 1099 2007-11-28 18:28:47Z baehren                $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Adriaan Renting (renting@astron.nl)                                   *
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
#include <string>
#include <sstream>

#include "MSReader.h"
#include "MatWriter.h"

#include <casa/Arrays.h>

using namespace std;

/*!
  \file tMatWriter.cpp

  \author Adriaan Renting
*/

int main(int argc, char ** argv){
	string msFileName("/dop156_0/jeurink/L2007_01575_SB0-1.MS");
	string outputDir = "testdir/";

	string polarizations[4];
	polarizations[0] = "xx";
	polarizations[1] = "xy";
	polarizations[2] = "yx";
	polarizations[3] = "yy";
	
	int freqStart = 0;
	int freqStop = 256;
	int timeSlotStart = 0;
	int timeSlotStop = 5;

	int bandID = 0;
	int polarizationID = 1;

	Cube<int> iCube;

	cout << "Rows: " << iCube.nrow() << endl;
	cout << "Columns: " << iCube.ncolumn() << endl;
	cout << "Planes: " << iCube.nplane() << endl;
	
	iCube.resize(10,10,10);
	
	cout << "Rows: " << iCube.nrow() << endl;
	cout << "Columns: " << iCube.ncolumn() << endl;
	cout << "Planes: " << iCube.nplane() << endl;
	
	
	MSReader* msReader = new MSReader(msFileName);
	int nAntenae = msReader->getNumberAntennae();

	//msReader->setCubeSize(nAntenae, freqStop-freqStart);
	MatWriter* matWriter = new MatWriter(nAntenae, nAntenae, freqStop - freqStart);

	cout << "Create ostringstream..." << flush ;
	ostringstream stm("");
	stm << "bla";
	cout << stm.str();
	return 0;
}
