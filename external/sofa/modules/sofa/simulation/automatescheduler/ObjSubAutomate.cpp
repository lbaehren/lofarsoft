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
#include <math.h>

#include "Edge.h"
#include "sofa/simulation/automatescheduler/Node.h"
#include "ObjSubAutomateNodeExec.h"
#include "ObjSubAutomateCondExec.h"
#include "ObjSubAutomateAttributeNodeExec.h"
#include "ObjSubAutomateAttributeCondExec.h"
#include "ObjSubAutomate.h"

namespace sofa
{

namespace simulation
{

namespace automatescheduler
{

//////////////////////////////////////////
// MonoFrequencyObjSubAutomateAttribute //
//////////////////////////////////////////


MonoFrequencyObjSubAutomateAttribute::MonoFrequencyObjSubAutomateAttribute(updateFunc _functionPtr, std::string _param)
{
	functionPtr = _functionPtr;
	param = _param;
}

MonoFrequencyObjSubAutomateAttribute::updateFunc MonoFrequencyObjSubAutomateAttribute::getFunction()
{
	return functionPtr;
}

std::string MonoFrequencyObjSubAutomateAttribute::getParameters()
{
	return param;
}


///////////////////////////////////
// MultiFrequencyUpdateAttribute //
///////////////////////////////////


MultiFrequencyUpdateAttribute::MultiFrequencyUpdateAttribute(updateFunc _functionPtr, char* _param)
{
	functionPtr = _functionPtr;
	strcpy(param, _param);
}

MultiFrequencyUpdateAttribute::updateFunc MultiFrequencyUpdateAttribute::getFunction()
{
	return functionPtr;
}

char* MultiFrequencyUpdateAttribute::getParameters()
{
	return param;
}


/////////////////////////////////
// MultiFrequencyCondAttribute //
/////////////////////////////////


MultiFrequencyCondAttribute::MultiFrequencyCondAttribute(evalFunc _functionPtr, double _param)
{
	functionPtr = _functionPtr;
	param = _param;
}

MultiFrequencyCondAttribute::evalFunc MultiFrequencyCondAttribute::getFunction()
{
	return functionPtr;
}

double MultiFrequencyCondAttribute::getParameters()
{
	return param;
}


////////////////////
// ObjSubAutomate //
////////////////////


ObjSubAutomate::ObjSubAutomate(double _xPos)
{
	xPos = _xPos;
	
   	entry = new ObjSubAutomateNodeExec(this, "entry", xPos, 0);
	entry->setAction(&ObjSubAutomate::onEnter);

	incr = new ObjSubAutomateNodeExec(this, "incr", xPos + 2, 0);
	incr->setAction(&ObjSubAutomate::incrCounter);

	exit = new Node("exit", xPos + 3, 0);

	exitCond = new ObjSubAutomateCondExec(this);
	exitCond->setAction(&ObjSubAutomate::testCounter);
	incr->addOutput(new Edge( exitCond, incr, exit ));

	gotoExitCond = new ObjSubAutomateCondExec(this);
	gotoExitCond->setAction(&ObjSubAutomate::gotoExitTest);
	entry->addOutput( new Edge( gotoExitCond, entry, exit ));

	maxCounter = 0;
	nbPatterns = 0;
	nbPatterns2Execute = 0;
	enableExit = false;
}

Node *ObjSubAutomate::getExitNode(void)
{
	return exit;
}


void ObjSubAutomate::incrCounter(void)
{
	counter++;
}

void ObjSubAutomate::resetCounter(void)
{
	counter = 0;
}

bool ObjSubAutomate::testCounter(void)
{
	return counter == maxCounter;
}

void ObjSubAutomate::setExitCondition(unsigned int _maxCounter)
{
	maxCounter = _maxCounter;
}

bool ObjSubAutomate::execTest(void)
{
	return (nbPatterns2Execute-- > 0);
}

void ObjSubAutomate::gotoExit(void)
{
	enableExit = true;
}

bool ObjSubAutomate::gotoExitTest(void)
{
	if (enableExit)
	{
		enableExit = false;
		return true;
	}
	else
	{
		return false;
	}
}


/////////////////////////////////
// MonoFrequencyObjSubAutomate //
/////////////////////////////////


MonoFrequencyObjSubAutomate::MonoFrequencyObjSubAutomate(double _xPos)
	: ObjSubAutomate(_xPos)
{

}

void MonoFrequencyObjSubAutomate::onEnter(void)
{	
	resetCounter();
	int i;

	nbPatterns2Execute = actionList.size();

	if (nbPatterns2Execute > 0)
	{
		// Creation de Patterns
		int patternsNeaded = nbPatterns2Execute - nbPatterns;

		if (patternsNeaded > 0)
		{
			// Allocation de nouvelles patterns
			for (i = 0; i < patternsNeaded; i++)
				addPattern();

			nbPatterns += patternsNeaded;
		}
		
		// Attribution des taches
		for (i = 0; i < nbPatterns2Execute; i++)
		{
			MonoFrequencyObjSubAutomateAttribute *myAttrib = (tabNode[i]->getExecList())->at(i);
			tabNode[i]->addAttribute(*myAttrib);
		}
	}
	else
	{
		gotoExit();
	}
}

void MonoFrequencyObjSubAutomate::addPattern()
{
	// Ajout d'une "Branche" dans le Conteneur

	// Variables statiques temporaires utiles pour l'affichage
	static double y = 0;
	static int objNumber = 0;

	char objId[64];
	sprintf(objId, "obj_%d", objNumber);
	
	ObjSubAutomateAttributeNodeExec<MonoFrequencyObjSubAutomateAttribute> *obj =
		new ObjSubAutomateAttributeNodeExec<MonoFrequencyObjSubAutomateAttribute>(actionList, objId, xPos + 1, y);

	tabNode.push_back(obj);

	NodePush *pushObj = new NodePush(objId, xPos + 1, y, obj);

	ObjSubAutomateCondExec *execCond = new ObjSubAutomateCondExec(this);
	execCond->setAction(&ObjSubAutomate::execTest);
	
	entry->addOutput(new Edge(execCond, entry, pushObj));
	obj->addOutput(new Edge(new Cond(), obj, incr ));

	y += 1.0;
	objNumber++;
}

void MonoFrequencyObjSubAutomate::setActionList(std::vector<MonoFrequencyObjSubAutomateAttribute *> _list)
{
	actionList.assign(_list.begin(), _list.end());
}


///////////////////////////////////////////
// MultiFrequencyObjSubAutomateAttribute //
///////////////////////////////////////////


MultiFrequencyObjSubAutomate::MultiFrequencyObjSubAutomate(double _xPos)
	: ObjSubAutomate(_xPos)
{

}

void MultiFrequencyObjSubAutomate::setEndOfUpdateCondList(std::vector<MultiFrequencyCondAttribute *> _list)
{
	endOfUpdateCondList.assign(_list.begin(), _list.end());
}

void MultiFrequencyObjSubAutomate::setUpdateList(std::vector<MultiFrequencyUpdateAttribute *> _list)
{
	updateList.assign(_list.begin(), _list.end());
}

void MultiFrequencyObjSubAutomate::setUpdateCondList(std::vector<MultiFrequencyCondAttribute *> _list)
{
	updateCondList.assign(_list.begin(), _list.end());
}

void MultiFrequencyObjSubAutomate::onEnter(void)
{
//	resetCounter();
	int i;

	nbPatterns2Execute = updateList.size();
	
	if ((endOfUpdateCondList.size() != updateList.size()) ||
		(updateCondList.size() != updateList.size()) ||
		(updateCondList.size() != endOfUpdateCondList.size()))
	{
		std::cerr << "Wrong Parameters in MultiFrequencySubAutomate Entry\n";
	}

	if (nbPatterns2Execute > 0)
	{
		// Creation de Patterns
		int patternsNeaded = nbPatterns2Execute - nbPatterns;

		if (patternsNeaded > 0)
		{
			// Allocation de nouvelles patterns
			for (i = 0; i < patternsNeaded; i++)
				addPattern();

			nbPatterns += patternsNeaded;
		}
		
		// Attribution des taches
		for (i = 0; i < nbPatterns2Execute; i++)
		{
			MultiFrequencyCondAttribute *attrib1 = (tabEndOfUpdateCond[i]->getExecList())->at(i);
			tabEndOfUpdateCond[i]->addAttribute(*attrib1);

			MultiFrequencyUpdateAttribute *attrib2 = (tabNodes[i]->getExecList())->at(i);
			tabNodes[i]->addAttribute(*attrib2);

			MultiFrequencyCondAttribute *attrib3 = (tabUpdateCond[i]->getExecList())->at(i);
			tabUpdateCond[i]->addAttribute(*attrib3);
		}
	}
	else
	{
		gotoExit();
	}
}

void MultiFrequencyObjSubAutomate::addPattern()
{
	// Ajout d'une "Branche" dans le Conteneur

	// Variables statiques temporaires utiles pour l'affichage
	static double y = 0;
	static int objNumber = 0;

	char objId[64];
	sprintf(objId, "obj_%d", objNumber);

	ObjSubAutomateAttributeCondExec<MultiFrequencyCondAttribute> *endOfUpdateCond =
		new ObjSubAutomateAttributeCondExec<MultiFrequencyCondAttribute>(endOfUpdateCondList);

	ObjSubAutomateAttributeNodeExec<MultiFrequencyUpdateAttribute> *obj =
		new ObjSubAutomateAttributeNodeExec<MultiFrequencyUpdateAttribute>(updateList, objId, xPos + 1, y);

	ObjSubAutomateAttributeCondExec<MultiFrequencyCondAttribute> *updateCond =
		new ObjSubAutomateAttributeCondExec<MultiFrequencyCondAttribute>(updateCondList);

	tabEndOfUpdateCond.push_back(endOfUpdateCond);
	tabNodes.push_back(obj);
	tabUpdateCond.push_back(updateCond);

	NodePush *pushObj = new NodePush("", xPos + 1, y, obj);

	ObjSubAutomateCondExec *execCond = new ObjSubAutomateCondExec(this);
	execCond->setAction(&ObjSubAutomate::execTest);
	
	entry->addOutput(new Edge(execCond, entry, pushObj));

	obj->addOutput(new Edge(updateCond, obj, pushObj ));
	obj->addOutput(new Edge(endOfUpdateCond, obj, incr ));

	y += 1;
	objNumber++;
}

} // namespace automatescheduler

} // namespace simulation

} // namespace sofa
