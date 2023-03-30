#!/bin/sh

# 1. stash
# 2. fetch up
# 3. rebase
# 4. stash

RED='\e[1;31m' # 红
RES='\e[0m'

function echo_color {
	echo -e "${RED}************$1************${RES}"
}


echo_color "git stash start"
git stash 
echo_color "git stash end"

echo_color "git fetch up start"
git fetch up
echo_color "git fetch up end"

echo_color "git rebase start"
git rebase 'youre_branch_name'
echo_color "git rebase end"

echo_color "git stash pop start"
git stash pop
echo_color "git stash pop end"
