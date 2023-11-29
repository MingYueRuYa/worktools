# -*- coding:utf-8 -*-
"""
@file： svn_update_code.py
@description: 更新svn 代码
@author: 635672377@qq.com
@date：2023/11/15
"""

import subprocess
import time
import os
import logging
import random
import sys
import locale
from datetime import datetime
import json


class SvnHelper:
	def __init__(self):
		self.restart = False
		self.log_file_name = 'svn_update.log'
		self.delete_log_file()
		logging.basicConfig(filename=self.log_file_name, level=logging.INFO,
		                    format='%(asctime)s - %(levelname)s - %(message)s')
		self.svn_path = "svn.exe"

	def exit_svn(self):
		logging.info('-----------------svn update exit-----------------')

	def delete_log_file(self):
		try:
			# 删除文件
			os.remove(self.log_file_name)
		except Exception as e:
			pass

	def get_svn_path(self):
		'''
		:return: 返回项目自带的svn工具
		'''
		return self.svn_path

	def reset_restart(self):
		self.restart = False

	@property
	def top_dir(self):
		'''
		:return: 根据脚本的目录返回到顶级目录
		'''
		current_dir = os.getcwd()
		return current_dir + '/../../..'

	def cleanup_code(self):
		command = [self.get_svn_path(), 'cleanup', self.top_dir]
		logging.info('-----------ready clenaup code-----------')
		logging.info('-----------{}-----------'.format(command))
		self.run_command(command, True)

	def update_code(self):
		command = [self.get_svn_path(), '--accept=postpone', 'update', self.top_dir]
		logging.info('-----------ready update code-----------')
		logging.info('-----------{}-----------'.format(command))
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
					try:
						content = output.decode('utf-8')
						target_str = 'database is locked'
						if target_str in content:
							self.restart = True
							logging.warning('we ready to restart cleanup and update code')
							return 1
					except Exception as e:
						logging.warning('decode utf-8 format error')
						pass

			if process.returncode == 0:
				self.restart = False
				logging.info("execute {} successful".format(command))
			else:
				logging.error("execute {} error".format(command))
				return -1
		else:
			output, error = process.communicate()
			if process.returncode == 0:
				logging.info("execute {} successful".format(command))
			else:
				logging.error("execute {} error".format(command))
				logging.error("error code {} ".format(process.returncode))
				return -1

		return process.returncode

	def generate_config(self):
		'''
		生成配置文件
		{
        "version":0.1,
		"app_name":"svn_update_code",
		"app_path_d":"",
		"app_desc":"",
		"app_param_d":"",
		"app_start_dir_d": "",
		"start_time":"14:30"
		}
		:return:
		'''
		date = self.get_date()
		d = {
			"version": 0.2,
			"app_name": "",
			"app_path": "",
			"app_desc": "",
			"app_param": "",
			"app_start_dir": "",
			"start_time": "",
			'start_date':"",
			'end_date': "",
		}
		d["app_name"] = '自动更新代码_' + str(int(time.time()))
		d["app_path"] = 'Python.exe'
		d["app_param"] = os.path.realpath(__file__)
		d["app_start_dir"] = os.getcwd()
		d["start_date"] = date[0]
		d["end_date"] = date[1]
		# 从21:00 ~ 8:00 随机索引一个时间
		hour_array = [21, 22, 23, 0, 1, 2, 3, 4, 5, 6, 7, 8]
		min_array = [0, 30]
		hour = random.choice(hour_array)
		min = random.choice(min_array)
		d['start_time'] = [{'hour':hour, 'min':min}]
		with open('config.json', "w", encoding="utf-8") as file:
			file.write(json.dumps(d, indent=4, ensure_ascii=False))

	def get_date(self):
		# 获取系统的区域设置
		current_locale = locale.getdefaultlocale()

		now_time = datetime.now()
		max_time = datetime(2099,12,31)
		if len(current_locale) > 0 and 'en_US' == current_locale[0]:
			start_date = now_time.strftime('%m/%d/%Y')
			end_date = max_time.strftime('%m/%d/%Y')
		else:
			start_date = now_time.strftime('%Y/%m/%d')
			end_date = max_time.strftime('%Y/%m/%d')
		return (start_date, end_date)


if __name__ == "__main__":
	if len(sys.argv) >= 2:
		if sys.argv[1] == '-config':
			helper = SvnHelper()
			helper.generate_config()
	else:
		helper = SvnHelper()
		while True:
			helper.reset_restart()
			ret_code = helper.cleanup_code()
			if ret_code == -1:
				break
			ret_code = helper.update_code()
			if ret_code == -1:
				break
			if not helper.restart:
				break
		helper.exit_svn()