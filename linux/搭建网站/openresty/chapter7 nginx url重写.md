

# chapter 7  TODO

nginx有两种重定向方式，一个是反向代理，另外一个就是rewrite

## 7.1什么是nginx 的rewrite规则

Rewrite主要的功能就是实现URL的重写，Nginx的Rewrite规则采用PCRE(Perl Compatible Regular Expressions)Perl兼容正则表达式的语法进行规则匹配，如果您需要Nginx的Rewrite功能，在编译Nginx之前，须要编译安装PCRE库。

正则表达式（英文：Regular Expression）在计算机科学中，是指一个用来描述或匹配一系列符合某个句法规则的字符串的单个字符串。最初的正则表达式出现于理论计算机科学的自动控制理论和形式化语言理论中。在这些领域中有对计算（自动控制）的模型和对形式化语言描述与分类的研究。20世纪40年代，WarrenMu11h与WalterPitts将神经系统中的神经元描
述成小而简单的自动控制元。20世纪50年代，数学家斯蒂芬·科尔·克莱尼利用称之为正则集合的数学符号来描述此模型“肯·汤普逊将此符号系统引入编辑器QED,然后是Unix上的编辑器，并最终引入grep。自此，正则表达式被广泛使用于各种Unix或类似Unix的工具，例如Perl.

通过Rewrite规则，可以实现规范的URL、根据变量来做URL转向及选择配置。例如，些使用MVC框架的程序只有一个入口，可以通过Rewnte来实现。一些动态URL地址须要伪装成静态HTML,便于搜索引擎抓取，也需要Rewri忙来处理·一些由于目录结构、域名变化的旧URL,须要跳转到新的URL上，也可通过Rewrite来处理。

# 7．2NginxRewrite规则相关指令


## rewrite

基本用法是 rewrite patten replace flag

patten是正则表达式，与patten匹配的URL会被改写为replace，flag可选

rewrite 指令的最后一项参数为flag 标记，支持的flag 标记有：

- last： 停止当前这个请求，并根据rewrite匹配的规则重新发起一个请求。新请求又从第一阶段开始执行…
- break：相对last，break并不会重新发起一个请求，只是跳过当前的rewrite阶段，并执行本请求后续的执行阶段…
- • redirect一返回302 临时重定向，浏览器地址栏会显示跳转后的URL 地址：
- • permanen「一返回到l 永久重定向，浏览器地址栏会显示跳转后的URL 地址。

在以上的标记中， last 和break 用来实现URI 重写，浏览器地址栏的URL 地址不变，但在服务器端访问的路径发生了变化。redirect 和permanent 用来实现URL 跳转，浏览器地址栏会显示跳转后的URL 地址。

Nginx Rewrite规则相关指令有if、rewrite、set、return、breake，其中rewnte是最关键的指令。




```shell
rewrite ^/b/(.*)\.html /play.php?video=$1 break;
```
如果加上if语句，示例如下：
```shell
if (!-f $request_filename)
{
    rewrite ^/img/(.*)$ /site/$host/images/$1   last;
}
```

## 7.2.1 break指令
语法：break

默认值：none

使用环境：server,location,if

该指令的作用是完成当前的规则集，不再处理rewrite指令。
示例如下：
```shell
if($slow) {
    limit_rate 10k;
    break;
}
```


## 7.2.2 if指令
语法：if(condition)(…}

默认值：none

使用环境：server,location

该指令用于检查一个条件是否符合．如果条件符合，则执行大括号内的语句。if指令不支持嵌套，不支持多个条件&&和Il处理。

以下信息可以被指定为条件：

- （1)变量名，错误的值包括：空字符串""，或者任何以0开始的字符串：

