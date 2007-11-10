/*
 *  Shape.h
 *  LusionEngine
 *
 *  Created by Erik Engheim on 19.1.07.
 *  Copyright 2007 Translusion. All rights reserved.
 *
 */

#pragma once

#include "Types.h"
#include <Geometry/Rect2.hpp>
#include <Geometry/Circle.hpp>
#include <Geometry/Segment2.hpp>

#include <Core/SharedObject.hpp>

#include <Base/ShapeIterator.h>

class SpriteCommand;

class Shape : public SharedObject
{
public:
  // Constructors
  Shape();
  virtual ~Shape();

  // Accessors
  virtual Rect2 boundingBox() const = 0;  
  virtual int   noShapes() const;  
  virtual ShapeIterator* shapeIterator() const;
    
  // Request
  virtual bool isSimple() const;
  virtual bool collide(Shape* other, real t, real dt, SpriteCommand* command = 0) = 0;  
  virtual bool inside(const Point2& p, real t, real dt, SpriteCommand* command = 0) = 0;
    
  // Calculations
  virtual bool intersect(const Circle& c) const;
  virtual bool intersect(const Rect2& r) const;
  virtual bool intersect(const Segment2& s) const;
  virtual bool intersect(ConstPointIterator2 begin, ConstPointIterator2 end) const;      
   
  virtual void draw(const Rect2& r) const;
   
  // Operations
  virtual void addKid(Shape* shape);
  virtual void removeKid(Shape* shape);
  
  virtual void update(real start_time, real delta_time);  
};