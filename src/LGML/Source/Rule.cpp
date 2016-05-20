/*
 ==============================================================================

 Rule.cpp
 Created: 4 May 2016 5:06:55pm
 Author:  bkupe

 ==============================================================================
 */

#include "Rule.h"
#include "RuleCondition.h"
#include "RuleConditionGroup.h"
#include "ScriptedCondition.h"
#include "RuleConsequence.h"
#include "ScriptedConsequence.h"


Rule::Rule(const String &_name) :
ControllableContainer(_name),
conditionType(NONE),
rootConditionGroup(nullptr),
scriptedCondition(nullptr)
{

    nameParam = addStringParameter("Name", "Name of the rule", _name);
    enabledParam = addBoolParameter("Enabled", "Enable / Disable the rule",true);

    isActiveParam = addBoolParameter("active", "Whether the rule is active or not.\nA rule is active if its conditions are validated.", false);
    isActiveParam->isControllableFeedbackOnly = true;

    setConditionType(ConditionType::SCRIPT);
}

Rule::~Rule()
{
	clear();
}

void Rule::setConditionType(ConditionType value)
{
    if (conditionType == value) return;

    switch (conditionType)
    {
        case SCRIPT:
			if(scriptedCondition != nullptr) scriptedCondition->removeConditionListener(this);
            scriptedCondition = nullptr;
            break;

        case VISUAL:
			if (rootConditionGroup != nullptr) rootConditionGroup->removeConditionGroupListener(this);
			rootConditionGroup = nullptr;

            break;
        default:
            break;
    }

    conditionType = value;

    switch (conditionType)
    {
        case SCRIPT:
            scriptedCondition = new ScriptedCondition(this);
			scriptedCondition->addConditionListener(this);
            break;

        case VISUAL:
            rootConditionGroup = new RuleConditionGroup(this,nullptr);
			rootConditionGroup->addConditionGroupListener(this);
            break;

        default:
            break;
    }

    ruleListeners.call(&RuleListener::ruleConditionTypeChanged, this);
}

ControlVariableReference * Rule::addReference()
{
    ControlVariableReference * cvr = new ControlVariableReference();
    references.add(cvr);
	cvr->addReferenceListener(this);
	ruleListeners.call(&RuleListener::referenceAdded, this, cvr);
    return cvr;
}

void Rule::removeReference(ControlVariableReference * cvr)
{
    cvr->removeReferenceListener(this);
    ruleListeners.call(&RuleListener::referenceRemoved, this, cvr);
    references.removeObject(cvr);
}


RuleConsequence * Rule::addConsequence()
{
    RuleConsequence * c = new ScriptedConsequence(this);
    consequences.add(c);
	c->addConsequenceListener(this);
    ruleListeners.call(&RuleListener::consequenceAdded,c);
	return c;
}

void Rule::removeConsequence(RuleConsequence * c)
{
    ruleListeners.call(&RuleListener::consequenceRemoved, c);
	c->removeConsequenceListener(this);
    consequences.removeObject(c);
}

void Rule::askForRemoveConsequence(RuleConsequence * c)
{
	removeConsequence(c);
}

void Rule::onContainerParameterChanged(Parameter * p)
{
    if (p == nameParam)
    {
        setNiceName(nameParam->stringValue());
    }else if (p == isActiveParam)
    {
        ruleListeners.call(&RuleListener::ruleActivationChanged, this);
    }
}

void Rule::askForRemoveReference(ControlVariableReference * r)
{
    removeReference(r);
}

void Rule::referenceAliasChanged(ControlVariableReference * r)
{
	ruleListeners.call(&RuleListener::referenceValueUpdate,this,r);
}

void Rule::referenceValueChanged(ControlVariableReference * r)
{
	ruleListeners.call(&RuleListener::referenceValueUpdate,this,r);
}

void Rule::conditionActivationChanged(RuleCondition *)
{
	isActiveParam->setValue((conditionType == SCRIPT)?scriptedCondition->isActive:rootConditionGroup->isActive());
}

bool Rule::isActive()
{
	return isActiveParam->boolValue();
}

void Rule::clear()
{
	scriptedCondition = nullptr;
	while (consequences.size() > 0)
	{
		consequences[0]->remove();
	}
}


void Rule::remove()
{
    ruleListeners.call(&RuleListener::askForRemoveRule, this);
}

var Rule::getJSONData()
{
	var data = ControllableContainer::getJSONData();


	var refData;
	for (auto &r : references) refData.append(r->getJSONData());
	data.getDynamicObject()->setProperty("references", refData);

	data.getDynamicObject()->setProperty("conditionType", (int)conditionType);
	if (conditionType == SCRIPT) data.getDynamicObject()->setProperty("scriptedCondition", scriptedCondition->getJSONData());
	else data.getDynamicObject()->setProperty("conditionGroup", rootConditionGroup->getJSONData());

	var consequencesData;
	for (auto &c : consequences) consequencesData.append(c->getJSONData());
	data.getDynamicObject()->setProperty("consequences", consequencesData);

	return data;
}

void Rule::loadJSONDataInternal(var data)
{
	Array<var> * refData = data.getDynamicObject()->getProperty("references").getArray();
	for (auto &rData : *refData)
	{
		ControlVariableReference *r = addReference();
		r->loadJSONData(rData);
	}

	setConditionType((ConditionType)(int)data.getDynamicObject()->getProperty("conditionType"));
	if (conditionType == SCRIPT) scriptedCondition->loadJSONData(data.getDynamicObject()->getProperty("scriptedCondition"));
	else rootConditionGroup->loadJSONData(data.getDynamicObject()->getProperty("conditionGroup"));

	Array<var> *  consequencesData = data.getDynamicObject()->getProperty("consequences").getArray();
	if (consequencesData != nullptr)
	{
		for (auto &cData : *consequencesData)
		{
			RuleConsequence * c = addConsequence();
			c->loadJSONData(cData);
		}
	}
}
