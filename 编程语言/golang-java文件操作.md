按照unix哲学，万物皆文件。 这也体现了文件类在编程中的重要作用。

本文以[Working with Files in Go](https://www.devdungeon.com/content/working-files-go)为基础，增加java实现。

收集文件类相关code snippet。

下面的java代码优先使用nio 方法。涉及以下package

```java

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import static java.io.File.separator;

import java.nio.ByteBuffer;
import java.nio.channels.FileChannel;
import java.nio.charset.Charset;
import java.nio.charset.StandardCharsets;
import java.nio.file.*;
import java.nio.file.attribute.PosixFilePermission;
import java.nio.file.attribute.PosixFilePermissions;
import java.nio.file.attribute.UserPrincipal;
import java.nio.file.attribute.UserPrincipalLookupService;
import java.nio.file.attribute.BasicFileAttributeView;
import java.nio.file.attribute.BasicFileAttributes;
import java.nio.file.attribute.FileAttribute;
import java.nio.file.attribute.FileTime;
import java.nio.file.attribute.GroupPrincipal;
import java.nio.file.attribute.PosixFileAttributeView;

import static java.nio.file.StandardCopyOption.*;
import static java.nio.file.StandardOpenOption.*;

import java.util.LinkedList;
import java.util.List;
import java.util.Set;
import java.util.regex.Pattern;
import java.util.zip.GZIPInputStream;
import java.util.zip.GZIPOutputStream;

import java.util.Scanner;
```

# 2基本操作

## 2.1 打开、创建文件

```go
//CreateFile 如果存在则可读写打开，如果不存在则创建
func (my *MyFile) CreateFile(path string) (f *os.File) {
	//判断文件是否存在
	_, err := os.Stat(path)
	if err != nil {
		if os.IsExist(err) {
			f, err = os.OpenFile(path, os.O_RDWR|os.O_CREATE, 0)
		} else {
			f, err = os.Create(path)
		}

		if err != nil {
			f = nil
		}
	}
	return
}
```

```java
public class MyFile {
    //CreateFile 如果存在则方法path，否则创建并返回path
    Path CreateFile(String pathstr) throws IOException {
        final Path path = Paths.get(pathstr);

        if (!Files.exists(path, LinkOption.NOFOLLOW_LINKS)) {
            /**
             * windows不支持
            */
            Set<PosixFilePermission> perms = PosixFilePermissions.fromString("rw-rw-r--");
            FileAttribute<Set<PosixFilePermission>> attrs = PosixFilePermissions.asFileAttribute(perms);
            Files.createFile(path/*,attrs*/);
        }
        return path;
    }
}	
```

## 2.2 Truncate文件

```go
func Truncate(newSize int)() {
    err := os.Truncate("test.txt", newSize)
    if err != nil {
        log.Fatal(err)
    }
}
```

```java
public class MyFile {
    public void Truncate(Path path,int newSize) {
        //使用jdk7 try with resource语法，等价golang  defer
        try (FileChannel outChan = new FileOutputStream(path.toFile(), true).getChannel()) {
            outChan.truncate(newSize);
            //上面标识append为true的作用体现，如果截断后需要继续写就需要采用这种方式
            //outChan.write(src)
          }catch (Exception e) {
            e.printStackTrace();
        }
    }
}
```

## 2.3 得到文件信息

```go
func GetFileInfo(path string) {
    // 如果文件不存在，则返回错误
    fileInfo, err := os.Stat(path)
    if err != nil {
        log.Fatal(err)
    }
    fmt.Println("File name:", fileInfo.Name())
    fmt.Println("Size in bytes:", fileInfo.Size())
    fmt.Println("Permissions:", fileInfo.Mode())
    fmt.Println("Last modified:", fileInfo.ModTime())
    fmt.Println("Is Directory: ", fileInfo.IsDir())
    fmt.Printf("System interface type: %T\n", fileInfo.Sys())
    fmt.Printf("System info: %+v\n\n", fileInfo.Sys())
}
```

```java
    void GetFileInfo(final Path path) throws IOException {
        System.out.println("File name:" + path.getFileName());
        System.out.println("Size in bytes:" + Files.size(path));
        System.out.println("Last modified:" + Files.getLastModifiedTime(path));
        System.out.println("Is Directory: " + Files.isDirectory(path));

        Set<PosixFilePermission>  p = Files.getPosixFilePermissions(path, LinkOption.NOFOLLOW_LINKS);
        System.out.print("Permissions: ");
        for (PosixFilePermission permission : p) {      
            System.out.print(permission.name() + " ");      
        }
    }
```

## 2.4 重命名和移动

```go
func (my *MyFile) Move(source, target string) {
	err := os.Rename(source, target)
	if err != nil {
		log.Fatal(err)
	}
}
```

```java
    void Move(final Path source, final Path target) throws IOException {
        Files.move(source, target, REPLACE_EXISTING,COPY_ATTRIBUTES);
    }
```

## 2.5 删除文件

```go
func (my *MyFile)  Remove(path string) {
    err := os.Remove(path)
    if err != nil {
        log.Fatal(err)
    }
}
```

```java
        Files.deleteIfExists(path)
        Files.delete(path);
```

## 2.7 检查文件是否存在

见2.1

## 2.8 检查读写权限

```go
func (my *MyFile) TestRW(path string)  {
    // 这个例子测试写权限，如果没有写权限则返回error。
    // 注意文件不存在也会返回error，需要检查error的信息来获取到底是哪个错误导致。
    file, err := os.OpenFile(path, os.O_WRONLY, 0666)
    if err != nil {
        if os.IsPermission(err) {
            log.Println("Error: Write permission denied.")
        }
    }
    file.Close()
    // 测试读权限
    file, err = os.OpenFile("test.txt", os.O_RDONLY, 0666)
    if err != nil {
        if os.IsPermission(err) {
            log.Println("Error: Read permission denied.")
        }
    }
    file.Close()
}
```

```java
    void TestRW(Path path) throws IOException {
        Set<PosixFilePermission>  p = Files.getPosixFilePermissions(path, LinkOption.NOFOLLOW_LINKS);
        
        if (!p.contains(PosixFilePermission.OWNER_READ)) {
            System.out.println("Error: Read permission denied.");
        }
        if( !p.contains(PosixFilePermission.OWNER_WRITE)) {
            System.out.println("Error: Write permission denied.");
        }
    }
```

## 2.9 改变权限、拥有者、时间戳

```go
func main() {
    // 使用Linux风格改变文件权限
    err := os.Chmod("test.txt", 0777)
    if err != nil {
        log.Println(err)
    }
    // 改变文件所有者
    err = os.Chown("test.txt", os.Getuid(), os.Getgid())
    if err != nil {
        log.Println(err)
    }
    // 改变时间戳
    twoDaysFromNow := time.Now().Add(48 * time.Hour)
    lastAccessTime := twoDaysFromNow
    lastModifyTime := twoDaysFromNow
    err = os.Chtimes("test.txt", lastAccessTime, lastModifyTime)
    if err != nil {
        log.Println(err)
    }
}
```

```java
    void ModifyFileAttr(Path path) throws IOException {
        // 使用linux风格改变文件权限
        Set<PosixFilePermission> perms = PosixFilePermissions.fromString("rwxrwxrwx");
        Files.setPosixFilePermissions(path, perms);
        // 也可以采用下面每个单独设置方式
        // // using PosixFilePermission to set file permissions 777
        // Set<PosixFilePermission> perms = new HashSet<PosixFilePermission>();
        // // add owners permission
        // perms.add(PosixFilePermission.OWNER_READ);
        // perms.add(PosixFilePermission.OWNER_WRITE);
        // perms.add(PosixFilePermission.OWNER_EXECUTE);
        // // add group permissions
        // perms.add(PosixFilePermission.GROUP_READ);
        // perms.add(PosixFilePermission.GROUP_WRITE);
        // perms.add(PosixFilePermission.GROUP_EXECUTE);
        // // add others permissions
        // perms.add(PosixFilePermission.OTHERS_READ);
        // perms.add(PosixFilePermission.OTHERS_WRITE);
        // perms.add(PosixFilePermission.OTHERS_EXECUTE);
        // Files.setPosixFilePermissions(path, perms);

        // 改变文件所有者与某个orig文件相同
        PosixFileAttributeView xx = Files.getFileAttributeView(path, PosixFileAttributeView.class,
                LinkOption.NOFOLLOW_LINKS);
        // 另外一种方式;
        UserPrincipalLookupService lookupService = FileSystems.getDefault().getUserPrincipalLookupService();
        GroupPrincipal group = lookupService.lookupPrincipalByGroupName("group-name");
        UserPrincipal  owner = lookupService.lookupPrincipalByName("owner-name");
        // //另外一种方式，从某个特定文件获取得到group、owner
        // Path orig = Paths.get("orig");// just for example
        // PosixFileAttributes attr = Files.readAttributes(path, PosixFileAttributes.class, LinkOption.NOFOLLOW_LINKS);
        // GroupPrincipal group = attr.group();
        // UserPrincipal owner = attr.owner();
        xx.setGroup(group);
        xx.setOwner(owner);

        // 改变时间戳
        FileTime now = FileTime.fromMillis(System.currentTimeMillis());
        // Files.setLastModifiedTime(path, now);
		//modify lastModifiedTime & lastAccessTime , dont modify createtime
        Files.getFileAttributeView(path, BasicFileAttributeView.class).setTimes(now, now, null);
    }
```

## 2.10 硬链接和软链接

```go
func main() {
    // 创建一个硬链接。
    // 创建后同一个文件内容会有两个文件名，改变一个文件的内容会影响另一个。
    // 删除和重命名不会影响另一个。
    err := os.Link("original.txt", "original_also.txt")
    if err != nil {
        log.Fatal(err)
    }
    fmt.Println("creating sym")
    // Create a symlink
    err = os.Symlink("original.txt", "original_sym.txt")
    if err != nil {
        log.Fatal(err)
    }
    // Lstat返回一个文件的信息，但是当文件是一个软链接时，它返回软链接的信息，而不是引用的文件的信息。
    // Symlink在Windows中不工作。
    fileInfo, err := os.Lstat("original_sym.txt")
    if err != nil {
        log.Fatal(err)
    }
    fmt.Printf("Link info: %+v", fileInfo)
    //改变软链接的拥有者不会影响原始文件。
    err = os.Lchown("original_sym.txt", os.Getuid(), os.Getgid())
    if err != nil {
        log.Fatal(err)
    }
}
```

```java
    void CreateLink(Path path) throws IOException {
        //创建一个硬链接path_also.txt
        Path normalPath = path.toAbsolutePath().normalize();
        System.out.println(normalPath);
        String link = normalPath.getParent().toString() + separator + path.getFileName().toString() + ".link";
        System.out.println(link);
        Files.createLink(Paths.get(link), path);
        
        //创建软连接
        String softLink = normalPath.getParent().toString() + separator + path.getFileName().toString() + ".softlink";
        //这里省略attrs的设置，前面的创建文件的样例已经演示过
        Files.createSymbolicLink(Paths.get(softLink), path);
    }
```



## 遍历目录

```go

import (
	"container/list"
	"fmt"
	"os"
	"path/filepath"
	"regexp"
)

func main() {

	my := NewMyFile()
	startPath, _ := filepath.Abs("..\\")

	fmt.Println("startpath is:" + startPath)
	my.WalkFileTree(startPath)
	fmt.Println(my.result)
}

type MyFile struct {
	result *list.List
}

//WalkFileTree 演示遍历目录
func (my *MyFile) WalkFileTree(startPath string) {

	filePath, err := filepath.Abs(startPath)

	err = filepath.Walk(filePath, my.findJavaVisitor)
	if err != nil {
		fmt.Printf("filepath.Walk() returned %v\n", err)
	}
	fmt.Printf("result.size()=%d", my.result.Len())
}

func NewMyFile() *MyFile {
	my := MyFile{}
	my.result = list.New()
	return &my
}

func (my *MyFile) findJavaVisitor(path string, f os.FileInfo, err error) error {

	if f == nil {
		return err
	}
	if f.IsDir() {
		return nil
	}

    //收集后缀为java、go的文件
	_, filename := filepath.Split(path)
	match, err := regexp.MatchString(".*\\.(java|go)", filename)

	if match == true {
		// os.Remove(path)
		my.result.PushBack(path)
	}

	return nil

}

```

```java
public class MyFile {
    void WalkFileTree(Path startPath) throws IOException {
        List<Path> result = new LinkedList<Path>();
        Files.walkFileTree(startPath, new FindJavaVisitor(result));
        System.out.println("result.size()=" + result.size());

        System.out.println("done.");
    }

    private static class FindJavaVisitor extends SimpleFileVisitor<Path> {
        private List<Path> result;

        public FindJavaVisitor(List<Path> result) {
            this.result = result;
        }

        @Override
        public FileVisitResult visitFile(Path path, BasicFileAttributes attrs) throws IOException {
            // return super.visitFile(file, attrs);
            String filePath = path.toFile().getAbsolutePath();
            if (filePath.matches(".*\\.(java|go)")) {
                try {
                    //do something for the file 
                    // Files.deleteIfExists(path);
                } catch (IOException e) {
                    e.printStackTrace();
                }
                result.add(path.getFileName());
            }
            return FileVisitResult.CONTINUE;
        }
    }

}
```



# 3 读写

## 3.1 复制文件

```go
func main() {
    // 打开原始文件
    originalFile, err := os.Open("test.txt")
    if err != nil {
        log.Fatal(err)
    }
    defer originalFile.Close()
    // 创建新的文件作为目标文件
    newFile, err := os.Create("test_copy.txt")
    if err != nil {
        log.Fatal(err)
    }
    defer newFile.Close()
    // 从源中复制字节到目标文件
    bytesWritten, err := io.Copy(newFile, originalFile)
    if err != nil {
        log.Fatal(err)
    }
    log.Printf("Copied %d bytes.", bytesWritten)
    // 将文件内容flush到硬盘中
    err = newFile.Sync()
    if err != nil {
        log.Fatal(err)
    }
}
```

```java
    void CopyFile(Path source, Path target) throws IOException {

        if (source.toFile().isDirectory()) {
            System.out.println("source is directory");
        } else if ( source.toFile().isFile() ) {
            System.out.println("source is file");
        }

        //注意这个方法仅仅copy file，如果是要拷贝目录需要使用Files.walkFileTree
        Files.copy(source, target, REPLACE_EXISTING);
    }
```

## 3.2 跳转到文件指定位置(Seek)

```go
func main() {
    file, _ := os.Open("test.txt")
    defer file.Close()
    // 偏离位置，可以是正数也可以是负数
    var offset int64 = 5
    // 用来计算offset的初始位置
    // 0 = 文件开始位置
    // 1 = 当前位置
    // 2 = 文件结尾处
    var whence int = 0
    newPosition, err := file.Seek(offset, whence)
    if err != nil {
        log.Fatal(err)
    }
    fmt.Println("Just moved to 5:", newPosition)
    // 从当前位置回退两个字节
    newPosition, err = file.Seek(-2, 1)
    if err != nil {
        log.Fatal(err)
    }
    fmt.Println("Just moved back two:", newPosition)
    // 使用下面的技巧得到当前的位置
    currentPosition, err := file.Seek(0, 1)
    fmt.Println("Current position:", currentPosition)
    // 转到文件开始处
    newPosition, err = file.Seek(0, 0)
    if err != nil {
        log.Fatal(err)
    }
    fmt.Println("Position after seeking 0,0:", newPosition)
}
```

```java
    void Seek(final Path path) throws IOException {

        FileChannel fc = FileChannel.open(path, READ, WRITE);
		GZIPInputStream gis = new GZIPInputStream(Channels.newInputStream(fc));

        // 得到当前位置
        long currentPosition = fileChannel.position();
        // 设当前位置为距离文件开头5
        fileChannel.position(5);

        //从当前位置开始读取30个字节
        ByteBuffer buffer = ByteBuffer.allocate(30);
        do {
            fileChannel.read(buffer);
        } while (buffer.hasRemaining());
    }
```

## 3.3 写文件

```go
func main() {
    // 可写方式打开文件
    file, err := os.OpenFile(
        "test.txt",
        os.O_WRONLY|os.O_TRUNC|os.O_CREATE,
        0666,
    )
    if err != nil {
        log.Fatal(err)
    }
    defer file.Close()
    // 写字节到文件中
    byteSlice := []byte("Bytes!\n")
    bytesWritten, err := file.Write(byteSlice)
    if err != nil {
        log.Fatal(err)
    }
    log.Printf("Wrote %d bytes.\n", bytesWritten)
}
```

## 3.4 快写文件
ioutil包有一个非常有用的方法WriteFile()可以处理创建／打开文件、写字节slice和关闭文件一系列的操作。如果你需要简洁快速地写字节slice到文件中，你可以使用它。

```go
func main() {
    err := ioutil.WriteFile("test.txt", []byte("Hi\n"), 0666)
    if err != nil {
        log.Fatal(err)
    }
}
```

```java
//java可以采用下面的类似方法
//    public static Path write(Path path, byte[] bytes, OpenOption... options)
```

## 3.5 使用缓存写

```go
func main() {
    // 打开文件，只写
    file, err := os.OpenFile("test.txt", os.O_WRONLY, 0666)
    if err != nil {
        log.Fatal(err)
    }
    defer file.Close()
    // 为这个文件创建buffered writer
    bufferedWriter := bufio.NewWriter(file)
    // 写字节到buffer
    bytesWritten, err := bufferedWriter.Write(
        []byte{65, 66, 67},
    )
    if err != nil {
        log.Fatal(err)
    }
    log.Printf("Bytes written: %d\n", bytesWritten)
    // 写字符串到buffer
    // 也可以使用 WriteRune() 和 WriteByte()   
    bytesWritten, err = bufferedWriter.WriteString(
        "Buffered string\n",
    )
    if err != nil {
        log.Fatal(err)
    }
    log.Printf("Bytes written: %d\n", bytesWritten)
    // 检查缓存中的字节数
    unflushedBufferSize := bufferedWriter.Buffered()
    log.Printf("Bytes buffered: %d\n", unflushedBufferSize)
    // 还有多少字节可用（未使用的缓存大小）
    bytesAvailable := bufferedWriter.Available()
    if err != nil {
        log.Fatal(err)
    }
    log.Printf("Available buffer: %d\n", bytesAvailable)
    // 写内存buffer到硬盘
    bufferedWriter.Flush()
    // 丢弃还没有flush的缓存的内容，清除错误并把它的输出传给参数中的writer
    // 当你想将缓存传给另外一个writer时有用
    bufferedWriter.Reset(bufferedWriter)
    bytesAvailable = bufferedWriter.Available()
    if err != nil {
        log.Fatal(err)
    }
    log.Printf("Available buffer: %d\n", bytesAvailable)
    // 重新设置缓存的大小。
    // 第一个参数是缓存应该输出到哪里，这个例子中我们使用相同的writer。
    // 如果我们设置的新的大小小于第一个参数writer的缓存大小， 比如10，我们不会得到一个10字节大小的缓存，
    // 而是writer的原始大小的缓存，默认是4096。
    // 它的功能主要还是为了扩容。
    bufferedWriter = bufio.NewWriterSize(
        bufferedWriter,
        8000,
    )
    // resize后检查缓存的大小
    bytesAvailable = bufferedWriter.Available()
    if err != nil {
        log.Fatal(err)
    }
    log.Printf("Available buffer: %d\n", bytesAvailable)
}
```

```java
    //注意这里的例子是字符文件，如果是二进制文件，需要使用类似Files.newInputStream

    // 更多有关NIO的例子见[Java NIO File Read Write with Channels](https://javapapers.com/java/java-nio-file-read-write-with-channels/)
    void WriteFileWithBuffer(Path path) throws IOException {
        Charset charset = Charset.forName("US-ASCII");
        Path filePath = FileSystems.getDefault().getPath(".", "tempCopy1.txt");
    
        BufferedWriter bufferedWriter = Files.newBufferedWriter(path,charset);
        String str="测试写入数据";
    
        System.out.println(str);
        bufferedWriter.write(str, 0, str.length());      
        bufferedWriter.flush();
        bufferedWriter.close();  
    }
```


## 3.6 读取最多N个字节

```go
func main() {
    // 打开文件，只读
    file, err := os.Open("test.txt")
    if err != nil {
        log.Fatal(err)
    }
    defer file.Close()
    // 从文件中读取len(b)字节的文件。
    // 返回0字节意味着读取到文件尾了
    // 读取到文件会返回io.EOF的error
    byteSlice := make([]byte, 16)
    bytesRead, err := file.Read(byteSlice)
    if err != nil {
        log.Fatal(err)
    }
    log.Printf("Number of bytes read: %d\n", bytesRead)
    log.Printf("Data read: %s\n", byteSlice)
}
```

```java
//使用FileChannel.read方法，示例见3.2 跳转到文件指定位置
```

## 3.7 读取正好N个字节

```go
func main() {
    // Open file for reading
    file, err := os.Open("test.txt")
    if err != nil {
        log.Fatal(err)
    }
    // file.Read()可以读取一个小文件到大的byte slice中，
    // 但是io.ReadFull()在文件的字节数小于byte slice字节数的时候会返回错误
    byteSlice := make([]byte, 2)
    numBytesRead, err := io.ReadFull(file, byteSlice)
    if err != nil {
        log.Fatal(err)
    }
    log.Printf("Number of bytes read: %d\n", numBytesRead)
    log.Printf("Data read: %s\n", byteSlice)
}
```

```java
//TODO
```

## 3.9 读取全部字节

```go
func main() {
    file, err := os.Open("test.txt")
    if err != nil {
        log.Fatal(err)
    }
    // os.File.Read(), io.ReadFull() 和
    // io.ReadAtLeast() 在读取之前都需要一个固定大小的byte slice。
    // 但ioutil.ReadAll()会读取reader(这个例子中是file)的每一个字节，然后把字节slice返回。
    data, err := ioutil.ReadAll(file)
    if err != nil {
        log.Fatal(err)
    }
    fmt.Printf("Data as hex: %x\n", data)
    fmt.Printf("Data as string: %s\n", data)
    fmt.Println("Number of bytes read:", len(data))
}
```

```java
//使用Files.readAllBytes(path)
```

## 3.11 使用缓存读

```go
func main() {
    // 打开文件，创建buffered reader
    file, err := os.Open("test.txt")
    if err != nil {
        log.Fatal(err)
    }
    bufferedReader := bufio.NewReader(file)
    // 得到字节，当前指针不变
    byteSlice := make([]byte, 5)
    byteSlice, err = bufferedReader.Peek(5)
    if err != nil {
        log.Fatal(err)
    }
    fmt.Printf("Peeked at 5 bytes: %s\n", byteSlice)
    // 读取，指针同时移动
    numBytesRead, err := bufferedReader.Read(byteSlice)
    if err != nil {
        log.Fatal(err)
    }
    fmt.Printf("Read %d bytes: %s\n", numBytesRead, byteSlice)
    // 读取一个字节, 如果读取不成功会返回Error
    myByte, err := bufferedReader.ReadByte()
    if err != nil {
        log.Fatal(err)
    }
    fmt.Printf("Read 1 byte: %c\n", myByte)     
    // 读取到分隔符，包含分隔符，返回byte slice
    dataBytes, err := bufferedReader.ReadBytes('\n')
    if err != nil {
        log.Fatal(err)
    }
    fmt.Printf("Read bytes: %s\n", dataBytes)           
    // 读取到分隔符，包含分隔符，返回字符串
    dataString, err := bufferedReader.ReadString('\n')
    if err != nil {
        log.Fatal(err)
    }
    fmt.Printf("Read string: %s\n", dataString)     
    //这个例子读取了很多行，所以test.txt应该包含多行文本才不至于出错
}
```

意这里的例子是字符文件，如果是二进制文件，需要使用类似Files.newOutputStream


```java
    void ReadFileWithBuffer(Path path) throws IOException {
        Charset charset = Charset.forName("US-ASCII");
        Path filePath = FileSystems.getDefault().getPath(".", "temp.txt");
    
        BufferedReader bufferedReader = Files.newBufferedReader(filePath,charset);
        String line = null;
        while ((line = bufferedReader.readLine()) != null) {
            System.out.println(line);
        }        
        bufferedReader.close();
    }
```



## 3.12 使用 scanner
在golang， Scanner是bufio包下的类型,在处理文件中以分隔符分隔的文本时很有用。通常我们使用换行符作为分隔符将文件内容分成多行。在CSV文件中，逗号一般作为分隔符。os.File文件可以被包装成bufio.Scanner，它就像一个缓存reader。我们会调用Scan()方法去读取下一个分隔符，使用Text()或者Bytes()获取读取的数据。

分隔符可以不是一个简单的字节或者字符，有一个特殊的方法可以实现分隔符的功能，以及将指针移动多少，返回什么数据。如果没有定制的SplitFunc提供，缺省的ScanLines会使用newline字符作为分隔符，其它的分隔函数还包括ScanRunes和ScanWords,皆在bufio包中。

下面的例子中，为一个文件创建了bufio.Scanner，并按照单词逐个读取：

```go
func main() {
    file, err := os.Open("test.txt")
    if err != nil {
        log.Fatal(err)
    }
    scanner := bufio.NewScanner(file)
    // 缺省的分隔函数是bufio.ScanLines,我们这里使用ScanWords。
    // 也可以定制一个SplitFunc类型的分隔函数
    scanner.Split(bufio.ScanWords)
    // scan下一个token.
    success := scanner.Scan()
    if success == false {
        // 出现错误或者EOF是返回Error
        err = scanner.Err()
        if err == nil {
            log.Println("Scan completed and reached EOF")
        } else {
            log.Fatal(err)
        }
    }
    // 得到数据，Bytes() 或者 Text()
    fmt.Println("First word found:", scanner.Text())
    // 再次调用scanner.Scan()发现下一个token
}
```

```java
    // 这个例子使用自定义的分隔符，比如使用来分隔“我  delimiter  看到   delimiter  你的书”
    void ScannerFile(Path path) throws IOException {

        Scanner sc = new Scanner(path,StandardCharsets.UTF_8);

        //如果不使用useDelimiter指定，Scanner默认使用的分隔符是空格、回车和tab
        //java的正则语法有些特殊，具体语法见[浅谈java正则表达式](https://blog.csdn.net/Denial_learn/article/details/100606078)
        sc.useDelimiter(Pattern.compile("\\s*delimiter\\s*|\\s*\n\\s*"));

        var result = new LinkedList<String>();

        while(sc.hasNext()) {
            result.add(sc.next());
        }
        System.out.println(result.toString());
        
        sc.close();
    }

```

# 4 压缩

## 4.1 打包(zip) 文件

```go
// This example uses zip but standard library
// also supports tar archives
func main() {
    // 创建一个打包文件
    outFile, err := os.Create("test.zip")
    if err != nil {
        log.Fatal(err)
    }
    defer outFile.Close()
    // 创建zip writer
    zipWriter := zip.NewWriter(outFile)
    // 往打包文件中写文件。
    // 这里我们使用硬编码的内容，你可以遍历一个文件夹，把文件夹下的文件以及它们的内容写入到这个打包文件中。
    var filesToArchive = []struct {
        Name, Body string
    } {
        {"test.txt", "String contents of file"},
        {"test2.txt", "\x61\x62\x63\n"},
    }
    // 下面将要打包的内容写入到打包文件中，依次写入。
    for _, file := range filesToArchive {
            fileWriter, err := zipWriter.Create(file.Name)
            if err != nil {
                    log.Fatal(err)
            }
            _, err = fileWriter.Write([]byte(file.Body))
            if err != nil {
                    log.Fatal(err)
            }
    }
    // 清理
    err = zipWriter.Close()
    if err != nil {
            log.Fatal(err)
    }
}
```

下面从网上找的工具类待check

```java
//使用 [ Zip File System Provider](https://docs.oracle.com/javase/7/docs/technotes/guides/io/fsp/zipfilesystemprovider.html)

import java.io.IOException;
import java.net.URI;
import java.nio.file.FileSystem;
import java.nio.file.FileSystems;
import java.nio.file.FileVisitResult;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.nio.file.SimpleFileVisitor;
import java.nio.file.StandardCopyOption;
import java.nio.file.attribute.BasicFileAttributes;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.HashMap;
import java.util.Map;

public class ZipUtil {

    /**
     * Returns a zip file system
     *
     * @param zipFilename to construct the file system from
     * @param create      true if the zip file should be created
     * @return a zip file system
     */
    private static FileSystem createZipFileSystem(String zipFilename, boolean create) throws IOException {
        // convert the filename to a URI
        final Path path = Paths.get(zipFilename);

        // defined in java.net.JarURLConnection
        final URI uri = URI.create("jar:file:" + path.toUri().getPath());

        final Map<String, String> env = new HashMap<>();
        if (create) {
            env.put("create", "true");
        }
        return FileSystems.newFileSystem(uri, env);
    }

    /**
     * Unzips the specified zip file to the specified destination directory.
     * Replaces any files in the destination, if they already exist.
     *
     * @param zipFilename the name of the zip file to extract
     * @param destDirname the directory to unzip to
     */
    public static void unzip(String zipFilename, String destDirname) throws IOException {

        final Path destDir = Paths.get(destDirname);
        // if the destination doesn't exist, create it
        if (Files.notExists(destDir)) {
            System.out.println(destDir + " does not exist. Creating...");
            Files.createDirectories(destDir);
        }

        try (FileSystem zipFileSystem = createZipFileSystem(zipFilename, false)) {
            final Path root = zipFileSystem.getPath("/");

            // walk the zip file tree and copy files to the destination
            Files.walkFileTree(root, new SimpleFileVisitor<Path>() {
                @Override
                public FileVisitResult visitFile(Path file, BasicFileAttributes attrs) throws IOException {
                    final Path destFile = Paths.get(destDir.toString(), file.toString());
                    System.out.printf("Extracting file %s to %s\n", file, destFile);
                    Files.copy(file, destFile, StandardCopyOption.REPLACE_EXISTING);
                    return FileVisitResult.CONTINUE;
                }

                @Override
                public FileVisitResult preVisitDirectory(Path dir, BasicFileAttributes attrs) throws IOException {
                    final Path dirToCreate = Paths.get(destDir.toString(), dir.toString());
                    if (Files.notExists(dirToCreate)) {
                        System.out.printf("Creating directory %s\n", dirToCreate);
                        Files.createDirectory(dirToCreate);
                    }
                    return FileVisitResult.CONTINUE;
                }
            });
        }
    }

    /**
     * Creates/updates a zip file.
     *
     * @param zipFilename the name of the zip to create
     * @param filenames   list of filename to add to the zip
     */
    public static void create(String zipFilename, String... filenames) throws IOException {

        try (FileSystem zipFileSystem = createZipFileSystem(zipFilename, true)) {
            final Path root = zipFileSystem.getPath("/");

            // iterate over the files we need to add
            for (String filename : filenames) {
                final Path src = Paths.get(filename);

                // add a file to the zip file system
                if (!Files.isDirectory(src)) {
                    final Path dest = zipFileSystem.getPath(root.toString(), src.toString());
                    final Path parent = dest.getParent();
                    if (Files.notExists(parent)) {
                        System.out.printf("Creating directory %s\n", parent);
                        Files.createDirectories(parent);
                    }
                    Files.copy(src, dest, StandardCopyOption.REPLACE_EXISTING);
                } else {
                    // for directories, walk the file tree
                    Files.walkFileTree(src, new SimpleFileVisitor<Path>() {
                        @Override
                        public FileVisitResult visitFile(Path file, BasicFileAttributes attrs) throws IOException {
                            final Path dest = zipFileSystem.getPath(root.toString(), file.toString());
                            Files.copy(file, dest, StandardCopyOption.REPLACE_EXISTING);
                            return FileVisitResult.CONTINUE;
                        }

                        @Override
                        public FileVisitResult preVisitDirectory(Path dir, BasicFileAttributes attrs)
                                throws IOException {
                            final Path dirToCreate = zipFileSystem.getPath(root.toString(), dir.toString());
                            if (Files.notExists(dirToCreate)) {
                                System.out.printf("Creating directory %s\n", dirToCreate);
                                Files.createDirectories(dirToCreate);
                            }
                            return FileVisitResult.CONTINUE;
                        }
                    });
                }
            }
        }
    }

    /**
     * List the contents of the specified zip file
     */
    public static void list(String zipFilename) throws IOException {

        System.out.printf("Listing Archive:  %s\n", zipFilename);

        // create the file system
        try (FileSystem zipFileSystem = createZipFileSystem(zipFilename, false)) {

            final Path root = zipFileSystem.getPath("/");

            // walk the file tree and print out the directory and filenames
            Files.walkFileTree(root, new SimpleFileVisitor<Path>() {
                @Override
                public FileVisitResult visitFile(Path file, BasicFileAttributes attrs) throws IOException {
                    print(file);
                    return FileVisitResult.CONTINUE;
                }

                @Override
                public FileVisitResult preVisitDirectory(Path dir, BasicFileAttributes attrs) throws IOException {
                    print(dir);
                    return FileVisitResult.CONTINUE;
                }

                /**
                 * prints out details about the specified path such as size and modification
                 * time
                 * 
                 * @param file
                 * @throws IOException
                 */
                private void print(Path file) throws IOException {
                    final DateFormat df = new SimpleDateFormat("MM/dd/yyyy-HH:mm:ss");
                    final String modTime = df.format(new Date(Files.getLastModifiedTime(file).toMillis()));
                    System.out.printf("%d  %s  %s\n", Files.size(file), modTime, file);
                }
            });
        }
    }
}

```

## 4.2 抽取(unzip) 文件

```go
func main() {
    zipReader, err := zip.OpenReader("test.zip")
    if err != nil {
        log.Fatal(err)
    }
    defer zipReader.Close()
    // 遍历打包文件中的每一文件/文件夹
    for _, file := range zipReader.Reader.File {
        // 打包文件中的文件就像普通的一个文件对象一样
        zippedFile, err := file.Open()
        if err != nil {
            log.Fatal(err)
        }
        defer zippedFile.Close()
        // 指定抽取的文件名。
        // 你可以指定全路径名或者一个前缀，这样可以把它们放在不同的文件夹中。
        // 我们这个例子使用打包文件中相同的文件名。
        targetDir := "./"
        extractedFilePath := filepath.Join(
            targetDir,
            file.Name,
        )
        // 抽取项目或者创建文件夹
        if file.FileInfo().IsDir() {
            // 创建文件夹并设置同样的权限
            log.Println("Creating directory:", extractedFilePath)
            os.MkdirAll(extractedFilePath, file.Mode())
        } else {
            //抽取正常的文件
            log.Println("Extracting file:", file.Name)
            outputFile, err := os.OpenFile(
                extractedFilePath,
                os.O_WRONLY|os.O_CREATE|os.O_TRUNC,
                file.Mode(),
            )
            if err != nil {
                log.Fatal(err)
            }
            defer outputFile.Close()
            // 通过io.Copy简洁地复制文件内容
            _, err = io.Copy(outputFile, zippedFile)
            if err != nil {
                log.Fatal(err)
            }
        }
    }
}
```

```java
//见4.2节工具类
```

## 4.3 压缩文件

```go
// 这个例子中使用gzip压缩格式，标准库还支持zlib, bz2, flate, lzw
package main
import (
    "os"
    "compress/gzip"
    "log"
)
func main() {
    outputFile, err := os.Create("test.txt.gz")
    if err != nil {
        log.Fatal(err)
    }
    gzipWriter := gzip.NewWriter(outputFile)
    defer gzipWriter.Close()
    // 当我们写如到gizp writer数据时，它会依次压缩数据并写入到底层的文件中。
    // 我们不必关心它是如何压缩的，还是像普通的writer一样操作即可。
    _, err = gzipWriter.Write([]byte("Gophers rule!\n"))
    if err != nil {
        log.Fatal(err)
    }
    log.Println("Compressed data written to file.")
}
```

```java
    void GzipFile(Path sourceFile, Path zipfile) throws IOException {

        try (OutputStream outs = Files.newOutputStream(zipfile, WRITE);
                InputStream in = Files.newInputStream(sourceFile, READ);
                GZIPOutputStream gzipOutputStream = new GZIPOutputStream(outs);) {

            byte[] data = in.readAllBytes(); // get data from somewhere.
            gzipOutputStream.write(data);
        }

    }

    void unGzipFile(final Path path) throws IOException {

        // 方式1，使用channel构建GZIPInputStream
        // FileChannel fc = FileChannel.open(path, READ);
        // GZIPInputStream gzin = new GZIPInputStream(Channels.newInputStream(fc));

        // 方式2，使用Files构建GZIPInputStream
        final GZIPInputStream gzin = new GZIPInputStream(Files.newInputStream(path, READ));

        final OutputStream fout = Files.newOutputStream(Paths.get("out.dcomp"), CREATE, WRITE);

        int num;
        final byte[] buf = new byte[1024];
        while ((num = gzin.read(buf, 0, buf.length)) != -1) {
            fout.write(buf, 0, num);
        }
        fout.flush();
        fout.close();

    }
```

## 4.4 解压缩文件

```go
import (
    "compress/gzip"
    "log"
    "io"
    "os"
)
func main() {
    // 打开一个gzip文件。
    // 文件是一个reader,但是我们可以使用各种数据源，比如web服务器返回的gzipped内容，
    // 它的内容不是一个文件，而是一个内存流
    gzipFile, err := os.Open("test.txt.gz")
    if err != nil {
        log.Fatal(err)
    }
    gzipReader, err := gzip.NewReader(gzipFile)
    if err != nil {
        log.Fatal(err)
    }
    defer gzipReader.Close()
    // 解压缩到一个writer,它是一个file writer
    outfileWriter, err := os.Create("unzipped.txt")
    if err != nil {
        log.Fatal(err)
    }
    defer outfileWriter.Close()
    // 复制内容
    _, err = io.Copy(outfileWriter, gzipReader)
    if err != nil {
        log.Fatal(err)
    }
}
```

```java
    void unGzipFile(final Path path) throws IOException {

        // 方式1，使用channel构建GZIPInputStream
        // FileChannel fc = FileChannel.open(path, READ);
        // GZIPInputStream gzin = new GZIPInputStream(Channels.newInputStream(fc));

        // 方式2，使用Files构建GZIPInputStream
        final GZIPInputStream gzin = new GZIPInputStream(Files.newInputStream(path, READ));

        final OutputStream fout = Files.newOutputStream(Paths.get("out.dcomp"), CREATE, WRITE);

        int num;
        final byte[] buf = new byte[1024];
        while ((num = gzin.read(buf, 0, buf.length)) != -1) {
            fout.write(buf, 0, num);
        }
        fout.flush();
        fout.close();

    }
```

## 5 其它

## 5.1 临时文件和目录

对golang，ioutil提供了两个函数: TempDir() 和 TempFile()。使用完毕后，调用者负责删除这些临时文件和文件夹。
有一点好处就是当你传递一个空字符串作为文件夹名的时候，它会在操作系统的临时文件夹中创建这些项目（/tmp on Linux）。
os.TempDir()返回当前操作系统的临时文件夹。

```go
func main() {
     // 在系统临时文件夹中创建一个临时文件夹
     tempDirPath, err := ioutil.TempDir("", "myTempDir")
     if err != nil {
          log.Fatal(err)
     }
     fmt.Println("Temp dir created:", tempDirPath)
     // 在临时文件夹中创建临时文件
     tempFile, err := ioutil.TempFile(tempDirPath, "myTempFile.txt")
     if err != nil {
          log.Fatal(err)
     }
     fmt.Println("Temp file created:", tempFile.Name())
     // ... 做一些操作 ...
     // 关闭文件
     err = tempFile.Close()
     if err != nil {
        log.Fatal(err)
    }
    // 删除我们创建的资源
     err = os.Remove(tempFile.Name())
     if err != nil {
        log.Fatal(err)
    }
     err = os.Remove(tempDirPath)
     if err != nil {
        log.Fatal(err)
    }
}
```

```java
    void  CreateTempFile(Path dir) throws IOException {

        Path tempFile = Files.createTempFile( dir, "myprefix-", ".txt");
        System.out.println("temp file created: " + tempFile);
        System.out.println("temp file size: "+ Files.size(tempFile));

        //输出类似
		// temp file created: ./myprefix-12592527991989979377.txt
		// temp file size: 0

        //生成临时目录的例子
        // Files.createTempDirectory(dir, "prefix");
    }
```

## 5.2 通过HTTP下载文件 TODO

## 5.3 哈希和摘要 TODO

