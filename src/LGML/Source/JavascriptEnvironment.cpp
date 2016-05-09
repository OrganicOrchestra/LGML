/*
 ==============================================================================

 JavascriptEnvironnement.cpp
 Created: 5 May 2016 9:03:35am
 Author:  Martin Hermant

 ==============================================================================
 */

#include "JavascriptEnvironment.h"
#include "TimeManager.h"
#include "NodeManager.h"
#include "DebugHelpers.h"

juce_ImplementSingleton(JavascriptEnvironment::GlobalEnvironment);


JavascriptEnvironment::JavascriptEnvironment(const String & ns):localNamespace(ns){

    registerNativeObject("g", getGlobalEnv());

    //    // minimal
    linkToControllableContainer("time",TimeManager::getInstance());
    linkToControllableContainer("node",NodeManager::getInstance());

}

JavascriptEnvironment::~JavascriptEnvironment(){
    for(auto & n:linkedContainerNamespaces){
        if(n->container.get()) n->container->removeControllableContainerListener(this);
    }
}

void JavascriptEnvironment::linkToControllableContainer(const String & jsNamespace,ControllableContainer * c){
    c->addControllableContainerListener(this);
    DynamicObject * obj = createDynamicObjectFromContainer(c,nullptr);
    getGlobalEnv()->setProperty(jsNamespace, obj);
    if(!existInContainerNamespace(jsNamespace)){
        linkedContainerNamespaces.add(new JsContainerNamespace(jsNamespace,c,obj)) ;
    }

}

bool JavascriptEnvironment::existInNamespace(const String & name,const String & module ){
    return getGlobalEnv()->getProperty(name).getDynamicObject()->getProperty(module).getDynamicObject() != nullptr;
}

JavascriptEnvironment::JsContainerNamespace* JavascriptEnvironment::getContainerNamespace(ControllableContainer * c){
    JsContainerNamespace* result = nullptr;
    jassert(c!=nullptr);
    for(auto & n:linkedContainerNamespaces){
        ControllableContainer * inspected = c;
        while(inspected!=nullptr){
            if(n->container == inspected){
                return n;
            }
            inspected = inspected->parentContainer;
        }

    }
    return result;
}
JavascriptEnvironment::JsContainerNamespace* JavascriptEnvironment::getContainerNamespace(const String & n){
    JsContainerNamespace* result = nullptr;
    for(auto & n:linkedContainerNamespaces){
        if(n->nsName==n)
            return n;

    }
    return result;
}

bool JavascriptEnvironment::existInContainerNamespace(const String & n){
    return getContainerNamespace(n)!=nullptr;
}
void    JavascriptEnvironment::removeNamespace(const String & jsNamespace){
    if(JsContainerNamespace * c = getContainerNamespace(jsNamespace)){
        linkedContainerNamespaces.removeObject(c);
        // you should'nt call this function for container namespace
        jassertfalse;
    }
    GlobalEnvironment::getInstance()->removeNamespace(jsNamespace);
    registerNativeObject(getModuleName(), nullptr);
}


DynamicObject* JavascriptEnvironment::createDynamicObjectFromContainer(ControllableContainer * container,DynamicObject *parent){
    DynamicObject*  d = parent;
    if(!container->skipControllableNameInAddress)
        d= new DynamicObject();


    for(auto &c:container->controllables){

        if(Parameter * p = dynamic_cast<Parameter*>(c)){
            DynamicObject* dd= new DynamicObject();

            dd->setProperty("get", p->value);
            dd->setProperty("_ptr", (int64)p);
            dd->setMethod("set", JavascriptEnvironment::set);
            d->setProperty(p->shortName, dd);


        }
        else if(Trigger * t = dynamic_cast<Trigger*>(c)){
            DynamicObject* dd= new DynamicObject();
            dd->setMethod("t", JavascriptEnvironment::set);
            dd->setProperty("_ptr", (int64)t);
            d->setProperty(t->shortName, dd);

        }
    }
    for(auto &c:container->controllableContainers){
        DynamicObject * childObject = createDynamicObjectFromContainer(c,d);
        if(!c->skipControllableNameInAddress)
            d->setProperty(c->shortName, childObject);


    }

    return d;
}


void JavascriptEnvironment::loadFile(const File &f){


    if(f.existsAsFile() && f.getFileExtension() == ".js"){

        internalLoadFile(f);
    }

}

void JavascriptEnvironment::reloadFile(){
    if(!currentFile.existsAsFile())return;
    internalLoadFile(currentFile);
}

void JavascriptEnvironment::showFile(){
    if(!currentFile.existsAsFile())return;
    currentFile.startAsProcess();
}



void JavascriptEnvironment::internalLoadFile(const File &f ){
    StringArray destLines;
    f.readLines(destLines);
    String jsString = destLines.joinIntoString("\n");
    currentFile = f;
    removeNamespace(localNamespace);
    buildLocalNamespace();
    Result r=execute(jsString);
    if(r.failed()){
        hasValidJsFile = false;
        LOG("========Javascript error =================\n"+r.getErrorMessage());
    }
    else{
        hasValidJsFile = true;
        LOG("script Loaded successfully : "+f.getFullPathName());
    }
    newJsFileLoaded();
}

