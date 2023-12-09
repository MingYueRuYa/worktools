# -*- coding:utf-8 -*-
"""
@file： install.py
@description: 安装头文件和lib
@author: 635672377@qq.com
@date：2023/11/28
"""

import os
import shutil


class App:
	def __init__(self):
		self.header_files = []
		self.log_dir = ['crash_generation_client', 'crash_generation_server', 'crash_report_sender']

	def read_log(self, log_path):
		if not os.path.exists(log_path):
			return
		with open(log_path, 'r', encoding='utf-8') as file:
			content = file.readline()
			target_str = '注意: 包含文件:'
			while content:
				content = content.strip()
				if target_str in content:
					content = content.replace(target_str, '').strip()
					if not content.startswith('C:'):
						self.header_files.append(content)
				content = file.readline()

	def intall_headers(self):
		"""
		1、lib文件
		2、头文件
		:return:
		"""
		root_dir = 'E:\\my_document\\platypus\\3rdparty\\breakpad\\src\\'
		dest_root_dir = 'E:\\breakpad\\include'
		for file in self.header_files:
			part_path = file.replace(root_dir, '')
			dest_root_file = dest_root_dir + '\\' + part_path
			dir_name = os.path.dirname(dest_root_file)
			if not os.path.exists(dir_name):
				os.makedirs(dir_name)
			shutil.copy(file, dest_root_file)
			print(file)

	def install_libs(self):
		pass

#E:\my_document\platypus\3rdparty\breakpad\src\client\windows\Debug\obj\crash_generation_client
#E:\my_document\platypus\3rdparty\breakpad\src\client\windows\Debug\obj\crash_report_sender

if __name__ == "__main__":
	app = App()
	app.read_log(
		'E:\\my_document\\platypus\\3rdparty\\breakpad\\src\\client\\windows\\Debug\\obj\\crash_report_sender\\crash_report_sender.log')
	app.intall_headers()
