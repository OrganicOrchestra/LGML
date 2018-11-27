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
#include "../Version.h" // for project Info



#ifndef BUILD_VERSION_UID
#error BUILD_VERSION_UID should be defined in Version.h
#endif

#if DEBUG
#define FORCE_CHECK_VERSION 1
#define SKIP_DOWNLOAD 1
#else
#define FORCE_CHECK_VERSION 0
#define SKIP_DOWNLOAD 0
#endif

//// code adapted from projucer

#include "AutoUpdater.h"
#include "DebugHelpers.h"


extern ApplicationProperties * getAppProperties();



LatestVersionChecker::LGMLVersionTriple::LGMLVersionTriple()
:  major ((ProjectInfo::versionNumber & 0xff0000) >> 16),
minor ((ProjectInfo::versionNumber & 0x00ff00) >> 8),
build ((ProjectInfo::versionNumber & 0x0000ff) >> 0)
{}

LatestVersionChecker::LGMLVersionTriple::LGMLVersionTriple (int lgmlVersionNumber)
:  major ((lgmlVersionNumber & 0xff0000) >> 16),
minor ((lgmlVersionNumber & 0x00ff00) >> 8),
build ((lgmlVersionNumber & 0x0000ff) >> 0)
{}

LatestVersionChecker::LGMLVersionTriple::LGMLVersionTriple (int majorInt, int minorInt, int buildNumber)
: major (majorInt),
minor (minorInt),
build (buildNumber)
{}

bool LatestVersionChecker::LGMLVersionTriple::fromString (const String& versionString,
                                                          LatestVersionChecker::LGMLVersionTriple& result)
{
    StringArray tokenizedString = StringArray::fromTokens (versionString, ".", StringRef());

    if (tokenizedString.size() != 3)
        return false;

    result.major = tokenizedString [0].getIntValue();
    result.minor = tokenizedString [1].getIntValue();
    result.build = tokenizedString [2].getIntValue();

    return true;
}

String LatestVersionChecker::LGMLVersionTriple::toString() const
{
    String retval;
    retval << major << '.' << minor << '.' << build;
    return retval;
}

bool LatestVersionChecker::LGMLVersionTriple::operator> (const LatestVersionChecker::LGMLVersionTriple& b) const noexcept
{
    if (major == b.major)
    {
        if (minor == b.minor)
            return build > b.build;

        return minor > b.minor;
    }

    return major > b.major;
}


static const String getAppFileName(){
    return
#if JUCE_MAC
    "LGML.app";
#elif JUCE_WINDOWS
    "LGML.exe";
#elif JUCE_LINUX
    "LGML";
#else
#error not supported platform
#endif

}
//==============================================================================
struct RelaunchTimer  : private Timer
{
    RelaunchTimer (const File& f)  : parentFolder (f)
    {
        startTimer (1500);
    }

    void timerCallback() override
    {
        stopTimer();

        File app = parentFolder.getChildFile (getAppFileName());


        JUCEApplication::quit();

        if (app.exists())
        {
            app.setExecutePermission (true);

#if JUCE_MAC
            app.getChildFile ("Contents")
            .getChildFile ("MacOS")
            .getChildFile ("LGML").setExecutePermission (true);
#endif

            app.startAsProcess("--dummy");
        }



        delete this;
    }

    File parentFolder;
};

//==============================================================================
class DownloadNewVersionThread   : public ThreadWithProgressWindow
{
public:
    DownloadNewVersionThread (LatestVersionChecker& versionChecker,URL u,
                              const String& extraHeaders, File target)
    : ThreadWithProgressWindow ("Downloading New Version", true, true),
    owner (versionChecker),
    result (Result::ok()),
    url (u), headers (extraHeaders), targetFolder (target)
    {
    }

