/*
 *  PolygonUtils.cpp
 *  LusionEngine
 *
 *  Created by Erik Engheim on 13.1.07.
 *  Copyright 2007 Translusion. All rights reserved.
 *
 */

#include "Utils/PolygonUtils.h"

#include <OpenGL/gl.h>

#include <numeric>
#include <iostream>

ProjectPoint::ProjectPoint(const Vector2& v1) : v(v1) {
}

real ProjectPoint::operator()(const Point2& v1) {
  return v.dot(v1);
}

/*! 
  Projects the two polygons stored in functor onto vector v.
  Returns true if the projected polygons overlap.
*/
bool ProjectPolygon::operator()(const Vector2& v)
{
 // Project all points on poly1 and poly2 onto line v
 transform(pb, pe, proj1.begin(), ProjectPoint(v));
 transform(qb, qe, proj2.begin(), ProjectPoint(v));   

 real min1 = *min_element(proj1.begin(), proj1.end());
 real max1 = *max_element(proj1.begin(), proj1.end());  
 real min2 = *min_element(proj2.begin(), proj2.end());
 real max2 = *max_element(proj2.begin(), proj2.end());  
  
 return min1 > max2 || max1 < min2;
}

/*!
  Check if there is a collision between two polygons. First polygon is defined
  by point sequence 'pb' to 'pe'. Second polygon by 'qb' to 'qe'. 
*/
bool intersect(
  ConstPointIterator2 pb, // Start of first polygon
  ConstPointIterator2 pe, 
  ConstPointIterator2 qb, // Start of second polygon
  ConstPointIterator2 qe)
{
  Polygon2 d1(pe-pb), d2(qe-qb);  // holds directions
  
  // Find direction of each edge in polygons
  CalcDirection calcDir;  
  adjacent_difference(pb, pe, d1.begin(), calcDir);
  adjacent_difference(qb, qe, d2.begin(), calcDir);
  d1[0] = calcDir(d1[0], *(pe-1));
  d2[0] = calcDir(d2[0], *(qe-1));
       
  // We have collision if it was not possible to find any separating axis
  // if ProjectPolygon is true for any of the directions then that direction
  // will be returned. This will make the equation false and signal no collision.
  return 
    find_if(d1.begin(), d1.end(), ProjectPolygon(pb, pe, qb, qe)) == d1.end() &&
    find_if(d2.begin(), d2.end(), ProjectPolygon(pb, pe, qb, qe)) == d2.end();   
}

bool intersect(const Polygon2& p1, const Polygon2& p2)
{
  return intersect(p1.begin(), p1.end(), p2.begin(), p2.end());
}

bool intersect(const Circle& circle, ConstPointIterator2 begin, ConstPointIterator2 end)
{
  ConstPointIterator2 it;
  for (it = begin+1; it != end; ++it) {
    if (circle.intersect(Segment2(*it, *(it-1)))) return true;
  }
  return false;
}

bool intersect(const Segment2& s, ConstPointIterator2 begin, ConstPointIterator2 end)
{
  ConstPointIterator2 it;
  for (it = begin+1; it != end; ++it) {
    if (s.intersect(Segment2(*it, *(it-1)))) return true;
  }
  return false;  
}

bool intersect(const Rect2& rect, ConstPointIterator2 begin, ConstPointIterator2 end)
{
  Segment2 bottom(rect.bottomLeft(), rect.bottomRight());
  Segment2 top(rect.topLeft(), rect.topRight());
  Segment2 left(rect.bottomLeft(), rect.topLeft());
  Segment2 right(rect.bottomRight(), rect.topRight());
    
  ConstPointIterator2 it;
  for (it = begin+1; it != end; ++it) {
    Segment2 seg(*it, *(it-1));
    if (seg.intersect(bottom)) return true;
    if (seg.intersect(top)) return true;    
    if (seg.intersect(left)) return true;        
    if (seg.intersect(right)) return true;        
  }
  return false;  
}

// NOTE: Only works for convex shapes
bool inside(ConstPointIterator2 pb, ConstPointIterator2 pe, const Point2& q)
{
  bool is_inside = true;
  Polygon2::const_iterator p;
  for (p = pb+1; p != pe; ++p) {
    if ((*p - *(p-1)).cross(q - *(p-1)) > 0.0) continue;
    is_inside = false;
    break;
  }
  return is_inside;
}

/*!
  Calculates a minimum bounding box for all supplied points
*/
Rect2 boundingBox(ConstPointIterator2 pb, ConstPointIterator2 pe)
{
  real min = numeric_limits<real>::min();
  real max = numeric_limits<real>::max();
    
  Rect2 r(Vector2(max, max), Vector2(min, min));
  Polygon2::const_iterator p;
  for (p = pb; p != pe; ++p) {
    r.surround(*p);
  }
  return r;
}

void gltTranslate(const Point2& pos)
{  
  glTranslated(pos.x(), pos.y(),0.0);
}

void gltVertex(const Point2& pos)
{  
  glVertex2d(pos.x(), pos.y());
}

void gltVertex(const Rect2& rect)
{
  glVertex2d(rect.xmin(), rect.ymin());
  glVertex2d(rect.xmax(), rect.ymin());
  glVertex2d(rect.xmax(), rect.ymax());
  glVertex2d(rect.xmin(), rect.ymax());                    
}

void gltVertex(const Segment2& segment)
{
  gltVertex(segment.source());
  gltVertex(segment.target());
}

/*! 
 Draws polygon in current OpenGL context with current color. 
 angle is given in degrees (not rad).
*/
void gltVertex(ConstPointIterator2 begin, ConstPointIterator2 end, const Point2& pos, double angle)
{
  glPushMatrix();
    gltTranslate(pos);
    glRotated(angle, 0.0, 0.0, 1.0);
    gltVertex(begin, end);
  glPopMatrix();  
}

/*! 
 Draws polygon in current OpenGL context with current color. 
*/
void gltVertex(ConstPointIterator2 begin, ConstPointIterator2 end)
{
  Polygon2::const_iterator p = begin;
  while (p != end) {
    gltVertex(*p);
    ++p;        
  }
}

// Debug
void gltPerformanceTest(int n)
{
  double m[16];
  GLenum matrixMode;
  glGetIntegerv(GL_MATRIX_MODE, (GLint*)&matrixMode);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glGetDoublev(GL_MODELVIEW_MATRIX, m);
 
  for (int i=0; i<n; ++i) {
    glRotated(40, 0,0,1);
    glGetDoublev(GL_MODELVIEW_MATRIX, m);    
    glMultMatrixd(m);
  }
  glPopMatrix();  
  glMatrixMode(matrixMode);
}

void trigPerformanceTest(int n)
{
  double m[16];
  GLenum matrixMode;
  glGetIntegerv(GL_MATRIX_MODE, (GLint*)&matrixMode);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();    
  glLoadIdentity();
  glGetDoublev(GL_MODELVIEW_MATRIX, m);
    
  double s, c;
  double angle = 0.15;

  s = sin(angle);
  c = cos(angle);      

  
  for (int i=0; i<n; ++i) {

    // glGetDoublev(GL_MODELVIEW_MATRIX, m);    
    // m[0] = c;
    // m[1] = s;
    // m[4] = -s;
    // m[5] = c;
    // glLoadMatrixd(m);    
  }
  
  glPopMatrix();  
  glMatrixMode(matrixMode);
}