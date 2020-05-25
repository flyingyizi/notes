

# 环境准备：

以下都是基于python3进行开发， 后面不再说明

```sh
#安装镜像源
conda config –add channels https://mirrors.ustc.edu.cn/anaconda/pkgs/free/
conda config --add channels https://mirrors.ustc.edu.cn/anaconda/pkgs/free/
conda config --add channels https://mirrors.ustc.edu.cn/anaconda/pkgs/main/
conda config --set show_channel_urls yes

#检查conda版本
conda --version
#升级conda
conda update conda
#创建新环境
conda create -n myenv
#移除环境
conda remove -n myenv --all
#激活某个环境
activate myenv
#返回原来环境
deactivate
#列出所有环境
conda info --envs
#给新环境安装Python3，并命名为python3：
conda create -n python3 python=3
#检查环境内的Python版本：
python --version
#查找包
conda search beautifulsoup4


conda install scray
scrapy startproject yourproject_name
scrapy crawl the_name_in_your_class_of_yourcode
```

如果采用vsocde作为python编程环境，以下几个操作常用：

- 在vscodeCommand Palette，通过ctrl+shift+p拉起，执行“Python: Select Interpreter”。通过该命令在多个python环境中选择自己希望的python环境。这个操作的结果是会设置“python.pythonPath ”值

- 在vscode setting中，搜索“python>auto complete: add brackets”,选择enable，这样对函数类会自动加上括号

- typeshed 是 Python 的 library stub 的集合，用于为Python标准库和Python内建，以及第三方软件包。此数据可用于静态分析，类型检查和类型推断。在 Python>Auto Complete:Typeshed Paths选项中添加Python安装路径中 site-packages 和 Scrips 所在文件夹的路径（例如“C:\prog\Anaconda3\Lib\site-packages”与“C:\prog\Anaconda3\Scripts”）

- 如果第三方python库的自动提示有问题，可以尝试enable jedi，不使用微软的python server。enable jedi方法是在vscode setting中搜索jedi enabled，并选中。

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

[scrapy  tutorial](https://doc.scrapy.org/en/latest/intro/tutorial.html)

[scrapy1.5中文文档](http://www.scrapyd.cn/doc/)

https://miyakogi.github.io/pyppeteer/reference.html

# scrapy

## scrapy shell

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
>>> shelp()
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



