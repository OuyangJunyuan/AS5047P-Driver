# AS5047P-Driver

@Author：OuyangJunyuan                                                                                                                                                               @Date：2019/07/29						                                                                                                                                             

AS5047P-Driver,for STM32 use only.

---

## 系统框图

* 外电源->内部IDO供电

* SPI进行Volatile Memory和OTP读写

* 霍尔元件->模拟后端(运放进行缓冲)->A/D

  ->ATAN(反正切，应用CORDIC数字计算算法(用加和移位等操作代替反三角乘方开方复杂操作))受到Memory控制

  ->自动增益反馈

  ->差值和动态角度误差补偿

  ->通过ABI或UVW输出

## 特点

### 总体特点

* 最高转速28k转每分钟
* 有DAEC(动态角度误差补偿)
* 高分辨率-14bit。
* 有分辨率ABI增量式接口。
* 可一次性编程永久存储

### 系统规格

* 核心分辨率14bit
* ABI设置步数可编程为100-4096步
* SPI读取角度时系统延时最高110us（Tdelay,动态补偿对象)
* 动态误差校正时间1.9us
* 采样率或spi刷新率最高247ns
* 动态角度误差(1700RPM速常下)最大0.02°
* 动态角度误差(14500RPM常速下)最大0.18°
* 动态角度误差(在25K 弧度每二次方秒角加速度下)最大0.175°
* 动态角度误差DAEC禁用，对在低速低于100rpm的时候有利于定位，可以获取0.016°rms(方均根)的低噪声
* 0°为磁铁贴在芯片表面S在芯片上半部分，N在下半部分。

接口规格

* SPI时钟周期最小100ns折合10MHZ？？？
* ABI在系统 OCF or COF error的时候被冻结位A=B=I=1。可以设置comp_h_error_en = 1comp_h_error_en = 1来启用

## 数据帧

cmd命令帧：

```
	15   	14   	[13：0]
   偶校验  0写1读   14位操作目标地址
```

读取数据帧：

		15   	14   	     [13：0]
	   偶校验  1有错误发生      14位数据

写数据帧：

```
	15   	14   	     [13：0]
   偶校验     0           14位数据
```

偶校验位bit15值是[14:0]这15bit中1的个数是否为偶数，是则该位为0，奇数该位为1凑偶。

## 数据收发

SPI使用时钟极性低，第二沿采集。

![1568213581433](C:\Users\70654\AppData\Roaming\Typora\typora-user-images\1568213581433.png)

读：发送一个cmd(bit14=1)，然后得到上次cmd所写地址的数据。所以可以在cmd后再发一个空操作。来读取cmd地址的值。

![1568213703771](C:\Users\70654\AppData\Roaming\Typora\typora-user-images\1568213703771.png)

写：cmd(bit14=0)，后再发一个数据帧，数据帧发送后得到一个该cmd地址上次的值。

## 功能

### 随时变动的寄存器

主要是读取

* ERRFL：0x0001，可以得到错误信息。
* PROG ：0x0003，OTP永久烧写相关。
* DIAAGC ：0x3FFC，一些检测信息。是否磁力过强。自动增益。
* MAG ：0x3FFD，CODDIC幅度信息
* ANGLE ：0x3FFE，无动态补偿的角度
* ANGLECOM ：0x3FFF，动态补偿和插值的角度

### 不随时变OTP寄存器

主要用来设置应用

R/W/P->读/写/编程烧写

* ZPOSM ：0x0016，零位置的高8位
* ZPOSL ：0x0017，零位置的低6位 和是否将磁场过强过弱放入系统error中触发冻结输出。
* SETTINGS1 ：0x0018，设置噪声，旋转方向，UVW_ABI选择，动态误差补偿禁用，ABI分辨率使用十进制还是二进制表示，数据选择(0->DAECANG 1->CORDICANG输出到3ffff寄存器)。PWM输出使能
* SETTINGS2 :0x0019,UVW极对数设置，ABI分辨率设置，滞后设置(根据ABI分辨率设置，看24页表)

---

## MORE

for more:

1. [ic datasheet](./datasheet/DataSheet2.pdf)
2. [pcb datasheet](./datasheet/DataSheet1.pdf)
3. [datasheet_note](./note/AS5047p文档解读.pdf)