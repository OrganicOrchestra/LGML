/*
  ==============================================================================

    UIHelpers.h
    Created: 4 Mar 2016 12:25:27pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef UIHELPERS_H_INCLUDED
#define UIHELPERS_H_INCLUDED

#include "JuceHeader.h"
#include "Style.h"

#define DRAW_CONTOUR 0

class ContourComponent : public Component
{
public:
	ContourComponent(Colour c = CONTOUR_COLOR) :c(c) {}
	virtual ~ContourComponent() {}
	Colour c;

	virtual void paint(Graphics &g) override
	{
#if DRAW_CONTOUR
		g.fillAll(c.withAlpha(.3f));
		g.setColour(c.withAlpha(.4f));
		g.drawRect(getLocalBounds());
#endif
	}
};

class ComponentUtil
{
public:

	static Point<int> getRelativeComponentPosition(Component * target, Component * source)
	{
		return source->globalPositionToRelative(target->localPointToGlobal(target->getLocalBounds().getTopLeft()));
	}

	static Point<int> getRelativeComponentPositionCenter(Component * target, Component * source)
	{
		return source->globalPositionToRelative(target->localPointToGlobal(target->getLocalBounds().getCentre()));
	}
};


#endif  // UIHELPERS_H_INCLUDED
