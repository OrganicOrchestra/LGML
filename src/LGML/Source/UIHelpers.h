/*
  ==============================================================================

    UIHelpers.h
    Created: 4 Mar 2016 12:25:27pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef UIHELPERS_H_INCLUDED
#define UIHELPERS_H_INCLUDED


#include "Style.h"
#include "JuceHeader.h"

#define DRAW_CONTOUR 0

class ContourComponent : public Component
{
public:
    ContourComponent(Colour c = CONTOUR_COLOR) :contourColor(c) {}
    virtual ~ContourComponent() {}
    Colour contourColor;

    #if DRAW_CONTOUR
    virtual void paint(Graphics &g) override
    {

        g.fillAll(contourColor.withAlpha(.3f));
        g.setColour(contourColor.withAlpha(.4f));
        g.drawRect(getLocalBounds());

    }
    #endif
};

#endif  // UIHELPERS_H_INCLUDED
