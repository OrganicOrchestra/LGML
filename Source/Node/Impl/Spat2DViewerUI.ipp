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

#if !ENGINE_HEADLESS

//#include "Spat2DViewerUI.h"
#include "../../UI/Style.h"
#include "../../Controllable/ControllableUIHelpers.h"

#define REPAINT_INTERVAL_MS 50

int getDefaultSizeForType(Spat2DElement::Type t){
    return t==Spat2DElement::Type::SOURCE?25:20;
}
Spat2DViewer::Spat2DViewer (Spat2DNode* _node) : node (_node)
{
    updateNumTargets();
    updateNumSources();
    node->addConnectableNodeListener (this);


    if (node->useGlobalTarget->boolValue())
    {
        globalTarget = std::make_unique< Spat2DTarget> (-1, Colours::green);
        addAndMakeVisible (globalTarget.get());
        globalTarget->radius = node->globalTargetRadius->floatValue();
        globalTarget->addSpatElementListener (this);
        globalTarget->setFloatPosition (node->globalTargetPosition->getPoint());
    }

    setOpaque (true);
//    setPaintingIsUnclipped(true);

}

Spat2DViewer::~Spat2DViewer()
{
    stopTimer();
    node->removeConnectableNodeListener (this);
    sources.clear();
    targets.clear();
}

void Spat2DViewer::updateNumSources()
{
    int numSources = node->numSpatInputs->intValue();

    for (int i = 0; i < sources.size(); i++)
    {
        sources[i]->removeSpatElementListener (this);
        removeChildComponent (sources[i]);
    }

    sources.clear();

    for (int i = 0; i < numSources; i++)
    {
        Spat2DSource* s = new Spat2DSource (i);

        s->addSpatElementListener (this);
        addAndMakeVisible (s);
        sources.add (s);
    }

    for (int i = 0; i < numSources; i++)
    {
        updateSourcePosition (i);
    }

    for(auto s:sources){
        s->toFront (false); //keep sources on top)
    }
//    resized();
}

void Spat2DViewer::updateNumTargets()
{
    int numTargets = node->numSpatOutputs->intValue();

    for (int i = 0; i < targets.size(); i++)
    {
        targets[i]->removeSpatElementListener (this);
        removeChildComponent (targets[i]);

    }

    targets.clear();

    for (int i = 0; i < numTargets; i++)
    {
        Spat2DTarget* t = new Spat2DTarget (i);

        t->addSpatElementListener (this);
        addAndMakeVisible (t);
        targets.add (t);

        updateTargetInfluence (i);
    }

    for (int i = 0; i < numTargets; i++)
    {
        updateTargetPosition (i);
    }
    updateTargetRadius();
    for(auto s:sources){
        s->toFront (false); //keep sources on top)
    }
//    resized();
}

void Spat2DViewer::updateTargetRadius()
{
    for (auto& t : targets)
    {
        t->setFloatRadius(node->targetRadius->floatValue());
    }



}

void Spat2DViewer::updateSourcePosition (int sourceIndex)
{
    if (sourceIndex == -1 || sourceIndex >= sources.size()) {jassertfalse;return;}

    sources[sourceIndex]->setFloatPosition (node->inputsPositionsParams[sourceIndex]->getPoint());
}

void Spat2DViewer::updateTargetPosition (int targetIndex)
{
    if (targetIndex == -1 || targetIndex >= targets.size())  {jassertfalse;return;}

    targets[targetIndex]->setFloatPosition (node->targetPositions[targetIndex]->getPoint());
}

void Spat2DViewer::updateTargetInfluence (int targetIndex,bool repaint)
{
    if (targetIndex == -1 || targetIndex >= targets.size())  {jassertfalse;return;}
    repaint &= (targets[targetIndex]->influence != node->outputsIntensities[targetIndex]->floatValue());
    targets[targetIndex]->influence = node->outputsIntensities[targetIndex]->floatValue();
    if(repaint)targets[targetIndex]->repaint();
}


void Spat2DViewer::resized()
{

    for (auto& s : sources)
    {
        s->setFloatPosition (s->position);

    }


    if (globalTarget != nullptr)
    {
        globalTarget->setFloatPosition (globalTarget->position);

    }

    for (auto& t : targets)
    {
        t->setFloatPosition(t->position);
        t->setFloatRadius(node->targetRadius->floatValue());
    }
}

