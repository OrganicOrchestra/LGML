/*
  ==============================================================================

    SpatNode.h
    Created: 2 Mar 2016 8:37:48pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef SPATNODE_H_INCLUDED
#define SPATNODE_H_INCLUDED




#include "NodeBase.h"

class Spat2DNode : public NodeBase
{
public:
	Spat2DNode();
	enum SpatMode {BEAM, PROXY};
	enum ShapeMode {FREE, CIRCLE};

	EnumParameter * spatMode; //Beam / Proxy
	EnumParameter * shapeMode; //Free, Circle

	IntParameter * numSpatInputs;
	IntParameter * numSpatOutputs;
	
	FloatParameter * targetRadius;

	//Circle shape
	FloatParameter * circleRadius;
	FloatParameter * circleRotation;

	Array<Point2DParameter<float> *> targetPositions;
	
	BoolParameter * useGlobalTarget;
	Point2DParameter<float> * globalTargetPosition;
	FloatParameter * globalTargetRadius;

	void setSourcePosition(int index, const Point<float> &position);
	void setTargetPosition(int index, const Point<float> &position);
	void updateInputOutputDataSlots();

	void updateTargetsFromShape();
	void computeAllInfluences();
	void computeInfluencesForTarget(int targetIndex);
	void computeInfluence(int sourceIndex, int targetIndex);
	float getValueForSourceAndTargetPos(const Point<float> &sourcePosition, const Point<float> &targetPosition,float radius);

	bool modeIsBeam();

	void onContainerParameterChanged(Parameter *) override;
	void processInputDataChanged(Data *) override;



	//AUDIO
	void updateChannelNames();
    void numChannelsChanged(bool isInput)override;
	virtual void processBlockInternal(AudioBuffer<float>& /*buffer*/, MidiBuffer& /*midiMessage*/) override;
	


	ConnectableNodeUI * createUI() override;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Spat2DNode)
};


#endif  // SPATNODE_H_INCLUDED
