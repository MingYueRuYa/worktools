#!/bin/sh

GREEN='\033[32m'
GREEN_END='\033[0m'

function echo_green {
	echo -e "${GREEN}************$1************${GREEN_END}"
}

echo "will install status.sh and update_code.sh"
cp ./update_code.sh /usr/bin/git_update_code

if [ $? -ne 0 ]; then
	echo "cp ./update_code.sh /usr/bin/git_update_code error"
	exit 1
fi

cp ./status.sh /usr/bin/git_st

if [ $? -ne 0 ]; then
	echo "cp ./status.sh /usr/bin/git_st"
	exit 1
fi

# 检查~/.bashrc是否存在
if [[ -e ~/.bashrc ]]; then
	# 检查字符串是否存在
  if grep -q "alias git_st=source git_st" ~/.bashrc; then
	  echo_green "find git_st command in ~/.bashrc"
  else
	echo "alias git_st='source git_st'" >> ~/.bashrc
  fi
else
	echo "alias git_st=\'source git_st\'" >> ~/.bashrc
fi

cp ./push.sh /usr/bin/git_push

if [ $? -ne 0 ]; then
	echo "cp ./push.sh /usr/bin/git_push"
	exit 1
fi

echo_green "install shell succussful"

echo "you can input git_update_code or git_st command"
