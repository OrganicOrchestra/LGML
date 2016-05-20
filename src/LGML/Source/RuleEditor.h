/*
  ==============================================================================

    RuleEditor.h
    Created: 9 May 2016 3:02:21pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef RULEEDITOR_H_INCLUDED
#define RULEEDITOR_H_INCLUDED

#include "CustomEditor.h"

#include "RuleUI.h"
#include "RuleConditionUI.h"
#include "RuleConditionGroupUI.h"
#include "ScriptedConditionUI.h"
#include "RuleConsequenceUI.h"
#include "ControlVariableReferenceUI.h"

class RuleEditor :
	public CustomEditor,
	public ButtonListener,
	public RuleListener
{
public:
	RuleEditor(RuleUI * _ruleUI);
	virtual ~RuleEditor();

	Rule * rule;

	TextButton addReferenceBT;
	TextButton addConsequenceBT;

	Component referenceContainer;
	Component consequenceContainer;

	//layout ui
	const int listGap = 2;
	const int containerGap = 10;
	const int referenceHeight = 30;
	const int consequenceHeight = 200;
	const int containerMargin = 2;
	const int scriptedConditionHeight = 150;


	OwnedArray<ControlVariableReferenceUI> referencesUI;
	ScopedPointer<RuleConditionGroupUI> ruleConditionGroupUI;
	ScopedPointer<ScriptedConditionUI> scriptedConditionUI;

	OwnedArray<RuleConsequenceUI> consequencesUI;

	void paint(Graphics &g) override;
	void resized()override;

	int getContentHeight() override;

	void addReferenceUI(ControlVariableReference * r);
	void removeReferenceUI(ControlVariableReference * r);

	void addConsequenceUI(RuleConsequence * c);
	void removeConsequenceUI(RuleConsequence * c);

	ControlVariableReferenceUI * getUIForReference(ControlVariableReference * r);
	RuleConsequenceUI * getUIForConsequence(RuleConsequence *c);

	void referenceAdded(Rule *, ControlVariableReference *) override;
	void referenceRemoved(Rule *, ControlVariableReference *) override;

	void consequenceAdded(RuleConsequence *) override;
	void consequenceRemoved(RuleConsequence *) override;


	void buttonClicked(Button * b) override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RuleEditor)
};


#endif  // RULEEDITOR_H_INCLUDED
