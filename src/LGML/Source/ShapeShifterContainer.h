/*
  ==============================================================================

    ShapeShifterContainer.h
    Created: 2 May 2016 3:11:25pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef SHAPESHIFTERCONTAINER_H_INCLUDED
#define SHAPESHIFTERCONTAINER_H_INCLUDED

#include "MovablePanel.h"

class ShapeShifterContainer : public Component
{
public:
	ShapeShifterContainer();
	virtual ~ShapeShifterContainer();

	enum Direction {NONE, HORIZONTAL, VERTICAL };

	Direction direction;
	OwnedArray<ShapeShifterContainer> innerContainers;
	Array<MovablePanel *> panels;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ShapeShifterContainer)
};


#endif  // SHAPESHIFTERCONTAINER_H_INCLUDED
