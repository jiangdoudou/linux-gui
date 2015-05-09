		 /*------------------------------------------------------------------*/
/* --- STC MCU International Limited -------------------------------*/
/* --- STC 1T Series MCU RC Demo -----------------------------------*/
/* --- Mobile: (86)13922805190 -------------------------------------*/
/* --- Fax: 86-755-82944243 ----------------------------------------*/
/* --- Tel: 86-755-82948412 ----------------------------------------*/
/* --- Web: www.STCMCU.com -----------------------------------------*/
/* If you want to use the program or the program referenced in the  */
/* article, please specify in which data and procedures from STC    */
/*------------------------------------------------------------------*/


/*************	功能说明	**************
红外接收程序。适用于市场上用量最大的HT6121/6122及其兼容IC的编码。
对于用户码与User_code定义不同的遥控器，程序会将用户码一起从串口输出。
使用模拟串口发送监控显示编码，显示内容为ASCII码和中文。
本接收程序基于状态机的方式，占用CPU的时间非常少。
HEX文件在本目录的/list里面。
******************************************/


/*************	用户系统配置	**************/
#define MAIN_Fosc		11059200L
//#define MAIN_Fosc		12000000L	//定义主时钟, 模拟串口和红外接收会自动适应。5~36MHZ

#define D_TIMER0		125			//选择定时器时间, us, 红外接收要求在60us~250us之间

#define	User_code		0xFD02		//定义红外接收用户码


/*************	以下宏定义用户请勿修改	**************/
#include	"reg51.H"
#include "CTYPE.H"
#include "STRING.H"

#include<stdio.h>  
#include<stdarg.h> 
#define	uchar	unsigned char
#define uint	unsigned int

#define freq_base			(MAIN_Fosc / 1200)
#define Timer0_Reload		(65536 - (D_TIMER0 * freq_base / 10000))




/*************	本地常量声明	**************/



/*************	本地变量声明	**************/
//sbit	P_TXD1 = P3^1;		//定义模拟串口发送脚，打印信息用
sfr AUXR = 0x8E;
sbit RXB = P3^0;                        //define UART TX/RX port
sbit TXB = P3^1;
sbit IR_LED = P3^4;					 //IR_LED
sbit	P_IR_RX = P3^5;		//定义红外接收输入端口
sbit 	PWR_EN = P3^2; //jiangdou  for LED
bit		P_IR_RX_temp;		//Last sample
bit		B_IR_Sync;			//已收到同步标志
uchar	IR_SampleCnt;		//采样计数
uchar	IR_BitCnt;			//编码位数
uchar	IR_UserH;			//用户码(地址)高字节
uchar	IR_UserL;			//用户码(地址)低字节
uchar	IR_data;			//数据原码
uchar	IR_DataShit;		//数据反码

bit		B_IrUserErr;		//User code error flag
bit		B_IR_Press;			//Key press flag,include repeat key.
uchar	IR_code;			//IR code	红外键码



typedef bit BOOL;
typedef unsigned char BYTE;
typedef unsigned int WORD;

BYTE TBUF,RBUF;
BYTE TDAT,RDAT;
BYTE TCNT,RCNT;
BYTE TBIT,RBIT;
BOOL TING,RING;
BOOL TEND,REND;

void UART_INIT();

BYTE t, r, i;
uint rx_flag = 0, size =0;
BYTE buf[16];
BYTE read_buff[16];

BYTE buffer[16];
BYTE *bb;
BYTE *dou ="dou:";
BYTE *key_buf;
unsigned long key_id;
/*************	本地函数声明	**************/
void InitTimer(void);
void UartInit(void);
void UART_INIT();
 BYTE Uart_Read(void);
void Uart_Send(BYTE txd);
void PrintString(unsigned char code *puts);
//int StrToInt(char *str);

/********************* 主函数 *************************/

  
sfr WDT_CONTR   = 0xc1;     //看门狗控制寄存器
   //from rk3288 "dou:29307while...while...while...while...while...while..."
