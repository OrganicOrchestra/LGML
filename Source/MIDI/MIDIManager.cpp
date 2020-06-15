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

#include "MIDIManager.h"
#include "MIDIListener.h"
#include "../Logger/LGMLLogger.h"

#include "../Utils/DebugHelpers.h"
#include "../Utils/GlobalKeyListener.h"

juce_ImplementSingleton(MIDIManager)

#if JUCE_DEBUG && 0
#define DBGMIDI(x) DBG(x)
#else
#define DBGMIDI(x)
#endif

#if !ENGINE_HEADLESS
#include "../JuceHeaderUI.h" // for key listener
    class ComputerKeyboardMIDIDevice : public ReferenceCountedObject,
                                       private KeyListener
{
public:
    ComputerKeyboardMIDIDevice() : octave(4)
    {
        GlobalKeyListener::addTraversingListener(this);
    }
    ~ComputerKeyboardMIDIDevice()
    {
        GlobalKeyListener::removeTraversingListener(this);
    }
    static String deviceName;
    void addMidiInputCallback(MidiInputCallback *cb) { midiCallbacks.add(cb); }
    void removeMidiInputCallback(MidiInputCallback *cb) { midiCallbacks.removeAllInstancesOf(cb); }
    void handleIncomingMidiMessage(const MidiMessage &message)
    {

        for (auto callback : midiCallbacks)
        {
            callback->handleIncomingMidiMessage(nullptr, message);
        }
    }

    int keyToPitch(int kc)
    {

        constexpr int numpitches = 12 + 5;
        static int pitches[numpitches] = {'q', 'z', 's', 'e', 'd', 'f', 't', 'g', 'y', 'h', 'u', 'j',
                                          'k', 'o', 'l', 'p', 'm'};
        for (int i = 0; i < numpitches; i++)
        {
            if (kc == pitches[i])
            {
                return i + 12 * octave;
            }
        }
        return -1;
    }
    //==============================================================================
    bool keyPressed(const KeyPress &key, Component * /*originatingComponent*/) override
    {
        const uint32 time = Time::getMillisecondCounter();
        const char c = key.getTextCharacter();
        if (c == 'w')
        {
            octave--;
            octave = jmax(octave, 0);
            LOG(juce::translate("computer keyboard octave = ") << String(octave));
            return true;
        }
        else if (c == 'x')
        {
            octave++;
            octave = jmin(octave, 8);
            LOG(juce::translate("computer keyboard octave = ") << String(octave));
            return true;
        }
        else
        {
            int note = keyToPitch(key.getTextCharacter());
            int channel = 1;
            static const uint8 vel = 127;

            if (note >= 0)
            {
                // avoid repetition on holded keys
                for (auto k : keysDown)
                {
                    if (k->key == key)
                    {
                        return true;
                    }
                }

                handleIncomingMidiMessage(MidiMessage::noteOn(channel, note, vel)
                                              .withTimeStamp(time / 1000.));

                keysDown.add(new KeyPressTime{key, note, time});
                return true;
            }
        }
        return false;
    }

    bool keyStateChanged(const bool isKeyDown, Component * /*originatingComponent*/) override
    {
        if (isKeyDown)
        {
            return false;
        }
        const uint32 now = Time::getMillisecondCounter();
        int channel = 1;

        int i = 0;
        int nWasDown = 0;
        while (i < keysDown.size())
        {
            auto keyd = keysDown.getUnchecked(i);

            const bool isDown = keyd->key.isCurrentlyDown();

            if (!isDown)
            {
                //                int millisecs = 0;
                //                const uint32 pressTime = keyd->timeWhenPressed;
                //                if (now > pressTime)
                //                    millisecs = (int) (now - pressTime);
                handleIncomingMidiMessage(MidiMessage::noteOff(channel, keyd->note, (uint8)0)
                                              .withTimeStamp(now / 1000.));
                keysDown.remove(i);
                nWasDown++;
            }
            i++;
        }

        return nWasDown > 0;
    }

    struct KeyPressTime
    {
        KeyPress key;
        int note;
        uint32 timeWhenPressed;
    };

    OwnedArray<KeyPressTime> keysDown;
    int octave;
    Array<MidiInputCallback *> midiCallbacks;
};

#else
    class ComputerKeyboardMIDIDevice : public ReferenceCountedObject
{
public:
    void addMidiInputCallback(MidiInputCallback *cb) { jassertfalse; }
    void removeMidiInputCallback(MidiInputCallback *cb) { jassertfalse; }
    static String deviceName;
};
#endif
String ComputerKeyboardMIDIDevice::deviceName(juce::translate("Computer Keyboard"));

