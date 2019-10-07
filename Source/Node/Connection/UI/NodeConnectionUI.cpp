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

#include "NodeConnectionUI.h"
#include "NodeConnectionEditor.hpp"

//==============================================================================
NodeConnectionUI::NodeConnectionUI (NodeConnection* connection, Connector* sourceConnector, Connector* destConnector) :
    InspectableComponent("NodeConnectionUI"),
    candidateDropConnector (nullptr),
    connection (connection),
    sourceConnector (nullptr),
    destConnector (nullptr)
{
    InspectableComponent::paintBordersWhenSelected = false;
    setSourceConnector (sourceConnector);
    setDestConnector (destConnector);

    if (connection != nullptr)
    {
        connection->addConnectionListener (this);
    }


        setInterceptsMouseClicks (true, true);
//    }

    addComponentListener (this);
    setWantsKeyboardFocus (true);

    anchorSource.setAlwaysOnTop(true);
    anchorDest.setAlwaysOnTop(true);
    addChildComponent (anchorSource);
    addChildComponent (anchorDest);

    anchorSource.setVisible (isEditing());
    anchorDest.setVisible (isEditing());
    anchorSource.addMouseListener(this, false);
    anchorDest.addMouseListener(this, false);



}

NodeConnectionUI::~NodeConnectionUI()
{
    if (sourceConnector != nullptr && sourceConnector->getNodeUI())
    {
        sourceConnector->getNodeUI()->removeComponentListener (this);
        sourceConnector->removeComponentListener (this);
    }

    sourceConnector = nullptr;

    if (destConnector != nullptr && destConnector->getNodeUI())
    {
        destConnector->getNodeUI()->removeComponentListener (this);
        destConnector->removeComponentListener (this);
    }

    destConnector = nullptr;

    candidateDropConnector = nullptr;

    if (connection)
        connection->removeConnectionListener (this);

}

void NodeConnectionUI::paint (Graphics& g)
{


    Colour baseColor = getBaseConnector()->boxColor;


    if (candidateDropConnector != nullptr) baseColor = Colours::yellow;

    else if (isSelected) baseColor = findColour (TextButton::buttonOnColourId);
    else if (_isMouseOver) baseColor = Colours::red;
    g.setColour (baseColor);
    g.strokePath (path, PathStrokeType (1.5f));
}

void NodeConnectionUI::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    buildPath();
}

DynamicObject * NodeConnectionUI::createObject(){
    return connection->createObject();
}

void NodeConnectionUI::buildPath()
{
    path.clear();

    Point<float> sourcePos;
    Point<float> endPos;

    if (isEditing())
    {
        Point<float> t1 = getLocalPoint (getBaseConnector(), getBaseConnector()->getLocalBounds().getCentre()).toFloat();
        Point<float> t2 = (candidateDropConnector != nullptr) ?
                          getLocalPoint (candidateDropConnector, candidateDropConnector->getLocalBounds().getCentre()).toFloat() :
                          getMouseXYRelative().toFloat();

        sourcePos = getBaseConnector() == sourceConnector ? t1 : t2;
        endPos = getBaseConnector() == sourceConnector ? t2 : t1;

    }
    else
    {
        sourcePos = getLocalPoint (sourceConnector, sourceConnector->getLocalBounds().getCentre()).toFloat();
        endPos = getLocalPoint (destConnector, destConnector->getLocalBounds().getCentre()).toFloat();
    }


    Array<Point<float>> hitPoints;

    //NORMAL CURVE
    if (sourcePos.x < endPos.x - 20)
    {
        float cubicFactor = .5f;
        float txDist = (endPos.x - sourcePos.x) * cubicFactor;

        path.startNewSubPath (sourcePos.x, sourcePos.y);
        path.cubicTo (sourcePos.translated (txDist, 0), endPos.translated (-txDist, 0), endPos);

        int numPoints = 10;

        for (int i = 0; i <= numPoints; i++)
        {
            hitPoints.add (path.getPointAlongPath (path.getLength()*i / numPoints));
        }
    }
    else
    {
        Path p;
        float nodeMargin = 20;
        float destMidY = sourcePos.y + (endPos.y - sourcePos.y) / 2;
        float limitY1;// = getBaseConnector() == sourceConnector ? sourcePos.y : endPos.y;
        float limitY2 = getBaseConnector() == sourceConnector ? endPos.y : sourcePos.y;

        bool pathGoUp = getBaseConnector() == sourceConnector ? endPos.x < sourcePos.y : endPos.x > sourcePos.y;

        float cy = (float)getBaseConnector()->getBounds().getCentreY();
        limitY1 = sourcePos.y + (pathGoUp ? -cy - 10 : getBaseConnector()->getParentComponent()->getHeight() - cy + 10);

        if (getSecondConnector() != nullptr)
        {
            float cy2 = (float)getSecondConnector()->getBounds().getCentreY();
            limitY2 = endPos.y + (!pathGoUp ? -cy2 - 10 : getSecondConnector()->getParentComponent()->getHeight() - cy2 + 10);
        }
        else
        {
            limitY2 += pathGoUp ? 30 : -30;
        }

        destMidY = jlimit<float> (jmin<float> (limitY1, limitY2), jmax<float> (limitY1, limitY2), destMidY);

        Point<float> t1 = sourcePos.translated (nodeMargin, 0);
        Point<float> t2 = t1.withY (destMidY);
        Point<float> t3 = t2.withX (endPos.x - nodeMargin);
        Point<float> t4 = t3.withY (endPos.y);

        const Array<Point<float>> points = { t1, t2, t3, t4 };

        p.startNewSubPath (sourcePos.x, sourcePos.y);

        for (auto& tp : points) p.lineTo (tp);

        p.lineTo (endPos);

        path.addPath (p.createPathWithRoundedCorners (20));

        hitPoints.add (sourcePos);
        hitPoints.addArray (points);
        hitPoints.add (endPos);
    }

    buildHitPath (hitPoints);
}

