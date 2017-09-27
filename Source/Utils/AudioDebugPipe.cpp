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

#include "AudioDebugPipe.h"
Array<AudioDebugPipe*> AudioDebugPipe::openedPipes;


String getAudioPipeFileName (const String& name)
{
    if (name.startsWith ("juce_")) {return name;}

    return "juce_" + name;
}

String getMsgPipeFileName (const String& name)
{
    if (name.startsWith ("juce_")) {return name;}

    return "juceMsg_" + name;
}


AudioDebugPipe::AudioDebugPipe (const String& name): Thread ("pipe :" + name)
{
    audioPipe.createNewPipe (getAudioPipeFileName (name), false);
    msgPipe.createNewPipe (getMsgPipeFileName (name), false);
    openedPipes.add (this);
    startThread();
}

AudioDebugPipe::~AudioDebugPipe()
{
    msgPipe.close();
    audioPipe.close();
    stopThread (400);

}

void AudioDebugPipe::deleteInstanciated()
{
    for (auto p : openedPipes)
    {
        delete p;
    }

    openedPipes.clear();
}


void AudioDebugPipe::push (const AudioBuffer<float>& b)
{

    buffer.addArray (b.getReadPointer (0), b.getNumSamples());

}

void AudioDebugPipe::push (float f)
{

    buffer.add (f);

}

void AudioDebugPipe::sendMessage (const String& c)
{
    DBG ("sending Msg :" + c);
    int _written = msgPipe.write (c.toRawUTF8(), (c.length() + 1) * sizeof (char), 10);

    if (_written < 0)
    {
        DBG ("can't open pipe : " + String (errno));
        jassertfalse;
    }

    DBG ("endSendingMessage : " + c);


}


void AudioDebugPipe::run()
{
    while (!threadShouldExit())
    {
        {
            sleep (100);
            ScopedLock lk (buffer.getLock());

            if (buffer.size())
            {
                //      DBG("writing to " + audioPipe.getName() + ":"+String(buffer.size()) );

                static int maxChunk = 1000;
                int i = 0;

                while (i < buffer.size() - 1)
                {
                    int toWrite = jmin (maxChunk, buffer.size() - 1 - i);
                    int _written = audioPipe.write (buffer.getRawDataPointer() + i, toWrite * sizeof (float), 30);

                    if (_written < 0)
                    {
                        DBG ("can't open pipe : " + String (errno));
                        jassertfalse;
                    }

                    i += _written / sizeof (float);
                }

                int written = i + 1;

                //      DBG("end writting for " + audioPipe.getName());

                if (int leftOver = buffer.size() - written)
                {
                    DBG ("left" << leftOver);
                    jassertfalse;
                }

                buffer.clearQuick();
            }
        }

    }


}

int AudioDebugPipe::idxOfPipe (const String& n)
{
    int i = 0;
    String pipeName = getAudioPipeFileName (n);

    for (auto& p : openedPipes)
    {
        if (p->audioPipe.getName() == pipeName)
        {
            return i;
        }

        i++;
    }

    return -1;
}




AudioDebugPipe* AudioDebugPipe::getOrCreatePipe (const String& name)
{
    int idx =  AudioDebugPipe::idxOfPipe (name);

    if (idx < 0) {return new AudioDebugPipe (name);}
    else {return openedPipes[idx];}

}
