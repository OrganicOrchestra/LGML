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


#ifndef PARAMETERPROXYUI_H_INCLUDED
#define PARAMETERPROXYUI_H_INCLUDED

#include "../ParameterProxy.h"
#include "ParameterUI.h"
#include "../../ControllableUIHelpers.h"

class ParameterProxyUI :
    public ParameterUI,
    public ParameterProxy::ParameterProxyListener,
    public ControllableReferenceUI::Listener
{
public:
    ParameterProxyUI (ParameterProxy* proxy = nullptr);
    virtual ~ParameterProxyUI();

    ControllableReferenceUI chooser;

    WeakReference<ParameterProxy> paramProxy;

    ScopedPointer<Component> linkedParamUI;


    void resized()override ;

    void setLinkedParamUI ( ParameterBase*);



    virtual void linkedParamChanged (ParameterProxy* c) override;
    virtual void choosedControllableChanged (ControllableReferenceUI*, Controllable* c) override;

    virtual void controllableNameChanged (Controllable* c) override;

};



#endif  // PARAMETERPROXYUI_H_INCLUDED
