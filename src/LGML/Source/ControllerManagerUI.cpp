/*
  ==============================================================================

    ControllerManagerUI.cpp
    Created: 8 Mar 2016 10:13:00pm
    Author:  bkupe

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "ControllerManagerUI.h"

#include "Style.h"
#include "ControllerFactory.h"

//==============================================================================
ControllerManagerUI::ControllerManagerUI(ControllerManager * manager) :
    ContourComponent(Colours::red),
    manager(manager)
{

    manager->addControllerListener(this);

}

ControllerManagerUI::~ControllerManagerUI()
{
    manager->removeControllerListener(this);
}



void ControllerManagerUI::controllerAdded(Controller * c)
{
    DBG("Controller added, addUI");
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

    placeElements();

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
    placeElements();
}

ControllerUI * ControllerManagerUI::getUIForController(Controller * controller)
{
    for (auto &cui : controllersUI)
    {
        if (cui->controller == controller) return cui;
    }

    return nullptr;
}

void ControllerManagerUI::placeElements()
{
    Rectangle<int> r = getLocalBounds().reduced(5);
    for (auto &cui : controllersUI)
    {
        r.setHeight(cui->getHeight());
        cui->setBounds(r);
        r.translate(0, cui->getHeight() + 10);
    }

    int targetHeight = jmax<int>(r.getTopLeft().y, getParentComponent()->getHeight());
    setSize(getWidth(), targetHeight);
}

void ControllerManagerUI::paint (Graphics&)
{
    //ContourComponent::paint(g);
}

void ControllerManagerUI::resized()
{

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
            if (event.mods.isCtrlDown())
            {
                manager->addController(ControllerFactory::ControllerType::OSCDirect);
            }
        }
    }

}

ControllerManagerViewport::ControllerManagerViewport(ControllerManager * controllerManager)
{
    cmui = new ControllerManagerUI(controllerManager);
    setViewedComponent(cmui);

}

void ControllerManagerViewport::paint(Graphics & g)
{
    g.setColour(BG_COLOR.darker());
    g.fillRoundedRectangle(getLocalBounds().toFloat(), 2);
}

void ControllerManagerViewport::resized()
{
    if (cmui->getHeight() < getHeight())
    {
        Rectangle<int> r = getLocalBounds();
        r.removeFromRight(18); //scrollbar
        cmui->setBounds(r);
    }
}
