#!/bin/bash

source /usr/bin/echo_color_func.sh

while getopts ":h" opt; do
  case $opt in
    h)
	  echo "usage:./get_commit_id.sh [commit count]"
	  echo "defualt commit count is 100"
      exit 0
      ;;
  esac
done

commit_counts=100

if [ $# -ge 1 ]; then
	commit_counts=$1
fi


commit_id=$(git rev-list --reverse HEAD | head -${commit_counts} | tail -1)

echo_green_color "The commit ID of the ${commit_counts}th commit is: ${commit_id}"
