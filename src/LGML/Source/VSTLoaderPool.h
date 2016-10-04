/*
  ==============================================================================

    VSTLoaderPool.h
    Created: 4 Oct 2016 3:15:01pm
    Author:  Martin Hermant

  ==============================================================================
*/

#ifndef VSTLOADERPOOL_H_INCLUDED
#define VSTLOADERPOOL_H_INCLUDED
#include "JuceHeader.h"

class VSTNode;

class VSTLoaderJob : public ThreadPoolJob{

  public :
  VSTLoaderJob(PluginDescription *_pd,VSTNode * node);
  PluginDescription * pd;
  VSTNode * originNode;

  JobStatus runJob() override;
  
};


class VSTLoaderPool : public ThreadPool{
public:

  juce_DeclareSingleton(VSTLoaderPool, false);
  VSTLoaderPool(): ThreadPool(4){
    
  }
  


};



#endif  // VSTLOADERPOOL_H_INCLUDED
