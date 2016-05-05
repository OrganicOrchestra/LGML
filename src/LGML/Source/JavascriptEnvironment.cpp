/*
 ==============================================================================

 JavascriptEnvironnement.cpp
 Created: 5 May 2016 9:03:35am
 Author:  Martin Hermant

 ==============================================================================
 */

#include "JavascriptEnvironment.h"
#include "NodeManager.h"
#include "ControlManager.h"
#include "ControllableContainer.h"

JavascriptEnvironment::JavascriptEnvironment(){
    localEnvironment = new DynamicObject();
    localEnvironment->setMethod("post", JavascriptEnvironment::post);
    registerNativeObject("g", localEnvironment);
    
    buildEnvironment();

}

void JavascriptEnvironment::buildEnvironment(){

    //        NodeManager;


    registerNativeObject("node", createDynamicObjectFromContainer(NodeManager::getInstance(),nullptr));
    registerNativeObject("ctl", createDynamicObjectFromContainer(ControllerManager::getInstance(),nullptr));


    //        TimeManager;
    //        ControllerManager;

    //        VSTManager;
    //        PresetManager;
    //        ControlRuleManager;


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
    }
    for(auto &c:container->controllableContainers){
        DynamicObject * childObject = createDynamicObjectFromContainer(c,d);
        if(!c->skipControllableNameInAddress)
            d->setProperty(c->shortName, childObject);


    }

    return d;
}


void JavascriptEnvironment::loadFile(const String & path){
    buildEnvironment();
    File f (path);
    if(f.existsAsFile() && f.getFileExtension() == ".js"){

        StringArray destLines;
        f.readLines(destLines);
        String jsString = destLines.joinIntoString("");

        Result r=execute(jsString);
        if(r.failed()){
            DBG("========Javascript error =================");
            DBG(r.getErrorMessage());
        }
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


void JavascriptEnvironment::loadTest(){
    
    loadFile("/Users/Tintamar/Desktop/tst.js");
}

