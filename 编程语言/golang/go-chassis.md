
[代码提交指南](http://servicecomb.apache.org/cn/developers/submit-codes/)

# [Minimize Installation](https://docs.go-chassis.com/getstarted/install.html)

    Install go 1.10+

    Clone the project

   `git clone git@github.com:go-chassis/go-chassis.git`
    Use go mod(go 1.11+, experimental but a recommended way)

    ```shell
        cd go-chassis
        GO111MODULE=on go mod download
        #optional
        GO111MODULE=on go mod vendor
    ```
    Install [service-center](http://servicecomb.apache.org/release/)

    Docker:
    ```sh
    docker pull servicecomb/service-center
    docker run -d -p 30100:30100 servicecomb/service-center
    ```

Archaius为动态配置框架，可从各种不同的source中读取配置

不同协议请求进入到各协议Server，Server将具体的协议请求转换为Invocation统一抽象模型，并传入Handler chain，在这里Chassis已经默认实现了很多的Handler，比如熔断，限流等，最终再进入Transport handler，使用具体的协议客户端传输到目标。

开发 restful service时，必须： [Must implement URLPatterns](https://docs.go-chassis.com/getstarted/writing-rest.html), and for other functions must use *restful.Context as the only input, and certainly the method name must start with uppercase

# 使用帮助

[官方example](https://github.com/go-chassis/go-chassis-examples)

## 1. Micro Service Definition微服务定义

编写文件名为`microservice.yaml`的文件， 参见[编写指导](https://docs.go-chassis.com/user-guides/microservice.html)

对应字段结构见“go-chassis\core\config\model\chassis.go”中以下内容定义
```go
// MicroserviceCfg microservice.yaml 配置项
type MicroserviceCfg struct {
	AppID              string           `yaml:"APPLICATION_ID"`
	ServiceDescription MicServiceStruct `yaml:"service_description"`
}

// MicServiceStruct 设置微服务的私有属性
type MicServiceStruct struct {
	Name               string              `yaml:"name"`
	Hostname           string              `yaml:"hostname"`
	Version            string              `yaml:"version"`
	Environment        string              `yaml:"environment"`
	Level              string              `yaml:"level"`
	Properties         map[string]string   `yaml:"properties"`
	InstanceProperties map[string]string   `yaml:"instance_properties"`
	ServicePaths       []ServicePathStruct `yaml:"paths"`
	ServicesStatus     string              `yaml:"status"`
	Schemas            []string            `yaml:"schemas"`
}
```

## 2. Registry注册发现配置

通过`chassis.yaml`和`microservice.yaml`文件进行配置,参见[编写指导](https://docs.go-chassis.com/user-guides/registry.html)。

`chassis.yaml`中配置使用的注册中心类型、注册中心的地址信息。


## 3. Service Discovery服务发现配置

服务发现的配置在`chassis.yaml`文件。 参见[编写指导](https://docs.go-chassis.com/user-guides/service-discovery.html)

## 4. Protocol Servers协议配置

配置在`chassis.yaml`文件。 参见[编写指导](https://docs.go-chassis.com/user-guides/protocols.html)

例如

```yaml
cse:
  protocols:
    rest:
      listenAddress: 0.0.0.0:5000
    rest-admin:
      listenAddress: 0.0.0.0:5001
    grpc:
      listenAddress: 0.0.0.0:6000
```

## 5. Handler chain处理链
处理链中包含一系列handler, 在一次调用中可以通过扩展调用链的方式来插入定制的逻辑处理

### Handler chain处理链配置

配置在`chassis.yaml`文件。 参见[编写指导](https://docs.go-chassis.com/user-guides/handler-chain.html)

例如下面配置的handler部分

```yaml
protocols:
  rest:
    listenAddress: 127.0.0.1:5001
  rest-admin:
    listenAddress: 127.0.0.1:5002
  highway:
    listenAddress: 127.0.0.1:5003
  grpc:
    listenAddress: 127.0.0.1:5004
handler:
  chain:
    Provider:
      rest: custom-handler, ratelimiter-provider
      highway: tracing-provider
```

作为消费者，如果自定义编写了custom-handler，那结尾必须是`transport  handler`, 例如

```yaml
handler:
  chain:
    Consumer:
      default: custom-handler, bizkeeper-consumer, router, loadbalance, ratelimiter-consumer,transport
```

### 自定义handler

参见[handler开发指导](https://docs.go-chassis.com/dev-guides/how-to-implement-handler.html)

查看源码可知道， `go-chassis\core\handler\handler_chain.go`文件中的全局变量ChainMap存放最终加载的所有handler

```go
// ChainMap just concurrent read
var ChainMap = make(map[string]*Chain)

// Chain struct for service and handlers
type Chain struct {
	ServiceType string
	Name        string
	Handlers    []Handler
}
```

例如 `go-chassis\core\handler\fault_inject_handler.go`文件是系统默认提供的"fault-inject"handler

实现自定义handler需要以下三个步骤：

- 实现满足下面接口的类，
    ```go
    //定义在 go-chassis\core\handler\handler.go
    // Handler interface for handlers
    type Handler interface {
        // handle invocation transportation,and tr response
        Handle(*Chain, *invocation.Invocation, invocation.ResponseCallBack)
        Name() string
    }
    ```
- 调用`handler.RegisterHandler(...)`注册上面的实现类，  
- 最后在chassis.yaml中添加handler chain相关配置。其中[service_type] 可配置为Provider或Consumer，[chain_name]默认为default。

下面是一个自定义插件的示例

首先编写插件
```go
package fake
import (
    "github.com/go-chassis/go-chassis/core/handler"
    "github.com/go-chassis/go-chassis/core/invocation"
    "log"
)
const Name = "fake-handler"
type FakeHandler struct{}

func init()                         { handler.RegisterHandler(Name, New) }
func New() handler.Handler          { return &FakeHandler{} }

func (h *FakeHandler) Name() string { return Name }
func (h *FakeHandler) Handle(chain *handler.Chain, inv *invocation.Invocation,
                             cb invocation.ResponseCallBack) {
    log.Printf("fake handler running for %v", inv.Endpoint)
    chain.Next(inv, cb)    
}
```

最后，在chassis.yaml中增加对应插件配置
```yaml
cse:
  handler:
    chain:
      Provider:
        default: fake-handler
```

### 使用预定义middleWare

参见 [Basic Auth middleWare](https://docs.go-chassis.com/middleware/basic-auth.html#id1)

在上面自定义插件编写中，写的代码是比较多的。因此框架也提供了部分已经完成了大部分代码的半成品插件（也称为middleware）。 

以使用basic auth为例，示例如何在自己的业务代码中使用它们

```go
import "github.com/go-chassis/go-chassis/middleware/basicauth"
ba = basicauth.BasicAuth{
	Realm: "test-realm",
	Authorize: func(user, password string) error {
	    //check your user name and password
	    return nil
	},
}
    
basicauth.Use(&ba)
```




## 6. Invoker客户端请求服务端

package: "github.com/go-chassis/go-chassis/core"

在客户端通过`core.NewRestInvoker`/ `core.NewRPCInvoker` 发送请求来与服务侧通信. 参见[Invoker使用说明](https://docs.go-chassis.com/user-guides/invoker.html)

## log日志

### log日志配置

用户可配置微服务的运行日志的相关属性，比如输出方式，日志级别，文件路径以及日志转储相关属性。日志配置文件为`lager.yaml`。 [参见日志文件配置说明](https://docs.go-chassis.com/user-guides/log.html)

## 7. 动态配置框架 Archaius


Archaius为动态配置框架，可从各种不同的source中读取配置

### 使用

[官方样例](https://github.com/go-chassis/go-archaius/tree/master/examples)

以配置文件event.yaml 为例

```yaml
name: Tom Smith
age: 1
#spouse
#   name: Jane Smith
#   age: 23
children:
   name: Jimmy Smith
   age: 19
```

编写一个程序
```go
package main
  
import (
        "fmt"
         "time"
        "github.com/go-chassis/go-archaius"
        "github.com/go-chassis/go-archaius/event"
        "github.com/go-mesh/openlogging"
)

//Listener is a struct used for Event listener
type Listener struct {
        Key string
}

//Event is a method for QPS event listening
func (e *Listener) Event(event *event.Event) {
        openlogging.GetLogger().Info(event.Key)
        openlogging.GetLogger().Infof(fmt.Sprintf("%v", event.Value))
        openlogging.GetLogger().Info(event.EventType)
}

func main() {
        err := archaius.Init(archaius.WithRequiredFiles([]string{
                "./event.yaml",
        }))
        if err != nil {
                openlogging.GetLogger().Error("Error:" + err.Error())
        }
        archaius.RegisterListener(&Listener{}, "age")
        for {
//              log.Println(archaius.Get("age"))
                time.Sleep(5 * time.Second)
        }
}
```
运行这个程序，修改age项，修改 children.age项，得到的是下面日志输出

```
...
2020/07/08 20:14:01 INFO: age
2020/07/08 20:14:01 INFO: 1
2020/07/08 20:14:01 INFO: UPDATE
2020/07/08 20:14:01 DEBUG: generated events []
2020/07/08 20:14:29 DEBUG: file event /home/atmel/temp/event.yaml, operation is 8. reload it.
2020/07/08 20:14:29 DEBUG: file renamed
2020/07/08 20:14:30 DEBUG: file event /home/atmel/temp/event.yaml, operation is 16. reload it.
2020/07/08 20:14:30 DEBUG: user default file handler
2020/07/08 20:14:30 DEBUG: generated events [0xc0000ba640]
2020/07/08 20:14:30 INFO: event received &{FileSource UPDATE children.age %!s(int=219)}
2020/07/08 20:14:30 INFO: event generated for age
2020/07/08 20:14:30 DEBUG: file event /home/atmel/temp/event.yaml, operation is 16. reload it.
2020/07/08 20:14:30 DEBUG: user default file handler
2020/07/08 20:14:30 DEBUG: generated events []
2020/07/08 20:14:30 INFO: children.age
2020/07/08 20:14:30 INFO: 219
2020/07/08 20:14:30 INFO: UPDATE
...
```

### 自定义配置

[官方文档开发](https://docs.go-chassis.com/dev-guides/archaius.html)

“go-archaius\source\file\file.go”文件是实现文件配置的实现，可以参考它来实现自己的配置


## 自定义启动代码bootstrap

bootstrap启动插件是单独提供出来的机制，在goChassis初始化时先执行，它的执行顺序还早于handlers的加载。

参考`go-chassis\bootstrap\bootstrap.go`可以看到如何使用

下面是一个如何使用的业务代码举例
```go
import "github.com/go-chassis/go-chassis/bootstrap"
type demoPlugin1 struct {	
}
func (x *demoPlugin) Init() error {
    定制代码
}
demoPlugin2 := bootstrap.Func(func() error {
	定制代码
})


bootstrap.InstallPlugin("plugin-name1", demoPlugin1)
bootstrap.InstallPlugin("plugin-name2", demoPlugin2)
```

## 8. loadbalance

### Filter

#### 配置

系统默认只实现了Available Zone Filter， 它的配置见[官方文档](https://go-chassis.readthedocs.io/en/latest/user-guides/filter.html)

#### 自定义Filter

假如只有配置中心配置, go-chassis 框架加载Filter的逻辑位于`loadbalancer.BuildStrategy`，简单来说就是： 轮询配置“chassis.yaml中的serverListFilters”名单，从`loadbalancer.Filters`中获得对应的filter-func，并执行。

系统默认提供了一个Filter，就是`loadbalancing.FilterAvailableZoneAffinity`, 自己实现自定义filter可以参照它来实现。

```go
package loadbalancer
// Filter receive instances and criteria, it will filter instances based on criteria you defined,criteria is optional, you can give nil for it
type Filter func(instances []*registry.MicroServiceInstance, criteria []*Criteria) []*registry.MicroServiceInstance

// 它是由InstallFilter进行填充
// Filters is a map of string and array of *registry.MicroServiceInstance
var Filters = make(map[string]Filter)

// InstallFilter install filter
func InstallFilter(name string, f Filter) {
	Filters[name] = f
}
```
要实现自己的Filter，需要有以下动作：

- 在业务代码中，实现filter 函数，该函数满足`loadbalancer.Filter`接口定义
- 在业务代码中，通过`loadbalancer.InstallFilter(filter-name string, f Filter)` 注册该自定义filter
- 在配置中声明启用该自定义filter，这里的filter-name需要与上一步的filter-name保持一致

	```yaml
	cse:
	loadbalance:
			serverListFilters: filter_name
	```


#  源码解读

以“go-chassis\examples\rest\server\main.go”例子为例作为入口，即以下面的代码作为入口来解读go-chassis框架

```go
package main

import (
	"github.com/go-chassis/go-chassis"
	"github.com/go-chassis/go-chassis/core/server"
	"github.com/go-chassis/go-chassis/examples/schemas"
	"github.com/go-mesh/openlogging"
)

//if you use go run main.go instead of binary run, plz export CHASSIS_HOME=/{path}/{to}/rest/server/

func main() {
	chassis.RegisterSchema("rest", &schemas.RestFulHello{}, server.WithSchemaID("RestHelloService"))
	if err := chassis.Init(); err != nil {
		openlogging.Fatal("Init failed." + err.Error())
		return
	}
	chassis.Run()
}
```

运行这个示例需要以下预先动作

- 首先运行起`sc`(service center)： `go-chassis\scripts\ci\start_latest_sc.sh` . 如果sc运行在本机，那官方的rest示例不需要修改，否则对应修改下`go-chassis\examples\rest\{server,client}\conf\chassis.yaml`中registry地址配置




## 1. 入口

```go
package main

import (
    //step1： 加载chassis package
	"github.com/go-chassis/go-chassis"
    "github.com/go-chassis/go-chassis/core/server"
    //本次示例需要用到的schemas.RestFulHello
	"github.com/go-chassis/go-chassis/examples/schemas"
	"github.com/go-mesh/openlogging"
)

//if you use go run main.go instead of binary run, plz export CHASSIS_HOME=/{path}/{to}/rest/server/

func main() {
    // step2，注册rest协议处理器，放入goChassis.schemas
    chassis.RegisterSchema("rest", &schemas.RestFulHello{}, server.WithSchemaID("RestHelloService"))    
    // step3，Init方法初始化默认的handler chain, 执行config.Init(), server.Init()...
    // server.Init()中会将goChassis.schemas各个元素包装为实现ProtocolServer接口的service，存放在goChassis.services
	if err := chassis.Init(); err != nil {
		openlogging.Fatal("Init failed." + err.Error())
		return
	}
	chassis.Run()
}
```

## 1.1 `chassis.RegisterSchema`解读

略
它


## 1.2 `chassis.Init()`解读

```go
//Init prepare the chassis framework runtime
func Init() error {
    // 加载默认DefaultConsumerChainNames
	if goChassis.DefaultConsumerChainNames == nil {
		defaultChain := strings.Join([]string{
			handler.Router,
			handler.Loadbalance,
			handler.TracingConsumer,
			handler.Transport,
		}, ",")
		goChassis.DefaultConsumerChainNames = map[string]string{
			common.DefaultKey: defaultChain,
		}
    }
    // 加载默认DefaultProviderChainNames
	if goChassis.DefaultProviderChainNames == nil {
		defaultChain := strings.Join([]string{
			handler.TracingProvider,
		}, ",")
		goChassis.DefaultProviderChainNames = map[string]string{
			common.DefaultKey: defaultChain,
		}
	}
    goChassis.hajackGracefulShutdown = GracefulShutdown
    // 执行初始化，这个是重点逻辑，
	if err := goChassis.initialize(); err != nil {
		log.Println("init chassis fail:", err)
		return err
	}
	openlogging.GetLogger().Infof("init chassis success, version is %s", metadata.SdkVersion)
	return nil
}
```

### 1.2.1 `goChassis.initHandler`

调用链关系是`chassis.Init() -> goChassis.initialize() -> goChassis.initHandler()`

从配置文件`chassis.yaml`中读取handler部分的配置内容进行初始化

```go
func (c *chassis) initHandler() error {
	if err := c.initChains(common.Provider); err != nil {
		openlogging.GetLogger().Errorf("chain int failed: %s", err)
		return err
	}
	if err := c.initChains(common.Consumer); err != nil {
		openlogging.GetLogger().Errorf("chain int failed: %s", err)
		return err
	}
	openlogging.Info("chain init success")
	return nil
}

```



### 1.2.2 `config.Init()`

调用链关系是`chassis.Init() -> goChassis.initialize() -> config.Init()`，该方法位于“go-chassis\core\config\config.go”

涉及几个环境变量，这几个环境变量分别干什么用，见“go-chassis\docs\user-guides\env.md”说明


```go
// Init is initialize the configuration directory, archaius, route rule, and schema
func Init() error {
	err := InitArchaius()
	if err != nil {
		return err
	}
    // 如果设置了SCHEMA_ROOT env，则从该处加载schema，否则从CHASSIS_CONF_DIR env
    // 处，如果它也没有设置，则最后从${CHASSIS_HOME}/conf 处加载schema
	//Upload schemas using environment variable SCHEMA_ROOT
	schemaPath := archaius.GetString(common.EnvSchemaRoot, "")
	if schemaPath == "" {
		schemaPath = fileutil.GetConfDir()
	}

	schemaError := schema.LoadSchema(schemaPath)
	if schemaError != nil {
		return schemaError
	}

	//set micro service names
	err = schema.SetMicroServiceNames(schemaPath)
	if err != nil {
		return err
	}

	runtime.NodeIP = archaius.GetString(common.EnvNodeIP, "")

	err = readFromArchaius()
	if err != nil {
		return err
	}

	runtime.ServiceName = MicroserviceDefinition.ServiceDescription.Name
	runtime.Version = MicroserviceDefinition.ServiceDescription.Version
	runtime.Environment = MicroserviceDefinition.ServiceDescription.Environment
	runtime.MD = MicroserviceDefinition.ServiceDescription.Properties
	runtime.App = MicroserviceDefinition.AppID
	if runtime.App == "" {
		runtime.App = common.DefaultApp
	}

	runtime.HostName = MicroserviceDefinition.ServiceDescription.Hostname
	if runtime.HostName == "" {
		runtime.HostName, err = os.Hostname()
		if err != nil {
			openlogging.Error("Get hostname failed:" + err.Error())
			return err
		}
	} else if runtime.HostName == common.PlaceholderInternalIP {
		runtime.HostName = iputil.GetLocalIP()
	}
	openlogging.Info("Host name is " + runtime.HostName)
	return err
}
```

### 1.2.3 `bootstrap.Bootstrap()`

调用链关系是`chassis.Init() -> goChassis.initialize() -> bootstrap.Bootstrap()`



## 1.3 `chassis.Run()`

从此处就进入event loop

```go
//Run bring up the service,it waits for os signal,and shutdown gracefully
//before all protocol server start successfully, it may return error.
func Run() error {
    // 
	err := goChassis.start()
	if err != nil {
		openlogging.Error("run chassis failed:" + err.Error())
		return err
	}
	if !config.GetRegistratorDisable() {
		//Register instance after Server started
		if err := registry.DoRegister(); err != nil {
			openlogging.Error("register instance failed:" + err.Error())
			return err
		}
	}

	waitingSignal()
	return nil
}
```


C:\home\go-chassis\server\restful\router.go
//Router is to define how route the request
type Router interface {
	//URLPatterns returns route
	URLPatterns() []Route
}
