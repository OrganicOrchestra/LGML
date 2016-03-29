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

class ControllableUI : public Component, public SettableTooltipClient, public Controllable::Listener
{
public:
    ControllableUI(Controllable * controllable);
    virtual ~ControllableUI();

    String tooltip;

    Controllable * controllable;

    // Inherited via Listener
    virtual void controllableStateChanged(Controllable * c) override;
    virtual void controllableControlAddressChanged(Controllable * c) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ControllableUI)

protected :
    void updateTooltip();
};



#endif  // CONTROLLABLEUI_H_INCLUDED
