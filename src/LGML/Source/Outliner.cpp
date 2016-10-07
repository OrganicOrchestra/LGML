/*
  ==============================================================================

    Outliner.cpp
    Created: 7 Oct 2016 10:31:23am
    Author:  bkupe

  ==============================================================================
*/

#include "Outliner.h"
#include "Engine.h"
#include "Style.h"

Engine& getEngine();

Outliner::Outliner(const String &contentName) : ShapeShifterContent(contentName)
{
	getEngine().addControllableContainerListener(this);

	
	rootItem = new OutlinerItem(&getEngine());
	treeView.setRootItem(rootItem);
	addAndMakeVisible(treeView);

	rebuildTree();
}

Outliner::~Outliner()
{
	getEngine().removeControllableContainerListener(this);
	
}

void Outliner::resized()
{
	treeView.setBounds(getLocalBounds());
}

void Outliner::paint(Graphics & g)
{
	//g.fillAll(Colours::green.withAlpha(.2f));
}


void Outliner::rebuildTree()
{
	rootItem->clearSubItems();
	buildTree(rootItem, &getEngine());
	rootItem->setOpen(true);
}

void Outliner::buildTree(OutlinerItem * parentItem, ControllableContainer * parentContainer)
{
	Array<ControllableContainer *> childContainers = parentContainer->controllableContainers;
	for (auto &cc : childContainers)
	{
		OutlinerItem * ccItem = new OutlinerItem(cc);
		parentItem->addSubItem(ccItem);
		
		buildTree(ccItem, cc);
		
	}
	
	Array<Controllable * > childControllables = parentContainer->getAllControllables(false);

	for (auto &c : childControllables)
	{

		OutlinerItem * cItem = new OutlinerItem(c);
		parentItem->addSubItem(cItem);

	}
}

void Outliner::childStructureChanged(ControllableContainer *)
{
	rebuildTree();
}


// OUTLINER ITEM

OutlinerItem::OutlinerItem(ControllableContainer * _container) :
	container(_container), controllable(nullptr), isContainer(true), inspectable(dynamic_cast<InspectableComponent *>(_container))
{
}

OutlinerItem::OutlinerItem(Controllable * _controllable) :
	container(nullptr), controllable(_controllable), isContainer(false), inspectable(dynamic_cast<InspectableComponent *>(_controllable))
{
}

OutlinerItem::~OutlinerItem()
{
}

void OutlinerItem::paintItem(Graphics & g, int width, int height)
{
	
	
}

bool OutlinerItem::mightContainSubItems()
{
	return false;
}

void OutlinerItem::itemClicked(const MouseEvent & e)
{
}

Component * OutlinerItem::createItemComponent()
{
	return new OutlinerItemComponent(this);
}

OutlinerItemComponent::OutlinerItemComponent(OutlinerItem * _item) : 
	InspectableComponent(_item->container),
	item(_item)
{

}

void OutlinerItemComponent::paint(Graphics & g)
{
	Rectangle<int> r = getLocalBounds();
	r.removeFromLeft(3);
	if (item->isSelected())
	{
		g.fillAll(item->isContainer? HIGHLIGHT_COLOR :TEXT_COLOR);
	}

	g.setColour(item->isSelected()?Colours::grey.darker():(item->isContainer ? HIGHLIGHT_COLOR : TEXT_COLOR));
	g.drawFittedText(item->isContainer ? item->container->niceName : item->controllable->niceName, r, Justification::left, 1);
}

void OutlinerItemComponent::mouseDown(const MouseEvent & e)
{
	item->setSelected(true, true);
	if (item->isContainer) selectThis();
}
