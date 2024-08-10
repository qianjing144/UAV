#include <STC8051U.H>
#include <intrins.h>
#include <math.h>
#include "main.h"
#include "i2c.h"
#include "lps22hh.h"
#include "UART.h"
#include "sbus.h"
#include "mpu6050.h"
#include "qmc5883l.h"
#include "motor.h"
#include "pid.h"
#include "timer.h"

#define LU 45
#define HU 110

#ifdef DEBUG
#include "OLED.h"
#endif

LPS22HH_t lps22hh_obj;
MPU6050_t mpu6050_obj;
QMC5883L_t qmc5883l_obj;

float targetAngleX=0.0f;
float targetAngleY=0.0f;
float targetYaw=0.0f;
float targetHeight=0.0f;

float u1=0.0f;
float u2=0.0f;
float u3=0.0f;
float u4=0.0f;

void delayxus(unsigned int xus)	//@11.0592MHz
{
	unsigned long edata i;
	unsigned char j=0;
	while(xus--)
	{
		for(j=0;j<13;j++)
		{
			_nop_();
			_nop_();
			_nop_();
			i = 4UL;
			while (i) i--;
		}
	}
}

void delayxms(unsigned int xms)	//@11.0592MHz
{
	unsigned long edata i;
	unsigned char j=0;
	while(xms--)
	{
		for(j=0;j<13;j++)
		{
			_nop_();
			_nop_();
			i = 2763UL;
			while (i) i--;
		}
	}
}

void sys_cfg2()
{
	P_SW2=0x80;
	CKCON=0x00;
	WTST=0x00;
	
//	P0M0= 0x00;
//	P0M1= 0x00;
	    
	P0M0 = 0xff; 
	P0M1 = 0x00; 

	P1M0= 0x00;
	P1M1= 0x00;
	P2M0 = 0x18; 
	P2M1 = 0x18; 

	P3M0= 0x00;
	P3M1= 0x00;
	P4M0= 0x00;
	P4M1= 0x00;
	P5M0= 0x00;
	P5M1= 0x00;
	
//	CLKSEL &= ~0x80; //PLL 96M
	
	XOSCCR=0xE4;//启动外部晶振
	while(!(XOSCCR&1));//等待时钟稳定
	CLKDIV=0x00;//时钟不分频
	CLKSEL=0x01;//选择外部晶振
	
	CLKSEL &= ~0x80;//选择PLL的96M作为PLL的输出时钟
	USBCLK &= ~0x60;//
	USBCLK|=0x00;//PPL输入时钟为12M,选择1分频
	
	USBCLK|=0x80;//启动PLL
	
	delayxus(60);
	
	HSCLKDIV=0x00;
	CLKSEL|=0x40;
	TFPU_CLKDIV=0x00;//96M
	PWMA_CLKDIV=4;//24M
}

void hardware_init()
{
	EA=1;
	uart1_init();
	i2c_init();
	timer3_init();	
	sbus_init();
	delayxms(200);
#ifdef DEBUG
	OLED_Init();
	OLED_Clear();
#endif
	lps22hh_obj.devAddr=0x5C;
	while(lps22hh_init(&lps22hh_obj))
	{
		#ifdef DEBUG
		OLED_ShowString(0, 0, "E_LPS22HH", 16, 0);
		#endif
		return;
	}
	mpu6050_obj.devAddr=0x68;
	while(mpu6050_init(&mpu6050_obj))
	{
		#ifdef DEBUG
		OLED_ShowString(0, 0, "E_MPU6050", 16, 0);
		#endif
		return;
	}
	qmc5883l_obj.devAddr=0x0D;
	while(qmc5883l_init(&qmc5883l_obj))
	{
		#ifdef DEBUG
		OLED_ShowString(0,0, "E_QMC5883l", 16,0);
		#endif
		return;
	}
	motor_init();
	Timer4_Init();
}

unsigned char AngleXY()
{
	static float pAX=0;
	static float iAX=0;
	static float dAX=0;

	static float pGX=0;//0.5 横滚
	static float iGX=0;
	static float dGX=0;//0.3

	static float uX=0.0;
	//X�??????????????????????????????????????????????????????????????????????????
	float RTAngleX=0;//定义X轴实时角度变�??????????????????????????????????????????????????????????????????????????
	float RTGyroX=0;//定义X轴实时角速度变量

	static float pAY=-1;
	static float iAY=0;
	static float dAY=0;

	static float pGY=-0.3;//0.3最好
	static float iGY=0;
	static float dGY=0;

	static float uY=0;
	//Y�??????????????????????????????????????????????????????????????????????????
	float RTAngleY=0;//定义Y轴实时角度变�??????????????????????????????????????????????????????????????????????????
	float RTGyroY=0;//定义Y轴实时角速度变量

	if(mpu6050_getAngleGn(&mpu6050_obj, &RTAngleX, &RTAngleY,&RTGyroX,&RTGyroY))
	{
		return 1;
	}
	uX=pid_angleX(targetAngleX, RTAngleX, RTGyroX, pAX, iAX, dAX);
	uX=pid_gyroX(uX,RTGyroX,pGX,iGX,dGX);
	uY=pid_angleY(targetAngleY, RTAngleY, RTGyroY, pAY, iAY, dAY);
//	OLED_Showdecimal(0,0,uY,3,2,12,0);
	uY=pid_gyroY(uY,RTGyroY-13,pGY,iGY,dGY);
	
	//1+
	u1+=uX;
	//2+
	u2+=uX;
	//3-
	u3-=uX;
	//4-
	u4-=uX;
	
	
	//1+
	u1+=uY;
	//2-
	u2-=uY;
	//3-
	u3-=uY;
	//4+
	u4+=uY;
	
	return 0;
}

