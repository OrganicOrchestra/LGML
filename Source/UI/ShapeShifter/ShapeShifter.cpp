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

#if !ENGINE_HEADLESS

#include "ShapeShifter.h"
#include "ShapeShifterContainer.h"


const int ShapeShifter::minSize(50);

ShapeShifter::ShapeShifter (Type _type) :
    shifterType (_type),
    preferredWidth (300), preferredHeight (300),
    parentShifterContainer (nullptr)
{

}

ShapeShifter::~ShapeShifter()
{
}

void ShapeShifter::setPreferredWidth (int newWidth)
{
    preferredWidth = jmax<int> (minSize, newWidth);
}

void ShapeShifter::setPreferredHeight (int newHeight)
{
    preferredHeight = jmax<int> (minSize, newHeight);

}

int ShapeShifter::getPreferredWidth()
{
    jassert(preferredWidth>0);
    return preferredWidth;
}

int ShapeShifter::getPreferredHeight()
{
    jassert(preferredHeight>0);
    return preferredHeight;
}

int ShapeShifter::getMinWidth(){
    return minSize;
}

int ShapeShifter::getMinHeight(){
    return minSize;
}
bool ShapeShifter::isDetached()
{
    return parentShifterContainer == nullptr;
}

void ShapeShifter::setParentContainer (ShapeShifterContainer* _parent)
{
    if (_parent == parentShifterContainer) return;

    parentShifterContainer = _parent;
}


var ShapeShifter::getCurrentLayout()
{
    var layout (new DynamicObject());
    layout.getDynamicObject()->setProperty ("type", (int)shifterType);
    layout.getDynamicObject()->setProperty ("width", preferredWidth);
    layout.getDynamicObject()->setProperty ("height", preferredHeight);
    return layout;
}

void ShapeShifter::loadLayout (var layout)
{
    setPreferredWidth (layout.getDynamicObject()->getProperty ("width"));
    setPreferredHeight (layout.getDynamicObject()->getProperty ("height"));
    
    loadLayoutInternal (layout);
}

#endif
