
# 有用链接

- [[be-a-professional-programmer]](https://github.com/stanzhai/be-a-professional-programmer)
- [[official docker file]](https://github.com/docker-library)
- [[windows 配置安装docker]](https://docs.microsoft.com/zh-cn/virtualization/windowscontainers/manage-docker/configure-docker-daemon)


    maven { url 'http://maven.aliyun.com/nexus/content/groups/public/' }
    maven{ url 'http://maven.aliyun.com/nexus/content/repositories/jcenter'}


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


