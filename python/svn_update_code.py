# -*- coding:utf-8 -*-

import subprocess
import time
import os


class SvnHelper:
	def __init__(self):
		pass

	@property
	def top_dir(self):
		'''
		:return: 根据脚本的目录返回到顶级目录
		'''
		current_dir = os.getcwd()
		return current_dir + '../../../server/server'

	def cleanup_code(self):
		print('-----------ready clenaup code-----------')
		self.run_command(['svn', 'cleanup', self.top_dir], True)

	def update_code(self):
		print('-----------ready update code-----------')
		self.run_command(['svn', 'update', self.top_dir], True)
		# svn_update = subprocess.Popen(['svn', 'update', self.top_dir], stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
		#                               shell=True)
		# # output, error = svn_update.communicate()
		#
		# while True:
		# 	output = svn_update.stdout.readline()
		# 	if output == b'' and svn_update.poll() is not None:
		# 		break
		# 	if output:
		# 		print(output)
		#
		# if svn_update.returncode == 0:
		# 	print("Code update successful")
		# else:
		# 	print("Code update failed")

	def run_command(self, command, debug):
		process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, shell=True)
		if debug:
			while True:
				output = process.stdout.readline()
				if output == b'' and process.poll() is not None:
					break
				if output:
					print(output)
		else:
			output, error = process.communicate()
			if process.returncode == 0:
				print("execute {} successful".format(command))
			else:
				print("execute {} error".format(command))

		return process.returncode


if __name__ == "__main__":
	helper = SvnHelper()
	helper.cleanup_code()
	helper.update_code()
