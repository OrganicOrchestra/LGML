/*
  ==============================================================================

    ScriptedConditionUI.h
    Created: 13 May 2016 6:41:16pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef SCRIPTEDCONDITIONUI_H_INCLUDED
#define SCRIPTEDCONDITIONUI_H_INCLUDED

#include "ScriptedCondition.h"//keep
#include "RuleConditionUI.h"
#include "JsLiveEditor.h"

class ScriptedConditionUI :
	public RuleConditionUI,
	public Button::Listener
{
public :
	ScriptedConditionUI(ScriptedCondition * condition);
	virtual ~ScriptedConditionUI();

	ScriptedCondition * scriptedCondition;

	JsLiveEditor jsEditor;
	TextButton reloadBT;

	void resized() override;

	void buttonClicked(Button * b) override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScriptedConditionUI)

};



#endif  // SCRIPTEDCONDITIONUI_H_INCLUDED
