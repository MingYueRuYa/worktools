#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Date    : 2024/11/07 22:33
# @Author  : shixiong.Liu (studycpp1024@gmail.com)

import os
import argparse


class App:
	def __init__(self) -> None:
		self._md_file_path = ''

	def run(self, args):
		self._parse_args(args)

	def _parse_args(self, args):
		parser = argparse.ArgumentParser(description='Tools for merge md files')
		parser.add_argument('-p', '--path', default='', type=str, help='md path')
		parse_args = parser.parse_args(args)
		self._md_file_path = parse_args.path

# import os

# paths = os.walk(r'./test')

# for path, dir_lst, file_lst in paths:
#     for file_name in file_lst:
#         print(os.path.join(path, file_name))


	def _merge_md_files(self):
		# µ±Ç°Ä¿Â¼
		dir_path = '.' if self._md_file_path == '' else self._md_file_path
		paths = os.walk(dir_path)
		for path, dir_lst, file_lst in paths:
			for file_name in file_lst:
				abs_path = os.path.join(path, file_name)
				if not os.path.isfile(abs_path):
					continue
				file_base_name, file_ext = os.path.splitext(file_name)
				if file_ext[1:] != 'md':
					continue
				with open(file_name, 'r') as file: 
					content = file.read() 
				
				with open('demo.md', 'rw') as file: 
					file.write('# ' + file_base_name + '\n')
					file.write(content);



if __name__ == "__main__":
	app = App()
	if not app.run(sys.argv[1:]):
		sys.exit(-1)