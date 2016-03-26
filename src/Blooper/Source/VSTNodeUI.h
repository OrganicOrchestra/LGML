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
class VSTNodeUI:public NodeBaseContentUI,public Button::Listener,public ChangeListener,public ControllableContainer::Listener{
public:
    VSTNodeUI(VSTNode * _owner);
    ~VSTNodeUI();
    OwnedArray<FloatSliderUI> paramSliders;

    void init() override;

    void updateVSTParameters();

    void controllableAdded(Controllable * c)override ;
    void controllableRemoved(Controllable * c)override;
    void controllableContainerAdded(ControllableContainer * cc)override;
    void controllableContainerRemoved(ControllableContainer * cc) override;
    void controllableFeedbackUpdate(Controllable *c) override;



    void changeListenerCallback(ChangeBroadcaster * c) override;


    void resized()override;

    void layoutSliderParameters(Rectangle<int> pArea);

    TextButton VSTListShowButton;
    TextButton showPluginWindowButton;
    VSTNode * owner;


    static void vstSelected (int modalResult, Component *  originComp);

    void buttonClicked (Button* button) override;

};


#endif  // VSTNODEUI_H_INCLUDED
