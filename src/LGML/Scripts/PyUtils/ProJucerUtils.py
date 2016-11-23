# THIS FILE IS BINDED TO RELATIVE LOCATIONS CHANGE THEM IF DISPLACING FILE

from writeSha import *

proJucerPath = "/Applications/Projucer.app/Contents/MacOS/Projucer"

JuceProjectPath = os.path.dirname(os.path.abspath(__file__));
JuceProjectPath = os.path.join(JuceProjectPath,"../../LGML.jucer");
JuceProjectPath = os.path.abspath(JuceProjectPath);



def hasValidProjucerPath():
	global proJucerPath
	return os.path.exists(proJucerPath)

def getIfNeeded(tmpFolder,credentials,osType):
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

def getVersion():
	global proJucerPath,JuceProjectPath
	return sh(proJucerPath+ " --get-version '" + JuceProjectPath+"'")[:-1]
	

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

