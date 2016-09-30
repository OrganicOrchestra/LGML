/*
  ==============================================================================

    SpatNodeUI.h
    Created: 29 Sep 2016 7:20:25pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef SPATNODEUI_H_INCLUDED
#define SPATNODEUI_H_INCLUDED

#include "NodeBaseContentUI.h"
#include "EnumParameterUI.h"
#include "SpatNode.h"
#include "Spat2DViewer.h"
#include "Spat3DViewer.h"

class SpatNodeContentUI : 
	public NodeBaseContentUI, 
	public SpatNode::SpatNodeListener
{
public:
	SpatNodeContentUI();
	virtual ~SpatNodeContentUI();

	SpatNode * spatNode;

	ScopedPointer<Spat2DViewer> viewer2D;
	ScopedPointer<Spat3DViewer> viewer3D;

	ScopedPointer<EnumParameterUI> spatModeUI;
	ScopedPointer<NamedControllableUI> inputStepper;
	ScopedPointer<NamedControllableUI> outputStepper;

	void updateSpatModeView();

	void resized() override;
	void init() override;

	
	// Inherited via SpatNodeListener
	virtual void modeChanged() override;
	virtual void numSpatInputsChanged() override;
	virtual void numSpatOutputsChanged() override;


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpatNodeContentUI)

};



#endif  // SPATNODEUI_H_INCLUDED
