
import os;
import json;
import urllib;
import multiprocessing



import sys
pathToAdd = os.path.abspath(os.path.join(__file__,os.path.pardir,os.path.pardir))
sys.path.insert(1,pathToAdd)

from PyUtils import *
from PyUtils import ProJucerUtils

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



def generateProductBaseName():
	return executable_name+ "_v"+str(ProJucerUtils.getVersion())



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
	print 'creating dmg'
	os.chdir(os.path.abspath(os.path.join(__file__,os.path.pardir)))
	dmgbuild.build_dmg(exportFileBaseName,"Le Grand Mechant Loop",settings_file = 'dmgbuild_conf.py',defines={'app':appPath})
	print 'dmg done at :'+exportFileBaseName+'.dmg'
	return exportFileBaseName+'.dmg'


def getCredential():
	if ('OWNCLOUDPASS' in os.environ) and ('OWNCLOUDUSER' in os.environ):
		credentials = os.environ['OWNCLOUDUSER']+':'+os.environ['OWNCLOUDPASS']
	
	else:
		credPath = os.path.abspath(os.path.join(__file__,os.pardir,os.pardir));
		credPath = os.path.join(credPath,"owncloud.password")

		with open(credPath) as json_data:
			credentials = json.loads(json_data.read())["pass"]
		

	return credentials

def sendToOwnCloud(originPath,destPath):
	sh("curl -X PUT \"https://163.172.42.66/owncloud/remote.php/webdav/"+destPath+"\" --data-binary @\""+originPath+"\" -u "+getCredential()+" -k",printIt=False)


def buildAll():
	ProJucerUtils.updateModulesPathIfNeeded()
	ProJucerUtils.proJucerPath = 'dummy'
	ProJucerUtils.getIfNeeded(tmpFolder=os.path.abspath(os.path.join(__file__,os.pardir,'tmp')),credentials=getCredential(),osType="osx")

	if ProJucerUtils.hasValidProjucerPath():
		ProJucerUtils.updateVersion(bumpVersion,specificVersion);
		ProJucerUtils.buildJUCE();
	else:
		print 'not updating projucer'
	
	buildApp(xcodeProjPath,configuration,appPath,njobs,cleanFirst);

def exportAll():
	import dmgbuild
	localPath = localExportPath+generateProductBaseName();
	dmgPath = createDmg(localPath,appPath);
	for p in localExportPath2:
		sh("cp "+dmgPath+" "+p+generateProductBaseName()+".dmg")
	if sendToOwncloud:
		ownCloudPath = "Tools/LGML/App-Dev/OSX/"+generateProductBaseName()+".dmg"
		sendToOwnCloud(localPath+".dmg",urllib.pathname2url(ownCloudPath))
	# gitCommit()

if __name__ == "__main__":
	print sys.argv
	
	import argparse
	parser = argparse.ArgumentParser(description='python util for building and exporting LGML')
	parser.add_argument('--build', action='store_true',
	                    help='build it')
	parser.add_argument('--export', action='store_true',
	                    help='export it')

	args = parser.parse_args()
	if len(sys.argv)==1:
		args.build = True;
		args.export=True;

	if args.build:
		buildAll();
	if args.export:
		sendToOwncloud = True
		exportAll();


