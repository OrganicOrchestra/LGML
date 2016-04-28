/*
 ==============================================================================

 NodeBaseHeaderUI.cpp
 Created: 8 Mar 2016 5:53:52pm
 Author:  bkupe

 ==============================================================================
 */

#include "NodeBaseHeaderUI.h"
#include "PresetManager.h"


NodeBaseHeaderUI::NodeBaseHeaderUI() : removeBT("X")
{
    node = nullptr;
    nodeUI = nullptr;

    Image removeImage = ImageCache::getFromMemory(BinaryData::removeBT_png, BinaryData::removeBT_pngSize);

    removeBT.setImages(false, true, true, removeImage,
                        0.7f, Colours::transparentBlack,
                        removeImage, 1.0f, Colours::transparentBlack,
                        removeImage, 1.0f, Colours::pink.withAlpha(0.8f),
                        0.5f);
    setSize(20, 30);

}

NodeBaseHeaderUI::~NodeBaseHeaderUI()
{
    if (node != nullptr && node->hasAudioOutputs) {
        node->audioProcessor->removeRMSListener(&vuMeter);
    }

}

void NodeBaseHeaderUI::setNodeAndNodeUI(NodeBase * _node, NodeBaseUI * _nodeUI)
{
    this->node = _node;
    this->nodeUI = _nodeUI;

    if (node != nullptr && node->hasAudioOutputs) {
        node->audioProcessor->addRMSListener(&vuMeter);
        addAndMakeVisible(vuMeter);
    }

    titleUI = node->nameParam->createStringParameterUI();

	DBG("Node Header UI, name Param : " << node->nameParam->stringValue());

    titleUI->setNameLabelVisible(false);
    titleUI->setInterceptsMouseClicks(false, false);
    addAndMakeVisible(titleUI);

    enabledUI = node->enabledParam->createToggle();
    addAndMakeVisible(enabledUI);

    addAndMakeVisible(grabber);
    addAndMakeVisible(removeBT);

	presetCB = new ComboBox("preset");
	updatePresetComboBox();
	addAndMakeVisible(presetCB);
	presetCB->addListener(this);
	presetCB->setTextWhenNothingSelected("Preset");
    init();

}

void NodeBaseHeaderUI::updatePresetComboBox()
{
	DBG("Update Preset Combobox : " << node->getPresetFilter());

	bool emptyFilter = node->getPresetFilter().isEmpty();
	presetCB->setEnabled(!emptyFilter);
	
	if (!emptyFilter) PresetManager::getInstance()->fillWithPresets(presetCB, node->getPresetFilter());
}


void NodeBaseHeaderUI::mouseDoubleClick(const MouseEvent &){
    if(titleUI){titleUI->valueLabel.showEditor();}
}

void NodeBaseHeaderUI::init()
{
    //to override

}

void NodeBaseHeaderUI::resized()
{
    if (!node) return;

    int vuMeterWidth = 14;
    int removeBTWidth = 15;
    int grabberWidth = 40;
	int presetCBWidth = 70;

    Rectangle<int> r = getLocalBounds();

    if (node->hasAudioOutputs) {
        Rectangle<int> vuMeterRect = r.removeFromRight(vuMeterWidth).reduced(4);
        vuMeter.setBounds(vuMeterRect);
    }

    r.reduce(5, 2);
    r.removeFromLeft(enabledUI->getWidth());

    removeBT.setBounds(r.removeFromRight(removeBTWidth));
	presetCB->setBounds(r.removeFromRight(presetCBWidth).reduced(0, 4));
	grabber.setBounds(r.removeFromRight(grabberWidth));
	titleUI->setBounds(r);

    enabledUI->setTopLeftPosition(5, 5);

}

void NodeBaseHeaderUI::comboBoxChanged(ComboBox * cb)
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

void NodeBaseHeaderUI::Grabber::paint(Graphics & g)
{
	g.setColour(FRONT_COLOR);
	Rectangle<float> r = getLocalBounds().reduced(5).toFloat();
	g.drawLine(r.getTopLeft().x, r.getRelativePoint(0.f, .2f).y, r.getTopRight().x, r.getRelativePoint(0.f, .2f).y, .4f);
	g.drawLine(r.getTopLeft().x, r.getCentreY(), r.getBottomRight().x, r.getCentreY(), .4f);
	g.drawLine(r.getBottomLeft().x, r.getRelativePoint(0.f, .8f).y, r.getBottomRight().x, r.getRelativePoint(0.f, .8f).y, .4f);

}
