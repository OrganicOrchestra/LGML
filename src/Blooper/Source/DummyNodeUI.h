/*
  ==============================================================================

    DummyNodeUI.h
    Created: 4 Mar 2016 12:01:05am
    Author:  bkupe

  ==============================================================================
*/

#ifndef DUMMYNODEUI_H_INCLUDED
#define DUMMYNODEUI_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "NodeBaseUI.h"

//==============================================================================
/*
*/
class DummyNodeUI    : public NodeBaseUI
{
public:
    DummyNodeUI();
    ~DummyNodeUI();


private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DummyNodeUI)
};


#endif  // DUMMYNODEUI_H_INCLUDED
