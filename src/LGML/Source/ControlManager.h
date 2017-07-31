/*
 ==============================================================================

 ControlManager.h
 Created: 2 Mar 2016 8:34:39pm
 Author:  bkupe

 ==============================================================================
 */

#ifndef CONTROLMANAGER_H_INCLUDED
#define CONTROLMANAGER_H_INCLUDED


#include "ControllerFactory.h"

class ControllerManager :  public ControllableContainer
{
public:

  ControllerManager();
  ~ControllerManager();

  juce_DeclareSingleton(ControllerManager, true);

  ControllerFactory factory;
  OwnedArray<Controller> controllers;

  Controller * addController(ControllerFactory::ControllerType controllerType);
  void removeController(Controller * c);
  void clear();


  void addFromVar(var & cData)override;

  class  Listener
  {
  public:
    /** Destructor. */
    virtual ~Listener() {}

    virtual void controllerAdded(Controller *) = 0;
    virtual void controllerRemoved(Controller *) = 0;
  };

  ListenerList<Listener> listeners;
  void addControllerListener(Listener* newListener) { listeners.add(newListener); }
  void removeControllerListener(Listener* listener) { listeners.remove(listener); }





  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ControllerManager)
};






#endif  // CONTROLMANAGER_H_INCLUDED
