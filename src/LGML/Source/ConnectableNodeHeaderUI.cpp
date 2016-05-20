/*
 ==============================================================================

 ConnectableNodeHeaderUI.cpp
 Created: 8 Mar 2016 5:53:52pm
 Author:  bkupe

 ==============================================================================
 */
#include "ConnectableNodeHeaderUI.h"
#include "ConnectableNodeUI.h"

#include "PresetManager.h"
#include "FloatSliderUI.h"

ConnectableNodeHeaderUI::ConnectableNodeHeaderUI() : removeBT("X") ,
 vuMeterIn(VuMeter::Type::IN),
	vuMeterOut(VuMeter::Type::OUT)
{
    node = nullptr;
    nodeUI = nullptr;

    Image removeImage = ImageCache::getFromMemory(BinaryData::removeBT_png, BinaryData::removeBT_pngSize);

    removeBT.setImages(false, true, true, removeImage,
                        0.7f, Colours::transparentBlack,
                        removeImage, 1.0f, Colours::transparentBlack,
                        removeImage, 1.0f, Colours::white.withAlpha(.7f),
                        0.5f);
    removeBT.addListener(this);

    setSize(20, 40);

}

ConnectableNodeHeaderUI::~ConnectableNodeHeaderUI()
{
    if (node != nullptr)
	{
		node->removeRMSListener(&vuMeterOut);
		node->removeRMSListener(&vuMeterIn);

		node->removeControllableContainerListener(this);
		node->removeNodeListener(this);
    }

}

void ConnectableNodeHeaderUI::setNodeAndNodeUI(ConnectableNode * _node, ConnectableNodeUI * _nodeUI)
{
    this->node = _node;
    this->nodeUI = _nodeUI;

    if (node != nullptr)
	{
		if (node->hasAudioOutputs()) {
			node->addRMSListener(&vuMeterOut);
			addAndMakeVisible(vuMeterOut);

		}

		if (node->hasAudioInputs())
		{
			node->addRMSListener(&vuMeterIn);
			addAndMakeVisible(vuMeterIn);
		}

    }

	titleUI = node->nameParam->createStringParameterUI();
    titleUI->setNameLabelVisible(false);
    addAndMakeVisible(titleUI);

    enabledUI = node->enabledParam->createToggle();
    addAndMakeVisible(enabledUI);

    addAndMakeVisible(grabber);

	if(node->canBeRemovedByUser) addAndMakeVisible(removeBT);


    presetCB = new ComboBox("preset");
	if (node->canHavePresets)
	{
		updatePresetComboBox();
		addAndMakeVisible(presetCB);
		presetCB->addListener(this);
		presetCB->setTextWhenNothingSelected("Preset");
	}

	node->addControllableContainerListener(this);
	node->addNodeListener(this);

    init();
    resized();

}

void ConnectableNodeHeaderUI::updatePresetComboBox()
{

    bool emptyFilter = node->getPresetFilter().isEmpty();
    presetCB->setEnabled(!emptyFilter);

    if (!emptyFilter) PresetManager::getInstance()->fillWithPresets(presetCB, node->getPresetFilter());
}

void ConnectableNodeHeaderUI::init()
{
    //to override

}

void ConnectableNodeHeaderUI::resized()
{
    if (!node) return;

    int vuMeterWidth = 8;
    int removeBTWidth = 15;
    int grabberHeight = 12;
    int presetCBWidth = 100;


    Rectangle<int> r = getLocalBounds();

	grabber.setBounds(r.removeFromTop(grabberHeight));
	r.reduce(4, 0);
	r.removeFromTop(2);

    if (node->hasAudioOutputs())
	{
        vuMeterOut.setBounds(r.removeFromRight(vuMeterWidth));
    }

	if (node->hasAudioInputs())
	{
		vuMeterIn.setBounds(r.removeFromLeft(vuMeterWidth));

	}

	r.reduce(5, 2);

	enabledUI->setBounds(r.removeFromLeft(10));

    r.removeFromLeft(3);
	if (node->canBeRemovedByUser)
	{
		removeBT.setBounds(r.removeFromRight(removeBTWidth));
		r.removeFromRight(5);
	}
	
	if (node->canHavePresets)
	{
		presetCB->setBounds(r.removeFromRight(presetCBWidth));
		r.removeFromRight(5);
	}

    titleUI->setBounds(r);



}

void ConnectableNodeHeaderUI::nodeEnableChanged(ConnectableNode *)
{
	if (!node->enabledParam->boolValue())
	{
		vuMeterOut.setVoldB(0);

	}
	repaint();
}

void ConnectableNodeHeaderUI::comboBoxChanged(ComboBox * cb)
{
    DBG("Combobox ! " << cb->getSelectedId());
    int presetID = cb->getSelectedId();

    if (presetID == PresetChoice::SaveCurrent)
    {
        bool result = node->saveCurrentPreset();
        if(result) cb->setSelectedId(node->currentPreset->presetId, NotificationType::dontSendNotification);
        else cb->setSelectedItemIndex(-1, NotificationType::dontSendNotification);

    }if (presetID == PresetChoice::SaveToNew)
    {
        AlertWindow nameWindow("Save a new Preset","Choose a name for the new preset",AlertWindow::AlertIconType::QuestionIcon,this);
        nameWindow.addTextEditor("newPresetName", "New Preset");
        nameWindow.addButton("OK", 1, KeyPress(KeyPress::returnKey));
        nameWindow.addButton("Cancel", 0, KeyPress(KeyPress::escapeKey));

        int nameResult = nameWindow.runModalLoop();

        if (nameResult)
        {
            String presetName = nameWindow.getTextEditorContents("newPresetName");
            node->saveNewPreset(presetName);
            cb->clear(NotificationType::dontSendNotification);
            updatePresetComboBox();
            cb->setSelectedItemIndex(cb->getNumItems() - 1, NotificationType::dontSendNotification);
        }
        else
        {
            cb->setSelectedItemIndex(-1, NotificationType::dontSendNotification);
        }


    }else if (presetID == PresetChoice::ResetToDefault) //Reset to default
    {
        node->resetFromPreset();
        cb->setSelectedItemIndex(-1, NotificationType::dontSendNotification);
    }
    else
    {
        PresetManager::Preset * pre = PresetManager::getInstance()->getPreset(node->getPresetFilter(), cb->getItemText(cb->getSelectedItemIndex()));
        node->loadPreset(pre);
    }

}

void ConnectableNodeHeaderUI::buttonClicked(Button *)
{
    node->remove(true);
}

void ConnectableNodeHeaderUI::controllableContainerPresetLoaded(ControllableContainer *)
{
	if (!node->canHavePresets) return;

	int numOptions = 3;
	if (node->currentPreset != nullptr) presetCB->setSelectedItemIndex(node->currentPreset->presetId+numOptions-1, NotificationType::dontSendNotification);
}


void ConnectableNodeHeaderUI::Grabber::paint(Graphics & g)
{
    g.setColour(PANEL_COLOR.brighter(.2f));

	Rectangle<int> r = getLocalBounds().reduced(4);
	float left = (float)r.getTopLeft().x;
	float right = (float)r.getRight();
	g.drawHorizontalLine(r.getTopLeft().y, left, right);
	g.drawHorizontalLine(r.getCentreY(), left, right);
	g.drawHorizontalLine(r.getBottom(), left, right);
}
