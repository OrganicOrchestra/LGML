/*
  ==============================================================================

    MovablePanel.cpp
    Created: 2 May 2016 3:08:37pm
    Author:  bkupe

  ==============================================================================
*/

#include "ShapeShifterPanel.h"
#include "Style.h"

#include "ShapeShifterManager.h"

ShapeShifterPanel::ShapeShifterPanel(ShapeShifterContent *_content, ShapeShifterPanelTab * sourceTab) :
	currentContent(nullptr)
{
	addAndMakeVisible(header);
	header.addHeaderListener(this);

	if (sourceTab == nullptr)
	{
		addContent(_content);
	}else
	{
		attachTab(sourceTab);
	}

	
}

ShapeShifterPanel::~ShapeShifterPanel()
{
	header.removeHeaderListener(this);
	listeners.call(&Listener::panelRemoved, this);
}


void ShapeShifterPanel::setCurrentContent(ShapeShifterContent * _content)
{
	if (_content == currentContent) return;

	if (currentContent != nullptr)
	{
		ShapeShifterPanelTab * tab = header.getTabForContent(currentContent);
		if(tab != nullptr) tab->setSelected(false);
		removeChildComponent(currentContent);
	}


	currentContent = _content;

	if (currentContent != nullptr)
	{
		ShapeShifterPanelTab * tab = header.getTabForContent(currentContent);
		if (tab != nullptr) tab->setSelected(true);
		
		addAndMakeVisible(currentContent);
	}
	resized();
}

void ShapeShifterPanel::paint(Graphics & g)
{
	g.setColour(BG_COLOR);
	g.fillRect(getLocalBounds().withTrimmedTop(headerHeight));
}

void ShapeShifterPanel::resized()
{
	Rectangle<int> r = getLocalBounds();
	header.setBounds(r.removeFromTop(headerHeight));
	if (currentContent != nullptr)
	{
		currentContent->setBounds(r);
	}
}

void ShapeShifterPanel::attachTab(ShapeShifterPanelTab * tab)
{
	header.attachTab(tab);
	contents.add(tab->content);
	setCurrentContent(tab->content);
}

void ShapeShifterPanel::detachTab(ShapeShifterPanelTab * tab)
{
	ShapeShifterContent * content = tab->content;

	Rectangle<int> r = getScreenBounds();

	header.removeTab(tab,false);

	int cIndex = contents.indexOf(content);
	contents.removeAllInstancesOf(content);

	if (currentContent == content)
	{
		if (contents.size() > 0)
		{
			setCurrentContent(contents[jmax<int>(cIndex, 0)]);
		}else
		{
			listeners.call(&Listener::panelEmptied, this);
		}
	}

	ShapeShifterPanel * newPanel = ShapeShifterManager::getInstance()->createPanel(content,tab);
	ShapeShifterManager::getInstance()->showPanelWindow(newPanel,r);
}

void ShapeShifterPanel::addContent(ShapeShifterContent * content, bool setCurrent)
{
	header.addTab(content);
	contents.add(content);
	if(setCurrent) setCurrentContent(content);
}

void ShapeShifterPanel::removeTab(ShapeShifterPanelTab * tab)
{
	ShapeShifterContent * content = tab->content;
	header.removeTab(tab, true);

	int cIndex = contents.indexOf(content);
	contents.removeAllInstancesOf(content);

	if (currentContent == content)
	{
		if (contents.size() > 0)
		{
			setCurrentContent(contents[jmax<int>(cIndex, 0)]);
		}
		else
		{
			listeners.call(&Listener::panelEmptied, this);
		}
	}
}



void ShapeShifterPanel::tabDrag(ShapeShifterPanelTab * tab)
{
	if(!isDetached() || contents.size() > 1) detachTab(tab);
	else listeners.call(&Listener::tabDrag, this);
}

void ShapeShifterPanel::tabSelect(ShapeShifterPanelTab * tab)
{
	setCurrentContent(tab->content);
}

void ShapeShifterPanel::headerDrag()
{
	if (!isDetached()) listeners.call(&Listener::panelDetach, this);
	else listeners.call(&Listener::headerDrag, this);
}
