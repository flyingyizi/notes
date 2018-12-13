
本章节内容来自(http://www.cnblogs.com/sunfie/tag/Elasticsearch%E5%AD%A6%E4%B9%A0/)

- 数据准备

为了展示Elasticsearch中不同查询的用法，首先在Elasticsearch里面创建了employee相关的documents，每本书主要涉及以下字段： first_name, last_name, age,about,interests,操作如下：
```shell
curl -H'Content-Type: application/json'  -XPUT 'localhost:9200/megacorp/employee/3' -d '{ "first_name" : "Douglas", "last_name" : "Fir", "age" : 35, "about" : "I like to build cabinets", "interests": "forestry" }'
curl -H'Content-Type: application/json'  -XPUT 'localhost:9200/megacorp/employee/2' -d '{ "first_name" : "Jane", "last_name" : "Smith", "age" : 32, "about" : "I like to collect rock albums", "interests": "music" }'
curl -H'Content-Type: application/json'  -XPUT 'localhost:9200/megacorp/employee/1' -d '{ "first_name" : "John", "last_name" : "Smith", "age" : 25, "about" : "I love to go rock climbing", "interests": [ "sports", "music" ] }'
```

## 基本匹配查询(Basic Match Query)

基本匹配查询主要有两种形式：
- （1）使用Search Lite API，并将所有的搜索参数都通过URL传递；
- （2）使用Elasticsearch DSL，其可以通过传递一个JSON请求来获取结果。下面是在所有的字段中搜索带有"John"的结果
```shell
curl -H'Content-Type: application/json' -XGET 'localhost:9200/megacorp/employee/_search?q=John'
```
如果我们使用Query DSL来展示出上面一样的结果可以这么来写：
```shell
curl -H'Content-Type: application/json' -XGET 'localhost:9200/megacorp/_search' -d '
{
    "query": {
        "multi_match" : {
            "query" : "John"
        }
    }
}'
下面这个没有查询到，因为_all关键字在+6.0不支持了
curl -H'Content-Type: application/json' -XGET 'localhost:9200/megacorp/_search' -d '
{
    "query": {
        "multi_match" : {
            "query" : "John",
            "fields" : ["_all"]
        }
    }
}'
```

其输出和上面使用“/_ search?q=john”的输出一样。上面的multi_match关键字通常在查询多个fields的时候作为match关键字的简写方式。fields属性指定需要查询的字段，如果我们想查询所有的字段，这时候可以使用_all关键字，正如上面的一样。以上两种方式都允许我们指定查询哪些字段。比如，我们想查询interest中出现music的员工，那么我们可以这么查询：
```shell
curl -XGET 'localhost:9200/megacorp/employee/_search?q=interests:music'
```
然而，DSL方式提供了更加灵活的方式来构建更加复杂的查询（我们将在后面看到），甚至指定你想要的返回结果。下面的例子中，我将指定需要返回结果的数量，开始的偏移量（这在分页的情况下非常有用），需要返回document中的哪些字段以及高亮关键字：
```shell
curl -H'Content-Type: application/json' -XGET 'localhost:9200/megacorp/employee/_search?pretty' -d '
{
    "query": { 
        "match" : { 
            "interests" : "music" 
        }
    },
    "size": 2,
    "from": 0,
    "_source": [ 
        "first_name", 
        "last_name", 
        "interests" 
    ],
    "highlight": {
        "fields" : { 
            "interests" : { } 
        } 
    } 
}'
```
需要注意的是：对于查询多个关键字，match关键字允许我们使用and操作符来代替默认的or操作符。你也可以指定minimum_should_match操作符来调整返回结果的相关性(tweakrelevance)。

## Multi-field Search
正如我们之前所看到的，想在一个搜索中查询多个 document field （比如使用同一个查询关键字同时在title和summary中查询），你可以使用multi_match查询，使用如下：
```shell
curl -H'Content-Type: application/json'  -XGET 'localhost:9200/megacorp/employee/_search' -d '
{
    "query": {
        "multi_match" : {
            "query" : "rock",
            "fields": ["about", "interests"]
        }
    }
}'
```
## Boosting
　　我们上面使用同一个搜索请求在多个field中查询，你也许想提高某个field的查询权重,在下面的例子中，我们把interests的权重调成3，这样就提高了其在结果中的权重，这样把_id=4的文档相关性大大提高了，如下：
```shell
curl -s -H'Content-Type: application/json' -XGET 'localhost:9200/megacorp/employee/_search' -d '
{
    "query": {
        "multi_match" : {
            "query" : "rock",
            "fields": ["about", "interests^3"]
        }
    }
}'
```
Boosting不仅仅意味着计算出来的分数(calculated score)直接乘以boost factor，最终的boost value会经过归一化以及其他一些内部的优化

## Bool Query
我们可以在查询条件中使用AND/OR/NOT操作符，这就是布尔查询(Bool Query)。布尔查询可以接受一个must参数(等价于AND)，一个must_not参数(等价于NOT)，以及一个should参数(等价于OR)。比如，我想查询about中出现music或者climb关键字的员工，员工的名字是John，但姓氏不是smith，我们可以这么来查询：
```shell
curl -XGET 'localhost:9200/megacorp/employee/_search' -d '
{
    "query": {
        "bool": {
                "must": {
                    "bool" : { 
                        "should": [
                            { "match": { "about": "music" }},
                            { "match": { "about": "climb" }} ] 
                    }
                },
                "must": {
                    "match": { "first_nale": "John" }
                },
                "must_not": {
                    "match": {"last_name": "Smith" }
                }
            }
    }
}'
```

## Fuzzy Queries（模糊查询）
模糊查询可以在Match和 Multi-Match查询中使用以便解决拼写的错误，模糊度是基于Levenshteindistance计算与原单词的距离。使用如下：
```shell
curl -XGET 'localhost:9200/megacorp/employee/_search' -d '
{
    "query": {
        "multi_match" : {
            "query" : "rock climb",
            "fields": ["about", "interests"],
            "fuzziness": "AUTO"
        }
    },
    "_source": ["about", "interests", "first_name"],
    "size": 1
}'
```
上面我们将fuzziness的值指定为AUTO，其在term的长度大于5的时候相当于指定值为2，然而80%的人拼写错误的编辑距离(edit distance)为1，所有如果你将fuzziness设置为1可能会提高你的搜索性能

## Wildcard Query(通配符查询)
通配符查询允许我们指定一个模式来匹配，而不需要指定完整的trem。?将会匹配如何字符；*将会匹配零个或者多个字符。比如我们想查找所有名字中以J字符开始的记录，我们可以如下使用：
```
curl -XGET 'localhost:9200/megacorp/employee/_search' -d '
{
    "query": {
            "wildcard" : {
                "first_name" : "s*"
            }
        },
        "_source": ["first_name", "last_name"],
    "highlight": {
            "fields" : {
                "first_name" : {}
            }
        }
}'
```

## Regexp Query(正则表达式查询)
ElasticSearch还支持正则表达式查询，此方式提供了比通配符查询更加复杂的模式。比如我们先查找作者名字以J字符开头，中间是若干个a-z之间的字符，并且以字符n结束的记录，可以如下查询：
```shell
curl -XGET 'localhost:9200/megacorp/employee/_search' -d '
{
    "query": {
        "regexp" : {
            "first_name" : "J[a-z]*n"
        }
    },
    "_source": ["first_name", "age"],
    "highlight": {
        "fields" : {
            "first_name" : {}
        }
    }
}'
```

## Match Phrase Query(匹配短语查询)
匹配短语查询要求查询字符串中的trems要么都出现Document中、要么trems按照输入顺序依次出现在结果中。在默认情况下，查询输入的trems必须在搜索字符串紧挨着出现，否则将查询不到。不过我们可以指定slop参数，来控制输入的trems之间有多少个单词仍然能够搜索到，如下所示：
```shell
curl -XGET 'localhost:9200/megacorp/employee/_search' -d '
{
    "query": {
        "multi_match": {
            "query": "climb rock",
            "fields": [
                "about",
                "interests"
            ],
            "type": "phrase",
            "slop": 3
        }
    },
    "_source": [
        "title",
        "about",
        "interests"
    ]
}'
```
从上面的例子可以看出，id为4的document被搜索（about字段里面精确匹配到了climb rock），并且分数比较高；而id为1的document也被搜索到了，虽然其about中的climb和rock单词并不是紧挨着的，但是我们指定了slop属性，所以被搜索到了。如果我们将"slop":3条件删除，那么id为1的文档将不会被搜索到。

## Match Phrase Prefix Query(匹配短语前缀查询)
匹配短语前缀查询可以指定单词的一部分字符前缀即可查询到该单词，和match phrase query一样我们也可以指定slop参数；同时其还支持max_expansions参数限制被匹配到的terms数量来减少资源的使用,使用如下：
```shell
curl -XGET 'localhost:9200/megacorp/employee/_search' -d '
{
    "query": {
        "match_phrase_prefix": {
            "summary": {
                "query": "cli ro",
                "slop": 3,
                "max_expansions": 10
            }
        }
    },
    "_source": [
        "about",
        "interests",
        "first_name"
    ]
}'
```

## Query String
　　query_string查询提供了一种手段可以使用一种简洁的方式运行multi_match queries, bool queries, boosting, fuzzy matching, wildcards, regexp以及range queries的组合查询。在下面的例子中，我们运行了一个模糊搜索(fuzzy search)，搜索关键字是search algorithm，并且作者包含grant ingersoll或者tom morton。并且搜索了所有的字段，其中summary字段的权重为2：
```shell
curl -XGET 'localhost:9200/megacorp/employee/_search' -d '
{
    "query": {
        "query_string" : {
            "query": "(saerch~1 algorithm~1) AND (grant ingersoll) OR (tom morton)",
            "fields": ["_all", "summary^2"]
        }
    },
    "_source": [ "title", "summary", "authors" ],
    "highlight": {
        "fields" : {
            "summary" : {}
        }
    }
}'
```

## Simple Query String(简单查询字符串)
　　simple_query_string是query_string的另一种版本，其更适合为用户提供一个搜索框中，因为其使用+/|/- 分别替换AND/OR/NOT，如果用输入了错误的查询，其直接忽略这种情况而不是抛出异常。使用如下：
```shell
curl -POST 'localhost:9200/megacorp/employee/_search' -d '
{
    "query": {
        "simple_query_string" : {
        "query": "(saerch~1 algorithm~1) + (grant ingersoll) | (tom morton)",
        "fields": ["_all", "summary^2"]
        }
    },
    "_source": [ "title", "summary", "authors" ],
    "highlight": {
        "fields" : {
            "summary" : {}
        }
    }
}'
```

12. Term/Terms Query
　　前面的例子中我们已经介绍了全文搜索(full-text search)，但有时候我们对结构化搜索中能够精确匹配并返回搜索结果更感兴趣。这种情况下我们可以使用term和terms查询。在下面例子中，我们想搜索所有兴趣中有music的人：
```shell
curl -POST 'localhost:9200/megacorp/employee/_search' -d '
{
    "query": {
        "term" : {
            "interests": "music"
        }
    },
    "_source" : ["first_name","last_name","interests"]
}'
```

我们还可以使用terms关键字来指定多个terms，如下：
```shell
{
    "query": {
        "terms" : {
            "publisher": ["oreilly", "packt"]
        }
    }
}
```

## Term Query - Sorted

　　查询结果和其他查询结果一样可以很容易地对其进行排序，而且我们可以对输出结果按照多层进行排序：
```shell
curl -XPOST 'localhost:9200/megacorp/employee/_search' -d '
{
    "query": {
        "term" : {
            "interests": "music"
        }
    },
    "_source" : ["interests","first_name","about"],
    "sort": [
        { "publish_date": {"order":"desc"}},
        { "id": { "order": "desc" }}
    ]
}'
```

## Range Query(范围查询)
另一种结构化查询就是范围查询。在下面例子中，我们搜索所有发行年份为2015的图书：
```shell
curl -XPOST 'localhost:9200/person/worker/_search?pretty' -d '
{
    "query": {
        "range" : {
            "birthday": {
                "gte": "2017-02-01",
                "lte": "2017-05-01"
            }
        }
    },
    "_source" : ["first_name","last_name","birthday"]
}'
```

范围查询可以应用于日期，数字以及字符类型的字段。

 

## Filtered Query(过滤查询)
　　过滤查询允许我们对查询结果进行筛选。比如：我们查询about和interests中包含music关键字的员工，但是我们想过滤出birthday大于2017/02/01的结果，可以如下使用：
```shell
curl -XPOST :9200/megacorp/employee/_search?pretty' -d '
{
    "query": {
        "filtered": {
            "query" : {
                "multi_match": {
                    "query": "music",
                    "fields": ["about","interests"]
                }
            },
            "filter": {
                "range" : {
                    "birthday": {
                        "gte": 2017-02-01
                    }
                }
            }
        }
    },
    "_source" : ["first_name","last_name","about", "interests"]
}'
```

注意：过滤查询(Filtered queries)并不强制过滤条件中指定查询,如果没有指定查询条件，则会运行match_all查询，其将会返回index中所有文档，然后对其进行过滤，在实际运用中，过滤器应该先被执行，这样可以减少需要查询的范围，而且，第一次使用fliter之后其将会被缓存，这样会对性能代理提升。Filtered queries在即将发行的Elasticsearch 5.0中移除了，我们可以使用bool查询来替换他，下面是使用bool查询来实现上面一样的查询效果，返回结果一样：
```shell
curl -XPOST 'localhost:9200/megacorp/employee/_search?pretty' -d '
{
    "query": {
        "bool": {
            "must" : {
                "multi_match": {
                    "query": "music",
                    "fields": ["about","interests"]
                }
            },
            "filter": {
                "range" : {
                    "birthday": {
                        "gte": 2017-02-01
                    }
                }
            }
        }
    },
    "_source" : ["first_name","last_name","about", "interests"]
}'
```

## Multiple Filters(多过滤器查询)
　　多过滤器查询可以通过结合使用bool过滤查询实现。下面的示例中，我们将筛选出返回的结果必须至少有20条评论，必须是在2015年之前发布的，而且应该是由O'Reilly出版的，首先建立索引iteblog_book_index并向其插入数据，如下所示：
```shell
curl -XPOST 'localhost:9200/iteblog_book_index/book/1' -d '{ "title": "Elasticsearch: The Definitive Guide", "authors": ["clinton gormley", "zachary tong"], "summary" : "A distibuted real-time search and analytics engine", "publish_date" : "2015-02-07","num_reviews": 20, "publisher": "oreilly" }'
curl -XPOST 'localhost:9200/iteblog_book_index/book/2' -d '{ "title": "Taming Text: How to Find, Organize, and Manipulate It", "authors": ["grant ingersoll", "thomas morton", "drew farris"], "summary" : "organize text using approaches such as full-text search, proper name recognition, clustering, tagging, information extraction, and summarization", "publish_date" : "2013-01-24", "num_reviews": 12, "publisher": "manning" }'
curl -XPOST 'localhost:9200/iteblog_book_index/book/3' -d '{ "title": "Elasticsearch in Action", "authors": ["radu gheorge", "matthew lee hinman", "roy russo"], "summary" : "build scalable search applications using Elasticsearch without having to do complex low-level programming or understand advanced data science algorithms", "publish_date" : "2015-12-03", "num_reviews": 18, "publisher": "manning" }'
curl -XPOST 'localhost:9200/iteblog_book_index/book/4' -d '{ "title": "Solr in Action", "authors": ["trey grainger", "timothy potter"], "summary" : "Comprehensive guide to implementing a scalable search engine using Apache Solr", "publish_date" : "2014-04-05", "num_reviews": 23, "publisher": "manning" }'
```

然后执行如下查询语句：
```shell
curl -XPOST 'localhost:9200/iteblog_book_index/book/_search?pretty' -d '
{
    "query": {
        "filtered": {
            "query" : {
                "multi_match": {
                "query": "elasticsearch",
                "fields": ["title","summary"]
                }
            },
            "filter": {
                "bool": {
                    "must": {
                        "range" : { "num_reviews": { "gte": 20 } }
                    },
                    "must_not": {
                        "range" : { "publish_date": { "lte": "2014-12-31" } }
                    },
                    "should": {
                        "term": { "publisher": "oreilly" }
                    }
                }
            }
        }
    },
    "_source" : ["title","summary","publisher", "num_reviews", "publish_date"]
}'
```

## Function Score: Field Value Factor
　　在某些场景下，你可能想对某个特定字段设置一个因子(factor)，并通过这个因子计算某个文档的相关度(relevance score)。这是典型地基于文档(document)的重要性来抬高其相关性的方式。在下面例子中，我们想找到更受欢迎的图书(是通过图书的评论实现的)，并将其权重抬高，这里可以通过使用field_value_factor来实现
```shell
curl -XPOST 'localhost:9200/iteblog_book_index/book/_search?pretty' -d '
{
    "query": {
        "function_score": {
            "query": {
                "multi_match" : {
                    "query" : "search engine",
                    "fields": ["title", "summary"]
                }
            },
            "field_value_factor": {
                "field" : "num_reviews",
                "modifier": "log1p",
                "factor" : 2
            }
        }
    },
    "_source": ["title", "summary", "publish_date", "num_reviews"]
}'
```

 