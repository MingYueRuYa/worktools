# -*- coding:utf-8 -*-
import ctypes
from ctypes import *

dll = ctypes.CDLL('./task_dll_Win32.dll')

wchar_t = ctypes.c_wchar
dll.StartTaskScheduler.argtypes = [ctypes.POINTER(wchar_t)]
dll.StartTaskScheduler.restype = ctypes.c_int

app_name = "测试任务"
app_name = ctypes.create_unicode_buffer(app_name)
result = dll.StartTaskScheduler(app_name)


