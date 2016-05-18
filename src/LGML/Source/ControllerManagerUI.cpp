/*
  ==============================================================================

    ControllerManagerUI.cpp
    Created: 8 Mar 2016 10:13:00pm
    Author:  bkupe

  ==============================================================================
*/

#include "ControllerManagerUI.h"

#include "Style.h"
#include "ControllerFactory.h"
#include "Inspector.h"
#include "ShapeShifterManager.h"

//==============================================================================
ControllerManagerUI::ControllerManagerUI(const String &contentName, ControllerManager * manager) :
	ShapeShifterContent(contentName),
    manager(manager)
{

    manager->addControllerListener(this);

	for (auto &c : manager->controllers)
	{
		addControllerUI(c);
	}

}

ControllerManagerUI::~ControllerManagerUI()
{
    manager->removeControllerListener(this);
	clear();
}

void ControllerManagerUI::clear()
{
	while(controllersUI.size() > 0)
	{
		removeControllerUI(controllersUI[0]->controller);
	}
}


void ControllerManagerUI::controllerAdded(Controller * c)
{
	addControllerUI(c);
}

void ControllerManagerUI::controllerRemoved(Controller * c)
{
    removeControllerUI(c);
}

ControllerUI * ControllerManagerUI::addControllerUI(Controller * controller)
{
    if (getUIForController(controller) != nullptr)
    {
        DBG("Controller already exists");
        return nullptr;
    }

	ControllerUI * cui = controller->createUI();
    controllersUI.add(cui);
    addAndMakeVisible(cui);

	cui->selectThis();

	resized();
    return cui;
}

void ControllerManagerUI::removeControllerUI(Controller * controller)
{
    ControllerUI * cui = getUIForController(controller);
    if (cui == nullptr)
    {
        DBG("Controller not exist");
        return;
    }

    controllersUI.removeObject(cui);
    removeChildComponent(getIndexOfChildComponent(cui));
    resized();
}

ControllerUI * ControllerManagerUI::getUIForController(Controller * controller)
{
    for (auto &cui : controllersUI)
    {
        if (cui->controller == controller) return cui;
    }

    return nullptr;
}

void ControllerManagerUI::resized()
{
	Rectangle<int> r = getLocalBounds().reduced(5);
	int gap = 5;
    for (auto &cui : controllersUI)
    {
		cui->setBounds(r.removeFromTop(20));
		r.removeFromTop(gap);
    }
}

void ControllerManagerUI::paint (Graphics&)
{
    //ContourComponent::paint(g);
}


void ControllerManagerUI::mouseDown(const MouseEvent & event)
{
    if (event.eventComponent == this)
    {
        if (event.mods.isRightButtonDown())
        {

            ScopedPointer<PopupMenu> menu( new PopupMenu());
            ScopedPointer<PopupMenu> addNodeMenu( ControllerFactory::getControllerTypesMenu(0));
            menu->addSubMenu("Add Controller", *addNodeMenu);

            int result = menu->show();
            if (result >= 1 && result <= addNodeMenu->getNumItems())
            {
                manager->addController((ControllerFactory::ControllerType)(result - 1));
            }
		}
		else
		{
			if (Inspector::getInstanceWithoutCreating() != nullptr) Inspector::getInstance()->setCurrentComponent(nullptr);
		}
    }
}

