
# 有用链接

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


### 准备

- 设置

```sh
git config --global user.name "xxx"
git config --global user.email "xx@xx"
```

- 查看设置

```sh
git config --global user.name
git config --global user.email
```

- 创建仓库

```sh
git init
```

- 提交本地代码

```sh
git add filename/目录
git  commit -m  "comment"
```

- 撤销还没有提交的代码

就是对编辑了，还没有通过`git add xxx`进行提交的可以通过该命令撤销
```sh
git checkout app/xx/xx/abc.java
```

- 撤销已经提交的代码

```sh
git restore HEAD  app/xx/xx/abc.java
```

- 查看远程库地址

```sh
git remote -v
```