unsigned char Yaw()
{
	static float p=0;
	static float i=0;
	static float d=0;
	static float u=0;

	static unsigned char is_first_flag=1;//第一次运行标志位
	static float RTYaw=0;//定义存储实时真实的角度的变量
	static float dRTYaw=0;//存储实时相对起始点的偏航￿?????????????????????????????????????
	static float startYaw=0;//起始偏航￿?????????????????????????????????????

	if(qmc5883l_getHeading(&qmc5883l_obj, &RTYaw))
	{
		return 1;
	}
	if(is_first_flag==1)
	{
		is_first_flag=0;
		startYaw=RTYaw;//获取初始高度
	}
	dRTYaw=RTYaw-startYaw;
	u=pid_yaw(targetYaw, dRTYaw, p, i, d);
	u1+=u;
	u2-=u;
	u3+=u;
	u4-=u;
	return 0;
}

void Height()
{
	/* USER CODE BEGIN HeightTask */
	static float p=0;
	static float i=0;
	static float d=0;
	static float u=0;

	static unsigned char is_first_flag=1;
	static float StartAltitude=0;
	float RTHeight=0;
	float RTAltitude=0;
	if(lps22hh_get_alt(&lps22hh_obj,&RTAltitude)==0)
	{
		if(is_first_flag==1)
		{
			is_first_flag=0;
			StartAltitude=RTAltitude;//获取初始高度
		}
		RTHeight=RTAltitude-StartAltitude;//计算相对于起始点的高￿????????????????????????????????????????????????????????
		u=pid_height(targetHeight,RTHeight,p,i,d);//高度调整
	}
	u1+=u;
	u2+=u;
	u3+=u;
	u4+=u;
}

void rx()
{
	static unsigned char kX=0,kY=0,kYaw=0,kHeight=0;
	for(;sbus_getData(5)<800;)
	{
		motor_control(0,0,0,0);
	}
	
	/*
	 * 积分清零
	 */
	pid_setAngleXI(0);
	pid_setAngleYI(0);
	pid_setHeightI(0);
	pid_setYawI(0);
	/*
	 * 姿�?�调￿??????????????????????????
	 */
	targetAngleX=kX*sbus_getData(0);//X
	targetAngleY=kY*sbus_getData(1);//Y
	targetYaw+=kYaw*sbus_getData(3);//yaw
	if(targetYaw>360)//如果转动超过￿?????????????????????????????????????????????????????￿?????????????????????????????????????????????????????
	{
		targetYaw=0;//将目标偏航角度置0,即启动新的一￿?????????????????????????????????????????????????????
	}
	else if (targetYaw<0)//如果目标角度小于0
	{
		targetYaw=360;//启动新的￿?????????????????????????????????????????????????????￿?????????????????????????????????????????????????????
	}
	targetHeight+=kHeight*sbus_getData(2);//height
}

void u_limit()
{
	if(u1<LU)
	{
		u1=LU;
	}
	else if(u1>HU)
	{
		u1=HU;
	}
	if(u2<LU)
	{
		u2=LU;
	}
	else if(u2>HU)
	{
		u2=HU;
	}
	if(u3<LU)
	{
		u3=LU;
	}
	else if(u3>HU)
	{
		u3=HU;
	}
	if(u4<LU)
	{
		u4=LU;
	}
	else if(u4>HU)
	{
		u4=HU;
	}
}

void motor_run()
{
	motor_1(u1);
	motor_2(u2);
	motor_3(u3);
	motor_4(u4);
	u1=LU;
	u2=LU;
	u3=LU;
	u4=LU;
}

void main()
{
	sys_cfg2();
	hardware_init();
	while(1)
	{
		
	}
}


void uart2_isr(void) interrupt 8
{
	if (S2CON & 0x02)	//检测串口2发送中断
	{
		S2CON &= ~0x02;	//清除串口2发送中断请求位
	}
	if (S2CON & 0x01)	//检测串口2接收中断
	{
		RI=0;
		sbus_setRxCnt(sbus_getRxCnt()+1);
		if(sbus_getRxCnt()>=24)sbus_setRxCnt(0);
		sbus_setRx2TimeOut(5);
		S2CON &= ~0x01;	//清除串口2接收中断请求位
	}
}

void timer3_isr(void) interrupt 19
{
	if(sbus_getRx2TimeOut()>0)
	{
		sbus_setRx2TimeOut(sbus_getRx2TimeOut()-1);
		if(sbus_getRx2TimeOut()==0)
		{
			DMA_UR2R_CR=0x00;
			sbus_setRxCnt(0);
			DMA_UR2R_CR=0xA1;
		}
	}
}

void SBUS_DMA_Isr(void) interrupt 53
{
	
	if(DMA_UR2R_STA&0x01)
	{
		DMA_UR2R_STA &= ~0x01;
		sbus_inirq();
		sbus_setRxCnt(0);
		sbus_setRx2TimeOut(0);
		DMA_UR2R_CR=0xA1;
	}
}

void Timer4_Isr(void) interrupt 20
{
	AngleXY();
//	OLED_Showdecimal(0,1,u1,3,2,12,0);
//	OLED_Showdecimal(0,2,u2,3,2,12,0);
//	OLED_Showdecimal(0,3,u3,3,2,12,0);
//	OLED_Showdecimal(0,4,u4,3,2,12,0);
	Yaw();
	Height();
	rx();
	
	u_limit();
	
	motor_run();
}