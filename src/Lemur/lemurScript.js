lgml.ezra = new Object() ;
lgml.ezra.currentLooper = 0;
lgml.ezra.currentTrack = 0;
lgml.ezra.listeners = new Array();
lgml.ezra.listeners.push(0);


lgml.ezra.addListener = function(listener){
	
	// for(var i = 0 ;i < listeners.length ; i++){
	// 	if(listener == listeners[i]){
	// 		return;
	// 	}
	// }
	lgml.ezra.listeners.push(listener);
};

lgml.ezra.setCurrentLooper = function(l){
	lgml.ezra.currentLooper = l;
	callListeners();
};

lgml.ezra.setCurrentTrack = function(t){
	lgml.ezra.currentLooper = t;
	callListeners();
};

callListeners = function(){
	removeOldListeners();
	for(var i = 0 ; i < lgml.ezra.listeners.length ; i++){
		lgml.ezra.listeners[i].looperChanged(lgml.ezra.currentLooper,lgml.ezra.currentTrack);
	}
};

removeOldListeners = function(){
	newListener = new Array();
	for( var i = 0 ; i < lgml.ezra.listeners.size() ; i++){
		if(lgml.ezra.listeners[i] != undefined){
			newListener.push(lgml.ezra.listeners[i]);
		}
	}
	lgml.ezra.listeners = newListener;
};

fromMulti = function(args){
	for(var i = 0 ; i < args.length ; i++){
		if(args[i]>0)return i;
	}
	return -1;
};

// var recAndNotSelect = true;



	
onCtl_node_looper1_RecOrPlay_x = function( args){
	lgml.post(JSON.stringify(lgml.ezra.listeners));
	var num = fromMulti(args);
	lgml.ezra.setCurrentTrack(num);
	if(num>=0)lgml.node.looper1.tracks.elements[num].recOrPlay.trigger();
};

// onCtl_node_looper1_clear_x = function( args){
// 	var num = fromMulti(args);
// 	if(num>=0)lgml.node.looper1.tracks.elements[num].recOrPlay.trigger();
// };
// onCtl_node_looper1_tracks_x = function( args){
// 	var num = fromMulti(args);
// 	if(num>=0)lgml.node.looper1.tracks.elements[num].select.trigger();
// };


// on_node_looper1_selectTrack = function(args){
// 	lgml.post(JSON.stringify(args));
// }