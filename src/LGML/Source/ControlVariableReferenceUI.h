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

class ControlVariableReferenceUI :
	public Component,
	public ButtonListener,
	public ControlVariableReferenceListener,
	public Parameter::Listener
{
public :
	ControlVariableReferenceUI(ControlVariableReference * cvr);
	virtual ~ControlVariableReferenceUI();

	ControlVariableReference * cvr;

	ScopedPointer<StringParameterUI> aliasUI;
	Label referenceLabel;
	TextButton chooseBT;

	ImageButton removeBT;

	ScopedPointer<ParameterUI> currentVariableParamUI;

	void updateCurrentReference();

	void buttonClicked(Button * b);
	void resized();

	void referenceVariableChanged(ControlVariableReference *) override;

	void parameterValueChanged(Parameter * p);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ControlVariableReferenceUI)

};


#endif  // CONTROLVARIABLEREFERENCEUI_H_INCLUDED
