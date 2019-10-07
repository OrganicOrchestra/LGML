/*
  ==============================================================================

    LayoutUtils.h
    Created: 26 Sep 2019 5:52:01pm
    Author:  Martin Hermant

  ==============================================================================
*/

#pragma once

//#include "../JuceHeaderUI.h"

template<class ComponentContainerType>
void layoutComponentsInGrid (const ComponentContainerType & components,
                             Rectangle<int> pArea,
                             float targetComponentAR=4)
{
    if (components.size() == 0) return;
    if(pArea.getHeight()==0) return;


    const float AR = pArea.getWidth()*1.0f/pArea.getHeight();

    int targetNumLine = (int)(sqrt(components.size())*targetComponentAR/AR);
    targetNumLine = jmax(1,targetNumLine);

    int numLines = jmin ((int)(targetNumLine), components.size());
    int numCols = (int)ceil((components.size() ) *1.0f/ numLines ) ;

    int w = pArea.getWidth() / numCols;
    int h = pArea.getHeight() / numLines;
    int idx = 0;

    for (int i = 0 ; i < numCols ; i ++)
    {
        Rectangle<int> col = pArea.removeFromLeft (w);

        for (int j = 0 ; j < numLines ; j++)
        {
            components.getUnchecked (idx)->setBounds (col.removeFromTop (h).reduced (1));
            idx++;

            if (idx >= components.size())
            {
                break;
            }
        }

        if (idx >= components.size())
        {
            break;
        }
    }
}

