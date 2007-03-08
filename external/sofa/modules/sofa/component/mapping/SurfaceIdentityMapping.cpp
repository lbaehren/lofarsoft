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
#include <sofa/component/mapping/SurfaceIdentityMapping.h>
#include <sofa/core/componentmodel/behavior/MechanicalMapping.inl>
#include <sofa/defaulttype/Vec3Types.h>
#include <sofa/core/ObjectFactory.h>
#include <sofa/core/componentmodel/behavior/MappedModel.h>
#include <sofa/core/componentmodel/behavior/MechanicalState.h>



namespace sofa
{

namespace component
{

namespace mapping
{

using namespace core;

template <class BaseMapping>
SurfaceIdentityMapping<BaseMapping>::SurfaceIdentityMapping(In* from, Out* to)
: Inherit(from, to)
{
}

template <class BaseMapping>
SurfaceIdentityMapping<BaseMapping>::~SurfaceIdentityMapping()
{
}

template <class BaseMapping>
void SurfaceIdentityMapping<BaseMapping>::init()
{
	// We have to construct the correspondance index
	InVecCoord& in 	= *this->fromModel->getX();
        OutVecCoord& out 	= *this->toModel->getX();
	correspondingIndex.resize(out.size());

	// searching for the first corresponding point in the 'from' model (there might be several ones).
	for (unsigned int i = 0; i < out.size(); ++i)
	{
		bool found = false;
		for (unsigned int j = 0;  j < in.size() && !found; ++j )
		{
			if ( (out[i] - in[j]).norm() < 1.0e-10 )
			{
				correspondingIndex[i] = j;
				found = true;
			}
		}
		if (!found)
		{
			std::cerr<<"ERROR(SurfaceIdentityMapping): point "<<i<<"="<<out[i]<<" not found in input model."<<std::endl;
			correspondingIndex[i] = 0;
		}
	}
}

template <class BaseMapping>
void SurfaceIdentityMapping<BaseMapping>::apply( typename Out::VecCoord& out, const typename In::VecCoord& in )
{
	//out.resize(in.size());
	for(unsigned int i = 0; i < out.size(); ++i)
	{
		out[i] = in[ correspondingIndex[i] ];
	}
}

template <class BaseMapping>
void SurfaceIdentityMapping<BaseMapping>::applyJ( typename Out::VecDeriv& out, const typename In::VecDeriv& in )
{
	//out.resize(in.size());
	for(unsigned int i = 0; i < out.size(); ++i)
	{
		out[i] = in[ correspondingIndex[i] ];
	}
}

template <class BaseMapping>
void SurfaceIdentityMapping<BaseMapping>::applyJT( typename In::VecDeriv& out, const typename Out::VecDeriv& in )
{
	for(unsigned int i = 0; i < in.size(); ++i)
	{
		out[correspondingIndex[i]] += in[ i ];
	}
}


using namespace sofa::defaulttype;
using namespace core::componentmodel::behavior;

SOFA_DECL_CLASS(SurfaceIdentityMapping)


// Register in the Factory
int SurfaceIdentityMappingClass = core::RegisterObject("TODO-SurfaceIdentityMapping")
.add< SurfaceIdentityMapping< MechanicalMapping< MechanicalState<Vec3dTypes>, MechanicalState<Vec3dTypes> > > >()
.add< SurfaceIdentityMapping< MechanicalMapping< MechanicalState<Vec3fTypes>, MechanicalState<Vec3dTypes> > > >()
.add< SurfaceIdentityMapping< MechanicalMapping< MechanicalState<Vec3dTypes>, MechanicalState<Vec3fTypes> > > >()
.add< SurfaceIdentityMapping< MechanicalMapping< MechanicalState<Vec3fTypes>, MechanicalState<Vec3fTypes> > > >()
.add< SurfaceIdentityMapping< Mapping< MechanicalState<Vec3dTypes>, MappedModel<Vec3dTypes> > > >()
.add< SurfaceIdentityMapping< Mapping< MechanicalState<Vec3fTypes>, MappedModel<Vec3dTypes> > > >()
.add< SurfaceIdentityMapping< Mapping< MechanicalState<Vec3dTypes>, MappedModel<Vec3fTypes> > > >()
.add< SurfaceIdentityMapping< Mapping< MechanicalState<Vec3fTypes>, MappedModel<Vec3fTypes> > > >()
.add< SurfaceIdentityMapping< Mapping< MechanicalState<Vec3dTypes>, MappedModel<ExtVec3dTypes> > > >()
.add< SurfaceIdentityMapping< Mapping< MechanicalState<Vec3fTypes>, MappedModel<ExtVec3dTypes> > > >()
.add< SurfaceIdentityMapping< Mapping< MechanicalState<Vec3dTypes>, MappedModel<ExtVec3fTypes> > > >()
.add< SurfaceIdentityMapping< Mapping< MechanicalState<Vec3fTypes>, MappedModel<ExtVec3fTypes> > > >()
;


// Mech -> Mech
template class SurfaceIdentityMapping< MechanicalMapping< MechanicalState<Vec3dTypes>, MechanicalState<Vec3dTypes> > >;
template class SurfaceIdentityMapping< MechanicalMapping< MechanicalState<Vec3fTypes>, MechanicalState<Vec3fTypes> > >;
template class SurfaceIdentityMapping< MechanicalMapping< MechanicalState<Vec3dTypes>, MechanicalState<Vec3fTypes> > >;
template class SurfaceIdentityMapping< MechanicalMapping< MechanicalState<Vec3fTypes>, MechanicalState<Vec3dTypes> > >;

// Mech -> Mapped
template class SurfaceIdentityMapping< Mapping< MechanicalState<Vec3dTypes>, MappedModel<Vec3dTypes> > >;
template class SurfaceIdentityMapping< Mapping< MechanicalState<Vec3fTypes>, MappedModel<Vec3fTypes> > >;
template class SurfaceIdentityMapping< Mapping< MechanicalState<Vec3dTypes>, MappedModel<Vec3fTypes> > >;
template class SurfaceIdentityMapping< Mapping< MechanicalState<Vec3fTypes>, MappedModel<Vec3dTypes> > >;

// Mech -> ExtMapped
template class SurfaceIdentityMapping< Mapping< MechanicalState<Vec3dTypes>, MappedModel<ExtVec3dTypes> > >;
template class SurfaceIdentityMapping< Mapping< MechanicalState<Vec3fTypes>, MappedModel<ExtVec3fTypes> > >;
template class SurfaceIdentityMapping< Mapping< MechanicalState<Vec3dTypes>, MappedModel<ExtVec3fTypes> > >;
template class SurfaceIdentityMapping< Mapping< MechanicalState<Vec3fTypes>, MappedModel<ExtVec3dTypes> > >; 


} // namespace mapping

} // namespace component

} // namespace sofa

