
# 有用链接

[Git实用技巧](https://www.cnblogs.com/ludongguoa/p/15335877.html)
[书籍](https://git-scm.com/book/zh/v2)

# docker_build


- [[Connecting to GitHub with SSH]](https://help.github.com/articles/connecting-to-github-with-ssh/)

- [[Understanding the GitHub Flow]](https://guides.github.com/introduction/flow/)

   85  echo "# docker_build" >> README.md
   86  git init
   87  git add README.md
   88  git add *
   89  ls
   90  git add dockerUtil.sh
   91  git add redis-cluster/
   92  git commit -m "first commit"
   93  git remote add origin git@github.com:flyingyizi/docker_build.git
   94  git push -u origin master


   ./redis-trib.rb create --replicas 1 172.17.0.1:7000 172.17.0.1:7001  172.17.0.1:7002  172.17.0.1:7003  172.17.0.1:7004  172.17.0.1:7005 

docker run --net=host -i -t -d --name redis-1  redis:alpine /usr/local/bin/redis-server  --protected-mode no
 --net=host  redis /usr/local/bin/redis-server
#

https://mp.weixin.qq.com/wiki?t=resource/res_main&id=mp1421140183&token=&lang=zh_CN


## 常用

### 设置

```sh
git config --global user.name "xxx"
git config --global user.email "xx@xx"
```

### 查看设置

```sh
git config --global user.name
git config --global user.email
```

### 创建仓库

```sh
git init
```

### 创建原创仓库

echo "# rust_embed_scratch" >> README.md
git init
git add README.md
git commit -m "first commit"
git branch -M main
git remote add origin https://github.com/<name>/<repository>.git
git push -u origin main

### 提交本地代码

```sh
git add filename/目录
git  commit -m  "comment"
```

### 导出代码

```sh
git archive --format tar.gz --output "./output.tar.gz" master
```

### 统计

Linux 5.10.11 sources:
- 70,639 files (git ls-files | wc -l)
- 29,746,102 lines (git ls-files | xargs cat | wc -l)
- 962,810,769 bytes (git ls-files | xargs cat | wc -c)

## git pre-commit hook

每次git commit的时，git会主动调用project根目录下的` .git/hooks/pre-commit` 这个脚本(默认的*.sample不执行)，脚本可以是shell、python、ruby等可执行脚本，只要是 以非零状态 退出会导致中止，就commit失败。

下面的例子是强制要求提交代码需要符合linux编程风格，其中`./scripts/checkpatch.pl`是linux-source下的脚本：

```shell
#!/bin/sh
#
# pre-commit hook to run check-patch on the output and stop any commits
# that do not pass. Note, only for git-commit, and not for any of the
# other scenarios
#
# Copyright 2010 Ben Dooks, <ben-linux@fluff.org>
if git rev-parse --verify HEAD 2>/dev/null >/dev/null
then
against=HEAD
else
# Initial commit: diff against an empty tree object
against=4b825dc642cb6eb9a060e54bf8d69288fbee4904
f i
git diff --cached $against -- | ./scripts/checkpatch.pl --no-signoff -
```

## 打标签

[git tag的基本用法](https://www.jianshu.com/p/154d58451ef7)

```sh
#打标签
git tag -a v0.0 -m "init version"
#查看
git show v0.0
#删除tag
git tag --delete v0.0
```

## 撤销还没有提交的代码

就是对编辑了，还没有通过`git add xxx`进行提交的可以通过该命令撤销
```sh
git checkout app/xx/xx/abc.java
```

## 撤销已经提交的代码

```sh
git restore HEAD  app/xx/xx/abc.java
```

## 查看远程库地址

```sh
git remote -v
```

## 放弃本地修改，强制拉取更新

```shell
git fetch --all
git reset --hard origin/master
```

## git status 查看哪些文件被修改

## git diff 查看所有文件具体被修改的地方

git diff命令可以查看当前工作区与暂存区的差别，也就是说可以查看到当前我们修改或者是添加的，但是还没有add进暂存区的代码。它会列出改动前后的对比，方便我们进行查看和修改。e.g. "`git diff shell/prepare_data.sh`"

注意：在不加任何参数的情况下查看的是工作区（add命令之前）的代码和暂存区的差别

暂存区和本地git仓库的差别，也就是已经add了还没commit的改动。这个时候我们可以通过--cached参数来实现，--cached参数后面同样可以再跟上文件名，查看某一个具体的文件。

其他：
```
# 比较工作区和某一个提交的差别
git diff <commitid>

# 比较工作区和某一个提交某一个文件的差别
git diff <commitid> <filepath>

# 比较暂存区和某一个提交的差别
git diff --cached <commitid>

# 比较暂存区和某一个提交某一个文件的差别
git diff --cached <commitid> <filepath>
```

## 使用gitmodules

子模块允许你将一个 Git 仓库作为另一个 Git 仓库的子目录。 它能让你将另一个仓库克隆到自己的项目中，同时还保持提交的独立。

下面是一个典型的sub module的描述文件, 文件名固定是"`.gitmodules`". 该文件由“`git submodule add https://github.com/GPUOpen-LibrariesAndSDKs/Cauldron  xyz/efg`”生成。
```
[submodule "xyz/efg"]
	path = xyz/efg
	url = https://github.com/GPUOpen-LibrariesAndSDKs/Cauldron
```
注意，该命令不仅增加/更新"`.gitmodules`"，并且会下载该submodule

Clone submodules by running 'git submodule update --init --recursive'