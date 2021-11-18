- [Elasticsearch java手册](https://gitee.com/quanke/elasticsearch-java/)
- [[Elasticsearch 权威指南（中文版）]](https://es.xiaoleilu.com/)
- [[Elasticsearch的使用场景深入详解]](http://blog.csdn.net/laoyang360/article/details/52227541)
- [[Elasticsearch源码编译运行]](http://blog.csdn.net/u014652699/article/details/73639563)
- [[搭建elasticSearch源码调试环境]](http://blog.csdn.net/lyle2000w/article/details/74638363)
- [[Elasticsearch windows下一键安装实现深入详解]](http://blog.csdn.net/laoyang360/article/details/51900235)
- [[搜索引擎ElasticSearchV5.4.2系列二之ElasticSearchV5.4.2+kibanaV5.4.2+x-packV5.4.2安装]](https://www.cnblogs.com/cnmenglang/p/7058292.html)

- [[Elasticsearch 常用基本查询]](https://www.cnblogs.com/sunfie/p/6653778.html)
- [[【Elasticsearch】打分策略详解与explain手把手计算]](http://blog.csdn.net/molong1208/article/details/50623948)


## 启动elasticsearch

- 修改 C:\prog\elasticsearch-6.2.1\config\elasticsearch.yml 注意保存为nobom UTF-8,例如：
```
cluster.name: es-6.2.1-test 
node.name: node-es-103      
#node.master: false
#node.data: true
network.host: [_local_, 192.168.1.8]       #修改一下ES的监听地址，这样别的机器也可以访问
#http.port: 9200          
#discovery.zen.ping.multicast.enable: false   
discovery.zen.ping.unicast.hosts: 192.168.1.8:9300
# 增加新的参数，这样head插件可以访问es
http.cors.enabled: true 
http.cors.allow-origin: "*"
```
- 运行 bin\elasticsearch

## 安装elasticsearch-head
-[[下载地址]](https://github.com/mobz/elasticsearch-head)
- head已经支持5.x，所以现在网上有些手工修改教程是没有必要的。 建议采用docker方式，最方便。

- 访问head
```shell
http://localhost:9100
```

##  手工编译trace代码的方法
- 链接举例
- [[v5.4.2发行版本下载]](https://artifacts.elastic.co/downloads/elasticsearch/elasticsearch-5.4.2.zip)
- [[v5.4.2发行版本源代码下载]](https://codeload.github.com/elastic/elasticsearch/zip/v5.4.2)
- [[v5.4.2发行版本源代码下载 git clone -b v5.4.2 --depth 1 https://github.com/elastic/elasticsearch.git]]


- 安装gradle， 设置GRADLE_HOME，并添加%GRADLE_HOME%/bin加到PATH的环境变量。 由于当前gradle-logger-x.x.x最新的只是到4.3，所以gradle不要安装高于gradle4.3版本。
- step1:源码安装慢，可以在根目录的build.gradle文件中的allprojects内添加阿里云的镜像：
```
allprojects {
  repositories {  
    maven { url "http://maven.aliyun.com/nexus/content/groups/public/" }
  }
}
```
同时在~/.gradle下新增一个init.gradle文件,内容是:
```
allprojects { 
    repositories {
        def REPOSITORY_URL = 'http://maven.aliyun.com/nexus/content/groups/public/'
        all { ArtifactRepository repo ->
            if(repo instanceof MavenArtifactRepository){
                def url = repo.url.toString()
                if (url.startsWith('https://repo.maven.org/maven2') || url.startsWith('https://jcenter.bintray.com/')) {
                    project.logger.lifecycle "Repository ${repo.url} replaced by $REPOSITORY_URL."
                    remove repo
                }
            }
        }
        maven {
            url REPOSITORY_URL
        }
    }  
} 
```


- step2: gradle idea
- IDEA导入工程，导入的时候需要设置gradle_home，配置为gradle的安装根目录。勾选Offline work（否则会连到官网私服下载，会超级慢）和Use local gradle distribution

- step3: 编译

- 启动中碰到的问题解决，参见[[ElasticSearch本地调测环境构建]](https://www.cnblogs.com/hrbeu05/archive/2017/12/02/7954555.html)中列的问题. 大部分原因是源码的启动脚本：distribution\src\main\resources\bin\下elasticsearch.sh 中的变量设置问题。

-Des.path.home=C:\home\atmel\binelasticsearch -Dlog4j2.disable.jmx=true

C:\home\atmel\elasticsearch\distribution\src\main\resources\plugins


-Des.path.home=C:\home\atmel\es\elasticsearch-6.2.1 -Des.path.conf=C:\home\atmel\es\elasticsearch-6.2.1\distribution\src\main\resources\config  -Dlog4j2.disable.jmx=true