- (2 ）变量比较可以使用“＝” （表示等于）和“！＝” （表示不等于）运算符：
- (3 ）正则表达式模式匹配可以使用“ J ”和“～”符号：
- (4 ）"~"符号表示区分大小写字母的匹配：
- (5 ）"~*"符号表示不区分大小写字母的匹配（例如firefox 与FireFox 是匹配的）：
- (6 ）"!~"和"!~*"符号的作用刚好和“~”、“~*”相反，表示不匹配：
- (7 ）"-f"和"!-f"用来判断文件是否存在：
- ( 8) "-d"和"!-d"用来判断目录是否存在：
- (9 ）"-d"和"!-e"用来判断文件或目录是否存在：
- (10 ）"-x"和"!-x"用来判断文件是否可执行。

# 7.4例子


## rewrite flag是last与break区别：

准备：

- root  html 中存放lett.html 内容标识来自openresty
- root  c:/htmlDemo 中存放lett.html 内容标识来自htmldemo

### 使用last

```conf
    location / {
        root   c:/htmlDemo;
        index  index.html index.htm homepage.html;           
    }
    location ^~ /test/ {
        root  html;
        rewrite ^/test/(.*) /$1  last;
        }
```

得到结果是

```cmd
$ curl localhost/test/lett.html
  % Total    % Received % Xferd  Average Speed   Time    Time     Time  Current
                                 Dload  Upload   Total   Spent    Left  Speed
100   175  100   175    0     0    853      0 --:--:-- --:--:-- --:--:--   849
<!doctype html>
<html>
<head>
        <meta charset="UTF-8">
        <title>""</title>
</head>
<body>
    来自htmldemo
</body>
</html>
```

这个结果说明： 访问“localhost/test/lett.html” ，首先匹配到的是“location ^~ /test/”，由于其中的rewrite指令，使得访问地址变为“localhost/lett.html”，由于rewrite是与last指示，因此重新以新的请求地址发起请求，后面显然是匹配上“location /”，从而从“root  c:/htmlDemo”这个地方获取网页

### 使用break

准备：

- root  html 中存放lett.html 内容标识来自openresty
- root  c:/htmlDemo 中存放lett.html 内容标识来自htmldemo

```conf
    location / {
        root   c:/htmlDemo;
        index  index.html index.htm homepage.html;           
    }
    location ^~ /test/ {
        root  html;
        rewrite ^/test/(.*) /$1  break;
        }
```

得到结果是

```cmd
$ curl localhost/test/lett.html
  % Total    % Received % Xferd  Average Speed   Time    Time     Time  Current
                                 Dload  Upload   Total   Spent    Left  Speed
100   178  100   178    0     0    827      0 --:--:-- --:--:-- --:--:--   831
<!doctype html>
<html>
<head>
        <meta charset="UTF-8">
        <title>""</title>
</head>
<body>
        来自openresty html
</body>
</html>
```

对break，虽然“/test/xx”被重定向到“/xx”/,但是break指令不会重新开启一个新的请求继续匹配。即并不会去匹配“location /”。 那为什么不显示404，而是显示了“root  html”下的lett.html？ 这是因为break后面没有content指令，这会导致这location里只能执行nginx默认的content指令，即尝试找xx这个html页面并输出起内容

如果我们在break后面添加content相关的指令，即如果配置是

```conf
        location / {
            root   c:/tuxueqing/htmlDemo;
            index  index.html index.htm homepage.html;           
        }
        location ^~ /test/ {
            root  html;
            rewrite ^/test/(.*) /$1  break;
            #break后面的content指令
            content_by_lua ' 
              ngx.say("  content part")
            ';
        }
```

得到的结果是

```cmd
atmel@atmel-PC MINGW64 ~
$ curl localhost/test/lett.html
  % Total    % Received % Xferd  Average Speed   Time    Time     Time  Current
                                 Dload  Upload   Total   Spent    Left  Speed
100    15    0    15    0     0     73      0 --:--:-- --:--:-- --:--:--    73  
content part
```

表明这个只是执行了break后面的content指令，这个"rewrite ... break"相当于废了。


## rewrite: 修改访问路径例子

下面这个例子将“localhost/test/lett.html”更改为“localhost/lett.html”的访问。访问的文件是“root html”下的lett.html

```conf
        location ^~ /test/ {
            root  html;
            rewrite ^/test/(.*) /$1  break;
            #break后面的不能有content指令，这里你使用的nginx的默认content处理，具体说明见前面rewrite flag是break、last区别的例子中说明
        }
```

## rewrite:跳转到新域名上时保留路径例子

```conf
server  {  

   listen 80;  
  server_name www.old.com;  

  rewrite "^/(.*)$" http://www.new.com/$1;  

}  
``` 

## rewrite:与location配合实现图片文件跳转到cdn例子

```conf
location ~ .*\.(gif|jpg|jpeg|png|bmp|swf)$ {  
     expires 30d;  
     rewrite "^/uploadfile\/(.*)$" http://static.XXX.com/uploadfile/$1;  
} 
``` 
 