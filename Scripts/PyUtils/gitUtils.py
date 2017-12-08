import sys
import os
import time

try:
	from .shUtils import *
except (ImportError,ValueError) as e:
	from shUtils import *


gitShaPath = os.path.dirname(os.path.abspath(__file__));
gitShaPath = os.path.join(gitShaPath,"../../Source/GitSha.h");
gitShaPath = os.path.abspath(gitShaPath);

LGMLDir = os.path.dirname(os.path.abspath(__file__));
LGMLDir = os.path.join(LGMLDir,os.pardir,os.pardir);


def getGitSha(short = True,globalSha = True):
	if globalSha:
		oldDir = os.getcwd()
		os.chdir(LGMLDir)
	sha= sh('git rev-parse --verify HEAD',printIt=False)[:-1].strip()
	if globalSha:
		os.chdir(oldDir)
	if(short):
		sha = sha[0:8]
	return sha
	



if __name__=='__main__':
	print(getGitSha(False))
	print(getGitSha())
	# sh("git add "+gitShaPath)
	# exit(1)

