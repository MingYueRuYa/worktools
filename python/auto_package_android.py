# -*- coding:utf-8 -*-
from selenium import webdriver
from selenium.common import TimeoutException
from selenium.webdriver.chrome.service import Service
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
from selenium.webdriver.common.alert import Alert
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

	def __init__(self, url):
		self.driver = self.__start_chrome__(url)

	def __start_chrome__(self, url):
		chrome_path = os.getcwd() + '/chromedriver.exe'
		driver = webdriver.Chrome(service=Service(chrome_path))
		driver.implicitly_wait(10)
		driver.start_client()
		driver.get(url)
		return driver

	def load_url(self):
		exists_title = ['诺捷-移动开发', '数字化工作台', 'Digital Workbench']

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
		self.driver.switch_to.frame('yonbip_login_id')
		switch_label = self.driver.find_element(By.XPATH, "//label[@id='switchLabel']")
		if switch_label is None:
			print("Not find switch label")
			return
		else:
			print("find element")
		switch_label.click()

		input_username = self.driver.find_element(By.ID, 'username')
		input_username.clear()
		input_username.send_keys('phone_number')

		input_passwd = self.driver.find_element(By.ID, 'password')
		input_passwd.clear()
		input_passwd.send_keys('password')

		btn_login = self.driver.find_element(By.ID, 'submit_btn_login')
		btn_login.click()
		self.driver.switch_to.default_content()

	def close_alert(self):
		iframes = self.driver.find_elements(By.CLASS_NAME, "developer-iframe")
		self.driver.switch_to.frame(iframes[0])
		try:
			elements = self.driver.find_elements(By.CLASS_NAME, 'intro-head')
			ele = elements[0].find_element(By.TAG_NAME, 'svg').find_element(By.TAG_NAME, 'g')
			ele.click()
		except:
			print("not find alert dialog")
		self.driver.switch_to.default_content()

	def search_mobile_dev(self):
		iframes = self.driver.find_elements(By.CLASS_NAME, "developer-iframe")
		self.driver.switch_to.frame(iframes[0])
		css_selector_str = '[data-analyfun-name="移动应用开发"]'
		element = self.find_element_with_try(self.driver, By.CSS_SELECTOR, css_selector_str, 3)
		if element is None:
			print('not find elment')
		else:
			element.click()

		self.driver.switch_to.default_content()

	def switch_mobile_package(self):
		iframes = self.driver.find_elements(By.CLASS_NAME, "developer-iframe")
		self.driver.switch_to.frame(iframes[0])
		applist_div = self.driver.find_element(By.CLASS_NAME, 'section-applist')
		elements = applist_div.find_element(By.CLASS_NAME, 'content-wrap')
		divs = elements.find_elements(By.CLASS_NAME, 'app-item')
		while True:
			if len(divs) < 2:
				time.sleep(2)
				divs = elements.find_elements(By.CLASS_NAME, 'app-item')
			else:
				break

		divs[2].click()
		# 或者打印其他属性
		self.driver.switch_to.default_content()

		# 需要再找一遍，不知道原因
		applist_div = self.driver.find_element(By.CLASS_NAME, 'section-applist')
		elements = applist_div.find_element(By.CLASS_NAME, 'content-wrap')
		divs = elements.find_elements(By.CLASS_NAME, 'app-item')
		while True:
			if len(divs) < 2:
				time.sleep(2)
				divs = elements.find_elements(By.CLASS_NAME, 'app-item')
			else:
				break
		divs[1].click()
		tag_li = self.driver.find_element(By.CSS_SELECTOR, '[data-analybtn-id="go-mobile-pack-page"]')
		tag_li.click()

	def contains_title(self, title_list: []):
		def _predicate(driver):
			return driver.title in title_list

		return _predicate

	def find_element_with_try(self, driver, id, element, loop_time):
		wait = WebDriverWait(driver, 3)  # 创建WebDriverWait对象，设置等待时间为10秒
		loop = 0
		find_element = None
		while loop_time > loop:
			try:
				find_element = wait.until(EC.presence_of_element_located((id, element)))
				find_element.click()
				break
			except:
				print("元素未找到，等待1秒...")
				loop += 1
				time.sleep(1)
		return find_element

	def print_all_element(self):
		iframes = self.driver.find_elements(By.CLASS_NAME, "developer-iframe")
		self.driver.switch_to.frame(iframes[0])
		# elements = self.driver.find_elements(By.XPATH, "//*")
		applist_div = self.driver.find_element(By.CLASS_NAME, 'section-applist')
		elements = applist_div.find_element(By.CLASS_NAME, 'content-wrap')
		divs = elements.find_elements(By.CLASS_NAME, 'app-item')
		while True:
			if len(divs) < 2:
				time.sleep(2)
				divs = elements.find_elements(By.CLASS_NAME, 'app-item')
			else:
				break
		# ele = divs[1].find_element(By.CLASS_NAME, 'item-main-box')
		# ele.click()
		# ele = elements[0].find_element(By.TAG_NAME, 'svg').find_element(By.TAG_NAME, 'g')
		# ele.click()

		# 打印每个元素的文本或其他属性
		for element in divs:
			try:
				print('------------------')
				print(element.get_attribute('innerHTML'))
				print('------------------')
			except:
				# print('not have text attribute')
				pass
		divs[2].click()
		# 或者打印其他属性
		# print(element.get_attribute("attribute_name"))
		self.driver.switch_to.default_content()

		applist_div = self.driver.find_element(By.CLASS_NAME, 'section-applist')
		elements = applist_div.find_element(By.CLASS_NAME, 'content-wrap')
		divs = elements.find_elements(By.CLASS_NAME, 'app-item')
		while True:
			if len(divs) < 2:
				time.sleep(2)
				divs = elements.find_elements(By.CLASS_NAME, 'app-item')
			else:
				break
		divs[1].click()
		tag_li = self.driver.find_element(By.CSS_SELECTOR, '[data-analybtn-id="go-mobile-pack-page"]')
		tag_li.click()

	def package_config(self):
		# Android 平台
		ele = self.driver.find_element(By.CLASS_NAME, 'flex-col-center-box.platform-info-box')
		ele.click()
		# Android AAB格式
		btn = self.driver.find_element(By.CLASS_NAME, 'wui-switch.wui-switch-sm.wui-switch-span')
		btn.click()
		# 类型 正式版
		label = self.driver.find_elements(By.CLASS_NAME, 'wui-radio')[1]
		label.click()
		# Android CPU架构
		cpu_label = self.driver.find_element(By.CLASS_NAME, 'radio-item.wui-radio')
		cpu_label.click()
		# 备注
		mark_notes = self.driver.find_element(By.CLASS_NAME, 'wui-textarea.wui-input.md')
		mark_notes.send_keys('This packing operation is done by a machine.')
		# 打包按钮
		btn = self.driver.find_element(By.CLASS_NAME, 'package-btn')

	# TODO: 测试环境下先不要打包
	# btn.click()

	def download_package(self):
		# 1 获取打包的版本
		# 2、轮询打包是否完成 看打包的文字是否变为‘开始打包’
		btn = self.driver.find_element(By.CLASS_NAME, 'package-btn')
		# 最长等待时间10分钟
		wait_count = 10 * 60
		loop_count = 0
		while True:
			if loop_count * 3 > wait_count:
				print('wait package timeout...')
				return
			text = btn.get_attribute('data-analybtn-name')
			if text == '开始打包':
				# 打包结束
				break
			else:
				print('waiting package...')
				time.sleep(3)
				loop_count += 1
		# 3、获取下载的链接
		try:
			table = self.driver.find_element(By.CLASS_NAME, 'wui-table-tbody')
			# 只需要第一个
			tr_datas = table.find_elements(By.CLASS_NAME, 'wui-table-row')
			# 找到包含下载链接的div
			div_data = tr_datas[0].find_element(By.CLASS_NAME, 'url-address.flex-col-center-box')
			a_tag = div_data.find_element(By.TAG_NAME, 'a')
			download_url = a_tag.get_attribute('href')
			print(download_url)
		except:
			print('not find target element')

	def __delete__(self, instance):
		cookies = self.driver.get_cookies()
		savecookies(cookies)


if __name__ == "__main__":
	package = AutoPackPage(url)
	package.load_url()
	package.input_passwd()
	# package.print_all_element()
	time.sleep(10)
	package.close_alert()
	package.search_mobile_dev()
	# time.sleep(10)
	# package.print_all_element()
	package.switch_mobile_package()
	package.package_config()
	input('enter return')
