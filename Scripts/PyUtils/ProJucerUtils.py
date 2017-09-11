# THIS FILE IS BINDED TO RELATIVE LOCATIONS CHANGE THEM IF DISPLACING FILE

# from .writeSha import *
import os
import collections

proJucerPath = "/Applications/Projucer.app/Contents/MacOS/Projucer"
proJucerCommand = proJucerPath

JuceProjectPath = os.path.dirname(os.path.abspath(__file__));
JuceProjectPath = os.path.join(JuceProjectPath,"../../LGML.jucer");
JuceProjectPath = os.path.abspath(JuceProjectPath);



def hasValidProjucerPath(osType):
	global proJucerPath
	
	from distutils.spawn import find_executable
	ex = find_executable('Projucer')
	if ex:
		proJucerPath = os.path.abspath(ex)
	else : 
		print(proJucerPath)
		if(osType=='osx'):
			proJucerPath = "/Applications/Projucer.app/Contents/MacOS/Projucer"
		elif(osType=='linux'):
			proJucerPath = os.environ['HOME']+'/Dev/Projucer/linux/Projucer'
		else:
			raise NameError("osType notsupported : %s"%osType)


	return os.path.exists(proJucerPath)

def getProjucerCommand(tmpFolder,osType):
	global proJucerPath,proJucerCommand
	# update command
	if hasValidProjucerPath(osType):
		proJucerCommand = proJucerPath
		# headless server support
		if osType=='linux' and not 'DISPLAY' in os.environ:
			proJucerCommand = 'xvfb-run -a '+proJucerPath  ## -e /dev/stdout for debug



def getValidPath(pathToSearch,relativesPaths=None):
	relativesPaths = relativesPaths or ['.','..','../..','../../..','../../../..','../../../../..',os.environ['HOME'],os.environ['HOME']+'/Dev','../Dev']
	relativesPaths = [os.path.join(x,pathToSearch) for x in relativesPaths]
	possiblePaths = list(map(os.path.abspath,relativesPaths))
	for p in possiblePaths:
		if os.path.exists(p):
			return p
	print('not found paths in : ', possiblePaths);
	exit(1)

def updatePathsIfNeeded(osType):
	global JuceProjectPath

	import xml.etree.ElementTree as ET
	tree = ET.parse(JuceProjectPath)
	root = tree.getroot()
	hasChanged = False

	XMLOSTag = {'osx':'XCODE_MAC','linux':'LINUX_MAKE'}[osType]
	vstFolderTag = 'vst3Folder'
	if osType=='linux':
		vstFolderTag = 'vstFolder'

	# updateModule
	oldModulePath = root.findall('EXPORTFORMATS')[0].findall(XMLOSTag)[0].findall('MODULEPATHS')[0].findall('MODULEPATH')[0].attrib['path']
	if( not os.path.isabs(oldModulePath)):
		oldModulePath = os.path.abspath(os.path.join(JuceProjectPath,oldModulePath))
	
	if not os.path.exists(oldModulePath):
		print('current module path doesnt exists',oldModulePath,'searching a valid one')
		newPath = getValidPath('JUCE/modules')
		if newPath:
			hasChanged = True;
			print('found new module path updating projucer for : '+newPath)
			for x in root.findall('EXPORTFORMATS')[0].findall(XMLOSTag)[0].findall('MODULEPATHS')[0].findall('MODULEPATH'):
					x.attrib['path'] = newPath
		

	if hasChanged : tree.write(JuceProjectPath)

			
			
