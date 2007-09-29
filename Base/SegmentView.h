/*
 *  SegmentView.h
 *  LusionEngine
 *
 *  Created by Erik Engheim on 11.1.07.
 *  Copyright 2007 Translusion. All rights reserved.
 *
 */

#pragma once

#include "Base/View.h"

#include <Geometry/Segment2.hpp>

class SegmentView : public View
{
public:
  // Constructors
  SegmentView(Segments2::iterator begin, Segments2::iterator end );
  SegmentView();
  virtual ~SegmentView();

  // Accessors
  
  // Request
  void setArrowVisible(bool visible);
  bool arrowVisible() const;

  // Calculations
	virtual void draw(int image_index = 0) const;
  
  // Operations
  void addSegment(const Segment2& aSeg);
  
private:
  Segments2 iSegments;
  bool iShowArrowHead;  
};