

[GRBL – 如何對刀 ( Probing )](https://www.misterngan.com/3281/grbl-probing-howto/)

机床坐标系：

采用右手坐标系，X为指向操作者方向，Z为指向道具远离工件方向

G41（左，顺时针）补偿； G42（右，逆时针）补偿； 

工作坐标系均以机床原点为参考点，分别以各自与机床原点的偏移量表示，需要提前输入机床内部

https://github.com/goburrow/modbus


- [linuxcnc Gcode手册](http://linuxcnc.org/docs/html/gcode/g-code.html)
- [Gcode手册](https://reprap.org/wiki/G-code/zh_cn#G10:_.E6.89.93.E5.8D.B0.E5.A4.B4.E5.81.8F.E7.A7.BB)
- [GCode-Tutorial](https://github.com/uwplse/incarnate/wiki/GCode-Tutorial)
- [浅谈数控铣床中G54与G92指令](https://wenku.baidu.com/view/5567d063f46527d3240ce0a7.html)

#

## G43，G44，G49：刀具补偿(Tool Compensation)

G43是正向补偿， G44是负向补偿，因此用的刀比對Z軸的刀長就G43，短就G44

G43 uses the Z-axis height to determine the length of separate tools. It helps the CNC programming to comprehend the relation between the tip of the tool and the thing on which its action is going on.

Format: G43 Tool Length Compensation + (plus)

Example: N3 G43 H1 Z50

G44 is hardly used as an alternative to G43. Some do not consider its existence. G44 is a code that enables the control to start applying for tool length compensation. The control will drive the spindle to Z1.0 but it is brought down by positive offset amount H2. These offsets can be used again for any other program by just changing the value of that code.

G44 requires an address to summon the tool length to offset register value. For that, G44 generally takes H address. The value is always positive because it works in a negative direction by subtracting the length offset from all Z-axis positions.

Format: G44 tool length compensation – (minus)

Example: N172 G44 H02 Z5 M08

什么是刀具补偿？
CNC中的刀具长度补偿代码用于调整各种刀具的长度差异。刀具补偿代码无需更改零件程序就可以完成任务。

参考工具是已加载的标准长度，并在Z轴的帮助下被放下，直到它接触到曲面为止，然后您可以在此处设置Z参考位置。使用不同的工具重复相同的活动，并要求控件测量工具。然后，控件会比较Z轴位置和Z参考位置。两者之间的差异是工具的长度偏移。

使用更短的工具接触同一表面可以通过将Z轴进一步向下移动来实现。此活动的结果是负偏移。因此，负偏置与较短的刀具有关，而正偏置与使用较长的刀具有关。

刀具长度补偿使用3个G代码，分别为G43，G44和G49。

如何在数控机床上使用刀具长度补偿？
在这种补偿长度下，程序员在编写程序时可以忘记每个工具的长度。程序员不需要知道所用每种工具的长度。更好的方法是在每个刀具的第一个Z轴逼近运动中恢复刀具长度补偿。

在机器设置过程中，操作员必须输入每个刀具的刀具长度补偿值。因此，必须首先测量工具。

G43是常用的命令，用于恢复刀具长度补偿。除G43命令外，程序员还使用H代码。H代码专门指出了包含刀具长度值的偏移量。

什么是G43？
G43是刀具补偿G代码，程序员可以使用它来检索刀具长度补偿。在自动换刀操作中使用。

G43指示控件，在开始操作之前，需要考虑偏移寄存器中存储的刀具长度。然后将已注册的量添加到Z轴移动，直到Z轴移动被G49代码取消为止。

以G开头的代码可以重复活动无限次，直到停止为止。G代码用于启动动作。在CNC编程中，G43代码用于调节对比工具之间的长度差异。

G43使用Z轴高度确定单独工具的长度。它有助于CNC编程理解工具的尖端与其所作用的物体之间的关系。

格式： G43刀具长度补偿+（加号）

例如： N3 G43 H1 Z50

什么是G44？
G44是CNC编程中的代码，与在正方向上使用刀具长度补偿的G43相反，它在负方向上使用刀具长度补偿。在G44代码中，可以将刀具长度补偿提前并从刀具长度中扣除。

G44几乎不能替代G43。有些人不认为它的存在。G44是使控件能够开始申请刀具长度补偿的代码。控制器会将主轴驱动到Z1.0，但将其降低正偏移量H2。只需更改该代码的值，即可将这些偏移量再次用于任何其他程序。

G44需要一个地址来调用刀具长度到偏置寄存器的值。为此，G44通常采用H地址。该值始终为正，因为它通过从所有Z轴位置减去长度偏移量而在负方向上起作用。

格式： G44刀具长度补偿–（减）

例如： N172 G44 H02 Z5 M08

什么是G49？
G49是G代码，有助于取消G43和G44刀具长度补偿。如果将补偿量用作H00，则也可能取消刀具长度补偿。

G49通过取消当前刀具补偿功能起作用，并帮助其返回零值。在自动换刀（ATC）期间，当我们使用G43代码时，它将成为有用的代码。此代码不需要任何额外的参数。它通过清除正（G43）和负（G44）偏移来中和该值。

示例：在下面的示例中，借助G43代码建立刀具补偿，然后借助G49代码将值返回零。

结论
刀具长度补偿可帮助CNC编程机调整不同长度的刀具。高效的机械师必须设置Z轴原点，并且该机器可以自动更换刀具。所有使CNC机器能够相应工作的指令都称为G代码。如果您熟练掌握基本的G代码，则CNC事业的美好前景正在等待着您。

## G17, G18, G19: 平面选择

The G-Codes for the plane selection is:
G17 is used for XY Plane towards the front face whereas G18 is used for XZ axis design.
G19 is used for YZ Plane, and it works on the opposite plane of the G17. When a person switches on the CNC machine, the G17 is activated for the X Y plane as a default.

G17 XY Plane Selection:

G17 Format:
G17 G02 X_Y_ I_ J_



#
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


