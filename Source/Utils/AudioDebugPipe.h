/* Copyright © Organic Orchestra, 2017
*
* This file is part of LGML.  LGML is a software to manipulate sound in realtime
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation (version 3 of the License).
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
*/

#ifndef AUDIODEBUGPIPE_H_INCLUDED
#define AUDIODEBUGPIPE_H_INCLUDED

#include "../JuceHeaderAudio.h"


#define FORCE_DISABLE_DEBUGPIPE 1



class AudioDebugPipe : public Thread
{
public:

    AudioDebugPipe (const String& name);
    ~AudioDebugPipe();
    static void deleteInstanciated();

    void push (const AudioBuffer<float>& b);
    void push (const float f);
    void sendMessage (const String& c);

    static int idxOfPipe (const String& n);
    static AudioDebugPipe* getOrCreatePipe (const String& name);

    void run() override;

    NamedPipe audioPipe;
    NamedPipe msgPipe;

    static Array<AudioDebugPipe*> openedPipes;

    Array<float, CriticalSection> buffer;

};


#if !FORCE_DISABLE_DEBUGPIPE
    #define DBGAUDIO(name,b) if(juce_isRunningUnderDebugger()){AudioDebugPipe::getOrCreatePipe(name)->push(b);}
    #define DBGAUDIOSETBPM(name,b) if(juce_isRunningUnderDebugger()){AudioDebugPipe::getOrCreatePipe(name)->sendMessage("BPM "+String(b));}
    #define DEBUGPIPE_ENABLED juce_isRunningUnderDebugger()
#else
    #define DBGAUDIO(name,b)
    #define DBGAUDIOSETBPM(name,b)
    #define DEBUGPIPE_ENABLED 0
#endif


#endif  // AUDIODEBUGPIPE_H_INCLUDED
