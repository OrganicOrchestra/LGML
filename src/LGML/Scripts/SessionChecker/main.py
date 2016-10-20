from LGML import *
file = '/Users/Tintamar/Downloads/UJS ft N3O2.lgml'
fileOut = '/Users/Tintamar/Downloads/UJS ft N3O2_mod.lgml'

fileSession = Session(file)
actualUIDS = fileSession.getPresetNames();
setUID = list(set(actualUIDS))
# print len(actualUIDS) , len(setUID);

doublons = fileSession.getPresetDoublons()

oldPresets = fileSession.getOldPresets()
print "doublons" ,doublons
