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

#include "ParameterUI.h"

#include "../ParameterFactory.h"
#include "../../../Utils/DebugHelpers.h"
#include "../../../UI/Style.h"
#include "../../../UI/LGMLDragger.h"
#include "../../../FastMapper/FastMapper.h"
#include "../UndoableHelper.h"
#include "../../../UI/Inspector/Inspector.h"
#include "ParameterUIHelpers.h" 



//==============================================================================

//-------------------
ParameterUI::ParameterUI ( ParameterBase* _parameter) :
    InspectableComponent(_parameter,"ParameterUI"),
    parameter (_parameter),
    showLabel (true),
    showValue (true),
    customTextDisplayed (""),
    isMappingDest (false),
    isDraggable (true),
    wasShowing(true)
,defferTimer  (new DefferTimer(this))
{

    LGMLUIUtils::optionallySetBufferedToImage(this);
    setPaintingIsUnclipped(true);
    setOpaque(true);
    AllParamType::getAllParameterUIs().add(this);
    if (parameter.get())
    {

        parameter->addAsyncCoalescedListener (this);
        parameter->addParameterListener (this);
        controllableStateChanged (parameter.get());
        
    }
    else
    {
        jassertfalse;
    }

    hasValidControllable = (parameter.get() != nullptr);
    jassert (hasValidControllable);
    setName (parameter->niceName);
    parameter->addControllableListener (this);
    mappingState = NOMAP;
    LGMLDragger::getInstance()->applyMappingState (this);



}

ParameterUI::~ParameterUI()
{
    AllParamType::getAllParameterUIs().removeAllInstancesOf(this);
    if(auto * draggerI = LGMLDragger::getInstanceWithoutCreating()){
        draggerI->unRegisterDragCandidate (this);
    }

    if (parameter.get())
    {
        parameter->removeControllableListener (this);
        parameter->removeParameterListener (this);
        parameter->removeAsyncParameterListener (this);
    }

    ParameterUI::masterReference.clear();
}




void ParameterUI::setCustomText (const String text)
{
    String newText =juce::translate(text);
    if(newText!=customTextDisplayed){
        customTextDisplayed =newText;
        displayedTextChangedInternal();
        paramUIListeners.call(&ParameterUI::Listener::displayedTextChanged,this);
        repaint();
    }
}

String  ParameterUI::getDisplayedText () const{
    return customTextDisplayed.isNotEmpty() ? customTextDisplayed : parameter.get()?parameter->niceName:"No Parameter";
}
const ParameterUI::UICommandType & ParameterUI::getUICommands() const{
    static UICommandType dummy;
    return dummy;
}
void ParameterUI::mouseDown (const MouseEvent& e)
{
    UndoableHelpers::startNewTransaction(parameter,true);
    if (e.mods.isRightButtonDown())
    {
        PopupMenu p;
        p.addItem (1, juce::translate("Select Parameter (Alt+click)"));
        p.addItem (2, juce::translate("Copy control address"));
        p.addItem (5, juce::translate("Copy control value"));
        p.addItem (3, juce::translate("Add FastMap To"));
        p.addItem (4, juce::translate("Add FastMap From"));
        Array<Identifier> typeSwitch;
        if(parameter->isUserDefined){
            typeSwitch = ParameterFactory::getCompatibleTypes(parameter);
            if(typeSwitch.size()){
                PopupMenu tp;
                int i = 50;
                for( auto & t : typeSwitch){
                    tp.addItem(i,ParameterFactory::typeToNiceName(t.toString()));
                    i++;
                }
                p.addSubMenu(juce::translate("change parameter type"), tp);
            }

        }
        const UICommandType & cmds(getUICommands());
        if(cmds.size()){
        p.addSeparator();
            auto it = UICommandType::Iterator(cmds);
            while (it.next()){
                p.addItem (it.getKey()+100, juce::translate(it.getValue()));
            }
        }
        bool oldFocus = getWantsKeyboardFocus();
        setWantsKeyboardFocus(false);
        int result = p.show();
        setWantsKeyboardFocus(oldFocus);

        switch (result)
        {
            case 1:
                Inspector::getInstance()->selectOnly(this);
                break;

            case 2:
                SystemClipboard::copyTextToClipboard (parameter->controlAddress.toString());
                break;

            case 3:
                FastMapper::getInstance()->addFastMap()->referenceOut->setParamToReferTo ( ParameterBase::fromControllable (parameter));
                break;

            case 4:
                FastMapper::getInstance()->addFastMap()->referenceIn->setParamToReferTo ( ParameterBase::fromControllable (parameter));
                break;
            case 5:
                SystemClipboard::copyTextToClipboard (parameter->value.toString());
            default:
                if(result>=100){
                    processUICommand(result-100);
                }
                else if(result>=50){
                    ParameterContainer * parentC = dynamic_cast<ParameterContainer *>(parameter->parentContainer.get());


                    if(parentC){
                        int oriIdx = parentC->controllables.indexOf(parameter);
                        auto niceName = parameter->niceName;
                        var obj (parameter->createObject());
                        parentC->removeControllable(parameter);
                        auto np = ParameterFactory::createFromTypeID(typeSwitch.getUnchecked(result-50),niceName,nullptr);
                        parentC->addParameter(np,oriIdx,true);
                        np->configureFromObject(obj.getDynamicObject());
                    }
                    else{
                        jassertfalse;
                    }
                }
                break;

        }
    }
    if(e.mods.isAltDown()){
        Inspector::getInstance()->selectOnly(this);
    }

}
void ParameterUI::mouseUp (const MouseEvent& ) {

};

