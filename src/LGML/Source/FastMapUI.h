/*
  ==============================================================================

    FastMapUI.h
    Created: 17 May 2016 6:05:44pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef FASTMAPUI_H_INCLUDED
#define FASTMAPUI_H_INCLUDED

#include "FastMap.h"
#include "ControlVariableReferenceUI.h"
#include "ControllableHelpers.h"
#include "FloatSliderUI.h"
#include "InspectableComponent.h"


class FastMapUI :
	public InspectableComponent,
	public FastMapListener,
	public  ControllableChooser::Listener,
	public ButtonListener
{
public:
	FastMapUI(FastMap * f);
	virtual ~FastMapUI();

	ScopedPointer<ControlVariableReferenceUI> refUI;

	ScopedPointer<BoolToggleUI> enabledUI;

	ScopedPointer<FloatSliderUI> minInputUI;
	ScopedPointer<FloatSliderUI> maxInputUI;
	ScopedPointer<FloatSliderUI> minOutputUI;
	ScopedPointer<FloatSliderUI> maxOutputUI;
	ScopedPointer<BoolToggleUI> invertUI;

	ControllableChooser chooseTargetBT;

	FastMap * fastMap;
	ImageButton removeBT;


	void paint(Graphics &g) override;
	void resized() override;

	virtual void choosedControllableChanged(Controllable *) override;

	virtual void fastMapTargetChanged(FastMap *) override;
	void buttonClicked(Button * b) override;


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FastMapUI)

};




#endif  // FASTMAPUI_H_INCLUDED
