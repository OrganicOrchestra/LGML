/*
  ==============================================================================

    ControlVariableReferenceUI.h
    Created: 12 May 2016 4:05:58pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef CONTROLVARIABLEREFERENCEUI_H_INCLUDED
#define CONTROLVARIABLEREFERENCEUI_H_INCLUDED

#include "ControlVariableReference.h"
#include "ControllableHelpers.h"


class ControlVariableReferenceUI : public Component, public ButtonListener
{
public :
	ControlVariableReferenceUI(ControlVariableReference * cvr);
	virtual ~ControlVariableReferenceUI();

	ControlVariableReference * cvr;

	ScopedPointer<StringParameterUI> aliasUI;
	Label referenceLabel;
	TextButton chooseBT;
	
	void buttonClicked(Button * b);
	void resized();
	
};


#endif  // CONTROLVARIABLEREFERENCEUI_H_INCLUDED
