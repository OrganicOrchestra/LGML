/*
  ==============================================================================

    RuleManager.cpp
    Created: 4 May 2016 5:05:33pm
    Author:  bkupe

  ==============================================================================
*/

#include "RuleManager.h"
#include "Rule.h"

juce_ImplementSingleton(RuleManager)

RuleManager::RuleManager() :
	ControllableContainer("Rule")
{
	saveAndLoadRecursiveData = false;
}

RuleManager::~RuleManager()
{
	clear();
}

Rule * RuleManager::addRule(const String &ruleName)
{
	Rule * r = new Rule(ruleName);
	rules.add(r);
	r->addRuleListener(this);

	r->nameParam->setValue(getUniqueNameInContainer(r->nameParam->stringValue()));
	addChildControllableContainer(r);

	ruleManagerListeners.call(&RuleManager::Listener::ruleAdded, r);

	//setSelectedRule(r);
	return r;
}

void RuleManager::removeRule(Rule * _rule)
{
	ruleManagerListeners.call(&RuleManager::Listener::ruleRemoved, _rule);
	removeChildControllableContainer(_rule);
	rules.removeObject(_rule);
}


void RuleManager::askForRemoveRule(Rule * r)
{
	removeRule(r);
}

void RuleManager::clear()
{
	while (rules.size() > 0)
	{
		rules[0]->remove();
	}
}

var RuleManager::getJSONData()
{
	var data = ControllableContainer::getJSONData();

	var rulesData;
	for (auto &r : rules)
	{
		rulesData.append(r->getJSONData());
	}

	data.getDynamicObject()->setProperty("rules", rulesData);

	return data;
}

void RuleManager::loadJSONDataInternal(var data)
{
	clear();

	Array<var> * rulesData = data.getDynamicObject()->getProperty("rules").getArray();
	for (auto &rData : *rulesData)
	{
		Rule * r = addRule(rData.getDynamicObject()->getProperty("name"));
		r->loadJSONData(rData);
	}
}
