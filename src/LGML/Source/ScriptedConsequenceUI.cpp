/*
  ==============================================================================

    ScriptedConsequenceUI.cpp
    Created: 13 May 2016 6:41:05pm
    Author:  bkupe

  ==============================================================================
*/

#include "ScriptedConsequenceUI.h"


  ScriptedConsequenceUI::ScriptedConsequenceUI(ScriptedConsequence * consequence) :
	  RuleConsequenceUI(consequence),
	  scriptedConsequence(consequence),
	  jsEditor(&consequence->codeDocument),
	  reloadBT("Reload Script")

  {
	  addAndMakeVisible(jsEditor);
	  addAndMakeVisible(&reloadBT);
	  reloadBT.addListener(this);
  }

  ScriptedConsequenceUI::~ScriptedConsequenceUI()
  {
  }


  void ScriptedConsequenceUI::resized()
  {
	  Rectangle<int> r = getLocalBounds().reduced(2);
	  reloadBT.setBounds(r.removeFromBottom(15));
	  jsEditor.setBounds(r);
  }


  void ScriptedConsequenceUI::buttonClicked(Button * b)
  {
	  if (b == &reloadBT) scriptedConsequence->reloadScript();
  }
