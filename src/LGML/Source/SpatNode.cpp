/*
  ==============================================================================

    SpatNode.cpp
    Created: 2 Mar 2016 8:37:48pm
    Author:  bkupe

  ==============================================================================
*/

#include "SpatNode.h"
#include "NodeBaseUI.h"
#include "SpatNodeUI.h"

SpatNode::SpatNode() :
	NodeBase("Spat", NodeType::SpatType),
	numSpatInputs(nullptr), numSpatOutputs(nullptr), spatMode(nullptr)
{
	spatMode = addEnumParameter("Mode", "Spatialization Mode (2D/3D, Beam/Proxy)");
	spatMode->addOption("Beam 2D", Beam2DId.toString());
	spatMode->addOption("Beam 3D", Beam3DId.toString());
	spatMode->addOption("Proxy 2D", Proxy2DId.toString());
	spatMode->addOption("Proxy 3D", Proxy3DId.toString());
	
	spatMode->setValue("Proxy 2D");

	DBG("Spat mode created : " << spatMode->getValueData().toString());

	numSpatInputs = addIntParameter("Num Inputs", "Number of inputs to spacialize", 1, 0, 16);
	numSpatOutputs = addIntParameter("Num Outputs", "Number of spatialized outputs", 3, 0, 16);

	setPreferedNumAudioInput(numSpatInputs->intValue());
	setPreferedNumAudioOutput(numSpatOutputs->intValue());

	
	updateInputOutputDataSlots();
}

void SpatNode::setSourcePosition(int index, const Point<float>& position)
{
	Data * d = getInputData(index);
	if (d == nullptr) return;
	d->update(position.x, position.y, 0);
}

void SpatNode::setTargetPosition(int index, const Point<float>& position)
{
	targetsX[index]->setValue(position.x);
	targetsY[index]->setValue(position.y);
}

void SpatNode::updateInputOutputDataSlots()
{
	if (numSpatInputs == nullptr || numSpatOutputs == nullptr) return;


	//INPUT
	while (getTotalNumInputData() > numSpatInputs->intValue())
	{
		removeInputData("Input " + String(getTotalNumInputData()));
	}

	DataType dt = modeIsBeam()? DataType::Orientation : DataType::Position;
	while (getTotalNumInputData() < numSpatInputs->intValue())
	{
		addInputData("Input " + String(getTotalNumInputData() + 1), dt);
	}

	//OUTPUT
	while (getTotalNumOutputData() > numSpatOutputs->intValue())
	{
		removeOutputData("Influence " + String(getTotalNumOutputData()));
		FloatParameter * px = targetsX[getTotalNumOutputData() - 1];
		FloatParameter * py = targetsY[getTotalNumOutputData() - 1];
		targetsX.removeAllInstancesOf(px);
		targetsY.removeAllInstancesOf(py);
		removeControllable(px);
		removeControllable(py);
	}


	while (getTotalNumOutputData() < numSpatOutputs->intValue())
	{
		addOutputData("Influence " + String(getTotalNumOutputData() + 1), DataType::Number);
		targetsX.add(addFloatParameter("TargetX" + String(getTotalNumOutputData() + 1), "", 0, -1, 1));
		targetsY.add(addFloatParameter("TargetY" + String(getTotalNumOutputData() + 1), "", 0, -1, 1));
	}
}

bool SpatNode::modeIs2D()
{
	return spatMode->getValueData().toString() == Beam2DId.toString() || spatMode->getValueData().toString() == Proxy2DId.toString();
}

bool SpatNode::modeIsBeam()
{
	return spatMode->getValueData().toString() == Beam2DId.toString() || spatMode->getValueData().toString() == Beam3DId.toString();
}

void SpatNode::onContainerParameterChanged(Parameter * p)
{
	NodeBase::onContainerParameterChanged(p);
	
	if (p == spatMode)
	{
		//DBG("Spat Mode : " << spatMode->getValueData().toString());
		spatNodeListeners.call(&SpatNodeListener::modeChanged);
		
		removeAllInputDatas();
		updateInputOutputDataSlots();

	} else if (p == numSpatInputs)
	{
		setPreferedNumAudioInput(numSpatInputs->intValue());
		spatNodeListeners.call(&SpatNodeListener::numSpatInputsChanged);
		
		
		updateInputOutputDataSlots();

	} else if (p == numSpatOutputs)
	{
		setPreferedNumAudioOutput(numSpatOutputs->intValue());
		spatNodeListeners.call(&SpatNodeListener::numSpatOutputsChanged);

		updateInputOutputDataSlots();

	}
}

ConnectableNodeUI * SpatNode::createUI() {
	return new NodeBaseUI(this,new SpatNodeContentUI());
}
