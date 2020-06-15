/*
 ==============================================================================

 Copyright © Organic Orchestra, 2017

 This file is part of LGML. LGML is a software to manipulate sound in real-time

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation (version 3 of the License).

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 ==============================================================================
 */

#if !ENGINE_HEADLESS

#include "FastMapperUI.h"
#include "FastMapper.h"
#include "../Controllable/Parameter/UI/ParameterUIFactory.h"
#include "../Utils/FactoryUIHelpers.h"

struct CompareFastMaps
{
    static int compareElements(const FastMap *a, const FastMap *b)
    {
        if (a == b)
        {
            return 0;
        }
        const auto nameA = a->referenceIn->getLinkedParam() ? a->referenceIn->getLinkedParam()->niceName : "";
        const auto nameB = b->referenceIn->getLinkedParam() ? b->referenceIn->getLinkedParam()->niceName : "";
        return nameA.compare(nameB);
    }
};

FastMapperUI::FastMapperUI(const String &contentName, FastMapper *_fastMapper) : ShapeShifterContent(this, contentName, "Link parameters together\nAdd FastMap here\nCmd+m toggles mapping mode"),
                                                                                 InspectableComponent(_fastMapper),
                                                                                 fastMapper(_fastMapper),
                                                                                 mapsUI(new StackedContainerUI<FastMapUI, FastMap>(
                                                                                     [](FastMapUI *ui) { return ui->fastMap; },
                                                                                     [_fastMapper](int ia, int ib) {
                                                                                         int iia = _fastMapper->controllableContainers.indexOf(_fastMapper->maps[ia]);
                                                                                         int iib = _fastMapper->controllableContainers.indexOf(_fastMapper->maps[ib]);
                                                                                         _fastMapper->controllableContainers.swap(iia, iib);
                                                                                         _fastMapper->maps.swap(ia, ib);
                                                                                     },
                                                                                     45,
                                                                                     false, 2, 40))
{

    fastMapper->addControllableContainerListener(this);

    linkToSelection.setButtonText(juce::translate("Show from selected"));
    LGMLUIUtils::optionallySetBufferedToImage(&linkToSelection);
    linkToSelection.setTooltip(juce::translate("Filter viewed fastmap to currently selected element :\n- Node\n- Controller\n- Parameter\n- ..."));
    linkToSelection.setClickingTogglesState(true);
    linkToSelection.addListener(this);
    addAndMakeVisible(linkToSelection);

    potentialIn = ParameterUIFactory::createDefaultUI(fastMapper->potentialIn);
    potentialOut = ParameterUIFactory::createDefaultUI(fastMapper->potentialOut);
    candidateLabel.setText(juce::translate("candidate"), dontSendNotification);
    addAndMakeVisible(candidateLabel);
    addAndMakeVisible(potentialIn.get());
    addAndMakeVisible(potentialOut.get());

    addAndMakeVisible(mapsUI);

    addAndMakeVisible(addFastMapButton);
    addFastMapButton.addListener(this);
    addFastMapButton.setTooltip(juce::translate("Add FastMap"));

    contentIsFlexible = true;

    LGMLDragger::getInstance()->addSelectionListener(this);
    mapsUI.stUI->setSortFunction([](const FastMapUI *a, const FastMapUI *b) {
        return CompareFastMaps::compareElements(a->fastMap, b->fastMap);
    });
    mapsUI.stUI->setFilterFunction([this](FastMapUI *a) {
        return mapPassViewFilter(a->fastMap);
    });

    resetAndUpdateView();
}

FastMapperUI::~FastMapperUI()
{
    if (auto i = Inspector::getInstanceWithoutCreating())
    {
        i->removeInspectorListener(this);
    }
    fastMapper->removeControllableContainerListener(this);
    if (auto *i = LGMLDragger::getInstanceWithoutCreating())
    {
        i->removeSelectionListener(this);
    }

    clear();
}

void FastMapperUI::addFastMapUI(FastMap *f)
{
    mapsUI.addFromT(f);
}

void FastMapperUI::removeFastMapUI(FastMapUI *fui)
{

    if (fui == nullptr)
        return;
    mapsUI.removeUI(fui);
}

