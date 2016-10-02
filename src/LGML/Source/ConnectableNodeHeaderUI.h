/*
  ==============================================================================

    ConnectableNodeHeaderUI.h
    Created: 8 Mar 2016 5:53:52pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef CONNECTABLENODEHEADERUI_H_INCLUDED
#define CONNECTABLENODEHEADERUI_H_INCLUDED

#include "StringParameterUI.h"
#include "BoolToggleUI.h"

#include "ConnectableNode.h"
class VuMeter;
class ConnectableNodeUI;

class ConnectableNodeHeaderUI : public Component,
	public ComboBox::Listener,
	public Button::Listener,
	public ControllableContainerListener,
	public ConnectableNode::ConnectableNodeListener
{
public:

    class Grabber : public Component
    {
        void paint(Graphics &g) override;
    };

	ConnectableNodeHeaderUI();
    virtual ~ConnectableNodeHeaderUI();

    ConnectableNode * node;
    ConnectableNodeUI * nodeUI;

    ScopedPointer<StringParameterUI> titleUI;
	ScopedPointer<StringParameterUI> descriptionUI; 
	ScopedPointer<BoolToggleUI> enabledUI;
	ScopedPointer<VuMeter> vuMeterIn;
    ScopedPointer<VuMeter> vuMeterOut;

    Grabber grabber;
    ImageButton removeBT;
	TextButton miniModeBT;
    ScopedPointer<ComboBox> presetCB;


    virtual void setNodeAndNodeUI(ConnectableNode * node, ConnectableNodeUI * nodeUI);
    virtual void updatePresetComboBox(bool forceUpdate = false);
    virtual void init();


    virtual void resized() override;

    void updateVuMeters();

	bool bMiniMode;
	virtual void setMiniMode(bool value);

    // Inherited via Listeners
	virtual void nodeParameterChanged(ConnectableNode *,Parameter *) override;

    virtual void comboBoxChanged(ComboBox * comboBoxThatHasChanged) override;
	virtual void buttonClicked(Button *) override;
	virtual void controllableContainerPresetLoaded(ControllableContainer *) override;


     void numAudioInputChanged(ConnectableNode *, int /*newNumInput*/) override;
     void numAudioOutputChanged(ConnectableNode *, int /*newNumOutput*/) override;
    
    
    void handleCommandMessage(int id) override;
    enum {
        updatePresetCBID,
        repaintId
    }DrawingCommand;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConnectableNodeHeaderUI)




};




#endif  // CONNECTABLENODEHEADERUI_H_INCLUDED
