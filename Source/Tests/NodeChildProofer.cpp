/*
 ==============================================================================

 NodeChildProofer.h
 Created: 26 Sep 2016 4:16:42pm
 Author:  Martin Hermant

 ==============================================================================
 */


#if LGML_UNIT_TESTS
#include  "JuceHeader.h"
#include "../Node/NodeBase.h"
#include "../Node/Manager/NodeManager.h"
#include "../Engine.h"


#ifndef NODECHILDPROOFER_H_INCLUDED
#define NODECHILDPROOFER_H_INCLUDED

String outTestPath("/tmp/LGMLTest/");
File outTestDir;



class NodeChildProofer  : public UnitTest
{
public:
  NodeChildProofer(String NodeName,int _numActionsPerControllables = 10)  :
  UnitTest ("NodeChildProofer : "+NodeName),
  testingNodeName(NodeName),
  numActionsPerControllables(_numActionsPerControllables){}


  int numActionsPerControllables;
  String testingNodeName;



  class ChildProoferThread : public Thread{
  public:
    ChildProoferThread(String name,NodeChildProofer *o,ConnectableNode * _testingNode):Thread(name),owner(o),testingNode(_testingNode){

    }
    void run() override{
      Array<WeakReference<Controllable > > allControllables = testingNode->getAllControllables(true,true);
      int totalNumAction = owner->numActionsPerControllables * allControllables.size();
      int i = 0 ;
      while( i < totalNumAction ){

        WeakReference<Controllable> tested = allControllables.getReference(i%allControllables.size());
        if(tested.get()){
          if (((NodeBase*)tested->parentContainer)->type!=NodeType::ContainerInType &&
              ((NodeBase*)tested->parentContainer)->type!=NodeType::ContainerOutType &&
              tested != tested->parentContainer->nameParam
              && tested->shortName !="savePreset" &&
              tested->shortName !="loadFile" &&
              tested->shortName !="scriptPath" &&
              !tested->shortName.startsWith("exportAudio") &&
              tested!=testingNode->nameParam){
            String err ="Action failed for Controllable : "+tested->getControlAddress();
            if(tested.get()){
              owner->expect(doActionForControllable(tested),err );
              allControllables = testingNode->getAllControllables(true,true);
            }
            else{
              // controllable deleted before
//              jassertfalse;
            }

          }
        }
        i++;
      }

      File outFile = outTestDir.getChildFile(Thread::getThreadName());


      for(int i = 0 ; i < allTimeTaken.size() ;i++){
        Identifier cName = allTimeTaken.getName(i);
        outFile.appendText(cName.toString());
        double mean = 0;
        for(auto & v:*allTimeTaken[cName].getArray()){
          mean+=(double)v;


        }
        int num =allTimeTaken[cName].getArray()->size();

        mean/=jmax(num,1);
        outFile.appendText(" , ");
        outFile.appendText(String(mean));
        outFile.appendText("\n");
      }
    };
    bool doActionForControllable(Controllable * c){
      double startTime = juce::Time::getMillisecondCounterHiRes();
      switch(c->type ){
        case Controllable::TRIGGER:
          if(Trigger * t = dynamic_cast<Trigger*>(c)){t->trigger();}
          break;
        case Controllable::FLOAT:
          if(FloatParameter * t = dynamic_cast<FloatParameter*>(c)){t->setValue(Random().nextFloat());}
          break;
        case Controllable::INT:
          if(IntParameter * t = dynamic_cast<IntParameter*>(c)){t->setValue(Random().nextInt());}
          break;
        case  Controllable::BOOL:
          if(BoolParameter * t = dynamic_cast<BoolParameter*>(c)){t->setValue(Random().nextBool());}
          break;
        case Controllable::STRING:
          if(StringParameter * t = dynamic_cast<StringParameter*>(c)){t->setValue(String(Random().nextInt()));}
          break;
        case Controllable::RANGE:
          return false;
          break;
        case Controllable::ENUM:
        case Controllable::PROXY:
        case Controllable::POINT2D:
        case Controllable::POINT3D:
          break;


        default:
          return false;

      }
      double now =  juce::Time::getMillisecondCounterHiRes();
      double timeTaken = now - startTime;
      Identifier mapKey =c->getControlAddress();
      if(!allTimeTaken.contains(mapKey)){
        allTimeTaken.set(mapKey,Array<var>());
      }
      allTimeTaken[mapKey].getArray()->add(timeTaken);


      return true;
    }

    NodeChildProofer * owner;
    ConnectableNode * testingNode;
    NamedValueSet allTimeTaken;
  };

  void runTest() override  {
    outTestDir = File(outTestPath);
    if(!outTestDir.exists()){
      outTestDir.createDirectory();
    }
    getEngine()->createNewGraph();

    ConnectableNode * testingNode = NodeManager::getInstance()->mainContainer->addNode(NodeFactory::getTypeFromString(testingNodeName));

    expect(testingNode!=nullptr,"node not found for name : " +testingNodeName);
    {
      beginTest("childProofing single thread");
      ChildProoferThread child("monoThread",this,testingNode);
      child.run();

      // just to be sure that async message are handled too
      Thread::sleep(500);

    }

    {
      beginTest(" childProofing multi thread");
      OwnedArray<ChildProoferThread> childs;
      int numThreads = 2;
      for(int i = 0 ; i  < numThreads ; i++){ childs.add(new ChildProoferThread("thread"+String(i),this,testingNode));}
      for(auto & c:childs){c->startThread();Thread::sleep(10);}



      bool oneChildIsRunning = true;
      while(oneChildIsRunning){
        oneChildIsRunning = false;
        for(auto & c:childs){
          oneChildIsRunning |= c->isThreadRunning();
          Thread::sleep(100);
        }
      }

      // just to be sure that async message are handled too
      Thread::sleep(500);


    }







  }

};


static bool hasBeenBuilt = false;

bool buildTests(){
  if(!hasBeenBuilt){

    for(auto nName:nodeTypeNames){
      if(nName!="ContainerIn" && nName!= "ContainerOut" && !nName.contains("AudioDevice"))
        new NodeChildProofer(nName,10);
    }
    hasBeenBuilt = true;
  }
  else {
    DBG("trying to instanciate 2 tests");
    jassertfalse;
  }
  return true;
}


bool res = buildTests();


#endif // NODECHILDPROOFER_H_INCLUDED



#endif
