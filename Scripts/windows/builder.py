
import os;
import json;

import multiprocessing

import sys
pathToAdd = os.path.abspath(os.path.join(__file__,os.path.pardir,os.path.pardir))
sys.path.insert(1,pathToAdd)


from PyUtils import *
from PyUtils.builderBase import BuilderBase

class WindowsBuilder (BuilderBase):
	# default configuration
	default_cfg  = {
	"arch" : "Win32"
	}

	def __init__(self,cfg):
		BuilderBase.__init__(self,cfg)
		self.applyCfg(self.default_cfg)
		self.VSVersion = "VisualStudio2015"

	def getPlatformName(self):
		return "windows"


	def cleanApp(self):
		pass

	def buildApp(self):
		raise NotImplemented("don't support python based builds, see appveyor.yml")

	# use default method
	# def packageApp(self,exportpath=None):
	# 	raise NotImplemented("don't support python based package, see appveyor.yml")

	def getBinaryPath(self) :
		return os.path.join(self.cfg["lgml_root_path"],"Builds",self.VSVersion,self.cfg["arch"],self.cfg["build_cfg_name"],"App","LGML.exe")




if __name__ == "__main__":
	pass