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


#include "Spat2DNode.h"

REGISTER_NODE_TYPE (Spat2DNode)

constexpr float minSpatX = -1.5;
constexpr float maxSpatX = 2;
Spat2DNode::Spat2DNode (StringRef name) :
NodeBase (name),
alphaFilter(1)
{
    spatMode = addNewParameter<EnumParameter> ("Mode", "Spatialization Mode (2D/3D, Beam/Proxy)");
    spatMode->isEditable=false;
    //    spatMode->addOption ("Beam", BEAM);
    spatMode->addOption ("Proxy", PROXY,true);

    

    shapeMode = addNewParameter<EnumParameter> ("Shape", "Predefined shape or free positioning");
    shapeMode->isEditable=false;
    shapeMode->addOption ("Circle", ShapeMode::CIRCLE,true);
    shapeMode->addOption ("Free", ShapeMode::FREE,true);


    //circle
    circleDiameter = addNewParameter<FloatParameter> ("Circle Diameter", "Diameter of the circle to place the targets", .8f, 0.f, 1.f);
    circleDiameter->setSavable(false); // we store only final positions
    circleRotation = addNewParameter<FloatParameter> ("Circle Rotation", "Rotation of the circle to place the targets", 0.f, 0.f, 360.f);
    circleRotation->setSavable(false); // we store only final positions
    targetRadius = addNewParameter<FloatParameter> ("Target Radius", "Radius for all targets", .5f, 0.f, 1.f);

    numSpatInputs = addNewParameter<IntParameter> ("Num Inputs", "Number of inputs to spacialize", 1, 0, 16);

    numSpatOutputs = addNewParameter<IntParameter> ("Num Outputs", "Number of spatialized outputs", 3, 0, 16);

    useGlobalTarget = addNewParameter<BoolParameter> ("Use Global Target", "Use a global target that will act as a max influence and affect all targets.", false);
    globalTargetPosition = addNewParameter<Point2DParameter<floatParamType>> ("Global Target Position", "Position of the Global Target",0,0,Array<var>{minSpatX,minSpatX},Array<var>{maxSpatX,maxSpatX});
    globalTargetRadius = addNewParameter<FloatParameter> ("Global Target Radius", "Radius for the global target", .5f, 0.f, 1.f);
    useLogCurve = addNewParameter<BoolParameter>("logCurve", "use log curve for volume", false);
    constantPower = addNewParameter<BoolParameter>("constantPower", "in a perfect rosace the sum of the squares of the volumes stay to one", true);
    setPreferedNumAudioInput (numSpatInputs->intValue());
    setPreferedNumAudioOutput (numSpatOutputs->intValue() );

    updateIOParams();
    spatMode->setValue ("Proxy");
    shapeMode->setValue("Circle");
    computeAllInfluences();
}

void Spat2DNode::setSourcePosition (int index, const Point<floatParamType>& position)
{

    Point2DParameter<floatParamType>* d = inputsPositionsParams.getReference(index);

    if (d == nullptr) return;

    d->setPoint(position);

    computeAllInfluences();
}

void Spat2DNode::setTargetPosition (int index, const Point<floatParamType>& position)
{
    if (index == -1)
    {
        globalTargetPosition->setPointFrom (this,position);
        computeAllInfluences();
    }
    else
    {
        targetPositions[index]->setPointFrom (this,position);
        computeInfluencesForTarget (index);
    }
}



void Spat2DNode::updateTargetsFromShape()
{
    if (shapeMode == nullptr) return;

    switch ((int)shapeMode->getFirstSelectedValue())
    {
        case  ShapeMode::FREE:
            //do nothing
            break;

        case  ShapeMode::CIRCLE:{
            float angleStart = circleRotation->floatValue() / 360.f * float_Pi * 2;
            float angleStep = float_Pi * 2.f / (numSpatOutputs->intValue());
            float hRadius = circleDiameter->floatValue()*.5f;
            for (int i = jmin(targetPositions.size(),numSpatOutputs->intValue())-1; i >=0 ; i--)
            {
                Point2DParameter<floatParamType>* p = targetPositions[i];
                float angle = angleStart + i *  angleStep;
                p->setPointFrom (this,.5f + cosf (angle)*hRadius, .5f + sinf (angle)*hRadius);
            }

            if (useGlobalTarget->boolValue())
            {
                globalTargetPosition->setPoint (.5f, .5f);
            }

            computeAllInfluences();
            break;
        }
    }


}

void Spat2DNode::computeAllInfluences()
{
    //Only one source for now
    switch ((int)spatMode->getFirstSelectedValue())
    {
        case BEAM:
            break;

        case PROXY:

            for (int i = 0; i < numSpatOutputs->intValue(); i++)
            {
                computeInfluencesForTarget (i);
            }

            break;
    }
}
void Spat2DNode::prepareToPlay (double sampleRate,int maximumExpectedSamplesPerBlock) {
    NodeBase::prepareToPlay(sampleRate, maximumExpectedSamplesPerBlock);
    double dt = maximumExpectedSamplesPerBlock/sampleRate;
    constexpr double targetT = 0.025;
    alphaFilter = jmax<double>(0.0001,jmin<double>(1,dt/targetT));
}
void Spat2DNode::computeInfluencesForTarget (int targetIndex)
{
    for (int i = 0; i < numSpatInputs->intValue(); i++)
    {
        computeInfluence (i, targetIndex);
    }
}

