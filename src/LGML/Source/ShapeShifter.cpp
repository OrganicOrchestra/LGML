/*
  ==============================================================================

    ShapeShifter.cpp
    Created: 2 May 2016 6:33:16pm
    Author:  bkupe

  ==============================================================================
*/

#include "ShapeShifter.h"
#include "ShapeShifterContainer.h"

ShapeShifter::ShapeShifter() :
	preferredWidth(-1), preferredHeight(-1),
	parentContainer(nullptr)
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

bool ShapeShifter::isDetached()
{
	return parentContainer == nullptr;
}

void ShapeShifter::setParentContainer(ShapeShifterContainer * _parent)
{
	if (_parent == parentContainer) return;
	parentContainer = _parent;
}
