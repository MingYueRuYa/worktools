#!/bin/sh

source /usr/bin/echo_color_func.sh
source ./alias_git_command.sh

cp_command_arr=("./echo_color_func.sh" "./alias_git_command.sh")

for i in "${!cp_command_arr[@]}"
do
	command_content=${cp_command_arr[$i]}
	cp $command_content /usr/bin/$command_content
done

echo_green_color "will install status.sh and update_code.sh"

# define dict 
declare -A dict
dict['git_update_code']='update_code.sh'
dict['git_st']='status.sh'
dict['git_push']='push.sh'
dict['git_commit_id']='git_commit_id.sh'

for key in "${!dict[@]}"
do
	cp ./${dict[$key]} /usr/bin/$key
	echo_green_color "cp ./${dict[$key]} /usr/bin/$key"
done

command_str=""
for key in "${!dict[@]}"
do
	command_str="${command_str} $key "
done
echo_green_color "you can use command:$command_str"

echo_green_color "install shell succussful"

########################################

install_alias_command

