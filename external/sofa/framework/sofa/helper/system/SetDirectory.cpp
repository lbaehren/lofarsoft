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
#include <sofa/helper/system/SetDirectory.h>

#ifndef WIN32
#include <unistd.h>
#else
#include <windows.h>
#include <direct.h>
#endif

#include <string.h>
#include <iostream>

namespace sofa
{

namespace helper
{

namespace system
{

SetDirectory::SetDirectory(const char* filename)
{
	int len = strlen(filename);
	while (len>0 && filename[len]!='\\' && filename[len]!='/')
		--len;
	directory = new char[len+1];
	memcpy(directory, filename, len);
	directory[len]='\0';
	previousDir[0]='\0';
	if (directory[0])
	{
		std::cout << "chdir("<<directory<<")"<<std::endl;
#ifndef WIN32
		getcwd(previousDir, sizeof(previousDir));
		chdir(directory);
#else
		_getcwd(previousDir, sizeof(previousDir));
		_chdir(directory);
#endif
	}
}

SetDirectory::~SetDirectory()
{
	if (directory[0] && previousDir[0])
	{
		std::cout << "chdir("<<previousDir<<")"<<std::endl;
#ifndef WIN32
		chdir(previousDir);
#else
		_chdir(previousDir);
#endif
	}
	delete[] directory;
}

} // namespace system

} // namespace helper

} // namespace sofa

