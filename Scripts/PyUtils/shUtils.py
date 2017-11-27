import os;
import subprocess

def sh(cmd,printIt=True,detach=False):
	def inSH(cmd,detach):
		if printIt : print(("exec : "+cmd));
		res =  subprocess.Popen(cmd, shell=True,stdout=subprocess.PIPE, universal_newlines=True)
		if(not detach):
			stdout_lines = iter(res.stdout.readline, "")
			for stdout_line in stdout_lines:
				yield stdout_line

			res.stdout.close()
			return_code = res.wait()
			if return_code != 0:
				raise subprocess.CalledProcessError(return_code, cmd)
	res = ""
	for l in inSH(cmd,detach):
		if l != '\n':
			res+=l
			if(printIt): print(l)
	return res