    static void performDownload (LatestVersionChecker& versionChecker, URL u,
                                 const String& extraHeaders, File targetFolder)
    {
#if SKIP_DOWNLOAD
           new RelaunchTimer (targetFolder);
#else
        DownloadNewVersionThread d (versionChecker, u, extraHeaders, targetFolder);

        if (d.runThread())
        {
            if (d.result.failed())
            {
                AlertWindow::showMessageBoxAsync (AlertWindow::WarningIcon,
                                                  "Installation Failed",
                                                  d.result.getErrorMessage());
            }
            else

            {
                new RelaunchTimer (targetFolder);
            }
        }
    #endif
    }

    void run() override
    {
        setProgress (-1.0);

        MemoryBlock zipData;
        result = download (zipData);

        if (result.wasOk() && ! threadShouldExit())
        {
            setStatusMessage (juce::translate("Installing..."));
            result = owner.performUpdate (zipData, targetFolder);
        }
    }

    Result download (MemoryBlock& dest)
    {
        setStatusMessage (juce::translate("Downloading..."));

        int statusCode = 302;
        const int maxRedirects = 5;

        // we need to do the redirecting manually due to inconsistencies on the way headers are handled on redirects
        ScopedPointer<InputStream> in;

        for (int redirect = 0; redirect < maxRedirects; ++redirect)
        {
            StringPairArray responseHeaders;

            in = url.createInputStream (false, nullptr, nullptr, headers, 10000, &responseHeaders, &statusCode, 0);
            if (in == nullptr || statusCode != 302)
                break;

            String redirectPath = responseHeaders ["Location"];
            if (redirectPath.isEmpty())
                break;

            url = owner.getLatestVersionURL (headers, redirectPath);
        }

        if (in != nullptr && statusCode == 200)
        {
            int64 total = 0;
            MemoryOutputStream mo (dest, true);

            for (;;)
            {
                if (threadShouldExit())
                    return Result::fail ("cancel");

                int64 written = mo.writeFromInputStream (*in, 8192);

                if (written == 0)
                    break;

                total += written;

                setStatusMessage (String (juce::translate("Downloading...  (123)"))
                                  .replace ("123", File::descriptionOfSizeInBytes (total)));
            }

            return Result::ok();
        }

        return Result::fail ("Failed to download from: " + url.toString (false));
    }

    LatestVersionChecker& owner;
    Result result;
    URL url;
    String headers;
    File targetFolder;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DownloadNewVersionThread)
};


