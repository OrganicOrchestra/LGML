/* Copyright © Organic Orchestra, 2017
*
* This file is part of LGML.  LGML is a software to manipulate sound in realtime
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation (version 3 of the License).
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
*/


#include "FastMapper.h"

juce_ImplementSingleton(FastMapper)

FastMapper::FastMapper() :
ControllableContainer("Fast Maps"),
selectedContainerToListenTo(nullptr)
{

  potentialIn = addNewParameter<ParameterProxy>("potential Input","potential input for new fastMap");
  potentialOut = addNewParameter<ParameterProxy>("potential Output","potential output for new fastMap");
  autoAdd = addNewParameter<BoolParameter>("auto_add", "auto add param when In/Out potentials are set",true);
  LGMLDragger::getInstance()->addSelectionListener(this);
  Inspector::getInstance()->addInspectorListener(this);
  potentialIn->isSavable = false;
  potentialOut->isSavable = false;
  potentialIn->isPresettable = false;
  potentialOut->isPresettable = false;

}

FastMapper::~FastMapper()
{
  if(auto * dr = LGMLDragger::getInstanceWithoutCreating()){
    dr->removeSelectionListener(this);
  }
  if(auto * i = Inspector::getInstanceWithoutCreating()){
    i->removeInspectorListener(this);
  }
  clear();
}



void FastMapper::setPotentialInput(Parameter* p){
  potentialIn->setParamToReferTo(p);
  createNewFromPotentials();
}
void FastMapper::setPotentialOutput(Parameter* p ){
  potentialOut->setParamToReferTo(p);
  createNewFromPotentials();
}
void FastMapper::createNewFromPotentials(){
  if(potentialIn->get() && potentialOut->get() && autoAdd->boolValue()){
    addFastMap();

  }
}
void FastMapper::clear()
{
  while(maps.size()){
    removeFastmap(maps[0]);
  }
  potentialOut->setParamToReferTo(nullptr);
  potentialIn->setParamToReferTo(nullptr);

}

FastMap * FastMapper::addFastMap()
{

  FastMap * f = new FastMap();
  addChildControllableContainer(f);
  f->nameParam->isEditable = true;
  maps.add(f);
  setContainerToListen(nullptr);
  f->referenceIn->setParamToReferTo(potentialIn->get());
  f->referenceOut->setParamToReferTo(potentialOut->get());

  potentialIn->setParamToReferTo(nullptr);
  potentialOut->setParamToReferTo(nullptr);
  checkDuplicates(f);
  return f;
}

bool FastMapper::checkDuplicates(FastMap *f){
  bool dup = false;
  for(auto & ff:maps){
    if(ff==f)continue;
    if(ff->referenceIn->get() == f->referenceIn->get() &&
       ff->referenceOut->get()==f->referenceOut->get()){
      dup = true;
    }
    else if(ff->referenceIn->get() == f->referenceOut->get() &&
            ff->referenceOut->get()==f->referenceIn->get()){
      dup = true;
    }
    if(dup){
      removeFastmap(f);
      return true;

    }
  }
  return false;
}

void FastMapper::removeFastmap(FastMap * f)
{
  jassert(f);
  removeChildControllableContainer(f);
  maps.removeObject(f);
}

var FastMapper::getJSONData()
{
  var data = ControllableContainer::getJSONData();

  //	var mData;
  //	for (auto &f : maps)
  //	{
  //		mData.append(f->getJSONData());
  //	}
  //	data.getDynamicObject()->setProperty("fastMaps",mData);
  //
  return data;
}

ControllableContainer *  FastMapper::addContainerFromVar(const String & name,const var & fData)
{
  FastMap * f = addFastMap();

  return f;


}




void FastMapper::selectionChanged(Controllable *c ) {
  // getParameter is safe to call on null pointer as it's only a cast
  setPotentialInput(c->getParameter());

};

void FastMapper::currentComponentChanged(Inspector * i) {
  auto * newC = i->getCurrentSelected();
  if(newC==selectedContainerToListenTo)return;
  setContainerToListen(newC);

};

void FastMapper::setContainerToListen(ControllableContainer *newC){
  if(selectedContainerToListenTo){
    selectedContainerToListenTo->removeControllableContainerListener(this);
  }
  selectedContainerToListenTo = newC;
  if(selectedContainerToListenTo){
    selectedContainerToListenTo->addControllableContainerListener(this);
  }
}
void FastMapper::controllableFeedbackUpdate(ControllableContainer *notif,Controllable *ori) {
  ControllableContainer::controllableFeedbackUpdate(notif,ori);
  if(notif==selectedContainerToListenTo && ori->getParameter()->isEditable && LGMLDragger::getInstance()->isMappingActive){
    setPotentialOutput(ori->getParameter());
  }
};
