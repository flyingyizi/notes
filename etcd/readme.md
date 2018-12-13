

$\sum_{i=0}^N\int_{a}^{b}g(t,i)\text{d}t$

\alpha

- 安装启动命令：
```shell
export HostIP="192.168.1.8"
docker run -d -v /usr/share/ca-certificates/:/etc/ssl/certs -p 4001:4001 -p 2380:2380 -p 2379:2379 \
 --name etcd quay.io/coreos/etcd:v2.3.8 \
 -name etcd0 \
 -advertise-client-urls http://${HostIP}:2379,http://${HostIP}:4001 \
 -listen-client-urls http://0.0.0.0:2379,http://0.0.0.0:4001 \
 -initial-advertise-peer-urls http://${HostIP}:2380 \
 -listen-peer-urls http://0.0.0.0:2380 \
 -initial-cluster-token etcd-cluster-1 \
 -initial-cluster etcd0=http://${HostIP}:2380 \
 -initial-cluster-state new
 ```
- CTL举例
```shell
etcdctl -C http://192.168.1.8:2379 member list
etcdctl -C http://192.168.1.8:4001 member list
```

- A simple tcp server that notifies all clients whenever a file is modified. Includes a simple client which blinks the lights on a keyboard when a it recieves any notification from the server.
[[]](https://github.com/lelandbatey/watchserver)

- [[神经网络]](http://beust.com/weblog/2016/05/27/neural-network-in-kotlin/) ， [[代码]](https://github.com/cbeust/nnk)
- [[神经网络介绍]](https://www.zhihu.com/question/19833708)