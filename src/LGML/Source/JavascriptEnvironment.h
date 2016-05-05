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
class ControllableContainer;
class JavascriptEnvironment : public JavascriptEngine{
public:
    JavascriptEnvironment();

    void buildEnvironment();

    typedef juce::var::NativeFunctionArgs NativeFunctionArgs;


    DynamicObject *  createDynamicObjectFromContainer(ControllableContainer * c,DynamicObject * parent);


    void loadFile(const String & path);

    ReferenceCountedObjectPtr<DynamicObject> localEnvironment;


    class OwnedJsArgs {


    public:
        OwnedJsArgs(DynamicObject * _scope):scope(_scope){}
        void addArg(float f){ownedArgs.add(new var(f));}
        void addArgs(const StringArray & a){for(auto & s:a){addArg(s.getFloatValue());}}

        NativeFunctionArgs getNativeArgs(){
            return NativeFunctionArgs(scope,ownedArgs.getFirst(),ownedArgs.size());
        }
    private:
        DynamicObject * scope;
        OwnedArray<var> ownedArgs;
    };

    void loadTest();

    static var post(const NativeFunctionArgs& a);
    static var set(const NativeFunctionArgs& a);


};




#endif  // JAVASCRIPTENVIRONNEMENT_H_INCLUDED
