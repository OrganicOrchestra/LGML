/*
  ==============================================================================

    RuleManagerUI.cpp
    Created: 4 May 2016 5:07:07pm
    Author:  bkupe

  ==============================================================================
*/

#include "RuleManagerUI.h"

RuleManagerUI::RuleManagerUI(RuleManager * _ruleManager) :
	ruleManager(_ruleManager),
	ShapeShifterContent("Rules")
{
}

RuleManagerUI::~RuleManagerUI()
{
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
}
