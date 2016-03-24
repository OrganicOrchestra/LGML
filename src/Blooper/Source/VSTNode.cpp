/*
 ==============================================================================
 
 VSTNode.cpp
 Created: 2 Mar 2016 8:37:24pm
 Author:  bkupe
 
 ==============================================================================
 */

#include "VSTNode.h"

#include "NodeManager.h"
void  VSTNode::createPluginWindow(){

    

    if (PluginWindow* const w = PluginWindow::getWindowFor (this))
        w->toFront (true);

}

void VSTNode::closePluginWindow(){
    PluginWindow::closeCurrentlyOpenWindowsFor (this);
}




void VSTNode::VSTProcessor::numChannelsChanged(){
    NodeManager::getInstance()->audioGraph.removeIllegalConnections();
    // hack to force update renderingops in audioGraph
    NodeManager::getInstance()->audioGraph.removeConnection(-1);
}










#include "NodeBaseUI.h"
class VSTUI:public NodeBaseContentUI,public Button::Listener{
public:
    VSTUI(VSTNode * _owner):VSTListShowButton("VSTs"),showPluginWindowButton("showWindow"),owner(_owner){}
    void init() override{
        VSTListShowButton.addListener(this);
        showPluginWindowButton.addListener(this);
        addAndMakeVisible(showPluginWindowButton);
        addAndMakeVisible(VSTListShowButton);
        setSize(200, 100);
    }
    

    void resized()override{
        Rectangle<int> headerArea = getLocalBounds().removeFromTop(40);
        VSTListShowButton.setBounds(headerArea.removeFromLeft(headerArea.getWidth()/2));
        showPluginWindowButton.setBounds(headerArea);
    }
    
    TextButton VSTListShowButton;
    TextButton showPluginWindowButton;
    VSTNode * owner;
    
    
    static void vstSelected (int modalResult, Component *  originComp)
    {
        int index = VSTManager::getInstance()->knownPluginList.getIndexChosenByMenu(modalResult);
        if(index>=0 ){
           VSTUI * originVSTUI =  dynamic_cast<VSTUI*>(originComp);
            if(originVSTUI){
                originVSTUI->owner->generatePluginFromDescription(VSTManager::getInstance()->knownPluginList.getType (index));
            }
        }
    }
    
    void buttonClicked (Button* button) override
    {
        if (button == &VSTListShowButton){
            PopupMenu  VSTList;
            VSTManager::getInstance()->knownPluginList.addToMenu(VSTList, KnownPluginList::SortMethod::sortByCategory);
            owner->closePluginWindow();
            VSTList.showAt (&VSTListShowButton,0,0,0,0, ModalCallbackFunction::forComponent(&VSTUI::vstSelected, (Component*)this));
            
        }
        if(button == &showPluginWindowButton){
            owner->createPluginWindow();
        }
    }
    
};
NodeBaseUI * VSTNode::createUI(){return new NodeBaseUI(this,new VSTUI(this));}