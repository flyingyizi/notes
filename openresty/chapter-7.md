


# chapter 7

## 7.1什么是nginx 的rewrite规则

Rewrite主要的功能就是实现URL的重写，Nginx的Rewrite规则采用PCRE(Perl Compatible Regular Expressions)Perl兼容正则表达式的语法进行规则匹配，如果您需要Nginx的Rewrite功能，在编译Nginx之前，须要编译安装PCRE库。

正则表达式（英文：Regular Expression）在计算机科学中，是指一个用来描述或匹配一系列符合某个句法规则的字符串的单个字符串。最初的正则表达式出现于理论计算机科学的自动控制理论和形式化语言理论中。在这些领域中有对计算（自动控制）的模型和对形式化语言描述与分类的研究。20世纪40年代，WarrenMu11h与WalterPitts将神经系统中的神经元描
述成小而简单的自动控制元。20世纪50年代，数学家斯蒂芬·科尔·克莱尼利用称之为正则集合的数学符号来描述此模型“肯·汤普逊将此符号系统引入编辑器QED,然后是Unix上的编辑器，并最终引入grep。自此，正则表达式被广泛使用于各种Unix或类似Unix的工具，例如Perl.

通过Rewrite规则，可以实现规范的URL、根据变量来做URL转向及选择配置。例如，些使用MVC框架的程序只有一个入口，可以通过Rewnte来实现。一些动态URL地址须要伪装成静态HTML,便于搜索引擎抓取，也需要Rewri忙来处理·一些由于目录结构、域名变化的旧URL,须要跳转到新的URL上，也可通过Rewrite来处理。

## 7．2NginxRewrite规则相关指令

Nginx Rewrite规则相关指令有if、rewrite、set、return、breake，其中rewnte是最关键的指令。一个間单的NginxRewrite规则语法如下：
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

### 7.2.1 break指令
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


### 7.2.2 if指令
语法：if(condition)(…}

默认值：none

使用环境：server,location

该指令用于检查一个条件是否符合．如果条件符合，则执行大括号内的语句。if指令不支持嵌套，不支持多个条件&&和Il处理。

以下信息可以被指定为条件：

- （1)变量名，错误的值包括：空字符串""，或者任何以0开始的字符串：
