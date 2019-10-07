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


#ifndef JAVASCRIPTGLOBALENVIRONMENT_H_INCLUDED
#define JAVASCRIPTGLOBALENVIRONMENT_H_INCLUDED

#include "JsContainerSync.h"



// this class holds global namespace
// each JSEnvironment will be binded to this and so accessible from within each others

class JsGlobalEnvironment: public JsContainerSync
{
public:
    juce_DeclareSingleton (JsGlobalEnvironment, true)

    JsGlobalEnvironment();
    ~JsGlobalEnvironment();
    void removeNamespace (const String& ns);
    DynamicObject::Ptr getNamespaceObject (const String& ns);
    DynamicObject::Ptr getEnv() override;
    void clear();

private:



    static var post (const juce::var::NativeFunctionArgs& a);
    static var getMillis (const juce::var::NativeFunctionArgs& a);


    friend class JsEnvironment;

    var env;


};



#endif  // JAVASCRIPTGLOBALENVIRONMENT_H_INCLUDED
