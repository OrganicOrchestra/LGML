/*
  ==============================================================================

    NodeBaseHeaderUI.h
    Created: 8 Mar 2016 5:53:52pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef NODEBASEHEADERUI_H_INCLUDED
#define NODEBASEHEADERUI_H_INCLUDED




#include "StringParameterUI.h"
#include "BoolToggleUI.h"
#include "VuMeter.h"
#include "Style.h"


class NodeBaseUI;
class FloatSliderUI;

class NodeBaseHeaderUI : public Component,
	public ComboBox::Listener,
	public Button::Listener,
	public ControllableContainer::Listener,
	public NodeBase::NodeListener
{
public:

    class Grabber : public Component
    {
        void paint(Graphics &g) override;
    };

    NodeBaseHeaderUI();
    virtual ~NodeBaseHeaderUI();

    NodeBase * node;
    NodeBaseUI * nodeUI;

    ScopedPointer<StringParameterUI> titleUI;
    ScopedPointer<BoolToggleUI> enabledUI;
	VuMeter vuMeterIn;
    VuMeter vuMeterOut;

    Grabber grabber;
    ImageButton removeBT;
    ScopedPointer<ComboBox> presetCB;

    void mouseDoubleClick(const MouseEvent & e)override;


    virtual void setNodeAndNodeUI(NodeBase * node, NodeBaseUI * nodeUI);
    virtual void updatePresetComboBox();
    virtual void init();


    virtual void resized() override;


    // Inherited via Listeners
	virtual void nodeEnableChanged(NodeBase *) override;
    virtual void comboBoxChanged(ComboBox * comboBoxThatHasChanged) override;
	virtual void buttonClicked(Button *) override;
	virtual void controllableContainerPresetLoaded(ControllableContainer *) override;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NodeBaseHeaderUI)

   
	

};




#endif  // NODEBASEHEADERUI_H_INCLUDED
