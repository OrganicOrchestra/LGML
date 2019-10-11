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

#if !ENGINE_HEADLESS

#include "AppPropertiesUI.h"

#include "../Engine.h"
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_utils/gui/juce_AudioDeviceSelectorComponent.h> // AudioDevice selector
#include "../Utils/AutoUpdater.h" // for version checking
#include "../Audio/VSTManager.h"

extern ApplicationProperties* getAppProperties();

juce_ImplementSingleton(AppPropertiesUI);
/*
 extern Engine* getEngine();
 extern AudioDeviceManager& getAudioDeviceManager();
 */
String AppPropertiesUI::GeneralPageName("General");
String AppPropertiesUI::AudioPageName("Audio");
String AppPropertiesUI::AdvancedPageName("Advanced");
String AppPropertiesUI::PluginsPageName("Plugins");


class BoolPropUI : public BooleanPropertyComponent{
public:
    explicit BoolPropUI(const String & _name,const String  tooltip=""):BooleanPropertyComponent(juce::translate(_name),"1","0"),name(_name){
        jassert(getAppProperties()->getUserSettings()->containsKey(name));
        setTooltip(tooltip);
    }
    bool getState() const override{
        return  getAppProperties()->getUserSettings()->getBoolValue(name);

    }
    void setState(bool b)  override{
        BooleanPropertyComponent::setState(b);
        getAppProperties()->getUserSettings()->setValue(name,b);
        refresh();
    }
    String name ;
};


class EnumPropUI : public ChoicePropertyComponent{
public:
    typedef std::function<void(const String &)> CBType;
    EnumPropUI(const String & _name, StringArray _choices,CBType _cb,const String & suffix=String(),const String  tooltip=""):
    ChoicePropertyComponent(juce::translate(_name)+(suffix.isEmpty()?"":" "+juce::translate(suffix))),
    cb(std::move(_cb)),
    nonTranslatedChoices (std::move(_choices)),
    name(_name)
    {

        setTooltip(tooltip);
        for(auto &s:nonTranslatedChoices){
            ChoicePropertyComponent::choices.add(juce::translate(s));
        }
        auto cVal = getAppProperties()->getUserSettings()->getValue(name);
        si =-1;
        for(int i = 0 ; i < nonTranslatedChoices.size() ; i++){
            if(nonTranslatedChoices[i]==cVal){
                si=i;
                setIndex(i);
            }
        }

    }

    void setIndex (int newIndex)override{
        int newI = jmin(getChoices().size()-1,jmax(0,newIndex));

        if(newI!=si){

            getAppProperties()->getUserSettings()->setValue(name,nonTranslatedChoices[newI]);
            if(cb!=nullptr)
                cb(nonTranslatedChoices[newI]);
                }
        si = newI;
        refresh();
    }

    virtual int getIndex() const override{return si;}
    CBType cb;
    StringArray nonTranslatedChoices;
    int si ;
    String name ;
};

class FileListPropertyComponent : public TextPropertyComponent{
public:
    FileListPropertyComponent(const String& propertyName,String defaultValue="",bool isMultiLine=false,const String  tooltip=""):
    TextPropertyComponent (juce::translate(propertyName),//const String& propertyName,
                           100,//int maxNumChars,
                           isMultiLine,
                           true)//bool isEditable = true)
    ,name(propertyName)
    {
        setTooltip(tooltip);
        auto savedV = getAppProperties()->getUserSettings()->getValue(name);

            setText(savedV);
        

    }



    void setText(const String & t)override{
        auto files = StringArray();
        auto oldPaths = file_paths;
        file_paths.clear();
        files.addTokens(t, ",\n","\"");
        for(auto & p :files){
            
            if(!File(p).exists()){
                LOGE("file " << p << " is not valid");
            }
            else{
                file_paths.add(p);
            }
        }
        if(oldPaths!=file_paths){
            getAppProperties()->getUserSettings()->setValue(name,getText());
        }
        refresh();

    }
    String getText() const override{
        return file_paths.joinIntoString(",");
    }
    StringArray file_paths;
    String name ;
};

template<class FunctionType>
class BoolUnsavedPropUI : public BooleanPropertyComponent{
public:
    BoolUnsavedPropUI(const String & _name,FunctionType f,bool defaultV = false,const String  tooltip=""):BooleanPropertyComponent(juce::translate(_name),"1","0"),internalState(defaultV),func(f){

        setTooltip(tooltip);
    }
    bool getState() const override{
        return  internalState;

    }
    void setState(bool b)  override{
        BooleanPropertyComponent::setState(b);
        internalState = b;
        func(b);
        refresh();
    }
    bool internalState;
    FunctionType func;
};

