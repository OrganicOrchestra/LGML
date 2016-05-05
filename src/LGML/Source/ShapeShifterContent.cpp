/*
  ==============================================================================

    ShapeShifterContent.cpp
    Created: 3 May 2016 10:17:41am
    Author:  bkupe

  ==============================================================================
*/

#include "ShapeShifterContent.h"

ShapeShifterContent::ShapeShifterContent(const String & _contentName) :
	contentIsFlexible(false),
	contentName(_contentName)
  {
  }

  ShapeShifterContent::~ShapeShifterContent()
  {
  }

  String ShapeShifterContent::getName() const {
	  return contentName;
  }
