
import os;
from PyUtils import *



#  default values


bumpVersion = False
sendToOwncloud = False
cleanFirst = False;




def exportToOwncloud(exportedPath,sendToOwncloud):
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
	
	import argparse
	import multiprocessing
	parser = argparse.ArgumentParser(description='python util for building and exporting LGML')
	parser.add_argument('--build', action='store_true',
	                    help='build it',default = True)
	parser.add_argument('--package', action='store_true',
	                    help='package it',default = False)
	parser.add_argument('--export', action='store_true',
	                    help='export it',default = False)
	parser.add_argument('--os',help='os to use : osx, linux', default=None)

	parser.add_argument('--exportpath',help='path where to put binary', default=None)

	parser.add_argument('--configuration',help='configuration to use ', default=None)

	parser.add_argument('--version','-v',help='return current version ', action='store_true',default=False)
	parser.add_argument('--clean',help='clean', action='store_true',default=False)

	args = parser.parse_args()

	if args.version==True:
		print(ProJucerUtils.getXmlVersion());
		exit()

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
	if not args.configuration:
		args.configuration = 'Debug'

	if args.export:
		args.package = True

	builder = None
	if(args.os=='osx'):
		import osx;
		builder = osx.OSXBuilder()
	elif args.os == 'linux':
		import linux;
		builder=  linux.LinuxBuilder()
	else:
		raise NameError('no builder found for os :'+ args.os)

	builder.configuration = {
	"build_os" : args.os,
	"build_cfg_name":args.configuration,
	"njobs": min(8,multiprocessing.cpu_count()),
	"lgml_root_path" : os.path.abspath(os.path.join(__file__,os.pardir,os.pardir)),
	"export_path" : args.exportpath

	}
	if args.clean:
		builder.clean();
	if args.build:
		builder.buildApp();
	if args.package:
		if ( args.exportpath is not None ) and ( not os.path.exists(args.exportpath)):
				os.makedirs(args.exportpath)
		packagePath = builder.packageApp()
	if args.export:
		exportAll(packagePath,sendToOwncloud=True);


