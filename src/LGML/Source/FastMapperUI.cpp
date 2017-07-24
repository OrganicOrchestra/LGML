/*
  ==============================================================================

    FastMapperUI.cpp
    Created: 17 May 2016 6:05:39pm
    Author:  bkupe

  ==============================================================================
*/

#include "FastMapperUI.h"

FastMapperUI::FastMapperUI(FastMapper * _fastMapper, ControllableContainer * _viewFilterContainer) :
	fastMapper(_fastMapper), viewFilterContainer(_viewFilterContainer)
{
	fastMapper->addFastMapperListener(this);

	resetAndUpdateView();
}

FastMapperUI::~FastMapperUI()
{
	fastMapper->removeFastMapperListener(this);
	clear();

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


void FastMapperUI::resetAndUpdateView()
{
	removeAllChildren();
	mapsUI.clear();
	for (auto &f : fastMapper->maps)
	{
		if (mapPassViewFilter(f)) addFastMapUI(f);
	}
}

void FastMapperUI::setViewFilter(ControllableContainer * filterContainer)
{
	viewFilterContainer = filterContainer;
	resetAndUpdateView();
}

bool FastMapperUI::mapPassViewFilter(FastMap * f)
{
	if (viewFilterContainer == nullptr) return true;
//	if (f->reference.get() != nullptr && (ControllableContainer *)f->reference.->controller == viewFilterContainer) return true;
	if (f->referenceOut.get() != nullptr && viewFilterContainer->containsControllable(f->referenceOut.get())) return true;

	return false;
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
	while (mapsUI.size() > 0)
	{
		removeFastMapUI(mapsUI[0]->fastMap);
	}
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
class tst : CallbackMessage{
  
};

void FastMapperUI::fastMapAdded(FastMap *f )
{
  MessageManager::callAsync([this,f] (){ addFastMapUI(f);resized();});
//	addFastMapUI(f);
//	resized();
}

void FastMapperUI::fastMapRemoved(FastMap *f)
{
  MessageManager::callAsync([this,f] (){
	removeFastMapUI(f);
	resized();
});
}
