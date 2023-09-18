# -*- coding:utf-8 -*-
"""
将文件的CRLF修改为LF
"""

import os
import sys


def modify_encoding(file_path):
	with open(file_path, 'r', newline='') as file:
		content = file.read()

	updated_content = content.replace('\r\n', '\n')

	with open(file_path, 'w', newline='') as file:
		file.write(updated_content)


def process_folder(folder_path):
	for root, dirs, files in os.walk(folder_path):
		for file in files:
			if file.endswith('.idl'):
				print('Processing ' + file)
				file_path = os.path.join(root, file)
				modify_encoding(file_path)

if __name__ == "__main__":
	if len(sys.argv) < 2:
		print("请输入路径")
		sys.exit(1)

	folder_path = sys.argv[1]
	process_folder(folder_path)