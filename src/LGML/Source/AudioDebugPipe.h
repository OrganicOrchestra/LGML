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


#define FORCE_DISABLE_DEBUGPIPE 1



class AudioDebugPipe : public Thread{
public:

  AudioDebugPipe(const String & name);
  ~AudioDebugPipe();
  static void deleteAllPipes();

  void push(const AudioBuffer<float> & b);
  void push(const float f);
  void sendMessage(const String & c);

  static int idxOfPipe(const String & n);
  static AudioDebugPipe * getOrCreatePipe(const String & name);

  void run() override;

  NamedPipe audioPipe;
  NamedPipe msgPipe;

  static Array<AudioDebugPipe*> openedPipes;

  Array<float,CriticalSection> buffer;
  
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