void FastMapperUI::resetAndUpdateView()
{

    clear();
    for (auto &f : fastMapper->maps)
    {
        addFastMapUI(f);
    }
    //    Array<FastMap*> displayedMaps;
    //    Array<String> groupedNames;
    //    for (auto& f : fastMapper->maps)
    //    {
    //        if (mapPassViewFilter (f)){
    //            displayedMaps.add(f);
    //            String nameToAdd = "target";
    //            if(auto p=f->referenceIn->getLinkedParam()){
    //                nameToAdd = p->niceName;
    //            }
    //            groupedNames.add(nameToAdd);
    //        }
    //    }
    //
    //    displayedMaps.sort(compareFastMaps);
    //    int i = 0;
    //    for (auto& f : displayedMaps)
    //    {
    //        bool displayFull = true;
    //        if(i>0){
    //            displayFull = groupedNames[i-1]!=groupedNames[i];
    //        }
    //         addFastMapUI (f,displayFull,true);
    //        i++;
    //    }

    resized();
}

void FastMapperUI::setViewFilter(const Array<WeakReference<ContainerType>> &filterContainer)
{
    viewFilterContainers = filterContainer;
    viewFilterControllables.clear();
    WeakReference<Component> thisRef(this);
    MessageManager::getInstance()->callAsync([thisRef]() {
        if (thisRef)
        {
            static_cast<FastMapperUI *>(thisRef.get())->resetAndUpdateView();
        }
    });
}

void FastMapperUI::setViewFilter(const Array<WeakReference<ControllableType>> &filterControllable)
{
    viewFilterContainers.clear();
    viewFilterControllables = filterControllable;
    WeakReference<Component> thisRef(this);
    MessageManager::getInstance()->callAsync([thisRef]() {
        if (thisRef)
        {
            static_cast<FastMapperUI *>(thisRef.get())->resetAndUpdateView();
        }
    });
}

void FastMapperUI::resetViewFilter()
{
    viewFilterContainers.clear();
    viewFilterControllables.clear();
    WeakReference<Component> thisRef(this);
    MessageManager::getInstance()->callAsync([thisRef]() {
        if (thisRef)
        {
            static_cast<FastMapperUI *>(thisRef.get())->resetAndUpdateView();
        }
    });
}

bool FastMapperUI::mapPassViewFilter(FastMap *f)
{

    if (viewFilterContainers.size() == 0 && viewFilterControllables.size() == 0)
        return true;

    for (auto &viewFilterContainer : viewFilterContainers)
    {
        if (f->referenceIn->linkedParam != nullptr && (ControllableContainer *)f->referenceIn->linkedParam->isChildOf(viewFilterContainer))
            return true;

        if (f->referenceOut->linkedParam != nullptr && (ControllableContainer *)f->referenceOut->linkedParam->isChildOf(viewFilterContainer))
            return true;
    }

    for (auto &viewFilterControllable : viewFilterControllables)
    {
        if (f->referenceIn->linkedParam == viewFilterControllable ||
            f->referenceOut->linkedParam == viewFilterControllable)
            return true;
    }

    // pass through emptys
    return (!f->referenceOut->linkedParam && !f->referenceIn->linkedParam);
}

int getTargetHeight()
{
    if (auto ld = LGMLDragger::getInstanceWithoutCreating())
        return ld->isMappingActive ? 80 : 21;

    return 0;
}
constexpr int linkBtHeight = 21;
int FastMapperUI::getContentHeight() const
{

    return mapsUI.getSize() + getTargetHeight() + linkBtHeight + 10;
}

void FastMapperUI::resized()
{

    Rectangle<int> r = getLocalBounds().reduced(2);
    auto inputHeader = r.removeFromTop(getTargetHeight());
    candidateLabel.setBounds(inputHeader.removeFromLeft(100));

    potentialIn->setBounds(inputHeader.removeFromLeft(inputHeader.getWidth() / 2).reduced(2));
    potentialOut->setBounds(inputHeader.reduced(2));
    linkToSelection.setBounds(r.removeFromTop(linkBtHeight).reduced(2));

    // empty shows help
    if (mapsUI.getNumStacked() == 0)
    {
        //        auto labelR = r.removeFromTop(r.getHeight()/2);
        //        infoLabel.setVisible(true);
        //        infoLabel.setBounds(labelR);
        int side = (int)(jmin(r.getWidth(), r.getHeight()) * .95);
        addFastMapButton.setBounds(r.withSizeKeepingCentre(side, side));
    }
    else
    {
        r.removeFromTop(5);
        r.reduce(2, 0);
        mapsUI.setBounds(r);
        //        infoLabel.setVisible(false);
        r.removeFromRight(15); // scrollbar
        addFastMapButton.setFromParentBounds(r);
    }
}