void NodeConnectionUI::buildHitPath (Array<Point<float>> points)
{
    auto l = path.getLength();
    const double space = 15;
    auto p1 = path.getPointAlongPath (jmin (space, l * 0.1));

    anchorSource.setCentrePosition (p1.x, p1.y);
    p1 = path.getPointAlongPath (jmax (l - space, l * 0.9));
    anchorDest.setCentrePosition (p1.x, p1.y);
    float margin = 10;

    hitPath.clear();
    Array<Point<float>> firstPoints;
    Array<Point<float>> secondPoints;
    int numPoints = points.size();

    for (int i = 0; i < numPoints; i++)
    {
        Point<float> tp;
        Point<float> sp;

        if (i == 0 || i == numPoints - 1)
        {
            tp = points[i].translated (0, -margin);
            sp = points[i].translated (0, margin);
        }
        else
        {
            float angle1 = points[i].getAngleToPoint (points[i - 1]);
            float angle2 = points[i].getAngleToPoint (points[i + 1]);

            if (angle1 < 0) angle1 += float_Pi * 2;

            if (angle2 < 0) angle2 += float_Pi * 2;

            float angle = (angle1 + angle2) / 2.f;

            if (angle1 < angle2) angle += float_Pi;

            //            DBG("Point " << i << ", angle : " << angle << " >>  " << String(angle1>angle2));

            tp = points[i].getPointOnCircumference (margin, angle + float_Pi);
            sp = points[i].getPointOnCircumference (margin, angle);
        }

        firstPoints.add (tp);
        secondPoints.insert (0, sp);
    }

    hitPath.startNewSubPath (firstPoints[0]);

    for (int i = 1; i < firstPoints.size(); i++) hitPath.lineTo (firstPoints[i]);

    for (int i = 0; i < secondPoints.size(); i++) hitPath.lineTo (secondPoints[i]);

    hitPath.closeSubPath();
}

bool NodeConnectionUI::hitTest (int x, int y) {
    bool isOverAnchor = anchorDest.getBoundsInParent().contains(x,y)  ||
    anchorSource.getBoundsInParent().contains(x,y) ;
    if(isOverAnchor){
        DBG("overAnchor");
    }
    return hitPath.contains ((float)x, (float)y) || isOverAnchor;

}
void NodeConnectionUI::updateBoundsFromNodes()
{
    //  DBG("Update bounds from Nodes, is Editing ? " + String(isEditing()));
    if (!isEditing())
    {
        Component* nmui = getNodeManagerUI();

        if (nmui == nullptr) return;

        if (sourceConnector == nullptr || destConnector == nullptr) return;

        Point<int> cPos = nmui->getLocalPoint (sourceConnector, sourceConnector->getLocalBounds().getCentre());
        Point<int> mPos = nmui->getLocalPoint (destConnector, destConnector->getLocalBounds().getCentre());

        int minX = jmin<int> (cPos.x, mPos.x);
        int minY = jmin<int> (cPos.y, mPos.y);
        int tw = abs (cPos.x - mPos.x);
        int th = abs (cPos.y - mPos.y);
        int margin = 50;

        //      DBG(cPos.toString() + " // " + mPos.toString());

        setBounds (minX - margin, minY - margin, tw + margin * 2, th + margin * 2);

        repaint();
    }

}



void NodeConnectionUI::mouseDown (const MouseEvent& e)
{
    if (e.mods.isRightButtonDown())
    {
        PopupMenu m;
        //m.addItem(1,juce::translate("Edit connections..."));
        m.addItem (2, juce::translate("Delete all connections"));

        int result = m.show();

        switch (result)
        {
            case 1:
                //edit connection
                selectThis();
                break;

            case 2:
                connection->remove();
                break;
        }
    }
    else
    {
//        selectThis();


        if (isEditing() && candidateDropConnector)
        {
            if(auto nodeViewer = findParentComponentOfClass<NodeContainerViewer>()) {
                nodeViewer->finishEditingConnection();
            }
            else{
                jassertfalse;
            }
        }
        else if (!isEditing())
        {







        }

    }
}

