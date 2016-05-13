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
#include "ParameterUI.h"


class ControlVariableReferenceUI : public Component, public ButtonListener, public ControlVariableReference::ControlVariableReferenceListener
{
public :
	ControlVariableReferenceUI(ControlVariableReference * cvr);
	virtual ~ControlVariableReferenceUI();

	ControlVariableReference * cvr;

	ScopedPointer<StringParameterUI> aliasUI;
	Label referenceLabel;
	TextButton chooseBT;

	ScopedPointer<ParameterUI> currentVariableParamUI;

	void buttonClicked(Button * b);
	void resized();

	void currentReferenceChanged(ControlVariableReference *);
	
};


#endif  // CONTROLVARIABLEREFERENCEUI_H_INCLUDED
