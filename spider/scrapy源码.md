
启动爬虫有以下几种方法：

```python
# 通过scrapy project，启动爬虫，<spider>可通过scrapy list查找可用的名字
~$scrapy crawl [options] <spider>

# 通过spider文件启动爬虫，不依赖scrapy project
$scrapy runspider [options] <spider_file>
```

或单独写个爬虫启动python文件

```python
# 单独的启动文件，通过scrapy命令行启动爬虫
scrapy.cmdline.execute("scrapy crawl myspider".split())
```

下面基于该爬虫启动文件，我们来分析下scrapy源码

# comdline.execute()

```python
#执行scrapy\cmdline.py中execute
#原型：def execute(argv=None, settings=None):
scrapy.cmdline.execute(['scrapy', 'crawl', 'example'])

## 这导致，调用：
## 1.scrapy\utils\project.py 中get_project_settings()，以填充settings dict
## 2.scrapy\utils\project.py中inside_project()，以import由SCRAPY_SETTINGS_MODULE指定模块
## 3._get_commands_dict(),以获取得到scrapy.cmdline所有的命令模块
## 4.optparse.OptionParser进行命令参数解析
## 5.scrapy\crawler.py中CrawlerProcess，生成对象：cmd.crawler_process = CrawlerProcess(settings)
## 6._run_print_help(parser, _run_command, cmd, args, opts), 导致执行_run_command(cmd, args, opts)
### 
```

## 生成CrawlerProcess对象

```python
# scrapy\crawler.py
class CrawlerProcess(CrawlerRunner):
```

- 执行 `CrawlerProcess.__init__`

```python
##这导致,调用：
## 1. scrapy\crawler.py中`CrawlerRunner.__init__`,其中self.spider_loader = self._get_spider_loader(settings)
## 2. install_shutdown_handlers(self._signal_shutdown)
## 3. 初始化logging，configure_logging(self.settings, install_root_handler)
## 4. 打印scrapy版本号信息， log_scrapy_info(self.settings)

2020-06-03 02:58:24 [scrapy.utils.log] INFO: Scrapy 2.1.0 started (bot: my)
2020-06-03 02:58:24 [scrapy.utils.log] INFO: Versions: lxml 4.5.1.0, libxml2 2.9.5, cssselect 1.1.0, parsel 1.6.0, w3lib 1.22.0, Twisted 20.3.0, Python 3.7.6 | packaged by conda-forge | (default, Mar 23 2020, 22:22:21) [MSC v.1916 64 bit (AMD64)], pyOpenSSL 19.1.0 (OpenSSL 1.1.1g  21 Apr 2020), cryptography 2.9.2, Platform Windows-7-6.1.7601-SP1
2020-06-03 02:58:24 [scrapy.utils.log] DEBUG: Using reactor: twisted.internet.selectreactor.SelectReactor
```

## run

```python
#scrapy\commands\crawl.py

#在本次跟踪中，是：
#run(['example'], {'logfile': None, 'loglevel': None, 'nolog': None, 
#                   'profile': None, 'pidfile': None, 'set': [], 'pdb': None, 
#                   'spargs': {}, 'output': None, 'output_format': None})
def run(self, args, opts):
    if len(args) < 1:
        raise UsageError()
    elif len(args) > 1:
        raise UsageError("running 'scrapy crawl' with more than one spider is no longer supported")
    #获取spider project name
    spname = args[0]
    
    #scrapy\crawler.py中,生成crawl 
    crawl_defer = self.crawler_process.crawl(spname, **opts.spargs)

    if getattr(crawl_defer, 'result', None) is not None and issubclass(crawl_defer.result.type, Exception):
        self.exitcode = 1
    else:
        #启动
        self.crawler_process.start()

    if self.crawler_process.bootstrap_failed or \
            (hasattr(self.crawler_process, 'has_exception') and self.crawler_process.has_exception):
        self.exitcode = 1
```

其中生成crawl，将打印日志：

```text
2020-06-03 03:22:34 [scrapy.crawler] INFO: Overridden settings:
{'BOT_NAME': 'my',
 'NEWSPIDER_MODULE': 'my.spiders',
 'ROBOTSTXT_OBEY': True,
 'SPIDER_MODULES': ['my.spiders']}
2020-06-03 03:22:34 [scrapy.extensions.telnet] INFO: Telnet Password: 2663d5def7a4325a
2020-06-03 03:22:34 [scrapy.middleware] INFO: Enabled extensions:
['scrapy.extensions.corestats.CoreStats',
 'scrapy.extensions.telnet.TelnetConsole',
 'scrapy.extensions.logstats.LogStats']
 ```

