/*
 ==============================================================================

 ParameterContainer.h
 Created: 5 Sep 2017 3:59:42pm
 Author:  Martin Hermant

 ==============================================================================
 */

#pragma once
#include "../ControllableContainer.h"
// PARAMETER

#include "Parameter.h"

#include "../Parameter/NumericParameter.h"
#include "../Parameter/BoolParameter.h"
#include "../Parameter/StringParameter.h"
#include "../Parameter/EnumParameter.h"
#include "../Parameter/RangeParameter.h"
#include "../Parameter/ParameterList.h"
#include "../Parameter/Trigger.h"
//#include "../Parameter/FileParameter.h" // used sparsely so require explicit include


#include "../../Utils/FactoryObject.h"


class Presetable;

class StringParameter;
class Trigger;

class ParameterContainer: public ControllableContainer,
    public ParameterBase::Listener,
    public ControllableContainer::Listener,
    protected FactoryObject
{
public:
    DECLARE_OBJ_TYPE (ParameterContainer,"Parameter Container")
    virtual ~ParameterContainer();

    template<class T, class... Args>
    T* addNewParameter (const String& _niceName, const String& desc, Args...args);

    template<class T, class... Args>
    T* addNewUniqueParameter (const String& _niceName, const String& desc, Args...args);

    StringParameter* nameParam;
    virtual String getSubTypeName(){return "";} // can be used to distinguish different VSTs Nodes / Js Patches (used in preset)
    String const getNiceName() override;
    String setNiceName (const String& _niceName) override;
    
    bool isHidenInEditor;
    

    virtual ParameterContainer* addContainerFromObject (const String& name, DynamicObject*   data) ;
    virtual ParameterBase* addParameterFromVar (const String& name, const var& data) ;

    virtual void configureFromObject (DynamicObject* data) override;
    virtual DynamicObject* createObject() override;
    DynamicObject* createObjectMaxDepth(int maxDepth) ;
    DynamicObject* createObjectFiltered(std::function<bool(ParameterBase*)> controllableFilter,std::function<bool(ParameterContainer*)> containerFilter,int maxDepth=-1,bool includeUID=false,bool getNotExposed = false);
    

    //  controllableContainer::Listener
    virtual void childControllableRemoved (ControllableContainer*, Controllable*) override;
    



  
    static const Identifier uidIdentifier;


    int getDepthDistanceFromChildContainer(ParameterContainer * from);


    


    void setUserDefined (bool v) override;

    ParameterBase* addParameter ( ParameterBase* ,int idxToSwap=-1,bool doListen=true);
    Array<WeakReference<ParameterBase>> getAllParameters (bool recursive = false, bool getNotExposed = false);
    void clearUserDefinedParameters();
    // Inherited via ParameterBase::Listener
    virtual void parameterValueChanged ( ParameterBase* p, ParameterBase::Listener * notifier=nullptr) override;
    void newMessage (const  ParameterBase::ParamWithValue&)override;

    
    
    BoolParameter* containSavableObjects;

    ParameterContainer * getForUid(const Uuid &ui);
    static ParameterContainer * getForUidGlobal(const Uuid &ui);

    bool canHavePresets() const{return _canHavePresets;}
    bool presetSavingIsRecursive() const{return _presetSavingIsRecursive;}

    friend class PresetManager;
    std::unique_ptr<Presetable> presetable;

    class FeedbackListener{
    public:
        //        void setDepth(int d){depth=d;};
        virtual ~FeedbackListener(){
            while(listenedFBContainers.size()>0){
//                if(auto cc = listenedFBContainers.getLast().get())
//                    cc->removeControllableContainerListener(this);
//                else
                    listenedFBContainers.removeLast();
            }
        }
        virtual void parameterFeedbackUpdate (ParameterContainer*, ParameterBase*,ParameterBase::Listener * notifier) =0;
        Array<WeakReference<ControllableContainer>> listenedFBContainers;
        //        int depth;
    };



    // helper class to inject members
    template<class OwnerClass>
    class OwnedFeedbackListener : public FeedbackListener{
    public:
        OwnedFeedbackListener(OwnerClass * o):owner(o){}
        virtual ~OwnedFeedbackListener(){}
        void parameterFeedbackUpdate (ParameterContainer*, ParameterBase*,ParameterBase::Listener * notifier)override ;
        OwnerClass * owner;

    };
    ListenerList<FeedbackListener> controllableContainerFBListeners;
    ListenerList<FeedbackListener> directControllableContainerFBListeners;
    void addFeedbackListener (FeedbackListener* newListener,bool listenToDirectChild=false) ;
    void removeFeedbackListener (FeedbackListener* listener) ;
private:
    WeakReference< ParameterContainer >::Master masterReference;
    friend class WeakReference<ParameterContainer>;
    


protected:
    void dispatchFeedback (ParameterBase* c, ParameterBase::Listener * notifier);
    void dispatchFeedbackInternal (ParameterBase* c, ParameterBase::Listener * notifier);
    
    bool _canHavePresets;
    bool _presetSavingIsRecursive;

    // internal callback that a controllableContainer can override to react to any of it's parameter change
    // this is to avoid either:
    //      adding controllableContainerListener for each implementation
    //      or overriding parameterValueChanged and needing to call ControllableContainer::parameterValueChanged in implementation (it should stay independent as a different mechanism)
    //
    virtual void onContainerParameterChanged ( ParameterBase*) {}
    virtual void onContainerTriggerTriggered (Trigger*) {}
    virtual void onContainerParameterChangedAsync ( ParameterBase*, const var& /*value*/) {}


};
/// templates



template<class T, class... Args>
T* ParameterContainer::addNewParameter (const String& _niceName, const String& desc, Args...args)
{

    String targetName = getUniqueNameInContainer (_niceName);
    T* p = new T (targetName, desc, args...);
    p->resetValue (true,true);
    return static_cast<T*> (addParameter (p));


}

template<class T, class... Args>
T* ParameterContainer::addNewUniqueParameter (const String& targetName, const String& desc, Args...args)
{

    T* p = new T (targetName, desc, args...);
    p->resetValue (true,true);
    return static_cast<T*> (addParameter (p));

    
}
