/**********************************************************************************
** 红龙407开发板
** 论坛：bbs.openmcu.com
** 旺宝：www.openmcu.com
** 邮箱：support@openmcu.com

** 实验名：TIM测试
** KEIL MDK-ARM Standard Version：4.23
** 固件库(ST) Version: V1.0.0
** 使用外设: TIM
**********************************************************************************/
测试说明：TIM pwm例程
    编译并下载程序。

测试结果：
    示波器测量管脚：PA8 (TIM1 channel 1) 和 PB11(TIM1 channel 1N), 可以看到管脚输出占空比 50%;
                    PE11(TIM1 channel 2) 和 PB12(TIM1 channel 2N), 可以看到管脚输出占空比 37.5%;
                    PA9 (TIM1 channel 3) 和 PB13(TIM1 channel 3N), 可以看到管脚输出占空比 25%;
                    PA10(TIM1 channel 4) 可以看到管脚输出占空比 12.5%;
 






版本更新备注
    当前版本V1.0
    日期：2013.4.2

