
import os;
import json;
import urllib;


# configuration  = "Release"
configuration  = "Debug"
bumpVersion = True
specificVersion = ""#0.1.1"





localExportPath = "/Users/Tintamar/Google_Drive/LGML/Builds/OSX/"



proJucerPath = "~/Dev/JUCE/ProJucer.app/Contents/MacOS/ProJucer"
JuceProjectPath = "../LGML.jucer"
xcodeProjPath = "../Builds/MacOSX/" 
executable_name = "LGML"
gitPath = "../../../"
appPath = xcodeProjPath+"build/"+configuration+"/"+executable_name+".app"

import xml.etree.ElementTree as ET
tree = ET.parse(JuceProjectPath)
root = tree.getroot()
projectVersion =root.attrib["version"]


def sh(cmd):
	print ("exec : "+cmd);
	res =  os.popen(cmd).read()
	print res
	return res

def generateProductBaseName():
	return executable_name+"_v"+projectVersion+"_"+configuration
	
	
	
def getVersion():
	return sh(proJucerPath+ " --get-version " + JuceProjectPath)

getVersion()

def cleanCode(sourceFolder):
	# sourceFolder = os.path.abspath(sourceFolder)
	sh(proJucerPath+ " --remove-tabs "+sourceFolder);
	sh(proJucerPath+ " --tidy-divider-comments "+sourceFolder);
	sh(proJucerPath+ " --trim-whitespace "+sourceFolder);
	
	if(bumpVersion):
		sh(proJucerPath+ " --bump-version " + JuceProjectPath)
	elif specificVersion:
		sh(proJucerPath+ " --set-version " +specificVersion+" "+ JuceProjectPath)

def gitCommit():
		if(bumpVersion or specificVersion!=""):
			sh("cd "+gitPath+"&& git add -A && git commit -m"+getVersion())
			sh(proJucerPath+" --git-tag-version "+JuceProjectPath)


def buildJUCE(JuceProjectPath):
	sh(proJucerPath+" -h")
	sh(proJucerPath+ " --resave "+JuceProjectPath)


def buildApp(xcodeProjPath,configuration):
	sh("cd "+xcodeProjPath+ " && "\
		
		+" xcodebuild -project LGML.xcodeproj" \
		+" -configuration "+configuration
		+" -jobs 4 ")

def createAppdmgJSON(appPath ,destPath):
	jdata =  {
  	"title": "Le Grand Mechant Loop",
  	"icon": "",
  	"background": "../Resources/grandlouloup.png",
  	"icon-size": 80,
	"contents": [
	{ "x": 448, "y": 304, "type": "link", "path": "/Applications" },
	{ "x": 192, "y": 304, "type": "file", "path": appPath}]
	}


	with open(destPath,'w') as f:
		json.dump(jdata,f)

def createDmg(exportFileBaseName,appPath):
	if sh("which appdmg")!="":
		jsonPath = "dmgExport.json"
		createAppdmgJSON(appPath,jsonPath)
		dmgPath = exportFileBaseName+".dmg"
		sh("rm -f \""+dmgPath+"\"")
		sh("appdmg "+jsonPath+" \""+dmgPath+"\"")
		sh("rm "+jsonPath)

	else:
		print "no appdmg exporter : using zip"
		sh("zip -rv9 \""+exportFileBaseName+".zip\" \""+appPath+"\"")


def sendToOwnCloud(originPath,destPath):
	credPath = os.path.dirname(os.path.abspath(__file__));
	credPath = os.path.join(credPath,"owncloud.password")
	
	with open(credPath) as json_data:
		credentials = json.loads(json_data.read())

	sh("curl -X PUT \"http://195.154.11.18/owncloud/remote.php/webdav/"+destPath+"\" --data-binary @\""+originPath+"\" -u "+credentials["pass"])

# print executeCmd(proJucerPath+ " --status "+ projectPath)

cleanCode("../Source");
buildJUCE(JuceProjectPath);
buildApp(xcodeProjPath,configuration);
localPath = localExportPath+generateProductBaseName();
createDmg(localPath,appPath);
ownCloudPath = "Projets/LGML/App-Dev/OSX/"+generateProductBaseName()+".dmg"
sendToOwnCloud(localPath+".dmg",urllib.pathname2url(ownCloudPath))
gitCommit()

