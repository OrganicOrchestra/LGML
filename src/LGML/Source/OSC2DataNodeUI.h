/*
  ==============================================================================

    OSC2DataNodeUI.h
    Created: 19 Apr 2016 10:39:16am
    Author:  bkupe

  ==============================================================================
*/

#ifndef OSC2DATANODEUI_H_INCLUDED
#define OSC2DATANODEUI_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

#include "NodeBaseUI.h"
#include "ParameterUI.h"

//==============================================================================
/*
*/
class OSC2DataNodeContentUI    : public NodeBaseContentUI, OSC2DataNode::Listener
{
public:
    OSC2DataNodeContentUI();
    ~OSC2DataNodeContentUI();

    OSC2DataNode * o2dNode;

    OwnedArray<ParameterUI> parameterUIs;

    void paint (Graphics&)override;
    void resized()override;

    void init() override;

    //helpers
    void addUIForParameter(Parameter * p);
    void addParameterUI(ParameterUI *pui);

private:

    // Inherited via Listener
    virtual void parameterAdded(Parameter *) override;
    virtual void parameterRemoved(Parameter *) override;

    ParameterUI * getUIForParameter(Parameter * p)
    {
        for (auto &pui : parameterUIs)
        {
            if (pui->parameter == p) return pui;
        }

        return nullptr;
    }
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OSC2DataNodeContentUI)
};


#endif  // OSC2DATANODEUI_H_INCLUDED
