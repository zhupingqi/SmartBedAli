#### 介绍

智能家居是在互联网影响之下物联化的体现。智能家居通过物联网技术将家中的各种设备连接到一起，提供家电控制、照明控制、电话远程控制、室内外遥控、防盗报警、暖通控制、红外转发以及可编程定时控制等多种功能和手段。

    智能电动床属于智能家具产品的一类，反映了当前智能家居领域发展趋势和全新的产品概念。

    本教程使用STM32F405实现智能电动床的控制，基本功能如下：

    语音控制模块：LD3320，提供基于 " 关键词语列表 " 的语音识别，可以使用语音命令控制智能电动床进行相关操作，例如床头抬起，床位抬起，还原设备默认设置及指定智能电动床当前模式。

    BLE蓝牙：低功耗蓝牙，用来连接蓝牙遥控器及微信小程序，微信小程序提供WIFI配网及本地遥控操作

    物联网模块：EMW3080, 提供阿里云物联网平台访问，属性上报，远程控制等功能

    马达控制板：使用SPI协议最多控制4个电机，实现正反转控制，可用于双人电动床


#### 项目架构

![Image text](https://img-blog.csdnimg.cn/20200619102846650.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3dlaXhpbl80MjU4MTY2Ng==,size_16,color_FFFFFF,t_70)

#### STM32CubeMAX配置如下

![Image text](https://img-blog.csdnimg.cn/20200619112051324.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3dlaXhpbl80MjU4MTY2Ng==,size_16,color_FFFFFF,t_70)


#### 通讯协议如下

    SPI1 访问W25Q16Flash存储芯片

    软SPI(PA5,PA6,PA7) 访问马达控制板

    USAERT2 LD3320语音模块

    USAERT3 EMW3080阿里物联网模块

    USAERT6 BLE蓝牙

#### SRC部分源文件简介
    smartbed.c 马达控制
    emw3080.c 阿里云物理网访问模块
    ble.c 蓝牙访问
    w25qxx.c flash存储芯片读写
    ld3320.c 语音模块