bool ParameterUI::shouldBailOut()
{
    bool bailOut = parameter.get() == nullptr;

    // we want a clean deletion no?
    if (bailOut)
    {
        // TODO : changing vst preset sometimes hit that
        NLOGW("ParameterUI", "old component still displayed : " << getName());
        //jassertfalse;
    }

    return bailOut;

}



void ParameterUI::controllableStateChanged (Controllable* c)
{
    setAlpha (c->enabled ? 1 : .5f);
}

void ParameterUI::controllableControlAddressChanged (Controllable*)
{   
//repaint();
}

void ParameterUI::controllableNameChanged (Controllable*) {
    if(customTextDisplayed.isEmpty()){
        paramUIListeners.call(&ParameterUI::Listener::displayedTextChanged,this);
    }
    repaint();
}


String ParameterUI::getTooltip(){
    if(parameter.get())
        return juce::translate(parameter->description) + "\n"+juce::translate("Control Address")+" : " + parameter->controlAddress.toString();//"\nValue : "+parameter->value.toString();
    return "parameter is now deleted";
}

void ParameterUI::visibilityChanged(){
    bool _isShowing = isShowing();
    // do nothing if already in appropriate state
    if(_isShowing==wasShowing) return;
    // do nothing if detached
    if(getParentComponent()==nullptr) return;
    if (parameter.get()){
        if(_isShowing){
            parameter->addAsyncCoalescedListener (this);
            parameter->addParameterListener (this);
            parameter->addControllableListener (this);
            // don't trigger
            if(!dynamic_cast<Trigger*>(parameter.get()))
                if(!parameter->checkValueIsTheSame(lastValuePainted,parameter->value))
                    valueChanged(parameter->value);
        }
        else{
            parameter->removeAsyncParameterListener (this);
            parameter->removeParameterListener (this);
            parameter->removeControllableListener (this);
            lastValuePainted = parameter->value;
        }
    }
    if (auto ld = LGMLDragger::getInstanceWithoutCreating())
        ld->applyMappingState(this);
    wasShowing =_isShowing;

}

void ParameterUI::paint(Graphics & g){
    LGMLUIUtils::fillBackground(this,g);
}
void ParameterUI::parentHierarchyChanged(){
    visibilityChanged();
    InspectableComponent::parentHierarchyChanged();
};

void ParameterUI::setHasMappedParameter(bool s){

    hasMappedParameter = s;
    updateOverlayEffect();
}

class MapEffect : public ImageEffectFilter
{
public:
    MapEffect (const Colour& colour, uint32 _amount, String _text,bool _isMapped):
        amount (_amount),
        pRef (colour.getAlpha(), colour.getRed(), colour.getGreen(), colour.getBlue()),
        text (_text),
        isMapped(_isMapped)
    {
    }
    PixelARGB pRef;
    uint32 amount;
    String text;
    uint32 trueAmount;
    bool isMapped;

