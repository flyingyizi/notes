
通过脚本启动爬虫：

```python
    main.py
    from scrapy.cmdline import execute #可执行scrapy 脚本
    import sys
    import os
    main_path = os.path.abspath(__file__)
    sys.path.append(os.path.dirname(main_path))
    execute(["scrapy","crawl","jobbole"])
```

scrapy命令（`/usr/local/bin/scrapy`）就是一个python脚本，内容是：

```python
#!/usr/bin/python
# -*- coding: utf-8 -*-
import re
import sys
from scrapy.cmdline import execute
if __name__ == '__main__':
sys.argv[0] = re.sub(r'(-script\.pyw|\.exe)?$', '', sys.argv[0])
sys.exit(execute())
```

scrapy命令依据传入的值crawl，会调用`crawl.py`中的scrapy.commands中Command.run()方法

```python
    #scrapy.commands.crawl.py:
    def run(self, args, opts):
        if len(args) < 1:
            raise UsageError()
        elif len(args)
            raise UsageError("running 'scrapy crawl' with more than one spider is no longer orted")
        spname = args[0]
          self.crawler_process.crawl(spname, **opts.spargs)#生成一个crawler对象
        self.crawler_process.start()
```

crawler_process的类定义在`from scrapy.crawler import CrawlerProcess`，run方法中调用了CrawlerProcess实例的crawl和start，就这样整个爬虫程序就会运行起来了。

上面这个crawl方法的核心就是
```python
def crawl(self, *args, **kwargs):
    .....
    try:
        self.spider = self._create_spider(*args, **kwargs)
        self.engine = self._create_engine()
        start_requests = iter(self.spider.start_requests())
        yield self.engine.open_spider(self.spider, start_requests)
        yield defer.maybeDeferred(self.engine.start)
    except Exception:
    .....
```

此后，就交由scrapy的引擎来处理了。  [参见](https://blog.csdn.net/weixin_37947156/article/details/74533108)


