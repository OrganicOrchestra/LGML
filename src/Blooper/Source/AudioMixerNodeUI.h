/*
  ==============================================================================

    AudioMixerNodeUI.h
    Created: 7 Mar 2016 4:45:17pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef AUDIOMIXERNODEUI_H_INCLUDED
#define AUDIOMIXERNODEUI_H_INCLUDED

#include "JuceHeader.h"
#include "NodeBaseUI.h"

//==============================================================================
/*
*/
class AudioMixerNodeUI    : public NodeBaseUI
{
public:
    AudioMixerNodeUI(NodeBase * node);
    ~AudioMixerNodeUI();

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioMixerNodeUI)
};


#endif  // AUDIOMIXERNODEUI_H_INCLUDED
