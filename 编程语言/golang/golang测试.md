

[Testing in Go](https://rollout.io/blog/testing-in-go/)

[GoMock vs. Testify: Mocking frameworks for Go](https://blog.codecentric.de/2019/07/gomock-vs-testify/)

- [golang/mock](https://github.com/golang/mock)
- [testify/mock](https://github.com/stretchr/testify)

##  golang mock

gomock主要包含两个部分：" gomock库"和“ 辅助代码生成工具mockgen”, 入口可参考[Testing with GoMock: A Tutorial](https://blog.codecentric.de/en/2017/08/gomock-tutorial/)

下面是一个在项目中使用mock的例子

- step 1: 安装包

```sh
go get github.com/golang/mock/gomock
go get github.com/golang/mock/mockgen
```

- 生成mock文件
```sh
~/temp/goscrapy/crawler/download$mkdir -p mocks
~/temp/goscrapy/crawler/download$mockgen -destination=../download/mocks/mock_IMiddleWare.go   -package=mocks -source=download_interf.go
```
- 使用

下面这个例子中
```go
func TestMWManager_ProcessChain(t *testing.T) {

	mockCtrl := gomock.NewController(t)
	defer mockCtrl.Finish()

	reqord, respord := make([]int, 0), make([]int, 0)

	dummyResp := &util.SpiderResponse{}
	mockMW1 := mocks.NewMockIMiddleWare(mockCtrl)
	mockMW1.EXPECT().ProcessRequest(gomock.Any()).Return(nil, nil, nil).
		Do(
			func(req *util.SpiderRequest) {
				fmt.Println("call 1st dl middleware ProcessRequest")
				reqord = append(reqord, 1)
            })
    后面就把mockMW1当作IMiddleWare 类使用
```


## testify


## vscode 记录

### 测试用例支持标准输出

在编写golang测试用例中，如果有stdout输出的语句，正常情况下是不会有输出的，需要增加`-v(verbose)选项`才能得到完整输出。 对应命令行类似：

`go test -timeout 30s -v github.com/flyingyizi/goscrapy/crawler/download -run ^TestMWManager_ProcessRequestChain$`

在vscode中通过`"go.testFlags": ["-v"],`配置来设置。 对应GUI 入口是找到下面的位置，然后根据提示去编辑settings.json文件

```text
Go: Test Flags
Flags to pass to `go test`. If null, then buildFlags will be used.
```
