# THIS FILE IS BINDED TO RELATIVE LOCATIONS CHANGE THEM IF DISPLACING FILE

from writeSha import *

proJucerPath = "/Applications/Projucer.app/Contents/MacOS/Projucer"

JuceProjectPath = os.path.dirname(os.path.abspath(__file__));
JuceProjectPath = os.path.join(JuceProjectPath,"../../LGML.jucer");
JuceProjectPath = os.path.abspath(JuceProjectPath);



def getVersion():
	return sh(proJucerPath+ " --get-version '" + JuceProjectPath+"'")[:-1]
	

def getXmlVersion():
	import xml.etree.ElementTree as ET
	tree = ET.parse(JuceProjectPath)
	root = tree.getroot()
	return root.attrib["version"]


def formatCode(sourceFolder):
	# sh(proJucerPath+ " --remove-tabs "+sourceFolder);
	sh(proJucerPath+ " --tidy-divider-comments "+sourceFolder);
	sh(proJucerPath+ " --trim-whitespace "+sourceFolder);

def tagVersion():
	sh(proJucerPath+ " --git-tag-version '"+ JuceProjectPath+"'")

def updateVersion(bumpVersion,specificVersion):
	if(bumpVersion):
		sh(proJucerPath+ " --bump-version '" + JuceProjectPath+"'")
	elif specificVersion:
		sh(proJucerPath+ " --set-version " +specificVersion+" '"+ JuceProjectPath+"'")

	tagVersion()
	writeSha();


def buildJUCE():
	sh(proJucerPath+" -h")
	sh(proJucerPath+ " --resave '"+JuceProjectPath+"'")

#  allow to use this file as a simple version getter
# DO NOT CHANGE
if __name__=="__main__":
	print getXmlVersion();

