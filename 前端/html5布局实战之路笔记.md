

[web入门](https://developer.mozilla.org/zh-CN/docs/Learn/Getting_started_with_the_web)

[web在线实验](https://codepen.io/pen/)

[圖解：CSS Flex 屬性一點也不難](https://wcc723.github.io/css/2017/07/21/css-flex/)

[【前端】向blog或网站中添加语法高亮显示代码方法总结](https://www.cnblogs.com/dragonir/p/7426965.html)

html5 = html + css + js

### 文件夹建议

文件夹：

```sh
$mkdir -p {cn,en,images,css,js,imagestudio,flash,media,font,project,temp}
```

- temp: 存放客户原始子类
- poject 存放工程项目子类
- font 存放自定义字体
- media 存放多媒体
- flash 存放flash
- imagestudio 存放psd
- cn 存放中文网页
- en 存放因为网页


### 移动端开发时页面建议

为了使移动端的页面在不同的手机上同样的大小来显示，我们可以将页面的宽度固定，使用`<meta name="viewport" content="width=device-width, initial-scale=1.0">`，具体说明见 [关于HTML5中meta name="viewport" 的用法](https://blog.csdn.net/weixin_41143293/article/details/81948183)

### 自动刷新与跳转

在vscode html中输入meta选择redirect，出现片段

- 3s后跳转到url

```html
    <meta http-equiv="refresh" content="3; url=http://example.com">
```

- 3s后跳刷新，无转到

```html
    <meta http-equiv="refresh" content="3">
```

### em与strong tag区别

HTML5 草案 中做了详尽解释：

　　The em element represents stress emphasis of its contents. The placement of emphasis changes the meaning of the sentence.

　　The strong element represents strong importance for its contents. Changing the importance of a piece of text with the strong element does not change the meaning of the sentence.

　　em 表示内容的着重点(stress emphasis)，strong 表示内容的重要性(strong importance)，strong 不会改变所在句子的语意，em 则会改变所在句子的语义。

他们表达的都是逻辑上的语义， 对应的`“<b>, <i>”`粗体、斜体是一种物理语义，没有逻辑语义。显然我们应用有逻辑语义的


### 配合搜索引擎建议

vscode 中快捷片段：`link:favicon`,`meta:kw`,`meta:desc`

- 以下几个建议都要加上

```html
    <meta name="description" content="我的第一个文件">
    <!-- 这个网页的关键词，可以多个，用逗号分隔 -->
    <meta name="keywords" content="练习,读书">

    <link rel="shortcut icon" href="favicon.ico" type="image/x-icon">
```

### css 引入方式

下面三种方式的优先级是依次递减

- online

```html
    <div  style="border:1px  solid blue">1</div>
```

- 内部书写

```html
<head>
    <style>
        div {
            /* 5 px 黑色实线边框 */
            border: 5px solid black;
        }
    </style>
</head>
```

- 外部引入

```html
<link rel="stylesheet" href="style.css">
```

或下面方式，这种方式不建议，它不支持js

```html
<head>
    <style type="text/css">
        @import url("...")
    </style>
</head>
```

### 三种基本选择器方式

对同一个对象，同一种样式设置时：style引入的样式优先级是最高的，然后是id引入的，然后class引入的，最后是标签名引入的

而对没有冲突覆盖的，会进行叠加

对选择器优先使用类名选择器，因为它可以同时选择多个元素，更加灵活。像id选择器，一般是给js使用的。

#### id选择器

```text
#选择器名(id) {
    属性名:属性值;
    属性名:属性值;
}
```

例子

```html
    <style>
        #id1 
        {
            border:1px  solid blue;
        }
    </style>

    <div id="id1" >1</div>
```

#### 类名选择器

```text
.选择器名(类名) {
    属性名:属性值;
    属性名:属性值;
}
```

例子

```html
    <style>
        .class1 
        {
            border:1px  solid yellow;
        }
    </style>
</head>
<body>

    <div class="class1" >1</div>
```

#### 标签名选择器

```text
选择器名(标签名) {
    属性名:属性值;
    属性名:属性值;
}
```

例如下面的例子对标签dv制定css样式
```html
    <style>
        div 
        {
            border:1px  solid yellow;
        }
    </style>
</head>

```

#### 属性选择器


例如，为了将同时有 href 和 title 属性的 HTML 超链接的文本设置为红色，可以这样写：

```css
a[href][title] {color:red;}
```

### 加强的后代、子代选择器

如果是后代关系 不同选择器使用空格分开， 如果是子代关系，父子关系，不同选择器使用“>” 分开

约束逻辑关系从左往右逐渐加强

```text
选择器名1 选择器名2 选择器名3 ... {
    属性名:属性值;
    属性名:属性值;
}
```

例子

```css
/* id为wrap中里面的所有标签为p的元素，即所有后代不论隔多少层适用列出的规则 */
#wrap p {
    ...
}
```

### css样式重置

不同浏览器的默认样式有差异，因此推荐先进行样式重置.之所以存在默认样式，因为html出现在css前，所以才会有浏览器默认样式。

- reset.css  文件

下面这个是书上提供的范例

```css
@charset 'utf-8';
html{color:#000;background:#FFF;font-family:'Microsoft YaHei',sans-serif,Arial;}
body,div,dl,dt,dd,ul,ol,li,h1,h2,h3,h4,h5,h6,pre,code,form,fieldset,legend,input,button,textarea,p,blockquote,th,td,strong{padding:0;margin:0;font-family:'Microsoft YaHei',sans-serif,Arial;}
table{border-collapse:collapse;border-spacing:0;}
img{border:0;}
a{text-decoration:none; color:#333; outline:none;}
a:hover{text-decoration:underline;}
var,em,strong{font-style:normal;}
em,strong,th,var{font-style:inherit;font-weight:inherit;}
li{list-style:none;}
caption,th{text-align:left;}
h1,h2,h3,h4,h5,h6{font-size:100%;font-weight:normal;}
input,button,textarea,select,optgroup,option{font-family:inherit; font-size:inherit;font-style:inherit;font-weight:inherit;}
input,button,textarea,select{*font-size:100%;}
.clearfix {*zoom: 1;}
.clearfix:after {content: '\200B';clear: both;display: block;height: 0px;}
```

### 群组选择器写法

所谓群组选择器，是一种将规则相同选择器合并的写法

例如

```css
.wrap {
    规则A
}
p {
    规则A
}
```

可以采用如下群组写法

```css
.wrap,p {
    规则A
}
```



### 浏览器解析设置了“纵向外边距叠加＂的规则。

最初HTML 中的p 标签都存在默认的上下外边距，为的是和别的元素之间产生一定的距离。但是用在段落当中时，多个p 上下排列，每两个p 元素之间的距离就变得有些大了（上面p 元素的下外边距＋下面p 元素的上外边距），出于这样的考虑，浏览器解析设置了“纵向外边距叠加＂的规则。

当不需要这个默认处理时，可以按照：

解决这个问题并不难，可以为元素设置浮动，纵向外边距就不再叠加（浮动的知识在第4 章进行讲解）；另外一种方法则是规避掉这个问题，即为元素统一设置上或下，某一个方向的外边距，这样就不会出现重叠的现象和问题了。

### 父子之间用padding ，兄弟之间用margin


推荐例子。 在这个例子中，B/C都没有设置内容区的width/HEIGHT，但由于parent（A）设置了left/right padding, 所以就相当于自动auto规定了，这样导致B/C的board距离A的左右boad是55px

```html

    <style>
        .A {
            width: 200px;
            padding: 73px 55px;
            border: 3px solid #555;
        }

        .B {
            height: 194px;
            margin-bottom: 42px;
            border: 3px solid #555;
        }

        .C {
            height: 194px;
            border: 3px solid #555;
        }
    </style>
</head>

<body>

    <div class="A">
        <div class="B"></div>
        <div class="C"></div>
    </div>

</body>
```

# 整体布局

见书 第三章，第四章

`<div>`元素是整体布局唯一需要使用的html元素

## 样式属性分类

这些样式属性可以简单地划分为4 大类，分别是“显示样式”、“自身样式”、“文本样式”和“CSS3 新样式” 。

- (1) 显示样式：控制元素展示方式的属性，主要包括浮动(float 入定位(position) 、展示方式(display汃超出状态以及可视化(overflow 、visibility) 等。在第4 、5 、6 、8 章当中会进行介绍。

- (2) 自身样式：关千元素自身的样式属性，主要就是本章涉及的5 种属性（宽度width 、高度height 、外边距margin 、内边距padding 、边框border) 以及第9 章涉及的最大最小宽高。

- (3) 文本样式：用于处理背景图片、段落文章、文字字体的样式。第7 章中的各类样式均属于文本样式。

- (4) CSS3 新样式： CSS3 新增的属性，第13~16 章当中讲解的属性。


浏览器渲染是按照 “显示样式” --> “自身样式” --> “文本样式” --> “CSS3 新样式” 顺序.

在此给出比较完整的代码书写顺序（推荐）：
  - • display;
  - • position;
  - • position 相关的left 、top 、right 、bottom 、- z-index;
  - • float;
  - • clear;
  - • width;
  - • height;
  - • margin;
  - • padding;
  - • border;
  - • background;
  - • color;
  - • font;
  - • text-decoration;
  - • text-align;
  - • vertical-align;
  - • white-space ;
  - • text-XXX（其他的text 类属性）；
  - • CSS3 类。

# div盒模型

完整的盒模型是由width 、height （宽度和高度构成一＞内容）、border（边框）、padding （内边距）、margin （外边距）这几部分属性组成。 

div的width/height默认是auto，auto的意思是由它的子标签的宽/高来决定。其他默认为0

下面的例子较好的演示了这些属性，有空多看看

```html
    <style>
		.box1 {
			width: 100px;
			height: 100px;
			border-width: 50px;
			border-style: solid;
			border-color: red black green yellow;
		}
		.box2 {
			width: 0px;
			height: 0px;
			border-width: 50px;
			border-style: solid;
			border-color: red black green yellow;
		}
</style>
</head>

<body>
	<div class="box1"></div>
	<div class="box2"></div>
</body>
```


## margin： 外边距，可以理解为当前元素与父级或其他兄弟级元素之间的距离。

    例子

    ```css
    /* 对应顺序为“上－ 右－下－左”（从顶部，顺时针转下来） */
    /* 为三个值时，值与方向的对应顺序为“上－左右－下” */
    /* 为两个值时，值与方向的对应顺序为“上下－左右＂。 */
    /* 为一个值时，该值表示4 个方向的外边距均设置为这个属性值。 */
    margin: 5px 5px 5px 5px 
    ```

    注意：当不设置width时，会占据父级内容区的100% 。可以认为`margin:O auto ；`的代码是生效的，只是没有剩余的区域留给margin 值。

    注意：margin可以设置为负值


## padding: 外边距，可以理解为当前元素与元素边框之间的距离。

    例子说明参见margin


## border


    有 `border-[left|right|top]-width`,`border-[left|right|top]-style`,`border-[left|right|top]-color`三类设置

    对ing的有几种缩写方式:

    - boder-width: 可以取4个值，含义同margin
    - boder-style: 可以取4个值，含义同margin
    - boder-color: 可以取4个值，含义同margin
    - boder-[left|right|top] : 需要包含边框宽度、边框样式、边框颜色
    - boder: 需要包含边框宽度、边框样式、边框颜色

    例子

    ```css
    border:1px  solid yellow;
    ```

    注意：如果不为div 元素设置宽度，但设置了`n1argin: 0 auto ；`会怎么样？
    
    当div 没有设定固定宽度时，默认这个div 会占据父级内容区的100% 。可以认为`margin:O auto ；`的代码是生效的，只是没有剩余的区域留给margin 值。

## width, height

width/height 构成内容区。注意他们不设置时代表auto


# 浮动

在盒模型中提到过， div 元素属于块元素，块元素默认宽度是占满父级的，如果针对div设置了宽度，这个div 的盒模型尺寸虽然成为所设置的大小，但是依旧会“霸占＂着整个一行，不让别的元素使用。在网页布局当中，常常需要将块并排进行布局， div 标签无法实现这个需求，那么其他元素（标签）也不能实现

如果希望div 或其他元素”既能够设置宽高，又能够跟其他元素处千同一行“，就需要使用到浮动。

- 属性功能：设置一个元素发生浮动，浮动后的元素默认大小为内容大小，并且可以设置宽高，也可以与其他浮动元素或行元素处于同一行。 **想象个水槽来理解float**

## 左浮，与右浮共存

在浮动的过程中，左右浮动的模块井不互相干涉， 左浮动的模块有可能会阻挡其他左浮动的模块，右浮动的模块有可能会阻挡其他右浮动的模块．但是，无论左浮动还是右浮动，都会占据空间，因此在最终的空间占用上，还是会互相有所影响。

**浮动过程中，左浮与右浮互不干涉，但一旦最终落地，还是要看空间占位**

上面这个具体的例子见下面代码片段

```html
	<style>
		.wrap {
			width: 600px;
			border: 5px solid black;
		}
		.box1 {
			float: left;
			width: 150px;
			height: 150px;
			background: #999;
			/* 如下三行代码只是为了便于查看效果 */
			font-size: 40px;
			text-align: center;
			line-height: 150px;
		}
		.box2 {
			float: left;
			width: 150px;
			height: 300px;
			background: #ccc;
			/* 如下三行代码只是为了便于查看效果 */
			font-size: 40px;
			text-align: center;
			line-height: 300px;
		}
		.box3 {
			float: right;
			width: 150px;
			height: 150px;
			background: blue;
			/* 如下三行代码只是为了便于查看效果 */
			font-size: 40px;
			text-align: center;
			line-height: 150px;
		}
		.box4 {
			float: right;
			width: 150px;
			height: 200px;
			background: yellow;
			/* 如下三行代码只是为了便于查看效果 */
			font-size: 40px;
			text-align: center;
			line-height: 200px;
		}
	</style>
</head>
<body>
	<div class="wrap clearfix">	
		<div class="box1">1</div>
		<div class="box2">2</div>
		<div class="box3">3</div>
		<div class="box4">4</div>
	</div>
</body>
```

## 浮动影响

### 浮动元素对父级元素高度的影响



### 浮动元素对兄弟级元素布局的影响


浮动元素会对兄弟级元素造成影响，但是仅针对它后面的兄弟级元素，并不会对前面的兄弟级元素的布局造成影响。


## TODO 浮动清除

### 当一个元素中同时含有浮动，非浮动元素时

先放置浮动，再放置非浮动元素，对非浮动元素的位置会有影响。 下面演示了怎么去除该影响。

具体看下面这个例子，注意到其中clear去清除浮动的作用，以及width、height如果不设置自动扩充为parent剩下空间的作用

同时注意这里面使用背景色的技巧，有利于debug与最终生产代码的隔离

```html
<!doctype html>
<html>
<head>
	<meta charset="UTF-8">
	<title>HTML5布局之路 - 较为复杂的浮动布局案例</title>
	<link rel="stylesheet" href="../css/reset.css">
	<style>
		.wrap {
			width: 1000px;
			margin: 0 auto;
		}
		.common {
			float: left;
			width: 200px;
			height: 200px;
		}
		.left {
			float: left;
			width: 600px;
		}
		.right {
			float: right;
			width: 400px;
		}
		.left-common {
			float: left;
			width: 300px;
			height: 300px;
		}
		.box8 {
			clear: both;
			height: 100px;
		}
		.box9 {
			height: 150px;
		}
		.box10 {
			height: 250px;
		}
		.box11 {
			clear: both;
			height: 80px;	
		}
		/* 如下代码只是控制背景颜色，后期会删除，出于代码篇幅考虑，如下代码采用单行形式进行书写 */
		.box1 {background: #ccc;}
		.box2 {background: #a9a9a9;}
		.box3 {background: #ccc;}
		.box4 {background: #a9a9a9;}
		.box5 {background: #ccc;}
		.box6 {background: #999;}
		.box7 {background: #d9d9d9;}
		.box8 {background: #ccc;}
		.box9 {background: #999;}
		.box10 {background: #a9a9a9;}
		.box11 {background: #d9d9d9;}
	</style>
</head>
<body>
	<div class="wrap">
		<div class="common box1">1</div>
		<div class="common box2">2</div>
		<div class="common box3">3</div>
		<div class="common box4">4</div>
		<div class="common box5">5</div>
		<div class="left">
			<div class="left-common box6">6</div>
			<div class="left-common box7">7</div>
			<div class="box8">8</div>
		</div>
		<div class="right">
			<div class="box9">9</div>
			<div class="box10">10</div>
		</div>
		<div class="box11">11</div>
	</div>
</body>
</html>
```

### 当一个元素只包含浮动元素的时候，它会出现高度折叠,即元素的上下底边重合，和高度为0效果一样。

解决： 对假设为“class=article”的元素施加下面的代码，解决该.article所指示的元素坍塌问题：

```css
        .article:after {
            content: '\200B';clear: both;
            display: block;height: 0px;}
```

注意“:”冒号的前后不能有空格


# 布局时选择使用哪些tag？

见原书第五章

浏览器的默认字体大小规定为1em。

hl 为2em,h2 为1. Sem, h3 为1 今17em,h4 为lem,h5 为0.83em,h6 为0. 67em 。（ 此处小数点精
确到了第二位， em 表示字符大小，浏览器默认字体大小为16 像素， lem= 16px; 如果字体
大小设置为20px ，那么lem= 20px 。)

`<br>`代表一个软换行，和`<p>`代表一个段落不同。

TODO： 这章中的  块元素，与行元素的特定




# 模仿用户的搜索行为

[特大搜索黑产案曝光：黑百度 两年赚了7000万](https://www.sohu.com/a/218134467_609556)


# 可用性与可扩展性

## a tag

### 调整扩大可接触区

下面这个例子，表示将a的的可触区扩展到与父级留给它的空间一样大

```html
    <style>
        .tit a {
            display: block;
            height: 100%;
        } 
    </style>
<h2 class="tit"><a href="http://www.baidu.com" title="">存在链接的文章标题</a></h2>
```

## img tag

### img 图片底部存在几个像素的空白问题

这是个历史遗留问题，当初html设计出来时就专门留了这个空白

下面的例子演示了这个空白，为了解决这个问题通常解决方案是为img增加“`display: block;`”, 另外对img施加浮动也同样可以去掉该留白。

```html
	<style>
		.wrap {
			border: 5px solid red;
		}
		.wrap img {
			width: 100px;
			height: 100px;
		}
	</style>
</head>
<body>
	<div class="wrap">
		<img src="https://bkimg.cdn.bcebos.com/pic/4bed2e738bd4b31cb8fb700485d6277f9f2ff84e?x-bce-process=image/resize,m_lfit,w_268,limit_1/format,f_jpg" alt="HTML5" title="">		
	</div>
</body>
```

### 图像需要设置大小，避免图像尺寸变化引起布局问题


规避的方式比较简单，

- option1： 为img 设置固定的宽度和高度即可．此处可以为img 标签设置具体的像素值，也可以与其父级元索挂
钩，

- opton2：如果img 标签的父级元素有固定的宽度和高度，且img 标签需要占满父级元素的全部，可以对img 标签使用百分比这种相对度量单位

例如下面的例子是推荐的方法，其中“display: block”解决图片底部留空的问题

```css
		/* 存放img的parent */
		.parent {
			float: left;
			width: 152px;
			height: 152px;
			padding: 24px;
        }
		img {
			display: block;
			width: 100%;
			height: 100%;
		}        
```

## 内容超出处理

典型方法是在文本容器中添加

```css
            overflow: hidden;
            text-overflow: ellipsis;
```

还有个通常做法是通过代码，比如假设对id为con中的文本按照下面的代码处理，最多显示49个字符，多余的显示省略号

```html
	<script>
	    var con = document.getElementById('con');
	    var textCon = con.innerHTML;
	    con.innerHTML = textCon.substring(0, 49) + '…';
	</script>
```

# 定位布局

原书第八章

，最为常见的有两种布局模式，分别是浮动布局和定位布局。在日常开发当中，使用最多的是浮动布局，当在页面中出现多个元素层叠状态时，会考虑定位布局，例如平时的弹窗、黑色半透明蒙版层、返回顶部、微博等网站顶部的导航条等。

## 绝对定位元素针对谁进行定位

如果父级（无限）没有设定position 属性，那么当前的absolute 则结合top, right, left,bottom 属性以浏览器左上角为原始点进行定位。

如果父级（无限）设定position 属性，且属性值为relative 、absolute 、fixed ，那么当前的absolute 则结合top, right, left, bottom 属性以父级（最近）的左上角为原始点进行定位。

# TODO 内容超过边界


