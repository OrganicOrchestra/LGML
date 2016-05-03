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

ShapeShifterPanel::ShapeShifterPanel(ShapeShifterContent *_content, ShapeShifterPanelTab * sourceTab)
{

	addAndMakeVisible(header);
	header.addHeaderListener(this);

	if (sourceTab == nullptr)
	{
		header.addTab(_content);
		contents.add(_content);
		setCurrentContent(_content);
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
		removeChildComponent(_content);
	}

	currentContent = _content;


	if (currentContent != nullptr)
	{
		addAndMakeVisible(_content);
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
	Rectangle<int> tabBounds = content->getScreenBounds().withPosition(getScreenPosition());

	
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
	DBG("Detach tab, content width = " << content->getWidth());
	ShapeShifterManager::getInstance()->showPanelWindow(newPanel, tabBounds);
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
