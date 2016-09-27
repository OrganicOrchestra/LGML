file = '/Users/Tintamar/Downloads/UJS ft N3O2.lgml'
fileOut = '/Users/Tintamar/Downloads/UJS ft N3O2_mod.lgml'


import json
from ParseUtils import *
class Session : 

	def __init__(self,filePath):
		with open(filePath,'r') as f:
			self.jD = json.load(f);
			
			self.presets = self.jD['presetManager']['presets'];
			self.nodes = self.jD['nodeManager']['mainContainer']['nodes'];


	def getPresetNames(self):
		actualUIDS = []
		for j in self.presets:
			actualUIDS+=[j['filter']]
		return actualUIDS;

	def getOldPresets(self):
		pre = self.getPresetNames()
		old = []
		for p in pre:
			splitted = p.split('_')
			
			if len(splitted)>1:
				node = self.searchNodeForUID(splitted[1])
				
				if len(node) == 0:
					old+=[p]
				else:
					print node

			else:
				old+=[p]
		return old

	def removePreset(self,name):
		idx = getFirstIdxWithField(self.presets,"filter",name)
		if(idx>=0) : del self.presets[idx]


	def searchNodeForUID(self,uid):
		return searchNodeForUID(self.nodes,uid,'')

	def getPresetDoublons(self):
		return getDoublons(self.getPresetNames())


if __name__ == "__main__":
	file = '/Users/Tintamar/Downloads/UJS ft N3O2.lgml'
	fileOut = '/Users/Tintamar/Downloads/UJS ft N3O2_mod.lgml'
	fileSession = Session(file)
	print fileSession.getOldPresets()

