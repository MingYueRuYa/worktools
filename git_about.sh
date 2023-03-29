#!/bin/sh

# 1. stash
# 2. fetch up
# 3. rebase
# 4. stash

RED='\e[1;31m' # 红
RES='\e[0m'

function echo_color {
	echo -e "${RED}$1${RES}"
}


echo_color "git stash"
git stash 
echo_color "git stash"

echo_color "git fetch up"
git fetch up
echo_color "git fetch up"

echo_color "git rebase"
git rebase 'up/release/v1.1.0'
echo_color "git rebase"

echo_color "git stash pop"
git stash pop
echo_color "git stash pop"
