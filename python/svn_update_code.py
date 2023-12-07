# -*- coding:utf-8 -*-
"""
@file： svn_update_code.py
@description: 更新svn 代码
@author: liushixiong@corp.netease.com
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
import ctypes
import platform


class SvnHelper:
	def __init__(self):
		self.result = True
		self.restart = False
		self.log_file_name = 'svn_update_{}.log'.format(str(time.time()))
		self.notification_app_name = 'notification.exe'
		self.update_code_script_name = 'update_code.bat'
		# self.delete_log_file()
		logging.basicConfig(filename=self.log_file_name, level=logging.INFO,
		                    format='%(asctime)s - %(levelname)s - %(message)s')
		self.svn_path = self.top_dir + "/tools/sunshine/Tools/TortoiseSvnCommandLine/svn.exe"

	def update_result(self):
		return self.result

	@property
	def config_file_name(self):
		return 'config.json'

	def x64_platform(self):
		arch, _ = platform.architecture()
		# 判断Python解释器的位数
		if arch == '32bit':
			return False
		elif arch == '64bit':
			return True
		else:
			return False

	def exit_svn(self):
		logging.info('-----------------svn update exit-----------------')
		self.save_update_result()
		self.start_task()
		logging.info('-----------------start notification app-----------------')

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
				self.result = True
				logging.info("execute {} successful".format(command))
			else:
				self.result = False
				logging.error("execute {} error".format(command))
				return -1
		else:
			output, error = process.communicate()
			if process.returncode == 0:
				logging.info("execute {} successful".format(command))
			else:
				self.result = False
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
		中文和英文系统，所需要的时间格式不同
		:return:
		'''
		date = self.get_date()
		d = {
			"version": 0.3,
			"app_name": "",
			"app_path": "",
			"app_desc": "自动更新代码任务",
			"app_param": "",
			"app_start_dir": "",
			"start_time": "",
			'start_date':"",
			'end_date': "",
			'app_top_dir':"",
			'app_log_path':"",
			'notification_task_name':''
		}
		d["app_name"] = '自动更新代码_' + str(int(time.time()))
		d["app_path"] = os.getcwd() + '/../Python34/Python.exe'
		d["app_param"] = os.path.realpath(__file__)
		d["app_start_dir"] = os.getcwd()
		d["start_date"] = date[0]
		d["end_date"] = date[1]
		d["code_top_dir"] = self.top_dir
		d["app_log_path"] = os.getcwd() + '/' + self.log_file_name
		d['notification_task_name'] = 'notifcation_task_name_' + str(int(time.time()))
		d['update_script_path'] = os.getcwd() + '/' + self.update_code_script_name
		# 从21:00 ~ 8:00 随机索引一个时间
		hour_array = [21, 22, 23, 0, 1, 2, 3, 4, 5, 6, 7, 8]
		min_array = [0, 30]
		hour = random.choice(hour_array)
		min = random.choice(min_array)
		d['start_time'] = [{'hour':hour, 'min':min}]
		with open(self.config_file_name, "w", encoding="utf-8") as file:
			file.write(json.dumps(d, indent=4, ensure_ascii=False))

	def save_update_result(self):
		with open(self.config_file_name, 'r', encoding="utf-8") as file:
			data = json.load(file)
			data['update_result'] = self.result

		with open(self.config_file_name, "w", encoding="utf-8") as file:
			file.write(json.dumps(data, indent=4, ensure_ascii=False))


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

	def start_task(self):
		if self.x64_platform():
			dll_handle = ctypes.CDLL('./task_dll_x64.dll')
		else:
			dll_handle = ctypes.CDLL('./task_dll_Win32.dll')

		wchar_t = ctypes.c_wchar
		dll_handle.StartTaskScheduler.argtypes = [ctypes.POINTER(wchar_t)]
		dll_handle.StartTaskScheduler.restype = ctypes.c_int

		app_notification_task = self.get_notification_task_name()
		app_notification_task = ctypes.create_unicode_buffer(app_notification_task)
		result = dll_handle.StartTaskScheduler(app_notification_task)
		if result == 0:
			logging.info("start notifcation task successful")
		else:
			logging.info("start notifcation task error")

	def get_notification_task_name(self):
		if not os.path.exists(self.config_file_name):
			return None
		with open(self.config_file_name, 'r', encoding="utf-8") as file:
			data = json.load(file)
		return data.get('notification_task_name', None)

	def delete_notification_task(self):
		command = ['task.exe', '-dn']
		process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, shell=True)
        # notice: 这里不采用dll方式，是因为还需要手动提权操作，不方便
		# if self.x64_platform():
		# 	dll_handle = ctypes.CDLL('./task_dll_x64.dll')
		# else:
		# 	dll_handle = ctypes.CDLL('./task_dll_Win32.dll')
		#
		# wchar_t = ctypes.c_wchar
		# dll_handle.DeleteTaskScheduler.argtypes = [ctypes.POINTER(wchar_t)]
		# dll_handle.DeleteTaskScheduler.restype = ctypes.c_int
		#
		# app_notification_task = self.get_notification_task_name()
		# if app_notification_task is None:
		# 	return
		# app_notification_task = ctypes.create_unicode_buffer(app_notification_task)
		# result = dll_handle.DeleteTaskScheduler(app_notification_task)
		# if result == 0:
		# 	logging.info("delete task successful")
		# else:
		# 	logging.info("delete task error")


	def create_notification_task(self):
		command = ['task.exe', '-cn']
		process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, shell=True)

		# if self.x64_platform():
		# 	dll_handle = ctypes.CDLL('./task_dll_x64.dll')
		# else:
		# 	dll_handle = ctypes.CDLL('./task_dll_Win32.dll')
		#
		# wchar_t = ctypes.c_wchar
		# dll_handle.CreateTaskScheduler.argtypes = [ctypes.POINTER(wchar_t)]
		# dll_handle.CreateTaskScheduler.restype = ctypes.c_int
		#
		# app_path = os.getcwd() + '/' + self.notification_app_name
		# app_path = ctypes.create_unicode_buffer(app_path)
		#
		# app_notification_name = self.get_notification_task_name()
		# app_notification_name = ctypes.create_unicode_buffer(app_notification_name)
		#
		# task_desc = 'this is for update code notification task'
		# task_desc = ctypes.create_unicode_buffer(task_desc)
		#
		# app_work_dir = os.getcwd()
		# app_work_dir = ctypes.create_unicode_buffer(app_work_dir)
		#
		# # 表示由计划任务调用，当前的工作目录
		# app_param = 'start_by_task ' + (os.getcwd() + '/' + self.config_file_name)
		# app_param = ctypes.create_unicode_buffer(app_param)
		#
		# user_name = ''
		# user_name = ctypes.create_unicode_buffer(user_name)
		#
		# passwd = ''
		# passwd = ctypes.create_unicode_buffer(passwd)
		#
		# # system权限
		# mode = 3
		#
		# # 这里的时间参数忽略
		# result = dll_handle.CreateTaskScheduler(app_path, app_notification_name, task_desc, app_work_dir, app_param, user_name, passwd, 0, 0, mode)
		# if result == 0:
		# 	logging.info("Create notifcation task successful")
		# else:
		# 	logging.info("Create notifcation task error")


if __name__ == "__main__":

	if len(sys.argv) >= 2:
		if sys.argv[1] == '-config':
			helper = SvnHelper()
			helper.delete_notification_task()
			helper.generate_config()
			helper.create_notification_task()
	else:
		helper = SvnHelper()
		print('-------------start to update code ---------------')
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
		print('-------------finished update code ---------------')
