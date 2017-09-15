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


#ifndef JSNODEEDITOR_H_INCLUDED
#define JSNODEEDITOR_H_INCLUDED

#include "../../Scripting/Js/JsEnvironmentUI.h"
#include "../UI/ConnectableNodeContentUI.h"

class JsNodeUI : public ConnectableNodeContentUI,private ControllableContainerListener,JsEnvironment::Listener{


public:
    JsNodeUI();
    ~JsNodeUI();


    void init()override;

    void resized() override;

    void newJsFileLoaded(bool )override;
    ScopedPointer<JsEnvironmentUI> jsUI;

    OwnedArray<ControllableUI> varUI;
    void layoutUI();

    void controllableAdded(ControllableContainer *,Controllable *) override;
    void controllableRemoved(ControllableContainer*,Controllable *) override;


};



#endif  // JSNODEEDITOR_H_INCLUDED
