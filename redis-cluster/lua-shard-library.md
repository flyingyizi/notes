

## 爬虫

- [[Web Scraper in Go, similar to BeautifulSoup]](https://github.com/anaskhan96/soup)
- [[bluemonday: a fast golang HTML sanitizer (inspired by the OWASP Java HTML Sanitizer) to scrub user generated content of XSS]](https://github.com/microcosm-cc/bluemonday)
- [[A little like that j-thing, only in Go]](https://github.com/PuerkitoBio/goquery)
- [[go Web爬虫]](https://www.ctolib.com/categories/go-crawler.html)
- [[Fast and Elegant Scraping Framework for Golang]](https://github.com/gocolly/colly/)
- [[如何伪装浏览器见]](https://zhidao.baidu.com/question/2117242032496816307.html)
- [[parse-broken-html-with-golang]](https://stackoverflow.com/questions/24101721/parse-broken-html-with-golang)
- [[xx爬虫教程]](https://zhuanlan.zhihu.com/p/25296437)


##  

- [[java 生成22位UUID 改造]](http://blog.csdn.net/sskicgah/article/details/12904881)
- [[超短的19位UUID，性能几乎翻倍提升]](http://www.iteye.com/topic/1134781)
- [[MongoDB的ObjectId]](http://helloyesyes.iteye.com/blog/1089927)
- [[Golang实现snowflake算法（分布式唯一id生成器） ]](https://studygolang.com/articles/9753)
- [[Twitter的分布式自增ID算法snowflake (Java版)]](https://www.cnblogs.com/relucent/p/4955340.html)

以默认的innodb存储引擎为例：做为主键时，uuid和自增相比较，自增更适合。原因：1 uuid是无序的， 插入数据时，页的位置会发生变化，页分裂，速度慢。2 uuid占的空间大，并且innodb中，别的索引还都要包含主键的值，那么每个索引的空间也都会增大，占的空间大，需要读数据时一般会认为需要的io次数多。

作者：河南-老宋(志强)
链接：https://www.zhihu.com/question/43500172/answer/113356943
来源：知乎
著作权归作者所有。商业转载请联系作者获得授权，非商业转载请注明出处。