//==============================================================================
class UpdateUserDialog   : public Component,
public Button::Listener
{
public:
    UpdateUserDialog (const LatestVersionChecker::LGMLVersionTriple& version,
                      const String& productName,
                      const String& releaseNotes,
                      const char* overwriteFolderPath, bool hasDirectDownload)
    {


        LatestVersionChecker::LGMLVersionTriple currentVersion (ProjectInfo::versionNumber);

        addAndMakeVisible (titleLabel = new Label ("Title Label",
                                                   juce::translate("Download LGML version 123?").replace ("123", version.toString())));

        titleLabel->setFont (Font (15.00f, Font::bold));
        titleLabel->setJustificationType (Justification::centredLeft);
        titleLabel->setEditable (false, false, false);

        addAndMakeVisible (contentLabel = new Label ("Content Label",
                                                     juce::translate("A new version of LGML is available - would you like to download it?")
                                                     ));
        contentLabel->setFont (Font (15.00f, Font::plain));
        contentLabel->setJustificationType (Justification::topLeft);
        contentLabel->setEditable (false, false, false);

        addAndMakeVisible (okButton = new TextButton ("OK Button"));
        okButton->setButtonText (juce::translate("Download page"));
        okButton->addListener (this);

        if(hasDirectDownload){
            addAndMakeVisible (dlButton = new TextButton ("DL Button"));
            dlButton->setButtonText (juce::translate("Download now"));
            dlButton->addListener (this);
        }

        addAndMakeVisible(dontBotherMeCheck = new TextButton("StopBothering"));
        dontBotherMeCheck->setButtonText (juce::translate("Don't check"));
        dontBotherMeCheck->setClickingTogglesState(true);
        dontBotherMeCheck->addListener (this);


        addAndMakeVisible (cancelButton = new TextButton ("Cancel Button"));
        cancelButton->setButtonText (juce::translate("Cancel"));
        cancelButton->addListener (this);



        if(!(version>currentVersion)){
            titleLabel->setText(String("you have the latest version : 123 re-download it?").replace("123", version.toString()), dontSendNotification);
            contentLabel->setText("", dontSendNotification);
        }
        addAndMakeVisible (changeLogLabel = new Label ("Change Log Label",
                                                       juce::translate("Release Notes:")));
        changeLogLabel->setFont (Font (15.00f, Font::plain));
        changeLogLabel->setJustificationType (Justification::topLeft);
        changeLogLabel->setEditable (false, false, false);

        addAndMakeVisible (changeLog = new TextEditor ("Change Log"));
        changeLog->setMultiLine (true);
        changeLog->setReturnKeyStartsNewLine (true);
        changeLog->setReadOnly (true);
        changeLog->setScrollbarsShown (true);
        changeLog->setCaretVisible (false);
        changeLog->setPopupMenuEnabled (false);
        changeLog->setText (releaseNotes);


        juceIcon = Drawable::createFromImageData (BinaryData::grandlouloup_png,
                                                  BinaryData::grandlouloup_pngSize);

        setSize (518, overwritePath ? 345 : 269);

        lookAndFeelChanged();
    }

    ~UpdateUserDialog()
    {
        titleLabel = nullptr;
        contentLabel = nullptr;
        okButton = nullptr;
        dlButton = nullptr;
        cancelButton = nullptr;
        dontBotherMeCheck = nullptr;
        changeLogLabel = nullptr;
        changeLog = nullptr;
        overwriteLabel = nullptr;
        overwritePath = nullptr;
        juceIcon = nullptr;
    }

    void paint (Graphics& g) override
    {
        g.fillAll (findColour (CodeEditorComponent::backgroundColourId));
        g.setColour (findColour (CodeEditorComponent::defaultTextColourId));

        if (juceIcon != nullptr)
            juceIcon->drawWithin (g, Rectangle<float> (20, 17, 64, 64),
                                  RectanglePlacement::centred, 1.000f);
    }

    void resized() override
    {
        titleLabel->setBounds (88, 10, 397, 24);
        contentLabel->setBounds (88, 40, 397, 51);
        changeLogLabel->setBounds (22, 92, 341, 24);
        changeLog->setBounds (24, 112, 476, 102);

        int bW = 100;
        Rectangle<int> darea = getBounds().removeFromBottom(28);
        auto larea = darea.removeFromRight((dlButton?3:2)*bW);
        cancelButton->setBounds (larea.removeFromLeft(bW).reduced(2));
        if(dlButton)dlButton->setBounds (larea.removeFromLeft(bW).reduced(2));
        okButton->setBounds (larea.removeFromLeft(bW).reduced(2));


        dontBotherMeCheck->setBounds(darea.removeFromLeft(bW).reduced(2));
    }

    void buttonClicked (Button* clickedButton) override
    {
        if (DialogWindow* parentDialog = findParentComponentOfClass<DialogWindow>())
        {
            if      (clickedButton == okButton) parentDialog->exitModalState (1);
            else if (clickedButton == dlButton)        parentDialog->exitModalState (2);
            else if (clickedButton == cancelButton)    parentDialog->exitModalState (-1);
            else if(clickedButton==dontBotherMeCheck){
                getAppProperties()->getUserSettings()->setValue("check for updates",!dontBotherMeCheck->getToggleState());
                getAppProperties()->getUserSettings()->saveIfNeeded();
            }
        }
        else
            jassertfalse;
    }

    static DialogWindow* launch (const LatestVersionChecker::LGMLVersionTriple& version,
                                 const String& productName,
                                 const String& releaseNotes,
                                 const char* overwritePath = nullptr,bool hasDirectDownload=false)
    {
        OptionalScopedPointer<Component> userDialog (new UpdateUserDialog (version, productName,
                                                                           releaseNotes, overwritePath,hasDirectDownload), true);

        DialogWindow::LaunchOptions lo;
        lo.dialogTitle = "LGML version checker";
        
        lo.content = userDialog;
        lo.componentToCentreAround = nullptr;
        lo.escapeKeyTriggersCloseButton = true;
        lo.useNativeTitleBar = true;
        lo.resizable = false;
        lo.useBottomRightCornerResizer = false;

        return lo.launchAsync();
    }

private:
    bool hasOverwriteButton;
    ScopedPointer<Label> titleLabel, contentLabel, changeLogLabel, overwriteLabel, overwritePath;
    ScopedPointer<TextButton> okButton, cancelButton, dontBotherMeCheck,dlButton;
    ScopedPointer<TextEditor> changeLog;
    ScopedPointer<TextButton> overwriteButton;
    ScopedPointer<Drawable> juceIcon;

    void lookAndFeelChanged() override
    {
        //        cancelButton->setColour (TextButton::buttonColourId,
        //                                 findColour (secondaryButtonBackgroundColourId));
        changeLog->applyFontToAllText (changeLog->getFont());
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UpdateUserDialog)
};