void JavascriptEnvironment::post(const String & s){
    LOG(s);
}

var JavascriptEnvironment::post(const NativeFunctionArgs& a){
    DBG("posting : "+a.thisObject.toString());
    for(int i = 0 ; i < a.numArguments ;i++){
        post(a.arguments[i].toString());
    }
    return var();

}

void JavascriptEnvironment::addToNamespace(const String & elemName,DynamicObject *target,DynamicObject * global){
    jassert(target!=nullptr);
    jassert(elemName!="");
    int idx = elemName.indexOfChar('.');

    bool lastElem =(idx==-1);

    if(!lastElem){
        String elem = elemName.substring(0,idx);
        DynamicObject * dd = getGlobalEnv()->getProperty(elem).getDynamicObject();

        if(dd==nullptr)
        {
            dd= new DynamicObject();
            getGlobalEnv()->setProperty(elem, dd);
        }

        addToNamespace(elemName.substring(idx+1, elemName.length()), target,dd);
    }
    else{

        global->setProperty(elemName, target);
        registerNativeObject(elemName, target);
    }


}

void JavascriptEnvironment::addToLocalNamespace(const String & elem,DynamicObject *target){
    if(elem!="")
        addToNamespace(localNamespace+"."+elem, target,getGlobalEnv());
    else
        addToNamespace(localNamespace, target,getGlobalEnv());
}

void JavascriptEnvironment::removeFromNamespace(const String & name,const String & elemName){
    DynamicObject * dd = getGlobalEnv()->getProperty(name).getDynamicObject();
    if(dd!=nullptr) dd->removeProperty(elemName);
}

void JavascriptEnvironment::setNameSpaceName(const String & s){
    DynamicObject * d = GlobalEnvironment::getInstance()->getNamespaceObject(localNamespace);
    if(d){
        DynamicObject * clone = new DynamicObject(*d);
        removeNamespace(localNamespace);
        localNamespace = s;
        addToNamespace(localNamespace, clone);
    }

}
var JavascriptEnvironment::set(const NativeFunctionArgs& a){

    DynamicObject * d = a.thisObject.getDynamicObject();
    Controllable * c = dynamic_cast<Controllable*>((Controllable*)(int64)d->getProperty("_ptr"));
    bool success = false;

    if (c != nullptr)
    {

        success = true;

        if(a.numArguments==0 && c->type==Controllable::Type::TRIGGER){
            ((Trigger *)c)->trigger();
        }

        else{
            var value = a.arguments[0];
            switch (c->type)
            {
                case Controllable::Type::TRIGGER:
                    if (value.isBool() && (bool)value)
                        ((Trigger *)c)->trigger();
                    else if((value.isDouble()||value.isInt() || value.isInt64())&& (float)value>0)
                        ((Trigger *)c)->trigger();

                    break;

                case Controllable::Type::BOOL:
                    if ( value.isBool())
                        ((BoolParameter *)c)->setValue((bool)value);
                    break;

                case Controllable::Type::FLOAT:
                    if(value.isDouble()||value.isInt() || value.isInt64())
                        ((FloatParameter *)c)->setValue(value);
                    break;
                case Controllable::Type::INT:
                    if(value.isInt() || value.isInt64())
                        ((IntParameter *)c)->setValue(value);
                    break;


                case Controllable::Type::STRING:
                    if(value.isString())
                        ((StringParameter *)c)->setValue(value);
                    break;

                default:
                    success = false;
                    break;

            }
        }
    }


    return var();
}


void JavascriptEnvironment::childStructureChanged(ControllableContainer * c){
    if(JsContainerNamespace * ns= getContainerNamespace(c)){
        registerNativeObject(ns->nsName, createDynamicObjectFromContainer(ns->container, nullptr));
    }
}


String JavascriptEnvironment::printAllNamespace(){
    return namespaceToString(getRootObjectProperties());

}
String JavascriptEnvironment::namespaceToString(const NamedValueSet & v,int indentlevel ,bool showValue){
    String res;
    res+=" (";
    for(int i = 0 ; i < v.size() ; i++){
        var * vv = v.getVarPointerAt(i);
        if(!vv->isObject()){
            String name = v.getName(i).toString();
            res+= (i!=0?", ":"")+name + (showValue?'('+ vv->toString()+")":"") ;
        }
    }
    res+=+ ")\n";

    for(int i = 0 ; i < v.size() ; i++){
        var * vv = v.getVarPointerAt(i);
        String name = v.getName(i).toString();


        if(vv->isObject()){
            for(int  j = 0 ; j < indentlevel ; j ++ ){
                res+='-';
            }

            DynamicObject * d = vv->getDynamicObject();
            if(d!=nullptr){
                res+= name+":";
                res+=namespaceToString(d->getProperties(),indentlevel+1,showValue);}
        }
        
    }
    
    return res;
    
}




String JavascriptEnvironment::getModuleName(){
    
    int idx = localNamespace.indexOfChar('.');
    if(idx==-1){
        return  localNamespace;
    }
    else{
        return localNamespace.substring(idx+1);
    }
    
}