
gcocolly+goquery可以非常好地抓取HTML页面中的数据，但碰到页面是由Javascript动态生成时

解析网页，获取并操作html的dom

- java里用Jsoup，
- nodejs里用cheerio，
- golang里用goquery, 例子如[使用Golang实现网页爬虫](https://www.coderminer.com/blog/post/6714ba8b82159bcd25b2d5206983bf0f)

解析动态内容，所谓无头（Headless）浏览器就出现了，借助这个工具可以模拟webkit执行，还可以包含更多js库比如jQuery等对页面的js执行进行扩展。

- python 里用phontmJs
- golang 里用webloop
- java/python/golang 使用Headless Chrome headless firefox
  
Java的几个爬虫实现方式：HttpClients+jsoup，Jsoup，htmlunit，Headless Chrome

[golang使用chrome headless获取网页内容](https://www.cnblogs.com/apocelipes/p/9264673.html)

[headless chrome, Getting Started with Headless Chrome](https://developers.google.cn/web/updates/2017/04/headless-chrome)

[Headless chrome + ignore-certificate-errors](https://stackoverflow.com/questions/45510973/headless-chrome-ignore-certificate-errors)

```text
Simply said, Crawler4j is static crawler. Meaning that it can't parse the JavaScript on a page. So there is no way of getting the content you want by crawling that specific page you mentioned. Of course there are some workarounds to get it working.

If it is just this page you want to crawl, you could use a connection debugger. Check out this question for some tools. Find out which page the AJAX-request calls, and crawl that page.

If you have various websites which have dynamic content (JavaScript/ajax), you should consider using a dynamic-content-enabled crawler, like [Crawljax](http://www.crawljax.com/) (also written in Java).
```

```text
http://www.cnblogs.com/majianguo/p/8325314.html

gcocolly+goquery可以非常好地抓取HTML页面中的数据，但碰到页面是由Javascript动态生成时，用goquery就显得捉襟见肘了。解决方法有很多种，一，最笨拙但有效的方法是字符串处理，go语言string底层对应字节数组，复制任何长度的字符串的开销都很低廉，搜索性能比较高；二，利用正则表达式，要提取的数据往往有明显的特征，所以正则表达式写起来比较简单，不必非常严谨；三，使用浏览器控件，比如webloop；四，直接操纵浏览器，比如chromedp。一和二需要goquery提取javascript，然后再提取数据，速度非常快；三和四不需要分析脚本，在浏览器执行JavaScript生成页面后再提取，是比较偷懒的方式，但缺点是执行速度很慢，如果在脚本非常复杂、懒得分析脚本的情况下，牺牲下速度也是不错的选择。
```

[go lang chrome 爬虫,收集的几个资源](https://blog.csdn.net/fenglailea/article/details/82907480)

[web-scraping-in-2017-headless-chrome-tips-tricks](https://blog.phantombuster.com/web-scraping-in-2017-headless-chrome-tips-tricks-4d6521d695e8?gi=351b161e86f8)

[chromedp](go get -u github.com/chromedp/chromedp)

- 安装谷歌浏览器，
   如果需要使用chrome headless能力，至少需要60版本及以上，最新版本肯定是具备headless能力的。下面以linux为例说明下载路径
    ```shell

    #对于谷歌Chrome32位版本，使用如下链接：
    wget https://dl.google.com/linux/direct/google-chrome-stable_current_i386.deb
    #对于64位版本可以使用如下链接下载：
    wget https://dl.google.com/linux/direct/google-chrome-stable_current_amd64.deb

    sudo dpkg -i google-chrome*; sudo apt-get -f install
    ##然后，就可以去搜索使用了。 
    ```

- 安装并使用chrome xpath helper
  - 安装
    ```shell
    #1.对chrome的快捷键添加参数：在目标(T)后添加参数   --enable-easy-off-store-extension-install （注意在添加参数之前，要有个空格），添加完之后点击确认
    #2.通过快捷键启动chrome，进入谷歌浏览器的 "拓展程序" 页面（即在谷歌浏览器地址栏输入：chrome://extensions/）。将下载下来的“xpath-helper.crx”拖到里面
    ```
  - 使用
    在chrome中ctrl+shift+x 启动xpath helper，启动后会出现xpath 的query/result 窗口

    在query窗口录入/或通过chrome F12查找拷贝到的xpath录入在query窗口中，可以实时在result窗口与浏览器中看到结果



