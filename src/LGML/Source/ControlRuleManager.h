/*
  ==============================================================================

    ControlRuleManager.h
    Created: 4 May 2016 5:05:33pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef CONTROLRULEMANAGER_H_INCLUDED
#define CONTROLRULEMANAGER_H_INCLUDED

#include "JuceHeader.h"

class ControlRuleManager
{
public:
	juce_DeclareSingleton(ControlRuleManager, true)

	ControlRuleManager();
	virtual ~ControlRuleManager();


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ControlRuleManager)
};



#endif  // CONTROLRULEMANAGER_H_INCLUDED
