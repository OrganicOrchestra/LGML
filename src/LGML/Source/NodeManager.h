/*
  ==============================================================================

    NodeManager.h
    Created: 2 Mar 2016 8:33:39pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef NODEMANAGER_H_INCLUDED
#define NODEMANAGER_H_INCLUDED
#pragma once
/*
Node Manager Contain all Node and synchronize building of audioGraph (AudioProcessorGraph) and DataGraph (DataProcessorGraph)

*/

#include "DataProcessorGraph.h"
#include "ControllableContainer.h"
#include "NodeContainer.h"//keep


class NodeManager:
	public ControllableContainer,
public ThreadPool
{

public:
    NodeManager();
    ~NodeManager();

    juce_DeclareSingleton(NodeManager, true);

    
    DataProcessorGraph dataGraph;

    ScopedPointer<NodeContainer> mainContainer;

	void rebuildAudioGraph();

    void clear();

	var getJSONData() override;
	void loadJSONData(const var & data) override;
  bool isLoading;



    //Listener
    class  NodeManagerListener
    {
    public:
        virtual ~NodeManagerListener() {}
		virtual void managerCleared() {};
		virtual void managerProgressedLoading(float /*progress*/) {}
      virtual void managerEndedLoading(){};
    };

    ListenerList<NodeManagerListener> nodeManagerListeners;
    void addNodeManagerListener(NodeManagerListener* newListener) { nodeManagerListeners.add(newListener); }
    void removeNodeManagerListener(NodeManagerListener* listener) { nodeManagerListeners.remove(listener); }



private:
  // @ben no listener here for now, only because it's overkilling (and Im lazy..)
  void notifiedJobsEnded();
  void notifiedJobsProgressed(float progress);

  friend class JobsWatcher;

  class JobsWatcher:private Timer{
  public:
	  int startTotalJobNum;
	  int numJobsDone;
    JobsWatcher(NodeManager * _nm):owner(_nm){
      startTimer(100);
	  startTotalJobNum = owner->getNumJobs();
	  numJobsDone = 0;
	  DBG("Start timer with total job num " << startTotalJobNum);
    }

    void timerCallback() override{
		int newNumJobsDone = startTotalJobNum - owner->getNumJobs();
		if (newNumJobsDone != numJobsDone)
		{
			owner->notifiedJobsProgressed(numJobsDone*1.f/startTotalJobNum);
		}

		if (owner->getNumJobs() == 0) {
			owner->notifiedJobsEnded();
			stopTimer();
		}
		
    }
    NodeManager * owner;
    

  };




  ScopedPointer<JobsWatcher> jobsWatcher;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NodeManager)


};


#endif  // NODEMANAGER_H_INCLUDED
