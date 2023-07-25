#!/bin/bash

source /usr/bin/echo_color_func.sh

var_list=()
invalid_var_list=()

function update_var {
	suffix=1
	newstr=$1
	# 解决文件可能重名，如果重名则添加后缀方式解决
	while [[ "${var_list[*]}" =~ $newstr ]]
	do
	  newstr="$newstr""_""$suffix"
	  suffix=$(($suffix+1))
	done

	check_variable_name $newstr
	if [[ $? -eq 0 ]]; then
		invalid_var_list+=("$2")
	else
		var_list+=("$newstr")
		export "$newstr=$2"
		echo_green_color "$newstr=$2"
	fi
}

function check_variable_name() {
    local var_name=$1
    if [[ ! $var_name =~ ^[a-zA-Z_][a-zA-Z0-9_]*$ || $(type -t "$var_name") == "keyword" ]]; then
		return 0
    fi
	return 1
}

function print_list_content()
{
	for item in "${invalid_var_list[@]}"
	do
	  echo_red_color $item
	done
}

top_dir_path=`git rev-parse --show-toplevel`
# 获取 git status 输出的有变化的文件名，并保存到数组 files 中
files=( $(git status --porcelain | tee /dev/tty | awk '{print $2}') )

echo ""

if [ ${#files[@]} -eq 0 ]; then
    echo_green_color "Not find any modified file"
else
	echo_green_color "setting envirnoment variable start: "
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
		if [[ $origin_file_name = *.* ]]; then
			file_name="$file_name""_""${origin_file_name##*.}"
		fi
		update_var "$file_name" "$top_dir_path/$file"
	done
	echo_green_color "setting envirnoment variable end: "
fi

echo_red_color "print invalid name:"
print_list_content

unset RED
unset RES
unset GREEN
unset GREENEND

