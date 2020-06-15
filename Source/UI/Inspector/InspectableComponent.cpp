/* Copyright © Organic Orchestra, 2017
 *
 * This file is part of LGML.  LGML is a software to manipulate sound in real-time
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

#include "InspectableComponent.h"
#include "Inspector.h"

#include "../../Controllable/Parameter/UI/GenericParameterContainerEditor.h"
#include "../ShapeShifter/ShapeShifterFactory.h"
#include "../ShapeShifter/ShapeShifterManager.h"
#include "../Style.h"

#include "../../Controllable/Parameter/UI/ParameterUIFactory.h"

InspectableComponent::InspectableComponent(const String &_inspectableType) : inspectableType(_inspectableType),
                                                                             relatedParameterContainer(nullptr),
                                                                             relatedParameter(nullptr)
{
    init();
}
InspectableComponent::InspectableComponent(ParameterContainer *_relatedContainer, const String &_inspectableType) : inspectableType(_inspectableType),
                                                                                                                    relatedParameterContainer(_relatedContainer),
                                                                                                                    relatedParameter(nullptr)
{
    visibleName = juce::translate(_relatedContainer->getNiceName());
    init();
}

InspectableComponent::InspectableComponent(ParameterBase *_relatedParameter, const String &_inspectableType) : inspectableType(_inspectableType),
                                                                                                               relatedParameterContainer(nullptr),
                                                                                                               relatedParameter(_relatedParameter)
{
    visibleName = juce::translate(_relatedParameter->niceName);
    init();
}

void InspectableComponent::init()
{
    recursiveInspectionLevel = 0;
    canInspectChildContainersBeyondRecursion = true;
    isSelected = false;
    paintBordersWhenSelected = true;
    bringToFrontOnSelect = true;
    setWantsKeyboardFocus(true);
    setPaintingIsUnclipped(true);
    setOpaque(true);
}

void InspectableComponent::setRelatedContainer(ParameterContainer *pc)
{
    jassert(relatedParameter == nullptr);
    jassert(relatedParameterContainer == nullptr);
    relatedParameterContainer = pc;
    visibleName = juce::translate(relatedParameterContainer->getNiceName());
}
void InspectableComponent::setRelatedParameter(ParameterBase *p)
{
    jassert(relatedParameter == nullptr);
    jassert(relatedParameterContainer == nullptr);
    relatedParameter = p;
    visibleName = juce::translate(relatedParameter->niceName);
}
InspectableComponent::~InspectableComponent()
{
    InspectableComponent::masterReference.clear();
}

std::unique_ptr<InspectorEditor> InspectableComponent::createEditor()
{
    if (relatedParameterContainer)
        return std::make_unique<GenericParameterContainerEditor>(relatedParameterContainer);
    else if (relatedParameter)
        return std::make_unique<ComponentInspectorEditor>(new NamedParameterUI(ParameterUIFactory::createDefaultUI(relatedParameter), 100), 25);

    return nullptr;
}

void InspectableComponent::mouseUp(const MouseEvent &me)
{
    if (me.mods.isRightButtonDown())
        return;

    bool canSelect = me.originalComponent == this;
    if (!canSelect)
    {
        if (auto *p = dynamic_cast<InspectableComponent *>(me.originalComponent))
        {
            if (!p->isSelected)
            {
                canSelect = true;
            }
        }
        else
        {
            canSelect = true;
        }
    }
    if (canSelect)
        selectThis();
}

DynamicObject *InspectableComponent::createObject()
{
    if (auto *pc = getRelatedParameterContainer())
    {
        return pc->createObject();
    }
    else if (auto *p = getRelatedParameter())
    {
        return p->createObject();
    }
    jassertfalse; // should override for custom InspectableComponent
    return new DynamicObject();
}

Component *getFirstInspectableContainer(Component *c, int maxDepthCount)
{
    for (auto *cc : c->getChildren())
    {
        if (dynamic_cast<InspectableComponent *>(cc))
        {
            return c;
        }
    }
    if (maxDepthCount > 0)
    {
        for (auto *cc : c->getChildren())
        {
            if (auto ccc = getFirstInspectableContainer(cc, maxDepthCount - 1))
            {
                return ccc;
            }
        }
    }
    return nullptr;
}

bool InspectableComponent::keyPressed(const KeyPress &k)
{
    if (k == KeyPress('a', ModifierKeys::commandModifier, 0))
    {
        if (Component *iC = getFirstInspectableContainer(this, 4))
        {
            Array<WeakReference<InspectableComponent>> toSelect;
            for (auto *c : iC->getChildren())
            {
                if (auto ic = dynamic_cast<InspectableComponent *>(c))
                {
                    toSelect.add(ic);
                }
            }
            if (toSelect.size())
            {
                Inspector::getInstance()->selectComponents(toSelect);
                return true;
            }
        }
    }
    return false;
}

void InspectableComponent::selectThis()
{
    if (Inspector::getInstanceWithoutCreating() == nullptr)
    {
        ShapeShifterManager::getInstance()->showPanelWindowForContent(PanelName::InspectorPanel);
    }

    Inspector::getInstance()->selectOnly(this);
    if (isShowing() && !hasKeyboardFocus(true))
        grabKeyboardFocus();
}

void InspectableComponent::setVisuallySelected(bool value)
{
    if (value == isSelected)
        return;

    isSelected = value;

    if (value)
    {
        if (bringToFrontOnSelect)
            toFront(true);
    }
    else
    {
    }
    repaint();

    setSelectedInternal(value);
}

String InspectableComponent::getTooltip()
{
    if (relatedParameterContainer)
    {
        return juce::translate(relatedParameterContainer->getFactoryInfo());
    }
    else if (relatedParameter)
    {
        return juce::translate(relatedParameter->getFactoryInfo());
    }
    return juce::translate("no info");
}

void InspectableComponent::setSelectedInternal(bool)
{
    //to be overriden
}

void InspectableComponent::paint(Graphics &g)
{
    LGMLUIUtils::fillBackground(this, g);
}
void InspectableComponent::parentHierarchyChanged()
{
    repaint(); //update background
}

void InspectableComponent::paintOverChildren(juce::Graphics &g)
{

    if (isSelected && paintBordersWhenSelected)
    {
        g.setColour(findColour(TextButton::buttonOnColourId));
        g.drawRoundedRectangle(getLocalBounds().toFloat(), 4, 2);
    }
}

ParameterContainer *InspectableComponent::getRelatedParameterContainer()
{
    return relatedParameterContainer;
}
ParameterBase *InspectableComponent::getRelatedParameter()
{
    return relatedParameter;
}

#endif
