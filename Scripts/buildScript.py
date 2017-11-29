
import os,platform;
from PyUtils import gitUtils,ProJucerUtils
import json




configPath = os.path.abspath(os.path.join(__file__,os.pardir,'lgml_build_cfg.json'))


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
	basePath = "DEVSPECTACLES/Tools/LGML/App-Dev/dist/bleedingEdge/"+ProJucerUtils.getXmlVersion()
	exportedFile = os.path.basename(exportedPath)
	ownCloudPath = basePath + "/" + exportedFile

	#send opt
	OwncloudUtils.sendToOwnCloud(exportedPath,ownCloudPath)
	
	OwncloudUtils.sendToOwnCloud(configPath,ownCloudPath+".cfg")
	
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


	parser.add_argument('--os',help='os to use : osx, linux', default=None)
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
	"njobs": min(8,multiprocessing.cpu_count()),
	"lgml_root_path" : os.path.abspath(os.path.join(__file__,os.pardir,os.pardir)),
	"export_path" : args.exportpath,
	"version" : ProJucerUtils.getXmlVersion(),
	"git_sha" : gitUtils.getGitSha(),
	"arch" : args.arch,
	"binary_path" : None
	}

	if not args.configure:
		savedCfg = getSavedConfig();
		#if 
		if savedCfg:
			if not args.build and (args.package or args.export):
				for k in defaultCfg :
					if defaultCfg[k] is not None and k in savedCfg and defaultCfg[k]!=savedCfg[k]:
						raise NameError("config changed %s : was %s, is now %s)"%(k,savedCfg[k],defaultCfg[k]))
			defaultCfg = savedCfg

	# default Release
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

	# hack for windows
	elif defaultCfg["build_os"] == 'windows':
		from PyUtils.builderBase import BuilderBase;
		builder=  BuilderBase(cfg = defaultCfg)
		if( args.build):
			raise NameError("does'nt support windows building")
		cfg= builder.cfg
		builder.cfg["binary_path"] = os.path.join(cfg["lgml_root_path"],"Builds","VisualStudio2015",cfg["arch"],cfg["build_cfg_name"],"App","LGML.exe");
	
	else:
		raise NameError('no builder found for os :'+defaultCfg["build_os"])


	if args.configure :
		saveConfig(builder.cfg)

	print("config")
	print (json.dumps(builder.cfg,sort_keys=True,indent=4)) 

	# clean
	if args.clean:
		saveConfig({})
		builder.cleanApp()

	#build
	if args.build:
		builder.cfg["binary_path"] = builder.buildApp();

	# package
	if args.package:
		ep = builder.cfg["export_path"]
		if ( ep is not None ) and ( not os.path.exists(ep)):
				os.makedirs(ep)
		builder.cfg["packaged_path"] = builder.packageApp(ep)
		saveConfig(builder.cfg)

	#export
	if args.export:
		exportToOwncloud(builder);


