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

#include "Base/Group.h"
#include "Timing.h"
#include "Base/Action.h"
#include "Base/ShapeIterator.h"

#include <iostream>
#include <cassert>

#include <functional>
#include <algorithm>

using namespace std;

/*!
    \class Group Group.h
    \brief Collection for Shapes. 

    Is in many ways similar to ShapeGroup. However ShapeGroup is not mutable
    Once it is created Shapes can't be added or removed. A Group on the otherhand
    can have Shapes added and removed at any time. The downside of this is that
    the structure can be as efficient in handling collision, because one can't create
    an optimized hierarchical structure.
*/

// Gloal functions
static Group gRenderGroup;

Group* renderGroup()
{
  return &gRenderGroup;
}

// Constructors
Group::Group() : iCurShape(0)
{

}

Group::~Group()
{
  clear();
  // cout << hex << "0x" << (int)this << " group removed" << endl;  // DEBUG  
}

// Accessors
std::string
Group::typeName() const  
{ 
  return "Group"; 
}

int Group::noShapes() const
{
  return iShapes.size();
}

ShapeIterator* Group::iterator() const
{
  ShapeIterator* itr = new SetShapeIterator(iShapes);
  itr->autorelease();
  return itr;
}

Rect2 Group::boundingBox() const
{
  return iBBox;
}

// Request
bool Group::contains(Shape* shape) const
{
  return iShapes.find(shape) != iShapes.end();
}

bool Group::isSimple() const
{
  return false;
}

// Operations
void Group::addKid(Shape* shape)
{
  assert(shape != 0);
  
  if (!contains(shape)) {
    shape->retain();
    iCurShape = iShapes.insert(shape).first;
    if (iShapes.size() == 0)
      iBBox = shape->boundingBox();
    else
      iBBox.surround(shape->boundingBox());
    shape->addListener(this); // Be notified of deletes
  }
}

void Group::removeKid(Shape* shape)
{
  assert(shape != 0);
  
  if (contains(shape)) {
    if (*iCurShape == shape) nextShape();
    iShapes.erase(shape);
    shape->release();
    shape->removeListener(this);    
  }
}

/*!
  Updates all child shapes as well as updateting the boundingbox for group
*/
void Group::update(real start_time, real delta_time)
{
  if (iShapes.size() == 0)
    return;
    
  set<Shape*>::iterator shape = iShapes.begin();
  (*shape)->update(start_time, delta_time);
  Rect2 bbox = (*shape)->boundingBox();
 
  for (++shape; shape != iShapes.end(); ++shape) {
    (*shape)->update(start_time, delta_time);
    bbox.surround((*shape)->boundingBox());
  }
  iBBox = bbox;
}

/*!
  Do a round robin calling doPlanning on one shape at a time.
*/
void Group::doPlanning(real start_time, real delta_time)
{
  Shape* shape = nextShape();
  if (shape)
    shape->doPlanning(start_time, delta_time);
}

struct CompareShapeDepth : public binary_function<Shape*, Shape*, bool>
{
  bool operator()(const Shape* first, const Shape* second) const {
    return first->depth() > second->depth();
  }
};

/*!
  Draws all shapes inside or intersecting rectangle 
  \a r with the shapes of lowest depth drawn on top of those
  of higher depth.
*/
void Group::draw(const Rect2& r) const
{
  vector<Shape*> shapes(iShapes.begin(), iShapes.end());
  sort(shapes.begin(), shapes.end(), CompareShapeDepth());
  typedef vector<Shape*>::iterator iterator;
  
  for (iterator shape = shapes.begin(); shape != shapes.end(); ++shape) {
    (*shape)->draw(r);
  }  
}

void Group::clear()
{
  // Stop listening to shapes
  for_each(iShapes.begin(), iShapes.end(),
    bind2nd(mem_fun(&Shape::removeListener),
            this));
  for_each(iShapes.begin(), iShapes.end(),
    mem_fun(&SharedObject::release));  
  iShapes.clear();
}

/*! 
  Does a round robin for shapes in group. That means at each call it will return
  a different shape in the group, until all the shapes have been returned at which
  point it start returning the first shapes again.
*/
Shape* Group::nextShape()
{
  if (iShapes.empty())
    return 0;
  Shape* s = *iCurShape;
  if (++iCurShape == iShapes.end())
    iCurShape = iShapes.begin();
  return s;
}
  

bool Group::collide(Shape* other, real t, real dt, CollisionAction* command)
{
  bool is_col = false;
  set<Shape*>::iterator it;  
  for (it = iShapes.begin(); it != iShapes.end(); ++it) {
    Shape* shape = *it;
    if (secondsPassed() > t+dt)
      break;
      
    if (shape == other)
      continue;

    if(shape->collide(other, t, dt, command))
      is_col = true;
  }    
  return is_col;
}

bool Group::inside(const Point2& p, real t, real dt, Action* command)
{
  bool is_col = false;
  set<Shape*>::iterator it;  
  for (it = iShapes.begin(); it != iShapes.end(); ++it) {
    Shape* shape = *it;

    if (secondsPassed() > t+dt)
      break;
      
    if(shape->inside(p, t, dt, command))
      is_col = true;
  }    
  return is_col;  
}

// Event handling
void Group::shapeDestroyed(Shape* shape) 
{
  if (*iCurShape == shape) nextShape();  
  iShapes.erase(shape);
}

void Group::shapeKilled(Shape* shape) 
{
  removeKid(shape);
}
