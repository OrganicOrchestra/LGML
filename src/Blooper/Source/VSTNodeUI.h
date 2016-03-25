/*
  ==============================================================================

    VSTNodeUI.h
    Created: 24 Mar 2016 9:44:38pm
    Author:  Martin Hermant

  ==============================================================================
*/

#ifndef VSTNODEUI_H_INCLUDED
#define VSTNODEUI_H_INCLUDED



#include "NodeBaseUI.h"

#include "FloatSliderUI.h"
class VSTNodeUI:public NodeBaseContentUI,public Button::Listener,public ChangeListener,public ControllableContainer::Listener{
public:
    VSTNodeUI(VSTNode * _owner):
    VSTListShowButton("VSTs"),
    showPluginWindowButton("showWindow"),
    owner(_owner){
        owner->addChangeListener(this);
        owner->addControllableContainerListener(this);
    }
    ~VSTNodeUI(){
        owner->removeChangeListener(this);
        owner->removeControllableContainerListener(this);

    }
    
    void init() override{
        VSTListShowButton.addListener(this);
        showPluginWindowButton.addListener(this);
        addAndMakeVisible(showPluginWindowButton);
        addAndMakeVisible(VSTListShowButton);
        setSize(200, 100);
        updateVSTParameters();

    }
    
    void updateVSTParameters(){
        paramSliders.clear();
        
        int maxParameter = 20;
        int pCount = 0;

        for(auto &p:owner->VSTParameters){
            FloatSliderUI * slider = p->createSlider();
            paramSliders.add(slider);
            addAndMakeVisible(slider);
            pCount++;
            if(pCount>maxParameter){
                break;
            }
        }
        
        resized();
    }

    void controllableAdded(Controllable * c)override {};
    void controllableRemoved(Controllable * c)override{
        for(auto &p:paramSliders){
        if(p->floatParam == c){
            removeChildComponent (p);
            paramSliders.removeObject(p);
            break;
        }
        };
    }
    void controllableContainerAdded(ControllableContainer * cc)override{};
    void controllableContainerRemoved(ControllableContainer * cc) override{};
    void controllableFeedbackUpdate(Controllable *c) override{};
    
    
    
    void changeListenerCallback(ChangeBroadcaster * c) override{
        if(c == owner){
            updateVSTParameters();
        }
    }
    OwnedArray<FloatSliderUI> paramSliders;
    
    void resized()override{
        Rectangle<int> area = getLocalBounds();
        Rectangle<int> headerArea = area.removeFromTop(40);
        VSTListShowButton.setBounds(headerArea.removeFromLeft(headerArea.getWidth()/2));
        showPluginWindowButton.setBounds(headerArea);
        layoutSliderParameters(area.reduced(2));
        
    }
    
    void layoutSliderParameters(Rectangle<int> pArea){
        if(paramSliders.isEmpty())return;
        int maxLines = 4;
        
        int numLines = jmin(maxLines,paramSliders.size());
        int numCols = (paramSliders.size()-1)/maxLines + 1;
        
        int w = pArea.getWidth()/numCols;
        int h =pArea.getHeight()/numLines;
        int idx = 0;
        for(int i = 0 ; i < numCols ; i ++){
            Rectangle<int> col = pArea.removeFromLeft(w);
            for(int j = 0 ; j < numLines ; j++){
                paramSliders.getUnchecked(idx)->setBounds(col.removeFromTop(h).reduced(1));
                idx++;
                if(idx>=paramSliders.size()){
                    break;
                }
            }
            if(idx>=paramSliders.size()){
                break;
            }
        }
    }
    
    TextButton VSTListShowButton;
    TextButton showPluginWindowButton;
    VSTNode * owner;
    
    
    static void vstSelected (int modalResult, Component *  originComp)
    {
        int index = VSTManager::getInstance()->knownPluginList.getIndexChosenByMenu(modalResult);
        if(index>=0 ){
            VSTNodeUI * originVSTNodeUI =  dynamic_cast<VSTNodeUI*>(originComp);
            if(originVSTNodeUI){
                originVSTNodeUI->owner->generatePluginFromDescription(VSTManager::getInstance()->knownPluginList.getType (index));
            }
        }
    }
    
    void buttonClicked (Button* button) override
    {
        if (button == &VSTListShowButton){
            PopupMenu  VSTList;
            VSTManager::getInstance()->knownPluginList.addToMenu(VSTList, KnownPluginList::SortMethod::sortByCategory);
            owner->closePluginWindow();
            VSTList.showAt (&VSTListShowButton,0,0,0,0, ModalCallbackFunction::forComponent(&VSTNodeUI::vstSelected, (Component*)this));
            
        }
        if(button == &showPluginWindowButton){
            owner->createPluginWindow();
        }
    }
    
};


#endif  // VSTNODEUI_H_INCLUDED
