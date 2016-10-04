/*
  ==============================================================================

    VSTLoaderPool.cpp
    Created: 4 Oct 2016 3:20:41pm
    Author:  Martin Hermant

  ==============================================================================
*/

#include "VSTLoaderPool.h"
#include "VSTNode.h"

juce_ImplementSingleton(VSTLoaderPool)

VSTLoaderJob::VSTLoaderJob(PluginDescription *_pd,VSTNode * node):ThreadPoolJob("VSTLoader : "+node->shortName),pd(_pd),originNode(node){

}

VSTLoaderJob::JobStatus VSTLoaderJob::runJob(){
  originNode->generatePluginFromDescription(pd);
  originNode->triggerAsyncUpdate();
  return JobStatus::jobHasFinished;
}
