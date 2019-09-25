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

#include "../../UI/ShapeShifter/ShapeShifterManager.h"
#include "../../Utils/FactoryUIHelpers.h"
#include "../../Controllable/ControllableUIHelpers.h"

//==============================================================================
ControllerManagerUI::ControllerManagerUI (const String & contentName, ControllerManager* _manager):
    manager (_manager),
InspectableComponent(_manager),
ShapeShifterContent (this,
                     contentName,
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
               false
               ,2,40)
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
//        infoLabel.setVisible(true);
        auto labelR =getLocalBounds().withBottom(addControllerBt.getY());
        labelR=labelR.withSizeKeepingCentre(jmax(250,getWidth()),labelR.getHeight());
//        infoLabel.setBounds(labelR);
    }
    else{
//        infoLabel.setVisible(false);
        auto rr =getLocalBounds();
        rr.removeFromRight(15); // scrollbar
        addControllerBt.setFromParentBounds (rr);
    }
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

            std::unique_ptr<PopupMenu> menu ( new PopupMenu());
            std::unique_ptr<PopupMenu> addNodeMenu ( FactoryUIHelpers::getFactoryTypesMenu<ControllerFactory>());
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
        std::unique_ptr<PopupMenu> addNodeMenu ( FactoryUIHelpers::getFactoryTypesMenu<ControllerFactory>());

        int result = addNodeMenu->show();

        if (result > 0 )
        {
            String t = FactoryUIHelpers::getFactoryTypeNameFromMenuIdx<FactoryBase<Controller>>(result);
            addControllerUndoable(t);
        }
    }

}


void addOrRemoveControllerUndoable(const String & typeId,Controller * controller,ControllerManager *manager){
    getAppUndoManager().beginNewTransaction(String(controller?"remove":"add")+" Controller :"+typeId);
    getAppUndoManager().perform(new
                                FactoryUIHelpers::UndoableFactoryCreateOrDelete<Controller>
                                (typeId,
                                 [=](Controller * c){manager->addController(c);},
                                 [=](Controller * c){manager->removeController(c);},
                                 controller,
                                 controller!=nullptr
                                 )
                                );
}
void ControllerManagerUI::addControllerUndoable(const String & typeId){
    addOrRemoveControllerUndoable(typeId,nullptr,manager);
}
void ControllerManagerUI::removeControllerUndoable(Controller * controller){
    const String typeId  =controller->getFactoryTypeId().toString();
   addOrRemoveControllerUndoable(typeId,controller,manager);
}

#endif