//==============================================================================
class UpdaterDialogModalCallback : public ModalComponentManager::Callback
{
public:
    struct DelayedCallback  : private Timer
    {
        DelayedCallback (LatestVersionChecker& versionChecker,
                         URL& newVersionToDownload,
                         const String& extraHeaders,
                         const File& appParentFolder,
                         int returnValue)
        : parent (versionChecker), download (newVersionToDownload),
        headers (extraHeaders), folder (appParentFolder), result (returnValue)
        {
            startTimer (200);
        }

    private:
        void timerCallback() override
        {
            stopTimer();
            parent.modalStateFinished (result, download, headers, folder);

            delete this;
        }

        LatestVersionChecker& parent;
        URL download;
        String headers;
        File folder;
        int result;


        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayedCallback)
    };

    UpdaterDialogModalCallback (LatestVersionChecker& versionChecker,
                                URL& newVersionToDownload,
                                const String& extraHeaders,
                                const File& appParentFolder)
    : parent (versionChecker), download (newVersionToDownload),
    headers (extraHeaders), folder (appParentFolder)
    {}

    void modalStateFinished (int returnValue) override
    {
        // the dialog window is only closed after this function exits
        // so we need a deferred callback to the parent. Unfortunately
        // our instance is also deleted after this function is used
        // so we can't use our own instance for a timer callback
        // we must allocate a new one.

        new DelayedCallback (parent, download, headers, folder, returnValue);
    }

private:
    LatestVersionChecker& parent;
    URL download;
    String headers;
    File folder;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UpdaterDialogModalCallback)
};



//==============================================================================
LatestVersionChecker::LatestVersionChecker(bool force)  : Thread ("Updater"),
statusCode (-1),
hasAttemptedToReadWebsite (false),
force_show(force),
hasEnded(false)
{
    startTimer (2000);
}
constexpr int maxTimeOut = 20000;
LatestVersionChecker::~LatestVersionChecker()
{
    stopThread (maxTimeOut);
}

String LatestVersionChecker::getOSString()
{
    SystemStats::OperatingSystemType osType = SystemStats::getOperatingSystemType();

    if      ((osType & SystemStats::MacOSX)  != 0) return "osx";
    else if ((osType & SystemStats::Windows) != 0) return "windows";
    else if ((osType & SystemStats::Linux)   != 0) return "linux";
    else return SystemStats::getOperatingSystemName();
}

