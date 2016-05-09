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
	//selectedRule(nullptr)
{
}

RuleManager::~RuleManager()
{

}

Rule * RuleManager::addRule(const String &ruleName)
{
	Rule * r = new Rule(ruleName);
	rules.add(r);
	r->addRuleListener(this);

	listeners.call(&Listener::ruleAdded, r);

	//setSelectedRule(r);
	return r;
}

void RuleManager::removeRule(Rule * _rule)
{
	listeners.call(&Listener::ruleRemoved, _rule);
	rules.removeObject(_rule);
}

/*
void RuleManager::setSelectedRule(Rule * r)
{
	if (selectedRule == r) return;

	if (selectedRule != nullptr)
	{
		selectedRule->setSelected(false);
	}

	selectedRule = r;

	if (selectedRule != nullptr)
	{
		selectedRule->setSelected(true);
	}
}

void RuleManager::askForSelectRule(Rule * r)
{
	setSelectedRule(r);
}
*/

void RuleManager::askForRemoveRule(Rule * r)
{
	removeRule(r);
}
