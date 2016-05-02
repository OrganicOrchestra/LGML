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

class ShapeShifter : public juce::Component
{
public :
	ShapeShifter();
	virtual ~ShapeShifter();

	int preferredWidth;
	int preferredHeight;
	void setPreferredWidth(int newWidth);
	void setPreferredHeight(int newHeight);

};


#endif  // SHAPESHIFTER_H_INCLUDED
