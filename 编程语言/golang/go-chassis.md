
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

#     
