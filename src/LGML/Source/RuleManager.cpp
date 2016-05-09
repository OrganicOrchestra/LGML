/*
  ==============================================================================

    RuleManager.cpp
    Created: 4 May 2016 5:05:33pm
    Author:  bkupe

  ==============================================================================
*/

#include "RuleManager.h"

juce_ImplementSingleton(RuleManager)

RuleManager::RuleManager()
{
}

RuleManager::~RuleManager()
{
}

Rule * RuleManager::addRule(const String &ruleName)
{
	Rule * r = new Rule(ruleName);
	rules.add(r);
	return r;
}

void RuleManager::removeRule(Rule * _rule)
{
	rules.removeObject(_rule);
}
