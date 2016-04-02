///*
//  ==============================================================================
//
//    ConnectionEditor.h
//    Created: 1 Apr 2016 7:15:15pm
//    Author:  Martin Hermant
//
//  ==============================================================================
//*/
//



//////
// WIP check how we can abstract connection mechanisms and share UI objects with canvas
// Two possible direction for connection Routing editing : popup window with in / out   //  maya style unfolding
///////


//#ifndef CONNECTIONEDITOR_H_INCLUDED
//#define CONNECTIONEDITOR_H_INCLUDED
//
//#include "JuceHeader.h"
//#include "NodeConnectionUI.h"
//
//class ConnectionEditor : public Component{
//    public :
//    ConnectionEditor(NodeConnectionUI * cUI):connectionUI(cUI){
//
//    }
//
//    class ConnectorComponent : public ShapeButton{
//    public:
//
//        ConnectorComponent(const String & name,int _uid):
//        ShapeButton(name,Colours::white,Colours::black,Colours::yellow),
//        uid(_uid){
//
//        }
//
//        int uid;
//
//    };
//
//
//    class ConnectionUI : public Component , public ComponentListener{
//
//    public:
//
//        ConnectionUI(){
//
//        }
//        void componentMovedOrResized(Component * c) override{
//
//        }
//
//
//    void paint (Graphics& g)override
//    {
//
//        //DBG("PAINT !!");
//        Point<float> sourcePos;
//        Point<float> endPos;
//
//        if (isEditing())
//        {
//            sourcePos = getLocalPoint(getBaseConnector(),getBaseConnector()->getLocalBounds().getCentre()).toFloat();
//            endPos = (candidateDropConnector != nullptr)?
//            getLocalPoint(candidateDropConnector, candidateDropConnector->getLocalBounds().getCentre()).toFloat():
//            getMouseXYRelative().toFloat();
//        }else
//        {
//            sourcePos = getLocalPoint(sourceConnector, sourceConnector->getLocalBounds().getCentre()).toFloat();
//            endPos = getLocalPoint(destConnector, destConnector->getLocalBounds().getCentre()).toFloat();
//        }
//
//        Point<float> midPoint = (sourcePos + endPos) / 2;
//
//        //  int minDist = -200;
//        //  int maxDist = 100;
//        //  float minOffset = 0;
//        //  float maxOffset = 150;
//        //
//        //  float anchorOffset = jmap<float>(endPos.x-sourcePos.x, maxDist, minDist, minOffset, maxOffset);
//        //  anchorOffset = jmin<float>(jmax<float>(anchorOffset, minOffset), maxOffset);
//        //
//        //  int sourceAnchorX = (sourcePos.x + midPoint.x)/2 + anchorOffset;
//        //  int endAnchorX = (endPos.x+midPoint.x)/2  - anchorOffset;
//
//        //@ben I prefer that atm (at least it doesnt start with weird anchors)
//        //but I'm up for any other fancy complexoid cuved Path algos that never touch a node
//
//        float smoothBigConnector = 1+ .01f*(jmax<float>(10,std::abs(endPos.x - sourcePos.x))-10);
//        float anchorOffset = (endPos.x - sourcePos.x)/(2*smoothBigConnector);
//        float sourceAnchorX = sourcePos.x + anchorOffset;
//        float endAnchorX = endPos.x - anchorOffset;
//
//
//        int hitMargin = 10;
//        hitPath.clear();
//        hitPath.startNewSubPath(sourcePos.x, sourcePos.y - hitMargin);
//        hitPath.quadraticTo(sourceAnchorX, sourcePos.y - hitMargin, midPoint.x, midPoint.y - hitMargin);
//        hitPath.quadraticTo(endAnchorX, endPos.y - hitMargin, endPos.x, endPos.y - hitMargin);
//        hitPath.lineTo(endPos.x, endPos.y + hitMargin);
//        hitPath.quadraticTo(endAnchorX, endPos.y + hitMargin, midPoint.x, midPoint.y + hitMargin);
//        hitPath.quadraticTo(sourceAnchorX, sourcePos.y + hitMargin, sourcePos.x, sourcePos.y + hitMargin);
//        hitPath.closeSubPath();
//
//        Path p;
//        p.startNewSubPath(sourcePos.x, sourcePos.y);
//        p.quadraticTo(sourceAnchorX, sourcePos.y, midPoint.x, midPoint.y);
//        p.quadraticTo(endAnchorX, endPos.y, endPos.x, endPos.y);
//        
//        Colour baseColor = getBaseConnector()->dataType == NodeConnection::ConnectionType::AUDIO ? AUDIO_COLOR : DATA_COLOR;
//        g.setColour((candidateDropConnector != nullptr) ? Colours::yellow : isMouseOver()?Colours::orange:baseColor );
//        g.strokePath(p, PathStrokeType(2.0f));
//        
//    }
//
//
//};
//    void generateConnectors(NodeConnectionUI * cUI){
//        for(auto &c:cUI->connection->audioConnections){
//            addConnector(String(c.first),c.first,false);
//            addConnector(String(c.second),c.second,true);
//        }
//    }
//    void addConnector(const String & name,int uid,bool isInput){
//        ConnectorComponent *c= new ConnectorComponent(name,uid);
//        if(isInput){inputConnectors.add(c);}
//        else{outputConnectors.add(c);}
//        addAndMakeVisible(c);
//    }
//
//    void resized() override{
//
//        Rectangle<int> area = getLocalBounds();
//        Rectangle<int> outputArea = area.removeFromLeft(30);
//        int numConnout = inputConnectors.size();
//        float stepout = outputArea.getHeight()/numConnout;
//        for(auto &c:outputConnectors){
//            c->setBounds(outputArea.removeFromTop(stepout));
//        }
//
//        Rectangle<int> inputArea = area.removeFromLeft(30);
//        int numConnin = inputConnectors.size();
//        float stepin = inputArea.getHeight()/numConnin;
//        for(auto &c:inputConnectors){
//            c->setBounds(inputArea.removeFromTop(stepin));
//        }
//
//    }
//
//    OwnedArray<ConnectorComponent> outputConnectors;
//    OwnedArray<ConnectorComponent> inputConnectors;
//    NodeConnectionUI* connectionUI;
//
//
//};
//
//
//#endif  // CONNECTIONEDITOR_H_INCLUDED
