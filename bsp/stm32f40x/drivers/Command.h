#ifndef _COMMAND_H
#define _COMMAND_H
	#include "stm32f4xx.h"
	
	#define SA_READ(a)		(MISO_SWITCH[0]>>(a-1) & 0x01)
	#define ST_READ(a)		(MISO_SWITCH[1]>>(a-1) & 0x01)
	#define M_IO_READ(a,b) 	(IO_State[a-W_MOTOR1_OLD_ID] >> (b-1)    & 0x01)
	
	//SLAVER--GPS
	void CheckFloat(void);
	void G_M_CMD_FUNC(void);
	void S_GPS_X_FUNC(void);
	void S_GPS_Y_FUNC(void);
	void S_GPS_ANG_FUNC(void);
	void S_GYRO_C1_FUNC(void);
	void S_GYRO_C2_FUNC(void);
	void S_GYRO_RAD_FUNC(void);
	void S_ENC1_C1_FUNC(void);
	void S_ENC1_C2_FUNC(void);
	void S_ENC1_RAD_FUNC(void);
	void S_ENC1_ANG_FUNC(void);
	void S_ENC1_DIS_FUNC(void);
	void S_ENC2_C1_FUNC(void);
	void S_ENC2_C2_FUNC(void);
	void S_ENC2_RAD_FUNC(void);
	void S_ENC2_ANG_FUNC(void);
	void S_ENC2_DIS_FUNC(void);
	void R_FLAG_SET_FUNC(void);
	//SLAVER--MOTOR
	void W_MOTOR_SPEED_ANGLE_FUNC(ID_NUMDEF ID_NUM , union fi32_to_u8 SPEED_ANGLE);
	void W_MOTOR_OLD_FUNC(ID_NUMDEF ID_NUM , float rot, int16_t speed,int8_t cmd);
	void W_STEPPER_FUNC(u8 dir,u8 free,u32 step);
	
	
	void S_MOTOR1_ENC_FUNC(void);
	void S_MOTOR2_ENC_FUNC(void);
	void S_MOTOR3_ENC_FUNC(void);
	void S_MOTOR4_ENC_FUNC(void);
	void S_MOTOR5_ENC_FUNC(void);
	void S_MOTOR6_ENC_FUNC(void);
	void S_MOTOR7_ENC_FUNC(void);
	void S_MOTOR8_ENC_FUNC(void);
	//SLAVER--ISO
	void W_PWM_FUNC(double pwm0, double pwm1, double pwm2, double pwm3);
	void W_FLAG_FUNC(void);
	void S_SWITCH_FUNC(void);
	void W_MOTOR_ANGLE_FUNC(void);
	//SLAVER--CAMERA
	void S_CAMERA1_FUNC(void);
	void S_CAMERA2_FUNC(void);
	void S_CAMERA3_FUNC(void);
	void S_CAMERA4_FUNC(void);
	void S_CAMERA5_FUNC(void);
	//SLAVER--RELAY
	void W_RELAY64_FUNC(void);
	void W_RELAY8_FUNC(void);
	//SLAVER--SONIC
	void W_SONIC1_FUNC(void);
	void S_SONIC1_FUNC(void);
    void W_SONIC2_FUNC(void);
	void S_SONIC2_FUNC(void);
	
  void S_CAM1_TRANS_FUNC(void);
	void S_CAM2_TRANS_FUNC(void);
	void S_CAM3_TRANS_FUNC(void);
	void S_CAM4_TRANS_FUNC(void);
	void S_CAM5_TRANS_FUNC(void);


#endif
