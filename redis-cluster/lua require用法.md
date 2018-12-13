

### 2
- golang 绑lua  https://github.com/aarzilli/golua

- redis与luajit配合 https://github.com/qiye/redis-storage

## lua 函数
- 对string划界，除了匹配引号之外，我们可以像处理长注释一样，匹配两个“[[”“]]”。使用这种划界方式的string通常会有几行，如果第一个字符是“换行符”，则会被忽略掉，并且不会解释转义字符（如下示例），保持原样输出。通常在string中包含代码片段的时候使用比较方便。  有个疑问，如果我的string中含有“[[”或者“]]”，怎么办。Lua提供了一个更方便的解决办法，就是使用类似[====[这样的匹配符号，“=”个数任意，但是保证前后相同，如果“=”个数不同，那么就会被忽略掉。
- 如果lua函数只有一个参数，并且此参数是一个字符串或table构造式，那"()"是可有可无的
```lua
print "hello"
dofile 'a.lua'
print [[a multi-line 
   message]]
f{x=10, y=20}   
type{}
```



## 15.1 require函数
Lua提供了一个名为require的高层函数用来加载模块，但这个函数只假设了关于模块的基本概念。对于requtre而言，一个模块就是一段定义了一些值的代码。

要加载一个模块，只需简单地调用require"<模块名>"。该调用会返回一个由模块函数组成的table,并且还会定义一个包含该table的全局变量．然而，这些行为都是由模块完成的，而非require,所以，有些模块会选择返回其他值，或者具有其他的效果。

即使知道某些用到的模块可能己加载了，但只要用到require就是一个良好的编程习惯、可以将标准库排除在此规则之外，因为Lua总是会预先加线它们。不过，有些用户还是喜欢为标准库中的模块使用显式的require:
```lua
local m= require "io"
m.write("helloworld\n")
```

以下代码详细说明了require的行为：
```lua
function require(name)
 if not package.loaded(name) then
   local loader = findloader(name)
   if  loader == nil then
      error("unable to loadr module" .. name)
    end
    package.loaded[name] =true    -- 将模块标记为已经加载
    local res = loader(name)
    if res~= nil then
        package.loaded[name] = res
    end
 end
 return package.load(name)
end
```

首先，它在table package.loaded中检查模块是否己加载。如果是的话，require就返回相应的值. 因此，只要一个模块已加载过，后续的requure调用都将返回同一个值，不会再次加载它。

如果模块尚未加载，require就试着为该模块找一个加载器(loader),会先在table package.preload中查询传入的模块名如果在其中找到了一个函数，就以该函数作为模块的加载器。通过这个preload table，就有了一种通用的方法来处理各种不同的情况.通常这个table中不会找到有关指定模块的条目，那么requure就会尝试从Lua文件或C程序厍中加载模块。

如果require为指定模块找到了一个Lua文件，它就通过loadfile来加载该文件。而如果找到的是一个C程序厍，就通过loadlib来加载。注意，loadfile和loadhb都只是加载了代码，并没有运行它们。为了运行代码，require会以模块名作为参数来调用这些代码。如果加载器有返回值，requvre就将这个返回值存储到tablepackageloaded中．以此作为将来对同一模块调用的返回值。如果加载器没有返回值，require就会返回package.loaded中的值。在本章后面会看到，一个模块还可以将返回给require的值直接放入package.loaded中。

上述代码中还有一个重要的细节，就是在调用加载器前，require先将true赋予了package.loaded中的对应字段，以此将模块标记为己加载．这是因为如果一个模块要求加载另一个模块，而后者又要递归地加载前者。那么后者的require调用就会马上返回，从而避免了无限循环。
若要强制使require对同一个厍加载两次的话，可以简单地胤除package-loaded中的模块条目. 例如，在成功地require "foo"后，package.loaded["foo"]不为nil了。下面代码就可以再次加载该模块：
```lua
package.loaded["foo"] = nil
require "foo"
```
在搜索一个文件时，require所使用的路径与传统的路径有所不同“大部分程序所使用的路径就是一连串目录，指定了某个文件的具体位置。然而，ANSIc却没有任何关于目录的概念。所以，require采用的路径是一连串的模式(pattern)，其中每项都是一种将模块名转换为文件名的方式。进一步说，这种路径中的每项都是一个文件名，每项中还可以包含一个可选的
问号“require会用模块名来替换每个“？”，然后根据替换的结果来检查是否存在这样一个文件。如果不存在，就会尝试下一项。路径中的每项以分号隔开。例如，假设路径为：
```text
?;?.lua;c:\windows\?;/usr/local/lua/?/?.lua
```

那么，调用require"sql"就会试着打开以下文件：
```text
sql
sql.lua
c:\windows\sql
/usr/local/lua/sql/sql.lua
```
require函数只处理了分号（作为各项之间的分隔符）和问号。其他例如目录分隔符或文件扩展名，都由路径自己定义，
require用于搜索Lua文件的路径存放在变量package.path中。 例如：
```text
> =package.path
.\?.lua;C:\prog\openresty-1.13.6.1-win32\lualib\?.lua;C:\prog\openresty-1.13.6.1-win32\lua\?.lua;C:\prog\openresty-1.13.6.1-win32\lua\?\init.lua;
>
```

当Lua启动后，便以环境变量LUA_PATH的值来初始化这个变量。如果没有找到该环境变量，则使用一个编译时定义的
默认路径来初始化。在使用LUA_PATH时，Lua会将其中所有的子串“;;”替换成默认路径。例如，假设LUA_PATH为"mydir/?.lua;;",那么最终路径就是"mydir/?.lua"，并紧随默认路径．

如果require无法找到与模块名相符的Lua文件，它就会找c程序库。这类搜索会从变量package.cpath（相对于package.path)获取路径。而这个变量则是通过环境变量LUA_CPATH (相对于LUA_PATH)来初始化。在UNIX中，它的值一般是这样的:
```text
./?.so;/usr/local/lib/lua/5.1/?.so
```

注意，文件的扩展名是由路径定义的（例如，上例中使用的.so .而在Windows中，此路径通常可以是这样的：
```text
./?.dll;c:\program files\lua501\dll\?.dll
```

当找到一个C程序库后，require就会通过package.loadlib来加载它，loadllb在8.2节中己讨论过。c程序库与Lua程序块是不同的，它没有定义一个单一的主函数，而是导出了几个c函数。具有良好行为的C程序库应该导出一个名为“luaopen_<模块名>”的函数。require会在链接完程序厍后，尝试调用这个函数。将在26．2节中讨论如何编写c程序库。

一般通过模块的名称来使用它们。但有时必须将一个模块改名，以避免名称冲突。一种典型的情况是，在测试中需要加载同一模块的不同版本。对于一个Lua模块来说，其内部名
称不是固定的，可以轻易地编辑它以改变其名称。但是却无法编辑一个二进制数据模块中luaopen_*函数的名称。为了允许这种重命名，require用到了一个小技巧：如果一个模块名中包含了连字符，require就会用连字符后的内容来创建luaopen_*函数名。例如，若一个模块名为a-b，require就认为它的open函数名为luaopen_b，而不是luaopen_a-b. 因此，如果要使
用的两个模块名都为mod，那么可以将其中一个重命名为v1-mod（或者-mod，或其他类似形式),当调用mu1 = require "v1-mod时，require会找到改名后的文件v1-mod，并将其中的函数luaopen_mod作为open函数。

```lua
    print("--------------[package.loaded]--------------")  
    for n in pairs(package.loaded) do  
        print(n)  
    end        
    print("--------------[package.preload]--------------")  
    for n in pairs(package.preload) do  
        print(n)  
    end  
    print("--------------[package.path]--------------")  
    print(package.path)  
    print("--------------[package.cpath]--------------")  
    print(package.cpath)  
```

## 15.2 编写模块的基本方法
在Lua中创建一个模块最简单的方法是：创建一个table, 并将所有需要导出的函数放入其中，最后返回这个table.以下代码演示这种方法。注意，将inv声明为程序块的局部变量，就是将其定义成一个私有的名称。

