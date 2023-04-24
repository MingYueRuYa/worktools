#!/bin/sh

remote_names=`git remote`

# 遍历所有Git远程仓库的名称并打印它们
for name in ${remote_names}
do
  git push $name
done
