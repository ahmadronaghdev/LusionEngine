/*
 *  PolygonView.cpp
 *  LusionEngine
 *
 *  Created by Erik Engheim on 11.1.07.
 *  Copyright 2007 Translusion. All rights reserved.
 *
 */

#include "Base/PolygonView.h"
#include "Utils/PolygonUtils.h"

#include <CGAL/Polygon_2_algorithms.h> 

static Point2 gPoints[] = {Point2(-1.0, -1.0), Point2(1.0, 0.0), Point2(-1.0, 1.0)};

PolygonView::PolygonView(ConstPointIterator2 begin, ConstPointIterator2 end, GLenum style)
{
  init(begin, end, style);
}

PolygonView::PolygonView(GLenum style)
{
  Polygon2 poly(gPoints, gPoints+3);
  init(poly.begin(), poly.end(), style);
}

PolygonView::~PolygonView()
{

}

// Accessors
void PolygonView::setDrawStyle(GLenum style)
{
  iStyle = style;
}

GLenum PolygonView::drawStyle() const
{
  return iStyle;
}

// Calculations
void PolygonView::draw(int) const
{
  ShallowPoints2 poly = collisionPolygon();
  glColor3dv(iColor);	
  glBegin(iStyle);  
    gltVertex(poly.first, poly.second);  
  glEnd();
}

// Operations
void PolygonView::init( ConstPointIterator2 begin, ConstPointIterator2 end, GLenum style )
{
  setCollisionPolygon(begin, end);
  iStyle = style;
}
