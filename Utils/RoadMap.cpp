/*
	LusionEngine- 2D game engine written in C++ with Lua interface.
	Copyright (C) 2006  Erik Engheim

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along
	with this program; if not, write to the Free Software Foundation, Inc.,
	51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
#include <Utils/RoadMap.h>
#include <Base/CircleShape.h>
#include <Engine.h>

#include <Geometry/IO.hpp>

#include <cassert>

#include "Timing.h"

using namespace std;

const real ACCURACY		=	0.00005;  // a measure for the accuracy during roadmap construction
const real MAX_DIST	  =	10.0;	// maximum distance a sample can move in search for nearest obstacle  
const real MAX_DIST_SQUARED	=	MAX_DIST * MAX_DIST;	// maximum distance squared

// Functions
/*!
  The shape \a group can be a composite of many shapes. This function
  will find all the primitive shapes stored in the composite, compute the minkowski sum
  of them, and return a new composite made up of a bounding volume hierarchy.
*/
Shape* minkowskiSum(const Shape* group) {
  ShapeIterator* i = group->iterator();
  for (i->first(); !i->done(); i->next()) {
    Shape* shape = i->value();
    if (!shape->isSimple())
      continue;
    
  }
  return 0; // DEBUG Fix this later
}


// Constructors
ClosestPointFinder::ClosestPointFinder(Shape* obstacles, const Rect2& bbox) : iBBox(bbox), iObstacles(obstacles) {
  assert(iObstacles != 0);
  iT = secondsPassed();
  iDt = 1.0;
}

// Operations
void ClosestPointFinder::discCollision(const Circle& circle, Shape* shape, Points2& points) 
{
  CircleShape c(circle);
  iPoints.clear();
  shape->collide(&c, secondsPassed(), iDt, this);
  points = iPoints;
}

bool ClosestPointFinder::execute(Shape* , Shape* , Points2& points, real, real) 
{
  iPoints = points;
  return true;
}

bool ClosestPointFinder::nearestObstacle(const Point2& c, Point2& point_result)
{
  bool is_collision = false;
  real radius = 0.0;
	
	Rect2 view = worldView();
  if (view.width() < view.height())
    radius = view.width();
  else 
    radius = view.height();
  
  real stepsize = radius;
  Points2 points;    
	while (stepsize > ACCURACY && radius > ACCURACY) {
    discCollision(Circle(c, radius), iObstacles, points);
    if (!points.empty()) {
      point_result = points[0];
      if (point_result == c)
        return true;  // Can't find closer point if point is on obstacle border
      
      is_collision = true;
      stepsize = stepsize*0.5;
      radius = radius-stepsize;            
    }
    else {
      if (is_collision)
        stepsize = stepsize*0.5;
      else 
        stepsize = stepsize*2;
      radius = radius+stepsize;      
    }
	}

  return is_collision;
}

bool
ClosestPointFinder::equidistantVertex(const Vector2& c1, const Vector2& c2, Vector2& c_v) {
  // init step vector
  Vector2 ds = (c2-c1)*0.5;
  
	// init first test position and both nearest obstacles
  c_v = c1 + ds;
  Vector2 c1_obst;
  if (!nearestObstacle(c1, c1_obst))
    return false;
  Vector2 c2_obst;
  if (!nearestObstacle(c_v, c2_obst))
    return false;

	while (ds.squaredLength() > ACCURACY) {
		ds *= 0.5;

		// update test position
		if ((c2_obst - c1_obst).squaredLength() < ACCURACY) 
      c_v += ds;   // c1 and c_v probably had the same obstacle closest
		else
      c_v -= ds;  // They had different closest obstacles, so move c_v close to c1
      
		 // update nearest obstacle to test position
    if (!nearestObstacle(c_v, c2_obst))
      return false;
  }

  return true;
}

bool
ClosestPointFinder::retractSample(const Vector2& c, Vector2& c_v) {
  Vector2 c_close;
  if (!nearestObstacle(c, c_close))
    return false;

  // No point in trying to retract if we are on border already
  if (c == c_close)
    return false;
    
	 // init Step vector ds (stepsize may not be larget than 0.5f * maxdist)
  Vector2 ds = c - c_close;
  
  if (ds.squaredLength() * 2.0 > MAX_DIST_SQUARED)
    ds = ds.unit()*0.5*MAX_DIST;
  
  // init second point to find second nearest obstacle
  Vector2 c1 = c + ds;

	// while the nearest obstacle to c' (c_near) is approx. equal to c_close, keep searching
  bool c1_found = true;
  Vector2 c_near;
  if (!nearestObstacle(c1, c_near))
    return false;
    
	while ((c_near-c_close).squaredLength() < ACCURACY) {
		c1 = c1 + ds;
    
    if (!nearestObstacle(c1, c_near))
      return false;

		// break when c' has moved too far
		if ((c1 - c_close).squaredLength() > MAX_DIST_SQUARED ||  !iBBox.inside(c1)) {
      c1_found = false;
      break;
		}
	}

  // search for equidistant vertex
	if (c1_found) {
	  if (!equidistantVertex(c, c1, c_v))
      return false;
  }
	else  		  
    c_v = c1 - ds;

  return true;
}