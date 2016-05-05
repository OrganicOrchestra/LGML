/*
 ==============================================================================

 JavaScriptController.h
 Created: 5 May 2016 10:15:48am
 Author:  Martin Hermant

 ==============================================================================
 */

#ifndef JAVASCRIPTCONTROLLER_H_INCLUDED
#define JAVASCRIPTCONTROLLER_H_INCLUDED


#include "OSCDirectController.h"
#include "JavascriptEnvironment.h"

class JavascriptController : public OSCDirectController{
public:
    JavascriptController();

    void processMessage(const OSCMessage &m)override;
    void callForMessage(const OSCMessage & msg);


    static var sendOSCFromJS(const JavascriptEnvironment::NativeFunctionArgs& a);


private:
    String getJavaScriptFunctionName(const String &);
    ScopedPointer<JavascriptEnvironment> jsEnv;
    StringArray nonValidMessages;

    DynamicObject * createOSCJsObject();

};


#endif  // JAVASCRIPTCONTROLLER_H_INCLUDED