void Spat2DViewer::paint (Graphics& g)
{
    g.setColour (findColour (ResizableWindow::backgroundColourId).darker());
    g.fillRect (getLocalBounds());
}

void Spat2DViewer::nodeParameterChangedAsync (ConnectableNode*, ParameterBase* p){
    // non heavy changes

    if (p == node->numSpatInputs)  updateNumSources();
    else if (p == node->numSpatOutputs)  updateNumTargets();

    else if (p == node->globalTargetPosition){

            if (globalTarget != nullptr)
            {
                globalTarget->setFloatPosition (node->globalTargetPosition->getPoint());
                resized();
            }


    }

    else if (p == node->shapeMode)
    {
        bool circleMode = (int)node->shapeMode->getFirstSelectedValue() == Spat2DNode::ShapeMode::CIRCLE;

        for (int i = 0; i < targets.size(); i++) targets[i]->setEnabled (!circleMode);

    }
    else if (p == node->useGlobalTarget)
    {
        if (node->useGlobalTarget->boolValue())
        {
            globalTarget = std::make_unique< Spat2DTarget> (-1, Colours::green);
            addAndMakeVisible (globalTarget.get());
            globalTarget->radius = node->globalTargetRadius->floatValue();
            globalTarget->setFloatPosition (node->globalTargetPosition->getPoint());
            globalTarget->addSpatElementListener (this);
        }
        else
        {
            globalTarget->removeSpatElementListener (this);
            removeChildComponent (globalTarget.get());
            globalTarget = nullptr;
        }

        resized();

    }
    else if (p == node->globalTargetRadius)
    {
        if (globalTarget != nullptr)
        {
            globalTarget->radius = node->globalTargetRadius->floatValue();
            resized();
        }

    }
    else if (p == node->targetRadius){ if(!isTimerRunning()){Timer::startTimer(REPAINT_INTERVAL_MS);}}
    // heavy repainters
    else if (p == node->circleDiameter){if(!isTimerRunning()){Timer::startTimer(REPAINT_INTERVAL_MS);}}
    else if (p == node->circleRotation){if(!isTimerRunning()){Timer::startTimer(REPAINT_INTERVAL_MS);}}
    else if(node->outputsIntensities.contains((FloatParameter*)p)){
        if(!isTimerRunning()){Timer::startTimer(REPAINT_INTERVAL_MS);}
    }
    else if (p->isType<Point2DParameter<floatParamType>>())
    {


        Point2DParameter<floatParamType>* p2d = (Point2DParameter<floatParamType>*)p;
        if(node->targetPositions.contains(p2d)){
            if(!isTimerRunning()){Timer::startTimer(REPAINT_INTERVAL_MS);}

        }
        else if(node->inputsPositionsParams.contains(p2d)){
            if(!isTimerRunning()){Timer::startTimer(REPAINT_INTERVAL_MS);}
        }
        else{
            jassertfalse;
        }



    }


    //}                );
}


void Spat2DViewer::timerCallback(){
    if(node){
        for(int index = 0 ; index <  node->outputsIntensities.size() ; index++){
            updateTargetPosition (index);
            updateTargetInfluence(index,true);
        }
        for(int index = 0 ; index<node->inputsPositionsParams.size();index++){
            updateSourcePosition(index);
        }
        updateTargetRadius();
        repaint();
    }
    stopTimer();
}
void Spat2DViewer::childControllableAdded (ControllableContainer*, Controllable* c)
{
    if (c->isType<Point2DParameter<floatParamType>>()) updateNumTargets();
}

void Spat2DViewer::childControllableRemoved (ControllableContainer*, Controllable* c)
{
    if (c->isType<Point2DParameter<floatParamType>>()) updateNumTargets();
}

void Spat2DViewer::spatElementUserMoved (Spat2DElement* handle, const Point<floatParamType>& newPos)
{
    if (handle->type == Spat2DElement::Type::SOURCE)
    {
        node->setSourcePosition (handle->index, newPos);
    }
    else
    {
        node->setTargetPosition (handle->index, newPos);
    }

}


Spat2DSource::Spat2DSource (int _index): Spat2DElement (Type::SOURCE, _index, Colours::grey)
{
    int ds = getDefaultSizeForType(Type::SOURCE);
    setSize(ds,ds);
}



