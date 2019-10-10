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

#ifndef MIDIHELPERS_H_INCLUDED
#define MIDIHELPERS_H_INCLUDED
#include "../Controllable/Parameter/EnumParameter.h"
#include "MIDIListener.h"

namespace MIDIHelpers{

EnumParameterModel * getGlobalMidiModel();

    String midiMessageToDebugString(const MidiMessage &);
    String midiMessageToParamName(const MidiMessage &);
    MidiMessage midiMessageFromParam(const ParameterBase* p,int targetChannel);

struct MIDIIOChooser : EnumParameter::EnumListener{
        MIDIIOChooser(MIDIListener *l,bool showControllers,bool isOutputDevice);
        EnumParameter * getDeviceEnumParameter();
    private:
        EnumParameter * inP;
//        EnumParameter *outP;
        EnumParameter * listenedIn;
//        EnumParameter *listenedOut;
        MIDIListener * owner;
        bool showController;

        void enumOptionAdded (EnumParameter*, const Identifier&) override;
        void enumOptionRemoved (EnumParameter*, const Identifier&) override;
        void enumOptionSelectionChanged (EnumParameter*, bool /*isSelected*/, bool /*isValid*/, const Identifier&) override;
    bool isOutputDevice;
    bool autoOut=true;

    };

}


#endif  // MIDIHELPERS_H_INCLUDED
