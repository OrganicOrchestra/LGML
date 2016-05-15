/*
 ==============================================================================

 JsContainerSync.cpp
 Created: 9 May 2016 6:21:50pm
 Author:  Martin Hermant

 ==============================================================================
 */

#include "JsContainerSync.h"
#include "JsHelpers.h"


void JsContainerSync::linkToControllableContainer(const String & controllableNamespace,ControllableContainer * c){
    c->addControllableContainerListener(this);
    DynamicObject * obj = createDynamicObjectFromContainer(c,nullptr);
    if(!existInContainerNamespace(controllableNamespace)){
        linkedContainerNamespaces.add(new JsContainerNamespace(controllableNamespace,c,obj)) ;
    }

    getEnv()->setProperty(controllableNamespace, obj);

}



JsContainerSync::JsContainerNamespace* JsContainerSync::getContainerNamespace(ControllableContainer * c){
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


JsContainerSync::JsContainerNamespace* JsContainerSync::getContainerNamespace(const String & ns){
    JsContainerNamespace* result = nullptr;

    for(auto & n:linkedContainerNamespaces){
        if(n->nsName == ns) return n;
    }

    return result;
}

bool JsContainerSync::existInContainerNamespace(const String & ns){
    return getContainerNamespace(ns)!=nullptr;
}

DynamicObject* JsContainerSync::createDynamicObjectFromContainer(ControllableContainer * container,DynamicObject *parent)
{


    DynamicObject*  d = parent;
    if(!container->skipControllableNameInAddress)
        d= new DynamicObject();


    for(auto &c:container->controllables){

        if(Parameter * p = dynamic_cast<Parameter*>(c)){
            DynamicObject* dd= new DynamicObject();

            dd->setProperty("get", p->value);
            dd->setProperty(ptrIdentifier, (int64)p);
            dd->setMethod("set", setControllable);
            d->setProperty(p->shortName, dd);


        }
        else if(Trigger * t = dynamic_cast<Trigger*>(c)){
            DynamicObject* dd= new DynamicObject();
            dd->setMethod("t", setControllable);
            dd->setProperty(ptrIdentifier, (int64)t);
            d->setProperty(t->shortName, dd);

        }
    }
    for(auto &c:container->controllableContainers){
        jassert(c->shortName.isNotEmpty());
        bool isNumber = true;
        juce::String::CharPointerType cc = c->shortName.getCharPointer();
        while (!cc.isEmpty()){
            isNumber &= cc.isDigit();
            cc.getAndAdvance();
        }
        if(isNumber){

            static const Identifier ArrayIdentifier("elements");

            if(!d->hasProperty(ArrayIdentifier)){
                var aVar;
                DynamicObject * childObject = createDynamicObjectFromContainer(c,d);
                jassert(c->shortName.getIntValue() == 0);
                aVar.append(childObject);
                // skiping not handled
                jassert(!c->skipControllableNameInAddress);
                d->setProperty(ArrayIdentifier, aVar);

            }
            else{
                Array<var> * arrVar;
                arrVar = d->getProperty(ArrayIdentifier).getArray();
                jassert(c->shortName.getIntValue() == arrVar->size());
                DynamicObject * childObject =createDynamicObjectFromContainer(c,d);
                arrVar->add(childObject);
            }



        }
        else{
            DynamicObject * childObject = createDynamicObjectFromContainer(c,d);
            if(!c->skipControllableNameInAddress)
                d->setProperty(c->shortName, childObject);
        }
    }

    return d;
}

var JsContainerSync::setControllable(const juce::var::NativeFunctionArgs& a){

    Controllable * c = getObjectPtrFromJS<Controllable>(a);
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



void JsContainerSync::childStructureChanged(ControllableContainer * c){
    if(JsContainerNamespace * ns= getContainerNamespace(c)){
        getEnv()->setProperty(ns->nsName, createDynamicObjectFromContainer(ns->container, nullptr));
    }
}
