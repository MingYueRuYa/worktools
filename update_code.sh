#!/bin/sh

RED='\e[1;31m' # 红
RES='\e[0m'

GREEN='\033[32m'
GREEN_END='\033[0m'

function echo_red {
	echo -e "${RED}************$1************${RES}"
}

function echo_green {
	echo -e "${GREEN}************$1************${GREEN_END}"
}

stashed=0


# 获取git remote出来的远程服务器名称，并将多个名称放入数组中
# remote_names=($(git remote))

# 输出数组中的所有元素
# for name in "${remote_names[@]}"
# do
#   echo "The remote server name is: $name"
# done

# 查看是否有文件，子模块修改
echo_red "git status "
status=$(git status --porcelain -uno)

if [ -n "$status" ]; then
	echo_red "git stash start"
	echo_red "modified files"
	echo "$status"
	echo_red "modified files"
	git stash 
	stashed=1
	echo_red "git stash end"

	# 如果有3rd，hc字段，表示有子模块更新
	if echo "$status" | grep -qE "idl|hc"; then
		echo_red "update submodule starts"
		git submodule update --init --recursive
		echo_red "update submodule end"
	fi
else
	echo_green "Not find any modified."
fi
echo_red "git status "


echo_red "git fetch origin start"
git fetch origin
echo_red "git fetch origin end"

echo_red "git rebase start"
git rebase 'up/release/v1.1.20'
echo_red "git rebase end"

if [ $stashed -eq 1 ]; then
	echo_red "git stash pop start"
	git stash pop
	echo_red "git stash pop end"
fi
