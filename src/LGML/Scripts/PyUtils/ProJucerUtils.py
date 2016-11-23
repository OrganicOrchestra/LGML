# THIS FILE IS BINDED TO RELATIVE LOCATIONS CHANGE THEM IF DISPLACING FILE

from writeSha import *

proJucerPath = "/Applications/Projucer.app/Contents/MacOS/Projucer"

JuceProjectPath = os.path.dirname(os.path.abspath(__file__));
JuceProjectPath = os.path.join(JuceProjectPath,"../../LGML.jucer");
JuceProjectPath = os.path.abspath(JuceProjectPath);



def hasValidProjucerPath():
	global proJucerPath
	return os.path.exists(proJucerPath)

def getProjucerIfNeeded(tmpFolder,credentials,osType):
	global proJucerPath

	if  not hasValidProjucerPath() :
		if not os.path.exists(tmpFolder):
			os.makedirs(tmpFolder)
		proJucerPath = os.path.join(tmpFolder,'Projucer')
		print proJucerPath , hasValidProjucerPath()
		zipPath = os.path.join(tmpFolder,'Projucer.zip')
		if not hasValidProjucerPath():
			sh("curl -k \"https://163.172.42.66/owncloud/remote.php/webdav/Tools/LGML/Projucer/"+osType+"/Projucer.zip\" -u "+credentials+" > "+zipPath,printIt=False)
			sh('tar -xzf '+zipPath+' -C '+tmpFolder)
			if not hasValidProjucerPath():
				print 'projucer download failed'
		else:
			print 'using cached projucer : '+proJucerPath

def getValidPath(pathToSearch):
	relativesPaths = ['.','..','../..','../../..','../../../..','../../../../..',os.environ['HOME'],os.environ['HOME']+'/Dev']
	relativesPaths = map(lambda x:os.path.join(x,pathToSearch),relativesPaths)
	possiblePaths = map(os.path.abspath,relativesPaths)
	for p in possiblePaths:
		if os.path.exists(p):
			return p
	print 'not found paths in : ', possiblePaths;
	exit(1)

def updatePathsIfNeeded():
	global JuceProjectPath

	import xml.etree.ElementTree as ET
	tree = ET.parse(JuceProjectPath)
	root = tree.getroot()

	# vst sdk
	oldVSTPath = root.findall('EXPORTFORMATS')[0].findall('XCODE_MAC')[0].attrib['vst3Folder']
	if not os.path.exists(oldVSTPath):
		print 'current VST path not valid'
		newVSTPath = getValidPath("SDKs/VST3 SDK")
		if newVSTPath:
			print 'found VST SDK at :' + newVSTPath
			root.findall('EXPORTFORMATS')[0].findall('XCODE_MAC')[0].attrib['vst3Folder'] = newVSTPath

	# updateModule
	oldModulePath = root.findall('EXPORTFORMATS')[0].findall('XCODE_MAC')[0].findall('MODULEPATHS')[0].findall('MODULEPATH')[0].attrib['path']
	oldModulePath = os.path.abspath(os.path.join(JuceProjectPath,oldModulePath))
	
	if not os.path.exists(oldModulePath):
		print 'current module path doesnt exists',oldModulePath,'searching a valid one'
		newPath = getValidPath('JUCE/modules')
		if newPath:
			print 'found new module path updating projucer for : '+newPath
			for x in root.findall('EXPORTFORMATS')[0].findall('XCODE_MAC')[0].findall('MODULEPATHS')[0].findall('MODULEPATH'):
					x.attrib['path'] = newPath
			tree.write(JuceProjectPath)
			
			
	



def getVersion():
	return getXmlVersion();
	# global proJucerPath,JuceProjectPath
	# return sh(proJucerPath+ " --get-version '" + JuceProjectPath+"'")[:-1]
	

def getXmlVersion():
	import xml.etree.ElementTree as ET
	tree = ET.parse(JuceProjectPath)
	root = tree.getroot()
	return root.attrib["version"]


def formatCode(sourceFolder):
	global proJucerPath
	# sh(proJucerPath+ " --remove-tabs "+sourceFolder);
	sh(proJucerPath+ " --tidy-divider-comments "+sourceFolder);
	sh(proJucerPath+ " --trim-whitespace "+sourceFolder);

def tagVersion():
	global proJucerPath
	sh(proJucerPath+ " --git-tag-version '"+ JuceProjectPath+"'")

def updateVersion(bumpVersion,specificVersion):
	global proJucerPath,JuceProjectPath
	if(bumpVersion):
		sh(proJucerPath+ " --bump-version '" + JuceProjectPath+"'")
	elif specificVersion:
		sh(proJucerPath+ " --set-version " +specificVersion+" '"+ JuceProjectPath+"'")

	tagVersion()
	writeSha();


def buildJUCE():
	global proJucerPath
	sh(proJucerPath+" -h")
	sh(proJucerPath+ " --resave '"+JuceProjectPath+"'")

#  allow to use this file as a simple version getter
# DO NOT CHANGE
if __name__=="__main__":
	print getXmlVersion();

