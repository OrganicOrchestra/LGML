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
	ControllableContainer("Fast Maps")
{

  potentialIn = addNewParameter<ParameterProxy>("potential Input","potential input for new fastMap");
  potentialOut = addNewParameter<ParameterProxy>("potential Output","potential output for new fastMap");

}

FastMapper::~FastMapper()
{
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


