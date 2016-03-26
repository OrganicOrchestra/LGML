/*
 ==============================================================================
 
 MaincComponentFileDocument.cpp
 Created: 25 Mar 2016 7:07:20pm
 Author:  Martin Hermant
 
 ==============================================================================
 */

#include "MainComponent.h"

#include "NodeFactory.h"



// TODO Fuck XML Lets JSON that !!

String MainContentComponent::getDocumentTitle() {
    if (! getFile().exists())
        return "Unnamed";
    
    return getFile().getFileNameWithoutExtension();
}


Result MainContentComponent::loadDocument (const File& file){
    XmlDocument doc (file);
    ScopedPointer<XmlElement> xml (doc.getDocumentElement());
    
    if (xml == nullptr || ! xml->hasTagName ("LGMLPROJECT"))
        return Result::fail ("Not a valid filter graph file");
    
    restoreFromXml (*xml);
    return Result::ok();
}


Result MainContentComponent::saveDocument (const File& file){
    ScopedPointer<XmlElement> xml (createXml());
    
    if (! xml->writeToFile (file, String::empty))
        return Result::fail ("Couldn't write to the file");
    
    return Result::ok();
}



File MainContentComponent::getLastDocumentOpened() {
    RecentlyOpenedFilesList recentFiles;
    recentFiles.restoreFromString (getAppProperties().getUserSettings()
                                   ->getValue ("recentNodeGraphFiles"));
    
    return recentFiles.getFile (0);
}




void MainContentComponent::setLastDocumentOpened (const File& file) {
    
    RecentlyOpenedFilesList recentFiles;
    recentFiles.restoreFromString (getAppProperties().getUserSettings()
                                   ->getValue ("recentNodeGraphFiles"));
    
    recentFiles.addFile (file);
    
    getAppProperties().getUserSettings()->setValue ("recentNodeGraphFiles", recentFiles.toString());
}



/////////////////////
// saving


//==============================================================================
static XmlElement* createNodeXml (NodeBase* const node) noexcept
{
    
    XmlElement* e = new XmlElement ("NODE");
    e->setAttribute("NodeType", NodeFactory::nodeToString(node));
    e->setAttribute("NodeId", String(node->nodeId));
    
    XmlElement* p = new XmlElement("PARAMETERS");
    Array<Controllable*> cont =node->ControllableContainer::getAllControllables();
    for(auto &c:cont){
        Parameter * base = dynamic_cast<Parameter*>(c);
        if(base){
            p->setAttribute(base->shortName, base->toString());
        }
        else{
            // should never happen un less another Controllable type than parameter has been introduced
            jassertfalse;
        }
    }
    e->addChildElement(p);
    
    
    //    for (int i = 0; i < PluginWindow::NumTypes; ++i)
    //    {
    //          .... do we need support of other windows than main PluginWindow?
    //    }
    
    if(node->audioProcessor){
        
        
        XmlElement* state = new XmlElement ("STATE");
        MemoryBlock m;
        
        // TODO we could implement that for all node objects to be able to save any kind of custom data
        node->audioProcessor->getStateInformation (m);
        state->addTextElement (m.toBase64Encoding());
        e->addChildElement (state);
    }
    return e;
}

XmlElement* MainContentComponent::createXml() const
{
    XmlElement* projXml = new XmlElement("LGMLPROJECT");
    XmlElement* meta = new XmlElement("META");
    meta->setAttribute("LGMLVersion",ProjectInfo::versionString);
    
    projXml->addChildElement(meta);
    
    
    XmlElement* xml = new XmlElement ("NODEGRAPH");
    
    for (int i = 0; i < NodeManager::getInstance()->getNumNodes(); ++i){
        
        xml->addChildElement (createNodeXml (NodeManager::getInstance()->getNode (i)));
    }
    
    for (int i = 0; i < NodeManager::getInstance()->getNumConnections(); ++i)
    {
        const NodeConnection* const fc = NodeManager::getInstance()->getConnection(i);
        
        XmlElement* e = new XmlElement ("CONNECTION");
        
        e->setAttribute ("srcNodeId", (int) fc->sourceNode->nodeId);
        e->setAttribute ("dstNodeId", (int) fc->destNode->nodeId);
        e->setAttribute("connectionType",(int) fc->connectionType);
        // TODO embed routing info
        
        xml->addChildElement (e);
    }
    
    projXml->addChildElement(xml);
    return projXml;
}

/// ===================
// loading

void MainContentComponent::restoreFromXml (const XmlElement& proj)
{
    clear();
//    TODO check version Compat
//    XmlElement *meta = proj.getChildByName("META");
    
    XmlElement * xml = proj.getChildByName("NODEGRAPH");
    
    forEachXmlChildElementWithTagName (*xml, e, "NODE")
    {
        createNodeFromXml (*e);
        changed();
    }
    
    forEachXmlChildElementWithTagName (*xml, e, "CONNECTION")
    {
        NodeManager * nm = NodeManager::getInstance();
        NodeBase * srcNode = nm->getNodeForId(e->getIntAttribute ("srcNodeId"));
        NodeBase * dstNode = nm->getNodeForId(e->getIntAttribute ("dstNodeId"));
        int cType = e->getIntAttribute ("connectionType");
        if(srcNode && dstNode && isPositiveAndBelow(cType, (int)NodeConnection::ConnectionType::UNDEFINED)){
            nm->addConnection (srcNode,dstNode,NodeConnection::ConnectionType(cType));
        }
        else{
            
            // TODO nicely handle file format errors?
            jassertfalse;
        }
    }
    
    NodeManager::getInstance()->removeIllegalConnections();
}





void MainContentComponent::createNodeFromXml (const XmlElement& xml)
{
    NodeFactory::NodeType nodeType = NodeFactory::getTypeFromString(xml.getStringAttribute("NodeType"));
    int nodeId = xml.getIntAttribute("nodeId");
    NodeBase* node =  NodeManager::getInstance()->addNode(nodeType,nodeId);
    
    XmlElement * paramXml= xml.getChildByName("PARAMETERS");
    
    for(int i = 0;i < paramXml->getNumAttributes() ; i++){
        
        String curParamName = paramXml->getAttributeName(i);
        
        if(Controllable * c = node->getControllableByName(curParamName)){    
            if(Parameter * p = dynamic_cast<Parameter*>(c)){
                p->fromString(paramXml->getAttributeValue(i));
            }
            else{
                DBG("other Controllable than Parameters?");
                jassertfalse;
            }
        }
        else{
            DBG("attribute not found");
            jassertfalse;
        }
    }
    
    if(node->audioProcessor){
        if (const XmlElement* const state = xml.getChildByName ("STATE"))
        {
            MemoryBlock m;
            m.fromBase64Encoding (state->getAllSubText());
            node->audioProcessor->setStateInformation (m.getData(), (int) m.getSize());
        }
    }
    
}



//#if JUCE_MODAL_LOOPS_PERMITTED
//File MainContentComponent::getSuggestedSaveAsFile (const File& defaultFile){
//
//}
//#endif
