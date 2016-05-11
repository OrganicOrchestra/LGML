/*
 ==============================================================================

 JsHelpers.h
 Created: 9 May 2016 5:21:34pm
 Author:  Martin Hermant

 ==============================================================================
 */

#ifndef JSHELPERS_H_INCLUDED
#define JSHELPERS_H_INCLUDED



static const Identifier jsLocalIdentifier("local");
static const Identifier jsGlobalIdentifier("lgml");
static const Identifier ptrIdentifier("_ptr");
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



template<class T>
inline T* getObjectPtrFromJS(const var::NativeFunctionArgs & a){
    DynamicObject * d = a.thisObject.getDynamicObject();
    if(d==nullptr)return nullptr;
    return dynamic_cast<T*>((T*)(int64)d->getProperty(ptrIdentifier));
}

inline String getJsFunctionNameFromAddress(const String & n){
    StringArray arr;
    arr.addTokens(n, "/","");
    arr.remove(0);
    return    arr.joinIntoString("_");

}

inline StringArray splitFunctionName(const Identifier & i){
    StringArray res;
    res.addTokens(i.toString(), "_","");
    return res;

}
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

            if(name!= ptrIdentifier){
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




#endif  // JSHELPERS_H_INCLUDED
