


## ref

[关于Linux安装RT-PREEMPT实时补丁](https://blog.csdn.net/weixin_43455581/article/details/103899362?utm_term=preemptrt%E5%AE%89%E8%A3%85&utm_medium=distribute.pc_aggpage_search_result.none-task-blog-2~all~sobaiduweb~default-1-103899362&spm=3001.4430)， [相应应用例程](https://wiki.linuxfoundation.org/realtime/documentation/howto/applications/application_base)

- [getting-started-with-machinekit-on-the-chip](https://machinekoder.com/getting-started-with-machinekit-on-the-chip/)
- [machinekit-on-the-c-h-i-p-9-computer](https://machinekoder.com/machinekit-on-the-c-h-i-p-9-computer/), 
- [PICnc_5axis](http://www.wire2wire.org/PICnc_5axis/PICnc_5axis.html) ， [github](https://github.com/Wireb/PICnc-5axis)
- [picnc v2](https://github.com/kinsamanka/PICnc-V2) , [related wiki](https://github.com/kinsamanka/PICnc-V2/wiki)
- [new Debian Stretch + LinuxCNC uspace test ISO:](http://www.linuxcnc.org/testing-stretch-rtpreempt/linuxcnc-stretch-uspace-amd64-r8.iso)
- 虚拟机网络慢，尝试下 sudo ethtool -K ens160 tx off sg off tso off
- [Machinekit Raspbian Jessie for RPi1 and RPi2](https://www.raspberrypi.org/forums/viewtopic.php?p=920778)

- [a-tutorial-on-linux-for-real-time-tasks](https://hackaday.com/2014/04/25/a-tutorial-on-linux-for-real-time-tasks/)
- [Machinekit, LinuxCNC and GPIO.pdf](http://topcnc.ru/opi1_cnc/5%20-%20Machinekit,%20LinuxCNC%20and%20GPIO.pdf)

- [hal tutorial](http://www.machinekit.io/docs/hal/tutorial/)
- [accessing-raspberry-pi-via-xdmcp](https://blog.netsarang.com/530/accessing-raspberry-pi-via-xdmcp/)
##  raspberry & machinekit

- "machinekit-hal\src\hal\utils\halcmd_commands.c"中查看hal的各个命令
 1794  sudo apt-get install rt-tests
 1795  sudo cyclictest -p 80 -t5 -n  



2、备份SD卡
使用 dd 命令可以直接备份SD卡。这里树莓派的SD卡的路径是 /dev/sdc1 和 /dev/sdc2 ，所以备份整个SD卡的路径就是 /dev/sdc。
输入备份命令：
$ sudo dd if=/dev/sdc | gzip>/home/lixinxing/raspberry.gz

3、将树莓派镜像还原
备份完成后，如果需要还原树莓派，我们可以通过以下方法进行。
方法一：
在windows环境下，将备份文件解压，然后使用 win32diskimager 将备份文件写入到新的SD卡。
方法二：
在linux环境下，使用以下步骤将备份文件写入到SD卡。
1、删除SD卡分区。
使用 fdisk 命令对SD卡格式化
2、写入备份文件
$ sudo gzip -dc /home/lixinxing/raspberry.gz | sudo dd of=/dev/sdc
其中备份文件的位置、文件名和 SD卡的路径要根据实际选择。
这样就将备份还原到树莓派了，可以将SD卡插入树莓派启动！




## raspberry PI3 B 安装 machinekit

### 安装 RT-preempt系统补丁版本

- [安装指导#Raspberry Pi: Preempt-RT Patching Tutorial for Kernel 4.14.y ](https://lemariva.com/blog/2018/02/raspberry-pi-rt-preempt-tutorial-for-kernel-4-14-y). 
  根据该安装指导生成的linux image为[文件rt-kernel.tgz](./rt-kernel.tgz)，该image版本号是"4.14.59-rt37-v7+"



### 安装machinekit

- [安装指导](http://www.machinekit.io/docs/developing/machinekit-developing/)
- 实际执行指令

``` shell
sudo apt-get install git dpkg-dev
sudo apt-get install --no-install-recommends devscripts equivs
git clone https://github.com/machinekit/machinekit.git
cd machinekit
cd machinekit
# to add RT-PREEMPT support, add a 'r'
# to add Xenomai support, add an 'x'
# this builds for Posix, RT-PREEMPT, Xenomai:
debian/configure -pr
sudo scripts/apt-installbuilddeps
sudo mk-build-deps -ir

cd src
./autogen.sh
# for the Beaglebone, add --with-platform-beaglebone to ./configure
# for the Raspberry2, add --with-platform-raspberry to ./configure
# for PC platforms, add --with-rt-preempt  --with-posix  --with-xenomai
./configure  --with-platform-raspberry
##  please review and edit /home/pi/machinekit/etc/linuxcnc/machinekit.ini as needed!
##  touching up /home/pi/machinekit/lib/python/machinekit/config.py
##  checking usability of /home/pi/machinekit/lib/python/machinekit/config.py... done


######################################################################
#                Machinekit - Enhanced Machine Controller            #
######################################################################
#                                                                    #
#   Machinekit is a software system for computer control of machine  #
#   tools such as milling machines. Machinekit is released under the #
#   GPL.  Check out http://www.machinekit.io for more details.       #
#                                                                    #
#                                                                    #
#   It seems that ./configure completed successfully.                #
#   If things don't work check config.log for errors & warnings      #
#                                                                    #
#   Next compile by typing                                           #
#         make                                                       #
#         sudo make setuid                                           #
#                                                                    #
#   Before running the software, set the environment:                #
#         . (top dir)/scripts/rip-environment                        #
#                                                                    #
#   To run the software type                                         #
#         machinekit                                                 #
#                                                                    #
######################################################################

make
##  .......
##  make[1]: Leaving directory '/home/pi/machinekit/src'
##  for f in posix rt-preempt; do \
##      test -f ../libexec/rtapi_app_$f -a \
##  	    \( 0`stat -c %u ../libexec/rtapi_app_$f 2>/dev/null` \
##  		-ne 0 -o ! -u ../libexec/rtapi_app_$f \) \
##  	&& need_setuid=1; \
##      ln -sf /home/pi/machinekit/rtlib/prubin/pru_generic.bin ../rtlib/$f/pru_generic.bin; \
##      ln -sf /home/pi/machinekit/rtlib/prubin/pru_generic.dbg ../rtlib/$f/pru_generic.dbg; \
##      ln -sf /home/pi/machinekit/rtlib/prubin/pru_decamux.bin ../rtlib/$f/pru_decamux.bin; \
##      ln -sf /home/pi/machinekit/rtlib/prubin/pru_decamux.dbg ../rtlib/$f/pru_decamux.dbg; \
##  done; \
##  test "$need_setuid" = 1 && \
##      echo -n "You now need to run 'sudo make setuid' " && \
##      echo "in order to run in place." || true
##  You now need to run 'sudo make setuid' in order to run in place.
##  make: Leaving directory '/home/pi/machinekit/src'

sudo make setuid

# this script checks for missing configuration files
# and will give hints how to remedy:
../scripts/check-system-configuration.sh

##  If you wish to run this installation by default, add the next lines to your ~/.bashrc file, so that every new 
##  terminal is set up correctly for running Machinekit.
echo 'if [ -f ~/machinekit/scripts/rip-environment ]; then
    source ~/machinekit/scripts/rip-environment
    echo "Environment set up for running Machinekit"
fi' >> ~/.bashrc

```
- 最终结果： [machinekit.tgz](./machinekit.tgz) 是采用"--enable-run-in-place"编译的结果。


### 安装driver 示意
```shell
pi@raspberrypi:~/machinekit/src/hal/drivers $ comp --compile hal_gpio.c
##  Compiling realtime hal_gpio.c
##  Linking hal_gpio.so

pi@raspberrypi:~ $ find ~/ -name *hal_gpio*
## /home/pi/machinekit/src/depends/posix/hal/drivers/hal_gpio.d
## /home/pi/machinekit/src/depends/rt-preempt/hal/drivers/hal_gpio.d
## /home/pi/machinekit/src/hal/drivers/hal_gpio.so
## /home/pi/machinekit/src/hal/drivers/hal_gpio_h3_demo.hal
## /home/pi/machinekit/src/hal/drivers/hal_gpio.c
## /home/pi/machinekit/src/hal/drivers/hal_gpio_demo.hal
## /home/pi/machinekit/src/hal/drivers/hal_gpio_h3-README
## /home/pi/machinekit/src/hal/drivers/hal_gpio_h3.c
## /home/pi/machinekit/src/hal/user_comps/hal_gpio_mcp23017.py

## /home/pi/machinekit/bin/hal_gpio_mcp23017
## /home/pi/machinekit/rtlib/posix/hal_gpio.so
## /home/pi/machinekit/rtlib/rt-preempt/hal_gpio.so
```

- 下面的demo使用hal_gpio
```shell
pi@raspberrypi:~/machinekit $ halrun -f ./src/hal/drivers/hal_gpio_demo.hal
```


## 单独安装 Machinekit-hal实验（amd64 ubuntu）
```shell
sudo apt-get install libprotobuf-dev  
git clone https://github.com:zeromq/czmq.git
sudo apt-get install python-protobuf  python-pyftpdlib 
sudo apt-get install liburiparser-dev  uuid-dev  libavahi-client-dev libmodbus-dev  libglib2.0-dev   libreadline-dev   libxmu-dev  libboost-dev
sudo apt-get install libglu1-mesa-dev freeglut3-dev mesa-common-dev 

sudo apt-get install   libxinerama-dev

./configure --with-libusb-1.0=no --disable-gtk 


checking if /home/atmel/rpi-kernel/machinekit-hal/etc/linuxcnc/machinekit.ini exists ... /home/atmel/rpi-kernel/machinekit-hal/etc/linuxcnc/machinekit.ini does not exist - using default version
please review and edit /home/atmel/rpi-kernel/machinekit-hal/etc/linuxcnc/machinekit.ini as needed!
touching up /home/atmel/rpi-kernel/machinekit-hal/lib/python/machinekit/config.py
checking usability of /home/atmel/rpi-kernel/machinekit-hal/lib/python/machinekit/config.py... done


######################################################################
#                Machinekit - Enhanced Machine Controller            #
######################################################################
#                                                                    #
#   Machinekit is a software system for computer control of machine  #
#   tools such as milling machines. Machinekit is released under the #
#   GPL.  Check out http://www.machinekit.io for more details.       #
#                                                                    #
#                                                                    #
#   It seems that ./configure completed successfully.                #
#   If things don't work check config.log for errors & warnings      #
#                                                                    #
#   Next compile by typing                                           #
#         make                                                       #
#         sudo make setuid                                           #
#                                                                    #
#   Before running the software, set the environment:                #
#         . (top dir)/scripts/rip-environment                        #
#                                                                    #
#   To run the software type                                         #
#         machinekit                                                 #
#                                                                    #
######################################################################
```
- 单纯编译hal与编译整体的生成bin文件的区别
```text
atmel@atmel-virtual-machine:~/machinekit-hal/bin$ ls
5axisgui      gs2_vfd            halsampler        hal_temp_atlas  linmove     position    scaragui
adxl345       halcmd             halshow           hal_temp_bbb    maho600gui  puma560gui  shuttlexpress
comp          hal_gpio_mcp23017  hal_storage       hbmgui          mank        pumagui     sizes
configserver  hal_gy86           halstreamer       hexagui         max5gui     pyvcp       unionread
encdec        hal_input          haltalk           hy_vfd          mklauncher  rostock     videoserver
gladevcp      hal_pwm_pca9685    hal_temp_ads7828  instcomp        npbdecode   rtprintf    yapps
atmel@atmel-virtual-machine:~/machinekit-hal/bin$ ls ../../machinekit/bin/
5axisgui       gremlin               halshow           io           mb2hal        pyvcp          unionread
adxl345        gremlin_view          hal_storage       iov2         mdi           rostock        vfdb_vfd
axis           gs2_vfd               halstreamer       keystick     milltask      rs274          vfs11_vfd
axis-remote    gscreen               haltalk           linmove      mklauncher    rtprintf       videoserver
classicladder  halcmd                hal_temp_ads7828  lintini      mkwrapper     scaragui       xhc-hb04
comp           hal_gpio_mcp23017     hal_temp_atlas    linuxcnclcd  ngcgui        schedrmt       xlinuxcnc
configserver   hal_gy86              hal_temp_bbb      linuxcncrsh  npbdecode     shuttlexpress  yapps
debuglevel     hal_input             halui             linuxcncsvr  pncconf       sizes
drawbotkins    hal_manualtoolchange  hbmgui            linuxcnctop  position      stepconf
encdec         halmeter              hexagui           machinekit   profile_axis  teach-in
genserkins     hal_pwm_pca9685       hy_vfd            maho600gui   puma560gui    tooledit
gladevcp       halsampler            image-to-gcode    mank         pumagui       touchy
gmoccapy       halscope              instcomp          max5gui      pyngcgui      tracking-test
atmel@atmel-virtual-machine:~/machinekit-hal/bin$ 
```


## 举例
使用BCM pin 7,  对应gpio4
```shell
pi@raspberrypi:~/wiringPi $ halrun
halcmd: loadrt hal_gpio
halcmd: setp hal_gpio.pin-07-out 1
halcmd: start
halcmd: loadrt threads name1=fast period1=50000 name2=slow period2=1000000 
halcmd: show funct 
Exported Functions:
  Comp   Inst CodeAddr      Arg           FP   Users Type    Name
    66        0000761090a8  000000000000  NO       0 user    delinst
    78        000076d9ad00  000000000000  NO       0 thread  hal_gpio.read
    78        000076d9abd8  000000000000  NO       0 thread  hal_gpio.write
    66        000076108ea8  000000000000  NO       0 user    newinst

halcmd: addf hal_gpio.write slow
halcmd: setp hal_gpio.pin-07-out 0
halcmd: setp hal_gpio.pin-07-out 1
halcmd: setp hal_gpio.pin-07-out 0
halcmd: save all saved.hal
halcmd: setp hal_gpio.pin-07-out 1
halcmd: setp hal_gpio.pin-07-out 0
halcmd: 
```