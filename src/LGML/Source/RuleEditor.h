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
#include "RuleConsequenceUI.h"


class RuleEditor : public CustomEditor
{
public:
	RuleEditor(RuleUI * _ruleUI);
	virtual ~RuleEditor();

	Rule * rule;
	
	Component conditionContainer;
	Component consequenceContainer;

	OwnedArray<RuleConditionGroupUI> conditionGroupsUI;
	OwnedArray<RuleConsequenceUI> consequencesUI;

	void paint(Graphics &g) override;
	void resized()override;

	void addConditionGroupUI(RuleConditionGroup * c);
	void addConsequenceUI(RuleConsequence * c);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RuleEditor)
};


#endif  // RULEEDITOR_H_INCLUDED
