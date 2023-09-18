#!/bin/sh

command_arr=("alias count_id='git rev-list --count '" "alias git_all_log='git log --all --graph'"
			 "alias push='git push'" "alias rebase='git rebase'" "alias git_update_sub='git submodule update --init --recursive'"
		 	 "alias fetch='git fetch'" "alias tortise_log='tortoisegitproc.exe /command:log'"
		 	 "alias tortise_commit='tortoisegitproc.exe /command:commit'"
		 	 "alias git_st='source git_st'" "alias commit='git commit -m '"
		 	 "alias add='git add'" "alias checkout='git checkout'")

source /usr/bin/echo_color_func.sh

function install_alias_command {

echo_green_color "install alias command start"

for i in "${!command_arr[@]}"
do
	# 检查~/.bashrc是否存在
	if [[ ! -f ~/.bashrc ]]; then
		touch ~/.bashrc
	fi

	command_content=${command_arr[$i]}
	if grep -q "${command_content}" ~/.bashrc; then
		echo_red_color "find ${command_content} command in ~/.bashrc"
	else
		echo "${command_content}" >> ~/.bashrc
		echo_green_color "install ${command_content}"
	fi
done

echo_green_color "install alias command finished"

source ~/.bashrc

}

while [[ $# -gt 0 ]]
do
	key="$1"
	case $key in
		-i)
			install_alias_command
			shift
			;;
		-h)
			echo_green_color "usage:./alias_git_command.sh -i"
			echo_green_color "-i install" 
		shift
		;;
		*)
		shift
		;;
	esac
done

