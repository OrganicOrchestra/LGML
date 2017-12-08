/*
 ==============================================================================

 AppPropertiesUI.cpp
 Created: 2 Oct 2017 2:00:38pm
 Author:  Martin Hermant

 ==============================================================================
 */

#include "AppPropertiesUI.h"
#include "../Engine.h"

#include "../Utils/AutoUpdater.h" // for version checking

juce_ImplementSingleton(AppPropertiesUI);

extern Engine* getEngine();
extern AudioDeviceManager& getAudioDeviceManager();

String AppPropertiesUI::GeneralPageName("General");
String AppPropertiesUI::AudioPageName("Audio");
String AppPropertiesUI::AdvancedPageName("Advanced");
String AppPropertiesUI::PluginsPageName("Plugins");


class BoolPropUI : public BooleanPropertyComponent{
public:
    BoolPropUI(const String & _name):BooleanPropertyComponent(_name,"1","0"){
        jassert(getAppProperties()->getUserSettings()->containsKey(_name));

    }
    bool getState() const override{
        return  getAppProperties()->getUserSettings()->getBoolValue(getName());

    }
    void setState(bool b)  override{
        BooleanPropertyComponent::setState(b);
        getAppProperties()->getUserSettings()->setValue(getName(),b);
        refresh();
    }

};

template<class FunctionType>
class BoolUnsavedPropUI : public BooleanPropertyComponent{
public:
    BoolUnsavedPropUI(const String & _name,FunctionType f,bool defaultV = false):BooleanPropertyComponent(_name,"1","0"),internalState(defaultV),func(f){


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
    ActionPropUI(const String & name,FunctionType f):ButtonPropertyComponent(name,true),func(f){};
    void buttonClicked() override{
        func(this);
    }
    String getButtonText() const override{
        return getName();
    };

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
        AlertWindow aw("reset preferences",
                       "Are you sure you want to delete your prefs?",
                       AlertWindow::AlertIconType::WarningIcon);
        aw.addButton("Ok", 1);
        aw.addButton("Cancel", 2);
        if(aw.runModalLoop()==1){
            LOG("!! reseted user preferences");
            auto fl = {getAppProperties()->getUserSettings()->getFile()};
            for(auto f:fl){
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
            latestVChecker = new LatestVersionChecker(true);
            startTimer(100);
        }
        void timerCallback() override{
            if(isChecking()){
                if(bt.get()){
                    float alpha = jmap<float>((1.0+cos(count*float_Pi/5.0f))/2,0.3,1.) ;
                    bt->setAlpha(alpha);
                    count++;
                }
            }
            else{
                bt->setAlpha(1);
                stopTimer();
                latestVChecker = nullptr;
            }

        }
        ScopedPointer<LatestVersionChecker>  latestVChecker;
        WeakReference<Component> bt;
        int count;
    };
    VersionChecker* versionChecker = new VersionChecker();
    void checkUpdatesNow(ButtonPropertyComponent * bt){
        versionChecker->bt = bt;
        versionChecker->start();


    }
}

class PrefPanel : public PreferencesPanel{
    Component* createComponentForPage (const String& pageName)override{
        if(pageName==AppPropertiesUI::GeneralPageName){
            auto res =  new PropertyPanel();
            res->addProperties(
                               {
                                   new BoolPropUI("check for updates"),
                                   createActionProp("check for updates now",checkUpdatesNow)

                               } );
            return res;
        }
        else if (pageName==AppPropertiesUI::AudioPageName){
            auto* audioSettingsComp = new AudioDeviceSelectorComponent (getAudioDeviceManager(),
                                                                        0, 256,
                                                                        0, 256,
                                                                        false, false, false, false);
            return audioSettingsComp;


        }

        else if(pageName==AppPropertiesUI::AdvancedPageName){
            auto res =  new PropertyPanel();
            res->addProperties(
                               {new BoolPropUI("multiThreadedLoading"),
                                   createActionProp("reset preferences",resetPreferences),
                                   createUnsavedPropUI("stimulate Audio",stimulateAudio),

                               } );
            return res;
        }

        else if(pageName==AppPropertiesUI::PluginsPageName){
            auto vm = VSTManager::getInstance();
            if(vm){
                auto appProps = getAppProperties()?getAppProperties()->getUserSettings():nullptr;
                const File deadMansPedalFile = appProps?File(appProps->getFile().getSiblingFile ("RecentlyCrashedPluginsList")):File();

                auto res = new PluginListComponent (vm->formatManager,vm->knownPluginList,deadMansPedalFile,appProps, true);

                return res;
                }

                }
                return nullptr;

                }
                };






                ////////////
                // AppPropertiesUI
                //////////////

                static ScopedPointer<DrawableComposite>  createIcon(const String &n,PrefPanel * parent,const int color = 0){
                    DrawableComposite * res = new DrawableComposite();

                    //    DrawablePath * border = new DrawablePath();
                    //    Path circle;
                    //    circle.addEllipse(0, 0, parent->getButtonSize(), parent->getButtonSize());

                    //    border->setPath(circle);
                    //    border->setFill(Colours::transparentWhite);
                    //    border->setFill(parent->findColour(TextButton::ColourIds::textColourOnId).brighter());

                    //    res->addAndMakeVisible(border);


                    DrawableText * text = new DrawableText();
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
                    prefPanel->addSettingsPage(pageName,
                                               createIcon(pageName,prefPanel,normalColorId) ,
                                               createIcon(pageName,prefPanel,hoverColorId) ,
                                               nullptr//createIcon(GeneralPageName,prefPanel,downColorId)
                                               );
                }

                AppPropertiesUI::AppPropertiesUI():ResizableWindow("Settings",true){


                    prefPanel = new PrefPanel();

                    //    const int downColorId = TextButton::ColourIds::textColourOnId;
                    createForPageName(GeneralPageName,prefPanel);
                    createForPageName(AudioPageName,prefPanel);

                    createForPageName(PluginsPageName,prefPanel);


                    createForPageName(AdvancedPageName,prefPanel);


#ifdef JUCE_MAC
                    setUsingNativeTitleBar(true);
#else
                    setUsingNativeTitleBar(false);
#endif

                    setContentNonOwned(prefPanel, false);
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