class LGMLMIDIInCB : public MidiInputCallback
{
public:
    void handleIncomingMidiMessage(MidiInput *source,
                                   const MidiMessage &message) final
    {
        for (auto cb : midiCallbacks)
        {
            cb->handleIncomingMidiMessage(source, message);
        }
    }
    void addMidiInputCallback(MidiInputCallback *cb) { midiCallbacks.add(cb); }
    void removeMidiInputCallback(MidiInputCallback *cb) { midiCallbacks.removeAllInstancesOf(cb); }
    Array<MidiInputCallback *> midiCallbacks;
};

LGMLMIDIInCB LGMLMIDIInCBInstance;
constexpr int MIDICheckInterval(1000);
const String toLGMLMIDIName = "fromLGML";
MIDIManager::MIDIManager() : computerKeyboardDevice(nullptr)
{
#if JUCE_LINUX || JUCE_MAC
    toLGMLMidi = juce::MidiInput::createNewDevice(toLGMLMIDIName, &LGMLMIDIInCBInstance);
    juce::MidiOutput::createNewDevice("toLGML");
#else
    toLGMLMidi = nullptr;
#endif
}

MIDIManager::~MIDIManager()
{
    if (computerKeyboardDevice)
    {
        while (!computerKeyboardDevice->decReferenceCountWithoutDeleting())
        {
        }
        delete computerKeyboardDevice;
    }
}

void MIDIManager::init()
{
    startTimer(MIDICheckInterval);
}

void MIDIManager::updateDeviceList(bool updateInput)
{
    StringArray deviceNames = updateInput ? MidiInput::getDevices() : MidiOutput::getDevices();

    StringArray sourceArray = updateInput ? inputDevices : outputDevices;
#if !ENGINE_HEADLESS
    if (updateInput)
    {
        deviceNames.add(ComputerKeyboardMIDIDevice::deviceName);
    }
#endif
    // if(updateInput && toLGMLMidi.get()){
    //     deviceNames.add(toLGMLMIDIName);
    // }
    StringArray devicesToAdd;
    StringArray devicesToRemove;

    //check added
    for (auto &newD : deviceNames.strings)
    {
        if (!sourceArray.contains(newD))
            devicesToAdd.add(newD);
    }

    //check removed
    for (auto &sourceD : sourceArray.strings)
    {
        if (!deviceNames.contains(sourceD))
            devicesToRemove.add(sourceD);
    }

    if (updateInput)
        inputDevices = deviceNames;
    else
        outputDevices = deviceNames;

    for (auto &d : devicesToAdd)
    {
        //    jassert(!d.contains("error"));
        listeners.call(updateInput ? &MIDIManagerListener::midiInputAdded : &MIDIManagerListener::midiOutputAdded, d);
        //        NLOG ("MIDIManager", juce::translate("MIDI 123 Added : 456").replace("123", updateInput ? "Input" : "Output").replace("456", d));
    }

    for (auto &d : devicesToRemove)
    {
        //    jassert(!d.contains("error"));
        listeners.call(updateInput ? &MIDIManagerListener::midiInputRemoved : &MIDIManagerListener::midiOutputRemoved, d);
        //        NLOG ("MIDIManager",  juce::translate("MIDI 123 Removed : 456").replace("123", updateInput ? "Input" : "Output").replace("456", d));
    }
}

void MIDIManager::enableInputDevice(const String &deviceName)
{

    DeviceUsageCount *duc = getDUCForInputDeviceName(deviceName);
    DBGMIDI("MIDIManager  Enable Input device : " << deviceName << ", duc != null ?" << (duc != nullptr ? "true" : "false"));

    if (duc == nullptr)
    {
        duc = new DeviceUsageCount(deviceName);
        inputCounts.add(duc);
    }

    DBGMIDI("MIDIManager deviceCount before increment : " << duc->usageCount);

    duc->usageCount++;
    DBGMIDI("MIDIManager deviceCount after increment : " << duc->usageCount);

    if (duc->usageCount >= 1)
    {
        DBGMIDI("AudioDeviceManager:Enable Input device : " << duc->deviceName);
        if (deviceName == ComputerKeyboardMIDIDevice::deviceName)
        {
            if (!computerKeyboardDevice)
            {
                computerKeyboardDevice = new ComputerKeyboardMIDIDevice();
            }
            computerKeyboardDevice->incReferenceCount();
        }
        else
        {
            getAudioDeviceManager().setMidiInputEnabled(duc->deviceName, true);
        }
    }
}