void FastMapperUI::clear()
{
    mapsUI.clear();
}

void FastMapperUI::mouseDown(const MouseEvent &e)
{
    if (e.mods.isRightButtonDown())
    {
        PopupMenu m;
        m.addItem(1, juce::translate("Add FastMap"));
        int result = m.show();

        switch (result)
        {
        case 1:
            addFastMapUndoable();
            break;
        }
    }
}

void FastMapperUI::controllableContainerAdded(ControllableContainer *ori, ControllableContainer *cc)
{
    if (ori == fastMapper)
    {
        if (dynamic_cast<FastMap *>(cc))
        {
            WeakReference<ControllableContainer> wf(cc);
            WeakReference<Component> thisRef(this);
            MessageManager::callAsync([thisRef, wf]() {
                if (wf.get() && thisRef.get())
                {
                    auto fmui = static_cast<FastMapperUI *>(thisRef.get());
                    fmui->mapsUI.addFromT((FastMap *)wf.get());
                    fmui->resized();
                }
            });
        }
        else
        {
            jassertfalse; // just to check...
        }
    }

    //    addFastMapUI(f);
    //    resized();
}

void FastMapperUI::controllableContainerRemoved(ControllableContainer *ori, ControllableContainer *cc)
{
    if (ori == fastMapper)
    {

        WeakReference<Component> fui(mapsUI.getFromT((FastMap *)cc));
        execOrDefer([=]() {
            if (fui.get())
            {
                removeFastMapUI(dynamic_cast<FastMapUI *>(fui.get()));
                resized();
            }
        });
    }
}

void FastMapperUI::buttonClicked(Button *b)
{
    if (b == &linkToSelection)
    {
        if (linkToSelection.getToggleState())
        {
            Inspector::getInstance()->addInspectorListener(this);
            setViewFilter(Inspector::getInstance()->getContainersSelected());
        }
        else
        {
            Inspector::getInstance()->removeInspectorListener(this);
            resetViewFilter();
        }
    }
    else if (b == &addFastMapButton)
    {
        addFastMapUndoable();
    }
}

ParameterUI *getParamFromComponent(Component *comp)
{
    if (!comp)
        return nullptr;
    if (auto pui = dynamic_cast<ParameterUI *>(comp))
        return pui;
    for (auto c : comp->getChildren())
    {
        if (auto pui = dynamic_cast<ParameterUI *>(c))
            return pui;
    }
    return nullptr;
}

void FastMapperUI::selectionChanged(Inspector *i)
{
    jassert(linkToSelection.getToggleState());
    auto *curCont = i->getFirstCurrentContainerSelected();
    // prevent self selection
    if (dynamic_cast<FastMap *>(curCont) || dynamic_cast<FastMapper *>(curCont))
    {
        return;
    }

    if (WeakReference<ParameterContainer> container = dynamic_cast<ParameterContainer *>(curCont))
        setViewFilter(Array<WeakReference<ParameterContainer>>{container});
    else if (auto pui = getParamFromComponent(i->getFirstCurrentComponent()))
    {
        setViewFilter(pui->parameter);
    }
};

void FastMapperUI::mappingModeChanged(bool)
{
    resized();
}

void FastMapperUI::addFastMapUndoable()
{
    getAppUndoManager().beginNewTransaction("add FastMap ");
    getAppUndoManager().perform(new FactoryUIHelpers::UndoableCreate<ParameterContainer>(
        [=]() { return fastMapper->addFastMap(); },
        [=](ParameterContainer *f) {
            fastMapper->removeFastmap(dynamic_cast<FastMap *>(f));
        }));
}

#endif
