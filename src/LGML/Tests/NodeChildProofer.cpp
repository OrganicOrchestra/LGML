/*
 ==============================================================================

 NodeChildProofer.h
 Created: 26 Sep 2016 4:16:42pm
 Author:  Martin Hermant

 ==============================================================================
 */
#include  "JuceHeader.h"

#if LGML_UNIT_TESTS

#include "NodeBase.h"
#include "NodeManager.h"
#include "Engine.h"


#ifndef NODECHILDPROOFER_H_INCLUDED
#define NODECHILDPROOFER_H_INCLUDED



extrern Engine * getEngine();

class NodeChildProofer  : public UnitTest
{
public:
  NodeChildProofer(String NodeName,int _numActionsPerControllables = 10)  :
  UnitTest ("NodeChildProofer : "+NodeName),
  testingNodeName(NodeName),
  numActionsPerControllables(_numActionsPerControllables){}


  int numActionsPerControllables;
  String testingNodeName;

  bool doActionForControllable(Controllable * c){
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
    return true;
  }
  void runTest() override
  {
    getEngine()->createNewGraph();
    beginTest("childProofing");
    ConnectableNode * testingNode = NodeManager::getInstance()->mainContainer->addNode(NodeFactory::getTypeFromString(testingNodeName));

    expect(testingNode!=nullptr,"node not found for name : " +testingNodeName);

    Array<Controllable * > allControllables = testingNode->getAllControllables(true,true);

    
    int totalNumAction = numActionsPerControllables * allControllables.size();
    for(int i = 0 ; i < totalNumAction ; i ++){

      Controllable * tested = allControllables [i%allControllables.size()];
      String err ="Action failed for Controllable : "+tested->getControlAddress();
      expect(doActionForControllable(tested),err );
      allControllables = testingNode->getAllControllables(true,true);
    }

    // just to be sure that async message are handled too
    Thread::sleep(500);
    

  }
};


static bool hasBeenBuilt = false;

bool buildTests(){
  if(!hasBeenBuilt){
    for(auto nName:nodeTypeNames){
      if(nName!="ContainerIn" && nName!= "ContainerOut")
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
