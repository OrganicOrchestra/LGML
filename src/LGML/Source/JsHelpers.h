/*
 ==============================================================================

 JsHelpers.h
 Created: 9 May 2016 5:21:34pm
 Author:  Martin Hermant

 ==============================================================================
 */

#ifndef JSHELPERS_H_INCLUDED
#define JSHELPERS_H_INCLUDED

#include <sstream>

#include <iomanip>



inline void addToNamespace(const String & elemName,DynamicObject *target,DynamicObject * global){
    jassert(target!=nullptr);
    jassert(elemName!="");
    int idx = elemName.indexOfChar('.');

    bool lastElem =(idx==-1);

    if(!lastElem){
        String elem = elemName.substring(0,idx);
        DynamicObject * dd = global->getProperty(elem).getDynamicObject();

        if(dd==nullptr)
        {
            dd= new DynamicObject();
            global->setProperty(elem, dd);
        }

        addToNamespace(elemName.substring(idx+1, elemName.length()), target,dd);
    }
    else{

        global->setProperty(elemName, target);
        //        registerNativeObject(elemName, target);
    }


}

inline void removeNamespaceFromObject(const String & ns,DynamicObject * d){
    if(d==nullptr){
        DBG("js Ns removing failed : not found "+ns);
        return;
    }
    int idx = ns.indexOfChar('.');
    bool lastElem =(idx==-1);
    if(!lastElem){
        String elem = ns.substring(0,idx);
        return removeNamespaceFromObject(ns.substring(idx+1, ns.length()),d->getProperty(elem).getDynamicObject());
    }
    else{
        d->removeProperty(ns);
    }
}



inline DynamicObject *  getNamespaceFromObject(const String & ns,DynamicObject * d ){
    if(d==nullptr)return nullptr;
    int idx = ns.indexOfChar('.');

    bool lastElem =(idx==-1);

    if(!lastElem){
        String elem = ns.substring(0,idx);
        return getNamespaceFromObject(ns.substring(idx+1, ns.length()),d->getProperty(elem).getDynamicObject());
    }
    else{
        return d->getProperty(ns).getDynamicObject();
    }
}



inline String namespaceToString(const NamedValueSet & v,int indentlevel = 0 ,bool showValue = false,bool showptr = false){
    String res;
    res+=" (";
    bool initedComma = false;
    for(int i = 0 ; i < v.size() ; i++){
        var * vv = v.getVarPointerAt(i);

        if(!vv->isObject()){
            Identifier name = v.getName(i);
            static const Identifier ptrId("_ptr");
            if(name!= ptrId){
                res+= (initedComma?", ":"")+name.toString() + (showValue?'('+ vv->toString()+")":"") ;
                initedComma = true;
            }
        }
    }
    res+=+ ")\n";

    for(int i = 0 ; i < v.size() ; i++){
        var * vv = v.getVarPointerAt(i);
        String name = v.getName(i).toString();


        if(vv->isObject()){
            for(int  j = 0 ; j < indentlevel ; j ++ ){
                res+="*.";
            }

            DynamicObject * d = vv->getDynamicObject();
            if(showptr){
                res+="("+String::toHexString((int64)d)+")";
            }
            if(d!=nullptr){
                res+= name+":";
                res+=namespaceToString(d->getProperties(),indentlevel+1,showValue,showptr);}

        }

    }

    return res;

}



//////////////////
// helperclasses


static const Identifier jsLocalIdentifier("l");
static const Identifier jsGlobalIdentifier("g");
static const Array<Identifier> coreJsClasses = {
    jsLocalIdentifier,
    jsGlobalIdentifier,
    Identifier("Object"),
    Identifier("Array"),
    Identifier("String"),
    Identifier("Math"),
    Identifier("JSON"),
    Identifier("Integer")
};


#endif  // JSHELPERS_H_INCLUDED
