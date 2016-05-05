/*
 ==============================================================================

 JavascriptEnvironnement.cpp
 Created: 5 May 2016 9:03:35am
 Author:  Martin Hermant

 ==============================================================================
 */

#include "JavascriptEnvironment.h"
#include "TimeManager.h"

juce_ImplementSingleton(JavascriptEnvironment);

JavascriptEnvironment::JavascriptEnvironment(){
    localEnvironment = new DynamicObject();
    localEnvironment->setMethod("post", JavascriptEnvironment::post);
    registerNativeObject("g", localEnvironment);

    linkToControllableContainer("time",TimeManager::getInstance());


}


void JavascriptEnvironment::linkToControllableContainer(const String & jsNamespace,ControllableContainer * c){
    c->addControllableContainerListener(this);
    JsNamespace * n = new JsNamespace(jsNamespace,c,createDynamicObjectFromContainer(c,nullptr));
    registerNativeObject(jsNamespace, n->jsObject);
    linkedNamespaces.add(n );
}


void    JavascriptEnvironment::removeNamespace(const String & jsNamespace){
    int idx = 0;
    for(auto & n:linkedNamespaces){
        if(jsNamespace == n->name){
            linkedNamespaces.remove(idx);
            registerNativeObject(jsNamespace, nullptr);
            break;
        }
        idx++;
    }
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


void JavascriptEnvironment::loadFile(const String & path){

    File f (path);
    if(f.existsAsFile() && f.getFileExtension() == ".js"){
        loadedFiles.addIfNotAlreadyThere(path);
        internalLoadFile(f);
    }

}

void JavascriptEnvironment::rebuildAllNamespaces(){
    for(auto & path:loadedFiles){
        File f (path);
        if(f.existsAsFile() && f.getFileExtension() == ".js"){
            internalLoadFile(f);
        }
    }
}

void JavascriptEnvironment::internalLoadFile(const File &f ){
    StringArray destLines;
    f.readLines(destLines);
    String jsString = destLines.joinIntoString("");

    Result r=execute(jsString);
    if(r.failed()){
        DBG("========Javascript error =================");
        DBG(r.getErrorMessage());
    }
}

var JavascriptEnvironment::post(const NativeFunctionArgs& a){
    DBG("posting : "+a.thisObject.toString());
    for(int i = 0 ; i < a.numArguments ;i++){
        DBG(a.arguments[i].toString());
    }
    return var();

}

var JavascriptEnvironment::set(const NativeFunctionArgs& a){
    //    if(a.numArguments<1)jassertfalse;

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


void JavascriptEnvironment::structureChanged(ControllableContainer * c){
    ControllableContainer * inspected = c;
    bool found = false;
    while(inspected!=nullptr && !found){
        for(auto & n:linkedNamespaces){
            if(n->container == inspected){
                registerNativeObject(n->name, createDynamicObjectFromContainer(c, nullptr));
                found = true;
                break;
            }
        }
        inspected = inspected->parentContainer;
    }
}



