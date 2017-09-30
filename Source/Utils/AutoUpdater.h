/*
  ==============================================================================

    AutoUpdater.h
    Created: 29 Sep 2017 3:07:42pm
    Author:  Martin Hermant

  ==============================================================================
*/

#pragma once

#include "../JuceHeaderCore.h"

class UpdaterDialogModalCallback;

//==============================================================================
class LatestVersionChecker  : private Thread,
private Timer
{
public:
    struct LGMLVersionTriple
    {
        LGMLVersionTriple();
        LGMLVersionTriple (int lgmlVersionNumber);
        LGMLVersionTriple (int majorInt, int minorInt, int buildNumber);

        static bool fromString (const String& versionString, LGMLVersionTriple& result);
        String toString() const;

        bool operator> (const LGMLVersionTriple& b) const noexcept;

        int major, minor, build;
    };

    //==============================================================================
    struct LGMLServerLocationsAndKeys
    {
        const char* updateSeverHostname;
        const char* publicAPIKey;
        int apiVersion;
        const char* updatePath;
    };

    //==============================================================================
    LatestVersionChecker();
    ~LatestVersionChecker();

    static String getOSString();

    URL getLatestVersionURL (String& headers, const String& path) const;
    URL getLatestVersionURL (String& headers) const;

    void checkForNewVersion();
    bool processResult (var reply, const String& downloadPath);

    bool askUserAboutNewVersion (const LGMLVersionTriple& version,
                                 const String& releaseNotes,
                                 URL& newVersionToDownload,
                                 const String& extraHeaders);

    void askUserForLocationToDownload (URL& newVersionToDownload, const String& extraHeaders);

    

    virtual Result performUpdate (const MemoryBlock& data, File& targetFolder);

protected:
    const LGMLServerLocationsAndKeys& getLGMLServerURLsAndKeys() const;
    int getProductVersionNumber() const;
    const char* getProductName() const;
    bool allowCustomLocation() const;

private:
    //==============================================================================
    friend class UpdaterDialogModalCallback;

    // callbacks
    void timerCallback() override;
    void run() override;
    void modalStateFinished (int result,
                             URL& newVersionToDownload,
                             const String& extraHeaders,
                             File appParentFolder);

    int statusCode;
    var jsonReply;
    bool hasAttemptedToReadWebsite;
    String newRelativeDownloadPath;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LatestVersionChecker)
};
