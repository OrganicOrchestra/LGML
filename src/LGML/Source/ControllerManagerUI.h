/*
  ==============================================================================

    ControllerManagerUI.h
    Created: 8 Mar 2016 10:13:00pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef CONTROLLERMANAGERUI_H_INCLUDED
#define CONTROLLERMANAGERUI_H_INCLUDED


#include "ControlManager.h"
#include "ControllerUI.h"

#include "ShapeShifterContent.h"
//==============================================================================
/*
*/

class ControllerManagerUI : public Component,ControllerManager::Listener
{
public:
    ControllerManagerUI(ControllerManager * manager);
    ~ControllerManagerUI();

    ControllerManager * manager;

    OwnedArray<ControllerUI> controllersUI;

    ControllerUI * addControllerUI(Controller * controller);
    void removeControllerUI(Controller * controller);

    ControllerUI * getUIForController(Controller * controller);

    void paint (Graphics&)override;
    void resized()override;

    void mouseDown(const MouseEvent &e) override;

  int getContentHeight();

	void clear();
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ControllerManagerUI)

    // Inherited via Listener
    void controllerAdded(Controller *) override;
    void controllerRemoved(Controller *) override;
    void notifyParentViewPort(){if(auto * p = getParentComponent()) p->resized();}
};


class ControllerManagerUIViewport :
public ShapeShifterContentComponent
{
public:
  ControllerManagerUIViewport(const String &contentName, ControllerManagerUI * _UI) :
		controllerManagerUI(_UI),
		ShapeShifterContentComponent(contentName)
  {
    vp.setViewedComponent(controllerManagerUI, true);
    vp.setScrollBarsShown(true, false);
    vp.setScrollOnDragEnabled(false);
    addAndMakeVisible(vp);
    vp.setScrollBarThickness(10);


  }

  virtual ~ControllerManagerUIViewport()
  {

  }

  void resized() override {
    vp.setBounds(getLocalBounds());
    int th = jmax<int>(controllerManagerUI->getContentHeight(), getHeight());
    Rectangle<int> targetBounds = getLocalBounds().withPosition(controllerManagerUI->getPosition()).withHeight(th);
    targetBounds.removeFromRight(vp.getScrollBarThickness());
    controllerManagerUI->setBounds(targetBounds);
  }


  Viewport vp;
  ControllerManagerUI * controllerManagerUI;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ControllerManagerUIViewport)
};
#endif  // CONTROLLERMANAGERUI_H_INCLUDED