void Spat2DNode::computeInfluence (int sourceIndex, int targetIndex)
{
    if (sourceIndex >= inputsPositionsParams.size()){jassertfalse; return;}

    if (targetIndex >= outputsIntensities.size()) {jassertfalse; return;}
    

    Point2DParameter<floatParamType>* inputPos = inputsPositionsParams.getReference(sourceIndex);
    Point<floatParamType> sPos = inputPos->getPoint();
    FloatParameter* outputVal = outputsIntensities.getReference(targetIndex);

    float minValue = 0;

    if (useGlobalTarget->boolValue())
    {
        minValue = getValueForSourceAndTargetPos (sPos, globalTargetPosition->getPoint(), globalTargetRadius->floatValue());
    }

    if (numSpatInputs->intValue() > 0)
    {
        Point<floatParamType> tPos = targetPositions[targetIndex]->getPoint();

        float val = jmax<float> (minValue, getValueForSourceAndTargetPos (sPos, tPos, targetRadius->floatValue()+minValue));
        outputVal->setValueFrom (this,val);
    }
    else
    {
        outputVal->setValueFrom (this,0);
    }
}

float Spat2DNode::getValueForSourceAndTargetPos (const Point<floatParamType>& sourcePosition, const Point<floatParamType>& targetPosition, float radius)
{
    if (radius == 0) return 0;

    float dist = jlimit<float> (0, radius, sourcePosition.getDistanceFrom (targetPosition));

    return 1 - (dist / radius);
}

void Spat2DNode::numChannelsChanged (bool /*isInput*/)
{
    updateIOParams();

}

void Spat2DNode::updateIOParams(){

    for(int i = inputsPositionsParams.size() ;i<numSpatInputs->intValue(); i ++){
        auto np = addNewParameter<Point2DParameter<floatParamType> >
        ("input spat : "+String(i),
         "param for controlling spat position "+String(i),
         0.5+juce::Random().nextFloat()*.2f,0.5+juce::Random().nextFloat()*.2f,
         Array<var>{minSpatX,minSpatX},Array<var>{maxSpatX,maxSpatX}
         );
        inputsPositionsParams.add(np);

    }
    for(int i = inputsPositionsParams.size()-1;i>=numSpatInputs->intValue() ; i --){
        auto p = inputsPositionsParams.getReference(i);
        ParameterContainer::removeControllable(p);
        inputsPositionsParams.remove(i);

    }


    influences.resize(numSpatOutputs->intValue());
    for(int i = outputsIntensities.size() ;i<numSpatOutputs->intValue(); i ++){
        auto np = addNewParameter<FloatParameter >
        ("output spat : "+String(i),
         "intensity for spat speaker "+String(i),
         0.0f,0.0f,1.0f
         );
        np->isEditable = false;
        auto po = addNewParameter<Point2DParameter<floatParamType>>("target"+String(i), "target pos param",
                                                           0,0,
                                                           Array<var>{minSpatX,minSpatX},Array<var>{maxSpatX,maxSpatX}
                                                           );
        targetPositions.add(po);
        outputsIntensities.add(np);
        influences.set(i, 0);

    }
    for(int i = outputsIntensities.size()-1;i>=numSpatOutputs->intValue() ; i --){
        auto p = outputsIntensities.getReference(i);
        ParameterContainer::removeControllable(p);
        outputsIntensities.remove(i);

        auto po = targetPositions.getReference(i);
        ParameterContainer::removeControllable(po);
        targetPositions.remove(i);

    }
}

bool Spat2DNode::modeIsBeam()
{
    return (int)spatMode->getFirstSelectedValue() == BEAM;
}

void Spat2DNode::onContainerParameterChanged ( ParameterBase* p)
{

    if (p == spatMode)
    {

        updateTargetsFromShape();
    }
    else if (p == numSpatInputs)
    {

        setPreferedNumAudioInput (numSpatInputs->intValue());
        updateTargetsFromShape();

    }
    else if (p == numSpatOutputs)
    {
        setPreferedNumAudioOutput (numSpatOutputs->intValue() );

        updateChannelNames();
        updateTargetsFromShape();
    }
    else if (p == shapeMode || p == circleDiameter || p == circleRotation)
    {
        updateTargetsFromShape();
    }
    else if (p == globalTargetRadius || p == targetRadius || p == globalTargetPosition)
    {
        computeAllInfluences();
    }
    else if(inputsPositionsParams.contains((Point2DParameter<floatParamType> *)p)){
        //        int i = inputsPositionsParams.indexOf(p);
        computeAllInfluences();
    }

    NodeBase::onContainerParameterChanged (p);

}


void Spat2DNode::updateChannelNames()
{

    for (int i = 0; i < numSpatOutputs->intValue() ; i++)
    {
        setOutputChannelName (i, "Spat " + String (i ));
    }
}

inline float computeGain(float g,bool useLog,bool constantPow){
    return  constantPow?sqrt(g):useLog?Decibels::decibelsToGain(jmap<float>(g,0,1,-101,0)):g;

}

void Spat2DNode::processBlockInternal (AudioBuffer<float>& buffer, MidiBuffer&)
{


    int numSamples = buffer.getNumSamples();
    int numIn = numSpatInputs->intValue();
    int numOut = numSpatOutputs->intValue();
    bool logCurve = useLogCurve->boolValue();
    bool constantP = constantPower->boolValue();
    tempBuf.setSize(numIn, numSamples);
    for(int j = 0 ; j < getTotalNumInputChannels() ; j++ ){
        tempBuf.copyFrom(j, 0, buffer.getReadPointer(j),  numSamples);
    }

    for (int i = 0; i < numOut; i++)
    {

        float lastInfluence = influences[i];
        float influence = (1.f-alphaFilter)*lastInfluence + (alphaFilter)*computeGain(outputsIntensities.getUnchecked(i)->floatValue(),logCurve,constantP);

        for(int j = 0 ; j < numIn ; j++ ){
            buffer.copyFromWithRamp (i, 0, tempBuf.getReadPointer (j), numSamples, lastInfluence,influence);
        }
        influences.set(i,influence);
    }
    
}

