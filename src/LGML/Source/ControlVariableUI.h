/*
  ==============================================================================

    ControlVariableUI.h
    Created: 10 May 2016 3:02:05pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef CONTROLVARIABLEUI_H_INCLUDED
#define CONTROLVARIABLEUI_H_INCLUDED

#include "ControlVariable.h"
#include "ParameterUI.h"

class ControlVariableUI : public Component, public ButtonListener,  public LabelListener
{
public:
	ControlVariableUI(ControlVariable * variable);
	virtual ~ControlVariableUI();

	Label pNameLabel;
	ImageButton removeBT;

	ScopedPointer<ParameterUI> pui;
	ControlVariable * variable;

	void setNameIsEditable(bool value);

	void paint(Graphics &g) override;

	void resized() override;

	void buttonClicked(Button * b)override;
	void labelTextChanged(Label *b)override;
};


#endif  // CONTROLVARIABLEUI_H_INCLUDED
