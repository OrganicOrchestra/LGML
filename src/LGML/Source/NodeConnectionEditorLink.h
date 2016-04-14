/*
  ==============================================================================

    NodeConnectionEditorLink.h
    Created: 29 Mar 2016 7:00:07pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef NODECONNECTIONEDITORLINK_H_INCLUDED
#define NODECONNECTIONEDITORLINK_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "NodeConnectionEditorDataSlot.h"

//==============================================================================
/*
*/
class NodeConnectionEditorLink    : public Component
{
public:
    NodeConnectionEditorLink();
    ~NodeConnectionEditorLink();

    NodeConnectionEditorDataSlot * outSlot;
    NodeConnectionEditorDataSlot * inSlot;

    void paint (Graphics&);
    void resized();

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NodeConnectionEditorLink)
};


#endif  // NODECONNECTIONEDITORLINK_H_INCLUDED
