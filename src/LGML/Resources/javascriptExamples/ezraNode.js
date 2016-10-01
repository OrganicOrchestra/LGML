local.addIntParameter("selectedTrack","selected track",0,0,8);
local.addIntParameter("selectedLooper","selected looper",0,0,4);

on_node_looper1_selectTrack = function(v){
	lgml.post(v[])
local.selectedTrack = v[0];
}