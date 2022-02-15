
[get started](https://www.wolfram.com/language/fast-introduction-for-math-students/zh/entering-input/?source=footer)

 [mathematica 产品下载安装](https://www.0daydown.com/12/1262839.html)。 ， https://pan.baidu.com/share/init?surl=LAnV1vxFBJK9mUM6GUoBKw pass:8at6

[在Markdown文件里面直接运行Mathematica](https://zhuanlan.zhihu.com/p/429293727)

## 内容输入

菜单-->格式-->样式

title输入：  ALT 1
SECTION输入：  ALT 4
SUB SECTION输入： ALT 5
TEXT 输入： ALT 7
数学内容输入： ALT 8

 ## 数学内容输入

 输入内容，然后按下 SHIFT+ENTER 进行计算

 "%"代表最近一次的output，使用它方便写表达式

 函数：用逗号分隔内置函数的参数，并将其置于方括号内。例如“`GCD[12, 15]`”.如果不知道使用哪个函数，在行的开始处键入 = 即可进行自然语言输入,可以得到提示。注意内置函数都是以大写字母开头的，我们要遵循这个规则，自己定义的变量不要以大写字母开头。

 列表（list）：列表是有序的. 可以包含数字、变量、计算，甚至是其他列表。比如列表"{1,2,3}",比如列表"{1,2,3,{1,2,3}}". 许多运算是被应用到每个元素上的. 比如"{1, 2, 3} + 2"得到的是列表"{3,4,5}" . 从 1 开始，可以用 [[ ... ]] 来提取列表的元素,比如"{a, b, c, d}[[3]]"得到"c". 可以使用Range函数来构建列表,比如"Range[10]",得到列表"{1,2,3,4,5,6,7,8,9,10}"

 分数：用 CTRL+ / 输入分数。 对于分数可以用“Together”函数将分数表达式表示成最小公分母的形式。

 指数： 用“CTRL+6”输入排版式指数


 对分数、小数有两个常用的内置函数“N[expr]”，与“ScientificForm[expr]”

角度输入:键入 ESC deg ESC 可输入 ° 符号.

弧度Pi输入: 键入 ESC pi ESC 来输入 π 字符

极坐标：键入 ESC th ESC 可得到 θ 符号.

## 变量和函数

变量建议以大写字母开头，因为内置函数是以大写字母开头的

```
用 = 符号赋值： In[1]:=	x = 2
               Out[1]=	2
 
在表达式和命令中使用变量：In[2]:= 1 + 2 x
                        Out[2]=	5
 
清除赋值，x 保持未计算状态：In[3]:=	Clear[x]
                          Out[3]= 1 + 2 x
```

在写一个含变量的表达式时，如果要将变量替换掉，可以用“/.->”替换掉变量。例如"1+2x/.->2"得到结果是5

采用"f[x_]:="来定义自定义函数.例如"ff[y_]"=y+1"定义后, "ff[2]"的结果是3


## 二维绘图

多项式的二维曲线：Plot。 例如“Plot[x+1, {x,-10,10}]”

不等式的二维区域：RegionPlot. 例如"RegionPlot[x+y<10,{x,-3,3},{y,-1,1}]"


通过函数Show将多个不同的图一起绘制，例如"Show[{Plot[...],RegionPlot[...]}]"

## 三角函数

用恒等式自动展开（或化简）三角表达式：
```
In[1]:=	TrigExpand[Sin[2 x]]
Out[1]=	2Cos[x]Sin[x]
```

因式分解三角多项式：
```
In[2]:=	TrigFactor[Cos[x]^2 - Sin[x]^2]
Out[2]=	2Sin[π/4 -x]Sin[π/4 +x]
```

## 导数derivatives

