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


#ifndef JSNODE_H_INCLUDED
#define JSNODE_H_INCLUDED
#include "../../Scripting/Js/JsEnvironment.h"
#include "../NodeBase.h"




class JsNode : public NodeBase, public JsEnvironment
{
public :
    DECLARE_OBJ_TYPE (JsNode,"let's you write some script in lgml");

    

    void clearNamespace() final;

    void buildLocalEnv() final;


    void onContainerParameterChanged ( ParameterBase* p) final;


    static var addIntParameter (const var::NativeFunctionArgs& a);
    static var addFloatParameter (const var::NativeFunctionArgs& a);
    static var addStringParameter (const var::NativeFunctionArgs& a);
    static var addBoolParameter (const var::NativeFunctionArgs& a);
    static var addTriggerParameter (const var::NativeFunctionArgs& a);



    Array<Controllable* > jsDynamicParameters;

    String getSubTypeName()final;

};



#endif  // JSNODE_H_INCLUDED