std::unique_ptr<MidiOutput> MIDIManager::enableOutputDevice(const String &deviceName)
{
    if (deviceName == ComputerKeyboardMIDIDevice::deviceName)
    {
        return nullptr;
    }
    DeviceUsageCount *duc = getDUCForOutputDeviceName(deviceName);

    if (duc == nullptr)
    {
        duc = new DeviceUsageCount(deviceName);
        outputCounts.add(duc);
    }

    duc->usageCount++;

    //if (duc->usageCount == 1) getAudioDeviceManager().setMidiInputEnabled(duc->deviceName, true); //no output device handling ?

    StringArray inD = MidiOutput::getDevices();
    StringRef dRef(deviceName);
    std::unique_ptr<MidiOutput> out = MidiOutput::openDevice(inD.indexOf(dRef));

    if (out)
    {
        LOG(juce::translate("Midi Out opened : ") << out->getName());
    }
    else
    {
        const String available(inD.joinIntoString(", "));

        LOG(juce::translate("can't open MIDI out device : 123 \n available : 456").replace("123", deviceName).replace("456", available));
    }

    return out;
}

void MIDIManager::disableInputDevice(const String &deviceName)
{
    DeviceUsageCount *duc = getDUCForInputDeviceName(deviceName);

    if (duc == nullptr)
        return;

    duc->usageCount--;

    if (duc->usageCount == 0)
    {
        DBGMIDI("Disable Input device : " << duc->deviceName);
        if (deviceName == ComputerKeyboardMIDIDevice::deviceName)
        {
            if (computerKeyboardDevice)
            {
                if (computerKeyboardDevice->decReferenceCountWithoutDeleting())
                {
                    delete computerKeyboardDevice;
                    computerKeyboardDevice = nullptr;
                }
            }
            else
            {
                jassertfalse;
            }
        }
        else
        {
            getAudioDeviceManager().setMidiInputEnabled(duc->deviceName, false);
        }
    }
}

// @Ben seems weird mixing duc
// btw do we really need that for output??
void MIDIManager::disableOutputDevice(const String &deviceName)
{
    DeviceUsageCount *duc = getDUCForOutputDeviceName(deviceName);

    if (duc == nullptr)
        return;

    duc->usageCount--;
}

MIDIManager::DeviceUsageCount *MIDIManager::getDUCForInputDeviceName(const String &deviceName)
{
    for (auto &duc : inputCounts)
    {
        if (duc->deviceName == deviceName)
            return duc;
    }

    return nullptr;
}

MIDIManager::DeviceUsageCount *MIDIManager::getDUCForOutputDeviceName(const String &deviceName)
{
    for (auto &duc : outputCounts)
    {
        if (duc->deviceName == deviceName)
            return duc;
    }

    return nullptr;
}

void MIDIManager::timerCallback()
{
    updateLists();
    checkMIDIListenerStates();
}

void MIDIManager::updateLists()
{
    updateDeviceList(true);  //update inputs
    updateDeviceList(false); //update outputs
}

void MIDIManager::addMIDIListener(MIDIListener *l)
{
    MIDIListeners.add(l);
    listeners.add(l);
    checkMIDIListenerStates();
}
void MIDIManager::removeMIDIListener(MIDIListener *l)
{
    MIDIListeners.removeAllInstancesOf(l);
    listeners.remove(l);
    checkMIDIListenerStates();
}

void MIDIManager::addMidiInputCallback(const String &deviceName, MidiInputCallback *cb)
{
    if (deviceName == ComputerKeyboardMIDIDevice::deviceName)
    {
        if (computerKeyboardDevice)
        {
            computerKeyboardDevice->addMidiInputCallback(cb);
        }
        else
        {
            jassertfalse;
        }
    }
    else if (deviceName == toLGMLMIDIName)
    {
        if (toLGMLMidi)
        {
            LGMLMIDIInCBInstance.addMidiInputCallback(cb);
        }
    }
    else
    {
        getAudioDeviceManager().addMidiInputCallback(deviceName, cb);
    }
}
void MIDIManager::removeMidiInputCallback(const String &deviceName, MidiInputCallback *cb)
{
    if (deviceName == ComputerKeyboardMIDIDevice::deviceName)
    {
        if (computerKeyboardDevice)
        {
            computerKeyboardDevice->removeMidiInputCallback(cb);
        }
    }
    else if (deviceName == toLGMLMIDIName)
    {
        if (toLGMLMidi)
        {
            LGMLMIDIInCBInstance.removeMidiInputCallback(cb);
        }
    }
    else
    {
        getAudioDeviceManager().removeMidiInputCallback(deviceName, cb);
    }
}

void MIDIManager::checkMIDIListenerStates()
{
    // attempt to disable useless timer calls,
    // useless for now as we don't have proper callbacks on systems midi port add or deletion
#if 0
    bool allValid = true;
    for(auto l: MIDIListeners){
        if(!l->hasValidPort){
            allValid = false;
        }
    }
    if(allValid){
        stopTimer();
    }
    else{
        startTimer(MIDICheckInterval);
    }
#endif
}
