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


#include "GenericParameterContainerEditor.h"
#include "../../../UI/Inspector/InspectableComponent.h"
#include "ParameterUI.h"
#include "ParameterUIFactory.h"
#include "../ParameterContainer.h"
#include "../../../Preset/PresetChooserUI.h"
#include "../../../UI/Style.h"


GenericParameterContainerEditor::GenericParameterContainerEditor (ParameterContainer* _sourceContainer) :
    InspectorEditor(),
parentBT (juce::translate("Up"), juce::translate("Go back to parent container"))
{
    LGMLUIUtils::optionallySetBufferedToImage(&parentBT);

    parentBT.addListener (this);

    sourceContainer = _sourceContainer;
    addChildComponent (parentBT);


    setCurrentInspectedContainer (sourceContainer);

    sourceContainer->addControllableContainerListener (this);

    resized();
}

GenericParameterContainerEditor::~GenericParameterContainerEditor()
{
    if (sourceContainer.get())sourceContainer->removeControllableContainerListener (this);

    parentBT.removeListener (this);
    if(innerContainer.get())innerContainer->clear();
}

void GenericParameterContainerEditor::paint(Graphics & g){
    LGMLUIUtils::fillBackground(this,g);
}

void GenericParameterContainerEditor::setCurrentInspectedContainer (ParameterContainer* cc, bool forceUpdate,    int recursiveInspectionLevel, bool canInspectChildContainersBeyondRecursion)
{
//    if (cc == nullptr) return;
    
    if (innerContainer != nullptr)
    {
        if (!forceUpdate && cc == innerContainer->container) return;

        removeChildComponent (innerContainer.get());
        innerContainer = nullptr;
    }


    int ccLevel = 0;
    ControllableContainer* tc = cc;

    if(cc){
    while (tc != sourceContainer)
    {
        ccLevel++;
        tc = tc->parentContainer;
        jassert (tc != nullptr); //If here, trying to inspect a container that is not a child of the source inspectable container
    }

    
        innerContainer = std::make_unique<CCInnerContainerUI> (this, cc, 0, ccLevel == 0 ? recursiveInspectionLevel : 0, canInspectChildContainersBeyondRecursion);

    addAndMakeVisible (innerContainer.get());

    parentBT.setVisible (ccLevel > 0);

    if (parentBT.isVisible() && cc->parentContainer != nullptr) parentBT.setButtonText ("Up : " + cc->parentContainer->getNiceName());
    }
    resized();
    
}

int GenericParameterContainerEditor::getContentHeight() const
{
    if (innerContainer == nullptr || innerContainer->container.get()==nullptr) return 0;

    else return  innerContainer->getContentHeight() + parentBT.getHeight() + 5;
}

void GenericParameterContainerEditor::resized()
{
    InspectorEditor::resized();

    if (innerContainer == nullptr) return;

    Rectangle<int> r = getLocalBounds();

    if (parentBT.isVisible())
    {
        parentBT.setBounds (r.removeFromTop (20));
        r.removeFromTop (2);
    }

    innerContainer->setBounds (r);
}

void GenericParameterContainerEditor::clear()
{
    if (innerContainer == nullptr) return;

    innerContainer->clear();
}

void GenericParameterContainerEditor::buttonClicked (Button* b)
{
    if (b == &parentBT)
    {
        setCurrentInspectedContainer (static_cast<ParameterContainer*> (innerContainer->container->parentContainer));
    }
}

void GenericParameterContainerEditor::childStructureChanged (ControllableContainer*, ControllableContainer*,bool /*isAdded*/)
{
    if (!MessageManager::getInstance()->isThisTheMessageThread())
    {
        //    removeChildComponent(innerContainer);
        //    if(innerContainer){innerContainer->clear();removeChildComponent(innerContainer);innerContainer = nullptr;}
        postCommandMessage (CHILD_STRUCTURE_CHANGED);
    }
    else
    {
        handleCommandMessage (CHILD_STRUCTURE_CHANGED);
    }

}
void GenericParameterContainerEditor::containerWillClear(ControllableContainer * ){
//    handleCommandMessage(<#int cID#>)
}

