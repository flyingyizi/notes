

Docker 的基础是 Linux 容器（LXC）等技术,是在OS之上的虚拟。

可以在[docker hub官网](https://hub.docker.com/), 搜索自己感兴趣的`Dockfile`

# 常用命令

docker配置文件是`/etc/default/docker`
### 常用运维指令:  

`sudo systemctl [status|start|stop|restart] docker `

### 常用命令

- `docker pull `        Pull an image or a repository from a registry
-  `docker run`         Run a command in a new container
-  `docker search`      Search the Docker Hub for images， 这个查找不太靠谱，除非有比较精确的名字
-  `docker image`    一系列管理镜像的命令
-  `docker container`    一系列管理容器的命令，比如`docker container ls -a`最常用，注意如果不加a参数，仅会列出running状态的容器
-  `docker run`      

    例如`$ sudo docker run -t -i ubuntu:12.04 /bin/bash` 表示使用镜像`ubuntu:12.04`创建个容器，在该容器中运行bash，其中`-i`,`-t`的解释见下面

    ```shell
    atmel@mail:~$ docker run --help
    Usage:	docker run [OPTIONS] IMAGE [COMMAND] [ARG...]
    Run a command in a new container

    Options:
    -i, --interactive                    Keep STDIN open even if not attached
    -t, --tty                            Allocate a pseudo-TTY 给容器分配个伪终端
    -d, --detach                         Run container in background and print container ID 守护态运行容器
    -v, --volume list                    Bind mount a volume
        --volume-driver string           Optional volume driver for the container
        --volumes-from list              Mount volumes from the specified container(s)
    -w, --workdir string                 Working directory inside the container
    ...
    ```
- `docker build`  通过该执行命令的当前目录中`Dockfile`来创建镜像。`Dockerfile`  中每一条指令都创建镜像的一层


# 基本概念

Docker 包括三个基本概念，理解这三个概念就理解了 Docker 的整个生命周期。
- 镜像（Image）
- 容器（Container）
- 仓库（Repository）

## 镜像

### Dockfile创建自定义镜像

#### `CMD`

指定启动容器时执行的命令，每个 Dockerfile 只能有一条 CMD 命令。如果指定了多条命令，只有最后一条会被执行。如果用户启动容器时候指定了运行的命令，则会覆盖掉 CMD 指定的命令。

#### ENTRYPOINT

每个 Dockerfile 中只能有一个 ENTRYPOINT ，当指定多个时，只有最后一个起效。

### Docker Compose 项目

Dockerfile 可以让用户管理一个单独的应用容器；而 Compose 则允许用户在一个模板（YAML 格式）中定义一组相关联的应用容器（被称为一个 project ，即项目），

docker-compose是独立产品，需要在安装docker之后通过下面的命令进行安装

```shell
#安装docker-compose
pip install docker-compose 
```

在文件`docker-compose.yml`所在目录下执行 `docker-compose up` 命令，会整合输出所有容器的输出


# 案例


## redis

可以从官网下载默认配置文件`redis.conf`，[官网配置文件](http://download.redis.io/redis-stable/redis.conf)

典型命令：

```shell
#运行
docker run --restart=always --log-driver json-file --log-opt max-size=100m --log-opt max-file=2 -p 6379:6379 --name myredis -v /home/atmel/proxy_pool/redis/redis.conf:/etc/redis/redis.conf -v /home/atmel/proxy_pool/redis:/data -d redis redis-server /etc/redis/redis.conf --appendonly yes --requirepass "123456"
```