void NodeConnectionUI::mouseUp (const MouseEvent& e)
{
    if ( auto nodeViewer = findParentComponentOfClass<NodeContainerViewer>()) {
        if(nodeViewer->isEditingConnection()){
            nodeViewer->finishEditingConnection();
        }
        else{
            selectThis();
        }

    }
}

void NodeConnectionUI::mouseMove (const MouseEvent& e)
{
    


}

void NodeConnectionUI::mouseDrag (const MouseEvent& e)
{
    if(isEditing()){
        findParentComponentOfClass<NodeContainerViewer>()->mouseDrag(e);
    }
    else if(e.getDistanceFromDragStart()>0){
        jassert(!anchorSource.isHovered || (anchorSource.isHovered != anchorDest.isHovered));
        if (anchorSource.isHovered || anchorDest.isHovered)
        {

            if ( auto nodeViewer = findParentComponentOfClass<NodeContainerViewer>()) {

                if (connection->connectionType == NodeConnection::ConnectionType::AUDIO) {
                    nodeViewer->createAudioConnectionFromConnector(
                                                                   anchorSource.isHovered ? destConnector : sourceConnector, connection);
                    connection->remove();
                }
            }
            else{
                jassertfalse;
            }
        }
        

    }

    
}

void NodeConnectionUI::mouseEnter (const MouseEvent&)
{
    _isMouseOver = true;
    anchorSource.setVisible(true);
    anchorDest.setVisible(true);
    repaint();
}

void NodeConnectionUI::mouseExit (const MouseEvent&)
{
    _isMouseOver = false;
    if(!isSelected){
        anchorSource.setVisible(false);
        anchorDest.setVisible(false);
    }
    repaint();
}

void NodeConnectionUI::setSelectedInternal(bool v){
    anchorSource.setVisible (v);
    anchorDest.setVisible (v);

}

bool NodeConnectionUI::keyPressed (const KeyPress& key)
{
    if (!isSelected) return false;

    if (key.getKeyCode() == KeyPress::deleteKey || key.getKeyCode() == KeyPress::backspaceKey)
    {
        // todo don't remove when multiple selection
        connection->remove();
        return true;
    }

    return false;
}

void NodeConnectionUI::setSourceConnector (Connector* c)
{
    if (sourceConnector != nullptr)
    {
        sourceConnector->removeComponentListener (this);
        sourceConnector->getNodeUI()->removeComponentListener (this);
    }

    sourceConnector = c;

    if (sourceConnector != nullptr)
    {
        sourceConnector->addComponentListener (this);
        sourceConnector->getNodeUI()->addComponentListener (this);
    }

    repaint();
}

void NodeConnectionUI::setDestConnector (Connector* c)
{
    if (destConnector != nullptr)
    {
        destConnector->removeComponentListener (this);
        destConnector->getNodeUI()->removeComponentListener (this);
    }

    destConnector = c;

    if (destConnector != nullptr)
    {
        destConnector->addComponentListener (this);
        destConnector->getNodeUI()->addComponentListener (this);
    }


    repaint();
}

void NodeConnectionUI::componentMovedOrResized (Component&, bool, bool)
{
    updateBoundsFromNodes();
}

bool NodeConnectionUI::setCandidateDropConnector (Connector* connector)
{
    //check if connector can accept data
    if (getBaseConnector() == connector) return false;

    if (getBaseConnector()->ioType == connector->ioType) return false;

    if (getBaseConnector()->dataType != connector->dataType) return false;

    candidateDropConnector = connector;

    return true;
}

void NodeConnectionUI::cancelCandidateDropConnector()
{
    candidateDropConnector = nullptr;
}

void NodeConnectionUI::startEditing(bool editDest){
    Desktop::getInstance().addGlobalMouseListener(this);
}
bool NodeConnectionUI::finishEditing()
{
    Desktop::getInstance().removeGlobalMouseListener(this);
    bool success = candidateDropConnector != nullptr;

    if (success)
    {
        if (sourceConnector == nullptr)
        {
            setSourceConnector (candidateDropConnector);
        }
        else
        {
            setDestConnector (candidateDropConnector);

        }
    }

    candidateDropConnector = nullptr;



    return success;
}

std::unique_ptr<InspectorEditor> NodeConnectionUI::createEditor()
{
    return std::make_unique< NodeConnectionEditor> (this);
}

void NodeConnectionUI::handleCommandMessage (int /*commandId*/)
{
    repaint();
}


void NodeConnectionUI::connectionAudioLinkAdded (const std::pair<int, int>&)
{
    postCommandMessage (0);
}

void NodeConnectionUI::connectionAudioLinkRemoved (const std::pair<int, int>&)
{
    postCommandMessage (0);
}

String NodeConnectionUI::getTooltip() {
    String tt;
    if(connection){
        tt+=connection->getFactoryInfo()+"\n("+String(connection->model.audioConnections.size())+ " connections)\n";
    if(sourceConnector)
        if(auto nb = sourceConnector->getNodeBase())
             tt+="\nin : " + String(nb->getTotalNumOutputChannels());
    if(destConnector)
        if(auto nb = destConnector->getNodeBase())
            tt+="\nout : " + String(nb->getTotalNumOutputChannels());

    }
    return tt;
    }

#endif