def syncFileHierarchy():
	import xml.etree.ElementTree as ET
	tree = ET.parse(JuceProjectPath)
	mainGroup = tree.findall('MAINGROUP')[0]
	projDir = os.path.abspath(os.path.join(JuceProjectPath,os.pardir))
	def findFile(fname):
		root = projDir+'/Source'
		for dp, dn, filenames in os.walk(root):
			for f in filenames :
				if os.path.basename(f) == os.path.basename(fname):
					return os.path.join(dp, f)[len(projDir)+1:] 
		print (fname+'not found')
		return ''
		
	def setFromFilePath(ad,n,idx=0):
		
		if(idx<len(ad)-1):
			for l in n.findall('GROUP'):
				if l.attrib['name']==ad[idx]:
					setFromFilePath(ad,l,idx+1)
					return
			# not found
			nc = ET.Element('GROUP',{'name':ad[idx]})
			n.append(nc)
			setFromFilePath(ad,nc,idx+1)
		if(len(ad)-1==idx):
			print ('adding'+ str(ad))
			n.append(ET.Element('FILE',{'name':ad[idx],'file':'/'.join(ad),'compile':"1" if ad[idx][-4:]=='.cpp' else "0",'ressource':"0"}))
	def scanGroup(g):
		for ig in g.findall('GROUP'):
			scanGroup(ig)
		for f in g.findall('FILE'):
			fname = f.attrib['file']
			fpath = os.path.join(projDir,fname)
			if not os.path.exists(fpath):
				trup=findFile(fname)
				if trup!='':
					ad = trup.split('/')
					print(trup)
					setFromFilePath(ad,mainGroup)
				g.remove(f)
	scanGroup(mainGroup)
	tree.write(os.path.join(projDir,'export.jucer'))

def getModules():
	import xml.etree.ElementTree as ET
	tree = ET.parse(JuceProjectPath)
	
	modules = tree.findall('MODULES')[0]
	res = []
	for m in modules.iter('MODULE'):
		res+=[m.attrib['id']]
	return res

def getVersion():
	return getXmlVersion();
	# global proJucerPath,JuceProjectPath
	# return sh(proJucerPath+ " --get-version '" + JuceProjectPath+"'")[:-1]
	
def getVersionAsList():
	""" last element will be suffix or empty if not applicable
	"""
	v= getXmlVersion()
	s = v.split('.')

	i =0
	suffix = ""
	lastNum = ""
	while i< len(s[-1]):
		if not s[-1][i].isdigit(): 	suffix += s[-1][i]
		else :  										lastNum+=s[-1][i]
		i+=1
	s[-1] = int(lastNum)
	s = list(map(int,s));
	s+=[suffix]
	return s


	


def getXmlVersion():
	import xml.etree.ElementTree as ET
	tree = ET.parse(JuceProjectPath)
	root = tree.getroot()
	return root.attrib["version"]


def formatCode(sourceFolder):
	global proJucerCommand
	# sh(proJucerPath+ " --remove-tabs "+sourceFolder);
	sh(proJucerCommand+ " --tidy-divider-comments "+sourceFolder);
	sh(proJucerCommand+ " --trim-whitespace "+sourceFolder);

def tagVersion():
	global proJucerCommand
	sh(proJucerCommand+ " --git-tag-version '"+ JuceProjectPath+"'")

def updateVersion(bumpVersion,specificVersion):
	global proJucerCommand,JuceProjectPath

	if(bumpVersion):
		sh(proJucerCommand+ " --bump-version '" + JuceProjectPath+"'")
	elif specificVersion and (specificVersion!=getXmlVersion()):
		sh(proJucerCommand+ " --set-version " +specificVersion+" '"+ JuceProjectPath+"'")

	tagVersion()
	writeSha();


def buildJUCE():
	global proJucerCommand
	# sh(proJucerCommand+" -h")
	sh(proJucerCommand+ " --resave '"+JuceProjectPath+"'")

#  allow to use this file as a simple version getter
# DO NOT CHANGE


def updateProjucer(osType,bumpVersion,specificVersion):
	if hasValidProjucerPath(osType):
		updatePathsIfNeeded(osType)
		updateVersion(bumpVersion,specificVersion);
		buildJUCE();
		return True
	return False


if __name__=="__main__":
	syncFileHierarchy()
	# print(getModules());