template<class FunctionType>
class ActionPropUI : public ButtonPropertyComponent{
public:
    ActionPropUI(const String & name,FunctionType f,const String  tooltip=""):ButtonPropertyComponent(juce::translate(name),true),func(f){setTooltip(tooltip);}
    void buttonClicked() override{
        func(this);
    }
    String getButtonText() const override{
        return juce::translate(getName());
    }

    FunctionType func;


};


namespace{

    template<class FunctionType>
    ButtonPropertyComponent* createActionProp(const String & n,FunctionType f){
        return new ActionPropUI<FunctionType>(n,f);
    }

    template<class FunctionType>
    BooleanPropertyComponent* createUnsavedPropUI(const String & n, FunctionType f,bool defaultV = false){
        return new BoolUnsavedPropUI<FunctionType>(n,f,defaultV);
    }

    void resetPreferences(ButtonPropertyComponent * ){
        AlertWindow aw(juce::translate("reset preferences"),
                       juce::translate("Are you sure you want to delete your prefs?"),
                       AlertWindow::AlertIconType::WarningIcon);
        aw.addButton(juce::translate("Ok"), 1);
        aw.addButton(juce::translate("Cancel"), 2);
        if(aw.runModalLoop()==1){
            LOGW(juce::translate("reseted user preferences"));
            auto fl = {getAppProperties()->getUserSettings()->getFile()};
            for(const auto &f:fl){
                if(f.exists()){
#if JUCE_MAC
                    f.moveToTrash();
#else
                    f.deleteFile();
#endif
                }
            }
        }

    }
    void stimulateAudio(bool b){
        auto engine = getEngine();
        engine->stimulateAudio (b);
    }

    class VersionChecker : private DeletedAtShutdown ,private Timer {
    public:
        bool isChecking(){
            return (latestVChecker!=nullptr) && !latestVChecker->hasEnded;
        }
        void start(){
            count = 0;
            latestVChecker = std::make_unique< LatestVersionChecker>(true);
            startTimer(100);
        }
        void timerCallback() override{
            if(isChecking()){
                if(bt.get()){
                    auto alpha = jmap<float>((1.0f+ std::cos(count*float_Pi/5.0f))/2.0f,0.3f,1.f) ;
                    bt->setAlpha(alpha);
                    count++;
                }
            }
            else{
                bt->setAlpha(1);
                if(latestVChecker->end()){
                stopTimer();
                latestVChecker = nullptr;
                }
            }

        }
        std::unique_ptr<LatestVersionChecker>  latestVChecker;
        WeakReference<Component> bt;
        int count{};
    };
    VersionChecker* versionChecker = new VersionChecker();
    void checkUpdatesNow(ButtonPropertyComponent * bt){
        versionChecker->bt = bt;
        versionChecker->start();


    }


}

template <class T,class ...Args>
T* createNMod(std::function<void(T*)>  f,Args ...args){
    auto o = new T(args...);
    f(o);
    return o;
}

class PrefPanel : public PreferencesPanel{
    Component* createComponentForPage (const String& pageName)override{
        if(pageName==juce::translate(AppPropertiesUI::GeneralPageName)){
            auto res =  new PropertyPanel();
            res->addProperties
            (
             {
                 new BoolPropUI("check for updates"),
                 createActionProp(juce::translate("check for updates now"),checkUpdatesNow),
                 new EnumPropUI("language",Engine::getAvailableLanguages(),&Engine::setLanguage,juce::translate("(restart needed)"),juce::translate("language files must be located in ")+Engine::getTranslationFolder().getFullPathName())

             } );
            return res;
        }
        else if (pageName==juce::translate(AppPropertiesUI::AudioPageName)){
            auto* audioSettingsComp = new AudioDeviceSelectorComponent (getAudioDeviceManager(),
                                                                        0, 256,
                                                                        0, 256,
                                                                        false, false, false, false);
            return audioSettingsComp;


        }

        else if(pageName==juce::translate(AppPropertiesUI::AdvancedPageName)){
            auto res =  new PropertyPanel();

            res->addProperties(
                               {new BoolPropUI("multiThreadedLoading"),
                                   createActionProp(juce::translate("reset preferences"),resetPreferences),
                                   createUnsavedPropUI(juce::translate("stimulate Audio"),stimulateAudio),
                                   new FileListPropertyComponent("PureDataExternalPaths")

                               }
                               );
            return res;
        }

        else if(pageName==juce::translate(AppPropertiesUI::PluginsPageName)){
            auto vm = VSTManager::getInstance();
            if(vm){
                PropertiesFile* appProps(getAppProperties()?getAppProperties()->getUserSettings():nullptr);
                const File deadMansPedalFile ((appProps)?File(appProps->getFile().getSiblingFile ("RecentlyCrashedPluginsList")):File());

                auto res = new PluginListComponent (vm->formatManager,vm->knownPluginList,deadMansPedalFile,appProps, true);return res;


            }
        }
        return nullptr;

    }
};





