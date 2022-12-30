# esp-01s control servo

编译平台：PlatformIO

## 核心文件
```
* src/main.c  			源文件
* index.html  			内嵌网页源码
* PCB_2022-12-30.pdf 	底板原理图
```

## 修改代码

```c
#ifndef STASSID
#define STASSID "*********" 	// WIFI 名
#define STAPSK "*********"  	// WIFI 密码
#endif

// 舵机基本角度，不能太低，因为控制舵机的角度需要加减计算
const int g_base_r = 80;  
// 舵机旋转角度
const int g_switch_r = 40;
// 舵机运行时间
const int servo_runtime = 200;
```

## 编译

代码适配 arduino 框架， 可以复制到arduino中编译。 不难非常简单。

platformIO 的编译流程去网上搜索 “platformIO如何使用”。 不难非常简单。

## 烧写

在底板上有两个按钮， `BOT` 和 `RST`
按住 `BOT` 再按 `RST`,  松开`RST`， 就进入了 **下载模式** ，这时使用串口就可少写进去了。

## 使用

打开串口调试助手， 点击 `RST` 按钮重启模块， 等到打印出IP地址， 使用浏览器输入IP， 就会显示一个网页， 点击 `OPEN` 和 `CLOSE` 就可以控制了。