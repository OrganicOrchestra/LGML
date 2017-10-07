/*
 ==============================================================================

 AppPropertiesUI.cpp
 Created: 2 Oct 2017 2:00:38pm
 Author:  Martin Hermant

 ==============================================================================
 */

#include "AppPropertiesUI.h"
#include "../Engine.h"

juce_ImplementSingleton(AppPropertiesUI);

extern Engine* getEngine();
extern AudioDeviceManager& getAudioDeviceManager();

String AppPropertiesUI::GeneralPageName("General");
String AppPropertiesUI::AudioPageName("Audio");


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


class PrefPanel : public PreferencesPanel{
    Component* createComponentForPage (const String& pageName)override{
        if(pageName==AppPropertiesUI::GeneralPageName){
            auto res =  new PropertyPanel();
            res->addProperties(
            {new BoolPropUI("multiThreadedLoading"),
             new BoolPropUI("check for updates"),
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

        return nullptr;

    }
};






////////////
// AppPropertiesUI
//////////////

static ScopedPointer<DrawableComposite>  createIcon(const String &n,PrefPanel * parent){
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
    text->setColour(parent->findColour(TextButton::ColourIds::textColourOnId));
    text->setJustification(juce::Justification::centred);
    text->setFontHeight(parent->getButtonSize());
    res->addAndMakeVisible(text);
//


    return res;
}

AppPropertiesUI::AppPropertiesUI():ResizableWindow("Settings",true){


    prefPanel = new PrefPanel();

    prefPanel->addSettingsPage(GeneralPageName,createIcon(GeneralPageName,prefPanel) ,nullptr, nullptr);
    prefPanel->addSettingsPage(AudioPageName,createIcon(AudioPageName,prefPanel) ,nullptr, nullptr);


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
