/*
 * PID.h
 *
 *  Created on: Jul 13, 2024
 *      Author: qian-jing
 */

#ifndef PID_H_
#define PID_H_

float pid_angleX(float TargetAngle,float RTAngle,float RTGyro,float Kp, float Ki,float Kd);
float pid_angleY(float TargetAngle,float RTAngle,float RTGyro,float Kp, float Ki,float Kd);
float pid_yaw(float TargetYaw,float RTYaw,float Kp,float Ki,float Kd);
float pid_height(float TargetHeight,float RTHeight,float Kp,float Ki,float Kd);
void pid_setAngleXI(float val);
void pid_setAngleYI(float val);
void pid_setHeightI(float val);
void pid_setYawI(float val);

#endif /* PID_H_ */