void GenericParameterContainerEditor::handleCommandMessage (int cID)
{
    switch (cID)
    {
        case CHILD_STRUCTURE_CHANGED:
            // TODO: check differences
            startTimer (100);

            break;

        default:
            jassertfalse;
            break;
    }
}
void GenericParameterContainerEditor::timerCallback()
{
    setCurrentInspectedContainer (sourceContainer, true);

    inspectorEditorListeners.call (&InspectorEditorListener::contentSizeChanged, this);
    stopTimer();
};


/////////////////////////////
//Inner Container
//////////////////////////////

CCInnerContainerUI::CCInnerContainerUI (GenericParameterContainerEditor* _editor, ParameterContainer* _container, int _level, int _maxLevel, bool _canAccessLowerContainers) :
    editor (_editor),
    container (_container),
    level (_level),
    maxLevel (_maxLevel),
    canAccessLowerContainers (_canAccessLowerContainers),
    containerLabel ("containerLabel", _container->getNiceName())
{
//    setPaintingIsUnclipped(true);
    setOpaque(true);
    container->addControllableContainerListener (this);
//    containerLabel.setPaintingIsUnclipped(true);
    addAndMakeVisible (containerLabel);
    containerLabel.setFont (containerLabel.getFont().withHeight (10));
    containerLabel.setColour (containerLabel.backgroundColourId, findColour (ResizableWindow::backgroundColourId).brighter (.2f));
    containerLabel.setSize (containerLabel.getFont().getStringWidth (containerLabel.getText()) + 10, 14);



    rebuild();




}

CCInnerContainerUI::~CCInnerContainerUI()
{
    if(container.get())
        container->removeControllableContainerListener (this);
    clear();
}

void CCInnerContainerUI::rebuild()
{
    clear();

    if (container->canHavePresets())
    {
        presetChooser = std::make_unique<PresetChooserUI> (container);
        addAndMakeVisible (presetChooser.get());
    }

    for (auto& c : container->getControllablesOfType<ParameterBase> (false))
    {
        if(c==container->nameParam && !c->isEditable)continue;
        if (!c->isHidenInEditor ) addParameterUI (c);
    }

    if (level < maxLevel)
    {
        for (auto& cc : container->getContainersOfType<ParameterContainer> (false))
        {
            if(!cc->isHidenInEditor){
            addCCInnerUI (cc);
            }
        }
    }
    else if (level == maxLevel && canAccessLowerContainers)
    {
        for (auto& cc : container->getContainersOfType<ParameterContainer> (false))
        {
            if(!cc->isHidenInEditor){
            addCCLink (cc);
            }

        }


    }

    resized();
    editor->inspectorEditorListeners.call (&InspectorEditor::InspectorEditorListener::contentSizeChanged, editor);


}
void CCInnerContainerUI::addCCInnerUI (ParameterContainer* cc)
{

    CCInnerContainerUI* ccui = new CCInnerContainerUI (editor, cc, level + 1, maxLevel, canAccessLowerContainers);
    innerContainers.add (ccui);
    addAndMakeVisible (ccui);

}

void CCInnerContainerUI::removeCCInnerUI (ParameterContainer* cc)
{
    CCInnerContainerUI* ccui = getInnerContainerForCC (cc);

    if (ccui == nullptr) return;

    removeChildComponent (ccui);
    innerContainers.removeObject (ccui);
}

void CCInnerContainerUI::addCCLink (ParameterContainer* cc)
{


        CCLinkBT* bt = new CCLinkBT (cc);
    LGMLUIUtils::optionallySetBufferedToImage(bt);
        bt->addListener (this);
        addAndMakeVisible (bt);
        lowerContainerLinks.add (bt);

}

void CCInnerContainerUI::removeCCLink (ParameterContainer* cc)
{

    CCLinkBT* bt = getCCLinkForCC (cc);

    if (bt == nullptr) return;

    bt->removeListener (this);
    removeChildComponent (bt);
    lowerContainerLinks.removeObject (bt);
}

void CCInnerContainerUI::addParameterUI ( ParameterBase* c)
{
    if ( c->isHidenInEditor) return;


    auto cui = std::make_unique<NamedParameterUI> (ParameterUIFactory::createDefaultUI (c), 100);
    addAndMakeVisible (cui.get());
    parametersUI.add (std::move(cui));

}

void CCInnerContainerUI::removeParameterUI ( ParameterBase* c)
{
    NamedParameterUI*   cui = getUIForParameter (c);

    if (cui == nullptr) return;

    removeChildComponent (cui);
    parametersUI.removeObject(cui);//[cui](auto & p){return p.get()==cui;}) ;

}

