/*
  ==============================================================================

    RuleConditionUI.cpp
    Created: 9 May 2016 3:03:17pm
    Author:  bkupe

  ==============================================================================
*/

#include "RuleConditionUI.h"

RuleConditionUI::RuleConditionUI(RuleCondition * _condition) :
	condition(_condition)
{
}

RuleConditionUI::~RuleConditionUI()
{

}

void RuleConditionUI::paint(Graphics & g)
{
	g.fillAll(condition->isActive ? Colours::lightgreen : Colours::grey);
}

void RuleConditionUI::conditionActivationChanged(RuleCondition *)
{
	repaint();
}
