# -*- coding:utf-8 -*-
import time

from selenium import webdriver
from selenium.webdriver.chrome.service import Service
from selenium.webdriver.common.by import By
from selenium.webdriver.chrome.options import Options
import time
import os
import json

url = 'https://www.52pojie.cn/index.php'

def start_chrome():
    chrome_path = os.getcwd() + '/chromedriver.exe'
    options = Options()
    print(options.arguments)
    options.add_argument('--disable-blnk-features=AutomationControlled')
    driver = webdriver.Chrome(service=Service(chrome_path), options=options)
    driver.implicitly_wait(10)
    driver.start_client()
    driver.maximize_window()
    path = os.getcwd() + '/cookies/'
    if not os.path.exists(path):
        os.makedirs(path)
    file_path = path + 'cookies.txt'
    cookies_dict = {}
    if os.path.exists(file_path):
        file = open(file_path, 'r', encoding='utf-8')
        cookies_dict = json.load(file)

    # 先加载下URL
    driver.get(url)
    driver.delete_all_cookies()

    for item in cookies_dict:
        if 'expiry' in item:
            del item['expiry']
        if 'domain' in item:
            del item['domain']
        if 'httpOnly' in item:
            del item['httpOnly']
        if 'secure' in item:
            del item['secure']
        driver.add_cookie(item)

    driver.get(url)

    return driver


def savecookies(cookies):
    path = os.getcwd() + '/cookies/'
    if not os.path.exists(path):
        os.makedirs(path)
    with open(path + 'cookies.txt', 'w') as f:
        cookie = json.dumps(cookies)
        f.write(cookie)


if __name__ == "__main__":
    chrome_deriver = start_chrome()

    time.sleep(5)

    # qq_login_btn = chrome_deriver.find_element(By.XPATH, "//div[@id='um']/p[2]/a")
    # if qq_login_btn is not None:
    #     print('{} {}'.format('old href', qq_login_btn.get_attribute('href')))
    #     old_href = qq_login_btn.get_attribute('href')
    #     new_href = old_href.replace('index.php', '')
    #     chrome_deriver.execute_script("arguments[0].setAttribute('href', arguments[1])", qq_login_btn, new_href)
    #     print('{} {}'.format('new href', qq_login_btn.get_attribute('href')))
    #     qq_login_btn.click()
    # else:
    #     print('Not find checkin button')
    print(chrome_deriver.execute_script("return navigator.webdriver"))
    chrome_deriver.get_screenshot_as_file('1.png')

    time.sleep(10)
    cookies = chrome_deriver.get_cookies()
    savecookies(cookies)
    input('请输入回车键')

