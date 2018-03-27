/* Copyright © Organic Orchestra, 2017
 *
 * This file is part of LGML.  LGML is a software to manipulate sound in realtime
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation (version 3 of the License).
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */
#if !ENGINE_HEADLESS

#include "ControllerManagerUI.h"

#include "../../UI/Style.h"
#include "../ControllerFactory.h"
#include "../../UI/Inspector/Inspector.h"
#include "../../UI/ShapeShifter/ShapeShifterManager.h"
#include "../../Utils/FactoryUIHelpers.h"
#include "../../Controllable/ControllableUIHelpers.h"

//==============================================================================
ControllerManagerUI::ControllerManagerUI (const String & contentName, ControllerManager* _manager):
    manager (_manager),
ShapeShifterContentComponent (contentName,
                              "Communicate with the real world\nAdd your controllers here\n OSC / MIDI / Serial"),
controllersUI(new StackedContainerUI<ControllerUI, Controller>
              (
               [](ControllerUI *ui){return ui->controller;},
               [_manager](int ia, int ib){
                  int iia = _manager->controllableContainers.indexOf(_manager->controllers[ia]);
                  int iib = _manager->controllableContainers.indexOf(_manager->controllers[ib]);
                  _manager->controllableContainers.swap(iia,iib);
                  _manager->controllers.swap(ia,ib);
               },
               20,
               false)
              )
{

    addControllerBt.setTooltip (juce::translate("Add controller"));
    addControllerBt.addListener (this);

    manager->addControllerListener (this);
    addAndMakeVisible(controllersUI);
    for (auto& c : manager->controllers)
    {
        addControllerUI (c);
    }

    // should be last
    addAndMakeVisible (addControllerBt);

}

ControllerManagerUI::~ControllerManagerUI()
{
    manager->removeControllerListener (this);
    clear();
}

void ControllerManagerUI::clear()
{
    controllersUI.clear();

}


void ControllerManagerUI::controllerAdded (Controller* c)
{
    execOrDefer ([ = ]() { addControllerUI (c);});

}

void ControllerManagerUI::controllerRemoved (Controller* c)
{
    execOrDefer ([ = ]() { removeControllerUI (c);});

}

ControllerUI* ControllerManagerUI::addControllerUI (Controller* controller)
{
    
    if (controllersUI.getFromT(controller) != nullptr)
    {
        DBG ("Controller already exists");
        return nullptr;
    }

    {
        MessageManagerLock ml;

        if (auto cui = controllersUI.addFromT(controller)){
            cui->selectThis();
            resized();
            return cui;
        }
    }

    return nullptr;


}

void ControllerManagerUI::removeControllerUI (Controller* controller)
{
    {
        MessageManagerLock ml;
        controllersUI.removeFromT(controller);
    }

}



void ControllerManagerUI::resized()
{
    Rectangle<int> r = getLocalBounds().reduced (4,0);

    controllersUI.setBounds (r);

    if(controllersUI.getNumStacked()==0){
        int side = (int)( jmin(getWidth(),getHeight()) * .5);
        addControllerBt.setBounds(getLocalBounds().withSizeKeepingCentre(side,side));
        infoLabel.setVisible(true);
        auto labelR =getLocalBounds().withBottom(addControllerBt.getY());
        labelR=labelR.withSizeKeepingCentre(jmax(250,getWidth()),labelR.getHeight());
        infoLabel.setBounds(labelR);
    }
    else{
        infoLabel.setVisible(false);
        addControllerBt.setFromParentBounds (getLocalBounds());
    }
}

void ControllerManagerUI::paint (Graphics&)
{
    //ContourComponent::paint(g);
}

int ControllerManagerUI::getContentHeight() const
{

    return  controllersUI.getSize();


}


void ControllerManagerUI::mouseDown (const MouseEvent& event)
{
    if (event.eventComponent == this)
    {
        if (event.mods.isRightButtonDown())
        {

            ScopedPointer<PopupMenu> menu ( new PopupMenu());
            ScopedPointer<PopupMenu> addNodeMenu ( FactoryUIHelpers::getFactoryTypesMenu<ControllerFactory>());
            menu->addSubMenu ("Add Controller", *addNodeMenu);

            int result = menu->show();

            if (result > 0 )
            {
                String t = FactoryUIHelpers::getFactoryTypeNameFromMenuIdx<FactoryBase<Controller>>(result);
                addControllerUndoable(t);
            }
        }

    }
}




void ControllerManagerUI::buttonClicked (Button* b )
{
    if (b == &addControllerBt)
    {
        ScopedPointer<PopupMenu> addNodeMenu ( FactoryUIHelpers::getFactoryTypesMenu<ControllerFactory>());

        int result = addNodeMenu->show();

        if (result > 0 )
        {
            String t = FactoryUIHelpers::getFactoryTypeNameFromMenuIdx<FactoryBase<Controller>>(result);
            addControllerUndoable(t);
        }
    }

}

void ControllerManagerUI::addControllerUndoable(const String & typeId){
    getAppUndoManager().beginNewTransaction("add Controller :"+typeId);
    getAppUndoManager().perform(new
                                FactoryUIHelpers::UndoableFactoryCreate<Controller>
                                (typeId,
                                 [=](Controller * c){manager->addController(c);},
                                 [=](Controller * c){manager->removeController(c);}

                                )
                                );
}

#endif
