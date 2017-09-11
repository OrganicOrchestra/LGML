/* Copyright © Organic Orchestra, 2017
*
* This file is part of LGML.  LGML is a software to manipulate sound in realtime
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation (version 3 of the License).
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
*/


#include "ShapeShifter.h"
#include "ShapeShifterContainer.h"

ShapeShifter::ShapeShifter(Type _type) :
	shifterType(_type),
	preferredWidth(300), preferredHeight(300),
	parentContainer(nullptr)
{

}

ShapeShifter::~ShapeShifter()
{
}

void ShapeShifter::setPreferredWidth(int newWidth)
{
	preferredWidth = jmax<int>(150, newWidth);
}

void ShapeShifter::setPreferredHeight(int newHeight)
{
	preferredHeight = jmax<int>(50, newHeight);
}

int ShapeShifter::getPreferredWidth()
{
	return preferredWidth;
}

int ShapeShifter::getPreferredHeight()
{
	return preferredHeight;
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

bool ShapeShifter::isFlexible()
{
	return false;
}

var ShapeShifter::getCurrentLayout()
{
	var layout(new DynamicObject());
	layout.getDynamicObject()->setProperty("type", (int)shifterType);
	layout.getDynamicObject()->setProperty("width", preferredWidth);
	layout.getDynamicObject()->setProperty("height", preferredHeight);
	return layout;
}

void ShapeShifter::loadLayout(var layout)
{
	setPreferredWidth(layout.getDynamicObject()->getProperty("width"));
	setPreferredHeight(layout.getDynamicObject()->getProperty("height"));

	loadLayoutInternal(layout);
}
