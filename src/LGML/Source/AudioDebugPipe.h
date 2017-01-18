/*
 ==============================================================================

 AudioDebugPipe.h
 Created: 11 Jan 2017 12:15:18pm
 Author:  Martin Hermant

 ==============================================================================
 */

#ifndef AUDIODEBUGPIPE_H_INCLUDED
#define AUDIODEBUGPIPE_H_INCLUDED

#include "JuceHeader.h"

#ifndef DEBUGPIPE_ENABLED
  #define DEBUGPIPE_ENABLED 0
#endif

class AudioDebugPipe : public Thread{
public:

  AudioDebugPipe(const String & name);
  ~AudioDebugPipe();
  static void deleteAllPipes();

  void push(const AudioBuffer<float> & b);
  void sendMessage(const String & c);

  static int idxOfPipe(const String & n);
  static AudioDebugPipe * getOrCreatePipe(const String & name);

  void run() override;

  NamedPipe audioPipe;
  NamedPipe msgPipe;
  
  static Array<AudioDebugPipe*> openedPipes;

  Array<float,CriticalSection> buffer;
};


#if DEBUGPIPE_ENABLED
#define DBGAUDIO(name,b) if(juce_isRunningUnderDebugger()){AudioDebugPipe::getOrCreatePipe(name)->push(b);}
#define DBGAUDIOSETBPM(name,b) if(juce_isRunningUnderDebugger()){AudioDebugPipe::getOrCreatePipe(name)->sendMessage("BPM "+String(b));}
#else
#define DBGAUDIO(name,b) 
#define DBGAUDIOSETBPM(name,b)
#endif


#endif  // AUDIODEBUGPIPE_H_INCLUDED