NamedParameterUI*   CCInnerContainerUI::getUIForParameter ( ParameterBase* c)
{
    for (auto& cui : parametersUI)
    {

        if (cui->parameter == c) return cui;

    }

    return nullptr;
}

CCInnerContainerUI* CCInnerContainerUI::getInnerContainerForCC (ParameterContainer* cc)
{
    for (auto& ccui : innerContainers)
    {
        if (auto ncui = dynamic_cast<CCInnerContainerUI*> (ccui))
        {
            if (ncui->container == cc) return ncui;
        }
    }

    return nullptr;
}

CCInnerContainerUI::CCLinkBT* CCInnerContainerUI::getCCLinkForCC (ParameterContainer* cc)
{
    for (auto& _cclink : lowerContainerLinks)
    {
        if (auto cclink = dynamic_cast<CCInnerContainerUI::CCLinkBT*> (_cclink))
        {
            if (cclink->targetContainer == cc) return cclink;
        }
    }

    return nullptr;
}

int CCInnerContainerUI::getContentHeight() const
{
    if(!container.get()){
        jassertfalse;
        return 1;
    }
    int gap = 2;
    int ccGap = 5;
    int controllableHeight = 15;
    int ccLinkHeight = 20;
    int margin = 5;
    int presetChooserHeight = 15;

    int h = ccGap;
    h += parametersUI.size() * (controllableHeight + gap) + ccGap;

    for (auto& c : lowerContainerLinks)
    {
        bool isCustom = dynamic_cast<CCInnerContainerUI::CCLinkBT*> (c) == nullptr;
        h +=  (ccLinkHeight * (isCustom ? 3 : 1) + gap) + ccGap;
    }

    for (auto& ccui : innerContainers)
    {
        if(ccui->container.get()==nullptr) continue;
        if (auto icUI = dynamic_cast<CCInnerContainerUI*> (ccui)) {h += icUI->getContentHeight();}
        else {h += controllableHeight * 2;}

        h += ccGap;
    }

    if (container->canHavePresets()) h += presetChooserHeight + gap;

    h += containerLabel.getHeight();
    h += margin * 2;

    return h;
}

void CCInnerContainerUI::paint (Graphics& g)
{
    //if (level == 0) return;
    g.setColour(LGMLUIUtils::getCurrentBackgroundColor(this).brighter(.3f));
//    g.setColour (findColour (ResizableWindow::backgroundColourId).brighter (.3f));
    g.drawRoundedRectangle (getLocalBounds().toFloat(), 4, 2);
}

void CCInnerContainerUI::resized()
{
    if(!container.get()){
        jassertfalse;
        return;
    }
    int gap = 2;
    int ccGap = 5;
    int controllableHeight = 15;
    int ccLinkHeight = 20;
    int margin = 5;
    int presetChooserHeight = 15;

    Rectangle<int> r = getLocalBounds();
    containerLabel.setBounds (r.removeFromTop (containerLabel.getHeight()).withSizeKeepingCentre (containerLabel.getWidth(), containerLabel.getHeight()));

    r.reduce (margin, margin);

    if (customEditor)
    {
        customEditor->setBounds (r);
    }
    else
    {
        if (container->canHavePresets()  )
        {
            presetChooser->setBounds (r.removeFromTop (presetChooserHeight));
            r.removeFromTop (gap);
        }


        if (canAccessLowerContainers)
        {
            for (auto& cclink : lowerContainerLinks)
            {
                if (auto ccL =  dynamic_cast<CCInnerContainerUI::CCLinkBT*> (cclink) )
                {
                    if (ccL->targetContainer->isUserDefined)
                    {
                        cclink->setBounds (r.removeFromTop (ccLinkHeight));
                        r.removeFromTop (gap);
                    }
                }
            }

            r.removeFromTop (ccGap);
        }


        for (auto& cui : parametersUI)
        {
            cui->setBounds (r.removeFromTop (controllableHeight));
            r.removeFromTop (gap);
        }

        r.removeFromTop (ccGap);

        if (canAccessLowerContainers)
        {
            for (auto& cclink : lowerContainerLinks)
            {
                auto ccL = dynamic_cast<CCInnerContainerUI::CCLinkBT*> (cclink) ;
                bool isCustom = ccL == nullptr;

                if (!isCustom && ccL->targetContainer->isUserDefined) continue; // hide userDefined in inspector

                cclink->setBounds ( r.removeFromTop (ccLinkHeight) );
                r.removeFromTop (gap);
            }

            r.removeFromTop (ccGap);
        }

        for (auto& ccui : innerContainers)
        {
            if(ccui->container.get()==nullptr) continue;
            int h = controllableHeight * 2;

            if (auto icUI = dynamic_cast<CCInnerContainerUI*> (ccui)) {h = icUI->getContentHeight();}

            ccui->setBounds (r.removeFromTop (h));
            r.removeFromTop (ccGap);
        }
    }

}

