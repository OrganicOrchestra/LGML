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

#pragma once

#include "../../Scripting/Js/JsEnvironmentUI.h"
#include "../UI/ConnectableNodeContentUI.h"

class JsNodeUI : public ConnectableNodeContentUI, private ControllableContainerListener,FileParameter::Listener
{


public:
    JsNodeUI();
    ~JsNodeUI();


    void init()override;

    void resized() override;

    void loadingEnded(FileParameter * ) override;
    ScopedPointer<JsEnvironmentUI> jsUI;

    OwnedArray<ParameterUI> varUI;
    void layoutUI();

    void childControllableAdded (ControllableContainer*, Controllable*) override;
    void childControllableRemoved (ControllableContainer*, Controllable*) override;


};





#if NON_INCREMENTAL_COMPILATION
#include "JsNodeUI.cpp"
#endif
