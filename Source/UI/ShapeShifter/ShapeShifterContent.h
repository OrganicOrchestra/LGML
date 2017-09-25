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


#ifndef SHAPESHIFTERCONTENT_H_INCLUDED
#define SHAPESHIFTERCONTENT_H_INCLUDED

#include "../../JuceHeaderUI.h"//keep
#include "../Inspector/InspectableComponent.h"

class ShapeShifterContent
{
public:
	ShapeShifterContent(Component * contentComponent, const String &_contentName);
	virtual ~ShapeShifterContent();

	Component * contentComponent;
	String contentName;

	bool contentIsFlexible;

	bool contentIsShown;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ShapeShifterContent)
};

//Helper class if child class doesn't need to inherit a subclass of Component
class ShapeShifterContentComponent :
	public juce::Component,
	public ShapeShifterContent
{
public:
  ShapeShifterContentComponent(const String &contentName);
};

#endif  // SHAPESHIFTERCONTENT_H_INCLUDED
