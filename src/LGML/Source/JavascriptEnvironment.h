/*
 ==============================================================================

 JavascriptEnvironnement.h
 Created: 5 May 2016 9:03:35am
 Author:  Martin Hermant

 ==============================================================================
 */

#ifndef JAVASCRIPTENVIRONNEMENT_H_INCLUDED
#define JAVASCRIPTENVIRONNEMENT_H_INCLUDED

#include "JuceHeader.h"
#include "ControllableContainer.h"
class JavascriptEnvironment : public JavascriptEngine,public ControllableContainer::Listener{
public:
    JavascriptEnvironment(const String & ns);
    ~JavascriptEnvironment();



    typedef juce::var::NativeFunctionArgs NativeFunctionArgs;


    static  DynamicObject *  createDynamicObjectFromContainer(ControllableContainer * c,DynamicObject * parent);
    void    linkToControllableContainer(const String & jsNamespace,ControllableContainer * c);
    void    addToNamespace(const String & elemName,DynamicObject *target,DynamicObject * global = getGlobalEnv());
    void    addToLocalNamespace(const String & elem,DynamicObject *target);

    void setNameSpaceName(const String &);
    void removeFromNamespace(const String & name,const String & elemName);
    void    removeNamespace(const String & jsNamespace);
    void    loadFile(const File & f);
    void    reloadFile();
    void showFile();


    class JsContainerNamespace;
    bool existInNamespace(const String & name,const String & module );
    JsContainerNamespace* getContainerNamespace(ControllableContainer *);
    JsContainerNamespace* getContainerNamespace(const String & );
    bool existInContainerNamespace(const String &);

    //////////////////
    // helperclasses

    class JsContainerNamespace{
    public:
        JsContainerNamespace(const String & n,ControllableContainer * c,DynamicObject * o):nsName (n),container(c),jsObject(o){}
        WeakReference<ControllableContainer> container;
        DynamicObject::Ptr jsObject;
        String nsName;

    };


    class OwnedJsArgs {


    public:
        OwnedJsArgs(var _owner):owner(_owner){}
        void addArg(float f){ownedArgs.add(new var(f));}
        void addArg(String f){ownedArgs.add(new var(f));}
        void addArgs(const StringArray & a){for(auto & s:a){addArg(s.getFloatValue());}}

        NativeFunctionArgs getNativeArgs(){
            return NativeFunctionArgs(owner,ownedArgs.getFirst(),ownedArgs.size());
        }
        NativeFunctionArgs getLocalSopeNativeArgs(){
            return NativeFunctionArgs(nullptr,ownedArgs.getFirst(),ownedArgs.size());
        }
    private:
        var owner;
        OwnedArray<var> ownedArgs;
    };


    static DynamicObject * getGlobalEnv(){return GlobalEnvironment::getInstance()->getEnv();}

    String printAllNamespace();





    class GlobalEnvironment{
    public:
        juce_DeclareSingleton(GlobalEnvironment, true);

        GlobalEnvironment(){
            env = new DynamicObject();
            getEnv()->setMethod("post", JavascriptEnvironment::post);

        }

        void removeNamespace(const String & ns){
            removeNamespace(ns,getEnv());
        }
        void removeNamespace(const String & ns,DynamicObject * d){
            if(d==nullptr){
                DBG("js Ns removing failed : not found "+ns);
            }
            int idx = ns.indexOfChar('.');
            bool lastElem =(idx==-1);
            if(!lastElem){
                String elem = ns.substring(0,idx);
                return removeNamespace(ns.substring(idx+1, ns.length()),d->getProperty(elem).getDynamicObject());
            }
            else{
                d->removeProperty(ns);
            }
        }

        DynamicObject * getNamespaceObject(const String & ns){
            return getNamespaceObject(ns,getEnv());
        }

        DynamicObject *  getNamespaceObject(const String & ns,DynamicObject * d ){
            if(d==nullptr)return nullptr;
            int idx = ns.indexOfChar('.');

            bool lastElem =(idx==-1);

            if(!lastElem){
                String elem = ns.substring(0,idx);
                return getNamespaceObject(ns.substring(idx+1, ns.length()),d->getProperty(elem).getDynamicObject());
            }
            else{
                return d->getProperty(ns).getDynamicObject();
            }
        }

        DynamicObject * getEnv(){return env.getDynamicObject();}

    private:


        var env;
    };

    protected :
    String localNamespace;




    String namespaceToString(const NamedValueSet & v,int indentLevel = 0);

    String getModuleName();
    File currentFile;


private:

    OwnedArray<JsContainerNamespace>  linkedContainerNamespaces;




    static void  post(const String & s);
    void internalLoadFile(const File &);
    void childStructureChanged(ControllableContainer * )override;
    
    static var post(const NativeFunctionArgs& a);
    static var set(const NativeFunctionArgs& a);
    
};




#endif  // JAVASCRIPTENVIRONNEMENT_H_INCLUDED
