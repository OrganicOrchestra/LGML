
import os;
import json;
import urllib;
import multiprocessing



import sys
pathToAdd = os.path.abspath(os.path.join(__file__,os.path.pardir,os.path.pardir))
sys.path.insert(1,pathToAdd)

from PyUtils import *



njobs = multiprocessing.cpu_count()

# configuration  = "Release"
configuration  = "Debug"
bumpVersion = False
sendToOwncloud = False
specificVersion = ""
cleanFirst = False;
localExportPath2 = [
# "/Volumes/Thor/OO\ Projets/OwnCloud/Tools/LGML/App-Dev/OSX/"
# ,"/Volumes/Pguillerme/Documents/LGML/"
];
architecture = "i386"





rootPath = os.path.abspath(os.path.join(__file__,os.pardir,os.pardir,os.pardir))

localExportPath = os.path.join(rootPath,'Builds/MacOSX/build/')
localExportPath = os.path.abspath(localExportPath)+"/"

xcodeProjPath = os.path.join(rootPath,"Builds/MacOSX/")
executable_name = "LGML"+("" if configuration=="Release" else "_"+configuration)
appPath = os.path.join(xcodeProjPath,"build",configuration,executable_name+".app")

isBeta = False

def generateProductBaseName():
	name =  executable_name+ "_v"+str(ProJucerUtils.getVersion())
	return name



def buildApp(xcodeProjPath,configuration,appPath,njobs,clean = False):
	if len(appPath)>10:
		sh("rm -rf "+appPath)

	if clean:
		sh("cd "+xcodeProjPath+ " && "\
		+" xcodebuild -project LGML.xcodeproj" \
		+" -configuration "+configuration
		+" clean")

	sh("cd "+xcodeProjPath+ " && "\
		+" xcodebuild -project LGML.xcodeproj" \
		+" -configuration "+configuration
		+" -arch "+architecture
		+" -jobs "+str(njobs))



def createDmg(exportFileBaseName,appPath):
	import dmgbuild
	print 'creating dmg'
	os.chdir(os.path.abspath(os.path.join(__file__,os.path.pardir)))
	dmgbuild.build_dmg(exportFileBaseName,"Le Grand Mechant Loop",settings_file = 'dmgbuild_conf.py',defines={'app':appPath})
	print 'dmg done at :'+exportFileBaseName+'.dmg'
	return exportFileBaseName+'.dmg'



def buildAll(osType):
	global specificVersion
	ProJucerUtils.updatePathsIfNeeded(osType)
	ProJucerUtils.proJucerPath = 'dummy'
	ProJucerUtils.getProjucerIfNeeded(tmpFolder=os.path.abspath(os.path.join(__file__,os.pardir,'tmp')),credentials=OwncloudUtils.getCredential(),osType="osx")

	if ProJucerUtils.hasValidProjucerPath():
		ProJucerUtils.updateVersion(bumpVersion,specificVersion);
		ProJucerUtils.buildJUCE();
	else:
		print 'not updating projucer'
	
	buildApp(xcodeProjPath,configuration,appPath,njobs,cleanFirst);

def exportAll():
	global appPath

	localPath = localExportPath+generateProductBaseName();
	dmgPath = createDmg(localPath,appPath);
	for p in localExportPath2:
		sh("cp "+dmgPath+" "+p+generateProductBaseName()+".dmg")
	if sendToOwncloud:
		ownCloudPath = "Tools/LGML/App-Dev/OSX/"+generateProductBaseName()+".dmg"
		OwncloudUtils.sendToOwnCloud(localPath+".dmg",urllib.pathname2url(ownCloudPath))
	# gitCommit()

if __name__ == "__main__":
	global specificVersion
	print sys.argv
	
	import argparse
	parser = argparse.ArgumentParser(description='python util for building and exporting LGML')
	parser.add_argument('--build', action='store_true',
	                    help='build it')
	parser.add_argument('--export', action='store_true',
	                    help='export it')
	parser.add_argument('--beta', action='store_true',
	                    help='switch to beta version (only name affected for now)')
	parser.add_argument('--os',help='os to use : osx, linux', default='osx')

	args = parser.parse_args()
	needBuild = args.build
	needExport= args.export
	isBeta = args.beta
	if len(sys.argv)==1:
		needBuild = True;
		needExport=True;


	if isBeta:
		currentV  = ProJucerUtils.getVersionAsList()
		specificVersion = '.'.join(map(str,currentV[:-1]))+"beta"


		
	if needBuild:
		buildAll(args.os);
	if needExport:
		# send per default if used explicitly with export arg
		sendToOwncloud = args.export
		exportAll();


