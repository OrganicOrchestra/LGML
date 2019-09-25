/*
 ==============================================================================

 Copyright © Organic Orchestra, 2017

 This file is part of LGML. LGML is a software to manipulate sound in real-time

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation (version 3 of the License).

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 ==============================================================================
 */

#include "MIDIHelpers.h"
#include "MIDIManager.h"
#include "../Controllable/Parameter/ParameterContainer.h"
#include "../Controller/ControllerManager.h"
#include "../Controller/Impl/MIDIController.h"


namespace MIDIHelpers{


    String midiMessageToDebugString(const MidiMessage & message){
        if (message.isController()){
            return  String("CC 123 > 456 (Channel 789)")
            .replace("123", String (message.getControllerNumber()))
            .replace("456", String (message.getControllerValue()))
            .replace("789",String (message.getChannel()));
        }
        else if (message.isNoteOnOrOff())
        {
            bool isNoteOn = message.isNoteOn();

            return  String("123 : 456 (#789) : Ch 1011")
            .replace("123", String (isNoteOn ? "NoteOn" : "NoteOff"))
            .replace("456", MidiMessage::getMidiNoteName (message.getNoteNumber(), true, true, 0))
            .replace("789",String (message.getNoteNumber()))
            .replace("1011",String (message.getChannel()));

        }

        else if (message.isPitchWheel())
        {
            return "pitch wheel " + String (message.getPitchWheelValue());

        }
        else
        {
            return  "message : " + message.getDescription();
        }
    }


//    if (isPositiveAndBelow (note, 128))
//    {
//        String s (useSharps ? sharpNoteNames[note % 12]
//                  : flatNoteNames [note % 12]);
//
//        if (includeOctaveNumber)
//            s << (note / 12 + (octaveNumForMiddleC - 5));
//
//            return s;
//            }
//
//            return {};
    String midiMessageToParamName(const MidiMessage & message){
        if(message.isController()){
            return "CC "+String(message.getControllerNumber());
        }
        else if(message.isNoteOnOrOff()){
            return MidiMessage::getMidiNoteName (message.getNoteNumber(), true, true, 0);
        }

            return "";

    }
    MidiMessage midiMessageFromParam(const ParameterBase* p,int channel){
        static const Array<String> sharpNoteNames ( { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" });
//        static int octaveNumForMiddleC = 0;

        if(!p)return MidiMessage(); // return sysex if not defined
        String pn = p->niceName;
        int truChannel = jmax(1,channel);
        if(pn.startsWith(String("CC "))){
            return MidiMessage::controllerEvent(truChannel, pn.substring(3).getIntValue(), p->floatValue()*127);
        }
        else{
            int noteIdx = sharpNoteNames.indexOf(pn.substring(0, 1));

            if(noteIdx>=0){
                int noteNameLength = 1;
                if(pn[1]=='#'){
                    noteIdx++;
                    noteNameLength++;
                }
                int octave = pn.substring(noteNameLength).getIntValue();
                int noteNum = noteIdx+12*(octave+5);
                if(p->floatValue()>0){
                    return MidiMessage::noteOn(truChannel,noteNum , p->floatValue());// normalized velocity
                }
                else{
                    return MidiMessage::noteOff(truChannel,noteNum );
                }
            }



        }

    }

    class MIDIInModel : public EnumParameterModel,MIDIManager::MIDIManagerListener{
        public :
        MIDIInModel(){
            auto mm=MIDIManager::getInstance();
            mm->addMIDIManagerListener(this);
            for(auto &n:mm->inputDevices){
                midiInputAdded(n);
            }
        }
        ~MIDIInModel(){
            if(auto mm = MIDIManager::getInstanceWithoutCreating())
                mm->removeMIDIManagerListener(this);
        }
        void midiInputAdded (String& k) override{
            if(k!="<error>"){
            addOption(k, k, true);
            }
            else{
                LOGE(juce::translate("error while checking MIDI Input"));
            }
        };
        void midiInputRemoved (String& k)override {
            removeOption(k, true);
        };
        void refresh() override{
            MIDIManager::getInstance()->updateLists();
        }

    };
    class MIDIOutModel : public EnumParameterModel,MIDIManager::MIDIManagerListener{
        public :
        MIDIOutModel(){
            auto mm=MIDIManager::getInstance();
            mm->addMIDIManagerListener(this);
            for(auto &n:mm->outputDevices){
                midiOutputAdded(n);
            }
        }
        ~MIDIOutModel(){
            if(auto mm = MIDIManager::getInstanceWithoutCreating())
                mm->removeMIDIManagerListener(this);
        }
        void midiOutputAdded (String& k) override{
            addOption(k, k, true);
        };
        void midiOutputRemoved (String& k)override {
            removeOption(k, true);
        };
        void refresh() override{
            MIDIManager::getInstance()->updateLists();
        }

    };

    class MIDIControllerModel : public EnumParameterModel,
    ControllerManager::Listener,
    ParameterBase::Listener{
    public:
        MIDIControllerModel(){
            auto cm = ControllerManager::getInstance();
            cm->addControllerListener(this);
            for(auto c:cm->getContainersOfType<MIDIController>(false)){
                controllerAdded(c);
            }
        }
        ~MIDIControllerModel(){
            if(auto cm=ControllerManager::getInstanceWithoutCreating()){
                cm->removeControllerListener(this);

            }

        }
        void controllerAdded (Controller* c) override{
            if(auto mc = dynamic_cast<MIDIController*>(c)){
                c->nameParam->addParameterListener(this);
                addOption(mc->getNiceName(), MCToValue(mc), true);
            }
        };
        void controllerRemoved (Controller* c) override{
            if(auto mc=dynamic_cast<MIDIController*>(c)){
                removeOption(mc->getNiceName(), true);
            }
        };
        void parameterValueChanged ( ParameterBase* p, ParameterBase::Listener * /*notifier*/) override{
            auto c = p->parentContainer.get();
            if(auto cont = dynamic_cast<MIDIController*>(c)){
                jassert(p==cont->nameParam);
                const var v = MCToValue(cont);
                auto & k = getIdForValue(v);
                removeOption(k, true);
                addOption(c->getNiceName(), v, true);

            }

        };
        static var MCToValue(MIDIController *mc){
            return var(int64(mc));
        }
        static MIDIController* MCFromValue(const var & v){
            if(v.isInt64()){
                return dynamic_cast<MIDIController*>((MIDIController*)(int64)v);
            }
            return nullptr;
        }
    };

    template<class T>
    EnumParameterModel * getGlobalModel(){
        static T  em;//(nullptr);
//        if(em==nullptr){
//            em = new T();
//        }
        return &em;
    }



    ParameterContainer * getCont(MIDIListener *l){
        return dynamic_cast<ParameterContainer*>(l);
    }


    MIDIIOChooser::MIDIIOChooser(MIDIListener *l,bool autoOut,bool _showController):
    showController(_showController),owner(l),listenedIn(nullptr),inP(nullptr){
        auto cont = getCont(owner);
        if(cont!=nullptr){
            EnumParameterModel * m;
            if(autoOut){
                m = getGlobalModel<MIDIInModel>();
                inP = cont->addNewParameter<EnumParameter>("MidiDeviceChooser", "midi in param", m);
                listenedIn = inP;

            }
            if(showController){
                m = getGlobalModel<MIDIControllerModel>();
                inP = cont->addNewParameter<EnumParameter>("MIDIControllerChooser", "MIDIController to link to", m);

            }
            if(inP){
                inP->isEditable = false;
                inP->addEnumParameterListener(this);
            }
            else{
                jassertfalse;
            }

        }
        else{
            jassertfalse;
        }



    };

    void MIDIIOChooser::enumOptionAdded (EnumParameter*, const Identifier&) {};
    void MIDIIOChooser::enumOptionRemoved (EnumParameter*, const Identifier&) {};
    void MIDIIOChooser::enumOptionSelectionChanged (EnumParameter* ep, bool isSelected, bool isValid, const Identifier&) {


        if(showController && dynamic_cast<MIDIControllerModel*>(ep->getModel())){


            //                auto cm = ControllerManager::getInstance();

            MIDIController* c(nullptr);
            if(ep->selectionIsNotEmpty()){
                c=MIDIControllerModel::MCFromValue(ep->getFirstSelectedValue());
            }


            if(listenedIn){
                listenedIn->removeEnumParameterListener(this);

            }

            listenedIn = c?c->midiChooser.getDeviceInEnumParameter():nullptr;
            if(listenedIn){
                listenedIn ->addEnumParameterListener(this);
                owner->setCurrentDevice(listenedIn->getFirstSelectedValue().toString());
            }
            else{
                owner->setCurrentDevice("");
            }


        }

        else if(isValid && isSelected){
            owner->setCurrentDevice(ep->getFirstSelectedValue().toString());
        }
        else {//if (!isSelected || !isValid){
            owner->setCurrentDevice("");
        }
        
        
    };
    EnumParameter * MIDIIOChooser::getDeviceInEnumParameter(){
        return inP;
    }
    
    
};
