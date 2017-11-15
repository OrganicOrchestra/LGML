
import os,platform;
from PyUtils import *
import json




configPath = os.path.abspath(os.path.join(__file__,os.pardir,'lgml_build_cfg.json'))


def saveConfig(cfg):
	with open(configPath,'w') as f:
		cfg = json.dump(cfg,f,indent=4)
		return cfg


def getSavedConfig():
	if(os.path.exists(configPath)):
		with open(configPath,'r') as f:
			cfg = json.load(f)
			return cfg
	else:
		print ('no config found')
		return None

def exportToOwncloud(builder,exportedPath):
	from PyUtils import OwncloudUtils
	basePath = "DEVSPECTACLES/Tools/LGML/App-Dev/dist/bleedingEdge/"#+ProJucerUtils.getXmlVersion()
	OwncloudUtils.makeDirIfNotExistent(basePath)
	exportedFile = os.path.basename(exportedPath)
	ownCloudPath = os.path.join(basePath,exportedFile)
	OwncloudUtils.sendToOwnCloud(exportedPath,ownCloudPath)
	OwncloudUtils.sendToOwnCloud(configPath,ownCloudPath+".cfg")


if __name__ == "__main__":
	
	import argparse
	import multiprocessing
	parser = argparse.ArgumentParser(description='python util for building and exporting LGML')
	parser.add_argument('--configure', action='store_true',
	                    help='build it',default = False)

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
	parser.add_argument('--configuration',help='build configuration name ', default='Debug')
	parser.add_argument('--version','-v',help='return current version ', action='store_true',default=False)
	

	args = parser.parse_args()

	if args.version==True:
		print(ProJucerUtils.getXmlVersion());
		exit()

# try to find os
	if not args.os:
		curOs = os.name
		curPlatform = platform.system()

		if curPlatform=='Linux':
			args.os = 'linux'
		elif curPlatform=='Darwin':
			args.os = 'osx'

		else:
			raise NameError('platform not supported : ',curOs,curPlatform)



	builder = None
	defaultCfg = {
	"build_os" : args.os,
	"build_cfg_name":args.configuration,
	"njobs": min(8,multiprocessing.cpu_count()),
	"lgml_root_path" : os.path.abspath(os.path.join(__file__,os.pardir,os.pardir)),
	"export_path" : args.exportpath,
	"version" : ProJucerUtils.getXmlVersion(),
	"git_sha" : gitUtils.getGitSha()
	}

	if args.configure==False:
		savedCfg = getSavedConfig();
		if savedCfg:
			if (savedCfg["build_os"]!=args.os):
				raise NameError("config made for another builder os (%s vs %s)"%(savedCfg["build_os"],args.os))
			defaultCfg = savedCfg


	if(args.os=='osx'):
		import osx;
		builder = osx.OSXBuilder(cfg=defaultCfg)
	elif args.os == 'linux':
		import linux;
		builder=  linux.LinuxBuilder(cfg = defaultCfg)
	else:
		raise NameError('no builder found for os :'+ args.os)


	if args.configure ==True:
		saveConfig(builder.cfg)

	print(json.dumps(builder.cfg,sort_keys=True,indent=4));
	if args.clean:
		saveConfig({})
		builder.cleanApp();
	if args.build:
		builder.buildApp();
	if args.package:
		ep = builder.cfg["export_path"]
		if ( ep is not None ) and ( not os.path.exists(ep)):
				os.makedirs(ep)
		builder.cfg["packaged_path"] = builder.packageApp(ep)
		saveConfig(builder.cfg)
	if args.export:
		exportToOwncloud(builder,builder.cfg["packaged_path"]);