Spat2DTarget::Spat2DTarget (int _index, Colour c) : Spat2DElement (Type::TARGET, _index, c)
{
    int ds = getDefaultSizeForType(Type::TARGET);
    setSize(ds,ds);
    radius = .25f;
}

void Spat2DTarget::setFloatRadius(float r,bool _repaint ){
    radius = r;
    auto sui = findParentComponentOfClass<Spat2DViewer>();
    int ds = radius*2.0*sui->getWidth()  ;
    Point<int> curCentre = getBounds().getCentre();
    Rectangle<int> dB {curCentre.x-ds/2,curCentre.y-ds/2,ds,ds};
    setBounds(dB);

}
void Spat2DTarget::paint (Graphics& g)
{
    auto sui = findParentComponentOfClass<Spat2DViewer>();
    float maxRad = radius*sui->getWidth()*2.0;
    Rectangle<float> r  = getLocalBounds().toFloat().withSizeKeepingCentre (maxRad, maxRad);
    Colour c = isMouseOver() ? Colours::yellow : color;
//    g.setColour (color.darker (.3f).withAlpha (.05f));
//    g.fillEllipse (r);
    g.setColour (color.brighter (.3f).withAlpha (.2f));
    g.fillEllipse (r.withSizeKeepingCentre (maxRad * influence, maxRad * influence));
    g.setColour (c.withAlpha (.5f));
    g.drawEllipse (r, 1);
}

Spat2DElement::Spat2DElement (Type _type, int _index,  Colour _color) : type (_type), index (_index), color (_color),handle(this)
{

    setInterceptsMouseClicks(false,true);
    addAndMakeVisible(handle);
    setPaintingIsUnclipped(true);

}

Spat2DElement::~Spat2DElement()
{
}

void Spat2DElement::moved (){
    handle.setCentrePosition(getLocalBounds().getCentre());
}


void Spat2DElement::setFloatPosition (const Point<floatParamType> & newPosition)
{
     auto parent = findParentComponentOfClass<Spat2DViewer>();

    position.setXY (newPosition.x, newPosition.y);
    setCentrePosition (position.x * parent->getWidth(), position.y * parent->getHeight());
}


CircularHandleComponent::CircularHandleComponent(Spat2DElement* o):owner(o){

    setRepaintsOnMouseActivity (true);
    size=getDefaultSizeForType(owner->type);
    numLabel.setInterceptsMouseClicks(false,false);
    numLabel.setJustificationType(Justification::centred);
    numLabel.setText(String(owner->index),dontSendNotification);
    numLabel.setColour(Label::ColourIds::textColourId, Colours::white);
    
    addAndMakeVisible(numLabel);

    LGMLUIUtils::optionallySetBufferedToImage(this);
    setPaintingIsUnclipped(true);
    setInterceptsMouseClicks(true,true);
    setSize(size,size);
    
}

void CircularHandleComponent::paint (Graphics& g)
{
    Rectangle<int> r = getLocalBounds();
    Colour c = isMouseOver() ? Colours::yellow : owner->color;
    g.setColour (c.withAlpha (.5f));
    g.fillEllipse (r.toFloat());

}

void CircularHandleComponent::resized () {
    numLabel.setBounds(getLocalBounds());
}

void CircularHandleComponent::mouseDown (const MouseEvent& e)
{
    if (e.mods.isRightButtonDown())
    {
        owner->elemListeners.call (&Spat2DElement::Listener::spatElementUserMoved, owner, Point<floatParamType> (.5, .5));
    }

    toFront (true);
}

void CircularHandleComponent::mouseDrag (const MouseEvent& e)
{

    if (e.mods.isLeftButtonDown())
    {   auto parent = findParentComponentOfClass<Spat2DViewer>();
        Point<floatParamType> newPos {
            jlimit<float> (0, 1, parent->getMouseXYRelative().x * 1. / parent->getWidth()),
            jlimit<float> (0, 1, parent->getMouseXYRelative().y * 1. / parent->getHeight())

        };
        owner->elemListeners.call (&Spat2DElement::Listener::spatElementUserMoved, owner, newPos);
    }
}


bool CircularHandleComponent::hitTest (int x, int y)
{
    if (!isEnabled()) return false;

    //  Component * parent = getParentComponent();
    Point<float> relPoint (x, y);
    float dist = relPoint.getDistanceFrom (getLocalBounds().getCentre().toFloat());
    return dist < size / 2;
}

#endif
