/*
  ==============================================================================

    RuleManagerUI.h
    Created: 4 May 2016 5:07:07pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef RULEMANAGERUI_H_INCLUDED
#define RULEMANAGERUI_H_INCLUDED

#include "RuleManager.h"
#include "RuleUI.h"
#include "ShapeShifterContent.h"

class RuleManagerUI : public ShapeShifterContent, public RuleManager::Listener
{
public:
	RuleManagerUI(const String &contentName, RuleManager * _ruleManager);
	virtual ~RuleManagerUI();


	RuleManager * ruleManager;

	OwnedArray<RuleUI> rulesUI;

	void clear();


	void resized() override;

 	void addRuleUI(Rule * r);
	void removeRuleUI(Rule * r);

	RuleUI * getUIForRule(Rule * r);


	// Inherited via Listener
	virtual void ruleAdded(Rule *) override;
	virtual void ruleRemoved(Rule *) override;

	void mouseDown(const MouseEvent &e) override;


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RuleManagerUI)
};


#endif  // RULEMANAGERUI_H_INCLUDED
