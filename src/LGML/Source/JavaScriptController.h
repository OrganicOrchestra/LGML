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
    JavascriptController(JavascriptEnvironment *);

    void callForMessage(const String & msg);


private:
    String getJavaScriptFunctionName(const String &);
    ScopedPointer<JavascriptEnvironment> jsEnv;

};


#endif  // JAVASCRIPTCONTROLLER_H_INCLUDED
