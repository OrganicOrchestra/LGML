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
#include "JsEnvironment.h"

class JavascriptController : public OSCDirectController ,public JsEnvironment{
public:
    JavascriptController();
    ~JavascriptController();
    bool processMessageInternal(const OSCMessage &m)override;
    Result callForMessage(const OSCMessage & msg);
    void callonAnyMsg(const OSCMessage & msg);


    static var sendOSCFromJS(const juce::var::NativeFunctionArgs& a);
    void onContainerParameterChanged(Parameter * p) override;


    void newJsFileLoaded()override;
    ControllerUI * createUI() override;


private:
    void buildLocalEnv() override;
    String getJavaScriptFunctionName(const String &);


    DynamicObject * createOSCJsObject();


    StringParameter * jsPath;
};


#endif  // JAVASCRIPTCONTROLLER_H_INCLUDED
