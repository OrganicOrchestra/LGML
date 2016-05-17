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

void FastMapper::askForRemoveFastMap(FastMap * f)
{
	removeFastmap(f);
}
