/*
  ==============================================================================

    ShapeShifterPanelHeader.cpp
    Created: 3 May 2016 2:25:55pm
    Author:  bkupe

  ==============================================================================
*/

#include "ShapeShifterPanelHeader.h"
#include "Style.h"
#include "ShapeShifterFactory.h"
#include "ShapeShifterPanel.h"

ShapeShifterPanelHeader::ShapeShifterPanelHeader()
{
	addMouseListener(this, true);
  addPannelMenu.setButtonText("+");
  addAndMakeVisible(addPannelMenu);
  addPannelMenu.setConnectedEdges(0);
  addPannelMenu.setTooltip("add tab to this panel");
  addPannelMenu.addListener(this);
}

ShapeShifterPanelHeader::~ShapeShifterPanelHeader()
{

}

void ShapeShifterPanelHeader::addTab(ShapeShifterContent * content)
{
	ShapeShifterPanelTab * tab = new ShapeShifterPanelTab(content);
	attachTab(tab);
}

void ShapeShifterPanelHeader::removeTab(ShapeShifterPanelTab * tab, bool doRemove)
{
	tab->removeShapeShifterTabListener(this);
	removeChildComponent(tab);
	tabs.removeObject(tab,doRemove);
	resized();
}

void ShapeShifterPanelHeader::attachTab(ShapeShifterPanelTab * tab)
{
	tab->addShapeShifterTabListener(this);
	addAndMakeVisible(tab);
	tabs.add(tab);
	resized();
}

ShapeShifterPanelTab * ShapeShifterPanelHeader::getTabForContent(ShapeShifterContent * content)
{
	for (auto &t : tabs)
	{
		if (t->content == content) return t;
	}
	return nullptr;
}

void ShapeShifterPanelHeader::mouseDown(const MouseEvent & e)
{
	ShapeShifterPanelTab * tab = dynamic_cast<ShapeShifterPanelTab *>(e.eventComponent);
	if (tab != nullptr)
	{
		listeners.call(&Listener::tabSelect, tab);
	}

}

void ShapeShifterPanelHeader::mouseDrag(const MouseEvent & e)
{
	int minDetachDistanceY = 10;

	if (e.eventComponent == this)
	{
		if (abs(e.getDistanceFromDragStartY()) > minDetachDistanceY) listeners.call(&Listener::headerDrag);
	}else
	{
		ShapeShifterPanelTab * tab = dynamic_cast<ShapeShifterPanelTab *>(e.eventComponent);
		if (abs(e.getDistanceFromDragStartY()) > minDetachDistanceY && tab != nullptr) listeners.call(&Listener::tabDrag,tab);
	}

}

void ShapeShifterPanelHeader::paint(Graphics & g)
{
	g.fillAll(BG_COLOR.brighter(.1f));
}

void ShapeShifterPanelHeader::resized()
{
	//re arrange tabs
	Rectangle<int> r = getLocalBounds();
	if (r.getWidth() == 0 || r.getHeight() == 0) return;
  const int buttonSize = r.getHeight()-2;
  addPannelMenu.setBounds(r.removeFromRight(buttonSize).withSizeKeepingCentre(buttonSize,buttonSize));

	for (auto &t : tabs)
	{
		t->setBounds(r.removeFromLeft(jmin<int>(getWidth(),t->getLabelWidth())));
	}
}

void ShapeShifterPanelHeader::askForRemoveTab(ShapeShifterPanelTab * tab)
{
	listeners.call(&Listener::askForRemoveTab, tab);
}

void ShapeShifterPanelHeader::buttonClicked (Button* b){
  if(b==&addPannelMenu){
    PopupMenu menu;
    int currentID = 1;
    for (auto &n : globalPanelNames)
    {
      menu.addItem(currentID, n, true);
      currentID++;
    }
    int resID = menu.showMenu(PopupMenu::Options().withTargetComponent(&addPannelMenu));
    if(resID>0){
      String nameToAdd(globalPanelNames[resID-1]);
      auto content = ShapeShifterFactory::createContentForName(nameToAdd);
      if(ShapeShifterPanel * parent = dynamic_cast<ShapeShifterPanel*>(getParentComponent())){
        parent->addContent(content);
      }
    }
  }
}
