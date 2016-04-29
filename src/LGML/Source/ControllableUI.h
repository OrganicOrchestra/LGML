/*
  ==============================================================================

    ControllableUI.h
    Created: 9 Mar 2016 12:02:16pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef CONTROLLABLEUI_H_INCLUDED
#define CONTROLLABLEUI_H_INCLUDED


#include "Controllable.h"
#include "Style.h"

class ControllableUI : public Component, public SettableTooltipClient, public Controllable::Listener
{
public:
    ControllableUI(Controllable * controllable);
    virtual ~ControllableUI();

    String tooltip;

    WeakReference<Controllable>  controllable;

    // Inherited via Listener
    virtual void controllableStateChanged(Controllable * c) override;
    virtual void controllableControlAddressChanged(Controllable * c) override;
    void controllableBeingDeleted(Controllable *) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ControllableUI)

protected :
    void updateTooltip();
};


//    this class allow to automaticly generate label / ui element for parameter listing in editor
//    it owns the created component
class NamedControllableUI : public ControllableUI{
public:
    NamedControllableUI(ControllableUI * ui,int _labelWidth):
    ControllableUI(ui->controllable),
    ownedControllableUI(ui),
    labelWidth(_labelWidth){

        addAndMakeVisible(controllableLabel);

        controllableLabel.setJustificationType(Justification::centredLeft);
        controllableLabel.setColour(Label::ColourIds::textColourId, TEXT_COLOR);
        controllableLabel.setText(ui->controllable->niceName, dontSendNotification);

        addAndMakeVisible(ui);
        setBounds(ownedControllableUI->getBounds()
                  .withTrimmedRight(-labelWidth)
                  .withHeight(jmax((int)controllableLabel.getFont().getHeight() + 4,ownedControllableUI->getHeight())));
    }

    void resized()override{
        Rectangle<int> area  = getLocalBounds();
        controllableLabel.setBounds(area.removeFromLeft(labelWidth));
        ownedControllableUI->setBounds(area);
    }
    Label controllableLabel;
    int labelWidth;
    ScopedPointer <ControllableUI > ownedControllableUI;
};


#endif  // CONTROLLABLEUI_H_INCLUDED