void CCInnerContainerUI::clear()
{
    parametersUI.clear();
    innerContainers.clear();
    lowerContainerLinks.clear();
}

void CCInnerContainerUI::childControllableAdded (ControllableContainer*, Controllable* c)
{
    if (c->parentContainer != container) return;

    if (c->isHidenInEditor) return;

    auto pc = static_cast <ParameterBase*> (c);
    WeakReference<Component> thisRef(this);
    WeakReference<ParameterBase> pcRef(pc);
    MessageManager::callAsync(
                              [thisRef,pcRef](){
                                  if(thisRef && pcRef){
                                      auto *thisR = reinterpret_cast<CCInnerContainerUI*>(thisRef.get());
                                      thisR->addParameterUI (pcRef);
                                  }
                              });
}

void CCInnerContainerUI::childControllableRemoved (ControllableContainer*, Controllable* c)
{
    auto pc = static_cast <ParameterBase*> (c);
    jassert (pc);
    WeakReference<Component> thisRef(this);
    WeakReference<ParameterBase> pcRef(pc);
    MessageManager::callAsync(
                              [thisRef,pcRef](){
                                  if(thisRef && pcRef){
                                      auto *thisR = reinterpret_cast<CCInnerContainerUI*>(thisRef.get());
                                      thisR->removeParameterUI (pcRef);
                                  }
                              });

}

void CCInnerContainerUI::controllableContainerAdded (ControllableContainer*, ControllableContainer* cc)
{
    if (cc->parentContainer != container) return;

    auto pc = static_cast<ParameterContainer*> (cc);
    jassert (pc);
    if (pc->isHidenInEditor) return;

    WeakReference<Component> thisRef(this);
    WeakReference<ParameterContainer> pcRef(pc);
    MessageManager::callAsync(
                              [thisRef,pcRef](){
                                  if(thisRef && pcRef){
                                      auto *thisR = reinterpret_cast<CCInnerContainerUI*>(thisRef.get());

                                      if (thisR->level < thisR->maxLevel) thisR->addCCInnerUI (pcRef);
                                      else if (thisR->canAccessLowerContainers) thisR->addCCLink (pcRef);
                                  }
                              });
}

void CCInnerContainerUI::controllableContainerRemoved (ControllableContainer*, ControllableContainer* cc)
{
    auto pc = static_cast<ParameterContainer*> (cc);
    WeakReference<Component> thisRef(this);
    WeakReference<ParameterContainer> pcRef(pc);
    MessageManager::callAsync(
                              [thisRef,pcRef](){
                                  if(thisRef && pcRef){
                                      auto *thisR = reinterpret_cast<CCInnerContainerUI*>(thisRef.get());
                                      thisR->removeCCInnerUI (pcRef);
                                      thisR->removeCCLink (pcRef);
                                  }
                              });

}


void CCInnerContainerUI::childStructureChanged (ControllableContainer*, ControllableContainer*,bool /*isAdded*/)
{
    //resized();

}

void CCInnerContainerUI::buttonClicked (Button* b)
{

    CCLinkBT* bt = dynamic_cast<CCLinkBT*> (b);

    if (bt == nullptr) return;

    editor->setCurrentInspectedContainer (bt->targetContainer);

}

CCInnerContainerUI::CCLinkBT::CCLinkBT (ParameterContainer* _targetContainer) :
    targetContainer (_targetContainer),
    TextButton ("[ "+juce::translate("Inspect")+" " + _targetContainer->getNiceName() + " >> ]")
{
    LGMLUIUtils::optionallySetBufferedToImage(this);
    setPaintingIsUnclipped(true);
}

#endif

