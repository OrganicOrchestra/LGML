/*
 ==============================================================================

 TooltipShifter.h
 Created: 3 Nov 2017 10:44:48am
 Author:  Martin Hermant

 ==============================================================================
 */

#pragma once

class TooltipPanel :
public ShapeShifterContentComponent,
private Timer,
private TextEditor::Listener,
private Inspector::InspectorListener
{
public:
    TooltipPanel(const String & n):ShapeShifterContentComponent(n,"See whats under your mouse"),comp(nullptr){

        startTimer(50);


        Inspector::getInstance()->addInspectorListener(this);
        /// user description
        auto labelColor = findColour(Label::ColourIds::backgroundColourId).withAlpha(1.0f).darker();
         descriptionLabel.setColour(Label::ColourIds::backgroundColourId, labelColor);
        descriptionLabel.setText("User Description : ", dontSendNotification);
        addChildComponent(descriptionLabel);

        userDescription.setReadOnly(false);
        userDescription.setMultiLine(true);
        userDescription.setReturnKeyStartsNewLine(true);
        userDescription.setColour(TextEditor::ColourIds::backgroundColourId, findColour(Label::ColourIds::backgroundColourId));
        addChildComponent(userDescription);
        userDescription.setVisible(false);
        userDescription.addListener(this);

        //// general help

        helpLabel.setColour(Label::ColourIds::backgroundColourId, labelColor);
        addAndMakeVisible(helpLabel);


        label.setReadOnly(true);
        label.setMultiLine(true);
        label.setColour(TextEditor::ColourIds::backgroundColourId, findColour(Label::ColourIds::backgroundColourId));
        addAndMakeVisible(label);

    }

    ~TooltipPanel(){
        if(auto i = Inspector::getInstanceWithoutCreating()){
            i->removeInspectorListener(this);
        }
    }

    template<typename T>
    T* getItOrParent(Component * c){
        if(!c) return nullptr;
        if(auto p = dynamic_cast<T*>(c)){ return p;}
        return c->findParentComponentOfClass<T>();
    }

    void timerCallback() override{
        auto& desktop = Desktop::getInstance();
        auto mouseSource = desktop.getMainMouseSource();
        auto* newComp = mouseSource.isTouch() ? nullptr : mouseSource.getComponentUnderMouse();
//        TooltipClient* tc = getItOrParent<TooltipClient>(newComp);
        if( newComp!=comp.get() && newComp!=&label ){


//            if(auto pc = getItOrParent<ConnectableNodeUI>(newComp)){
//                    descriptionParameter = pc->connectableNode->descriptionParam ;
//            }

            comp = newComp;
                updateLabel();



            infoLabel.setVisible(false);
        }
    }

    void updateLabel(){

        if(descriptionParameter.get()){
            userDescription.setText(descriptionParameter->stringValue());
            userDescription.setVisible(true);
            descriptionLabel.setVisible(true);
            auto descCompName = descriptionParameter->parentContainer->getNiceName();
            auto typeName =NodeFactory::getFactoryNiceNameForInstance(dynamic_cast<NodeBase*>(descriptionParameter->parentContainer.get()));
            descriptionLabel.setText(juce::translate("User Description")+" : "+ descCompName+"("+typeName+")", dontSendNotification);
        }
        else{
            userDescription.setVisible(false);
            descriptionLabel.setVisible(false);
        }

        TooltipClient* tc = getItOrParent<TooltipClient>(comp);

        String newTT = tc?tc->getTooltip():String::empty;
        if(newTT.isEmpty()){
            InspectableComponent* ic = getItOrParent<InspectableComponent>(comp);
            newTT=ic?ic->getTooltip():String::empty;
        }
        auto hs  = juce::translate("Help");
        if(newTT.isNotEmpty()){
            if(auto *pui = dynamic_cast<ParameterUI*>(tc)){
                hs+=" : "+pui->parameter->niceName + "("+FactoryBase<Parameter>::getFactoryNiceNameForInstance(pui->parameter.get())+")";
            }
        }
        else{
            //            jassertfalse;
        }
        helpLabel.setText(hs,dontSendNotification);
        label.setText(newTT, dontSendNotification);
        resized();
    }

    void resized()override{
        ShapeShifterContentComponent::resized();
        auto area = getLocalBounds();
        auto labelHeight = descriptionLabel.getFont().getHeight();
        if( userDescription.isVisible() ){
            auto half = area.removeFromTop(area.getHeight()/2);
            descriptionLabel.setBounds(half.removeFromTop(labelHeight));
            userDescription.setBounds( half);
        }
        helpLabel.setBounds(area.removeFromTop(labelHeight));
        label.setBounds(area);
    }

    // user description

     void textEditorTextChanged (TextEditor&) override{
         if(descriptionParameter.get()){
             descriptionParameter->setValue(userDescription.getText());
         }
         else{
             jassertfalse;
         }
    }

    void currentComponentChanged (Inspector* ) override{
        if(auto i = Inspector::getInstanceWithoutCreating()){
            if(auto * curNode = dynamic_cast<ConnectableNode*>(i->getCurrentContainerSelected())){
                descriptionParameter = curNode->descriptionParam;
            }
        }
        updateLabel();
    };

    WeakReference<Component> comp;
    Label descriptionLabel,helpLabel;
    TextEditor label;
    TextEditor userDescription;
    WeakReference<Parameter> descriptionParameter;
};
