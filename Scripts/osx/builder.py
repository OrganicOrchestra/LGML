
import os;
import json;

import multiprocessing

import sys
pathToAdd = os.path.abspath(os.path.join(__file__,os.path.pardir,os.path.pardir))
sys.path.insert(1,pathToAdd)


from PyUtils import *
from PyUtils.builderBase import BuilderBase

class OSXBuilder (BuilderBase):
	
	xcodeProjPath = os.path.join(BuilderBase.rootPath,"Builds/MacOSX/")
	localExportPath = os.path.abspath(os.path.join(BuilderBase.rootPath,'Builds/MacOSX/build/'))+'/'
	# default configuration
	default_cfg  = {
	"arch" : "x86_64",
	}

	def __init__(self,cfg):
		BuilderBase.__init__(self,cfg)
		self.applyCfg(self.default_cfg)
		# self.localExportPath+=self.cfg['arch']+'/'

	def getPlatformName(self):
		return "osx"

	def cleanApp(self):
		self.removeOldApp()
		sh("cd "+self.xcodeProjPath+ " && "\
			+' xcodebuild -workspace LGMLWS.xcworkspace/  -scheme "LGML - App"' \
			+" -configuration "+ self.cfg["build_cfg_name"]
			+" clean")

	def buildApp(self):
		self.removeOldApp()
		sh("cd "+self.xcodeProjPath+ " && " \
			+' xcodebuild -workspace LGMLWS.xcworkspace/  -scheme "LGML - App"' \
			+" -configuration "+ self.cfg["build_cfg_name"]
			+" -arch "+self.cfg["arch"]
			+" -jobs "+str(self.cfg["njobs"]))
		return self.getBinaryPath();

	def packageApp(self,exportpath=None):
		localPath = os.path.join((exportpath or self.localExportPath),self.getNameWithVersion());
		dmgPath = self.createDmg(localPath);
		return dmgPath

	def getBinaryPath(self) :
		return os.path.join(self.xcodeProjPath,"build",self.cfg["build_cfg_name"],self.cfg["appName"]+".app")

	def removeOldApp(self):
		appPath = self.getBinaryPath()
		buildPath = os.path.abspath(os.path.join(appPath, os.pardir))
		print('//////////removing',buildPath)
		if len(buildPath)>10:
			sh("rm  -rf "+buildPath)

	def getArchReadable(self):
		if self.cfg['arch'] == "i386":
			return "32bit"
		elif self.cfg['arch'] == "x86_64":
			return "64bit"
		raise NameError("can't find current arch")
	

	def createDmg(self,exportFileBaseName):
		import dmgbuild
		import shutil
		print('creating dmg')
		betaSuffix = ""
		trueBinaryPath = self.getBinaryPath()
		if self.cfg["build_cfg_name"]=="Beta":
			betaBinaryPath = self.getBinaryPath().split(".app")[0]+'_%s_Beta.app'%(self.getVersion())
			shutil.copytree(trueBinaryPath,betaBinaryPath)
			trueBinaryPath = betaBinaryPath
			betaSuffix = "Beta"
		dmgPath = exportFileBaseName+'_(%s)_%s.dmg'%(self.getArchReadable(),betaSuffix)
		os.chdir(os.path.abspath(os.path.join(__file__,os.path.pardir)))
		dmgbuild.build_dmg(dmgPath,"Le Grand Mechant Loop",settings_file = 'dmgbuild_conf.py',defines={'app':trueBinaryPath})
		print('dmg done at :'+dmgPath)
		return dmgPath



		# gitCommit()

if __name__ == "__main__":
	opts = {}
	opts["build_cfg_name"] = "Beta"
	builder = OSXBuilder(opts)
	print (builder.cfg)
	builder.buildApp()
	builder.packageApp()
# 	global specificVersion
# 	print sys.argv
	
# 	import argparse
# 	parser = argparse.ArgumentParser(description='python util for building and exporting LGML')
# 	parser.add_argument('--build', action='store_true',
# 	                    help='build it')
# 	parser.add_argument('--export', action='store_true',
# 	                    help='export it')
# 	parser.add_argument('--beta', action='store_true',
# 	                    help='switch to beta version (only name affected for now)')
# 	parser.add_argument('--os',help='os to use : osx, linux', default='osx')

# 	args = parser.parse_args()
# 	needBuild = args.build
# 	needExport= args.export
# 	isBeta = args.beta
# 	if len(sys.argv)==1:
# 		needBuild = True;
# 		needExport=True;


# 	if isBeta:
# 		currentV  = ProJucerUtils.getVersionAsList()
# 		specificVersion = '.'.join(map(str,currentV[:-1]))+"beta"


		
# 	if needBuild:
# 		buildAll(args.os);
# 	if needExport:
# 		# send per default if used explicitly with export arg
# 		sendToOwncloud = args.export
# 		exportAll();


