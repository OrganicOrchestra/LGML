/*
  ==============================================================================

    FastMapperUI.cpp
    Created: 17 May 2016 6:05:39pm
    Author:  bkupe

  ==============================================================================
*/

#include "FastMapperUI.h"

FastMapperUI::FastMapperUI(FastMapper * _fastMapper) :
	fastMapper(_fastMapper)
{
	fastMapper->addFastMapperListener(this);
}

FastMapperUI::~FastMapperUI()
{
	fastMapper->removeFastMapperListener(this);
}

void FastMapperUI::addFastMapUI(FastMap * f)
{
	FastMapUI * fui = new FastMapUI(f);
	mapsUI.add(fui);
	addAndMakeVisible(fui);
	fastMapperUIListeners.call(&FastMapperUIListener::fastMapperContentChanged,this);
}

void FastMapperUI::removeFastMapUI(FastMap * f)
{
	FastMapUI * fui = getUIForFastMap(f);
	if (fui == nullptr) return;

	removeChildComponent(fui);
	mapsUI.removeObject(fui);

	fastMapperUIListeners.call(&FastMapperUIListener::fastMapperContentChanged, this);
}

FastMapUI * FastMapperUI::getUIForFastMap(FastMap *f)
{
	for (auto &fui : mapsUI)
	{
		if (fui->fastMap == f) return fui;
	}

	return nullptr;
}

int FastMapperUI::getContentHeight()
{
	return mapsUI.size() * (mapHeight + gap) + 4;
}

void FastMapperUI::resized()
{
	Rectangle<int> r = getLocalBounds().reduced(2);
	for (auto & fui : mapsUI)
	{
		fui->setBounds(r.removeFromTop(mapHeight));
		r.removeFromTop(gap);
	}
}

void FastMapperUI::clear()
{
}

void FastMapperUI::mouseDown(const MouseEvent & e)
{
	if (e.mods.isRightButtonDown())
	{
		PopupMenu m;
		m.addItem(1, "Add Fast Map");
		int result = m.show();
		switch (result)
		{
		case 1:
			fastMapper->addFastMap();
			break;
		}
	}
}

void FastMapperUI::fastMapAdded(FastMap *f )
{
	addFastMapUI(f);
	resized();
}

void FastMapperUI::fastMapRemoved(FastMap *f)
{
	removeFastMapUI(f);
	resized();
}