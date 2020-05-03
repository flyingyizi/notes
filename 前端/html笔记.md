


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

# python 相关

### Start a local web server in ~/Desktop/app. 

```sh
# 注意windows计算机名不能包含中文，否则下面命令会出现utf-8问题
cd ~/Desktop/app
python -m SimpleHTTPServer # Python 2
python -m http.server # Python 3
```

