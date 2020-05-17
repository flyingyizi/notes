
[html5 microData](http://html5doctor.com/microdata/)

[live microdata](https://foolip.org/microdatajs/live/)可以实验提取microData。实验下

```html
<section>
  <h3><a href="http://atnd.org/events/5181" title="WDE-ex Vol11『iPad
  のウェブデザイン：私たちがみつけたこと 』 : ATND">WDE-ex Vol.11 — Designing
  for iPad: Our experience so far</a></h3>
  <p>On <time datetime="2010-07-21T19:00:00+09:00">July 21st 19:00
  </time>-<time datetime="2010-07-21T20:00:00+09:00">20:00</time> at
  <span itemscope itemtype="http://schema.org/Organization">
  <a itemprop="url" href="http://www.apple.com/jp/retail/ginza/map/">
  <span itemprop="name">Apple Ginza</span></a></span>,
  <span itemscope itemtype="http://schema.org/Person">
  <a itemprop="url" href="http://informationarchitects.jp/" title="iA">
  <span itemprop="name">Oliver Reichenstein</span>, CEO of iA</a>
  </span>, will share the lessons they've learned while creating three
  iPad apps and one iPad website.</p>
</section>
```

得到的json数据是

```json
{
  "items": [
    {
      "type": [
        "http://schema.org/Organization"
      ],
      "properties": {
        "url": [
          "http://www.apple.com/jp/retail/ginza/map/"
        ],
        "name": [
          "Apple Ginza"
        ]
      }
    },
    {
      "type": [
        "http://schema.org/Person"
      ],
      "properties": {
        "url": [
          "http://informationarchitects.jp/"
        ],
        "name": [
          "Oliver Reichenstein"
        ]
      }
    }
  ]
}
```

microData是通过属性来定义，记住不是元素。

## itemscope and itemprop

- itemscope — 定义item group。defines a group of name-value pair(s), called an item

- itemprop="property-name" — 定义item。adds a property to a microdata item. The name of the property can be a word or URL, and the value is the ‘content’ of the element with this attribute

```html
<p itemscope>I’m going to the <span itemprop="name">Salter Cane</span> gig next week.
Excited!</p>
```

上面的例子可知： name = "Salter Cane"

但是对下面的例子，就是 url="http://www.saltercane.com/", 以及date="2010-07-18"

```html
<p itemscope>I’m going to the <a itemprop="url"
  href="http://www.saltercane.com/">Salter Cane</a> gig
  <time itemprop="date" datetime="2010-07-18">next week</time>. Excited!</p>
```

