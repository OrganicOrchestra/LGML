/*
 ==============================================================================

 VSTNodeUI.cpp
 Created: 24 Mar 2016 9:44:38pm
 Author:  Martin Hermant

 ==============================================================================
 */

#include "VSTNodeUI.h"


VSTNodeContentUI::VSTNodeContentUI():
VSTListShowButton("VSTs"),
showPluginWindowButton("showWindow")
{
    
}
VSTNodeContentUI::~VSTNodeContentUI(){
	vstNode->removeVSTNodeListener(this);
	vstNode->removeControllableContainerListener(this);
}

void VSTNodeContentUI::init() {
	vstNode = (VSTNode *)node;
    VSTListShowButton.addListener(this);
    showPluginWindowButton.addListener(this);
    addAndMakeVisible(showPluginWindowButton);
    addAndMakeVisible(VSTListShowButton);
    setSize(200, 100);
    updateVSTParameters();

	vstNode->addVSTNodeListener(this);
	vstNode->addControllableContainerListener(this);

}

void VSTNodeContentUI::updateVSTParameters(){
    paramSliders.clear();

    int maxParameter = 20;
    int pCount = 0;

    for(auto &p: vstNode->VSTParameters){
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

void VSTNodeContentUI::controllableAdded(Controllable *) {};
void VSTNodeContentUI::controllableRemoved(Controllable * c){

    for(auto &slider:paramSliders){
        if(slider->parameter == c){
            removeChildComponent (slider);
            paramSliders.removeObject(slider);
            break;
        }
    };

}
void VSTNodeContentUI::controllableContainerAdded(ControllableContainer *){};
void VSTNodeContentUI::controllableContainerRemoved(ControllableContainer *) {};
void VSTNodeContentUI::controllableFeedbackUpdate(Controllable *) {};



//Listener From VSTNode
void VSTNodeContentUI::newVSTSelected() {
    updateVSTParameters();
}

void VSTNodeContentUI::resized(){
    Rectangle<int> area = getLocalBounds();
    Rectangle<int> headerArea = area.removeFromTop(40);
    VSTListShowButton.setBounds(headerArea.removeFromLeft(headerArea.getWidth()/2));
    showPluginWindowButton.setBounds(headerArea);
    layoutSliderParameters(area.reduced(2));

}

void VSTNodeContentUI::layoutSliderParameters(Rectangle<int> pArea){
    if(paramSliders.size() == 0) return;
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


void VSTNodeContentUI::vstSelected (int modalResult, Component *  originComp)
{
    int index = VSTManager::getInstance()->knownPluginList.getIndexChosenByMenu(modalResult);
    if(index>=0 ){
        VSTNodeContentUI * originVSTNodeUI =  dynamic_cast<VSTNodeContentUI*>(originComp);
        if(originVSTNodeUI){
            originVSTNodeUI->vstNode->identifierString->setValue(VSTManager::getInstance()->knownPluginList.getType (index)->createIdentifierString());
//            originVSTNodeUI->owner->generatePluginFromDescription(VSTManager::getInstance()->knownPluginList.getType (index));
        }
    }
}

void VSTNodeContentUI::buttonClicked (Button* button)
{
    if (button == &VSTListShowButton){
        PopupMenu  VSTList;
        VSTManager::getInstance()->knownPluginList.addToMenu(VSTList, KnownPluginList::SortMethod::sortByCategory);
		vstNode->closePluginWindow();
        VSTList.showAt (&VSTListShowButton,0,0,0,0, ModalCallbackFunction::forComponent(&VSTNodeContentUI::vstSelected, (Component*)this));

    }
    if(button == &showPluginWindowButton){
		vstNode->createPluginWindow();
    }
}


////
// HEADER UI
//

VSTNodeHeaderUI::VSTNodeHeaderUI()
{
	//setSize(getWidth(), 80);
}

VSTNodeHeaderUI::~VSTNodeHeaderUI()
{
	vstNode->removeVSTNodeListener(this);
}

void VSTNodeHeaderUI::init()
{
	vstNode = (VSTNode *)node;
	vstNode->addVSTNodeListener(this);
}


void VSTNodeHeaderUI::newVSTSelected()
{
	updatePresetComboBox();
}