const LatestVersionChecker::LGMLServerLocationsAndKeys& LatestVersionChecker::getLGMLServerURLsAndKeys() const
{
    static LatestVersionChecker::LGMLServerLocationsAndKeys urlsAndKeys =
    {
        "https://storage.organic-orchestra.com/tools/LGML/",
        "",
        1,
        "version.json"
    };

    return urlsAndKeys;
}

int LatestVersionChecker::getProductVersionNumber() const   { return ProjectInfo::versionNumber; }
const char* LatestVersionChecker::getProductName() const    { return ProjectInfo::projectName; }
bool LatestVersionChecker::allowCustomLocation() const      { return true; }


Result LatestVersionChecker::performUpdate (const MemoryBlock& data, File& targetFolder)
{
    File unzipTarget;

    {
        MemoryInputStream input (data, false);
        ZipFile zip (input);

        if (zip.getNumEntries() == 0){
            return Result::fail ("The downloaded file wasn't a valid LGML file!");
        }

        unzipTarget = targetFolder;



        Result r (zip.uncompressTo (unzipTarget));

        if (r.failed())
        {

            return r;
        }
    }



    return Result::ok();
}


URL LatestVersionChecker::getLatestVersionURL (String& headers, const String& path) const
{
    const LatestVersionChecker::LGMLServerLocationsAndKeys& urlsAndKeys = getLGMLServerURLsAndKeys();

    String updateURL;
    bool isAbsolute = (path.startsWith ("http://") || path.startsWith ("https://"));
    bool isRedirect = path.isNotEmpty();

    if (isAbsolute)
    {
        updateURL = path;
    }
    else
    {
        updateURL << urlsAndKeys.updateSeverHostname
        << (isRedirect ? path : String (urlsAndKeys.updatePath));


    }

    headers.clear();

    if (! isAbsolute)
    {
        //        headers << "X-API-Key: " << urlsAndKeys.publicAPIKey;

        if (! isRedirect)
        {
            headers << "\nContent-Type: application/json\n"
            << "Accept: application/json;";
            //            <<" version=" << urlsAndKeys.apiVersion;
        }
    }

    return URL (updateURL);
}

URL LatestVersionChecker::getLatestVersionURL (String& headers) const
{
    String emptyString;
    return getLatestVersionURL (headers, emptyString);
}

void LatestVersionChecker::checkForNewVersion()
{
    hasAttemptedToReadWebsite = true;

    {
        String extraHeaders;
        URL updateURL (getLatestVersionURL (extraHeaders));
        StringPairArray responseHeaders;

        const int numRedirects = 0;

        const ScopedPointer<InputStream> in (updateURL.createInputStream (false, nullptr, nullptr,
                                                                          extraHeaders, maxTimeOut*3/4, &responseHeaders,
                                                                          &statusCode, numRedirects));

        if (threadShouldExit())
            return;  // can't connect: fail silently.

        if (in != nullptr && (statusCode == 303 || statusCode == 400 || statusCode==200))
        {
            // if this doesn't fail then there is a new version available.
            // By leaving the scope of this function we will abort the download
            // to give the user a chance to cancel an update
            if (statusCode == 303)
                newRelativeDownloadPath = responseHeaders ["Location"];


            jsonReply = JSON::parse (in->readEntireStreamAsString());


        }
    }

    if (! threadShouldExit())
        startTimer (100);
}

