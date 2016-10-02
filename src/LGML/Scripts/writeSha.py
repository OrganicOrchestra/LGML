import sys
import os
import time
from PyUtils import *


sha = sh("git rev-parse --verify HEAD")[:-1]

print "sha"
print sha

sourcePath = os.path.dirname(os.path.abspath(__file__));
sourcePath = os.path.join(sourcePath,"../Source/GitSha.h");
sourcePath = os.path.abspath(sourcePath);


date =  time.strftime("%d/%m/%y (%H:%M)")

with open(sourcePath,'w') as f:
	f.write('#define GIT_SHA  "'+sha+'"\n')
	f.write('#define GIT_SHA_SHORT  "'+sha[0:7]+'"\n')
	f.write('#define COMMIT_DATE  "'+date+'"\n')

