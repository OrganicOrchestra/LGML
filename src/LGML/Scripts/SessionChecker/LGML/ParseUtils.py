def getName(node):
	for  n in node['parameters']:
		if n['controlAddress'] == "/name":
			return n['value']
	return "++"

def searchNodeForUID(root,uid,currentNS):
	res = []
	for n in root:
		curName = '/'+getName(n);
		if uid in n['uid']:
			res+=[currentNS+curName]
		if 'nodes' in n:
			l = searchNodeForUID(n['nodes'],uid,currentNS+curName)
			if l : res+=l;
	return res


def getFirstIdx(l,elem):
	i = 0;
	for k in l:
		if k==elem:
			return i;
		i+=1
	return -1;

def getFirstIdxWithField(l,field,elem):
	i = 0;
	for k in l:
		if k[field]==elem:
			return i;
		i+=1
	return -1;

def getDoublons(listToCheck):
	doublons = []
	for n in listToCheck:
		if n not in doublons:
			count = 0
			for nn in listToCheck:
				if(nn==n):
					count+=1

			if count!=1:
				doublons+=[n]
	return doublons