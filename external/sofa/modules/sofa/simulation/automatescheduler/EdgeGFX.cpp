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
#include <sofa/simulation/automatescheduler/EdgeGFX.h>
#include <sofa/simulation/automatescheduler/Node.h>

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

/////////////
// EdgeGFX //
/////////////

EdgeGFX::EdgeGFX(simulation::automatescheduler::Node *p, simulation::automatescheduler::Node *n)
{
	prev = p;
	next = n;
}


void EdgeGFX::draw()
{
	glBegin(GL_LINES);
	glColor3f(0.0,0.0,0.0);
	glVertex3d(prev->x,prev->y,prev->z);
	glColor3f(1.0,1.0,1.0);
	glVertex3d(next->x,next->y,next->z);
	glEnd();
}

} // namespace automatescheduler

} // namespace simulation

} // namespace sofa
