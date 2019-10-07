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


#ifndef MIDICONTROLLER_H_INCLUDED
#define MIDICONTROLLER_H_INCLUDED

#include "../Controller.h"

#include "../../Scripting/Js/JsEnvironment.h"

#include "../../JuceHeaderUI.h"

class JsKeyboardListener;

class KeyboardController :
public Controller,
public JsEnvironment,
private KeyListener
{
public :
    
    DECLARE_OBJ_TYPE_DEFAULTNAME (KeyboardController, "Keyboard","use your computer keyboard to control LGML")
    virtual ~KeyboardController();

    // should be implemented to build localenv
    void buildLocalEnv() override;

    BoolParameter* logIncoming;


    void    onContainerParameterChanged ( ParameterBase* )override;
    bool keyPressed (const KeyPress& key, Component* originatingComponent) override;
    bool keyStateChanged (const bool isKeyDown, Component* originatingComponent) override;

    



    static var createJsKeyboardListener (const var::NativeFunctionArgs&);


     OwnedArray<JsKeyboardListener, CriticalSection> jsKeyboardListener;

    

    // from jsenvironment
    void clearNamespace()override;

    struct KeyPressTime
    {
        KeyPress key;
        uint32 timeWhenPressed;
    };

    OwnedArray<KeyPressTime> keysDown;
private:




    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KeyboardController)
};

class JsKeyboardListener
{
public :
    JsKeyboardListener (JsEnvironment* js, char _keyToListen):
        jsEnv (js),
        keyToListen (_keyToListen)
    {
        buildVarObject();

    }

    static Identifier keyRecievedId;
    virtual ~JsKeyboardListener() {}
    void buildVarObject()
    {
        object = new DynamicObject();
        DynamicObject* dob = object.getDynamicObject();
        dob->setMethod (keyRecievedId, &JsKeyboardListener::dummyCallback);

    }

    // overriden in Js
#pragma warning(push)
#pragma warning(disable:4305 4800)
    static var dummyCallback (const var::NativeFunctionArgs& /*a*/)
    {
        return var::undefined();
    }
#pragma warning(pop)

    void processKeyPress (const KeyPress& )
    {
//        if (channel == 0 || channel == m.getChannel())
//        {
//            if ((isNoteListener && m.isNoteOnOrOff()) || (m.isController()))
//            {
//                int numToTest = isNoteListener ? m.getNoteNumber() : m.getControllerNumber();
//
//                if (numToTest == numberToListen)
//                {
//                    var value = isNoteListener ? m.getVelocity() : m.getControllerNumber();
//                    object.getDynamicObject()->setProperty (midiValueId, 0);
//                    jsEnv->callFunctionFromIdentifier (midiReceivedId, var::NativeFunctionArgs (object, &value, 1), true);
//                }
//
//            }
//        }
    }
    JsEnvironment* jsEnv;
    char keyToListen;
    var object;
};



#endif  // MIDICONTROLLER_H_INCLUDED
