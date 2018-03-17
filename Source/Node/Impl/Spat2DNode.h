/* Copyright © Organic Orchestra, 2017
*
* This file is part of LGML.  LGML is a software to manipulate sound in realtime
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation (version 3 of the License).
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
*/


#ifndef SPATNODE_H_INCLUDED
#define SPATNODE_H_INCLUDED




#include "../NodeBase.h"

class Spat2DNode : public NodeBase
{
public:
    DECLARE_OBJ_TYPE (Spat2DNode,"paint with sound in space")
    enum SpatMode {BEAM, PROXY};
    enum ShapeMode {FREE, CIRCLE};

    EnumParameter* spatMode;  //Beam / Proxy
    EnumParameter* shapeMode;  //Free, Circle

    IntParameter* numSpatInputs;
    IntParameter* numSpatOutputs;

    FloatParameter* targetRadius;

    //Circle shape
    FloatParameter* circleRadius;
    FloatParameter* circleRotation;

    Array<Point2DParameter<float> *> targetPositions;

    BoolParameter* useGlobalTarget;
    Point2DParameter<float>* globalTargetPosition;
    FloatParameter* globalTargetRadius;

    void setSourcePosition (int index, const Point<float>& position);
    void setTargetPosition (int index, const Point<float>& position);
    void updateInputOutputDataSlots();

    void updateTargetsFromShape();
    void computeAllInfluences();
    void computeInfluencesForTarget (int targetIndex);
    void computeInfluence (int sourceIndex, int targetIndex);
    float getValueForSourceAndTargetPos (const Point<float>& sourcePosition, const Point<float>& targetPosition, float radius);

    bool modeIsBeam();

    void onContainerParameterChanged ( ParameterBase*) override;

    void processInputDataChanged (Data*) override;



    //AUDIO
    void updateChannelNames();
    void numChannelsChanged (bool isInput)override;
    virtual void processBlockInternal (AudioBuffer<float>& /*buffer*/, MidiBuffer& /*midiMessage*/) override;




    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Spat2DNode)
};


#endif  // SPATNODE_H_INCLUDED
