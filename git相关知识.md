
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

### 提交本地代码

```sh
git add filename/目录
git  commit -m  "comment"
```

### 导出代码

```sh
git archive --format tar.gz --output "./output.tar.gz" master
```

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

### 撤销还没有提交的代码

就是对编辑了，还没有通过`git add xxx`进行提交的可以通过该命令撤销
```sh
git checkout app/xx/xx/abc.java
```

### 撤销已经提交的代码

```sh
git restore HEAD  app/xx/xx/abc.java
```

### 查看远程库地址

```sh
git remote -v
```

### 放弃本地修改，强制拉取更新

```shell
git fetch --all
git reset --hard origin/master
```

### git status 查看哪些文件被修改

### git diff 查看所有文件具体被修改的地方


