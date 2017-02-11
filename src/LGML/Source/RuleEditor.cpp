/*
  ==============================================================================

    RuleEditor.cpp
    Created: 9 May 2016 3:02:21pm
    Author:  bkupe

  ==============================================================================
*/

#include "RuleEditor.h"

RuleEditor::RuleEditor(RuleUI * _ruleUI) :
	InspectorEditor(),
	rule(_ruleUI->rule),
	addReferenceBT("Add Reference"),
	addConsequenceBT("Add Consequence"),
	ruleConditionGroupUI(nullptr),
	scriptedConditionUI(nullptr)
{

	addReferenceBT.addListener(this);
	addConsequenceBT.addListener(this);

	addAndMakeVisible(&addReferenceBT);

	addAndMakeVisible(&referenceContainer);

	for (auto &r : rule->references)
	{
		addReferenceUI(r);
	}

	if (rule->conditionType == Rule::ConditionType::VISUAL)
	{
		ruleConditionGroupUI = new RuleConditionGroupUI(rule->rootConditionGroup);
		addAndMakeVisible(ruleConditionGroupUI);
	}
	else
	{
		scriptedConditionUI = new ScriptedConditionUI(rule->scriptedCondition);
		addAndMakeVisible(scriptedConditionUI);
	}


	addAndMakeVisible(&addConsequenceBT);
	addAndMakeVisible(&consequenceContainer);
	for (auto &c : rule->consequences)
	{
		addConsequenceUI(c);
	}

	rule->addRuleListener(this);
}

RuleEditor::~RuleEditor()
{
	rule->removeRuleListener(this);
}


void RuleEditor::paint(Graphics &g)
{
	//g.setColour(Colours::purple);
	//g.fillRoundedRectangle(getLocalBounds().toFloat(),8);

	g.setColour(BG_COLOR.brighter(.2f));
	g.drawRoundedRectangle(referenceContainer.getBounds().expanded(2).toFloat(),2,2);
	g.drawRoundedRectangle(consequenceContainer.getBounds().expanded(2).toFloat(),2,2);
}

void RuleEditor::resized()
{
	Rectangle<int> r = getLocalBounds();


	addReferenceBT.setBounds(r.removeFromTop(20));
	r.removeFromTop(containerGap);
	referenceContainer.setBounds(r.removeFromTop(referencesUI.size()*(referenceHeight + listGap)).reduced(containerMargin));

	Rectangle<int> rr = referenceContainer.getLocalBounds();
	for (auto & refUI : referencesUI)
	{
		refUI->setBounds(rr.removeFromTop(referenceHeight));
		rr.removeFromTop(listGap);
	}

	r.removeFromTop(containerGap);

	if (ruleConditionGroupUI != nullptr)
	{
		ruleConditionGroupUI->setBounds(r.removeFromTop(50));//will be dynamic
	}

	if (scriptedConditionUI != nullptr)
	{
		scriptedConditionUI->setBounds(r.removeFromTop(scriptedConditionHeight));
	}

	r.removeFromTop(containerGap);

	addConsequenceBT.setBounds(r.removeFromTop(20));

	r.removeFromTop(containerGap);
	consequenceContainer.setBounds(r.removeFromTop(consequencesUI.size()*(consequenceHeight + listGap)).reduced(containerMargin));

	Rectangle<int> cr = consequenceContainer.getLocalBounds();
	for (auto & cUI : consequencesUI)
	{
		cUI->setBounds(cr.removeFromTop(consequenceHeight));
		rr.removeFromTop(listGap);
	}
;}

int RuleEditor::getContentHeight()
{
	int tH = 0;
	tH += 20 * 2; //buttons
	tH += containerGap * 6; //
	tH += referencesUI.size()*(referenceHeight + listGap);

	if(ruleConditionGroupUI != nullptr) tH += ruleConditionGroupUI->getHeight();
	if (scriptedConditionUI != nullptr) tH += scriptedConditionHeight;

	tH += consequencesUI.size()*(consequenceHeight + listGap);
	tH += containerMargin * 4;

	return tH;
}

void RuleEditor::addReferenceUI(ControlVariableReference * r)
{
	ControlVariableReferenceUI * rui = new ControlVariableReferenceUI(r);
	referencesUI.add(rui);
	referenceContainer.addAndMakeVisible(rui);
}

void RuleEditor::removeReferenceUI(ControlVariableReference * r)
{
	ControlVariableReferenceUI * rui = getUIForReference(r);
	if (rui == nullptr) return;

	referenceContainer.removeChildComponent(rui);
	referencesUI.removeObject(rui);

	resized();
}

void RuleEditor::addConsequenceUI(RuleConsequence * c)
{
	RuleConsequenceUI * cui = c->createUI();
	consequencesUI.add(cui);
	consequenceContainer.addAndMakeVisible(cui);

}

void RuleEditor::removeConsequenceUI(RuleConsequence * c)
{
	RuleConsequenceUI * cui = getUIForConsequence(c);
	if (cui == nullptr) return;

	consequenceContainer.removeChildComponent(cui);
	consequencesUI.removeObject(cui);

}

ControlVariableReferenceUI * RuleEditor::getUIForReference(ControlVariableReference * r)
{
	for (auto &rui : referencesUI)
	{
		if (rui->cvr == r) return rui;
	}

	return nullptr;
}

RuleConsequenceUI * RuleEditor::getUIForConsequence(RuleConsequence * c)
{
	for (auto &cui : consequencesUI)
	{
		if (cui->consequence == c) return cui;
	}

	return nullptr;
}

void RuleEditor::referenceAdded(Rule *, ControlVariableReference * cvr)
{
	addReferenceUI(cvr);
	inspectorEditorListeners.call(&InspectorEditorListener::contentSizeChanged, this);
}

void RuleEditor::referenceRemoved(Rule *, ControlVariableReference * cvr)
{
	removeReferenceUI(cvr);
	inspectorEditorListeners.call(&InspectorEditorListener::contentSizeChanged, this);
}

void RuleEditor::consequenceAdded(RuleConsequence * c)
{
	addConsequenceUI(c);
	inspectorEditorListeners.call(&InspectorEditorListener::contentSizeChanged, this);
}

void RuleEditor::consequenceRemoved(RuleConsequence * c)
{
	removeConsequenceUI(c);
	inspectorEditorListeners.call(&InspectorEditorListener::contentSizeChanged, this);
}


void RuleEditor::buttonClicked(Button * b)
{
	if (b == &addReferenceBT)
	{
		rule->addReference();
	}
	else if (b == &addConsequenceBT)
	{
		rule->addConsequence();
	}
}
