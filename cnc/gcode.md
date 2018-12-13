




https://github.com/goburrow/modbus


- [linuxcnc Gcode手册](http://linuxcnc.org/docs/html/gcode/g-code.html)
- [Gcode手册](https://reprap.org/wiki/G-code/zh_cn#G10:_.E6.89.93.E5.8D.B0.E5.A4.B4.E5.81.8F.E7.A7.BB)
- [GCode-Tutorial](https://github.com/uwplse/incarnate/wiki/GCode-Tutorial)
- [浅谈数控铣床中G54与G92指令](https://wenku.baidu.com/view/5567d063f46527d3240ce0a7.html)

### LinuxCNC中RS-274/NGC解析器的编译和使用
http://blog.sina.com.cn/s/blog_a2a6dd380102vrai.html

 LinuxCNC是一个著名的开源数控软件，目前最新发行版本是:LinuxCNC 2.6.8，截止2015/6/14.LinuxCNC来源于NIST的EMC项目，先后推出了EMC1和EMC2，由于与EMC公司的版权纠纷，遂改名为LinuxCNC。

      在LinuxCNC源码中包含一个独立的G代码(RS-274)解析器(Stand Alone Interpreter，下文我们简称SAI),我们可以自己编译得到SAI。


你可以在这里下载得到RS274NGC_3独立解析器的源代码rs274ngc2.zip和说明文档RS274NGC_3.pdf:

       https://code.google.com/p/rs274ngc/downloads/list	  

第一部分 编译SAI
    1.进入源代码所在目录，该目录包含以下11个文件:

        ​​a.源代码文件(7个) :

           canon.hh  rs274ngc_return.hh  rs274ngc.hh

           ​canon_pre.cc  driver.cc   rs274ngc_error.cc  rs274ngc_pre.cc 

        b.参数文件(2个):rs274ngc.tool_default    rs274ngc.var

        c.makefile文件(1个):Makefile

        d.版权信息文件(1个):COPYING

   2.源代码文件介绍     


### gcode 命令含义
```go
func categorizeLine(input string) (output string) {
	if input == "" {
		return ""
	}
	switch []rune(input)[0] {
	case 'G':
		return "Standard GCode command, such as move to a point"
	case 'M':
		return "RepRap-defined command, such as turn on a cooling fan"
	case 'T':
		return "Select tool nnn. In RepRap, a tool is typically associated with a nozzle, which may be fed by one or more extruders."
	case 'S':
		return "Command parameter, such as time in seconds; temperatures; voltage to send to a motor"
	case 'P':
		return "Command parameter, such as time in milliseconds; proportional (Kp) in PID Tuning"
	case 'X':
		return "A X coordinate, usually to move to. This can be an Integer or Fractional number."
	case 'Y':
		return "A Y coordinate, usually to move to. This can be an Integer or Fractional number."
	case 'Z':
		return "A Z coordinate, usually to move to. This can be an Integer or Fractional number."
	case 'U':
		return "Additional axis coordinates (RepRapFirmware)"
	case 'V':
		return "Additional axis coordinates (RepRapFirmware)"
	case 'W':
		return "Additional axis coordinates (RepRapFirmware)"
	case 'I':
		return "Parameter - X-offset in arc move; integral (Ki) in PID Tuning"
	case 'J':
		return "Parameter - Y-offset in arc move"
	case 'D':
		return "Parameter - used for diameter; derivative (Kd) in PID Tuning"
	case 'H':
		return "Parameter - used for heater number in PID Tuning"
	case 'F':
		return "Feedrate in mm per minute. (Speed of print head movement)"
	case 'R':
		return "Parameter - used for temperatures"
	case 'Q':
		return "Parameter - not currently used"
	case 'E':
		return "Length of extrudate. This is exactly like X, Y and Z, but for the length of filament to consume."
	case 'N':
		return "Line number. Used to request repeat transmission in the case of communications errors."
	case '*':
		return "Checksum. Used to check for communications errors."
	case ';':
		return "Comment"
	default:
		return "?"
}
```


# linucnc
[EMC2的硬件抽象层原理与实现](https://wenku.baidu.com/view/76b1cc796f1aff00bfd51e40.html)

```txt
│  
├─bin                   可执行文件目录，在编译后存在
│      
├─configs               预先定义的针对不同控制卡及模拟运行的目录
│          
├─docs
│                  
├─gnupg
│      
├─include
│      .gitignore
│      
├─lib                   用户模式目标库文件，在编译后存在
│              
├─nc_files              NC例子文件
│              
├─rtlib                 内核实时模式目标库文件，在编译后存在
│      
├─scripts               shell程序目录，有启动cmc2，设置cmc2运行环境等shell程序
│      
├─share                 共享文件目录
│          
├─src
│  │  
│  ├─emc
│  │  ├─canterp          实现命令的规范解释输出
│  │  │      
│  │  ├─ini             ini配置文件相关操作
│  │  │      
│  │  ├─iotask          I/O接口
│  │  │      
│  │  ├─kinematics      轨迹和运动相关，例如工件坐标和机器坐标的转换实现
│  │  │      
│  │  ├─motion          运动控制模块，通过共享内存和其他模块通信
│  │  │      
│  │  ├─motion-logger
│  │  │      
│  │  ├─nml_intf        EMC特定的NML实现，用于在emc2中的所有通信
│  │  │      
│  │  ├─pythonplugin
│  │  │      
│  │  ├─rs274ngc        G代码解释器
│  │  │      
│  │  ├─sai
│  │  │      
│  │  ├─task            emc2的核心模块，实现任务分配执行
│  │  │      
│  │  ├─toolstore
│  │  │          
│  │  ├─tp
│  │              
│  ├─external
│  │                  
│  ├─hal                硬件抽象层，为所有硬件提供统一的接口
│  │              
│  ├─libnml             精简的RCSlib
│          
├─tcl                   早期的基于tcl/tk的用户节目
│          
└─tests
```

编译安装最好是利用linuxcnc.org提供的cmc2-install-sim.sh(模拟实时)/cmc2-install.sh(实时)命令安装或者编译。 



cmc2-install-sim.sh(模拟实时)

为了更清楚了解emc2安装要求和过程，我们以emc2拟时版为例，进行手工编译安装首先安装debian或Ubunlu，在编译emc2前要安装emc2编译需要的所有支持包 打开http://www.linuxcnc.org/hardy/dist/hardy/emc2.3-sim/source/emc2_2.3.0.dsc 会到如下一行：
Build-Depends:bwidget,debhelper(>>4．0．0), g++，gettext，ghostscnpt,grorf,imagemagick，libc6-dev，
libgll-mesa-dev,libglul-mesa-dev,libgnomeprintui2.2-dev，libgtk2.0-dev,libncurses-dev,libreadline5-dev,libxaw7-dev,linux-headers-2．6．24．16-rtai．linux-Image-2.6．24．6.，
lyx，make，python，python-dev,rtai-modules-2．6．24．16．rtai，
dev，bin，extra，exlive—extra-
tk8.4-dev，xsltproc
除了里面涉及rtai的相关包，我们可以用apt -get 命令安装其他所有包。
第二步进入到解压后的源文件吕录cd ./emc2_2.3.0/src, 在源文件目录中编译安装emc2，依次输入如下
```shell
./configure --enable-simulator --enable-run-in-placemake 
sudo make setuid 
```
编译完成后运行./script/emc进行测试。
参数说明：
--enable-run-in-place在当前文件夹中运行。 如果想在安装前先测试，者担心会盖了已安装好的文件，可



# RS274NGC 规范翻译 


