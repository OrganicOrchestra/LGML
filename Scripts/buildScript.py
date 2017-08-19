
import os;
from PyUtils import *



#  default values


bumpVersion = False
sendToOwncloud = False
specificVersion = ""
cleanFirst = False;


executable_name = "LGML"

rootPath = os.path.abspath(os.path.join(__file__,os.pardir,os.pardir))
isBeta = False

def generateProductBaseName():
	name =  executable_name+ "_v"+str(ProJucerUtils.getVersion())
	return name




def buildAll(osType,configuration):
	global specificVersion

	if not ProJucerUtils.updateProjucer(osType,bumpVersion,specificVersion):
		print ('not updating projucer')
	
	if osType=='osx':
		import osx
		osx.buildApp(configuration = configuration);#xcodeProjPath,configuration,appPath,njobs,cleanFirst);
	elif osType=='linux':
		import linux
		linux.buildApp(configuration = configuration)

def packageAll(osType,configuration,exportpath):
	baseName = generateProductBaseName()
	if osType=='osx':
		import osx
		exportedPath = osx.exportApp(baseName,configuration=configuration,exportpath=exportpath);
	elif osType=='linux':
		import linux
		exportedPath = linux.exportApp(baseName,configuration=configuration,exportpath=exportpath);
	else:
		raise NameError("os type not supported")



	return exportedPath

def exportAll(exportedPath,sendToOwncloud):


	if sendToOwncloud:
		if osType=='osx':
			ownCloudPath = "Tools/LGML/App-Dev/OSX/"+generateProductBaseName()+".dmg"
		elif osType=='linux':
			import platform
			distName = ''.join(platform.linux_distribution()[:-1])
			ownCloudPath = "Tools/LGML/App-Dev/Linux/"+distName+"/"+generateProductBaseName()+".tar.gz"
			
		OwncloudUtils.sendToOwnCloud(exportedPath,ownCloudPath)

	# gitCommit()

if __name__ == "__main__":
	print(sys.argv)
	
	import argparse
	parser = argparse.ArgumentParser(description='python util for building and exporting LGML')
	parser.add_argument('--build', action='store_true',
	                    help='build it',default = True)
	parser.add_argument('--package', action='store_true',
	                    help='package it',default = False)
	parser.add_argument('--export', action='store_true',
	                    help='export it',default = False)
	parser.add_argument('--beta', action='store_true',
	                    help='switch to beta version (only name affected for now)',default=True)
	parser.add_argument('--os',help='os to use : osx, linux', default=None)

	parser.add_argument('--exportpath',help='path where to put binary', default=None)

	parser.add_argument('--configuration',help='configuration to use ', default=None)

	args = parser.parse_args()



# try to find os
	if not args.os:
		import platform,os
		curOs = os.name
		curPlatform = platform.system()

		if curPlatform=='Linux':
			args.os = 'linux'
		elif curPlatform=='Darwin':
			args.os = 'osx'

		else:
			print('platform not supported : ',curOs,curPlatform)
			exit(1)

# default configurations for oses
	if args.os=='osx' and not args.configuration:
		args.configuration = 'Debug'
	elif args.os=='linux' and not args.configuration:
		args.configuration = 'Ubuntu'

	if args.export:
		args.package = True

	if args.beta:
		currentV  = ProJucerUtils.getVersionAsList()
		specificVersion = '.'.join(map(str,currentV[:-1]))+"beta"


		
	if args.build:
		buildAll(args.os,args.configuration);
	if args.package:
		packageAll(args.os,args.configuration,args.exportpath)
	if args.export:
		exportAll(args.os,args.configuration,sendToOwncloud=True);


