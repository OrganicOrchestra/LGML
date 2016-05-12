/*
  ==============================================================================

    DataInNodeUI.h
    Created: 19 Apr 2016 10:39:16am
    Author:  bkupe

  ==============================================================================
*/

#ifndef DATAINNODEUI_H_INCLUDED
#define DATAINNODEUI_H_INCLUDED

#include "NodeBaseUI.h"
#include "ParameterUI.h"
#include "DataInNode.h"

//==============================================================================
/*
*/
class DataInNodeContentUI    : public NodeBaseContentUI, DataInNode::DataInListener
{
public:
    DataInNodeContentUI();
    ~DataInNodeContentUI();

    DataInNode * dataInNode;

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
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DataInNodeContentUI)
};


#endif  // DATAINNODEUI_H_INCLUDED