bool LatestVersionChecker::processResult (const var& reply, const String& downloadPath)
{
    if (statusCode == 303 || statusCode ==200)
    {
        String versionString = reply.getProperty ("version", var()).toString();
        String releaseNotes = reply.getProperty ("notes", var()).toString();
        LGMLVersionTriple version;

        if (versionString.isNotEmpty() && releaseNotes.isNotEmpty())
        {
            if (LGMLVersionTriple::fromString (versionString, version))
            {
                String extraHeaders;
                String urlPage = reply.getProperty("download_page",
                                                   "http://organic-orchestra.com/forum/d/6-lgml-telechargements").toString();
                URL newVersionToDownload (urlPage);
                bool hasDirectDownload = false;

                auto zips = reply.getProperty("zip_link", var());
                if(zips.hasProperty(BUILD_VERSION_UID)){
                    String _path = downloadPath;
                    if (statusCode==200){
                        _path = versionString <<'/'<< zips.getProperty(BUILD_VERSION_UID, var()).toString();
                    }
                    newVersionToDownload = getLatestVersionURL (extraHeaders, _path);
                    hasDirectDownload =true;
                }







                return askUserAboutNewVersion (version, releaseNotes, newVersionToDownload, extraHeaders,hasDirectDownload );

            }
        }
    }


    else if (statusCode == 400)
    {
        // In the far-distant future, this may be contacting a defunct
        // URL, so hopefully the website will contain a helpful message
        // for the user..
        var errorObj = reply.getDynamicObject()->getProperty ("error");

        if (errorObj.isObject())
        {
            String message = errorObj.getProperty ("message", var()).toString();

            if (message.isNotEmpty())
            {
                AlertWindow::showMessageBox (AlertWindow::WarningIcon,
                                             juce::translate("LGML Updater"),
                                             message);

                return false;
            }
        }
    }

    // try again
    return true;
}

bool LatestVersionChecker::askUserAboutNewVersion (const LatestVersionChecker::LGMLVersionTriple& version,
                                                   const String& releaseNotes,
                                                   URL& newVersionToDownload,
                                                   const String& extraHeaders,bool hasDirectDownload)
{
    LGMLVersionTriple currentVersion (getProductVersionNumber());

    if (FORCE_CHECK_VERSION || force_show || (version > currentVersion))
    {
        File appParentFolder (File::getSpecialLocation (File::currentApplicationFile).getParentDirectory());
        DialogWindow* modalDialog = nullptr;


        modalDialog = UpdateUserDialog::launch (version, getProductName(), releaseNotes,
                                                appParentFolder.getFullPathName().toRawUTF8(), hasDirectDownload);


        if (modalDialog != nullptr)
        {
            UpdaterDialogModalCallback* callback = new UpdaterDialogModalCallback (*this,
                                                                                   newVersionToDownload,
                                                                                   extraHeaders,
                                                                                   appParentFolder);

            // attachCallback will delete callback
            if (ModalComponentManager* mm = ModalComponentManager::getInstance())
                mm->attachCallback (modalDialog, callback);

        }

        //        return false;
    }

    return false;
}

void LatestVersionChecker::modalStateFinished (int result,
                                               URL& newVersionToDownload,
                                               const String& extraHeaders,
                                               File appParentFolder)
{



        if(result==1){
            const String link(newVersionToDownload.toString(false));
            Process::openDocument(link, "");
        }
        else if (result == 2){
            DownloadNewVersionThread::performDownload (*this, newVersionToDownload, extraHeaders, appParentFolder);
        }
        //        else
        //            askUserForLocationToDownload (newVersionToDownload, extraHeaders);



}





void LatestVersionChecker::timerCallback()
{
    
    
    if (hasAttemptedToReadWebsite)
    {
        bool restartTimer = true;
        if (jsonReply.isObject()){
            restartTimer = processResult (jsonReply, newRelativeDownloadPath);
            hasEnded = true;
            jassert(!restartTimer);
            LOG(juce::translate("has checked for new version"));
        }
        
        hasAttemptedToReadWebsite = false;
        
        if (restartTimer){
            LOGE(juce::translate("can't check for new version are you offline?"));
            hasEnded = true;
            if(statusCode!=0)startTimer(500);
            else stopTimer();
        }
        else{
            stopTimer();
        }
    }
    else
    {
        stopTimer();
        startThread (3);
    }
}

void LatestVersionChecker::run()
{
    checkForNewVersion();
}
#endif // ENGINE_HEADLESS