这个run主要动作可以和[Run Scrapy from a script](https://doc.scrapy.org/en/latest/topics/practices.html#run-scrapy-from-a-script)对照着看：

```python
import scrapy
from scrapy.crawler import CrawlerProcess

class MySpider(scrapy.Spider):
    # Your spider definition
    ...

process = CrawlerProcess(settings={
    "FEEDS": {
        "items.json": {"format": "json"},
    },
})

process.crawl(MySpider)
process.start() # the script will block here until the crawling is finished
```

### 生成对象

由`self.crawler_process.crawl`（scrapy/crawler.py中CrawlerRunner类的方法crawl）触发，生成相关对象crawl，engine，spider... 下面对各部分进行介绍

```python
#scrapy/crawler.py中CrawlerRunner类的方法crawl
    def crawl(self, crawler_or_spidercls, *args, **kwargs):
        """
        Run a crawler with the provided arguments.

        It will call the given Crawler's :meth:`~Crawler.crawl` method, while
        keeping track of it so it can be stopped later.

        If `crawler_or_spidercls` isn't a :class:`~scrapy.crawler.Crawler`
        instance, this method will try to create one using this parameter as
        the spider class given to it.

        Returns a deferred that is fired when the crawling is finished.

        :param crawler_or_spidercls: already created crawler, or a spider class
            or spider's name inside the project to create it
        :type crawler_or_spidercls: :class:`~scrapy.crawler.Crawler` instance,
            :class:`~scrapy.spiders.Spider` subclass or string

        :param list args: arguments to initialize the spider

        :param dict kwargs: keyword arguments to initialize the spider
        """
        crawler = self.create_crawler(crawler_or_spidercls)
        return self._crawl(crawler, *args, **kwargs)
```

其中的“`self._crawl`”内容如下

```python
    #scrapy/crawler.py中CrawlerRunner类的方法_crawl
    def _crawl(self, crawler, *args, **kwargs):
        self.crawlers.add(crawler)
        #调用Crawler类对象的crawl方法，创建spider，engine...
        d = crawler.crawl(*args, **kwargs)
        self._active.add(d)

        def _done(result):
            self.crawlers.discard(crawler)
            self._active.discard(d)
            self.bootstrap_failed |= not getattr(crawler, 'spider', None)
            return result

        return d.addBoth(_done)
```

#### Crawler类对象的crawl方法

```python
#scrapy\crawler.py中
class Crawler(object):
    @defer.inlineCallbacks
    def crawl(self, *args, **kwargs):
        assert not self.crawling, "Crawling already taking place"
        self.crawling = True

        try:
            self.spider = self._create_spider(*args, **kwargs)
            self.engine = self._create_engine()
            start_requests = iter(self.spider.start_requests())
            yield self.engine.open_spider(self.spider, start_requests)
            yield defer.maybeDeferred(self.engine.start)
        except Exception:
            # In Python 2 reraising an exception after yield discards
            # the original traceback (see https://bugs.python.org/issue7563),
            # so sys.exc_info() workaround is used.
            # This workaround also works in Python 3, but it is not needed,
            # and it is slower, so in Python 3 we use native `raise`.
            if six.PY2:
                exc_info = sys.exc_info()

            self.crawling = False
            if self.engine is not None:
                yield self.engine.close()

            if six.PY2:
                six.reraise(*exc_info)
            raise
               ...
```

### crawler_process.start()



```python
#scrapy/crawler.py中CrawlerProcess类的方法start
    def start(self, stop_after_crawl=True):
        """
        This method starts a Twisted `reactor`_, adjusts its pool size to
        :setting:`REACTOR_THREADPOOL_MAXSIZE`, and installs a DNS cache based
        on :setting:`DNSCACHE_ENABLED` and :setting:`DNSCACHE_SIZE`.

        If `stop_after_crawl` is True, the reactor will be stopped after all
        crawlers have finished, using :meth:`join`.

        :param boolean stop_after_crawl: stop or not the reactor when all
            crawlers have finished
        """
        if stop_after_crawl:
            d = self.join()
            # Don't start the reactor if the deferreds are already fired
            if d.called:
                return
            d.addBoth(self._stop_reactor)

        reactor.installResolver(self._get_dns_resolver())
        tp = reactor.getThreadPool()
        tp.adjustPoolsize(maxthreads=self.settings.getint('REACTOR_THREADPOOL_MAXSIZE'))
        reactor.addSystemEventTrigger('before', 'shutdown', self.stop)
        #启动事件循环
        reactor.run(installSignalHandlers=False)  # blocking call
```


# 执行过程

文件路径:"scrapy\commands\shell.py"

```python
class Command(ScrapyCommand):
    ...

    def run(self, args, opts):
        url = args[0] if args else None
        if url:
            # first argument may be a local file
            url = guess_scheme(url)

        spider_loader = self.crawler_process.spider_loader

        spidercls = DefaultSpider
        if opts.spider:
            spidercls = spider_loader.load(opts.spider)
        elif url:
            spidercls = spidercls_for_request(spider_loader, Request(url),
                                              spidercls, log_multiple=True)

        # The crawler is created this way since the Shell manually handles the
        # crawling engine, so the set up in the crawl method won't work
        crawler = self.crawler_process._create_crawler(spidercls)
        # The Shell class needs a persistent engine in the crawler
        crawler.engine = crawler._create_engine()
        crawler.engine.start()

        self._start_crawler_thread()

        shell = Shell(crawler, update_vars=self.update_vars, code=opts.code)
        shell.start(url=url, redirect=not opts.no_redirect)
        ...
```
