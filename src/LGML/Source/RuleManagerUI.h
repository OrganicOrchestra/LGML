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
#include "ShapeShifterContent.h"

class RuleManagerUI : public ShapeShifterContent
{
public:
	RuleManagerUI(RuleManager * _ruleManager);
	virtual ~RuleManagerUI();

	void mouseDown(const MouseEvent &e) override;
	
	RuleManager * ruleManager;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RuleManagerUI)
};


#endif  // RULEMANAGERUI_H_INCLUDED
