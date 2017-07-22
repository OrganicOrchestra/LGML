/*
  ==============================================================================

    ControllableUI.cpp
    Created: 9 Mar 2016 12:02:16pm
    Author:  bkupe

  ==============================================================================
*/

#include "ControllableUI.h"
#include "Controllable.h"

ControllableUI::ControllableUI(Controllable * controllable) :
    controllable(controllable),
Component(controllable->niceName)
{
  LGMLComponent::setLGMLElement(controllable);
    jassert(controllable!=nullptr);
    controllable->addControllableListener(this);
    updateTooltip();

}

ControllableUI::~ControllableUI()
{
    if(controllable.get())controllable->removeControllableListener(this);
}

void ControllableUI::controllableStateChanged(Controllable * c)
{
    setAlpha(c->enabled ? 1 : .5f);
}

void ControllableUI::controllableControlAddressChanged(Controllable *)
{
  updateTooltip();
}

void ControllableUI::mouseDown(const MouseEvent & e)
{
	if (e.mods.isRightButtonDown())
	{
		PopupMenu p;
		p.addItem(1, "Copy control address");
		int result = p.show();
		switch (result)
		{
		case 1:
			SystemClipboard::copyTextToClipboard(controllable->controlAddress);
			break;
		}
	}
}

void ControllableUI::updateTooltip()
{
    setTooltip(controllable->description + "\nControl Address : " + controllable->controlAddress);
}


//////////////////
// NamedControllableUI

NamedControllableUI::NamedControllableUI(ControllableUI * ui,int _labelWidth):
ControllableUI(ui->controllable),
ownedControllableUI(ui),
labelWidth(_labelWidth){

  addAndMakeVisible(controllableLabel);

  controllableLabel.setJustificationType(Justification::centredLeft);
  controllableLabel.setColour(Label::ColourIds::textColourId, TEXT_COLOR);
  controllableLabel.setText(ui->controllable->niceName, dontSendNotification);
  if(ui->controllable->isUserDefined){
    controllableLabel.setEditable(true);
  }

  controllableLabel.setTooltip(ControllableUI::getTooltip());

  addAndMakeVisible(ui);
  setBounds(ownedControllableUI->getBounds()
            .withTrimmedRight(-labelWidth)
            .withHeight(jmax((int)controllableLabel.getFont().getHeight() + 4,ownedControllableUI->getHeight())));
}

void NamedControllableUI::resized(){
  Rectangle<int> area  = getLocalBounds();
  controllableLabel.setBounds(area.removeFromLeft(labelWidth));
		area.removeFromLeft(10);
  ownedControllableUI->setBounds(area);
}

void NamedControllableUI::labelTextChanged (Label* labelThatHasChanged) {
  if(ownedControllableUI.get()){
    ownedControllableUI->controllable->setNiceName(labelThatHasChanged->getText());
  }
};
