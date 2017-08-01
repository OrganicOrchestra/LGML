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

}

FastMapper::~FastMapper()
{
	clear();
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
	return f;
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


