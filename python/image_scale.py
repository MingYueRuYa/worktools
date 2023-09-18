# -*- coding: utf-8 -*-
"""
对图片扩大1.5倍
需要安装：pillow
"""

import sys
import os
from PIL import Image

def resolve_image(directory, scale=1.5):

    # 获取目录下所有文件名
    file_names = os.listdir(directory)

    # 遍历文件名
    for file_name in file_names:
        # 判断文件是否为图片文件
        if file_name.endswith(".jpg") or file_name.endswith(".png"):
            # 拼接图片文件的完整路径
            image_path = os.path.join(directory, file_name)

            # 打开图片
            image = Image.open(image_path)

            # 获取原始图片的尺寸
            width, height = image.size
            # 只有在宽或者高小于300才处理
            if not(width < 300 or height < 300):
                continue
            # 计算放大后的尺寸
            new_width = int(width * scale)
            new_height = int(height * scale)

            # 调整图片大小
            resized_image = image.resize((new_width, new_height))

            # 保存放大后的图片
            resized_image.save(image_path)


# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    if len(sys.argv) < 3:
        print("请输入路径和放大的倍数：如：python scale_image.py D:\image 1.5")
        sys.exit(1)

    folder_path = sys.argv[1]
    scale = float(sys.argv[2])
    resolve_image(folder_path, scale)