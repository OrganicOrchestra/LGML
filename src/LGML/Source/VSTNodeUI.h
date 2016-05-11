/*
  ==============================================================================

    VSTNodeUI.h
    Created: 24 Mar 2016 9:44:38pm
    Author:  Martin Hermant

  ==============================================================================
*/

#ifndef VSTNODEUI_H_INCLUDED
#define VSTNODEUI_H_INCLUDED



#include "NodeBaseUI.h"

#include "FloatSliderUI.h"

class VSTNodeHeaderUI : public NodeBaseHeaderUI, public VSTNode::VSTNodeListener
{
public:
    VSTNodeHeaderUI();
    ~VSTNodeHeaderUI();
    VSTNode * vstNode;

    void init() override;

    // Inherited via ChangeListener
    void newVSTSelected() override;
};

class VSTNodeContentUI:public NodeBaseContentUI,public Button::Listener, public VSTNode::VSTNodeListener,public ControllableContainerListener{
public:
    VSTNodeContentUI();
    ~VSTNodeContentUI();
    OwnedArray<FloatSliderUI> paramSliders;

    void init() override;

    void updateVSTParameters();

    void controllableAdded(Controllable * c)override ;
    void controllableRemoved(Controllable * c)override;
    void controllableContainerAdded(ControllableContainer * cc)override;
    void controllableContainerRemoved(ControllableContainer * cc) override;
    void controllableFeedbackUpdate(Controllable *c) override;

    void newVSTSelected() override;


    void resized()override;

    void layoutSliderParameters(Rectangle<int> pArea);

    TextButton VSTListShowButton;
    TextButton showPluginWindowButton;
    VSTNode * vstNode;


    static void vstSelected (int modalResult, Component *  originComp);

    void buttonClicked (Button* button) override;

};


#endif  // VSTNODEUI_H_INCLUDED
