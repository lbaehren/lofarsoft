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
#include <sofa/component/constraint/FixedConstraint.inl>
#include <sofa/core/ObjectFactory.h>
#include <sofa/defaulttype/Vec3Types.h>
#include <sofa/defaulttype/RigidTypes.h>

namespace sofa
{

namespace component
{

namespace constraint
{

using namespace sofa::defaulttype;

SOFA_DECL_CLASS(FixedConstraint)

int FixedConstraintClass = core::RegisterObject("Attach given particles to their initial positions")
.add< FixedConstraint<Vec3dTypes> >()
.add< FixedConstraint<Vec3fTypes> >()
.add< FixedConstraint<RigidTypes> >()
;

template <>
void FixedConstraint<RigidTypes>::draw()
{
	const SetIndexArray & indices = f_indices.getValue();
	if (!getContext()->getShowBehaviorModels()) return;
	VecCoord& x = *mstate->getX();
	glDisable (GL_LIGHTING);
	glPointSize(10);
	glColor4f (1,0.5,0.5,1);
	glBegin (GL_POINTS);
	for (SetIndexArray::const_iterator it = indices.begin(); it != indices.end(); ++it)
	{
	   gl::glVertexT(x[0].getCenter());
	}
	glEnd();
}

template <>
void FixedConstraint<RigidTypes>::projectResponse(VecDeriv& res) 
{
  res[0] = Deriv();
}

template class FixedConstraint<Vec3dTypes>;
template class FixedConstraint<Vec3fTypes>;

} // namespace constraint

} // namespace component

} // namespace sofa

