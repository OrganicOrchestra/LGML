import os;
import subprocess

def sh(cmd,printIt=True):
	def inSH(cmd,printIt):
		if printIt : print("exec : "+cmd);
		res =  subprocess.Popen(cmd, shell=True,stdout=subprocess.PIPE, universal_newlines=True)
		stdout_lines = iter(res.stdout.readline, "")
		for stdout_line in stdout_lines:
			yield stdout_line

		res.stdout.close()
		return_code = res.wait()
		if return_code != 0:
			raise subprocess.CalledProcessError(return_code, cmd)

	res = ""
	for l in inSH(cmd,printIt):
		if l != '\n':
			res+=l
			if(printIt): print(l)
	return res


def start_thread(cmd,stdout = None):
	pipeOut = stdout
	if stdout is False:
		pipeOut = None
	elif stdout is None:
		pipeOut = subprocess.PIPE
	res =  subprocess.Popen(cmd, shell=True,stdout=pipeOut,stderr=pipeOut, universal_newlines=True)
	return res