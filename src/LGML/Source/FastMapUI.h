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

#include "InspectableComponent.h"
#include "ParameterProxyUI.h"

class FastMapUI :
	public InspectableComponent,
	public  ControllableReferenceUI::Listener,
	public ButtonListener
{
public:
	FastMapUI(FastMap * f);
	virtual ~FastMapUI();

	ParameterProxyUI refUI;
  ParameterProxyUI targetUI;

	ScopedPointer<ParameterUI> enabledUI;

	ScopedPointer<ParameterUI> minInputUI;
	ScopedPointer<ParameterUI> maxInputUI;
	ScopedPointer<ParameterUI> minOutputUI;
	ScopedPointer<ParameterUI> maxOutputUI;
	ScopedPointer<ParameterUI> invertUI;



	FastMap * fastMap;
	ImageButton removeBT;


	void paint(Graphics &g) override;
	void resized() override;

//	virtual void choosedControllableChanged(ControllableReferenceUI*,Controllable *) override;
//	virtual void fastMapTargetChanged(FastMap *) override;


  void mouseDown(const MouseEvent &e) override;
	void buttonClicked(Button * b) override;


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FastMapUI)

};




#endif  // FASTMAPUI_H_INCLUDED
