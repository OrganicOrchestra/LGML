/*
 ==============================================================================

 JsContainerSync.h
 Created: 9 May 2016 6:21:50pm
 Author:  Martin Hermant

 ==============================================================================
 */

#ifndef JSCONTAINERSYNC_H_INCLUDED
#define JSCONTAINERSYNC_H_INCLUDED

#include "ControllableContainer.h"
//allow to keep structural changes from containers with js namespace
class JsContainerSync: private ControllableContainerListener
{

public:

  JsContainerSync():aggregChanges(this){};
  virtual ~JsContainerSync();


  // should return current env
  virtual DynamicObject::Ptr getEnv() = 0;

  void    linkToControllableContainer(const String & jsNamespace,ControllableContainer * c);
  bool existInContainerNamespace(const String &);


  bool isDirty();


protected :

  void childStructureChanged(ControllableContainer *,ControllableContainer * )override;
  void childAddressChanged(ControllableContainer * c) override;

  void updateControllableNamespace(ControllableContainer * c);


private:

  void removeAllListeners(ControllableContainer * c);

  class JsContainerNamespace{
  public:
    JsContainerNamespace(const String & n,ControllableContainer * c,DynamicObject * o):nsName (n),container(c),jsObject(o){}
    WeakReference<ControllableContainer> container;
    DynamicObject::Ptr jsObject;
    String nsName;

  };
  JsContainerNamespace* getContainerNamespace(ControllableContainer *);
  JsContainerNamespace* getContainerNamespace(const String & );


  DynamicObject *  createDynamicObjectFromContainer(ControllableContainer * c,DynamicObject * parent);

  class AggregChanges : private Timer{
  public:
    AggregChanges(JsContainerSync * _owner):owner(_owner){
      startTimer(1000);
    }

    void timerCallback() override{
      if(nsToUpdate.size()){
        Array<JsContainerNamespace * > processed;
        for(auto & ns:nsToUpdate){
          if(!processed.contains(ns)){
            owner->getEnv()->setProperty(ns->nsName, owner->createDynamicObjectFromContainer(ns->container, nullptr));
            processed.add(ns);
          }
        }
        nsToUpdate.clear();
      }
    }
    void addNs(JsContainerNamespace * ns){
      if(ns==nullptr){
        return;
      }
      nsToUpdate.addIfNotAlreadyThere(ns);
    }
    Array<JsContainerNamespace * > nsToUpdate;
    JsContainerSync * owner;
  };

  AggregChanges aggregChanges;

  OwnedArray<JsContainerNamespace>  linkedContainerNamespaces;
  
};





#endif  // JSCONTAINERSYNC_H_INCLUDED
