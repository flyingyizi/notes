



- [[SpringMVC集成Swagger插件以及Swagger注解的简单使用]](https://segmentfault.com/a/1190000010554445)
- [[使用swagger作为restful api的doc文档生成]](https://www.cnblogs.com/woshimrf/p/5863318.html)

## 3.2 设置Tomcat环境变量

新建一个环境变量：变量名：TOMCAT_HOME. 变量值：你的TOMCAT解压后的目录，如E:\apache-tomcat-7.0.26。

 
## 3.3 验证Tomcat环境变量的配置是否成功

运行Tomcat解压目录下的 bin/startup.bat，启动Tomcat服务器。在任何一款浏览器的地址栏中输入http://localhost:8080 ，如果界面显示如下图，则说明Tomcat的环境变量配置成功。


# 4. 部署Web项目的War包到Tomcat服务器
 
- FTP获取war包和sql脚本
 
- 配置Web项目的虚拟目录

   将projectName.war包，复制到Tomcat的webapp下。这样当配置好后的访问路径便为：http://localhost:8080/projectName/login.jsp。

在访问之前，需要修改tomcat服务器的配置文件，打开：

tomcat解压目录\conf\context.xml。将运行该web项目时，需要配置的数据库连接字符串增加到该xml文件中。增加后的context.xml为：
```xml
<Context>
<!-- Default set of monitored resources -->
<WatchedResource>WEB-INF/web.xml</WatchedResource>
<!-- Uncomment this to disable session persistence across Tomcat restarts -->
<!--
<Manager pathname="" />
-->
<!-- Uncomment this to enable Comet connection tacking (provides events
on session expiration as well as webapp lifecycle) -->
<!--
<Valve className="org.apache.catalina.valves.CometConnectionManagerValve" />
-->
(这里填写本Web项目运行时，需要连接的数据库配置。)
</Context>
```

## [Java][Maven]Maven部署Web项目报错webxml attribute is required

   maven的web项目默认的webroot是在src\main\webapp。如果在此目录下找不到web.xml就抛出以上的异常。解决办法：需要在pom.xml中增加<webResources>配置，如下：
```xml
    <build>    
    <finalName>simple-webapp</finalName>    
    <plugins>    
        <plugin>    
            <groupId>org.apache.maven.plugins</groupId>    
            <artifactId>maven-war-plugin</artifactId>    
            <version>2.1.1</version>    
            <configuration> 
                  <webXml>WebContent\WEB-INF\web.xml</webXml>
            </configuration>
        </plugin>    
    </plugins>    
     </build>    
```

org.apache.maven.archetypes:maven-archetype-webapp
