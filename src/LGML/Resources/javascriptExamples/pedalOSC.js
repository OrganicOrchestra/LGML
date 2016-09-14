onCtl_pedal_rec = function(v){

var loop =lgml.node.ezra.selectedLooper.get();
	if(loop==1){
		lgml.node.looper1.recOrPlay.trigger();
		lgml.post(loop);	
	}
	else if(loop==2){
		lgml.node.looper2.recOrPlay.trigger();
		lgml.post(loop);	
	}
	else if(loop==3){
		lgml.node.looper3.recOrPlay.trigger();
		lgml.post(loop);	
	}
	else if(loop==4){
		lgml.node.looper4.recOrPlay.trigger();
		lgml.post(loop);	
	}
};