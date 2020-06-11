

/////////////////////////////////////////////////////

 chrome编辑网页

1按F12打开开发者工具

2切换到console选项卡

3输入 document.body.contentEditable="true" 后回车

[luanyifengHTML 教程](https://wangdoc.com/html/)

[HTML 5 教程](https://www.w3school.com.cn/html5/index.asp)

# 1. 概述

浏览器的网页开发，涉及三种技术：HTML、CSS 和 JavaScript。HTML 语言定义网页的结构和内容，CSS 样式表定义网页的样式，JavaScript 语言定义网页与用户的互动行为。


### 锚点anchor

锚点（anchor）是网页内部的定位点，使用#加上锚点名称，放在网址的最后，比如#anchor。浏览器加载页面以后，会自动滚动到锚点所在的位置。
锚点名称通过网页元素的id属性命名。


# 重点标签tag

标签名是大小写不敏感，比如`"<title>"`和`"<TITLE>"`是同一个标签。不过，一般习惯都是使用小写。

标签是源码角度来看，元素是从编程角度来看，比如`"<p>"`标签对应网页的p元素。

## meta tag

meta是head区的一个辅助性标签。其主要作用有：搜索引擎优化（SEO），定义页面使用语言，自动刷新并指向新的页面，实现网页转换时的动态效果，控制页面缓冲，网页定级评价，控制网页显示的窗口等

###  http-equiv 属性，content 属性

http-equiv属性用来覆盖 HTTP 回应的头信息字段，content属性是该字段的内容。

```html
<meta http-equiv="Content-Security-Policy" content="default-src 'self'">
```

## base tag

`<base>`标签指定网页内部的所有相对 URL 的计算基准。整张网页只能有一个`<base>`标签，而且只能放在`<head>`里面。它是单独使用的标签，没有闭合标签，下面是一个例子。

```html
<head>
<base href="https://www.example.com/files/" target="_blank">
</head>
```

`<base>`标签的href属性给出计算的基准网址，target属性给出如何打开链接的说明（参见《链接》一章）。已知计算基准是"https://www.example.com/files/"，那么相对 URL foo.html，就可以转成绝对 URL "https://www.example.com/files/foo.html"。

一旦设置了`<base>`，就对整个网页都有效。如果要改变某个链接的行为，只能用绝对链接替代相对链接。尤其需要注意锚点，这时锚点也是针对`<base>`计算的，

## cite,q与blockquote tag

cite是用来标明引言的，所谓引言，就是对一本书、一篇报告或其它出版的来源资料的引用。可使用该标签对参考文献的引用进行定义，比如书籍或杂志的标题。如：

```html
　　<cite>《傲慢与偏见》</cite>
```

　　q是什么呢，是用来分离文本中的引语，定义一个短的引用。如：

```html
　　<q>能一起吃顿饭么？</q>，那位帅气的先生带着期盼的眼神等她回复。
```

　　再说说blockquote，它是用来定义一段引语的。比如我们要引用某篇诗文或者文章片段，如：

```html
　　<blockquote>
　　　<pre>
　　　   你站在桥上看风景，
　　　   看风景人在楼上看你。
　　　   明月装饰了你的窗子，
　　　   你装饰了别人的梦。
　　　</pre>
　　</blockquote>
```

　　需要注意的是，q是用来定义一个短的引语，blockquote是用来定义一段引语，它们的范围和作用级别不同。q引用的是一句话或者一句文档 摘要，而blockquote则是引用一段或者大篇幅。q可以只包含存文本，或者内联元素，但blockquote的子节点必须为块元素，而不能是文本或 者内联元素，当然，孙子辈的就无所谓了，这点进行strict XHTML验证你就会知晓。

　　有一点相同的就是q和blockquote都有一个重要而且相同的属性cite，“cite？这不是一个标签元素么？我们前面才说到啊。”是的，确实有一个<cite>标签，但同时也有一个cite属性，它是用来注明引用来源的URL。

　　如上面的blockquote的例子，我们就可以加上cite属性来表明该引用的来源：

```html
　　<blockquote cite=“http://www.doyoe.com/plwj/article.asp?fid=pop&id=54”...
```
　　至于如何区分cite标签和q,blockquote的作用，也可以举2个例子：

q与cite：

```html
　　<q>废那事干啥，直接上不就得了</q>，<cite>黑皮</cite>先生如是说。
```

blockquote与cite：

```html
　　<blockquote>
　　　<pre>
　　　   最是那一低头的温柔，
　　　   象一朵水莲花不胜凉风的娇羞，
　　　   道一声珍重，道一声珍重，
　　　   那一声珍重里有蜜甜的忧愁——
　　　   沙扬娜拉！
　　　</pre>
　　</blockquote>
　　<p>——摘自<cite>《志摩的诗》</cite></p>
```
　　cite,q,blockquote三者虽然都有引用的作用，但却各司其职，互不干涉。

## html5 视频

当前，video 元素支持三种视频格式：

- Ogg = 带有 Theora 视频编码和 Vorbis 音频编码的 Ogg 文件

- MPEG4 = 带有 H.264 视频编码和 AAC 音频编码的 MPEG 4 文件

- WebM = 带有 VP8 视频编码和 Vorbis 音频编码的 WebM 文件

```html
<head>
    <base href="https://www.w3school.com.cn/">
</head>
<body>
    <video src="/i/movie.ogg" width="320" height="240" controls="controls">
        Your browser does not support the video tag.
    </video>
    <!--
    controls,autoplay,height,width, src,loop,preload, 属性。
    <video> 与 </video> 之间插入的内容是供不支持 video 元素的浏览器显示的
    <video ..> <source src=..> <source src=..>  </video> 另外一种source写法。source 元素可以链接不同的视频文件。浏览器将使用第一个可识别的格式
    -->
</body>
```

## 有序、无序列表,以及描述列表

```text
<ul>  
   <li>   </li>
</ul>   

<ol>  
   <li>   </li>
</ol>   
```

描述列表 (description list)中包含：描述部分（description description）和描述术语（description terms）
```text
<dl>
  <dt></dt>
    <dd></dd>
</dl>
```

### 例子

```html
        <aside  style="float:left;width: 50%;">
            <p>Mozilla 是一个全球社区，这里聚集着来自五湖四海的</p>
            <!-- 无序列表（Unordered List） -->
            <ul>
                <li>技术人员</li>
                <li>思考者</li>
                <li>建造者</li>
            </ul>
            <!-- 有序列表（Ordered List） -->
            <ol>
                <li>技术人员</li>
                <li>思考者</li>
                <li>建造者</li>
            </ol>

            <p>我们致力于……</p>
        </aside>

```

对应DOM实现

```kotlin
    var root = document.getElementById("section1")?.
    getElementsByTagName("article")?.asList()
    var article = root!![0]

     var div= document.create.div {
         p { + "Mozilla 是一个全球社区，这里聚集着来自五湖四海的" }
         ul {
             li { + "技术人员" }
             li { + "思考者" }
             li { + "建造者" }
         }
         ol {
             li { + "技术人员" }
             li { + "思考者" }
             li { + "建造者" }
         }
         p { + "我们致力于……" }

     }
    article.appendChild(div)
```

## 其他

### time tag

```html
        <!-- 提供可供机器识别的格式的 <time> 元素 -->
        <time datetime="2019-02-22">2019 年 2 月 22 日</time>
```

### pre and code tag

共同点：都主要应用于浏览器显示计算机中的源代码。
不同点： code标签的一个功能是暗示浏览器code标签所包围的文本是计算机源代码，浏览器可以做出自己的 样式处理，pre标签则没有这项功能，但是pre标签可以保留文本中的空格和换行符，保留文本中的空格和换行符是计算机源代码显示 所必须的样式。

### itemscope、itemtype、itemprop 属性

运用到seo（搜索引擎优化）上，DOM做标记。微数据是有特定属性名称的词汇表中特定的元素属性的数据，它是一种方便机器识别的数据


运用到了seo上，微数据是有特定属性名称的词汇表中特定的元素属性的数据，它是一种方便机器识别的数据

# vscode 相关

对“*.html”文件，快速通过html模板生成html内容：

- 输入“!”，然后按键TAB
- 输入“html”，然后根据提示选择"html:5"

# chrome 相关

[chrome-devtools工具使用官方](https://developers.google.com/web/tools/chrome-devtools)

[chrome-devtools工具使用博客](https://www.cnblogs.com/lxhbky/p/9959587.html)

## console 

### Viewing logged messages

在console中可以查看到log信息

Web developers often log messages to the Console to make sure that their JavaScript is working as expected. To log a message, you insert an expression like console.log('Hello, Console!') into your JavaScript. 

```javascript
console.log('Hello, Console!')
console.warn(
console.error(..)    
```

### Using the Console to change the page's title.

```javascript
document.querySelector("h1").textContent = 'what'
```

### 验证 选择器

在通过F12打开devtool后，再按ESC，使得 console与element在同一界面中这样方便去验证

比如在console中输入下面的代码看有咩有选择到的

#### 采用css selector语法

```js
document.querySelectorAll(".wrap .leftDiv > .demo")
// 与下面的等价
$(".wrap .leftDiv > .demo")
```

#### 采用xpath语法

XPath 使用路径表达式在 XML 文档中进行导航

这个使用"`$x(path)`"方式

# python 相关

### Start a local web server in ~/Desktop/app. 

```sh
# 注意windows计算机名不能包含中文，否则下面命令会出现utf-8问题
cd ~/Desktop/app
python -m SimpleHTTPServer # Python 2
python -m http.server # Python 3
```