void main(void)
{
	unsigned char *pp;
	UartInit();			//初始化Timer1
	InitTimer();		//初始化Timer0

	UART_INIT();
	//PrintString("****** STC系列MCU红外接收程序 2010-12-10 ******\r\n");	//上电后串口发送一条提示信息

	EA  = 1;//打开所有中断
	PWR_EN = 1; //PWR_EN for rk3288 
	IR_LED = 1;

	 WDT_CONTR = 0x35;       //看门狗定时器溢出时间计算公式: (12 * 32768 * PS) / FOSC (秒)
                            //设置看门狗定时器分频数为32,溢出时间如下:
                            //11.0592M : 1.14s
                            //18.432M  : 0.68s
                            //20M      : 0.63s
	WDT_CONTR |= 0x20;      //启动看门狗
	 PrintString("****** STC-MCU 2015-05-06,by jiangdou qq:344283973********\r\n");
	while(1)
	{
		//LED = 1;
		WDT_CONTR = 0x35;//喂狗
		if(B_IR_Press)		//有IR键按下
		{
			
			//IR_LED = !IR_LED;
			if(IR_code == 0x45)//system shutdown rk3288!!!!		 //11.0592M
			{
				PWR_EN = !PWR_EN;
				IR_LED = !IR_LED;
			
			}
			B_IR_Press = 0;		//清除IR键按下标志
		}

//###################### ++for uart
#if 1
		if (REND)
        {
            REND = 0;
			
            
			if(RBUF == '#'){ //"#" # 表示结束位	   //dou:12345678#
				r = 0;
				rx_flag=1; //RX标志
			}else{
				buf[r++ & 0x0f] = RBUF;
				size = r;//接收的数据长度
			}
        }										//关机命令"dou:a"
		if(rx_flag == 1){ //r > 16
			rx_flag = 0;
			memset(read_buff, "", sizeof(read_buff));
			strcpy(read_buff, buf);
			memset(buf, "", sizeof(buf));
//			for (i=0;i<size;i++)
//				{
//					buffer[i]=*pp++;
//					Uart_Send(read_buff[i]); //发送接收的数据！！// dou:12345678
//				}
			PrintString("\r\n");
			
			bb = read_buff;
			pp = strstr(bb, "dou:shut");//关机命令"dou:shut"
			if(pp != NULL){
						PWR_EN = 0;	//RK3288关机命令
						IR_LED = !IR_LED;
				}
			pp = strstr(bb, "dou:");//"dou:"为关键字
			if(pp != NULL){
				PrintString("dou:2135");//发送KEY_ID
				
				
//				pp = strstr(bb, ":");//ok
//				
//				pp = pp +1;
//				for (i=0;i<(size - 5);i++)
//				{
//					buffer[i]=*pp++;
//					Uart_Send(buffer[i]);// = 12345678	//12345678
					//if(pp == '#')
					//	break;
//				}

				
				PrintString("\r\n");
			}//endif(pp != NULL){
		}//if(r == 0x0f){ //r > 16
#else
		PrintString("\r\n");
#endif
//####################### ++for uart
	}
}

 
void PrintString(unsigned char code *puts)		//发送一串字符串
{
    for (; *puts != 0;	puts++)  Uart_Send(*puts); 	//遇到停止符0结束
}

/*
int StrToInt(char *str)
{
	 unsigned long value  = 0;
	 unsigned long sign   = 1;
	 unsigned long result = 0;
	 if(NULL == str)
	 {
		return -1;
	 }
	 if('-' == *str)
	 {
		  sign = -1;
		  str++;
	 }
	 while(*str)
	 {
		  value = value * 10 + *str - '0';
		  str++;
	 }
	 result = sign * value;
	 return result;
}

*/

//###############################
BYTE Uart_Read(void)
{
    while(!REND);
    REND = 0;
    return RBUF;
}

//###############################
void Uart_Send(BYTE txd)
{
	while (!TEND);
    TEND = 0;
    TBUF = txd;
    TING = 1;

}
//###############################



/******************** 红外采样时间宏定义, 用户不要随意修改	*******************/

#if ((D_TIMER0 <= 250) && (D_TIMER0 >= 60))
	#define	D_IR_sample			D_TIMER0		//定义采样时间，在60us~250us之间
#endif

#define D_IR_SYNC_MAX		(15000/D_IR_sample)	//SYNC max time
#define D_IR_SYNC_MIN		(9700 /D_IR_sample)	//SYNC min time
#define D_IR_SYNC_DIVIDE	(12375/D_IR_sample)	//decide data 0 or 1
#define D_IR_DATA_MAX		(3000 /D_IR_sample)	//data max time
#define D_IR_DATA_MIN		(600  /D_IR_sample)	//data min time
#define D_IR_DATA_DIVIDE	(1687 /D_IR_sample)	//decide data 0 or 1
#define D_IR_BIT_NUMBER		32					//bit number

//*******************************************************************************************
//**************************** IR RECEIVE MODULE ********************************************

void IR_RX_HT6121(void)
{
	uchar	SampleTime;

	IR_SampleCnt++;							//Sample + 1

	F0 = P_IR_RX_temp;						//Save Last sample status
	P_IR_RX_temp = P_IR_RX;					//Read current status
	if(F0 && !P_IR_RX_temp)					//Last sample is high，and current sample is low, so is fall edge
	{
		SampleTime = IR_SampleCnt;			//get the sample time
		IR_SampleCnt = 0;					//Clear the sample counter

			 if(SampleTime > D_IR_SYNC_MAX)		B_IR_Sync = 0;	//large the Maxim SYNC time, then error
		else if(SampleTime >= D_IR_SYNC_MIN)					//SYNC
		{
			if(SampleTime >= D_IR_SYNC_DIVIDE)
			{
				B_IR_Sync = 1;					//has received SYNC
				IR_BitCnt = D_IR_BIT_NUMBER;	//Load bit number
			}
		}
		else if(B_IR_Sync)						//has received SYNC
		{
			if(SampleTime > D_IR_DATA_MAX)		B_IR_Sync=0;	//data samlpe time to large
			else
			{
				IR_DataShit >>= 1;					//data shift right 1 bit
				if(SampleTime >= D_IR_DATA_DIVIDE)	IR_DataShit |= 0x80;	//devide data 0 or 1
				if(--IR_BitCnt == 0)				//bit number is over?
				{
					B_IR_Sync = 0;					//Clear SYNC
					if(~IR_DataShit == IR_data)		//判断数据正反码
					{
						if((IR_UserH == (User_code / 256)) &&
							IR_UserL == (User_code % 256))
								B_IrUserErr = 0;	//User code is righe
						else	B_IrUserErr = 1;	//user code is wrong
							
						IR_code      = IR_data;
						B_IR_Press   = 1;			//数据有效
					}
				}
				else if((IR_BitCnt & 7)== 0)		//one byte receive
				{
					IR_UserL = IR_UserH;			//Save the User code high byte
					IR_UserH = IR_data;				//Save the User code low byte
					IR_data  = IR_DataShit;			//Save the IR data byte
				}
			}
		}
	}
}

