/*
  ==============================================================================

    ScriptedConsequenceUI.h
    Created: 13 May 2016 6:41:05pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef SCRIPTEDCONSEQUENCEUI_H_INCLUDED
#define SCRIPTEDCONSEQUENCEUI_H_INCLUDED

#include "RuleConsequenceUI.h"
#include "ScriptedConsequence.h"//keep
#include "JsLiveEditor.h"

class ScriptedConsequenceUI : public RuleConsequenceUI, public Button::Listener
{
public:
	ScriptedConsequenceUI(ScriptedConsequence * consequence);
	virtual ~ScriptedConsequenceUI();

	ScriptedConsequence * scriptedConsequence;
	JsLiveEditor jsEditor;
	TextButton reloadBT;

	void resized() override;

	void buttonClicked(Button * b) override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScriptedConsequenceUI)

};



#endif  // SCRIPTEDCONSEQUENCEUI_H_INCLUDED
