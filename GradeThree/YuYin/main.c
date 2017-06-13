/***************************飞音云电子****************************
**  工程名称：YS-V0.7语音识别模块驱动程序
**	CPU: STC11L08XE
**	晶振：22.1184MHZ
**	波特率：9600 bit/S
**	配套产品信息：YS-V0.7语音识别开发板
**                http://yuesheng001.taobao.com
**  作者：zdings
**  联系：751956552@qq.com
**  修改日期：2013.9.13
**  说明：口令模式： 即每次识别时都需要说“小杰”这个口令 ，才能够进行下一级的识别
/***************************飞音云电子******************************/
#include "config.h"
/************************************************************************************/
//	nAsrStatus 用来在main主程序中表示程序运行的状态，不是LD3320芯片内部的状态寄存器
//	LD_ASR_NONE:		表示没有在作ASR识别
//	LD_ASR_RUNING：		表示LD3320正在作ASR识别中
//	LD_ASR_FOUNDOK:		表示一次识别流程结束后，有一个识别结果
//	LD_ASR_FOUNDZERO:	表示一次识别流程结束后，没有识别结果
//	LD_ASR_ERROR:		表示一次识别流程中LD3320芯片内部出现不正确的状态
/***********************************************************************************/
uint8 idata nAsrStatus = 0;
void MCU_init();
void ProcessInt0(); //识别处理函数
void delay(unsigned long uldata);
void User_handle(uint8 dat);//用户执行操作函数
void Delay200ms();
void Led_test(void);//单片机工作指示
uint8_t G0_flag = DISABLE; //运行标志，ENABLE:运行。DISABLE:禁止运行
sbit LED = P4 ^ 2; //信号指示灯


uint8 DZKD = 0X00  ; //该命令码0x00用户不可进行修改。一级命令电子科技大学 喊出这句口令程序开始识别别的
uint8 KC=	0x01;		//开车
uint8 KWS=	0x02;	//结束命令 科沃斯 喊出后停车并且需要再喊电子科技大学才开始识别命令
uint8 TC= 0x03	;		//停车
uint8 ZZ =0x04	;		//左转
uint8 YZ =0x05	;		//右转
uint8 DAOCHE =0x06;			//倒车
uint8 JIASU= 0x07;			//加速
uint8 JIANSU=	0x08;			//减速
uint8 ERROR =	0x09 ;
uint8 KouLing =	0x10 ;

/***********************************************************
* 名    称： void  main(void)
* 功    能： 主函数	程序入口
* 入口参数：
* 出口参数：
* 说    明：
* 调用方法：
**********************************************************/
void  main(void)
{
    uint8 idata nAsrRes;
    uint8 i = 0;
    Led_test();
    MCU_init();
    LD_Reset();
    UartIni(); /*串口初始化*/
    nAsrStatus = LD_ASR_NONE;		//	初始状态：没有在作ASR


    while (1)
    {
        switch (nAsrStatus)
        {
            case LD_ASR_RUNING:
            case LD_ASR_ERROR:
                break;
            case LD_ASR_NONE:
                {
                    nAsrStatus = LD_ASR_RUNING;
                    if (RunASR() ==
                        0)	/*	启动一次ASR识别流程：ASR初始化，ASR添加关键词语，启动ASR运算*/
                    {
                        nAsrStatus = LD_ASR_ERROR;
                    }
                    break;
                }
            case LD_ASR_FOUNDOK: /*	一次ASR识别流程结束，去取ASR识别结果*/
                {
                    nAsrRes = LD_GetResult();		/*获取结果*/
                    User_handle(nAsrRes);//用户执行函数
                    nAsrStatus = LD_ASR_NONE;
                    break;
                }
            case LD_ASR_FOUNDZERO:
            default:
                {
                    nAsrStatus = LD_ASR_NONE;
                    break;
                }
        }// switch
    }// while

}
/***********************************************************
* 名    称： 	 LED灯测试
* 功    能： 单片机是否工作指示
* 入口参数： 无
* 出口参数：无
* 说    明：
**********************************************************/
void Led_test(void)
{
    LED = ~ LED;
    Delay200ms();
    LED = ~ LED;
    Delay200ms();
    LED = ~ LED;
    Delay200ms();
    LED = ~ LED;
    Delay200ms();
    LED = ~ LED;
    Delay200ms();
    LED = ~ LED;
}
/***********************************************************
* 名    称： void MCU_init()
* 功    能： 单片机初始化
* 入口参数：
* 出口参数：
* 说    明：
* 调用方法：
**********************************************************/
void MCU_init()
{
    P0 = 0xff;
    P1 = 0xff;
    P2 = 0xff;
    P3 = 0xff;
    P4 = 0xff;


    LD_MODE = 0;		//	设置MD管脚为低，并行模式读写
    IE0 = 1;
    EX0 = 1;
    EA = 1;
}
/***********************************************************
* 名    称：	延时函数
* 功    能：
* 入口参数：
* 出口参数：
* 说    明：
* 调用方法：
**********************************************************/
void Delay200us()		//@22.1184MHz
{
    unsigned char i, j;
    _nop_();
    _nop_();
    i = 5;
    j = 73;
    do
    {
        while (--j);
    }
    while (--i);
}

