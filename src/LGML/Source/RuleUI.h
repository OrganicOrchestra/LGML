/*
  ==============================================================================

    RuleUI.h
    Created: 4 May 2016 5:07:18pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef RULEUI_H_INCLUDED
#define RULEUI_H_INCLUDED

#include "Rule.h"
#include "BoolToggleUI.h"
#include "StringParameterUI.h"
#include "SelectableComponent.h"

class RuleUI : public SelectableComponent, public ButtonListener, public Rule::RuleListener
{
public:
	RuleUI(Rule * _rule);
	virtual ~RuleUI();

	Rule * rule;

	ScopedPointer<BoolToggleUI> enabledBT;
	ScopedPointer<StringParameterUI> nameTF;
	ImageButton removeBT;

	void paint(Graphics &g) override;
	void resized() override;

	void mouseDown(const MouseEvent &e) override;

	void buttonClicked(Button * b);
	
	virtual void ruleActivationChanged(Rule *) override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RuleUI)

};



#endif  // RULEUI_H_INCLUDED
