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
#include "ParameterUI.h"
#include "Spat2DNode.h"
#include "Spat2DViewer.h"
#include "Spat3DViewer.h"

class Spat2DNodeContentUI : 
	public NodeBaseContentUI, 
	public Spat2DNode::ConnectableNodeListener
{
public:
	Spat2DNodeContentUI();
	virtual ~Spat2DNodeContentUI();

	Spat2DNode * spatNode;

	ScopedPointer<Spat2DViewer> viewer2D;

	ScopedPointer<ParameterUI> spatModeUI;
	ScopedPointer<NamedControllableUI> inputStepper;
	ScopedPointer<NamedControllableUI> outputStepper;
	ScopedPointer<ParameterUI> radiusUI;
	ScopedPointer<ParameterUI> useGlobalUI;
	ScopedPointer<ParameterUI> globalRadiusUI;

	ScopedPointer<ParameterUI> shapeModeUI;
	ScopedPointer<ParameterUI> circleRadiusUI;
	ScopedPointer<ParameterUI> circleRotationUI;

	void resized() override;
	void init() override;

	void updateShapeModeView();
	
	void nodeParameterChanged(ConnectableNode *, Parameter * p) override;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Spat2DNodeContentUI)
};




#endif  // SPATNODEUI_H_INCLUDED
