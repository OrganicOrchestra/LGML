
from writeSha import *

proJucerPath = "/Applications/ProJucer.app/Contents/MacOS/ProJucer"
JuceProjectPath = "../LGML.jucer"

def getVersion():
	return sh(proJucerPath+ " --get-version " + JuceProjectPath)[:-1]
	




def formatCode(sourceFolder):
	# sh(proJucerPath+ " --remove-tabs "+sourceFolder);
	sh(proJucerPath+ " --tidy-divider-comments "+sourceFolder);
	sh(proJucerPath+ " --trim-whitespace "+sourceFolder);

def tagVersion():
	sh(proJucerPath+ " --git-tag-version "+ JuceProjectPath)

def updateVersion(bumpVersion,specificVersion):
	if(bumpVersion):
		sh(proJucerPath+ " --bump-version " + JuceProjectPath)
	elif specificVersion:
		sh(proJucerPath+ " --set-version " +specificVersion+" "+ JuceProjectPath)

	tagVersion()
	writeSha();


def buildJUCE():
	sh(proJucerPath+" -h")
	sh(proJucerPath+ " --resave "+JuceProjectPath)

if __name__=="__main__":
	tagVersion()

