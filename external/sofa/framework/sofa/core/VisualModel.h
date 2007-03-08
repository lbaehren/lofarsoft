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
#ifndef SOFA_CORE_VISUALMODEL_H
#define SOFA_CORE_VISUALMODEL_H

#include <sofa/core/objectmodel/BaseObject.h>

namespace sofa
{

namespace core
{

/*! \class VisualModel
  *  \brief An interface which all VisualModel inherit.
  *  \author Fonteneau Sylvere
  *  \version 0.1
  *  \date    02/22/2004
  *
  *	 <P> This Interface is used for the VisualModel. VisualModel is an interface implemented by all VisualModel.<BR>
  *  VisualModel is drawn so it has a function draw. The function updatePosition is used to compute the displacement of vertices (only in OBJModel)<BR>
  *  It's a generic interface that is bound by a Mapping with a BehaviorModel<BR>
  *  This Interface will change a lot when a SceneGraph Manager like OpenSceneGraph is used <BR></P>
  */
class VisualModel : public virtual objectmodel::BaseObject
{
public:
	virtual ~VisualModel() { }

	/*! \fn void initTextures() 
	 *  \brief initialize the textures
	 */
	virtual void initTextures() = 0;

	/*! \fn void draw() 
	 *  \brief display the VisualModel object.
	 */
	virtual void draw() = 0;
	
	/*! \fn void update()
	 *  \brief used to compute the displacement of the model
	 */
	virtual void update() = 0;
	
	/*! \fn void addBBox()
	 *  \brief used to add the bounding-box of this visual model to the
	 *  given bounding box in order to compute the scene bounding box or
	 *  cull hidden objects.
	 *
	 *  Return false if the visual model does not define any bounding box,
	 *  which should only be the case for "debug" objects, as this lack of
	 *  information might affect performances and leads to incorrect scene
	 *  bounding box.
	 */
	virtual bool addBBox(double* /*minBBox*/, double* /*maxBBox*/)
	{
		return false;
	}
	
	/// Append this mesh to an OBJ format stream.
	/// The number of vertices position, normal, and texture coordinates already written is given as parameters
	/// This method should update them
	virtual void exportOBJ(std::string /*name*/, std::ostream* /*out*/, std::ostream* /*mtl*/, int& /*vindex*/, int& /*nindex*/, int& /*tindex*/){};

};

} // namespace core

} // namespace sofa

#endif
