# -*- coding:utf-8 -*-

import subprocess
import time
import os
import logging
import random
import sys
import json


class SvnHelper:
	def __init__(self):
		self.log_file_name = 'svn_update.log'
		self.delete_log_file()
		logging.basicConfig(filename=self.log_file_name, level=logging.INFO,
		                    format='%(asctime)s - %(levelname)s - %(message)s')

	def delete_log_file(self):
		try:
			# 删除文件
			os.remove(self.log_file_name)
		except Exception as e:
			pass

	@property
	def top_dir(self):
		'''
		:return: 根据脚本的目录返回到顶级目录
		'''
		current_dir = os.getcwd()
		return current_dir + '/../../..'

	def cleanup_code(self):
		command = ['svn', 'cleanup', self.top_dir]
		print('-----------ready clenaup code-----------')
		print('-----------{}-----------'.format(command))
		logging.info('-----------ready clenaup code-----------')
		self.run_command(command, True)

	def update_code(self):
		command = ['svn', 'update', '--accept postpone', self.top_dir]
		print('-----------ready update code-----------')
		print('-----------{}-----------'.format(command))
		logging.info('-----------ready update code-----------')
		self.run_command(command, True)

	def run_command(self, command, debug):
		process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, shell=True)
		if debug:
			while True:
				output = process.stdout.readline()
				if output == b'' and process.poll() is not None:
					break
				if output:
					logging.info(output)
			if process.returncode == 0:
				print("execute {} successful".format(command))
				logging.info("execute {} successful".format(command))
			else:
				print("execute {} error".format(command))
				logging.error("execute {} error".format(command))
		else:
			output, error = process.communicate()
			if process.returncode == 0:
				print("execute {} successful".format(command))
				logging.info("execute {} successful".format(command))
			else:
				print("execute {} error".format(command))
				logging.error("execute {} error".format(command))

		return process.returncode

	def generate_config(self):
		'''
		生成配置文件
		{
		"app_name":"svn_update_code",
		"app_path_d":"",
		"app_desc":"",
		"app_param_d":"",
		"app_start_dir_d": "",
		"start_time":[{"hour":13, "min":30}, {"hour":23, "min":30}]
		}
		:return:
		'''
		d = {
			"app_name": "",
			"app_path": "",
			"app_desc": "",
			"app_param": "",
			"app_start_dir": "",
			"start_time": [{"hour": 13, "min": 30}, {"hour": 23, "min": 30}]
		}
		d["app_name"] = 'svn_update_code_' + str(int(time.time()))
		d["app_path"] =	os.getcwd() + '/../Python34/Python.exe'
		d["app_param"] = os.path.realpath(__file__)
		d["app_start_dir"] = os.getcwd()
		# 从21:00 ~ 8:00 随机索引一个时间
		hour_array = [21,22,23,0,1,2,3,4,5,6,7,8]
		min_array = [0, 30]
		hour = random.choice(hour_array)
		min = random.choice(min_array)
		d['start_time'] = [{'hour':hour, 'min':min}]
		with open('config.json', "w", encoding="utf-8") as file:
			file.write(json.dumps(d, indent=4, ensure_ascii=False))


if __name__ == "__main__":
	helper = SvnHelper()
	if len(sys.argv) == 1:
		helper.cleanup_code()
		helper.update_code()
	else:
		if sys.argv[1] == '-config':
			helper.generate_config()

