/*
  ==============================================================================

    ControlRuleManagerUI.h
    Created: 4 May 2016 5:07:07pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef CONTROLRULEMANAGERUI_H_INCLUDED
#define CONTROLRULEMANAGERUI_H_INCLUDED

#include "ControlRuleManager.h"
#include "ShapeShifterContent.h"

class ControlRuleManagerUI : public ShapeShifterContent
{
public:
	ControlRuleManagerUI(ControlRuleManager * crm);
	virtual ~ControlRuleManagerUI();

	ControlRuleManager * crm;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ControlRuleManagerUI)
};


#endif  // CONTROLRULEMANAGERUI_H_INCLUDED
