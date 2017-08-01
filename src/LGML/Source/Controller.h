/*
 ==============================================================================

 Controller.h
 Created: 2 Mar 2016 8:49:50pm
 Author:  bkupe

 ==============================================================================
 */

#ifndef CONTROLLER_H_INCLUDED
#define CONTROLLER_H_INCLUDED


#include "ControllableContainer.h"


class ControllerUI;
class ControllerEditor;


class Controller :
public ControllableContainer
{
public:
  Controller(const String &name = "[Controller]");
  virtual ~Controller();

  int controllerTypeEnum;

  BoolParameter * enabledParam;

  Trigger * activityTrigger;

  ControllableContainer  userContainer;



  virtual void internalVariableAdded(Parameter * ){};
  virtual void internalVariableRemoved(Parameter * ){};
  virtual void onContainerParameterChanged(Parameter * p) override;
  virtual void onContainerTriggerTriggered(Trigger * ) override;



  virtual ControllerUI * createUI();
  virtual ControllerEditor *  createEditor() ;

  void remove();


  var getJSONData() override;

  // identifiers
  static const Identifier controllerTypeIdentifier;

  

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Controller)
};



#endif  // CONTROLLER_H_INCLUDED
