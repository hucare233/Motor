
# Funcition

![logo](\md_pictures\logo.png)
![bejite](\md_pictures\bejite.png)
![sanjiu](\md_pictures\sanjiu.png)

---

## Click here

[![hucare: Motor Driver](https://img.shields.io/badge/%E5%8F%AE%E5%92%9A%E8%9B%8B-MotorPro-blue)](http://www.github.com/hucare233)

---

+ tim2:电机超时检测
+ tim3：发送VESC,DJ,Elmo，Epos报文
+ usart1：虚拟示波器
+ usart2：串口屏

## 中断优先级

|中断/事件|抢占优先级|响应优先级|用途|
|:-:|:-:|:-:|:-:|
|TIM3_IRQn|1|0|VESC、DJ电机驱动|
|TIM2_IRQn|2|2|电机超时检测|
|CAN2_RX0_IRQn|2|0|DJ报文获取|
|CAN2_RX1_IRQn|2|1|ELMO/VESC/EPOS报文获取|
|CAN1_RX0_IRQn|3|0|主控|
|USART2_IRQn|4|1|串口屏|
|USART2_DMA|7|1||
|USART1_DMA|6|1|虚拟示波器|
|USART3_IRQn|4|0|编码器|

---  

# 待做的事

+ 将can1的接收放入接收队列（已完成）
+ 转向电机（已完成）
+ 本杰明驱动器的位置模式实现（上位机完成，刘定川接手）
+ 底盘Flat90调试（未完成）

# 注意事项

+ TIM1、TIM8-TIM11的时钟为APB2时钟的两倍即168M，TIM2-TIM7、TIM12~TIM14的时钟为APB1的时钟的两倍即84M。
+ 定时器分频系数不为1，将时钟倍数乘2作为定时器时钟。
+ 编码器线数4倍频
+ 使用操作系统时，在使用与其他任务共享的全局变量要进入临界区
+ `OSTimeDly(1) = 0.1ms`
+ 虚拟示波器用的`CRC16`协议
+ 数码管显示 :

## LED

| Number  |  status   |
| :----: | :-------: |
|  1   | dj超时  |
|  2  | vesc超时 |
| 3  |   elmo超时    |
| 4 |   epos超时   |

## 矩阵键盘

|一|二|三|四|
|:-:|:-:|:-:|:-:|
|F-放歌|E-|D-|C-|
|B-|A-|9-|8-|
|7-STOP|6-BEGIN|5-失能ELMO|4-使能ELMO|
|3-STOP|2-BEGIN|1-失能大疆|0-使能大疆|

---

## ***********************************/

下面有部分直接上了王锟学长的原话，很有总结意义

## /***********************************

---

## DJ电机相关事项

+ DJ电机电流是一起发的，包括所有电机。所以多个板子控制电机的时候不要将can2线连在一起
+ DJ电机开始位置记录因为在`pulse_caculate()`函数里面开始distance会先运行一次才会在对`valuePrv`进行赋值，所以会有`0~8192`的脉冲误差（已尝试解决一部分误差）
+ 将电机使能放在了start里面以达成延时使能的要求从而避免初始位置误差  

## ELMO相关事项

+ 电机模式枚举体因ELMO的电流、速度、位置为固定1，2，5
+ enable只是一个状态，用于观测，并不能真正`像3508实现使能
+ 关于ElmoCANopen协议详见`文档9.1.1.1`
+ `intrinsic.PULSE`直接乘4方便计算
+ JV、SP的计算方法  ——    `速度*编码器线数*4/60=jv(sp)`,位置计算同理
+ U10电机有时候会犯病，可能是因为没有霍尔的影响，目前能找到的解决方法只有重新上电，日后可以研究下霍尔的使用
+ PVT模式数组的设置`不可保存`，得在程序中手动添加
+ PVT数组 `QP[1]=0 QV[1]=0;`，MP[3]: 0-不循环 1-循环
+ RM=0在MO=0时发送，PV.PT=1在MO=1之后发送，在下一个BG执行
+ HMM模式method设置为`actual method` ，设置当前位置为零点，`offset position 和 homing position均给0`

## VESC相关事项

+ VESC [Github](http://github.com/vedderb) 地址
+ VESC [论坛](http://vesc-project.com/vesc_tool) 地址
+ VESC需要定时赋值否则会自动释放
+ VESC暂时只用到了速度模式，所以程序基本没写多少（位置模式我在上位机用队里的磁编试过，但是板子的can好像有点问题一直就没做成）

## EPOS相关事项

+ EPOS的电机使能需要连续在控制字写入`0x06 0x07 0x0F`,使能一次后只需再发送0x0F就行
+ 速度模式王锟学长已经摸得很清楚了，具体可以直接看他总结
+ 位置模式有两种模式，类似于编码器的绝对式和增量式，可以借助代码实践理解
+ 官方提供的halt和quickstop的刹车效果不很理想，平时基本不用，速度模式下的锁位置通过直接更改为位置模式完成
+ 王锟学长之前遇到过4个电机用机构的代码没问题，与主控加入调试后会有驱动器报错现象，初步结论可能是因为主控发的某些报文它也能接收
+ 位置模式再到达目标后，若想再次运动必须`再发一次0x0F`
+ epos没有查询电流的功能，只能查询当前扭矩在除以一个系数算出电流（参数在flat90手册里头）
+ 启动HMM模式，控制字写入`0x1F`

# 个人臊皮日志——

---

## 2020.8.21

1. 队里移植的ucos系统跟原子的不同，已有的模板不可擅自更改（切记切记）

---

## 2020.9.1

1. DJ在定时器设置发送报文的时间最好设置在1ms左右，同期人曾将其改为100ms，发生了十分鬼畜的现象，为此找了很久问题

---

## 2020.9.3

1. 加入了elmo程序，测试完成

---

## 2020.9.10

1. 加入了vesc程序，并添加tim2用作vesc和elmo超时检测

---

## 2020.9.12

1. 添加了epos的程序，测试了位置环，发送后不要漏掉在发送一次0X0F

---

## 2020.9.20

1. 与主控通信，加入了can1程序

---

## 2020.9.22

1. 写完了转向电机的程序

---

## 2020.9.25

1. 加入了串口屏程序，现在因为方便在usart2中通过接收更改标志位（日后与主控通信时候更改掉）/*******/                别忘了！！！

---

## 2020.9.26

1. 猛然发现elmo和epos的报错id好像一样，，，，啊这，继续想办法中

---

## 2020.9.29

1. 前天盲目自信写完程序就没测试，今天一看一堆bug，长记性了，（我之前把电机的超时检测都放在频率1ms的tim2里头，会出现很明显的丢包，频率改至200ms就没啥问题了）

---

## 2020.10.3

1. 在起始任务中使能电机以清除起始误差

---

## 2020.10.6

1. 吐了，更新keil后发现有个警告，啥方法都试了结果还是不行，但是可以用，估计应该是软件bug，杀人诛心![youl]![youl]

---

## 2020.10.14

1. 被抓去学主控了![xk]![fad]，最近就没什么时间学机构= =
2. 美化了下自己的代码，顺便把Read Me弄得骚一些![dedede] (不是偷懒没写代码![doge])

---

## 2020.10.18

1. 大三准备比赛这几天就很放松，看主控代码，机构这边VESC得捡起来![youl],希望上级能拨款买几个新的回来= =

---

## 2020.10.26

1. 人类早期驯服vscode

---

## 2020.11.1

1. 打工魂打工人，打工都是人上人

---

## 2020.11.4

1. 加入了新的示波器（任务中的延时可`控制FPS`）

---

## 2020.11.6

1. 显示器到了，快乐
2. 使用了elmo的PVT模式

---

## 2020.11.7

1. 成功配置git
2. 做一个合格木匠人![doge]

---

## 2020.11.9

1. 底盘调试基本完成，注意GearRatio在can1中已经算过了，在位置计算中不用加上齿轮外参
2. 原来外参齿轮比的计算在主控代码中，我认为最好改为机构负责，毕竟调的大大小小的机构总不可能齿数比一直是1，也能减轻主控的负担

---

## 2020.11.10

1. 加入串口屏键盘

# --to be continue

# 一只19级的咸鱼

[fad]: \md_pictures\fad.png
[shuai]: \md_pictures\shuai.png
[xk]: \md_pictures\xk.png
[cy]: \md_pictures\cy.png
[youl]: \md_pictures\youl.png
[dedede]: \md_pictures\123.gif
[doge]: \md_pictures\doge.png
