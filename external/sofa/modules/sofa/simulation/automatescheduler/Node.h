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
#ifndef SOFA_SIMULATION_AUTOMATESCHEDULER_NODE_H
#define SOFA_SIMULATION_AUTOMATESCHEDULER_NODE_H

#include <vector>
#include <sofa/simulation/automatescheduler/NodeGFX.h>

namespace sofa
{

namespace simulation
{

namespace automatescheduler
{

class StateMachine;
class Edge;


//////////
// Node //
//////////

class Node : public NodeGFX
{
	public:
		std::vector<Edge*> tabOutputs;  

	public:
		Node();
		Node(char *s, double x, double y);
		virtual ~Node () {};

		virtual void onEnter(StateMachine *);
		virtual void onExit(StateMachine *);
		virtual void execute(char *src);
                
		void update(StateMachine *sm);
		void addOutput(Edge *);
};

//////////////
// NodeExec //
//////////////

class NodeExec : public Node
{
	void (*myFuncPtr1)(void);
	void (*myFuncPtr2)(char *);
	char param[100];

public:

	NodeExec(char *s, double x, double y) : Node(s,x,y){};

	void setAction( void (*func)(void) )
	{
		myFuncPtr1 = func;

		myFuncPtr2 = NULL;
	}

	void setAction( void (*func)(char *), char *_param)
	{
		myFuncPtr2 = func;
		strcpy(param, _param);

		myFuncPtr1 = NULL;
	}

	void execute(char *)
	{
		if (myFuncPtr1 == NULL)
		{
			myFuncPtr2(param);
		}
		else
		{
			myFuncPtr1();
		}
	}
};

//////////////
// NodePush //
//////////////

class NodePush : public Node
{
	Node *src;

public:

	NodePush(char *s, double x, double y, Node *sr) : Node(s,x,y) 
	{
		src = sr;
	}

	void execute(char *){} 

	void onExit(StateMachine *sm);

	void draw(){};
};

/////////////////////
// NodePushDisplay //
/////////////////////

class NodePushDisplay : public Node
{
	Node *src;

public:

	NodePushDisplay(char *s, double x, double y, Node *sr) : Node(s,x,y) 
	{
		src = sr;
	}

	void execute(char *){} 

	void onExit(StateMachine *sm);

	void draw(){};

};

////////////////////
// NodePushPeriph //
////////////////////

class NodePushPeriph : public Node
{
	Node *src;

public:

	NodePushPeriph(char *s, double x, double y, Node *sr) : Node(s,x,y) 
	{
		src = sr;
	}

	void execute(char *){} 

	void onExit(StateMachine *sm);

	void draw(){};

};

} // namespace automatescheduler

} // namespace simulation

} // namespace sofa

#endif