//#include"15F104.H"
//sfr	T2H  = 0xD6;
//sfr	T2L  = 0xD7;
sfr INT_CLKO = 0x8F;
sfr	T2H  = 0xD6;
sfr	T2L  = 0xD7;
sfr IE2  = 0xAF;
/**************** Timer初始化函数 ******************************/
void InitTimer(void)
{
 
	AUXR &=  ~(1<<4);		// Timer2 停止运行
	//T2H = Timer0_Reload / 256;	// 数据位
	//T2L = Timer0_Reload % 256;	// 数据位
	T2L = 0x9A;		//设置定时初值
	T2H = 0xFA;
	//IE2  |=  (1<<2);		// 允许Timer2中断
	AUXR |=  (1<<2);		// 1T
	AUXR |=  0x10;	//Timer2 开始运行
	 IE2=0x04;       //开启定时器2中断允许

//	EA  = 1;
}


/********************** Timer0中断函数************************/
//void timer0 (void) interrupt 1
void timer1 (void) interrupt 12 
{
	
	IR_RX_HT6121();
	
}


/********************** 模拟串口相关函数************************/
#define BAUD  0xFE80                  // 9600bps @ 11.0592MHz

void UartInit(void)		// 9600bps @ 11.0592MHz
{
//	SCON = 0x50;		//8位数据,可变波特率
//	AUXR = 0x40;		//定时器1时钟为Fosc,即1T B7 B6 12分频
	//AUXR &= 0xFE;		//串口1选择定时器1为波特率发生器
//	TMOD &= 0x0F;		//设定定时器1为16位自动重装方式
//	TL1 = 0xE8;		//设定定时初值
//	TH1 = 0xFF;		//设定定时初值
//	ET1 = 1;		//使能定时器1中断
//	TR1 = 1;		//启动定时器1
	TMOD = 0x00;                        //timer0 in 16-bit auto reload mode
    AUXR = 0x80;                        //timer0 working at 1T mode
    TL0 = BAUD;
    TH0 = BAUD>>8;                      //initial timer0 and set reload value
    TR0 = 1;                            //tiemr0 start running
    ET0 = 1;                            //enable timer0 interrupt
    PT0 = 1;                            //improve timer0 interrupt priority
 //   EA = 1; 
}

/********************** Timer1中断函数************************/
//void tm1() interrupt 3 using 1
void timer0() interrupt 1 using 1
{
    if (RING)
    {
        if (--RCNT == 0)
        {
            RCNT = 3;                   //reset send baudrate counter
            if (--RBIT == 0)
            {
                RBUF = RDAT;            //save the data to RBUF
                RING = 0;               //stop receive
                REND = 1;               //set receive completed flag
            }
            else
            {
                RDAT >>= 1;
                if (RXB) RDAT |= 0x80;  //shift RX data to RX buffer
            }
        }
    }
    else if (!RXB)
    {
        RING = 1;                       //set start receive flag
        RCNT = 4;                       //initial receive baudrate counter
        RBIT = 9;                       //initial receive bit number (8 data bits + 1 stop bit)
    }

    if (--TCNT == 0)
    {
        TCNT = 3;                       //reset send baudrate counter
        if (TING)                       //judge whether sending
        {
            if (TBIT == 0)
            {
                TXB = 0;                //send start bit
                TDAT = TBUF;            //load data from TBUF to TDAT
                TBIT = 9;               //initial send bit number (8 data bits + 1 stop bit)
            }
            else
            {
                TDAT >>= 1;             //shift data to CY
                if (--TBIT == 0)
                {
                    TXB = 1;
                    TING = 0;           //stop send
                    TEND = 1;           //set send completed flag
                }
                else
                {
                    TXB = CY;           //write CY to TX port
                }
            }
        }
    }
}

//-----------------------------------------
//initial UART module variable

void UART_INIT()
{
    TING = 0;
    RING = 0;
    TEND = 1;
    REND = 0;
    TCNT = 0;
    RCNT = 0;
}

