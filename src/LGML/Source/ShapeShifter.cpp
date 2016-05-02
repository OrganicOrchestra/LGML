/*
  ==============================================================================

    ShapeShifter.cpp
    Created: 2 May 2016 6:33:16pm
    Author:  bkupe

  ==============================================================================
*/

#include "ShapeShifter.h"

ShapeShifter::ShapeShifter() :
	preferredWidth(-1), preferredHeight(-1)
{
}

ShapeShifter::~ShapeShifter()
{
}

void ShapeShifter::setPreferredWidth(int newWidth)
{
	preferredWidth = jmax<int>(5, newWidth);
}

void ShapeShifter::setPreferredHeight(int newHeight)
{
	preferredHeight = jmax<int>(0, newHeight);
}