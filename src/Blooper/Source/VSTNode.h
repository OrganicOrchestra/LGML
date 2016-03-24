/*
 ==============================================================================
 
 VSTNode.h
 Created: 2 Mar 2016 8:37:24pm
 Author:  bkupe
 
 ==============================================================================
 */

#ifndef VSTNODE_H_INCLUDED
#define VSTNODE_H_INCLUDED


#include "VSTManager.h"
#include "NodeBase.h"

#include "PluginWindow.h"

AudioDeviceManager& getAudioDeviceManager();


class VSTNode : public NodeBase,public ChangeBroadcaster,public AudioProcessorListener
{
    
public:
    VSTNode(NodeManager * nodeManager,uint32 nodeId) :NodeBase(nodeManager,nodeId,"VST",new VSTProcessor(this)),blockFeedback(false) {

    }
    ~VSTNode(){
        PluginWindow::closeCurrentlyOpenWindowsFor (this);
    }
    
    void generatePluginFromDescription(PluginDescription * desc){
        VSTProcessor * vstProcessor = dynamic_cast<VSTProcessor*>(audioProcessor);
        vstProcessor->generatePluginFromDescription(desc);
    }
    
    
    void createPluginWindow();
    void closePluginWindow();
    
    void parameterValueChanged(Parameter * p) override;

    void audioProcessorParameterChanged (AudioProcessor* processor,
                                                 int parameterIndex,
                                                 float newValue) override;

    void audioProcessorChanged (AudioProcessor* processor)override{};
    //kept to keep cross compatibility with pluginWindow
    // but we should implement our mechanism to save VST WindowsPosition and displayed state
    // do we really need it?
    NamedValueSet properties;
    
    Array<FloatParameter *> VSTParameters;
    void setParameterFromProcessor(AudioProcessor * p);

    
    
    class VSTProcessor : public NodeAudioProcessor{
        
    public:
        VSTProcessor(VSTNode * _owner):owner(_owner){
            
        }
        ~VSTProcessor(){}
        
        AudioProcessorEditor * createEditor()override{
            if(innerPlugin)return innerPlugin->createEditor();
                else return nullptr;
        }
        
        void generatePluginFromDescription(PluginDescription * desc){
            delete innerPlugin.release();
            String errorMessage;
            AudioDeviceManager::AudioDeviceSetup result;
            getAudioDeviceManager().getAudioDeviceSetup (result);
            if (AudioPluginInstance* instance = VSTManager::getInstance()->formatManager.createPluginInstance
                (*desc, result.sampleRate, result.bufferSize, errorMessage)){
                
                // try to align the precision of the processor and the graph
                instance->setProcessingPrecision (
                                                  //                                                  instance->supportsDoublePrecisionProcessing() ? precision:
                                                  singlePrecision);
                owner->setParameterFromProcessor(instance);
                
                int numIn=instance->getMainBusNumInputChannels();
                int numOut = instance->getMainBusNumOutputChannels();
                
                setPlayConfigDetails(numIn,numOut,result.sampleRate, result.bufferSize);
                //                instance->setPlayConfigDetails (numIn,numOut,result.sampleRate, result.bufferSize);
                instance->prepareToPlay (result.sampleRate, result.bufferSize);
                
                
                // TODO check if scoped pointer deletes old innerPlugin
                innerPlugin=instance;
            }
            
            else{
                
                DBG(errorMessage);
                jassertfalse;
            }
        }
        
        
        void numChannelsChanged()override;
        void prepareToPlay(double sampleRate,int blockSize) override{if(innerPlugin){innerPlugin->prepareToPlay(sampleRate,blockSize);}}
        void releaseResources() override {if(innerPlugin){innerPlugin->releaseResources();}};
        bool hasEditor() const override{if(innerPlugin){return innerPlugin->hasEditor();}return false;};
        
        
        void processBlockInternal(AudioBuffer<float>& buffer,MidiBuffer& midiMessages)override{
            if(innerPlugin){
                if( buffer.getNumChannels() >= jmax(innerPlugin->getTotalNumInputChannels(),innerPlugin->getTotalNumOutputChannels()))
                {innerPlugin->processBlock(buffer, midiMessages);}
                else{
                    DBG("dropAudio");
                }
            }
        };
        
        VSTNode * owner;
        ScopedPointer<AudioPluginInstance> innerPlugin;
    };
    NodeBaseUI * createUI()override;
    
    bool blockFeedback;    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VSTNode)
};



#endif  // VSTNODE_H_INCLUDED
