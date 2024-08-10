/*
 * PID.c
 *
 *  Created on: Jul 13, 2024
 *      Author: qian-jing
 */

float Angle_X_I=0;	//存储X相差角度的累计值
float Gyro_X_I=0;
float Angle_Y_I=0;	//存储Y相差角度的累计值
float Gyro_Y_I=0;
float Height_I	=0;	//存储相差高度的累计值
float Yaw_I		=0;	//定义存储偏航角度累计的变量

float pid_angleX(float TargetAngle,float RTAngle,float RTGyro,float Kp, float Ki,float Kd)
{
	float Angle=0;
	Angle=RTAngle-TargetAngle;//获取目标角度和目前角度的差值
	Angle_X_I+=Angle;		//角度积分累积
	if(Angle_X_I>30)
	{
		Angle_X_I=30;
	}
	return Kp*Angle-Ki*Angle_X_I+Kd*RTGyro;
}
float pid_gyroX(float TargetGyro, float RTGyro,float Kp,float Ki,float Kd)
{
	float Gyro=0;
	static float PastGyro=0;
	float Gyro_D=0;
	Gyro=RTGyro-TargetGyro;
	Gyro_X_I+=Gyro;
	if(Gyro_X_I>30)
	{
		Gyro_X_I=30;
	}
	Gyro_D=RTGyro-PastGyro;
	PastGyro=Gyro;
	return Kp*Gyro-Ki*Angle_X_I+Kd*Gyro_D;
}
float pid_angleY(float TargetAngle,float RTAngle,float RTGyro,float Kp, float Ki,float Kd)
{
	float Angle=0;						//定义存储当前角度的变量
	Angle=RTAngle-TargetAngle;//获取目标角度和目前角度的差值
	Angle_Y_I+=Angle;		//角度积分累计
	if(Angle_Y_I>30)
	{
		Angle_Y_I=30;
	}
	return Kp*Angle-Ki*Angle_Y_I+Kd*RTGyro;	//返回调整值 double类型
}
float pid_gyroY(float TargetGyro, float RTGyro,float Kp,float Ki,float Kd)
{
	float Gyro=0;
	static float PastGyro=0;
	float Gyro_D=0;
	Gyro=RTGyro-TargetGyro;
	Gyro_Y_I+=Gyro;
	if(Gyro_Y_I>30)
	{
		Gyro_Y_I=30;
	}
	Gyro_D=RTGyro-PastGyro;
	PastGyro=Gyro;
	return Kp*Gyro-Ki*Angle_Y_I+Kd*Gyro_D;
}
float pid_yaw(float TargetYaw,float RTYaw,float Kp,float Ki,float Kd)
{
	float Yaw=0;													//定义存储当前偏航角度的变量
	static float PastYaw=0;						//定义存储上一次获得的偏航角度的变量
	float Yaw_D=0;											//定义存储偏航角度变化速度的变量
	Yaw=RTYaw-TargetYaw;
	Yaw_D=RTYaw-PastYaw;							//获取目标角度与设定偏航角度的差
	PastYaw=Yaw;												//记录这一次的变化角度,将在下一次的时候作为上一次的偏航角度使用,用于计算偏航角度的变化速度
	Yaw_I+=Yaw;											//累计误差
	return Kp*Yaw+Ki*Yaw_I+Kd*Yaw_D;//返回调整值 double类型
}
float pid_height(float TargetHeight,float RTHeight,float Kp,float Ki,float Kd)
{
	float Height=0;					//定义存储当前角度的变量
	static float PastHeight=0;			//定义存储上一次获得的高度的变量
	float Height_D=0;				//定义存储高度变化速度的变量
	Height=RTHeight-TargetHeight;								//计算相差高度
	Height_D=Height-PastHeight;									//计算高度变化量
	PastHeight=Height;													//记录这一次的变化量,将在下一次的时候作为上一次的高度使用,用于计算高度的变化速度
	Height_I+=Height;									//累计相差角度

	return Kp*Height+Ki*Height_I+Kd*Height_D;//返回调整值 double类型
}
void pid_setAngleXI(float val)
{
	Angle_X_I=val;
}
void pid_setAngleYI(float val)
{
	Angle_Y_I=val;
}
void pid_setHeightI(float val)
{
	Height_I=val;
}
void pid_setYawI(float val)
{
	Yaw_I=val;
}
