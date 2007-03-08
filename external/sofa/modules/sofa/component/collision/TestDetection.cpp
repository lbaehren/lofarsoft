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
#include <sofa/component/collision/TestDetection.h>
#include <sofa/component/collision/Sphere.h>
#include <sofa/component/collision/Triangle.h>
#include <sofa/component/collision/Line.h>
#include <sofa/component/collision/Point.h>
#include <sofa/helper/FnDispatcher.h>
#include <sofa/core/ObjectFactory.h>
#include <sofa/simulation/tree/GNode.h>
#include <map>
#include <queue>
#include <stack>
#include <GL/gl.h>
#include <GL/glut.h>



/* for debugging the collision method */
#ifdef _WIN32
#include <windows.h>
#endif

namespace sofa
{

namespace component
{

namespace collision
{

SOFA_DECL_CLASS(TestDetection)

    int TestDetectionClass = core::RegisterObject("TODO-TestDetectionClass")
.add< TestDetection >()
;

using namespace sofa::defaulttype;
using namespace core;
using std::endl; // added for testing 
using std::cout; // added for testing

using namespace core::objectmodel;

TestDetection::TestDetection()
: bDraw(dataField(&bDraw, false, "draw", "enable/disable display of results"))
{
}

void TestDetection::addCollisionModel(CollisionModel *cm)
{	
	// Check if the collision model is empty
	if (cm->empty())
		return;
	for (std::vector<CollisionModel*>::iterator it = collisionModels.begin(); it != collisionModels.end(); ++it)
	{
		CollisionModel* cm2 = *it;
		// Check if the collision models are movables
		if (cm->isStatic() && cm2->isStatic())
			continue;
		// Check if there exists a manner to check the collision between the collision models
		if (!cm->canCollideWith(cm2))
			continue;

		// Find the required type of intersection between the different collision models.
		// So far: cube-cube, sphere-sphere, sphere-ray, etc.
		// Define the intersection function to use
		// findIntersector is based on a map.
		core::componentmodel::collision::ElementIntersector* intersector = intersectionMethod->findIntersector(cm, cm2);

		if (intersector == NULL)
			continue;

		if (intersector->canIntersect(cm, cm2))	
		{ // If colAdd the colliding models pair to a list so they can be tested in the narrow phase.
			cmPairs.push_back(std::make_pair(cm, cm2));
		}		

	}
	collisionModels.push_back(cm);
}

void TestDetection::addCollisionPair(const std::pair<CollisionModel*, CollisionModel*>& cmPair)
{

	simulation::tree::GNode* node = dynamic_cast<simulation::tree::GNode*>(getContext());
	if (node && !node->getLogTime()) node=NULL; // Only use node for time logging
	//simulation::tree::GNode::ctime_t t0=0, t=0;
	//simulation::tree::GNode::ctime_t ft=0;
	
	// Collision Models
	CollisionModel *cm1 = cmPair.first; 
	CollisionModel *cm2 = cmPair.second;

	typedef  std::pair<CollisionElementIterator,CollisionElementIterator>  TestPair;
	TestPair pair; 
	// List of colliding pairs to be tested
	std::queue< TestPair > pairList;

	// Root nodes of the colliding models 
	pair.first = cm1->getFirst()->begin(); 
	pair.second = cm2->getFirst()->begin();
	
	//// Find the intersector to be used.
	core::componentmodel::collision::ElementIntersector* intersector = intersectionMethod->findIntersector(cm1, cm2);

	//// Find the intersector to be used.
	CollisionModel *finalcm1 = cm1->getLast();
	CollisionModel *finalcm2 = cm2->getLast();
	core::componentmodel::collision::ElementIntersector* finalIntersector = intersectionMethod->findIntersector(finalcm1, finalcm2);

	if (intersector == NULL || finalIntersector == NULL) {
		//cout << "couldn't find intersector" << endl;
		return;
	}

	pairList.push( pair );  
    
	while ( pairList.size() > 0 )
	{
		pair = pairList.front();
		pairList.pop();
			
		//Get intersector at each time since the order of the elements in the 
		//pair might change at each iteration
		//intersector = intersectionMethod->findIntersector(
		//	pair.first.getCollisionModel(), pair.second.getCollisionModel()) ;

		//if (intersector == NULL) 
		//cout << "couldn't find intersector" << endl;

		//if (intersector->canIntersect( pair.first,pair.second))
		if ((pair.first.getCollisionModel()->getFirst()==cm1)?intersector->canIntersect( pair.first,pair.second):intersector->canIntersect( pair.second,pair.first))
		{
			if ( pair.second.isLeaf() ) { 
				if ( pair.first.isLeaf() ) { 
					// The elements of the pair are leaves of the given collision model. 
					// Now, we need to store the children of the leaves, i.e. the primitives.
					// For this, we use "getExternalChildren"
					// Sphere tree model is the only that does not have associated external children.
					// Its primivives are the leaves of the collision model.
					// On the other hand, cube model, for example, has associated triangles
					CollisionElementIterator child1Offirst = pair.first.getExternalChildren().first;
					CollisionElementIterator child2Offirst = pair.first.getExternalChildren().second;

					CollisionElementIterator child1Ofsecond = pair.second.getExternalChildren().first;
					CollisionElementIterator child2Ofsecond = pair.second.getExternalChildren().second;

					if ( child1Offirst.valid() )
					{
						if (child1Ofsecond.valid() )
						{
							//cout << "introduce the children of both " << endl;
							if (child1Offirst.getCollisionModel()==finalcm1)
							{
								for (CollisionElementIterator it1 = child1Offirst; it1 != child2Offirst; ++it1) 
									for (CollisionElementIterator it2 = child1Ofsecond; it2 != child2Ofsecond; ++it2) 
										if ( finalIntersector->canIntersect( it1, it2 ) )
											elemPairs.push_back( std::make_pair( it1, it2 ) );
							}
							else
							{
								for (CollisionElementIterator it1 = child1Offirst; it1 != child2Offirst; ++it1) 
									for (CollisionElementIterator it2 = child1Ofsecond; it2 != child2Ofsecond; ++it2) 
										if ( finalIntersector->canIntersect( it2, it1 ) )
											elemPairs.push_back( std::make_pair( it2, it1 ) );
							}
						}
						else{
							//cout << "introduce the children of pairFirst only " << endl;
							if (child1Offirst.getCollisionModel()==finalcm1)
							{
								for (CollisionElementIterator it1 = child1Offirst; it1 != child2Offirst; ++it1) 
									if ( finalIntersector->canIntersect( it1, pair.second ) )
										elemPairs.push_back( std::make_pair( it1, pair.second ) );
							}
							else
							{
								for (CollisionElementIterator it1 = child1Offirst; it1 != child2Offirst; ++it1) 
									if ( finalIntersector->canIntersect( pair.second, it1 ) )
										elemPairs.push_back( std::make_pair( pair.second, it1 ) );
							}
						}
					}
					else if (child1Ofsecond.valid() )
					{
						//no test to test child1Offirst here
						//if (child1Offirst.valid()) {
						//	//cout << " introduce the children of both " << endl;
						//}
						//else {
						if (child1Ofsecond.getCollisionModel()==finalcm1)
						{
							//cout << " introduce the children of pair.second only " << endl;
							for (CollisionElementIterator it1 = child1Ofsecond; it1 != child2Ofsecond; ++it1)
								if ( finalIntersector->canIntersect( it1, pair.first ) )
									elemPairs.push_back( std::make_pair( it1, pair.first ) );
						}
						else
						{
							//cout << " introduce the children of pair.second only " << endl;
							for (CollisionElementIterator it1 = child1Ofsecond; it1 != child2Ofsecond; ++it1)
								if ( finalIntersector->canIntersect( pair.first, it1 ) )
									elemPairs.push_back( std::make_pair( pair.first, it1 ) );
						}
					}
					else elemPairs.push_back( pair ); 
					
				} else { //Do this, in case one is leaf and the other not			 
					CollisionElementIterator begin = pair.first.getInternalChildren().first;
					CollisionElementIterator end = pair.first.getInternalChildren().second;															

					if ( !(begin.valid() && end.valid())  ) {  // Do this in case begin or end
																	// are external children
						CollisionElementIterator begin = pair.first.getExternalChildren().first;
						CollisionElementIterator end = pair.first.getExternalChildren().second;										
					
						for ( CollisionElementIterator it1 = begin; it1 != end; ++it1) {
							pairList.push( std::make_pair( pair.second, it1));
						}
					} // for external children

					else {
						for ( CollisionElementIterator it1 = begin; it1 != end; ++it1) {
							pairList.push( std::make_pair( pair.second, it1 ) );						
						}
					}
				}

			} else {
					CollisionElementIterator begin = pair.second.getInternalChildren().first;
					CollisionElementIterator end = pair.second.getInternalChildren().second;
					
					for ( CollisionElementIterator it2 = begin; it2 != end; ++it2) {
						pairList.push( std::make_pair( it2, pair.first ) );
					}				
			}
		} // end of testing for collision
	} // end while
 // 
	
}

void TestDetection::draw()
{
	if (!bDraw.getValue()) return;
	
	if (!elemPairs.empty())
	{
		glDisable(GL_LIGHTING);
		glColor3f(1.0, 0.0, 1.0);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glLineWidth(3);
		glPointSize(5);
		std::vector<std::pair<CollisionElementIterator, CollisionElementIterator> >::iterator it = elemPairs.begin();
		std::vector<std::pair<CollisionElementIterator, CollisionElementIterator> >::iterator itEnd = elemPairs.end();
		//std::cout << "Size : " << elemPairs.size() << std::endl;
		for (; it != itEnd; it++)
		{	
			it->first.draw();
			it->second.draw();
		}
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glLineWidth(1);
		glPointSize(1);
	}
}





} // namespace collision

} // namespace component

} // namespace sofa

