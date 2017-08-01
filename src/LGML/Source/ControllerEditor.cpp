/*
  ==============================================================================

    ControllerEditor.cpp
    Created: 10 May 2016 9:31:40am
    Author:  bkupe

  ==============================================================================
*/

#include "ControllerEditor.h"
#include "Style.h"
#include "ParameterUIFactory.h"
ControllerEditor::ControllerEditor(Controller * _controller,bool generateAuto) :
	InspectorEditor(),
	controller(_controller),
	addParameterBT("Add Variable")
{
	addParameterBT.addListener(this);
  if(generateAuto){
  editor = new GenericControllableContainerEditor(controller);
  addAndMakeVisible(editor);
  }
  addAndMakeVisible(addParameterBT);
}

ControllerEditor::~ControllerEditor()
{

}


void ControllerEditor::resized()
{
	InspectorEditor::resized();
  auto r =getLocalBounds();
	addParameterBT.setBounds(r.removeFromTop(20));
  if(editor){
    editor->setBounds(r);
  }
}

int ControllerEditor::getContentHeight()
{
  return InspectorEditor::getContentHeight() + 20  + (editor?editor->getContentHeight():0);
}


void ControllerEditor::buttonClicked(Button * b)
{
	if (b == &addParameterBT)
	{
    
    controller->userContainer.addNewParameter<FloatParameter>("var","Custom Variable");

	}
}


