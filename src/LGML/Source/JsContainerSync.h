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
#include "JuceHeader.h"
//allow to keep structural changes from containers with js namespace
class JsContainerSync: public ControllableContainerListener
{

public:

    virtual ~JsContainerSync(){
        for(auto & n:linkedContainerNamespaces){
            if(n->container.get()) n->container->removeControllableContainerListener(this);
        }
    }

    // should return current env
    virtual DynamicObject * getEnv() = 0;
    
    void    linkToControllableContainer(const String & jsNamespace,ControllableContainer * c);

    static  DynamicObject *  createDynamicObjectFromContainer(ControllableContainer * c,DynamicObject * parent);

    bool existInContainerNamespace(const String &);


    class JsContainerNamespace{
    public:
        JsContainerNamespace(const String & n,ControllableContainer * c,DynamicObject * o):nsName (n),container(c),jsObject(o){}
        WeakReference<ControllableContainer> container;
        DynamicObject::Ptr jsObject;
        String nsName;

    };
    JsContainerNamespace* getContainerNamespace(ControllableContainer *);
    JsContainerNamespace* getContainerNamespace(const String & );

    void childStructureChanged(ControllableContainer * )override;

private:



    static var setControllable(const juce::var::NativeFunctionArgs& a);
    OwnedArray<JsContainerNamespace>  linkedContainerNamespaces;
    
};





#endif  // JSCONTAINERSYNC_H_INCLUDED
