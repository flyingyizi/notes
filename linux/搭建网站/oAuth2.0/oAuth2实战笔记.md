

规范：[The OAuth 2.0 Authorization Framework](https://tools.ietf.org/html/rfc6749)

[随书代码](https://github.com/oauthinaction/oauth-in-action-code)

[阮一峰 oAuth2 教程](http://www.ruanyifeng.com/blog/2019/04/oauth_design.html)

[go-oauth-example](https://github.com/sohamkamani/go-oauth-example)

[登录github](github.com/login)后，通过“Settings--> Developer settings-->OAuth Apps-->Register a new application”,或[直接链接](https://github.com/settings/applications/new),进行登记：

例如登记以下信息：

```text
application name: my-oauth2-demo
homepage url:     http://localhost:8080
application description: 实验oAuth2
authorization callback url：http://localhost:8080/oauth2/redirect
```

登记完成，提交表单以后，GitHub 应该会返回客户端 ID（client ID）和客户端密钥（client secret），这就是应用的身份识别码。

```text
Client ID
    f577fa3d6d2e3748ce75
Client Secret
    53a411123c46dee15ea769774448f5d928e25e25
```




有没有一种办法，让快递员能够自由进入小区，又不必知道小区居民的密码，而且他的唯一权限就是送货，其他需要密码的场合，他都没有权限？  

oAuth2就是用来解决类似授权场景的。OAuth 是一种授权机制。数据的所有者告诉系统，同意授权第三方应用进入系统，获取这些数据。系统从而产生一个短期的进入令牌（token），用来代替密码，供第三方应用使用。

OAuth的参与实体至少有如下三个：

- · RO (resource owner): 资源所有者，对资源具有授权能力的人。如上文中的用户Alice。

- · RS (resource server): 资源服务器，它存储资源，并处理对资源的访问请求。如Google资源服务器，它所保管的资源就是用户Alice的照片。

- · Client: 第三方应用，它获得RO的授权后便可以去访问RO的资源。如网易印像服务。

此外，为了支持开放授权功能以及更好地描述开放授权协议，OAuth引入了第四个参与实体：

- · AS (authorization server): 授权服务器，它认证RO的身份，为RO提供授权审批流程，并最终颁发授权令牌(Access Token)。读者请注意，为了便于协议的描述，这里只是在逻辑上把AS与RS区分开来；在物理上，AS与RS的功能可以由同一个服务器来提供服务。

![](image/oAuth-abstact-protocol-flow.dib)

如上图所示，协议的基本流程如下：

- (1) Client请求RO的授权，请求中一般包含：要访问的资源路径，操作类型，Client的身份等信息。

- (2) RO批准授权，并将“授权证据”发送给Client。至于RO如何批准，这个是协议之外的事情。典型的做法是，AS提供授权审批界面，让RO显式批准。这个可以参考下一节实例化分析中的描述。

- (3) Client向AS请求“访问令牌(Access Token)”。此时，Client需向AS提供RO的“授权证据”，以及Client自己身份的凭证。

- (4) AS验证通过后，向Client返回“访问令牌”。访问令牌也有多种类型，若为bearer类型，那么谁持有访问令牌，谁就能访问资源。

- (5) Client携带“访问令牌”访问RS上的资源。在令牌的有效期内，Client可以多次携带令牌去访问资源。

- (6) RS验证令牌的有效性，比如是否伪造、是否越权、是否过期，验证通过后，才能提供服务。

2.2 授权类型

OAuth为了支持这些不同类型的第三方应用，提出了多种授权类型，如
- 授权码 (Authorization Code Grant)、
- 隐式授权 (Implicit Grant)、
- RO凭证授权 (Resource Owner Password Credentials Grant)、
- Client凭证授权 (Client Credentials Grant)。

不管哪一种授权方式，第三方应用申请令牌之前，都必须先到系统备案，说明自己的身份，然后会拿到两个身份识别码：客户端 ID（client ID）和客户端密钥（client secret）。这是为了防止令牌被滥用，没有备案过的第三方应用，是不会拿到令牌的。

## 授权类型：授权码(Authorization Code)

授权码（authorization code）方式，指的是第三方应用先申请一个授权码，然后再用该码获取令牌。

![](image/Authorization-Code-Flow.dib)

如上图所示，授权码类型的开放授权协议流程描述如下：

- (1) Client初始化协议的执行流程。首先通过HTTP 302来重定向RO用户代理到AS。Client在redirect_uri中应包含如下参数：client_id, scope (描述被访问的资源), redirect_uri (即Client的URI), state (用于抵制CSRF攻击). 此外，请求中还可以包含access_type和approval_prompt参数。当approval_prompt=force时，AS将提供交互页面，要求RO必须显式地批准（或拒绝）Client的此次请求。如果没有approval_prompt参数，则默认为RO批准此次请求。当access_type=offline时，AS将在颁发access_token时，同时还会颁发一个refresh_token。因为access_token的有效期较短（如3600秒），为了优化协议执行流程，offline方式将允许Client直接持refresh_token来换取一个新的access_token。

- (2) AS认证RO身份，并提供页面供RO决定是否批准或拒绝Client的此次请求（当approval_prompt=force时）。

(3) 若请求被批准，AS使用步骤(1)中Client提供的redirect_uri重定向RO用户代理到Client。redirect_uri须包含authorization_code，以及步骤1中Client提供的state。若请求被拒绝，AS将通过redirect_uri返回相应的错误信息。

(4) Client拿authorization_code去访问AS以交换所需的access_token。Client请求信息中应包含用于认证Client身份所需的认证数据，以及上一步请求authorization_code时所用的redirect_uri。

(5) AS在收到authorization_code时需要验证Client的身份，并验证收到的redirect_uri与第3步请求authorization_code时所使用的redirect_uri相匹配。如果验证通过，AS将返回access_token，以及refresh_token（若access_type=offline）。


## 隐式授权 (Implicit Grant)




[使用dex搭建一个身份认证系统](https://jeremyxu2010.github.io/2019/08/%E4%BD%BF%E7%94%A8dex%E6%90%AD%E5%BB%BA%E4%B8%80%E4%B8%AA%E8%BA%AB%E4%BB%BD%E8%AE%A4%E8%AF%81%E7%B3%BB%E7%BB%9F/)