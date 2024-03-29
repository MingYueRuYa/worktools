from selenium import webdriver
from selenium.webdriver.chrome.service import Service
from selenium.webdriver.common.by import By
import time
import os
import json


url: str = 'https://mp.weixin.qq.com/'

def start_chrome():
    chrome_path = os.getcwd() + '/chromedriver.exe'
    driver = webdriver.Chrome(service=Service(chrome_path))
    driver.implicitly_wait(10)
    driver.start_client()
    driver.get(url)
    driver.delete_all_cookies()

    path = os.getcwd() + '/cookies/'
    if not os.path.exists(path):
        os.makedirs(path)
    file_path = path + 'wechat_cookies.txt'
    cookies_dict = {}
    if os.path.exists(file_path):
        file = open(file_path, 'r', encoding='utf-8')
        cookies_dict = json.load(file)

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
    with open(path + 'wechat_cookies.txt', 'w') as f:
        cookie = json.dumps(cookies)
        f.write(cookie)


if __name__ == "__main__":
    chrome_deriver = start_chrome()

    time.sleep(30)

    # qq_login_btn = chrome_deriver.find_element(By.XPATH, "//div[@id='um']/p[2]/a")
    # print(qq_login_btn.get_attribute('href'))
    # qq_login_btn.click()

    # time.sleep(10)
    cookies = chrome_deriver.get_cookies()
    savecookies(cookies)


