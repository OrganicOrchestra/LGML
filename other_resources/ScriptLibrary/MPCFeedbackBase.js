/////////////////////MPC DEFINITIONS

//PADS
var pads = [49,55,51,53,
			48,47,45,43,
			40,38,46,44,
			37,36,42,82];
			
function setMPCPadColor(pad, red, green) //red and green range = 0-7
{
	var r = parseInt(red);
	var g = parseInt(green);
	var col = ((r%8) << 3) | (g%8);
	local.sendCC(9,pads[pad],col);   
}; 

//////////////////////////////////////


//INIT
for(var i=0;i<16;i++) 
{
	setMPCPadColor(i,0,0);
}


//UPDATE
/*
var lastTime = lgml.getMillis();
var iter = 0;
onUpdate = function()
{
	
	var curTime = lgml.getMillis();
	if(curTime - lastTime < 20) return;
	
	//lgml.post("update");
	for(var i=0;i<16;i++) 
	{
		var tx = i%4;
		var ty = parseInt(Math.floor(i/4));
		var r = tx*8/4;
		var g =ty*8/4;
		r += iter;
		g += iter;
		setMPCPadColor(pads[i],r,g);
	}
	
	lastTime = curTime;
	iter++;
	
};
*/

//FUNCTIONS


//EVENTS
on_node_looper1_tracks_0_mute = function(value)
{
	setMPCPadColor(0,2,value*7);
};