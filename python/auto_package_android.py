from selenium import webdriver
from selenium.common import TimeoutException
from selenium.webdriver.chrome.service import Service
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
import time
import os
import json

url: str = 'https://dbox.diwork.com/#/'
file_name: str = 'diwork_cookies.txt'


def savecookies(cookies):
	path = os.getcwd() + '/cookies/'
	if not os.path.exists(path):
		os.makedirs(path)
	with open(path + file_name, 'w') as f:
		cookie = json.dumps(cookies)
		f.write(cookie)


class AutoPackPage(object):

	def __init__(self):
		self.driver = self.start_chrome()

	def start_chrome(self) -> object:
		chrome_path = os.getcwd() + '/chromedriver.exe'
		driver = webdriver.Chrome(service=Service(chrome_path))
		driver.implicitly_wait(10)
		driver.start_client()
		driver.get(url)
		driver.delete_all_cookies()
		driver.get(url)
		return driver

	def load_url(self):
		exists_title = ['诺捷-移动开发', '数字化工作台']

		wait = WebDriverWait(self.driver, 3)  # 创建WebDriverWait对象，设置等待时间为10秒
		while True:
			try:
				wait.until(self.contains_title(exists_title))  # 等待页面标题包含特定的文本
				break  # 如果页面加载完成，则跳出循环
			except TimeoutException:
				# 页面未加载完成时的操作，例如刷新页面或执行其他操作
				print('wait for web load finished...')
				self.driver.refresh()

	def input_passwd(self):
		pass

	def contains_title(self, title_list: []):
		def _predicate(driver):
			return driver.title in title_list

		return _predicate

	def __delete__(self, instance):
		cookies = self.driver.get_cookies()
		savecookies(cookies)


if __name__ == "__main__":
	package = AutoPackPage()
    package.start_chrome()
    package.load_url()
	input('enter return:')
