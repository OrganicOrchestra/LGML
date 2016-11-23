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


def updateModulesPathIfNeeded():
	global proJucerPath,JuceProjectPath

	possibleJucesPaths = map(os.path.abspath,map(lambda x:x+'/JUCE/modules',[os.environ['HOME']+'/Dev','../../','../../../','../../../../','../../../../../']))

	import xml.etree.ElementTree as ET
	tree = ET.parse(JuceProjectPath)
	root = tree.getroot()
	oldPath = root.findall('EXPORTFORMATS')[0].findall('XCODE_MAC')[0].findall('MODULEPATHS')[0].findall('MODULEPATH')[0].attrib['path']
	oldPath = os.path.abspath(os.path.join(JuceProjectPath,oldPath))
	
	if not os.path.exists(oldPath):
		print 'current module path doesnt exists',oldPath,'searching a valid one'
		newPath = ''
		for p in possibleJucesPaths:
			if os.path.exists(p):
				newPath = p
				break;
		if newPath:
			print 'found new module path updating projucer for : '+newPath
			for x in root.findall('EXPORTFORMATS')[0].findall('XCODE_MAC')[0].findall('MODULEPATHS')[0].findall('MODULEPATH'):
					x.attrib['path'] = newPath
			tree.write(JuceProjectPath)
		else:
			print 'not found modulepaths in : ', possibleJucesPaths;
			exit(1)
	



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

