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


#ifndef NODEMANAGER_H_INCLUDED
#define NODEMANAGER_H_INCLUDED
#pragma once
/*
Node Manager Contain all Node and synchronize building of audioGraph (AudioProcessorGraph) and DataGraph (DataProcessorGraph)

*/

#include "../../Data/DataProcessorGraph.h"
#include "../../Controllable/Parameter/ParameterContainer.h"
#include "../NodeContainer/NodeContainer.h"


class NodeManager:
    public NodeContainer,
    public ThreadPool
{

public:
    DECLARE_OBJ_TYPE (NodeManager)
    ~NodeManager();

    juce_DeclareSingleton (NodeManager, true);


    DataProcessorGraph dataGraph;


    void rebuildAudioGraph();

    void clear()override;


    void configureFromObject (DynamicObject* data) override;
    bool isLoading;



    //Listener
    class  NodeManagerListener
    {
    public:
        virtual ~NodeManagerListener() {}
        virtual void managerCleared() {};
        virtual void managerProgressedLoading (float /*progress*/) {}
        virtual void managerEndedLoading() {};
    };

    ListenerList<NodeManagerListener> nodeManagerListeners;
    void addNodeManagerListener (NodeManagerListener* newListener) { nodeManagerListeners.add (newListener); }
    void removeNodeManagerListener (NodeManagerListener* listener) { nodeManagerListeners.remove (listener); }



private:
    // @ben no listener here for now, only because it's overkilling (and Im lazy..)
    friend class Engine;
    void notifiedJobsEnded();
    void notifiedJobsProgressed (float progress);

    friend class JobsWatcher;

    class JobsWatcher: private Timer
    {
    public:
        int startTotalJobNum;
        int numJobsDone;
        JobsWatcher (NodeManager* _nm): owner (_nm)
        {
            startTimer (100);
            startTotalJobNum = owner->getNumJobs();
            numJobsDone = 0;
            DBG ("Start timer with total job num " << startTotalJobNum);
        }

        void timerCallback() override
        {
            int newNumJobsDone = startTotalJobNum - owner->getNumJobs();

            if (newNumJobsDone != numJobsDone)
            {
                owner->notifiedJobsProgressed (numJobsDone * 1.f / startTotalJobNum);
            }

            if (owner->getNumJobs() == 0)
            {
                owner->notifiedJobsEnded();
                stopTimer();
            }

        }
        NodeManager* owner;


    };




    ScopedPointer<JobsWatcher> jobsWatcher;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NodeManager)


};


#endif  // NODEMANAGER_H_INCLUDED
