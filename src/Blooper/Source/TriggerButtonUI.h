/*
  ==============================================================================

    TriggerButtonUI.h
    Created: 8 Mar 2016 3:45:53pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef TRIGGERBUTTONUI_H_INCLUDED
#define TRIGGERBUTTONUI_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class TriggerButtonUI    : public Component
{
public:
    TriggerButtonUI();
    ~TriggerButtonUI();

    void paint (Graphics&);
    void resized();

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TriggerButtonUI)
};


#endif  // TRIGGERBUTTONUI_H_INCLUDED
