
## apt 管理命令

重点命令： dpkg -L 查看安装包安装到哪里了

|||
|--|--|
|apt-cache search           |--(package 搜索包) |
|apt-cache show             |-(package 获取包的相关信息，如说明、大小、版本等) |
|apt-get install            |--(package 安装包) |
|apt-get install            |-(package --reinstall 重新安装包) |
|apt-get -f install         |-(强制安装, "-f = --fix-missing"当是修复安装吧...) |
|apt-get remove             |(package 删除包) |
|apt-get remove --purge     |--(package 删除包，包括删除配置文件等) |
|apt-get autoremove --purge |(package 删除包及其依赖的软件包+配置文件等（只对6.10有效，强烈推荐）) |
|apt-get update             |-更新源 |
|apt-get upgrade            |-更新已安装的包 |
|apt-get dist-upgrade       |-----升级系统 |
|apt-get dselect-upgrade    |-使用 dselect 升级 |
|apt-cache depends          |--(package 了解使用依赖) |
|apt-cache rdepends         |--(package 了解某个具体的依赖,当是查看该包被哪些包依赖吧...) |
|apt-get build-dep          |--(package 安装相关的编译环境) |
|apt-get source             |-(package 下载该包的源代码) |
|apt-get clean && apt-get autoclean |-清理下载文件的存档 && 只清理过时的包 |
|apt-get check              |-检查是否有损坏的依赖 |
|dpkg -S filename           |查找filename属于哪个软件包 |
|apt-file search filename   |-查找filename属于哪个软件包 |
|apt-file list packagename  |-列出软件包的内容 |
|apt-file update            |-更新apt-file的数据库 |
|pkg --info "软件包名"       |--列出软件包解包后的包名称.|
|dpkg -l                    |--列出当前系统中所有的包.可以和参数less一起使用在分屏查看. (类似于rpm -qa)|
|dpkg -l \|grep -i "软件包名" |--查看系统中与"软件包名"相关联的包.|
|dpkg -s                    |查询已安装的包的详细信息.|
|dpkg -L                    |查询系统中已安装的软件包所安装的位置. (类似于rpm -ql)|
|dpkg -S                    |查询系统中某个文件属于哪个软件包. (类似于rpm -qf)|
|dpkg -I                    |查询deb包的详细信息,在一个软件包下载到本地之后看看用不用安装(看一下呗).|
|dpkg -i                    |手动安装软件包(这个命令并不能解决软件包之前的依赖性问题),如果在安装某一个软件包的时候遇到了软件依赖的问题,可以用apt-get -f install在解决信赖性这个问题.|
|dpkg -r                    |卸载软件包.不是完全的卸载,它的配置文件还存在.|
|dpkg -P                    |全部卸载(但是还是不能解决软件包的依赖性的问题)|
|dpkg -reconfigure          | 重新配置|

