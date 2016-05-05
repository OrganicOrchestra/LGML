/*
 ==============================================================================

 JavascriptEnvironnement.cpp
 Created: 5 May 2016 9:03:35am
 Author:  Martin Hermant

 ==============================================================================
 */

#include "JavascriptEnvironment.h"
#include "NodeManager.h"
#include "ControllableContainer.h"

JavascriptEnvironment::JavascriptEnvironment(){
    localEnvironment = new DynamicObject();


}

void JavascriptEnvironment::buildEnvironment(){

    //        NodeManager;


    localEnvironment->setProperty("node", createDynamicObjectFromContainer(NodeManager::getInstance()));

    localEnvironment->setMethod("post", JavascriptEnvironment::post);
    localEnvironment->setMethod("set", JavascriptEnvironment::set);
    //        TimeManager;
    //        ControllerManager;

    //        VSTManager;
    //        PresetManager;
    //        ControlRuleManager;

    registerNativeObject("g", localEnvironment);
}


DynamicObject* JavascriptEnvironment::createDynamicObjectFromContainer(ControllableContainer * container){

    DynamicObject*  d= new DynamicObject();

    for(auto &c:container->controllables){
        if(Parameter * p = dynamic_cast<Parameter*>(c)){
            d->setProperty(p->niceName, p->value);

        }
    }
    for(auto &c:container->controllableContainers){
        d->setProperty(c->niceName, createDynamicObjectFromContainer(c));

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
    if(a.numArguments<1)jassertfalse;

    StringArray addrArray;
    addrArray.addTokens(a.arguments[0].toString(),juce::StringRef("/"), juce::StringRef("\""));
    juce::Array<String> addSplit = addrArray.strings;


    addSplit.remove(0);
    String controller = addSplit[0];

    bool success = false;


    if (controller == "node")
    {
        addSplit.remove(0);
        Controllable * c = NodeManager::getInstance()->getControllableForAddress(addSplit);


        if (c != nullptr)
        {

            success = true;

            if(a.numArguments==1 && c->type==Controllable::Type::TRIGGER){
                ((Trigger *)c)->trigger();
            }

            else{
                var value = a.arguments[1];
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
    }

    return var();
}


void JavascriptEnvironment::loadTest(){

    loadFile("/Users/Tintamar/Desktop/tst.js");
}

