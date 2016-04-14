/*
  ==============================================================================

    NodeConnectionEditorDataSlot.h
    Created: 29 Mar 2016 7:03:43pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef NODECONNECTIONEDITORDATASLOT_H_INCLUDED
#define NODECONNECTIONEDITORDATASLOT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class NodeConnectionEditorDataSlot    : public Component
{
public:
    NodeConnectionEditorDataSlot();
    ~NodeConnectionEditorDataSlot();

    void paint (Graphics&);
    void resized();

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NodeConnectionEditorDataSlot)
};


#endif  // NODECONNECTIONEDITORDATASLOT_H_INCLUDED
