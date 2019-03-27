
import os,platform;
from PyUtils import gitUtils,ProJucerUtils
import json
import zipfile
import tempfile

rootLGMLPath = os.path.abspath(os.path.join(__file__,os.pardir,os.pardir)) 
configPath = os.path.abspath(os.path.join(rootLGMLPath,"Scripts",'lgml_build_cfg.json'))


def saveConfig(cfg):
	print("saving config")
	with open(configPath,'w') as f:
		cfg = json.dump(cfg,f,indent=4)
		return cfg


def getSavedConfig():
	print('loading savedCfg')
	if(os.path.exists(configPath)):
		with open(configPath,'r') as f:
			cfg = json.load(f)
			return cfg

	print ('no config found')
	return None


def exportToOwncloud(builder):
	from PyUtils import OwncloudUtils

	# send binaries
	exportedPath = builder.cfg["packaged_path"]
	basePath = "Spectacles/Tools/LGML/App-Dev/dist/bleedingEdge/"+ProJucerUtils.getXmlVersion()
	exportedFile = os.path.basename(exportedPath)
	ownCloudPath = basePath + "/" + exportedFile
	
	OwncloudUtils.sendToOwnCloud(exportedPath,ownCloudPath)
	# send zip
	zp = builder.cfg["zipped_path"]
	if zp is not None:
		OwncloudUtils.sendToOwnCloud(zp,basePath+"/"+os.path.basename(zp))
	else:
		raise NameError("no zip provided")


	#send opt
	OwncloudUtils.sendToOwnCloud(configPath,basePath+"/"+builder.getUID()+".cfg")

	preprocessor = builder.getPreprocessor()
	if preprocessor:
		with open(configPath+".preprocessor",'w') as preFp:
			preFp.write(preprocessor);
		OwncloudUtils.sendToOwnCloud(configPath+".preprocessor",ownCloudPath+".preprocessor")

	




if __name__ == "__main__":
	
	import argparse
	import multiprocessing
	parser = argparse.ArgumentParser(description='python util for building and exporting LGML')
	
	parser.add_argument('--configure', action='store_true',
	                    help='configure it (and save config locally)',default = False)

	parser.add_argument('--build', help='build it',
											action='store_true',default = False)

	parser.add_argument('--package', action='store_true',
											help='package it',default = False)

	parser.add_argument('--export', help='export it',
											action='store_true',default = False)

	parser.add_argument('--clean',help='clean',
											action='store_true', default=False)


	parser.add_argument('--os',help='os to use : osx, linux, windows', default=None)
	parser.add_argument('--packagesuffix',help='suffix to add', default=None)
	parser.add_argument('--exportpath',help='path where to put binary', default=None)
	parser.add_argument('--configuration',help='build configuration name ', default=None)
	parser.add_argument('--version','-v',help='return current version ', action='store_true',default=False)
	parser.add_argument('--arch', help='target architecture',default=None)
	

	args = parser.parse_args()

	if args.version==True:
		print(ProJucerUtils.getXmlVersion());
		exit()


	defaultCfg = {
	"build_os" : args.os,
	"build_cfg_name":args.configuration,
	"njobs": min(4,multiprocessing.cpu_count()),
	"lgml_root_path" : os.path.abspath(os.path.join(__file__,os.pardir,os.pardir)),
	"export_path" : args.exportpath,
	"version" : ProJucerUtils.getXmlVersion(),
	"git_sha" : gitUtils.getGitSha(),
	"arch" : args.arch,
	"binary_path" : None,
	"zipped_path" : None,
	"packagesuffix" : args.packagesuffix
	}

	if not args.configure:
		savedCfg = getSavedConfig();
		#if 
		if savedCfg:
			if not args.build and (args.package or args.export):
				for k in defaultCfg :
					if defaultCfg[k] is not None and k in savedCfg and defaultCfg[k]!=savedCfg[k]:
						raise NameError("config changed %s : was %s, is now %s : please rebuild the project)"%(k,savedCfg[k],defaultCfg[k]))
			defaultCfg = savedCfg
			defaultCfg["git_sha"] = gitUtils.getGitSha()

	# default to Release config
	if defaultCfg["build_cfg_name"] is None:
		defaultCfg["build_cfg_name"] = "Release"
	# auto detect os if not provided
	if not defaultCfg["build_os"] :
		# curOs = os.name
		curPlatform = platform.system()
		if curPlatform=='Linux':
			defaultCfg["build_os"] = 'linux'
		elif curPlatform=='Darwin':
			defaultCfg["build_os"] = 'osx'

	builder = None
	#osx
	if(defaultCfg["build_os"]=='osx'):
		import osx;
		builder = osx.OSXBuilder(cfg=defaultCfg)

	#linux
	elif defaultCfg["build_os"] == 'linux':
		import linux;
		builder=  linux.LinuxBuilder(cfg = defaultCfg)

	# fake builder for windows (see appveyor.yml)
	elif defaultCfg["build_os"] == 'windows':
		import windows
		builder=  windows.WindowsBuilder(cfg = defaultCfg)
		if( args.build):
			raise NotImplementedError("does'nt support windows building")

	else:
		raise NameError('no builder found for os :'+defaultCfg["build_os"])


	if args.configure :
		saveConfig(builder.cfg)

	print("config")
	builder.cfg["build_version_uid"] = builder.getShortUID();
	
	print (json.dumps(builder.cfg,sort_keys=True,indent=4)) 


	# clean
	if args.clean:
		saveConfig({})
		builder.cleanApp()

	#build
	if args.build:
		builder.fillVersionInfo();
		builder.cfg["binary_path"] = builder.buildApp();
		

	# package
	if args.package:
		ep = builder.cfg["export_path"]
		if  ep is not None :
			ep = ep.strip('\'" ')
			if  not os.path.exists(ep):
				os.makedirs(ep)

		pkgPath = builder.packageApp(ep)
		builder.cfg["packaged_path"]=pkgPath
		builder.cfg["packaged_name"]=os.path.basename(pkgPath)
		if (ep is not None):
			builder.cfg["zipped_path"] = builder.zipApp(ep);
		saveConfig(builder.cfg)


	#export
	if args.export:
		exportToOwncloud(builder);