    template<typename T>
    void  applyFunction (Image::BitmapData& data)
    {
        for (int y = 0; y < data.height; ++y)
        {
            uint8* p = data.getLinePointer (y);

            for (int x = 0; x < data.width; ++x)
            {
                T* pp = ((T*) p);

                pp->desaturate();
                pp->tween (pRef, trueAmount);

                p += data.pixelStride;
            }
        }
    }
    void applyEffect (Image& image, Graphics& g, float /*scaleFactor*/, float alpha)
    {
        //  Image temp (image.getFormat(), image.getWidth(), image.getHeight(), true);
        trueAmount = (uint32) (alpha * alpha * amount);

        Image::BitmapData data (image, 0, 0, image.getWidth(), image.getHeight(), Image::BitmapData::readWrite);

        if (image.getFormat() == Image::PixelFormat::ARGB)
        {
            applyFunction<PixelARGB> (data);
        }
        else if (image.getFormat() == Image::PixelFormat::RGB)
        {
            applyFunction<PixelRGB> (data);
        }
        else
        {
            jassertfalse;
        }


        g.drawImage (image, image.getBounds().toFloat());
        if(isMapped){
            auto r = image.getBounds();
            g.setColour(Colours::red);
            int side = 8;
            g.fillRect(r.removeFromBottom(side).removeFromRight(side));
        }
//        g.setColour (Colours::white);
//        g.drawFittedText (text, 0, 0, image.getWidth(), image.getHeight(), Justification::centred, 2);

    }
};



void  ParameterUI::setMappingState (const bool  b)
{
    if ( parameter && !parameter->isMappable())return;

    MappingState s = b ? (isMappingDest ? MAPDEST : MAPSOURCE) : NOMAP;

    if (s != mappingState)
    {
        if (s == NOMAP)
        {
            setInterceptsMouseClicks (true, true);

        }
        else
        {
            setInterceptsMouseClicks (true, false);
        }
    }
    

    mappingState = s;
    updateOverlayEffect();

}

void ParameterUI::updateOverlayEffect(){
    if (mappingState!=NOMAP)
    {
        Colour c =isMappingDest ? Colours::red : Colours::blue;
        if (isSelected) c = Colours::green;
        mapEffect  = std::make_unique<MapEffect> (c, isSelected? 100:50, getName(),FastMapper::getInstance()->isParameterMapped(parameter));
    }
    else
    {
        mapEffect = nullptr;
    }

    setComponentEffect (mapEffect.get());
    repaint();

}

void ParameterUI::setMappingDest (bool _isMappingDest)
{
    isMappingDest = _isMappingDest;

    if (mappingState != NOMAP)
    {
        mappingState = isMappingDest ? MAPDEST : MAPSOURCE;
    }

}

void ParameterUI::newMessage (const ParameterBase::ParamWithValue& p)
{
    if (p.isRange())
    {
        rangeChanged (p.parameter);
    }
    else
    {   defferTimer->trigger(p.value);
//        valueChanged (p.value);
    }
};




//////////////////
// NamedParameterUI


NamedParameterUI::NamedParameterUI (std::unique_ptr<ParameterUI>  ui_, int _labelWidth, bool labelA):
    ParameterUI (ui_->parameter)
,ownedParameterUI (std::move(ui_))
    ,labelWidth (_labelWidth)
    ,labelAbove (labelA)
    ,controllableLabel(new LabelLinkedTooltip(ownedParameterUI.get()))
{
    ownedParameterUI->showLabel = false;
    // prevent mapping state for named parameterUI -> inner will handle it
    setMappingState(false);
    setPaintingIsUnclipped(true);
    addAndMakeVisible (controllableLabel.get());


    controllableLabel->setText (ownedParameterUI->visibleName, dontSendNotification);

    if (ownedParameterUI->parameter->isUserDefined)
    {
        controllableLabel->setEditable (true);
        controllableLabel->addListener (this);
    }
    LGMLUIUtils::optionallySetBufferedToImage(controllableLabel.get());

    addAndMakeVisible (ownedParameterUI.get());
    ownedParameterUI->toFront (false);
//    setBounds (ownedParameterUI->getBounds()
//               .withTrimmedRight (-labelWidth)
//               .withHeight (jmax ((int)controllableLabel.getFont().getHeight() + 4, ownedParameterUI->getHeight())));
}

void NamedParameterUI::resized()
{
    Rectangle<int> area  = getLocalBounds();

    if (controllableLabel->getText().isNotEmpty())
    {
        if (labelAbove)
        {
            controllableLabel->setBounds (area.removeFromTop (jmin (18, area.getHeight() / 2)));
        }
        else
        {
            controllableLabel->setBounds (area.removeFromLeft (labelWidth));
            area.removeFromLeft (10);
        }
    }

    ownedParameterUI->setBounds (area);
}

void NamedParameterUI::labelTextChanged (Label* labelThatHasChanged)
{
    if (ownedParameterUI.get())
    {
        ownedParameterUI->parameter->setNiceName (labelThatHasChanged->getText());
    }
};

void  NamedParameterUI::controllableControlAddressChanged (Controllable* c){
    if(c && c==parameter){
    controllableLabel->setText (juce::translate(parameter->niceName), dontSendNotification);
    }
}

#endif
