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
	ControllableContainer("FastMapper")
{
	saveAndLoadRecursiveData = false;
}

FastMapper::~FastMapper()
{
	clear();
}

void FastMapper::clear()
{
	while (maps.size() > 0)
	{
		maps[0]->remove();
	}
}

FastMap * FastMapper::addFastMap()
{
	FastMap * f = new FastMap();
	addChildControllableContainer(f);
	f->addFastMapListener(this);
	maps.add(f);
	fastMapperListeners.call(&FastMapperListener::fastMapAdded, f);
	return f;
}

void FastMapper::removeFastmap(FastMap * f)
{
	removeChildControllableContainer(f);
	f->removeFastMapListener(this);
	fastMapperListeners.call(&FastMapperListener::fastMapRemoved, f);
	maps.removeObject(f);
}

var FastMapper::getJSONData()
{
	var data = ControllableContainer::getJSONData();

	var mData;
	for (auto &f : maps)
	{
		mData.append(f->getJSONData());
	}
	data.getDynamicObject()->setProperty("fastMaps",mData);

	return data;
}

void FastMapper::loadJSONDataInternal(var data)
{
	Array<var> * mData = data.getDynamicObject()->getProperty("fastMaps").getArray();

	if (mData != nullptr)
	{
		for (auto &fData : *mData)
		{
			FastMap * f = addFastMap();
			f->loadJSONData(fData);
		}
	}
}

void FastMapper::askForRemoveFastMap(FastMap * f)
{
	removeFastmap(f);
}
