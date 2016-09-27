///*
//  ==============================================================================
//
//    NodeChildProofer.h
//    Created: 26 Sep 2016 4:16:42pm
//    Author:  Martin Hermant
//
//  ==============================================================================
//*/
//#include  "JuceHeader.h"
//#include "NodeBase.h"
//#if LGML_UNIT_TESTS
//
//
//#ifndef NODECHILDPROOFER_H_INCLUDED
//#define NODECHILDPROOFER_H_INCLUDED
//
//
//
//
//
//class NodeChildProofer  : public UnitTest
//{
//public:
//  NodeChildProofer(String NodeName,int _numActionsPerControllables = 10)  :
//  UnitTest ("NodeChildProofer : "+NodeName),
//  testingNodeName(NodeName),
//  numActionsPerControllables(_numActionsPerControllables){}
//
//  
//  int numActionsPerControllables;
//  String testingNodeName;
//
//  bool doActionForControllable(Controllable * c){
//    switch(c->type ){
//      case Controllable::TRIGGER:
//        if(Trigger * t = dynamic_cast<Trigger*>(c)){t->trigger();}
//        break;
//      case Controllable::FLOAT:
//        if(FloatParameter * t = dynamic_cast<FloatParameter*>(c)){t->setValue(Random().nextFloat());}
//        break;
//      case Controllable::INT:
//        if(IntParameter * t = dynamic_cast<IntParameter*>(c)){t->setValue(Random().nextInt());}
//        break;
//      case  Controllable::BOOL:
//        if(BoolParameter * t = dynamic_cast<BoolParameter*>(c)){t->setValue(Random().nextBool());}
//        break;
//      case Controllable::STRING:
//        if(StringParameter * t = dynamic_cast<StringParameter*>(c)){t->setValue(String(Random().nextInt()));}
//        break;
//      case Controllable::RANGE:
//        return false;
//        break;
//      case Controllable::PROXY:
//        break;
//
//
//      default:
//        return false;
//
//    }
//    return true;
//  }
//  void runTest() override
//  {
//
//    NodeBase * testingNode = dynamic_cast<NodeBase*>(NodeFactory::createNode(NodeFactory::getTypeFromString(testingNodeName)));
//    NodeManager::getInstance()->mainContainer->addNode(testingNode);
//    expect(testingNode,"node not found for name : " +testingNodeName);
//    beginTest ("GetAllParameters");
//    Array<Controllable * > allControllables = testingNode->getAllControllables(true,true);
//
//    beginTest ("generate random sessions");
//    int totalNumAction = numActionsPerControllables * allControllables.size();
//    for(int i = 0 ; i < totalNumAction ; i ++){
//      Controllable * tested = allControllables [i%allControllables.size()];
//      expect(doActionForControllable(tested),"Action failed for Controllable : "+tested->getControlAddress() );
//    }
//
//
//
//  }
//};
//
//
//static bool hasBeenBuilt = false;
//
//bool buildTests(){
//  if(!hasBeenBuilt){
//    for(auto nName:nodeTypeNames){
//      new NodeChildProofer(nName,10);
//    }
//    hasBeenBuilt = true;
//  }
//  else {
//    DBG("trying to instanciate 2 tests");
//    jassertfalse;
//  }
//  return true;
//}
//
//
//buildTests();
//
//
//#endif // NODECHILDPROOFER_H_INCLUDED
//
//
//
//#endif
