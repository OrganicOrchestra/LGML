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


void  VSTNode::createPluginWindow(){
    if (PluginWindow* const w = PluginWindow::getWindowFor (this))
        w->toFront (true);
    
}

void VSTNode::closePluginWindow(){
    PluginWindow::closeCurrentlyOpenWindowsFor (this);
}

void VSTNode::parameterValueChanged(Parameter * p) {
    if(blockFeedback)return;
    for(int i = VSTParameters.size() -1; i>=0;--i){
        if(VSTParameters.getUnchecked(i) == p){
            VSTProcessor * vstProcessor = dynamic_cast<VSTProcessor*>(audioProcessor);
            vstProcessor->innerPlugin->setParameter(i, VSTParameters.getUnchecked(i)->value);
            
            break;
        }
        
    }
};
void VSTNode::initParameterFromProcessor(AudioProcessor * p){
    p->addListener(this);
    if(!VSTParameters.empty()){
        for(auto &c:VSTParameters){
            removeControllable(c);
        }
    }
    
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



