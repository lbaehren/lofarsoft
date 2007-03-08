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
#ifndef SOFA_COMPONENT_CONSTRAINT_BOXCONSTRAINT_INL
#define SOFA_COMPONENT_CONSTRAINT_BOXCONSTRAINT_INL

#if !defined(__GNUC__) || (__GNUC__ > 3 || (_GNUC__ == 3 && __GNUC_MINOR__ > 3))
#pragma once
#endif

//#include <sofa/component/constraint/FixedConstraint.inl>
#include <sofa/component/constraint/BoxConstraint.h>
#include <sofa/helper/gl/template.h>


namespace sofa
{

namespace component
{

namespace constraint
{

using std::cout;
using std::cerr;
using std::endl;

using namespace sofa::defaulttype;

template <class DataTypes>
BoxConstraint<DataTypes>::BoxConstraint()
        : Inherited(NULL)
        , box( dataField( &box, Vec6(0,0,0,1,1,1), "box", "DOFs in the box defined by xmin,ymin,zmin, xmax,ymax,zmax are fixed") )
        {}

template <class DataTypes>
void BoxConstraint<DataTypes>::init()
{
    Inherited::init();
    vector <unsigned> indices;

    const Vec6& b=box.getValue();
    this->mstate->getIndicesInSpace( indices, b[0],b[3],b[1],b[4],b[2],b[5] );
    for(int i = 0; i < (int)indices.size(); i++)
    {
        this->addConstraint(indices[i]);
    }
}



template <class DataTypes>
BoxConstraint<DataTypes>::~BoxConstraint()
{}


template <class DataTypes>
void BoxConstraint<DataTypes>::draw()
{
    if (!this->getContext()->getShowBehaviorModels())
        return;
    const VecCoord& x = *this->mstate->getX();
    glDisable (GL_LIGHTING);
    glPointSize(10);
    glColor4f (1,0.5,0.5,1);
    glBegin (GL_POINTS);
    const SetIndex& indices = this->f_indices.getValue();
    for (typename SetIndex::const_iterator it = indices.begin();
                it != indices.end();
                ++it)
        {
            gl::glVertexT(x[*it]);
        }
    glEnd();
    
    ///draw the constraint box
    const Vec6& b=box.getValue();
    const Real& Xmin=b[0]; 
    const Real& Xmax=b[3]; 
    const Real& Ymin=b[1]; 
    const Real& Ymax=b[4]; 
    const Real& Zmin=b[2]; 
    const Real& Zmax=b[5]; 
    glBegin(GL_LINES);
	glVertex3d(Xmin,Ymin,Zmin);
    glVertex3d(Xmin,Ymin,Zmax);
    glVertex3d(Xmin,Ymin,Zmin);
    glVertex3d(Xmax,Ymin,Zmin);
    glVertex3d(Xmin,Ymin,Zmin);
    glVertex3d(Xmin,Ymax,Zmin);
    glVertex3d(Xmin,Ymax,Zmin);
    glVertex3d(Xmax,Ymax,Zmin);
    glVertex3d(Xmin,Ymax,Zmin);
    glVertex3d(Xmin,Ymax,Zmax);
    glVertex3d(Xmin,Ymax,Zmax);
    glVertex3d(Xmin,Ymin,Zmax);
    glVertex3d(Xmin,Ymin,Zmax);
    glVertex3d(Xmax,Ymin,Zmax);
    glVertex3d(Xmax,Ymin,Zmax);
    glVertex3d(Xmax,Ymax,Zmax);
    glVertex3d(Xmax,Ymin,Zmax);
    glVertex3d(Xmax,Ymin,Zmin);
    glVertex3d(Xmin,Ymax,Zmax);
    glVertex3d(Xmax,Ymax,Zmax);
    glVertex3d(Xmax,Ymax,Zmin);
    glVertex3d(Xmax,Ymin,Zmin);
    glVertex3d(Xmax,Ymax,Zmin);
    glVertex3d(Xmax,Ymax,Zmax);
    glEnd();
}

template <class DataTypes>
bool BoxConstraint<DataTypes>::addBBox(double* minBBox, double* maxBBox)
{
	const Vec6& b=box.getValue();
	if (b[0] < minBBox[0]) minBBox[0] = b[0];
	if (b[1] < minBBox[1]) minBBox[1] = b[1];
	if (b[2] < minBBox[2]) minBBox[2] = b[2];
	if (b[3] > maxBBox[0]) maxBBox[0] = b[3];
	if (b[4] > maxBBox[1]) maxBBox[1] = b[4];
	if (b[5] > maxBBox[2]) maxBBox[2] = b[5];
	return true;
}

} // namespace constraint

} // namespace component

} // namespace sofa

#endif
