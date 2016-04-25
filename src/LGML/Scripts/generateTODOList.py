import fnmatch
import os
import re


sourcePath = os.path.dirname(os.path.abspath(__file__));
sourcePath = os.path.join(sourcePath,"../Source");
sourcePath = os.path.abspath(sourcePath);



print sourcePath

# get sources files to scan
exts = ['*.h','*.cpp'] 
patterns = re.compile('|'.join(fnmatch.translate(p) for p in exts ))
matches = []
for root, dirnames, filenames in os.walk(sourcePath):
    matches.extend([os.path.join(root,f) for f in filenames if patterns.match(f)])



todoRe = re.compile("\s*//\s*TODO");
atRe = re.compile("\s*//\s*@");
commentRe = re.compile("\s*//.*");
blankRe = re.compile("^\s*$");

# create TODO list

TODOs = {}
for file in matches:
	with open(file,'r') as f:
		TODOCommentLine = -1;
		curTODO = "";
		idx = 0;
		for l in f.readlines():
			if TODOCommentLine ==-1 and (todoRe.match(l) or atRe.match(l)):
				TODOCommentLine = idx;
				curTODO+=l
			elif TODOCommentLine>=0 and commentRe.match(l):
				curTODO+=l
			elif TODOCommentLine>=0 and not blankRe.match(l):
				if not file in TODOs:
					TODOs[file] = {}	
				TODOs[file][TODOCommentLine] = curTODO
				curTODO=""
				TODOCommentLine=-1
			idx+=1


# generate TODO file

TODOFilePath = os.path.dirname(os.path.abspath(__file__));
TODOFilePath = os.path.join(sourcePath,"../GeneratedTODOs.txt");

with open(TODOFilePath,'w') as f:
	f.write("//////////////////////////////////////\n \
Auto - Generated TODO list from source code with Scripts/generateTODOList.py\n \
useless to overwrite !! but useful for rainy sunday code cleaning and improving\n \
TODOs in source code are not meant to be features but improvement and cleaning of CODE\n \
a discussion can be added after TODO comment line in source code (but not meant to be long) \n \
Of course this list can also be used to remove unnecessary TODOs...\n \
///////////////////////////////////////\n\n\n")

	for t in TODOs:
		f.write(t+"\n")
		for l in TODOs[t]:
			f.write("line : "+str(l)+"\n")
			f.write("\t"+TODOs[t][l]+"\n\n")


exit(1)



