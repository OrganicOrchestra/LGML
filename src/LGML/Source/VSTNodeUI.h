/*
  ==============================================================================

    VSTNodeUI.h
    Created: 24 Mar 2016 9:44:38pm
    Author:  Martin Hermant

  ==============================================================================
*/

#ifndef VSTNODEUI_H_INCLUDED
#define VSTNODEUI_H_INCLUDED



#include "FloatSliderUI.h"
#include "MIDIUIHelper.h"
#include "VSTNode.h"
#include "NodeBaseHeaderUI.h"
#include "NodeBaseContentUI.h"

class VSTNodeHeaderUI : public NodeBaseHeaderUI, public VSTNode::VSTNodeListener
{
public:
    VSTNodeHeaderUI();
    ~VSTNodeHeaderUI();
    VSTNode * vstNode;

    void init() override;

    // Inherited via ChangeListener
    void newVSTSelected() override;
	void midiDeviceChanged() override {}
};

class VSTNodeContentUI:
	public NodeBaseContentUI,
	public Button::Listener,
	public VSTNode::VSTNodeListener,
	private ControllableContainerListener,
	public ComboBoxListener
{
public:
    VSTNodeContentUI();
    ~VSTNodeContentUI();
    OwnedArray<FloatSliderUI> paramSliders;

	VSTNode * vstNode;

	TextButton VSTListShowButton;
	TextButton showPluginWindowButton;

	MIDIDeviceChooser midiDeviceChooser;

	ScopedPointer<ParameterUI> activityBlink;

    void init() override;
	void resized()override;

    void updateVSTParameters();
    void newVSTSelected() override;
	void midiDeviceChanged() override;

    void layoutSliderParameters(Rectangle<int> pArea);
    void comboBoxChanged(ComboBox *cb) override;
    void buttonClicked(Button* button) override;


	void controllableAdded(ControllableContainer *,Controllable * c)override;
	void controllableRemoved(ControllableContainer *,Controllable * c)override;
	void controllableContainerAdded(ControllableContainer *,ControllableContainer * cc)override;
	void controllableContainerRemoved(ControllableContainer *,ControllableContainer * cc) override;
	void controllableFeedbackUpdate(ControllableContainer *originContainer,Controllable *c) override;


	static void vstSelected(int modalResult, Component *  originComp);
	void handleCommandMessage(int )override;

  bool isDirty;

};


#endif  // VSTNODEUI_H_INCLUDED
