/// LGML 
/// this is an example script for Javascript in LGML



///access to LGML environment:
/// in any javascripted object we have two accessible namespaces :
/// "lgml" that represent the whole LGML environment and let you change about any parameters
/// "local" that is a shortcut to local object class (OSC or MIDI atm)
/// example the send method of javascriptOSCController named myController can be accessed via local.send(args) or lgml.OSC.myController(args)


/// example of accessible functions from LGML

// lgml.node.audioInNode.yPosition.set(150);
lgml.node.audioDeviceIn.masterVolume.set(.5);
// lgml.node.zouzou.audioOutNode.set("bambi");
lgml.time.play.trigger();


/// all enumered elements can be accessed via array named elements
// eg access to looper track 0 ==> lgml.node.looper.tracks.elements[0]
// eg numtracks = lgml.node.looper.tracks.elements.length



/// lgml parameter Changes
/// any function (starting with on) that can be parsed to a valid lgml controllable address is called on corresponding controllable change
/// i.e on_time_play 
var smoothedVolume;
var smoothFactor = .1;
on_node_audioInNode_masterVolume = function(a) {
	smoothedVolume = a*smoothFactor + (1.0-smoothFactor)*smoothedVolume;
	lgml.node.looper.tracks.elements[0].volume.set(smoothedVolume);	
};


/// lgml containerChanges
// listen to all params inside a container, then parsing can be made from relative address list passed (osc address splitted in list)

on_node_audioInNode = function(relativeAddressList,v) {
	if(relativeAddress[0]=="enabled"){
		lgml.node.looper.enabled.set(v);	
	};
};





///////////////////////
/// OSC
/// each object can define callback function for different types of events so the API is less fixed
///  control callback functions should start by onCtl....()
///  then corresponding object are adresses splited by "_" underscore
/// i.e /foo/bar 1 2 -> onCtl_foo_bar(arg1,arg2)

/// a special method onAnyMsg can react to anything and parse address in javascript
/// special functions
//// lgml.post : post a message in LGML Logger

onCtl_Foo_Bar  = function ( a , b ){
	lgml.post(this + "osc arguments :" + a + "," + b );
	// any send call will send a message on OSC controller out port
	local.send("/lolo","l");
};

onCtl_AnyMsg = function (adress , args){
	lgml.post(JSON.stringify(args));
};


//////////////////////////
/// MIDI
// example for script attached to MIDI controllers
// atm sending MIDI is not handled in LGML

// onCC on incoming CC
onCC = function(cc,value){

	lgml.post("CC : "+cc + " /" + value);
};

// onNote on incoming Note
onNote = function(Note,Velocity){
	if(Velocity>0){
		lgml.node.looper.tracks.elements[Note-40].volume.set(0);
		
	}
	else{
		lgml.post("Note : "+Note + " /" + Velocity);
	}
};

// onPitchWheel

onPitchWheel = function(value){

};




