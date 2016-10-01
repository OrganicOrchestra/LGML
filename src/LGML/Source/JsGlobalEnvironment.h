/*
  ==============================================================================

    JsGlobalEnvironment.h
    Created: 9 May 2016 5:13:16pm
    Author:  Martin Hermant

  ==============================================================================
*/

#ifndef JAVASCRIPTGLOBALENVIRONMENT_H_INCLUDED
#define JAVASCRIPTGLOBALENVIRONMENT_H_INCLUDED

#include "JsContainerSync.h"



// this class holds global namespace
// each JSEnvironment will be binded to this and so accessible from within each others

class JsGlobalEnvironment: public JsContainerSync
{
public:
    juce_DeclareSingleton(JsGlobalEnvironment, true);

    JsGlobalEnvironment();

    void removeNamespace(const String & ns);
    DynamicObject * getNamespaceObject(const String & ns);
    DynamicObject * getEnv() override;


private:

	

    static var post(const juce::var::NativeFunctionArgs& a);
	static var getMillis(const juce::var::NativeFunctionArgs& a);


    friend class JsEnvironment;

    var env;

	
};



#endif  // JAVASCRIPTGLOBALENVIRONMENT_H_INCLUDED
