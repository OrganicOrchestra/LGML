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
	  codeEditor(consequence->codeDocument, &codeTokeniser)

  {
	  addAndMakeVisible(codeEditor);
  }

  ScriptedConsequenceUI::~ScriptedConsequenceUI()
  {
  }


  void ScriptedConsequenceUI::resized()
  {
	  Rectangle<int> r = getLocalBounds().reduced(2);
	  codeEditor.setBounds(r);
  }
