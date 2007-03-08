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
#include <sofa/simulation/tree/XMLPrintAction.h>
#include <sofa/helper/Factory.h>
#include <sofa/simulation/tree/GNode.h>

namespace sofa
{

namespace simulation
{

namespace tree
{

template<class T>
void XMLPrintAction::processObject(T obj)
{
    for (int i=0;i<=level;i++)
        m_out << "\t";
    
    m_out << "<Object type=\"" << obj->getTypeName() << "\"\n";
    obj->xmlWriteFields( m_out, level+1 );
    
    for (int i=0;i<=level;i++)
        m_out << "\t";
    m_out << "/>" << std::endl;
}

template<class Seq>
void XMLPrintAction::processObjects(Seq& list)
{
	if (list.empty()) return;
	// the following line breaks the compilator on Visual2003
	//for_each<XMLPrintAction, Seq, typename Seq::value_type>(this, list, &XMLPrintAction::processObject<typename Seq::value_type>);
	for (typename Seq::iterator it = list.begin(); it != list.end(); ++it)
	{
		typename Seq::value_type obj = *it;
		this->processObject<typename Seq::value_type>(obj);
	}
}

Action::Result XMLPrintAction::processNodeTopDown(GNode* node)
{
	for (int i=0;i<level;i++)
		m_out << "\t";
	m_out << "<Node ";
        
        node->xmlWriteFields(m_out,level);
        
        for (int i=0;i<level;i++)
            m_out << "\t";
        m_out<<">"<<endl;
        
	++level;
        processObjects(node->object);
        
	
	return RESULT_CONTINUE;
}

void XMLPrintAction::processNodeBottomUp(GNode* /*node*/)
{
    --level;
    
    for (int i=0;i<level;i++)
        m_out << "\t";
    m_out << "</Node>"<<std::endl;
    
}

} // namespace tree

} // namespace simulation

} // namespace sofa