////////////
// AppPropertiesUI
//////////////

static DrawableComposite *  createIcon(const String &n,PrefPanel * parent,const int color = 0){
    DrawableComposite * res = new DrawableComposite();

    //    DrawablePath * border = new DrawablePath();
    //    Path circle;
    //    circle.addEllipse(0, 0, parent->getButtonSize(), parent->getButtonSize());

    //    border->setPath(circle);
    //    border->setFill(Colours::transparentWhite);
    //    border->setFill(parent->findColour(TextButton::ColourIds::textColourOnId).brighter());

    //    res->addAndMakeVisible(border);


    auto * text = new DrawableText();
    text->setText(n.substring(0,1));
    text->setColour(parent->findColour(color>0? color:TextButton::ColourIds::textColourOffId));
    text->setJustification(juce::Justification::centred);
    text->setFontHeight(parent->getButtonSize());
    res->addAndMakeVisible(text);



    return res;
}

void createForPageName(const String & pageName,PrefPanel * prefPanel){
    const int normalColorId = TextButton::ColourIds::textColourOffId;
    const int hoverColorId = TextButton::ColourIds::buttonOnColourId;
    prefPanel->addSettingsPage(juce::translate(pageName),
                               createIcon(pageName,prefPanel,normalColorId) ,
                               createIcon(pageName,prefPanel,hoverColorId) ,
                               nullptr//createIcon(GeneralPageName,prefPanel,downColorId)
                               );
}

AppPropertiesUI::AppPropertiesUI():ResizableWindow(juce::translate("Settings"),true){


    prefPanel = std::make_unique<PrefPanel>();

    //    const int downColorId = TextButton::ColourIds::textColourOnId;
    createForPageName(GeneralPageName,prefPanel.get());
    createForPageName(AudioPageName,prefPanel.get());

    createForPageName(PluginsPageName,prefPanel.get());


    createForPageName(AdvancedPageName,prefPanel.get());


#ifdef JUCE_MAC
    setUsingNativeTitleBar(true);
#else
    setUsingNativeTitleBar(false);
#endif

    setContentNonOwned(prefPanel.get(), false);
    auto mainScreenB = Desktop::getInstance().getDisplays().getMainDisplay().userArea;
    auto area = mainScreenB.withSizeKeepingCentre(mainScreenB.getWidth()/3, mainScreenB.getHeight()/2);
    setBounds(area);
    setResizable (true, true);
    setDraggable (true);
}

AppPropertiesUI::~AppPropertiesUI(){
    getAppProperties()->saveIfNeeded();
    if(getEngine()){
        getEngine()->audioSettingsHandler.saveCurrent();
    }
    // TODO we may implement a mechanism to close appProps files every time we access it?
    //    getAppProperties().closeFiles();
}

void AppPropertiesUI::userTriedToCloseWindow() {
    deleteInstance();
}

bool AppPropertiesUI::keyPressed (const KeyPress& key){

    if (key.isKeyCode (KeyPress::escapeKey) )
    {
        deleteInstance();
        return true;
    }
    return false;
}

void AppPropertiesUI::showAppSettings(const String & name){

    auto i = getInstance();
    if(name.isNotEmpty()){
        i->prefPanel->setCurrentPage(name);
    }
    i->setVisible (true);
    i->toFront (true);




}

int AppPropertiesUI::getDesktopWindowStyleFlags() const {
    return
    (ResizableWindow::getDesktopWindowStyleFlags() | ComponentPeer::windowHasCloseButton )
    & ~ComponentPeer::windowHasMaximiseButton & ~ComponentPeer::windowHasMinimiseButton;
    
}
void AppPropertiesUI::closeAppSettings(){
    deleteInstance();
    
}

#endif
