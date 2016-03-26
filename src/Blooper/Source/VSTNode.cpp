/*
 ==============================================================================
 
 VSTNode.cpp
 Created: 2 Mar 2016 8:37:24pm
 Author:  bkupe
 
 ==============================================================================
 */

#include "VSTNode.h"

#include "NodeManager.h"

#include "VSTNodeUI.h"


NodeBaseUI * VSTNode::createUI(){return new NodeBaseUI(this,new VSTNodeUI(this));}


VSTNode::VSTNode(NodeManager * nodeManager,uint32 nodeId) :
NodeBase(nodeManager,nodeId,"VST",new VSTProcessor(this)),blockFeedback(false){
    identifierString = addStringParameter("VST Identifier","string that identify a VST","");
    addChildControllableContainer(&pluginWindowParameter);
}


VSTNode::~VSTNode(){
    PluginWindow::closeCurrentlyOpenWindowsFor (this);
}


void VSTNode::generatePluginFromDescription(PluginDescription * desc){
    VSTProcessor * vstProcessor = dynamic_cast<VSTProcessor*>(audioProcessor);
    vstProcessor->generatePluginFromDescription(desc);
}
void  VSTNode::createPluginWindow(){
    if (PluginWindow* const w = PluginWindow::getWindowFor (this))
        w->toFront (true);
    
}

void VSTNode::closePluginWindow(){
    PluginWindow::closeCurrentlyOpenWindowsFor (this);
}

void VSTNode::parameterValueChanged(Parameter * p) {
    if(p==identifierString){
        if(identifierString->value!=""){
            PluginDescription * pd = VSTManager::getInstance()->knownPluginList.getTypeForIdentifierString (identifierString->value);
            if(pd){generatePluginFromDescription(pd);}
            else{DBG("VST : cant find plugin for identifier : "+identifierString->value);}
        }
        else{DBG("VST : no identifierStrind provided");}
    }
    
    // a VSTParameter is changed
    else{
        if(blockFeedback)return;
        for(int i = VSTParameters.size() -1; i>=0;--i){
            if(VSTParameters.getUnchecked(i) == p){
                VSTProcessor * vstProcessor = dynamic_cast<VSTProcessor*>(audioProcessor);
                vstProcessor->innerPlugin->setParameter(i, VSTParameters.getUnchecked(i)->value);
                
                break;
            }
            
        }
    }
};
void VSTNode::initParameterFromProcessor(AudioProcessor * p){
    p->addListener(this);
    
    for(auto &c:VSTParameters){removeControllable(c);}
    
    VSTParameters.clear();
    
    for(int i = 0 ; i < p->getNumParameters() ; i++){
        VSTParameters.add(addFloatParameter(p->getParameterName(i), p->getParameterLabel(i), p->getParameter(i)));
    }
    sendChangeMessage();
}


void VSTNode::VSTProcessor::numChannelsChanged(){
    NodeManager::getInstance()->audioGraph.removeIllegalConnections();
    // hack to force update renderingops in audioGraph
    NodeManager::getInstance()->audioGraph.removeConnection(-1);
}


void VSTNode::audioProcessorParameterChanged (AudioProcessor* processor,
                                              int parameterIndex,
                                              float newValue) {
    
    jassert(parameterIndex<VSTParameters.size());
    blockFeedback = true;
    
    VSTParameters.getUnchecked(parameterIndex)->setValue(newValue);
    
    blockFeedback = false;
}



