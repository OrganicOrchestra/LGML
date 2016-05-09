/*
  ==============================================================================

    RuleEditor.h
    Created: 9 May 2016 3:02:21pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef RULEEDITOR_H_INCLUDED
#define RULEEDITOR_H_INCLUDED

#include "JuceHeader.h"
#include "Rule.h"

class RuleEditor : public Component
{
public:
	juce_DeclareSingleton(RuleEditor,false)

	RuleEditor();
	virtual ~RuleEditor();

	Rule * currentRule;

	void setCurrentRule(Rule * r);
	void editRule(Rule * r);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RuleEditor)
};


#endif  // RULEEDITOR_H_INCLUDED
