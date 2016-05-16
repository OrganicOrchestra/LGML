/*
  ==============================================================================

    RuleManagerUI.cpp
    Created: 4 May 2016 5:07:07pm
    Author:  bkupe

  ==============================================================================
*/

#include "RuleManagerUI.h"
#include "MainComponent.h"

RuleManagerUI::RuleManagerUI(RuleManager * _ruleManager) :
	ruleManager(_ruleManager),
	ShapeShifterContent("Rules")
{
	ruleManager->addRuleManagerListener(this);
}

RuleManagerUI::~RuleManagerUI()
{
	ruleManager->removeRuleManagerListener(this);
}


void RuleManagerUI::resized()
{
	Rectangle<int> r = getLocalBounds().reduced(5);
	for (auto &rui : rulesUI)
	{
		rui->setBounds(r.removeFromTop(20));
		r.removeFromTop(3);
	}
}

void RuleManagerUI::addRuleUI(Rule * r)
{
	DBG("Add rule UI");
	RuleUI * rui = new RuleUI(r);
	rulesUI.add(rui);
	addAndMakeVisible(rui);

	rui->selectThis();

	resized();
}

void RuleManagerUI::removeRuleUI(Rule * r)
{
	RuleUI * rui = getUIForRule(r);
	if (rui == nullptr) return;

	removeChildComponent(rui);
	rulesUI.removeObject(rui);

	resized();
}

RuleUI * RuleManagerUI::getUIForRule(Rule * r)
{
	for (auto &rui : rulesUI)
	{
		if (rui->rule == r) return rui;
	}

	return nullptr;
}

void RuleManagerUI::ruleAdded(Rule * r)
{
	addRuleUI(r);
}

void RuleManagerUI::ruleRemoved(Rule * r)
{
	removeRuleUI(r);
}


void RuleManagerUI::mouseDown(const MouseEvent & e)
{

	if (e.mods.isRightButtonDown())
	{
		ScopedPointer<PopupMenu> menu(new PopupMenu());
		menu->addItem(1, "Add Rule");

		int result = menu->show();
		if (result == 1)
		{

			AlertWindow nameWindow("Create a new Rule", "Choose a name for the new rule", AlertWindow::AlertIconType::QuestionIcon, this);
			nameWindow.addTextEditor("newRuleName", "New Rule");
			nameWindow.addButton("OK", 1, KeyPress(KeyPress::returnKey));
			nameWindow.addButton("Cancel", 0, KeyPress(KeyPress::escapeKey));

			int nameResult = nameWindow.runModalLoop();

			if (nameResult)
			{
				String ruleName = nameWindow.getTextEditorContents("newRuleName");
				ruleManager->addRule(ruleName);
			}
		}
	}
	else
	{
		MainContentComponent::inspector->setCurrentComponent(nullptr);
	}
}