void  delay(unsigned long uldata)
{
    unsigned int j  =  0;
    unsigned int g  =  0;
    while (uldata--)
        Delay200us();
}

void Delay200ms()		//@22.1184MHz
{
    unsigned char i, j, k;

    i = 17;
    j = 208;
    k = 27;
    do
    {
        do
        {
            while (--k);
        }
        while (--j);
    }
    while (--i);
}

/***********************************************************
* 名    称： 中断处理函数
* 功    能：
* 入口参数：
* 出口参数：
* 说    明：
* 调用方法：
**********************************************************/
void ExtInt0Handler(void) interrupt 0
{
    ProcessInt0();
}
/***********************************************************
* 名    称：用户执行函数
* 功    能：识别成功后，执行动作可在此进行修改
* 入口参数： 无
* 出口参数：无
* 说    明：
**********************************************************/
void 	User_handle(uint8 dat)
{
	int i;
    //UARTSendByte(dat);//串口识别码（十六进制）
    if (0 == dat)
    {
        G0_flag = ENABLE;
        LED = 0;
        UARTSendByte(DZKD);
        UARTSendByte(0x0d);
        UARTSendByte(0x0a);
		PrintCom("准备开始\r\n"); /*text.....*/
    }
    else if (ENABLE == G0_flag)
    {
        //G0_flag=DISABLE;
        LED = 1;
        switch (dat)		  /*对结果执行相关操作,客户可删除Printcom 串口输出语句替换为其他需要控制的代码*/
        {
            case CODE_KWS:			/*命令“科沃斯”*/
                UARTSendByte(KWS);
                UARTSendByte(0x0d);
                UARTSendByte(0x0a);
                G0_flag = DISABLE;
				PrintCom("结束了\r\n");
                break;

            case CODE_KC:	 /*命令“开车”*/
                UARTSendByte(KC);
                UARTSendByte(0x0d);
                UARTSendByte(0x0a);
				PrintCom("跑车\r\n");
                break;

            case CODE_TC:		/*命令“停车”*/
                UARTSendByte(TC);
                UARTSendByte(0x0d);
                UARTSendByte(0x0a);
				PrintCom("不走\r\n");
                break;

            case CODE_ZZ:		/*命令“左转”*/
                UARTSendByte(ZZ);
                UARTSendByte(0x0d);
                UARTSendByte(0x0a);
				PrintCom("左转\r\n");
                break;

            case CODE_YZ:		/*命令“右转”*/
                UARTSendByte(YZ);
                UARTSendByte(0x0d);
                UARTSendByte(0x0a);
				PrintCom("右拐\r\n");
                break;

            case CODE_JIASU:		/*命令“加速”*/
                UARTSendByte(JIASU);
                UARTSendByte(0x0d);
                UARTSendByte(0x0a);
				PrintCom("加速\r\n");
                break;

		   case CODE_DAOCHE:		/*命令“倒车”*/
                UARTSendByte(DAOCHE);
                UARTSendByte(0x0d);
                UARTSendByte(0x0a);
				PrintCom("后退\r\n");
                break;

            case CODE_JIANSU:		/*命令“减速”*/
                UARTSendByte(JIANSU);
                UARTSendByte(0x0d);
                UARTSendByte(0x0a);
                PrintCom("慢点\r\n");
                break;

            case CODE_ERROR:		/*命令“错误”*/
			    i=i+1;
                PrintCom("错误指令\r\n"); /*text.....*/
				UARTSendByte(ERROR);
                UARTSendByte(0x0d);
                UARTSendByte(0x0a);
                //PrintCom("\r\n"); /*text.....*/
                break;

            default:
                PrintCom("请重新识别发口令\r\n"); /*text.....*/
                break;
        }
    }
    else
    {
				UARTSendByte(KouLing);
                UARTSendByte(0x0d);
                UARTSendByte(0x0a);
        PrintCom("请说出一级口令\r\n"); /*text.....*/
    }
}
