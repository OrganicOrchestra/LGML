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


#ifndef SHAPESHIFTER_H_INCLUDED
#define SHAPESHIFTER_H_INCLUDED

#include "../../JuceHeaderUI.h"//keep

class ShapeShifterContainer;

class ShapeShifter : public juce::Component
{
public :
	enum Type {PANEL, CONTAINER};

	ShapeShifter(Type _type);
	virtual ~ShapeShifter();

	ShapeShifterContainer * parentContainer;

	Type shifterType;

	int preferredWidth;
	int preferredHeight;
	void setPreferredWidth(int newWidth);
	void setPreferredHeight(int newHeight);

	virtual int getPreferredWidth();
	virtual int getPreferredHeight();

	bool isDetached();
	void setParentContainer(ShapeShifterContainer * _parent);

	virtual bool isFlexible();

	virtual var getCurrentLayout();
	virtual void loadLayout(var layout);
  virtual void loadLayoutInternal(var /*layout*/) =0;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ShapeShifter)
};


#endif  // SHAPESHIFTER_H_INCLUDED
