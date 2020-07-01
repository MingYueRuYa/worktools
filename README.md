# worktool
## 工作用到的小工具集合

### monkey

主要是为了修改VC Project rc文件的版本号

1.使用方法

  A.运行软件到你的Project的顶层目录，monkey会自动递归查找项目下的rc文件。

  B.然后加载rc文件，获取版本号，在table里modify一栏写入新的版本号

  C.最后点击Modify按钮就会自动更新rc文件的版本号

2.本软件采用VS 2013 + Qt5.6.3版本作为开发集成环境。

### dolphin 

主要是为了查看dll，exe的详细信息，比如：签名，文件名，大小，等等

### cattle

主要是为了解决，在git commit如果提交了中文注释，再使用git 生成的patch文件中
文件里面的中文注释会乱码。

所以通过将所有的commit提交记录导出，并和所有的patch根据commit-id对应起来，
并找到对应的patch文件

将git中所有的commit log导入到桌面commit csv文件中

> git log --date=iso --pretty=format:'"%h","%an","%ad","%s"'>> ~/Desktop/commit.csv

将当前commit id 之前的所有的commit生成patch文件

> git format-patch commit_id

#### 使用方法

1.将导出的commit.csv文件和导出的patch文件放到exe当面目录下。

2.点击commit log表格，根据commit ID找到对应的patch file，并补充patch table的内容

3.双击表格的单元格，就会用使用tortoise diff工具打开patch文件，前提是你装了tortoise

工具。如果没有装的，就会使用notepad++打开。否则没有任何操作。
