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
#ifndef SOFA_SIMULATION_AUTOMATESCHEDULER_OBJSUBAUTOMATE_H
#define SOFA_SIMULATION_AUTOMATESCHEDULER_OBJSUBAUTOMATE_H

#include <vector>
#include <string>

namespace sofa
{

namespace simulation
{

namespace automatescheduler
{

class Node;

/////////////////////////////////
// ObjSubAutomateNodeAttribute //
/////////////////////////////////


class ObjSubAutomateNodeAttribute
{
public :

	ObjSubAutomateNodeAttribute(){};

	virtual void execute(void) = 0;
	virtual ~ObjSubAutomateNodeAttribute(){};
};


/////////////////////////////////
// ObjSubAutomateCondAttribute //
/////////////////////////////////


class ObjSubAutomateCondAttribute
{
public :

	ObjSubAutomateCondAttribute(){};

	virtual bool eval(void) = 0;
	virtual ~ObjSubAutomateCondAttribute(){};
};


class ObjSubAutomateNodeExec;
class ObjSubAutomateCondExec;

////////////////////
// ObjSubAutomate //
////////////////////

class ObjSubAutomate
{
public :

	ObjSubAutomate(double _xPos);

	ObjSubAutomateNodeExec *entry;
	
	Node *getExitNode(void);

	void resetCounter(void);

	// Mise a jour du nombre de patterns a executer avant
	// de sortir du sous automate
	void setExitCondition(unsigned int _maxCounter);

	// Nombre de patterns dans le sous automate
	unsigned int nbPatterns;

	// Nombre de Patterns a executer
	int nbPatterns2Execute;

	// Condition d'execution d'une pattern
	bool execTest(void);

	// Court Circuit du sous Automate
	void gotoExit(void);

	virtual ~ObjSubAutomate() {};

protected :

	ObjSubAutomateNodeExec *incr;
	double xPos;

private :

	Node *exit;

	ObjSubAutomateCondExec *exitCond;
	ObjSubAutomateCondExec *gotoExitCond;

	// Compteur des branches executees
	unsigned int counter;

	// Parametrisation du sous automate
	virtual void onEnter(void) = 0;
	virtual void addPattern(void) = 0;

	void incrCounter(void);

	// Test sur le Compteur pour declencher la sortie du sous automate
	bool testCounter(void);
	unsigned int maxCounter;

	// Court Circuit du sous Automate
	bool enableExit;
	bool gotoExitTest(void);
};


//////////////////////////////////////////
// MonoFrequencyObjSubAutomateAttribute //
//////////////////////////////////////////


class MonoFrequencyObjSubAutomateAttribute : public ObjSubAutomateNodeAttribute
{
	typedef void(*updateFunc)(std::string);

public :

	MonoFrequencyObjSubAutomateAttribute(){};
	MonoFrequencyObjSubAutomateAttribute(updateFunc _functionPtr, std::string _param);

	updateFunc getFunction();
	std::string getParameters();

	void execute(void){
		functionPtr(param);
	};

private :
	
	updateFunc functionPtr;
	std::string param;
};


/////////////////////////////////
// MonoFrequencyObjSubAutomate //
/////////////////////////////////

class MonoFrequencyObjSubAutomateAttribute;
template <class T> class ObjSubAutomateAttributeNodeExec;

class MonoFrequencyObjSubAutomate : public ObjSubAutomate
{
public :

	MonoFrequencyObjSubAutomate(double _xPos);

	void setActionList(std::vector<MonoFrequencyObjSubAutomateAttribute *>);

	const std::vector<MonoFrequencyObjSubAutomateAttribute *>& getActionList() { return actionList; }

private :

	std::vector<MonoFrequencyObjSubAutomateAttribute *> actionList;
	std::vector<ObjSubAutomateAttributeNodeExec<MonoFrequencyObjSubAutomateAttribute> *> tabNode;

	void onEnter(void);
	void addPattern(void);
};


////////////////////////////////////////////
// MultiFrequencyObjSubAutomateAttributes //
////////////////////////////////////////////


class MultiFrequencyCondAttribute : public ObjSubAutomateCondAttribute
{
	typedef bool(*evalFunc)(double);

public :

	MultiFrequencyCondAttribute(){};
	MultiFrequencyCondAttribute(evalFunc _functionPtr, double _param);

	evalFunc getFunction();
	double getParameters();

	bool eval(void)
	{
		return functionPtr(param);
	}

private :
	
	evalFunc functionPtr;
	double param;
};


class MultiFrequencyUpdateAttribute : public ObjSubAutomateNodeAttribute
{
	typedef void(*updateFunc)(char *);

public :

	MultiFrequencyUpdateAttribute(){};
	MultiFrequencyUpdateAttribute(updateFunc _functionPtr, char* _param);

	updateFunc getFunction();
	char *getParameters();

	void execute(void)
	{
		functionPtr(param);
	};

private :
	
	updateFunc functionPtr;
	char param[100];
};


//////////////////////////////////
// MultiFrequencyObjSubAutomate //
//////////////////////////////////

template <class T> class ObjSubAutomateAttributeCondExec;
class MultiFrequencyCondAttribute;
class MultiFrequencyUpdateAttribute;

class MultiFrequencyObjSubAutomate : public ObjSubAutomate
{
public :

	MultiFrequencyObjSubAutomate(double _xPos);

	void setEndOfUpdateCondList(std::vector<MultiFrequencyCondAttribute *>);
	void setUpdateList(std::vector<MultiFrequencyUpdateAttribute *>);
	void setUpdateCondList(std::vector<MultiFrequencyCondAttribute *>);

private :

	std::vector<MultiFrequencyCondAttribute *> endOfUpdateCondList;
	std::vector<MultiFrequencyUpdateAttribute *> updateList;
	std::vector<MultiFrequencyCondAttribute *> updateCondList;

	std::vector<ObjSubAutomateAttributeCondExec<MultiFrequencyCondAttribute> *> tabEndOfUpdateCond;
	std::vector<ObjSubAutomateAttributeNodeExec<MultiFrequencyUpdateAttribute> *> tabNodes;
	std::vector<ObjSubAutomateAttributeCondExec<MultiFrequencyCondAttribute> *> tabUpdateCond;

	void onEnter(void);
	void addPattern(void);
};

} // namespace automatescheduler

} // namespace simulation

} // namespace sofa

#endif
