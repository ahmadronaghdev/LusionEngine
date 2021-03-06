/*
 *  ShapeTests.cpp
 *  LusionEngine
 *
 *  Created by Erik Engheim on 10.11.07.
 *  Copyright 2007 Translusion. All rights reserved.
 *
 */

#include "ShapeTests.h"

#include "Utils/PolygonUtils.h"

#include "Base/CircleShape.h"
#include "Base/RectShape2.h"
#include "Base/SegmentShape2.h"
#include "Base/ShapeGroup.h"
#include "Core/AutoreleasePool.hpp"

#include <numeric>
#include <vector>

using namespace std;

static real t = 0.0f;
static real dt = 1.0f;

ShapeTests::ShapeTests(TestInvocation *invocation)
    : TestCase(invocation)
{
}


ShapeTests::~ShapeTests()
{
}

void ShapeTests::testIntersections()
{
  AutoreleasePool::begin();

  // Test circle vs circle
  CircleShape* c1 = new CircleShape(Circle(Vector2(0.0f, 0.0f), 4.0f));
  CircleShape* c2 = new CircleShape(Circle(Vector2(2.0f, 2.0f), 4.0f)); // overlap
  CircleShape* c3 = new CircleShape(Circle(Vector2(10.0f, 0.0f), 2.0f)); // outside
      
  CPTAssert(c1->inside(Vector2(1.0f, 1.0f), t, dt));
  CPTAssert(!c1->inside(Vector2(5.0f, 1.0f), t, dt));  
  
  CPTAssert(c1->collide(c2, t, dt));    
  CPTAssert(!c1->collide(c3, t, dt));    
    
  // Test circle with rectangles
  RectShape2* r1 = new RectShape2(Rect2(Vector2(1.0f, 1.0f), Vector2(6.0f, 6.0f)));
  RectShape2* r2 = new RectShape2(Rect2(Vector2(6.0f, 6.0f), Vector2(10.0f, 10.0f)));
    
  CPTAssert(c1->collide(r1, t, dt));    
  CPTAssert(!c1->collide(r2, t, dt));    
  
      
  // Cleanup
  c1->release();
  c2->release();
  c3->release();    
  
  r1->release();
  r2->release();
  AutoreleasePool::end();
}

void ShapeTests::testMovement()
{
  AutoreleasePool::begin();

  // Test bounding box calculations
  Point2 points[] = {Point2(-1.0f, -1.0f), Point2(1.0f, 0.0f), Point2(-1.0f, 1.0f)};
  
  real min = -numeric_limits<real>::max();
  real max = numeric_limits<real>::max();
    
  Vector2 minvec = Vector2(min, min);
  Vector2 maxvec = Vector2(max, max);
  Rect2 r(maxvec, minvec);
  CPTAssert(r.min() == maxvec && r.max() == minvec); 
  Point2* p = points;
  for (; p != points+3; ++p) {
    r = r.surround(*p);
  }
   
  CPTAssert(r.min() == points[0]);
  CPTAssert(r.max() == Vector2(1.0f, 1.0f));    
  AutoreleasePool::end();  
}

void ShapeTests::testHierarchyIterators()
{
  AutoreleasePool::begin();
  CircleShape* c1 = new CircleShape(Circle(Vector2(0.0f, 0.0f), 4.0f));
  CircleShape* c2 = new CircleShape(Circle(Vector2(2.0f, 2.0f), 4.0f)); // overlap
  CircleShape* c3 = new CircleShape(Circle(Vector2(10.0f, 0.0f), 2.0f)); // outside
  
  vector<Shape*> shapes;
  shapes.push_back(c1);
  shapes.push_back(c2);
  shapes.push_back(c3);
      
  ShapeGroup* group = new ShapeGroup(shapes.begin(), shapes.end());
  group->setTag(4321);
  ShapeIterator* itr = group->iterator();
  itr->setTag(1234);
  itr->retain();
  CPTAssert(itr != 0);
  CPTAssert(!itr->done());  
  itr->next();
  CPTAssert(!itr->done());  
  itr->next();  
  CPTAssert(itr->done());    
  CPTAssert(group->noShapes() == 3);
  
  // Cleanup
  c1->release();
  c2->release();
  c3->release();   
  
  group->release();
  AutoreleasePool::end();  
}

static ShapeTests test1(TEST_INVOCATION(ShapeTests, testIntersections));
static ShapeTests test2(TEST_INVOCATION(ShapeTests, testMovement));
static ShapeTests test3(TEST_INVOCATION(ShapeTests, testHierarchyIterators));