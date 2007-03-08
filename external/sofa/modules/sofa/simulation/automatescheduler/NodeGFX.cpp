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
#include <iostream>

#include <sofa/simulation/automatescheduler/NodeGFX.h>
#include <sofa/simulation/automatescheduler/utils.h>
#include <sofa/helper/gl/glfont.h>

// added by Sylvere F.
#ifdef _WIN32
#include <windows.h>
#endif /* _WIN32 */

#include <GL/gl.h>

namespace sofa
{

namespace simulation
{

namespace automatescheduler
{

using namespace helper::gl;

NodeGFX::NodeGFX(char *str, double x, double y )
{
	STRING_CPY(str, this->str);
	this->x = x;
	this->y = y;
	z = 0;
	status = STATUS_BLOCK;
}

void NodeGFX::draw()
{
	switch( status )
	{
		case STATUS_BLOCK :
			glColor3f(1.0,0.0,0.0);
			break;

		case STATUS_READY :
			glColor3f(1.0,1.0,0.0);
			break;

		case STATUS_EXEC :
			glColor3f(0.0,1.0,0.0);
			break;
	} 

	glPointSize(20.0);
	glBegin(GL_POINTS);
	glVertex3d(x,y,z);
	glEnd();

	glColor3f(1.0f,1.0f,1.0f);
	glfntWriteBitmap((float)x - 0.0f, (float)y, str);
}

} // namespace automatescheduler

} // namespace simulation

} // namespace sofa
