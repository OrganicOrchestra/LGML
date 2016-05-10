/// LGML 
/// this is an example script for Javascript and osc controller



///access to LGML environment:
/// in any javascripted object we have two accessible namespaces :
/// g (for global) that represent the whole LGML environment and let you change about any parameters
/// l (for local) that is a shortcut to local object class 
/// example the send method of javascriptOSCController named myController can be accessed via l.send(args) or g.OSC.myController(args)


/// local code :
/// each object can define callback function for different types of events so the API is less fixed
///  callback functions should start by on....()
/// in the OSC case any incoming OSC message try to run a function onAddressPatternWithCapsAfterEachSlashes
/// i.e /foo/bar 1 2 -> onFooBar(arg1,arg2)

/// a special method onAnyMsg can react to anything and parse address in javascript




/// special functions
//// g.post : post a message in LGML Logger


onFooBar  =function ( a,b){
		g.post(this + "osc arguments :" + a + "," + b);
		// any send call will send a message on OSC controller out port
		l.send("/lolo","l");
};

onAnyMsg = function (adress , args){
	g.post(JSON.stringify(args));
};



/// example of accessible functions from LGML
g.node.audioInNode.yPosition.set(50);

g.node.audioInNode.masterVolume.set(0);
g.time.play.t();