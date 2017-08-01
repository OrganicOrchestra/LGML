/*
 ==============================================================================

 ControllableUI.cpp
 Created: 9 Mar 2016 12:02:16pm
 Author:  bkupe

 ==============================================================================
 */

#include "ControllableUI.h"
#include "Controllable.h"
#include "LGMLDragger.h"
#include "FastMapper.h"

ControllableUI::ControllableUI(Controllable * controllable) :
controllable(controllable),
isMappingDest(false),
isDraggable(true),
isSelected(false)
{
  if(LGMLDragger::getInstance()->isMappingActive){
    mappingState = isMappingDest?MAPDEST:MAPSOURCE;
  }
  else{
    mappingState=NOMAP;
  }


  jassert(controllable!=nullptr);
  setName(controllable->niceName);
  controllable->addControllableListener(this);
  updateTooltip();

}

ControllableUI::~ControllableUI()
{
  LGMLDragger::getInstance()->unRegisterDragCandidate(this);
  if(controllable.get())controllable->removeControllableListener(this);
}

void ControllableUI::controllableStateChanged(Controllable * c)
{
  setAlpha(c->enabled ? 1 : .5f);
}

void ControllableUI::controllableControlAddressChanged(Controllable *)
{
  updateTooltip();
  repaint();
}

void ControllableUI::mouseDown(const MouseEvent & e)
{
  if (e.mods.isRightButtonDown())
  {
    PopupMenu p;
    p.addItem(1, "Copy control address");
    p.addItem(2, "Add FastMap To");
    p.addItem(3, "Add FastMap From");
    int result = p.show();
    switch (result)
    {
      case 1:
        SystemClipboard::copyTextToClipboard(controllable->controlAddress);
        break;
      case 2:
        FastMapper::getInstance()->addFastMap()->referenceOut->setParamToReferTo(controllable->getParameter());
        break;
      case 3:
        FastMapper::getInstance()->addFastMap()->referenceIn->setParamToReferTo(controllable->getParameter());
        break;
    }
  }
}

void ControllableUI::updateTooltip()
{
  setTooltip(controllable->description + "\nControl Address : " + controllable->controlAddress);
}


void  ControllableUI::setMappingState(const bool  b){
  MappingState s = b?(isMappingDest?MAPDEST:MAPSOURCE):NOMAP;
  if(s!=mappingState){
    if(s==NOMAP){setInterceptsMouseClicks(true, true);}
    else{
      setInterceptsMouseClicks(true, false);
    }
  }
  mappingState = s;
  repaint();
}
void ControllableUI::paintOverChildren(Graphics &g) {
  jassert(controllable!=nullptr );
  Component::paintOverChildren(g);
  if(mappingState!=NOMAP ){
    if(mappingState==MAPSOURCE){
      jassert(!isMappingDest);
      g.setColour(Colours::white.withAlpha(0.5f));
    }
    else{
      jassert(isMappingDest);
      g.setColour(Colours::blue.withAlpha(0.5f));
    }

    g.fillAll();
    if(isSelected){
      g.setColour(Colours::red);
      g.drawRect(getLocalBounds(),2);
    }
  }

}




void ControllableUI::setMappingDest(bool _isMappingDest){
  isMappingDest = _isMappingDest;
  if(mappingState!=NOMAP){
    mappingState = isMappingDest?MAPDEST:MAPSOURCE;
  }

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
    controllableLabel.addListener(this);
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
