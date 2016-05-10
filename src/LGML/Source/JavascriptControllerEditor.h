/*
  ==============================================================================

    JavascriptControllerEditor.h
    Created: 10 May 2016 8:01:18pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef JAVASCRIPTCONTROLLEREDITOR_H_INCLUDED
#define JAVASCRIPTCONTROLLEREDITOR_H_INCLUDED

#include "OSCControllerEditor.h"
#include "JavascriptControllerUI.h"

class JsEnvironmentUI;

class JavascriptControllerEditor : public OSCControllerEditor
{
public:
	JavascriptControllerEditor(JavascriptControllerUI * jsControllerUI);
	virtual ~JavascriptControllerEditor();

	JavascriptController * jsController;

	ScopedPointer<JsEnvironmentUI> scriptUI;

	void resizedInternalOSC(Rectangle<int> bounds);



	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(JavascriptControllerEditor)
};


#endif  // JAVASCRIPTCONTROLLEREDITOR_H_INCLUDED
