
#  架构

![scrapy架构图](./images/scrapy_architecture_02.png)


## scheduler

实现类是`SCHEDULER = 'scrapy.core.scheduler.Scheduler'`,
```python
class ExecutionEngine:

    def __init__(self, crawler, spider_closed_callback):
        ...
        self.scheduler_cls = load_object(self.settings['SCHEDULER'])
        ...
```

# 源码解读

见