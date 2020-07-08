  

![scrapy架构图](./images/scrapy_architecture_02.png)

[博客](https://www.cnblogs.com/my8100/tag/scrapy/)

[自动批量关注微信公众号（非逆向）](https://blog.csdn.net/wnma3mz/article/details/105448808)

[scrapy-selenium](https://github.com/clemfromspace/scrapy-selenium)

https://stackoverflow.com/questions/17975471/selenium-with-scrapy-for-dynamic-page

# 环境准备：

以下都是基于python3进行开发， 后面不再说明。 对anaconda环境安装请见 “python笔记.md”中windows安装部分的说明。

python，ipython也请按照要求的版本号，其他版本会出现在ipython中autocomplete时出现莫名的debug信息

```shell
#安装python 3.7，高版本存在time module兼容问题
conda create -n spider python=3.7
conda activate spider
#先安装ipython
conda install ipython==7.0.1
#再安装scrapy
conda install scrapy=2.1.0

#生成项目
scrapy startproject yourproject_name
scrapy crawl the_name_in_your_class_of_yourcode
```

以下是第一个最简单的爬虫，当然它什么也不会做

```sh
(myenv) c:\tuxueqing\spider>scrapy startproject my
New Scrapy project 'my', using template directory 'C:\prog\Anaconda3\envs\myenv\
lib\site-packages\scrapy\templates\project', created in:
    c:\tuxueqing\spider\my

You can start your first spider with:
    cd my
    scrapy genspider example example.com

(myenv) c:\tuxueqing\spider>
```

[scrapy  官方文档](https://docs.scrapy.org/en/latest)

[scrapy1.5中文文档](http://www.scrapyd.cn/doc/)

[Scrapy 1.7中文文档](https://www.osgeo.cn/scrapy/)

https://miyakogi.github.io/pyppeteer/reference.html


# scrapy命令

[scrapy命令官方文档](https://docs.scrapy.org/en/latest/topics/commands.html)

scrapy命令支持参数如下所示：

```shell
#列出scrapy支持的所有命令
(myenv) PS C:\spider> scrapy --help
Scrapy 2.1.0 - no active project

Usage:
  scrapy <command> [options] [args]

Available commands:
  bench         Run quick benchmark test
  fetch         Fetch a URL using the Scrapy downloader
  genspider     Generate new spider using pre-defined templates
  runspider     Run a self-contained spider (without creating a project)
  settings      Get settings values
  shell         Interactive scraping console
  startproject  Create new project
  version       Print Scrapy version
  view          Open URL in browser, as seen by Scrapy
  
  ## 以下几个命令仅仅会在一个scrapy project内才会出现
  check         Check spider contracts
  crawl         Run a spider
  edit          Edit spider
  list          List available spiders
  parse         Parse URL (using its spider) and print the results

Use "scrapy <command> -h" to see more info about a command
(myenv) PS C:\tuxueqing\spider>
```

下面对每个命令的使用尝试下，同时记录关注点

## scrapy view

`scrapy view [options] <url>`这个命令主要目的是查看下scrapy看到的内容是什么，因为我们通过浏览器访问url与scrapy看到url的内容可能是不一样的。通过它能更方便我们编写爬虫。

注意，这个命令需要在有GUI的系统中运行，因为它需要打开个browser显示scrapy视角看到的网页，如果是一个文本终端就没有显示了、

```shell
(myenv) PS C:\tuxueqing\spider\my> scrapy view --help
Usage
=====
  scrapy view [options] <url>

Fetch a URL using the Scrapy downloader and show its contents in a browser

Options
=======
--help, -h              show this help message and exit
--spider=SPIDER         use this spider
--no-redirect           do not handle HTTP 3xx status codes and print response as-is
...
(myenv) PS C:\tuxueqing\spider\my>
```

## scrapy genspider

这个命令是为我们的项目用来新增加个spider模板的，建立这个模板可以基于系统内置的模板（通过`scrapy genspider --list`得到系统模板列表），换句话说就是为我们编码减少些工作量的，我们铺代码可以基于生成的模板增加我们自己的代码。

该命令支持的参数如下：

```shell
(myenv) PS C:\tuxueqing\spider\my> scrapy genspider -h
Usage
=====
  scrapy genspider [options] <name> <domain>

Generate new spider using pre-defined templates

Options
=======
--help, -h              show this help message and exit
--list, -l              List available templates
--edit, -e              Edit spider after creating it
--dump=TEMPLATE, -d TEMPLATE
                        Dump template to standard output
--template=TEMPLATE, -t TEMPLATE
                        Uses a custom template.
--force                 If the spider already exists, overwrite it with the
                        template
...
(myenv) PS C:\tuxueqing\spider\my>
```

### 初始化项目的一个爬虫

下面是一个使用的典型例子，创建的模板文件位于“`C:\tuxueqing\spider\my\my\spiders\example.py`”:

```shell
#先创建一个项目
(myenv) PS C:\tuxueqing\spider> scrapy startproject my
New Scrapy project 'my', using template directory 'C:\prog\Anaconda3\envs\myenv\
lib\site-packages\scrapy\templates\project', created in:
    C:\tuxueqing\spider\my

You can start your first spider with:
    cd my
    scrapy genspider example example.com
(myenv) PS C:\tuxueqing\spider> cd my
#采用系统内置crawl模板新创建个spider模板，
(myenv) PS C:\tuxueqing\spider\my> scrapy genspider -t crawl  example example.com
Created spider 'example' using template 'crawl' in module:
  my.spiders.example
(myenv) PS C:\tuxueqing\spider\my>
```

该"example.py"文件的内容如下：

```python
# -*- coding: utf-8 -*-
import scrapy
from scrapy.linkextractors import LinkExtractor
from scrapy.spiders import CrawlSpider, Rule

class ExampleSpider(CrawlSpider):
    name = 'example'
    allowed_domains = ['example.com']
    start_urls = ['http://example.com/']

    rules = (
        Rule(LinkExtractor(allow=r'Items/'), callback='parse_item', follow=True),
    )

    def parse_item(self, response):
        item = {}
        #item['domain_id'] = response.xpath('//input[@id="sid"]/@value').get()
        #item['name'] = response.xpath('//div[@id="name"]').get()
        #item['description'] = response.xpath('//div[@id="description"]').get()
        return item
```
### 查看系统模板

下面的参数演示了： 查看系统有哪些模板； 将模板内容导出到stdout查看

```shell
# 查看有哪些 系统模板
(myenv) PS C:\tuxueqing\spider\my> scrapy genspider -l
Available templates:
  basic
  crawl
  csvfeed
  xmlfeed

# 查看xmlfeed模板的内容  
(myenv) PS C:\tuxueqing\spider\my> scrapy genspider -d xmlfeed
# -*- coding: utf-8 -*-
from scrapy.spiders import XMLFeedSpider

class $classname(XMLFeedSpider):
    name = '$name'
    allowed_domains = ['$domain']
    start_urls = ['http://$domain/feed.xml']
    iterator = 'iternodes' # you can change this; see the docs
    itertag = 'item' # change it accordingly

    def parse_node(self, response, selector):
        item = {}
        #item['url'] = selector.select('url').get()
        #item['name'] = selector.select('name').get()
        #item['description'] = selector.select('description').get()
        return item

(myenv) PS C:\tuxueqing\spider\my>
```

## scray settings

scrapy配置可以是系统级也可以是项目级别的，通常都设置项目级。通过这个命令可以查看配置中配置项的值。 在下面演示中，演示了

```shell
(myenv) PS C:\tuxueqing\spider\my> scrapy settings -h
Usage
=====
  scrapy settings [options]

Get settings values

Options
=======
--help, -h              show this help message and exit
--get=SETTING           print raw setting value
--getbool=SETTING       print setting value, interpreted as a boolean
--getint=SETTING        print setting value, interpreted as an integer
--getfloat=SETTING      print setting value, interpreted as a float
--getlist=SETTING       print setting value, interpreted as a list
...
#就是项目中setting.py里面的BOT_NAME的值
(myenv) PS C:\tuxueqing\spider\my> scrapy settings --get=BOT_NAME
my

#就是项目中setting.py里面的ROBOTSTXT_OBEY的值
(myenv) PS C:\tuxueqing\spider\my> scrapy settings --get=ROBOTSTXT_OBEY
```

## scrapy runspider与scrapy crawl

`scrapy crawl [options] <spider>`与"`scrapy runspider [options] <spider_file>`"

它们的差异包括以下几点：

- `<spider>` is the project name (在scrapy project项目目录下通过`scrapy list` 可以得到列表).
- `<spider_file>` 是spider全路径文件名，is the path to the file that contains the spider.
- `scrapy crawl`是依赖有scrapy project存在的，而`scrapy runspider`不依赖

scrapy crawl 命令支持的参数：

```shell
(base) PS C:\tuxueqing\spider\my> scrapy crawl -h
Usage
=====
  scrapy crawl [options] <spider>

Run a spider

Options
=======
--help, -h              show this help message and exit
-a NAME=VALUE           set spider argument (may be repeated)
--output=FILE, -o FILE  dump scraped items into FILE (use - for stdout)
--output-format=FORMAT, -t FORMAT
                        format to use for dumping items with -o
...
```

### 常用arguments

#### -s

`-s CLOSESPIDER_ITEMCOUNT=90`

系统默认配置项列表，可以在"scrapy\settings\default_settings.py"中查看到，也可以查看项目中的"scrapy.cfg"，但一般这里面比较少。

#### -o
对“`-o FILE`”, 会自动根据sheme保存为特定格式

例如：

```shell
#保存为json
$ scrapy crawl basic -o items.json
$ cat items.json
[{"price": ["334.39"], "address": ["Angel, London"], "description":
["website court… offered"], "image_urls": ["../images/i01.jpg"], "title":
["set unique family well"]}]

#保存为json，数据在一行
$ scrapy crawl basic -o items.jl
#保存为csv
$ scrapy crawl basic -o items.csv
#保存为xml
$ scrapy crawl basic -o items.xml
#通过ftp协议
$ scrapy crawl basic -o "ftp://user:pass@ftp.scrapybook.com/items.json "
#通过s3协议
$ scrapy crawl basic -o "s3://aws_key:aws_secret@scrapybook/items.json"
```

#### -a

对`"-a"`参数，这个传递的是spider argument，典型的spider argument通常写到该spider的`def start_requests(self, arguments)`  参数中，当然如果


### 常见问题

#### 导出json为unicode问题

通过"`-o xxxx.json`"导出爬取结果，往往中文的内容显示为ascii化的unicode，这看请来很不方便。

这个问题的原因是python json库的`json.dumps`默认处理就是将unicode安装ascii化导出。 解决这个问题的方式是：

在`project settings.py` 中配置 `FEED_EXPORT_ENCODING = 'utf-8'`

## scrapy list

在scrapy project中列出有几个spider，以及他们的名字

例如

```shell
(myenv) PS C:\tuxueqing\spider\my> scrapy list
example
```

## scrapy edit

通过该命令来打开编辑器去编辑spider，通常我们编程都是常常打开这编辑器，这个方式不太有必要，但如果需要也可以通过这个方式来使用。这个命令使用的编辑器时由环境变量`EDITOR`设置的。

使用看下面例子

```shell
#查看spider name
(myenv) c:\tuxueqing\spider\my>scrapy list
example

#设置编辑器为code，在这次环境中code是vscode已经放入了全局path中
(myenv) c:\tuxueqing\spider\my>set EDITOR=code
#编辑该spider，运行该命令后将会看到code被拉起，并且打开了example.py文件
(myenv) c:\tuxueqing\spider\my>scrapy edit example
(myenv) c:\tuxueqing\spider\my>
```

## scrapy bench

测试当前机器的抓取性能

执行该命令可能会出现类似"`twisted.internet.error.CannotListenError: Couldn't listen on any:8998`"错误，如果出现这种错误，建议采用下面的方式查出占用端口的进程，然后杀掉进程重新尝试

```sh
#查找出进程id
(myenv) PS C:\tuxueqing\spider> netstat -ano|findstr "8998"
  TCP    0.0.0.0:8998           0.0.0.0:0              LISTENING       3156
#查找出该进程id对应是哪个进程  
(myenv) PS C:\tuxueqing\spider> tasklist | findstr "3156"
SWVisualize.BoostService.     3156 Services                   0     10,608 K
(myenv) PS C:\tuxueqing\spider>
```

上面是windows下查找进程的方法，相比下liunx更简单，因为“p”参数直接就显示了“pid、name”信息，通常采用`sudo netstat -antup`命令


```shell
atmel:~$scrapy bench
...
2020-06-01 10:23:38 [scrapy.statscollectors] INFO: Dumping Scrapy stats:
{'downloader/request_bytes': 246351,
 'downloader/request_count': 549,
 'downloader/request_method_count/GET': 549,
 'downloader/response_bytes': 1697683,
 'downloader/response_count': 549,
 'downloader/response_status_count/200': 549,
 'elapsed_time_seconds': 10.565515,
 'finish_reason': 'closespider_timeout',
 'finish_time': datetime.datetime(2020, 6, 1, 2, 23, 38, 792904),
 'log_count/INFO': 20,
 'memusage/max': 55541760,
 'memusage/startup': 55541760,
 'request_depth_max': 21,
 'response_received_count': 549,
 'scheduler/dequeued': 549,
 'scheduler/dequeued/memory': 549,
 'scheduler/enqueued': 10981,
 'scheduler/enqueued/memory': 10981,
 'start_time': datetime.datetime(2020, 6, 1, 2, 23, 28, 227389)}
2020-06-01 10:23:38 [scrapy.core.engine] INFO: Spider closed (closespider_timeout)
atmel:~$
```



































## scrapy fetch

这个命令对对应url的body或header下载下来输出到stdout,注意url必须带有sheme，并且只支持单个url，如果输入多个urls，不会有结果。

使用举例：`scrapy fetch --headers --nolog http://www.baidu.com`

```shell
#列出fetch指令支持的参数
(myenv) PS C:\tuxueqing\spider> scrapy fetch -h
Usage
=====
  scrapy fetch [options] <url>
...
Options
=======
--help, -h              show this help message and exit
--spider=SPIDER         use this spider
--headers               print response HTTP headers instead of body
...
```

## scrapy parseTODO

调试的时候用，比如检查某个特定parse的输出

例如

```python
$ scrapy parse --spider=basic http://xxxx

```

## scrapy checkTODO

支持的参数

```shell

(spider) c:\tuxueqing\spider\my>scrapy check --help
Usage
=====
  scrapy check [options] <spider>

Check spider contracts

Options
=======
--help, -h              show this help message and exit
--list, -l              only list contracts, without checking them
--verbose, -v           print contract tests for all spiders

Global Options
```

`Contracts`合约类似于spdier的单元测试(unit tests for spiders). 能让你快速检查是不是有问题发生了，比如你在几周前写了个爬虫，你希望快速检查是不是爬虫逻辑还是对的。

`Contracts`合约的各个合约项放置在紧随`docstring`(函数comment)之后，并且每行以“`@`”开头。

下面是一个例子：

```python
def parse(self, response):
    """ This function parses a property page.
    @url http://web:9312/properties/property_000000.html
    @returns items 1
    @scrapes title price description address image_urls
    @scrapes url project spider server date
    """
    ...
```

这个例子中的`Contracts`合约是表达： “check this URL and you should find one item with values on
those fields I enlist”. Now if you run scrapy check（`$scrapy check basic`）, it will go and check whether the contracts are satisfied:



## scrapy shellTODO

这个应该是最常用的scrapy命令，这是一个完全的python终端环境，可以使用任何在正常python REPL中使用的东西。建议确保系统安装了ipython（`conda install ipython`  或 `pip3 install ipython`），如果安装了，它使用的python REPL就是ipython，否则就是python默认终端。

`scrapy shell`支持的参数如下：

```shell
(spider) c:\tuxueqing\spider\my>scrapy shell -h
Usage
=====
  scrapy shell [url|file]

Interactive console for scraping the given url or file. Use ./file.html syntax
or full path for local file.

Options
=======
--help, -h              show this help message and exit
-c CODE                 evaluate the code in the shell, print the result and
                        exit
--spider=SPIDER         use this spider
--no-redirect           do not handle HTTP 3xx status codes and print response
                        as-is

Global Options
--------------
--logfile=FILE          log file. if omitted stderr will be used
--loglevel=LEVEL, -L LEVEL
                        log level (default: DEBUG)
--nolog                 disable logging completely
--profile=FILE          write python cProfile stats to FILE
--pidfile=FILE          write process ID to FILE
--set=NAME=VALUE, -s NAME=VALUE
                        set/override setting (may be repeated)
--pdb                   enable pdb on failure
```

### 常用arguments

其中最重要的两个argument是:

- -s USER_AGENT="Mozilla/5.0" : 设置USER_AGENT
- -pdb    ：enable interactive debugging in case of exceptions.

### 常见问题

#### scrapyshell莫名debug信息
```sh
(myenv) C:\Users\atmel>scrapy shell
2020-05-25 18:59:21 [scrapy.utils.log] INFO: Scrapy 1.6.0 started (bot: scrapybot)
...
[s] Available Scrapy objects:
[s]   scrapy     scrapy module (contains scrapy.Request, scrapy.Selector, etc)
[s]   crawler    <scrapy.crawler.Crawler object at 0x0000000004C31940>
[s]   item       {}
[s]   settings   <scrapy.settings.Settings object at 0x0000000004C31850>
[s] Useful shortcuts:
[s]   fetch(url[, redirect=True]) Fetch URL and update local objects (by default, redirects are followed)
[s]   fetch(req)                  Fetch a scrapy.Request and update local objects
[s]   shelp()           Shell help (print this help)
[s]   view(response)    View response in a browser
>>>
>>> fetch("http://www.baidu.com")
2020-05-25 19:05:24 [scrapy.core.engine] DEBUG: Crawled (200) <GET http://www.baidu.com> (referer: None)
>>> response.css("title")
[<Selector xpath='descendant-or-self::title' data='<title>百度一下，你就知道</title>'>]
>>>
```

这里有个很奇怪的问题，在`scrapy shell`中，通过TAB来自动完成是，出现莫名的debug信息。从网上信息看这个问题没有解决，可以先通过下面的方式规避掉:

方式1： 不限制安装python、ipython版本

```python
In [3]: import logging
In [4]: logging.getLogger('parso').setLevel(logging.WARNING)
```

方式2： 安装`pip install ipython==7.0.1`，这个版本的ipython没有这个问题，但对应的python要回退到3.7

如果采用了前面环境准备中的版本，那这个问题不会出现。如果不是，建议采用方式1，虽然有些麻烦，但至少不会影响env


### 配置选择python终端

在`scrapy shell`中使用什么python终端，系统也提供了自己配置的能力，例如在你的爬虫项目的`scrapy.cfg`中增加下面的语句，将导致在该爬虫项目执行`scrapy shell`使用的是bpython终端:

```conf
[settings]
shell = bpython
```

另外，还可以通过SCRAPY_PYTHON_SHELL环境来设置。

### spider调用scrapy shell

有时，希望在spider中检查到达的response，这可以通过`scrapy.shell.inspect_response`功能来实现

```python
class MySpider(scrapy.Spider):
...
    def parse(self, response):
        # We want to inspect one specific response.
        if ".org" in response.url:
            from scrapy.shell import inspect_response
            inspect_response(response, self)
```
上面的例子将导致控制转移到`scrapy shell`,这时就可以和通常一样在`scrapy shell`中检查response.最后，您按Ctrl-D（或Windows中的Ctrl-Z）退出外壳程序并继续爬网.


# scrapy选择器

从网页中提取数据常用的库包括： BeautifulSoup，lxml。 scrapy创建了自己的提取器Scrapy Selectors，Scrapy Selectors它基于[Parsel](https://parsel.readthedocs.io/en/latest/)。下面对scrapy提取器进行介绍。

[scrapy选择器官网](https://docs.scrapy.org/en/latest/topics/selectors.html#topics-selectors-ref)

scrapy选择器支持: css选择器（Selector.css）、xpath选择器（Selector.xpath）、正则（Selector.re）


下面的例子是直接使用Scrapy Selectors的例子：

```shell
In [11]: from scrapy.selector import Selector
In [12]: body = '<html><body><span>good</span></body></html>'
In [13]: Selector(text=body).xpath('//span/text()').get()
Out[13]: 'good'
```

### css选择器

[scrapy selector官网](https://docs.scrapy.org/en/latest/topics/selectors.html)
[scrapy提取数据之：CSS选择器 高级用法](https://blog.csdn.net/weixin_42547344/article/details/89306084)

在chrome devtools的consloe中可以直接执行css查询，例如“`$(".wrap .leftDiv > .demo")`”

`response.css(query)` 是`response.selector.css(query)`的简写形式。其中参数`query` 是包含要应用的CSS选择器的字符串


#### 提取属性

标准CSS选择器不支持选择文本节点或属性值。但Scrapy（parsel）实现了两个非标准的伪元素：

- 提取属性：“`选择器::attr(属性名)`”，
- 提取标签内容：“`选择器::text`”

#### 排除

有时提取的逻辑是需要在一段内容中去掉特定的内容，css选择器本身没有这方面的语法支撑，这时候需要变通做下，下面是一个例子

  ```python
  # 获取正文,
  content = s.css("article .sPACont").get()
  rubbish = s.css(
      "article .sPACont   .sHFixedA, .sPMc, .sForPER , a[onclick]").getall()
  # 排除
  for rb in rubbish:
      content = content.replace(rb, "")
  ```

#### 转换地址

在爬取数据后经常有需要将相对地址转换为绝对地址类似的替换要求，下面是一种文本替换实现方式：

  ```python
  # 将正文中的图片链接转成绝对地址
  images = Selector(text=content).css(" .sPACont img").getall()
  # 将正文中的引用链接转成绝对地址
  anchors = Selector(text=content).css(" .sPACont a").getall()
  conv = []
  # 构造替换映射-img
  for i in images:
      t = Selector(text=i)
      old = t.css("img::attr(src)").get()
      new = urljoin(response.url, old)
      i_new = i.replace(old, new)
      conv.append((i, i_new))
  # 构造替换映射-anchor
  for i in anchors:
      t = Selector(text=i)
      old = t.css("a::attr(href)").get()
      new = urljoin(response.url, old)
      i_new = i.replace(old, new)
      conv.append((i, i_new))
  # 替换
  for o, n in dict(conv).items():
      content = content.replace(o, n)
  ```

### 正则选择器

`response.re（regex，replace_entities = True）`是`response.selector.re（regex，replace_entities = True）`的简写形式。其中`regex`可以是已编译的正则表达式，也可以是将使用编译为正则表达式的字符串`re.compile(regex)`。

`re_first（regex，默认= None，replace_entities = True）`.应用给定的正则表达式并返回匹配的第一个unicode字符串。如果不匹配，则返回默认值（`None`如果未提供参数）。

默认情况下，字符实体引用由其对应的字符替换（`&amp;`和除外`&lt;`）。通过`replace_entities`作为False关闭这些替代开关。

### xpath选择器

在chrome的devtools console中直接可以执行xpath查询，例如“`$x('//h1')`”

XPath 使用路径表达式在 XML 文档中进行导航


# spider类与子类

[scrapy spiders官网](https://docs.scrapy.org/en/latest/topics/spiders.html)

`scrapy.spiders.Spider`是最基础的爬虫。 是所有其他爬虫的父类

## Spider

是`scrapy.spiders.Spider`，通过`scrapy genspider -t basic`创建的爬虫就是它。

## Generic Spiders

继承`Spider`而来的通用爬虫类，它典型的爬取是：

```python
def parse(self, response):
    # Get the next index URLs and yield Requests
    next_selector = response.xpath('//*[contains(@class,"next")]//@href')
    for url in next_selector.extract():
      yield Request(urlparse.urljoin(response.url, url))
```

### CrawlSpider

`scrapy.spiders.CrawlSpider`继承自`scrapy.spiders.Spider`， 它实现的`parse() method`使用`rules variable`可以方便的进行link跟踪。 使用`Rule`，scapy会自动发出`Request`，不需要我们手动发。

例如，假设在starturl页面中就可以找到翻页链接:

```python
class Example2Spider(CrawlSpider):
    ...
    #定义”提取动作“
    rules = (
      #获取翻页链接  callback为None,因此follow为True
      Rule(LinkExtractor(restrict_xpaths='...')),
      #提取响应页面
      Rule(LinkExtractor(restrict_xpaths='...'),callback='parse_item')
    )
    def parse_item(self, response):
        pass 
```

除非设置了callback，否则规则将跟随提取的url，这意味着它将扫描目标页面以查找额外的链接并跟随它们。如果设置了回调，则规则不会跟随目标页中的链接。如果您希望它跟随链接，您应该从回调方法返回/放弃`Request(follow-link)`，或者将Rule（）的follow参数设置为true。
unless callback is set, a Rule will follow the extracted URLs, which means that it will scan target pages for extra links and follow them. If a callback is set, the Rule won’t follow the links from target pages. If you would like it to follow links, you should either return/yield them from your callback method, or set the follow argument of Rule() to true.

如果在starturl中没有翻页链接的，那应该在`pasrse_item`中，增加类似下面的代码

```python
def parse_item(self,response):
    ...
    yield item
    #翻页代码
    next_page_url=...
    yield scrapy.Request(next_page_url,callback=self.parse_item)
```



#### Rule规则解析器

根据链接提取器中提取到的链接，根据指定规则提取解析器链接网页中的内容。

```python
Rule(LinkExtractor(...),  #指定链接提取器
     callback='parse_item', #指定规则解析器解析数据的规则（回调函数）
     follow=True, #是否将链接提取器继续作用到链接提取器提取出的链接网页中。
                  #当callback为None,follow的默认值为true
                  #如果是False只获取起始url页面中连接
     cb_kwargs= ,
     process_link= ,
     process_request = ,
     errback = ...)             
```


#### LinkExtractors链接提取器

```python
LinkExtractor(
　　allow=r'Items/'，# 满足括号中“正则表达式”的值会被提取，如果为空，则全部匹配。
　　deny=xxx,  # 满足正则表达式的则不会被提取。
　　restrict_xpaths=xxx, # 满足xpath表达式的值会被提取
　　restrict_css=xxx, # 满足css表达式的值会被提取
　　deny_domains=xxx, # 不会被提取的链接的domains。　
)
```




### XMLFeedSpider

`scrapy.spiders.XMLFeedSpider`,

### CSVFeedSpider

`scrapy.spiders.CSVFeedSpider`, is very similar to the XMLFeedSpider, except that it iterates over rows, instead of nodes. The method that gets called in each iteration is parse_row().

#### 使用basic解析csv输入

```python
class FromcsvSpider(scrapy.Spider):
  name = "fromcsv"
  def start_requests(self):
    with open("todo.csv", "rU") as f:
      reader = csv.DictReader(f)
      for line in reader:
        request = Request(line.pop('url'))
        request.meta['fields'] = line
        yield request

  def parse(self, response):
    item = Item()
    l = ItemLoader(item=item, response=response)
    for name, xpath in response.meta['fields'].iteritems():
        if xpath:
          item.fields[name] = Field()
          l.add_xpath(name, xpath)
    return l.load_item()
```

#### 使用CSVFeedSpider

直接使用模板"`scrapy genspider -t csvfeed`“生成的spider,注意下面例子的不同，第二个是以本地文件作为输入

```python
# start_urls = ['http://com.example/feed.csv']
# start_urls = ['file:///home/user/feed.csv']
```

下面是一个使用本地文件的例子，[how-to-use-scrapy-csvfeedspider-to-crawl-a-feed-that-has-commas-in-its-values](https://stackoverflow.com/questions/23522095/how-to-use-scrapy-csvfeedspider-to-crawl-a-feed-that-has-commas-in-its-values)
```python
# -*- coding: utf-8 -*-
from scrapy.spider import Spider
from scrapy.selector import Selector
from stackoverflow23429315.items import DemoItem
from scrapy.contrib.spiders import CSVFeedSpider
from scrapy import log

# Testing csv file:
# 1,"John, Doe","1234 Main Street, APT A","2nd Floor",John.Doe@test.com
# 2,"John2, Doe","1234 Main Street, APT A","2nd Floor",John.Doe@test.com
# 3,'John3, Doe','1234 Main Street, APT A','2nd Floor',John.Doe@test.com
# 4,'John4, Doe','1234 Main Street, APT A','2nd Floor',John.Doe@test.com

class DmozSpider(CSVFeedSpider):
    name = 'csvFeedTest'        
    start_urls = ['file:////home/vagrant/labs/stackoverflow23429315/test.csv']
    delimiter = ','
    headers = ['id', 'name', 'address1', 'address2', 'email']

    def parse_row(self, response, row):
        log.msg('Hi, this is a row!: %r' % row)

        item = DemoItem()
        item['id'] = row['id']
        item['name'] = row['name']
        item['address1'] = row['address1']
        item['address2'] = row['address2']
        item['email'] = row['email']
        return item

from scrapy.item import Item, Field

class DemoItem(Item):
    id = Field()
    name = Field()
    address1 = Field()
    address2 = Field()
    email = Field()
```



### SitemapSpider

`scrapy.spiders.SitemapSpider`,  allows you to crawl a site by discovering the URLs using Sitemaps

## 在response间传递数据

通过`meta` 方式,下面演示了在请求中携带，以及在目的地提取出来

```python
yield Request(url, meta={"title": title},callback=self.parse_item)
```

```python
def parse_item(self, response):
   l.add_value('title', response.meta['title'],
         MapCompose(unicode.strip, unicode.title))
```

## 启动爬虫

### 命令行传递starturl
```python
class MySpider(BaseSpider):
    name = 'my_spider'    
    def __init__(self, *args, **kwargs):
        """
        支持命令行传递starturl
        e.g： scrapy crawl my_spider -a start_urls="http://example1.com,http://example2.com"
        """
        super(MySpider,self).__init__(*args, **kwargs)
        self.start_urls = kwargs.get('start_urls').split(',')

#And start it like: scrapy crawl my_spider -a start_urls="http://some_url"
```

### crawl或runspider

```python
# 通过scrapy project，启动爬虫，<spider>可通过scrapy list查找可用的名字
~$scrapy crawl [options] <spider>

# 通过spider文件启动爬虫，不依赖scrapy project
$scrapy runspider [options] <spider_file>
```

### 单独启动文件

或单独写个爬虫启动python文件

```python
# 单独的启动文件，通过scrapy命令行启动爬虫
scrapy.cmdline.execute("scrapy crawl myspider".split())
```

# items

[scrapy items 官网](https://docs.scrapy.org/en/latest/topics/items.html)

### 简单使用

Scrapy提供了Item类。Item对象是种简单的容器，保存了爬取得到的数据。比如下面就是一个最简单的例子，Spider分析爬取到数据，组装结构化数据`MyItem`

```python
class MySpider(scrapy.Spider):
...
    def parse(self, response):
        for h3 in response.xpath('//h3').getall():
            yield MyItem(title=h3)
```

比如，下面定义PropertiesItem的方式，是一个较为标准的Item形式

```python
from scrapy.item import Item, Field
class PropertiesItem(Item):
    # Primary fields
    title = Field()
    price = Field()
    description = Field()
    address = Field()
    image_urls = Field()
    # Calculated fields
    images = Field()
    location = Field()
    # Housekeeping fields
    url = Field()
    project = Field()
    spider = Field()
    server = Field()
    date = Field()
```

### 构造复杂结构

首先要记住Item 对象是种简单的容器。其提供了 类似于词典(dictionary-like) 的API。这意味着你们存放什么它是不关心的。比如对`url = Field()`字段，里面是存放`list`，还是`dict`，还是`str`,完全有业务自己决定。

举例来说，我们定义了如下的Item：

```python
class CcherePageItem(scrapy.Item):
    #
    url = scrapy.Field()
    articles = scrapy.Field()


class CchereArticleItem(scrapy.Item):
    # define the fields for your item here like:
    # name = scrapy.Field()
    url = scrapy.Field()
    # 是回复哪篇文章
    anstourl = scrapy.Field()
    title = scrapy.Field()
    # 文章正文
    c = scrapy.Field()
    # 在cchere全看方式下"/thread/xx"具备的层级信息
    lvl = scrapy.Field()
    auth = scrapy.Field()
    date = scrapy.Field()
```

在使用的时候是下面方式：

```python
output_p = CcherePageItem()
output_p['url'] = response.url

# 存放导出的文章
output_articles = []
for target_list in expression_list:
    article = CchereArticleItem(...)
    output_articles.append(article)

output_p['articles'] = output_articles
yield output_p
```

这最后爬取出来的数据就类似下面：

```json
[
    {
        "url": "https://www.talkcc.net/thread/4519373/7",
        "articles": [
            {
                "lvl": "6",
                "title": "很奇怪的是商代",
                ...
            },
            {
                "lvl": "8",
                "title": "也许古代那是丘陵，现在被黄河都淤平了？",
                ...
            },
            ...
        ]
    },
    ...

```


### Item Loaders

[item loads官网](http://doc.scrapy.org/en/latest/topics/loaders.html)

在上面的例子中`MyItem(title=h3)`，我们是直接采用`Items类`的`dict-like api` 来进行item填充的. 但scrapy同时提供了`Item Loader`方式对item进行填充。


例子：

```python
    def parse(self, response):
    l = ItemLoader(item=PropertiesItem(), response=response)
    l.add_xpath('title', '//*[@itemprop="name"][1]/text()')
    ...
    return l.load_item()
```


# item pipelines

每个`item pipeline`组件（有时也称为“项目管道”）都是一个实现简单方法的Python类。他们接收到一个`item`并对其执行操作,比如后处理，清洗，存储...。还决定该`item`是否应继续通过管道或被删除并不再处理。

每个`item pipeline`组件都必须实现`def process_item(self, item, spider):`。

通常在pipeline实现对item进行清理，将数据最终归档（发送给第三方，写文件，写数据库...）

下面这个例子是一个较为复杂的例子，将item写入MongoDB
```python
import pymongo

class MongoPipeline:

    collection_name = 'scrapy_items'

    def __init__(self, mongo_uri, mongo_db):
        self.mongo_uri = mongo_uri
        self.mongo_db = mongo_db

    #返回pipeline instance
    @classmethod
    def from_crawler(cls, crawler):
        return cls(
            mongo_uri=crawler.settings.get('MONGO_URI'),
            mongo_db=crawler.settings.get('MONGO_DATABASE', 'items')
        )
    #open spider后将调用该方法
    def open_spider(self, spider):
        self.client = pymongo.MongoClient(self.mongo_uri)
        self.db = self.client[self.mongo_db]
    #close spider后将调用该方法
    def close_spider(self, spider):
        self.client.close()
    #必须执行以下操作之一：返回带有数据的字典，返回一个Item （或任何后代类）对象，
    #返回 Deferred或引发 DropItem异常。删除的项目不再由其他管道组件处理。
    
    def process_item(self, item, spider):
        self.db[self.collection_name].insert_one(dict(item))
        return item
        #raise DropItem("Missing price in %s" % item)
```        

# Feed exports


# Spiders Contracts

在官网的术语中有contract与contracts，对它们的翻译，先定为： contract合约项， contracts合约。

其中contracts合约是指关联了 contract合约项的callback函数。 例如:

```shell
$ scrapy check -l
first_spider
  * parse
  * parse_item
second_spider
  * parse
  * parse_item
```


[Spiders Contracts官网](https://docs.scrapy.org/en/latest/topics/contracts.html?highlight=Contracts)

## 内置合约项

#### @url

- `@url` : 设置检查合约时的sample url，如果没有它，则在`scrapy check ...`时忽略该该合约。语法是：`@url url`。对应类为`classscrapy.contracts.default.UrlContract`

#### @returns

- `@returns`: 设置检查合约时spdier应返回的`items and requests`个数下限与上限, 语法是`@returns item(s)|request(s) [min [max]]`. 对应类为`classscrapy.contracts.default.ReturnsContract[source]`

#### @cb_kwargs

- `@cb_kwargs` : 设置检查合约时`sample request`的`cb_kwargs attribute `. 例如`"@cb_kwargs {"arg1": "value1", "arg2": "value2", ...}"` .对应类为 `classscrapy.contracts.default.CallbackKeywordArgumentsContract[source]`

#### @scrapes
- `@scrapes` : 检查检查合约时callback返回的`items`应满足的成员字段列表，例如`@scrapes field_1 field_2 ...` . 对应类为`classscrapy.contracts.default.ScrapesContract[source]`

## 定制合约项

### 创建

通过继承`scrapy.contracts.Contract`可以创建自定义合约

```python
from scrapy.contracts import Contract
from scrapy.exceptions import ContractFail

class HasHeaderContract(Contract):
    """ Demo contract which checks the presence of a custom header
        @has_header X-CustomHeader
    """

    name = 'has_header'

    def pre_process(self, response):
        for header in self.args:
            if header not in response.headers:
                #通过返回ContractFail，指示不满足合约
                raise ContractFail('X-CustomHeader not present')
```

### 加载

通过配置文件中的`SPIDER_CONTRACTS` 加载， 例如

```python
SPIDER_CONTRACTS = {
    'myproject.contracts.ResponseCheck': 10,
    'myproject.contracts.ItemValidate': 10,
}
```


# 工具类

### urlparse.urljoin

作用：相对url转换为绝对url

```python
>>>  from urllib.parse import urljoin

>>> help(urljoin)
Help on function urljoin in module urlparse:
 
urljoin(base, url, allow_fragments=True)
    Join a base URL and a possibly relative URL to form an absolute
    interpretation of the latter.

## 例子
In [2]: from urllib.parse import urljoin

In [3]: urljoin("http://domain/folder/currentpage.html", "anotherpage.html")
Out[3]: 'http://domain/folder/anotherpage.html'

In [4]: urljoin("http://domain/folder/xyz/currentpage.html", "/anotherpage.html")
Out[4]: 'http://domain/anotherpage.html'

In [5]: urljoin("http://domain/folder/currentpage.html", "folder2/anotherpage.html")
Out[5]: 'http://domain/folder/folder2/anotherpage.html'

In [6]: urljoin("http://domain/folder/currentpage.html", "/folder2/anotherpage.html")
Out[6]: 'http://domain/folder2/anotherpage.html'

In [7]: urljoin("http://domain/abc/folder/currentpage.html", "/folder2/anotherpage.html")
Out[7]: 'http://domain/folder2/anotherpage.html'

In [8]: urljoin("http://domain/abc/folder/currentpage.html", "../anotherpage.html")
Out[8]: 'http://domain/abc/anotherpage.html'

In [9]:  
```
### 编解码解码JS中的encodeURIComponent与decodeURIComponent

引用urllib包中parse模块的quote和unquote

JS：

```js
encodeURIComponent(‘我爱编程’)
```

Python：

```python
from urllib import parse
jsStr=’%E6%88%91%E7%88%B1%E7%BC%96%E7%A8%8B’
print(parse.unquote(jsStr)) 
#—> 输出：我爱编程
print(jsStr==parse.quote(‘我爱编程’)) 
#—>输出：True
```

### scrapy.http.FormRequest

该类典型的使用场景是发送登录的user、passwd。 例如：

```python
# Start on the welcome page
def start_requests(self):
    return [
        Request(
        "http://web:9312/dynamic/nonce",
        callback=self.parse_welcome)
    ]
# Post welcome page's first form with the given user/pass
def parse_welcome(self, response):
    return FormRequest.from_response(
        response,
        formdata={"user": "user", "pass": "pass"}
        )
```

### 编码问题

如果存在编码问题，可以尝试：

```python
unicode(response.body.decode(response.encoding)).encode('utf-8')
newresponse = response.replace(encoding='utf-8')
```

# 反反爬

#### 动态变化userAgent

##### 增加配置

在项目的”settings.py“中增加userAgent处理的配置
```python
USER_AGENT_LIST = [...]
# Enable or disable downloader middlewares
# See https://docs.scrapy.org/en/latest/topics/downloader-middleware.html
DOWNLOADER_MIDDLEWARES = {
   'my.middlewares.RandomUserAgentMiddleware': 543,
#    'my.middlewares.MyDownloaderMiddleware': 543,
}
```

##### 增加RandomUserAgentMiddleware

在项目的“middlewares.py”中增加`RandomUserAgentMiddleware`

```python
# 随机选择 User-Agent 的下载器中间件
class RandomUserAgentMiddleware(object):
    def process_request(self, request, spider):
        # 从 settings 的 USER_AGENTS 列表中随机选择一个作为 User-Agent
        user_agent = random.choice(spider.settings['USER_AGENT_LIST'])
        request.headers['User-Agent'] = user_agent
        return None
 
    def process_response(self, request, response, spider):
        # 验证 User-Agent 设置是否生效
        logger.info("headers ::> User-Agent = " + str(request.headers['User-Agent'], encoding="utf8"))
        return response
```

##### 增加下载间隔

```python
# Configure a delay for requests for the same website (default: 0)
# See https://docs.scrapy.org/en/latest/topics/settings.html#download-delay
# See also autothrottle settings and docs
DOWNLOAD_DELAY = 3
```

#### 动态IPtodo

[Python爬虫代理IP池(proxy pool)](https://github.com/jhao104/proxy_pool)

#### 一行js代码识别Selenium+Webdriver及其应对方案

https://www.cnblogs.com/xieqiankun/p/hide-webdriver.html

# scrapy配置参数

系统默认配置项列表，可以在"scrapy\settings\default_settings.py"中查看到，也可以查看项目中的"scrapy.cfg"，但一般这里面比较少。

重要的配置参数如下：

![重要的配置参数](#images/EssentialSettings.png)


#### CLOSESPIDER_TIMEOUT

使用这个配置来配合定时的去crontab中执行了定时任务，避免定时拉起时，原来的任务还没有结束

#### CLOSESPIDER_ERRORCOUNT

#### CLOSESPIDER_ITEMCOUNT

#### CLOSESPIDER_PAGECOUNT


### logging for analysis

### closing


#### LOG_LEVEL
  日志层级包括：DEBUG (lowest level), INFO, WARNING, ERROR, and CRITICAL (highest level). 以及SILENT （不打印日志）
#### LOGSTATS_INTERVAL
  打印日志的频度，默认60s
#### LOG_ENABLED
#### LOG_FILE
  默认是stdout  
#### LOG_STDOUT
  如果为true，意味着所有输出到stdout的都打印到log中

### stats for analysis

#### STATS_DUMP
  默认是开启的
#### DOWNLOADER_STATS
  设置为false，代表对download不吉利stats
#### DEPTH_STATS
  设置是否统计site depth
#### DEPTH_STATS_VERBOSE
  如果需要site depth更详细的信息，设置它
#### STATSMAILER_RECPTS
  当crawl finish时发送stats到哪个邮箱

### telent  for analysis

#### TELENTCONSOLE_ENABLED
#### TELENTCONSOLE_PORT

### feeds

#### FEED_URI
#### FEED_PORMAT
#### FEED_STORE_EMPTY
#### FEED_EXPORT_FIELDS
#### FEED_URI_PARAMS

### crawling style

#### DEPTH_LIMIT
#### DEPTH_PRIORITY
#### SCHEDULER_DISK_QUEUE
#### SCHEDULER_MEMOY_QUEUE
#### ROBOTSTXT_OBKEY
#### COOKIES_ENABLED
#### REFERER_ENABLED
#### USER_AGENT
#### DEFAULT_REQUEST_HEADERS


### performance

#### CONCURRENT_REQUESTS

#### CONCURRENT_REQUESTS_PER_DOMAIN

#### CONCURRENT_REQUESTS_PER_IP

#### CONCURRENT_ITEMS

#### DOWNLOAD_TIMEOUT

#### DOWNLOAD_DELAY

#### RANDOMIZE_DOWNLOAD_DELAY

#### DNSCACHE_ENABLED


### http cache

#### HTTPCACHE_ENABLED

#### HTTPCACHE_DIR

#### HTTPCACHE_POLICY

#### HTTPCACHE_STORAGE

#### HTTPCACHE_DBM_MODULE

#### HTTPCACHE_EXPIRATION_SECS

#### HTTPCACHE_IGNORE_HTTP_CODES
#### HTTPCACHE_IGNORE_SCHEMES
#### HTTPCACHE_GZIP

### media download

#### IMAGES_STORE
#### IMAGES_EXPIRES
#### IMAGES_THUMBS
#### IMAGES_URLS_FIELD
#### IMAGES_RESULT_FIELD
#### IMAGES_MIN_HEIGHT
#### IMAGES_MIN_WIDTH
#### FILES_STORE
#### FILES_EXPIRES
#### FILES_URLS_FIELD
#### FILES_RESULT_FIELD

# 抓取记录

## gpai 抓取记录

### 链接预分析

gpai网是动态网页，使用静态抓取不可行，因此采用selenium+scrap的方式
#### 预分析结果

下面的代码是手工分析执行的结果

```python
# 支持In : webdriver.Chrome  webdriver.Firefox  webdriver.Edge webdriver.FirefoxOptions 。。。
from selenium import webdriver
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
from selenium.webdriver.common.by import By
#创建实例，直到页面oload方法执行完毕才返回
driver = webdriver.Firefox()
driver.get("http://s.gpai.net/sf/search.do?action=court&cityNum=310109&Page=1")
#等待分页导航出现
sel = 'body .page-bar'
try:
    WebDriverWait(driver, 10).until(
        lambda driver: driver.find_element_by_css_selector(sel))
except TimeoutException:
    print('加载失败')
    pass
from scrapy.selector import Selector
body=Selector(text=driver.page_source)
#获取分页链接list
pages = body.css(f'{sel}'+' ' +'a::attr("href")').getall()
#结果类似下面，需要从中分析max page
# ['http://s.gpai.net/sf/search.do?action=court&cityNum=310109&Page=0',
#  'http://s.gpai.net/sf/search.do?action=court&cityNum=310109&Page=1',
#...
#  'http://s.gpai.net/sf/search.do?action=court&cityNum=310109&Page=16',
#  'http://s.gpai.net/sf/search.do?action=court&cityNum=310109&Page=2']

#获取页面每项资源link
sel = '.main-col-list .list-item .item-tit a::attr("href")'
items = body.css(sel).getall()
##结果类似下面
# ['http://www.gpai.net/sf/item2.do?Web_Item_ID=28120',
# ...
#  'http://www.gpai.net/sf/item2.do?Web_Item_ID=28562']

#
driver = webdriver.Firefox()
driver.get('http://www.gpai.net/sf/item2.do?Web_Item_ID=28562')
body=Selector(text=driver.page_source)
sel = '.tab-list-con .panel-con'
# xx = body.css(sel).getall()
##获取竞买须知
sel = '#menuid2 + .d-title + .d-article'
item_notice = body.css(sel).get()
##获取标的物介绍
sel = '#menuid3 + .d-title + .d-block'
item_intro = body.css(sel).get()
##获取标的附件
sel = '.d-block-download a'
sels = body.css(sel)
item_attachs=dict()
for i in sels:
    title = i.css('a::text').get()
    href = i.css('a::attr("href")').get()
    item_attachs[title]=href
##获取标的视频介绍
sel='.d-block > .d-article video'
item_video=dict()
try:
    item_video['src']= body.css(sel+'::attr(src)').get().strip()
    item_video['poster']= body.css(sel+'::attr(poster)').get().strip()
except Exception:
    pass
##获取图片
sel='.ItemImgAll img'
item_images:list=body.css(sel+'::attr(src)').getall()
##获取优先购买人信息
sel='.d-block .d-block-tb table'
item_preferred_buyers=body.css(sel).get()
```



## cchere抓取记录

### article链接预分析

article链接是指url为类似`https://www.talkcc.net/article/4520813`的文章链接

#### 查看抓取到html source
```shell
# 查看html source，通过下面命令能使得与spider看到的一致
$scrapy view https://www.talkcc.net/article/4520813
```

#### 预分析结果

对“article/xxx”通过分析有以下几点认识：

- 文章正文内容在“`var ls=`”定义的变量中，
- 该变量需要通过位于[`js0.8.3.js`](https://www.talkcc.net/incs/js0.8.3.js)中的`zJ_PE`函数解码才可以得到文章html片段
- 该文章html片段中，正文内容在`<article class="sClear">`中
- 
- 文章中出现的“`var rs=`”变量存储内容，是正文右侧得花评论列表，应不是我们关系的内容

对`zJ_PE`函数转换为python实现是：

```python
# function zJ_PE(t) {
# 	var e = t.substring(0, 3);
# 	if ("*" != e.charAt(1) || "@" != e.charAt(2))
# 		return t;
# 	if (t = t.substring(3), 0 < parseInt(e.charAt(0))) {
# 		for (
# 			var n = new Array(/@/g, /#/g, /\$/g, /\?/g, /\^/g, /\[/g, /=/g, /</g, />/g, /\{/g, /\}/g, /\|/g, /:/g, /;/g, /,/g),
# 			i = new Array("%2", "%3", "%4", "%5", "%6", "%7", "%8", "%9", "%A", "%B", "%C", "%D", "%E", "%F", "e"),
# 			a = 0; a < n.length; a++)
# 			t = t.replace(n[a], i[a]);
# 		return decodeURIComponent(t)
# 	} return t
# }

from urllib import parse
#实现js0.8.3.js中的zJ_PE函数python版本
def zJ_PE(t:str):
	e=t[:3]
	if '*' != e[1] or '@' != e[2]:
		return t

	#定义转换字典
	conv = dict(
		[("@",'%2'),('#','%3'),('$','%4'),('?','%5'),('^','%6'),('[','%7'),('=','%8'),('<','%9'),
		('>','%A'),('{','%B'),('}','%C'),('|','%D'),(':','%E'),(';','%F'),(',','e')]
	)
	
	t=t[3:]
    
	if 0< int(e[0]):
		for k,v in conv.items():
			t=t.replace(str(k),str(v),-1)
		return parse.unquote(t)

	return t
```

### thread链接预分析

article链接是指url为类似`https://www.talkcc.net/thread/4519373`的文章链接

#### 查看抓取到html source
```shell
# 查看html source，通过下面命令能使得与spider看到的一致
$scrapy view https://www.talkcc.net/thread/4519373
```

#### 预分析结果

对“thread/xxx”通过分析有以下几点认识：

- 正文在“`var ls=`”定义的变量中
- 该变量需要通过位于[`js0.8.3.js`](https://www.talkcc.net/incs/js0.8.3.js)中的`zJ_PE`函数解码才可以得到正文html片段
- 正文html片段中，最前面的`<h3 class="sTAC sNoWrap">`含有文章所属专题信息

  ```python
  <h3 class="sTAC sNoWrap">
      <small>主题:</small>农业文明的几个问题 -- ziyun2015</h3>

  ```

- 正文html片段中，`<div id="D_GT_L" class="s_Sec">`含有正文articls

  ```html
  <div id="D_GT_L" class="s_Sec">
      ...
      <div class="s_SecC" style="margin-left:6em;">
          <article class="sClear">
              <div class="sPSInfo s_Smaller">4525096 复 <a href="/article/4525025">4525025</a><br><a href="/user/桥上"
                      c-u=24173>桥上</a></div>
              <div id="PD4525096" class="sPACont" c-mnu="4525096,1,0,0,0,0" c-pzt="4519373"><small>7</small><img
                      src="/bbsIMG/bf/face1.gif"> <b>不宜用韩非子和说文来解，</b> <i class="sUpV">1</i> <img src="/bbsIMG/np.gif"
                      alt="新"><br>
                  <p>因为那个解释是在之前。</p> <br>
                  <ul>
                      <li>本帖 <a href="javascript:void(0)" onclick="zJ_DirectGT(this,4525096)">跟 1，🌺 4</a></li>
                  </ul>
                  <div class="sHFixedA"></div>
                  <div class="sPMc"><small>2020-06-04 00:39:37</small></div>
                  <div class="sForPER"></div>
              </div>
          </article>
      </div>
      ...
  </div>
  ```

- 在分页情况下， 首页等于该starturl,翻页信息位于下面的片段

  ```html
  <h4 class="s_PGNav s_FW">
      <div class="s_FW_L">全看 <a href="/topic/4519373">分页</a> <a href="/alist/4519373">树展</a> <a
              href="/ainfo/4519373">一览</a> <a href="/article/4519373">主题</a></div>
      <div class="s_FW_R">
          <form method="post" onsubmit="location.assign('/thread/4519373/'+this.p.value);return false"><input name="p"
                  size="2" type="text" value="1" class="sCurPNum"> / 6 <a href="/thread/4519373">首页</a> 上页 <a
                  href="/thread/4519373/2">下页</a> <a href="/thread/4519373/6">末页</a></form>
      </div>
  </h4>
  ```

## 人民法院诉讼资产网抓取记录 selenium

```python
from importlib import import_module
driver_name='chrome'
webdriver_base_path = f'selenium.webdriver.{driver_name}'
driver_klass_module = import_module(f'{webdriver_base_path}.webdriver')
driver_klass = getattr(driver_klass_module, 'WebDriver')
driver_options_module = import_module(f'{webdriver_base_path}.options')
driver_options_klass = getattr(driver_options_module, 'Options')
driver_options = driver_options_klass()
driver_options.add_argument('blink-settings=imagesEnabled=false')
driver_options.add_experimental_option('excludeSwitches', ['enable-automation'])

from shutil import which
driver_kwargs = {
          'executable_path': which('chromedriver'),
           f'{driver_name}_options': driver_options,
          
}
driver = driver_klass(**driver_kwargs)


from scrapy.http.response.html import HtmlResponse
body = str.encode(driver.page_source)
response = HtmlResponse(
            driver.current_url,
            body=body,
            encoding='utf-8'
        )
import html2text
h = html2text.HTML2Text()

driver.get('https://www1.rmfysszc.gov.cn/Handle/110654.shtml')

sel = '#Title *::text'
item_title = response.css(sel).get()
# 获取标的视频介绍
item_video = None
# 竞买须知
sel = '#jmxz'
item_atten = response.css(sel).get()
item_atten = h.handle(item_atten)
# 竞买公告
sel = '#pmgg'
item_notice = response.css(sel).get()
item_notice = h.handle(item_notice)
# 获取标的物介绍
sel = '#bdjs11 table'
item_intro = response.css(sel).get()
item_intro = self.h.handle(item_intro)
# 获取状态
sel = '#time1 *::text'
item_state = response.css(sel).getall()
item_state = ' '.join(item_state).strip()
# 获取标的附件
sel = '#pmgg a'
sels = response.css(sel)
item_attachs = dict()
for i in sels:
    title = i.css('a::text').get()
    href = i.css('a::attr("href")').get()
    item_attachs[title] = href    
sel = '#bdjs11 table a'
sels = response.css(sel)
for i in sels:
    title = i.css('a::text').get()
    href = i.css('a::attr("href")').get()
    item_attachs[title] = href
    
    
# 获取图片
sel = '#bdjs11 img'
item_images: list = response.css(sel+'::attr(src)').getall()
# 获取优先购买人信息
sel = '#yxgmq .yxq'
item_preferred_buyers = response.css(sel).get()
item_preferred_buyers = self.h.handle(item_preferred_buyers)

# 获取竞买记录
sel = '#jjjl1'
item_rec = response.css(sel).get()
item_rec = self.h.handle(item_rec)
```

# 下载器中间件

[系统默认中间件，以及优先级数字](https://docs.scrapy.org/en/latest/topics/settings.html#std-setting-DOWNLOADER_MIDDLEWARES_BASE)，除非显示禁用，否则系统默认中间件会起作用的。

```python
DOWNLOADER_MIDDLEWARES = {
    'myproject.middlewares.CustomDownloaderMiddleware': 543,
    'scrapy.downloadermiddlewares.useragent.UserAgentMiddleware': None,
}
```
这上面例子中：

- 数字的含义：对中间件`process_request() `方法，将以递增的中间件顺序（100、200、300，...）被执行。
`process_response()`方法相反

- `None`代表禁用的意思

下载中间件process_request的几种返回：

- 如果返回`None`，则Scrapy将继续处理此请求，执行所有其他中间件。

- 如果它返回一个Response对象，Scrapy将忽略任何其他process_request()或process_exception()方法，或相应的下载功能; 它将返回该响应。process_response() 总是在每个响应上调用已安装中间件的方法。

- 如果返回一个Request对象，Scrapy将停止调用process_request方法并重新计划返回的请求。一旦执行了新返回的请求，就会在下载的响应上调用适当的中间件链。

- 如果引发IgnoreRequest异常，则将process_exception()调用已安装的下载器中间件的 方法。如果它们都不处理该异常，Request.errback则调用请求（）的errback函数。如果没有代码处理引发的异常，则将其忽略并且不记录（与其他异常不同）。



# selenium

前提： `pip install selenium`

[selenium python中文文档](https://selenium-python-zh.readthedocs.io/en/latest/index.html)

### 快速入手

```python
# 支持In : webdriver.Chrome  webdriver.Firefox  webdriver.Edge webdriver.FirefoxOptions 。。。
from selenium import webdriver
from selenium.common.exceptions import TimeoutException
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
from selenium.webdriver.common.by import By

#键盘支持
from selenium.webdriver.common.keys import Keys

#创建实例，直到页面oload方法执行完毕才返回
driver = webdriver.Firefox()
elem = driver.find_element_by_name("q")

#特殊的按键,比如回车.可以使用Keys类来输入，keys类继承自 selenium.webdriver.common.keys
elem.clear()  # 为了安全起见，我们先清除input输入框中的任何预填充的文本
elem.send_keys("pycon") #键盘输入
elem.send_keys(Keys.RETURN) #键盘输入回车,
driver.close()  #关闭标签页，如果要关闭整个浏览器，使用driver.quit()
```

下面是一个更复杂些，也更灵活些的代码，切换drver_name为`chrome`，切换为`firefox`...

```python
from importlib import import_module
driver_name='chrome'
webdriver_base_path = f'selenium.webdriver.{driver_name}'
driver_klass_module = import_module(f'{webdriver_base_path}.webdriver')
driver_klass = getattr(driver_klass_module, 'WebDriver')
driver_options_module = import_module(f'{webdriver_base_path}.options')
driver_options_klass = getattr(driver_options_module, 'Options')
driver_options = driver_options_klass()
#仅chrome支持
driver_options.add_argument('blink-settings=imagesEnabled=false')
driver_options.add_experimental_option('excludeSwitches', ['enable-automation'])

from shutil import which
driver_kwargs = {
          'executable_path': which('chromedriver'),
           f'{driver_name}_options': driver_options,
          
}
driver = driver_klass(**driver_kwargs)


from scrapy.http.response.html import HtmlResponse
body = str.encode(driver.page_source)
response = HtmlResponse(
            driver.current_url,
            body=body,
            encoding='utf-8'
        )
import html2text
h = html2text.HTML2Text()

driver.get('https://www1.rmfysszc.gov.cn/Handle/110654.shtml')

sel = '#Title *::text'
item_title = response.css(sel).get()
# 获取标的视频介绍
item_video = None
```

### 驱动下载

#### geckodriver
[WebDriver for Firefox 文档](https://firefox-source-docs.mozilla.org/testing/geckodriver/), [WebDriver for Firefox 下载路径](https://github.com/mozilla/geckodriver),需要将`geckodriver`放置在PATH可执行路径中

firefox 命令行参数完整列表，见[Command_Line_Options官网](https://developer.mozilla.org/en-US/docs/Mozilla/Command_Line_Options)

#### chromedriver
[WebDriver for Chrome 下载路径 ](https://sites.google.com/a/chromium.org/chromedriver/downloads)  下载的驱动需要与chrome的版本对应，可以通过`chrome://version/`查找chrome安装路径与chrome版本.需要将`chromedriver.exe`放置在PATH可执行路径中

参数说明：https://peter.sh/experiments/chromium-command-line-switches/

[selenium+python配置chrome浏览器的选项](https://www.jianshu.com/p/e49463ecd68b)

[chrome-flags-for-tools.md#--enable-automation](https://github.com/GoogleChrome/chrome-launcher/blob/master/docs/chrome-flags-for-tools.md#--enable-automation)

##### 几个重要的argument

启动参数 作用
-  –user-agent="" 设置请求头的User-Agent
-  –window-size=1366,768 设置浏览器分辨率（窗口大小）
-  –headless 无界面运行（无窗口）
-  –start-maximized 最大化运行（全屏窗口）
-  –incognito 隐身模式（无痕模式）

例如 `options.AddArgument("–incognito");//启动无痕/隐私模式`

#### msedgedriver

在edge浏览器通过`edge://version/`查看edge版本以及命令行，注意下载的驱动需要和edge版本保持一致。

[msedgedriver.exe下载路径 ](https://developer.microsoft.com/en-us/microsoft-edge/tools/webdriver/)
Use WebDriver (Chromium) for test automation
#### safari

Safari : https://webkit.org/blog/6900/webdriver-support-in-safari-10/



## 

### 打开页面

调用`driver.get("http://www.google.com")` WebDriver 将等待，直到页面完全加载完毕（其实是等到 onload 方法执行完毕）， 然后返回继续执行你的脚本。 值得注意的是，如果你的页面使用了大量的Ajax加载， WebDriver可能不知道什么时候页面已经完全加载。 如果你想确保也main完全加载完毕，可以使用:`ref:waits <waits>`

使用选择器`driver.find_element_by_xxx`，，如果没有找到将会抛出 ``NoSuchElementException``异常。

### 填写表格

例如对：

```html
<select>
  <option value ="volvo">Volvo</option>
  <option value ="saab">Saab</option>
  <option value="opel">Opel</option>
  <option value="audi">Audi</option>
</select>
```
使用selenium Select类
```python
from selenium.webdriver.support.ui import Select
select = Select(driver.find_element_by_name('name'))
select.select_by_index(index)
select.select_by_visible_text("text")
select.select_by_value(value)
```

### 等待页面加载完成(Waits)

提供两种类型的waits - 隐式和显式。 显式等待会让WebDriver等待满足一定的条件以后再进一步的执行。 而隐式等待让Webdriver等待一定的时间后再才是查找某元素。

#### 显式等待例子

- 例1
```python
from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC

driver = webdriver.Firefox()
driver.get("http://somedomain/url_that_delays_loading")
try:
    element = WebDriverWait(driver, 10).until(
        EC.presence_of_element_located((By.ID, "myDynamicElement"))
    )
finally:
    driver.quit()
```
- 例2
```python
WebDriverWait(driver, 100).until(
            lambda driver: driver.find_element_by_name(self.locator))
```

#### 隐式等待
如果某些元素不是立即可用的，隐式等待是告诉WebDriver去等待一定的时间后去查找元素。 默认等待时间是0秒，一旦设置该值，隐式等待是设置该WebDriver的实例的生命周期。

```python
from selenium import webdriver

driver = webdriver.Firefox()
driver.implicitly_wait(10) # seconds
driver.get("http://somedomain/url_that_delays_loading")
myDynamicElement = driver.find_element_by_id("myDynamicElement")
```

#### 多个元素的等待

https://stackoverflow.com/questions/22710154/python-selenium-implicit-wait-for-multiple-elements
```python
WebDriverWait(browser, 10).until(
    EC.presence_of_all_elements_located((By.CSS_SELECTOR, '#page a'))
)
```

### API

#### Action Chains
```python
from selenium import webdriver
webdriver.ActionChains
menu = driver.find_element_by_css_selector(".nav")
hidden_submenu = driver.find_element_by_css_selector(".nav #submenu1")

ActionChains(driver).move_to_element(menu).click(hidden_submenu).perform()
```
#### 选择器

通过`By`：
```python
from selenium.webdriver.common.by import By

driver.find_element(By.XPATH, '//button[text()="Some text"]')
driver.find_elements(By.XPATH, '//button')
```

通过driver.`find_elements_by_xxx`



### 常见问题

更多见[Frequently Asked Questions](https://github.com/SeleniumHQ/selenium/wiki/Frequently-Asked-Questions)

#### Scrapy - Reactor not Restartable [duplicate]

https://stackoverflow.com/questions/41495052/scrapy-reactor-not-restartable

#### 403问题TODO

https://stackoverflow.com/questions/33225947/can-a-website-detect-when-you-are-using-selenium-with-chromedriver

#### 查看selienum的user-agent

How to get user agent information in Selenium WebDriver with Python
通过执行js
`user_agent = driver.execute_script("return navigator.userAgent;")`

#### 执行js

```python
driver.execute_script("window.scrollTo(0, document.body.scrollHeight);")
```

如果是java，则类似:

```java
WebDriver driver; // Assigned elsewhere
JavascriptExecutor js = (JavascriptExecutor) driver;
js.executeScript("return document.title");
```

执行js需要返回值，取决于js代码中是否有返回

`js.executeScript("document.title");`will return null, but:

`js.executeScript("return document.title");`will return the title of the document.

#### 如果获取当前窗口的截图？
使用webdriver提供的 save_screenshot 方法:

```python
from selenium import webdriver

driver = webdriver.Firefox()
driver.get('http://www.python.org/')
driver.save_screenshot('screenshot.png')
driver.quit()
```

[Selenium with Python中文翻译文档](https://selenium-python-zh.readthedocs.io/en/latest/index.html)


        time.sleep(4)
        # 通过断言页面是否存在某些关键字来确定页面按照预期加载
        assert u"首页--光荣之路" in self.driver.page_source, u"页面源码中不存在该关键字！"

[selenium webdriver怎样判断网页加载完](https://jingyan.baidu.com/article/2d5afd69bfaaffc4a3e28e54.html)

一个简单的样例

```python
import scrapy
from selenium import webdriver

class ProductSpider(scrapy.Spider):
    name = "product_spider"
    allowed_domains = ['ebay.com']
    start_urls = [...']

    def __init__(self):
        self.driver = webdriver.Firefox()

    def parse(self, response):
        self.driver.get(response.url)

        while True:
            next = self.driver.find_element_by_xpath('//td[@class="pagn-next"]/a')

            try:
                next.click()

                # get the data and write it to scrapy items
            except:
                break

        self.driver.close()
```

## 微信

### 准备微信个人订阅号

登陆和注册链接，[微信公众平台](https://mp.weixin.qq.com/)， 打开微信公众平台官网：https://mp.weixin.qq.com/ 右上角点击“立即​注册”

