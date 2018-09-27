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

#if LGML_UNIT_TESTS

#include "../Node/NodeBase.h"
#include "../Node/Manager/NodeManager.h"
#include "../Engine.h"


#ifndef NODECHILDPROOFER_H_INCLUDED
#define NODECHILDPROOFER_H_INCLUDED

String outTestPath ("/tmp/LGMLTest/");
File outTestDir;



class NodeChildProofer  : public UnitTest
{
public:
    NodeChildProofer (String NodeName, int _numActionsPerControllables = 10)  :
    UnitTest ("NodeChildProofer : " + NodeName),
    testingNodeName (NodeName),
    numActionsPerControllables (_numActionsPerControllables) {}


    int numActionsPerControllables;
    String testingNodeName;



    class ChildProoferThread : public Thread
    {
    public:
        ChildProoferThread (String name, NodeChildProofer* o, ConnectableNode* _testingNode): Thread (name), owner (o), testingNode (_testingNode)
        {

        }
        void run() override
        {
            Array<WeakReference<Controllable > > allControllables = testingNode->getAllControllables (true, true);
            int totalNumAction = owner->numActionsPerControllables * allControllables.size();
            int i = 0 ;

            while ( i < totalNumAction )
            {

                WeakReference<Controllable> tested = allControllables.getReference (i % allControllables.size());

                if (tested.get())
                {
                    if (!dynamic_cast<ContainerInNode*>(tested->parentContainer.get()) &&
                        dynamic_cast<ContainerOutNode*>(tested->parentContainer.get()) &&
                        tested != static_cast<ParameterContainer*>(tested->parentContainer.get())->nameParam
                        && tested->shortName != "savePreset" &&
                        tested->shortName != "loadFile" &&
                        tested->shortName != "scriptPath" &&
                        !tested->shortName.startsWith ("exportAudio") &&
                        tested != testingNode->nameParam)
                    {
                        String err = "Action failed for Controllable : " + tested->getControlAddress();

                        if (tested.get())
                        {
                            owner->expect (doActionForControllable (tested), err );
                            allControllables = testingNode->getAllControllables (true, true);
                        }
                        else
                        {
                            // controllable deleted before
                            //              jassertfalse;
                        }

                    }
                }

                i++;
            }

            File outFile = outTestDir.getChildFile (Thread::getThreadName());


            for (int i = 0 ; i < allTimeTaken.size() ; i++)
            {
                Identifier cName = allTimeTaken.getName (i);
                outFile.appendText (cName.toString());
                double mean = 0;

                for (auto& v : *allTimeTaken[cName].getArray())
                {
                    mean += (double)v;


                }

                int num = allTimeTaken[cName].getArray()->size();

                mean /= jmax (num, 1);
                outFile.appendText (" , ");
                outFile.appendText (String (mean));
                outFile.appendText ("\n");
            }
        };
        bool doActionForControllable (Controllable* c)
        {
            double startTime = juce::Time::getMillisecondCounterHiRes();

            if (Trigger* t = dynamic_cast<Trigger*> (c))
            {t->trigger();}
            else  if (FloatParameter* t = dynamic_cast<FloatParameter*> (c))
            {t->setValue (Random().nextFloat());}
            else if (IntParameter* t = dynamic_cast<IntParameter*> (c))
            {t->setValue (Random().nextInt());}
            else    if (BoolParameter* t = dynamic_cast<BoolParameter*> (c))
            {t->setValue (Random().nextBool());}
            else if (StringParameter* t = dynamic_cast<StringParameter*> (c))
            {t->setValue (String (Random().nextInt()));}
            else{
                return false;
            }

            double now =  juce::Time::getMillisecondCounterHiRes();
            double timeTaken = now - startTime;
            Identifier mapKey = c->getControlAddress();

            if (!allTimeTaken.contains (mapKey))
            {
                allTimeTaken.set (mapKey, Array<var>());
            }

            allTimeTaken[mapKey].getArray()->add (timeTaken);


            return true;
        }

        NodeChildProofer* owner;
        ConnectableNode* testingNode;
        NamedValueSet allTimeTaken;
    };

    void runTest() override
    {
        outTestDir = File (outTestPath);

        if (!outTestDir.exists())
        {
            outTestDir.createDirectory();
        }

        getEngine()->createNewGraph();

        ConnectableNode* testingNode = NodeManager::getInstance()->addNode (NodeFactory::createFromTypeID (testingNodeName));

        expect (testingNode != nullptr, "node not found for name : " + testingNodeName);
        {
            beginTest ("childProofing single thread");
            ChildProoferThread child ("monoThread", this, testingNode);
            child.run();

            // just to be sure that async message are handled too
            Thread::sleep (500);

        }

        {
            beginTest (" childProofing multi thread");
            OwnedArray<ChildProoferThread> childs;
            int numThreads = 2;

            for (int i = 0 ; i  < numThreads ; i++) { childs.add (new ChildProoferThread ("thread" + String (i), this, testingNode));}

            for (auto& c : childs) {c->startThread(); Thread::sleep (10);}



            bool oneChildIsRunning = true;

            while (oneChildIsRunning)
            {
                oneChildIsRunning = false;

                for (auto& c : childs)
                {
                    oneChildIsRunning |= c->isThreadRunning();
                    Thread::sleep (100);
                }
            }

            // just to be sure that async message are handled too
            Thread::sleep (500);

        }
    }
};

bool buildTests()
{
    static bool hasBeenBuilt = false;

    if (!hasBeenBuilt)
    {
        
        for (auto nName : NodeFactory::getRegisteredTypes())
        {
            if (nName != "t_ContainerIn" && nName != "t_ContainerOut" && !nName.contains ("AudioDevice"))
                new NodeChildProofer (nName, 10);
        }
        
        hasBeenBuilt = true;
    }
    else
    {
        DBG ("trying to instanciate 2 tests");
        jassertfalse;
    }
    
    return true;
}


bool res = buildTests();


#endif // NODECHILDPROOFER_H_INCLUDED



#endif
