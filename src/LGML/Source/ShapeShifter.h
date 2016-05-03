/*
  ==============================================================================

    ShapeShifter.h
    Created: 2 May 2016 6:33:16pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef SHAPESHIFTER_H_INCLUDED
#define SHAPESHIFTER_H_INCLUDED

#include "JuceHeader.h"

class ShapeShifterContainer;

class ShapeShifter : public juce::Component
{
public :
	ShapeShifter();
	virtual ~ShapeShifter();
	ShapeShifterContainer * parentContainer;

	int preferredWidth;
	int preferredHeight;
	void setPreferredWidth(int newWidth);
	void setPreferredHeight(int newHeight);

	
	bool isDetached();
	void setParentContainer(ShapeShifterContainer * _parent);
	
	virtual void mouseEnter(const MouseEvent &) override { DBG("Mouse ENTER !"); }
	virtual void mouseExit(const MouseEvent &) override { DBG("Mouse EXIT !"); }
	virtual void mouseMove(const MouseEvent &) override { DBG("Mouse OVER"); }


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ShapeShifter)
};


#endif  // SHAPESHIFTER_H_INCLUDED
