
import os;
import json;

proJucerPath = "~/Dev/JUCE/ProJucer.app/Contents/MacOS/ProJucer"
JuceProjectPath = "../Blooper.jucer"


xcodeProjPath = "../Builds/MacOSX/" 
configuration  = "Debug"

exportPath = "/Users/Tintamar/Google_Drive/LGML/Builds/OSX/"

executable_name = "Blooper"
appPath = xcodeProjPath+"build/"+configuration+"/"+executable_name+".app"




def sh(cmd):
	print ("exec : "+cmd);
	print os.popen(cmd).read()

def cleanCode(sourceFolder):
	# sourceFolder = os.path.abspath(sourceFolder)
	sh(proJucerPath+ " --remove-tabs "+sourceFolder);
	sh(proJucerPath+ " --tidy-divider-comments "+sourceFolder);
	sh(proJucerPath+ " --trim-whitespace "+sourceFolder);

def buildJUCE(JuceProjectPath):
	sh(proJucerPath+" -h")
	sh(proJucerPath+ " --resave "+JuceProjectPath)


def buildApp(xcodeProjPath,configuration):
	sh("cd "+xcodeProjPath+ " && "\
		+" pwd &&" \
		+" xcodebuild -project Blooper.xcodeproj" \
		+" -configuration "+configuration
		+" -jobs 4 ")

def createAppdmgJSON(appPath ,destPath):
	jdata =  {
  	"title": "Le Grand Mechant Loop",
  	"icon": "",
  	"background": "dmgBack.png",
  	"icon-size": 80,
	"contents": [
	{ "x": 448, "y": 304, "type": "link", "path": "/Applications" },
	{ "x": 192, "y": 304, "type": "file", "path": appPath}]
	}


	with open(destPath,'w') as f:
		json.dump(jdata,f)

def copyDmg(exportFileBaseName,appPath):
	if sh("which appdmg")!="":
		jsonPath = "dmgExport.json"
		createAppdmgJSON(appPath,jsonPath)
		dmgPath = exportFileBaseName+".dmg"
		sh("rm -f \""+dmgPath+"\"")
		sh("appdmg "+jsonPath+" \""+dmgPath+"\"")
		sh("rm "+jsonPath)

	else:
		print "no appdmg exporter using zip"
		sh("zip -rv9 \""+exportFileBaseName+".zip\" \""+appPath+"\"")

# print executeCmd(proJucerPath+ " --status "+ projectPath)

cleanCode("../Source");
# buildJUCE(JuceProjectPath);
# buildApp(xcodeProjPath,configuration);
# copyDmg(exportPath+configuration+"/"+executable_name,appPath);



