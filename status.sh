#!/bin/bash

RED='\e[1;31m' # 红
RES='\e[0m'

GREEN='\033[32m'
GREEN_END='\033[0m'

function echo_red {
	echo -e "${RED}$1${RES}"
}

function echo_green {
	echo -e "${GREEN}$1${GREEN_END}"
}

# 获取 git status 输出的有变化的文件名，并保存到数组 files 中
files=( $(git status --porcelain | tee /dev/tty | awk '{print $2}') )

echo ""

if [ ${#files[@]} -eq 0 ]; then
    echo_green "Not find any modified file"
else
	echo_green "setting envirnoment variable start: "
	# 遍历文件列表，设置环境变量
	for file in "${files[@]}"
	do
		origin_file_name="$(basename "$file")"
		file_name="$(basename "$file")"
		if [[ $file_name == .* ]]; then
			IFS='.'
			arr=($file_name)
			unset IFS
			# 遍历数组中的每个元素
			file_name="${arr[1]}"
		else
			# 提取出文件名（不含后缀）
			file_name="${file_name%%.*}"
		fi
	# 设置环境变量
		file_name="$file_name""_""${origin_file_name##*.}"
		export "$file_name=$PWD/$file"
		echo_red "$file_name=$PWD/$file"
	done
	echo_green "setting envirnoment variable end: "
fi

unset RED
unset RES
unset GREEN
unset GREENEND

