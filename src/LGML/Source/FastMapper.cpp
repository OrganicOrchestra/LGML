/*
  ==============================================================================

    FastMapper.cpp
    Created: 17 May 2016 6:05:19pm
    Author:  bkupe

  ==============================================================================
*/

#include "FastMapper.h"

juce_ImplementSingleton(FastMapper)

FastMapper::FastMapper() :
	ControllableContainer("Fast Maps"),
selectedContainerToListenTo(nullptr)
{

  potentialIn = addNewParameter<ParameterProxy>("potential Input","potential input for new fastMap");
  potentialOut = addNewParameter<ParameterProxy>("potential Output","potential output for new fastMap");
  LGMLDragger::getInstance()->addSelectionListener(this);
  Inspector::getInstance()->addInspectorListener(this);
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
  if(potentialIn->get() && potentialOut->get()){
    addFastMap();

  }
}
void FastMapper::clear()
{
  while(maps.size()){
    removeFastmap(maps[0]);
  }
}

FastMap * FastMapper::addFastMap()
{
  
	FastMap * f = new FastMap();
	addChildControllableContainer(f);
	maps.add(f);
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

  if(selectedContainerToListenTo){
    selectedContainerToListenTo->removeControllableContainerListener(this);
  }
  selectedContainerToListenTo = newC;
  if(selectedContainerToListenTo){
    selectedContainerToListenTo->addControllableContainerListener(this);
  }

};


void FastMapper::controllableFeedbackUpdate(ControllableContainer *notif,Controllable *ori) {
  ControllableContainer::controllableFeedbackUpdate(notif,ori);
  if(notif==selectedContainerToListenTo && ori->getParameter()->isEditable){
//    MessageManager::getInstance()->callAsync([this,ori]()
                                             {setPotentialOutput(ori->getParameter());}
//                                             );
  }
};
