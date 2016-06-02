/**
 ******************************************************************************
 * @author  wrd_double
 * @date    2013-
 * @brief
 * @note

 ******************************************************************************
 * @attention
 ******************************************************************************
 */

#define __AR_TEST_
/* Includes ------------------------------------------------------------------*/
#include "pwm.h"
#include "fan.h"
#include "includes.h"
#include "math.h"
/* Private typedef -----------------------------------------------------------*/
/* Private dfine ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/**@attention :存储最后一次发给文林的光隔板的pwm波的占空比的值
 I */
//                   
int F_CH = 1, R_CH = 4, L_CH = 3, B_CH = 2;
int fantest=1;
static double pwm_duty[4] =
{ 20.0, 20.0, 20.0, 20.0 };

/**@attention :给涵道开到最大速度
 *
 */
void Fan_Set_Max(void)
{
	pwm_duty[F_CH - 1] = 43.0;
	pwm_duty[R_CH - 1] = 43.0;
	pwm_duty[L_CH - 1] = 43.0;
	pwm_duty[B_CH - 1] = 43.0;
	W_PWM_FUNC(pwm_duty[0], pwm_duty[1], pwm_duty[2], pwm_duty[3]);
}

/**@attention :停止涵道
 */
void Fan_Set_Min(void)
{
	pwm_duty[F_CH - 1] = 20.0;
	pwm_duty[R_CH - 1] = 20.0;
	pwm_duty[L_CH - 1] = 20.0;
	pwm_duty[B_CH - 1] = 20.0;

	W_PWM_FUNC(pwm_duty[0], pwm_duty[1], pwm_duty[2], pwm_duty[3]);
}

/**@attention :步进增加涵道的占空比 
 */
int Fan_Duty_Up(double delt)
{
	//left channel
	if (pwm_duty[F_CH - 1] + delt > 43.0)
	{
		pwm_duty[F_CH - 1] = 43.0;
	}
	else
	{
		pwm_duty[F_CH - 1] += delt;
	}

	//right channel
	if (pwm_duty[R_CH - 1] + delt > 43.0)
	{
		pwm_duty[R_CH - 1] = 43.0;
	}
	else
	{
		pwm_duty[R_CH - 1] += delt;
	}

	//front channel
	if (pwm_duty[L_CH - 1] + delt > 43.0)
	{
		pwm_duty[L_CH - 1] = 43.0;
	}
	else
	{
		pwm_duty[L_CH - 1] += delt;
	}

	//back channel
	if (pwm_duty[B_CH - 1] + delt > 43.0)
	{
		pwm_duty[B_CH - 1] = 43.0;
	}
	else
	{
		pwm_duty[B_CH - 1] += delt;
	}
	W_PWM_FUNC(pwm_duty[0], pwm_duty[1], pwm_duty[2], pwm_duty[3]);
	return 1;
}

int Fan_Duty_Dn(double delt)
{
	//left channel
	if (pwm_duty[F_CH - 1] - delt < 20.0)
	{
		pwm_duty[F_CH - 1] = 20.0;
	}
	else
	{
		pwm_duty[F_CH - 1] -= delt;
	}

	//right channel
	if (pwm_duty[R_CH - 1] - delt < 20.0)
	{
		pwm_duty[R_CH - 1] = 20.0;
	}
	else
	{
		pwm_duty[R_CH - 1] -= delt;
	}
	//front channel
	if (pwm_duty[L_CH - 1] - delt < 20.0)
	{
		pwm_duty[L_CH - 1] = 20.0;
	}
	else
	{
		pwm_duty[L_CH - 1] -= delt;
	}
	//back channel
	if (pwm_duty[B_CH - 1] - delt < 20.0)
	{
		pwm_duty[B_CH - 1] = 20.0;
	}
	else
	{
		pwm_duty[B_CH - 1] -= delt;
	}

	W_PWM_FUNC(pwm_duty[0], pwm_duty[1], pwm_duty[2], pwm_duty[3]);
	return 1;
}

void Fan_Stop(void)
{
	Fan_Set_Min();
}

/* 涵道的比率计算 100.0时是涵道最大的风速 */
/**@attention :pwm_chx range  1~4
 */
void Fan_Duty(u8 pwm_chx, double duty)
{
	assert_param(duty<=100.0);

	/* 限幅 */
	if (duty > 100.0)
	{
		pwm_duty[pwm_chx - 1] = 43.0;
		W_PWM_FUNC(pwm_duty[0], pwm_duty[1], pwm_duty[2], pwm_duty[3]);
		return;
	}
	else if (duty < 0.0)
	{
		pwm_duty[pwm_chx - 1] = 20.0;
		W_PWM_FUNC(pwm_duty[0], pwm_duty[1], pwm_duty[2], pwm_duty[3]);
		return;
	}

	pwm_duty[pwm_chx - 1] = 20.0 + duty * (43.0 - 20.0) / 100.0;
	W_PWM_FUNC(pwm_duty[0], pwm_duty[1], pwm_duty[2], pwm_duty[3]);
}

/**@attention: 返回某路涵道的值
 * 不直接使用pwm_duty 避免了越界的危险
 */
double get_fan_duty(u8 pwm_chx)
{
// 	if (pwm_chx != F_CH && pwm_chx != R_CH && pwm_chx != L_CH
// 			&& pwm_chx != B_CH)
// 	{
// 		LCD_Clear();

// 		LCD_Printf("err in [%s]", __FUNCTION__);
// 	}

	return pwm_duty[pwm_chx - 1];
}

/*
 * 函数名: Ductedfan_test
 * 描  述: 调试涵道的方向
 * 输  入: 任意一个涵道
 * 输  出: 无
 * 调  用: 外部调用
 */
void Ductedfan_test(int *CH)
{
	rt_uint32_t key_value;
	double speed_pwm = 0;

	/*->*/fan_test: while (1)
	{
		LCD_Clear();
		LCD_SetXY(0, 0);
		LCD_WriteString("======Ductedfan=======");
		LCD_SetXY(0, 1);
		LCD_WriteString("1.Speed");
		LCD_SetXY(0, 2);
		LCD_WriteString("2.ChangePort now:");
		LCD_WriteInt(*CH);

		if (rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
		{
			switch (key_value)
			{
			case key1:
				goto fan_speed;
			case key2:
				Input_IntValue(CH, "Port");
				break;
			case keyback:
				return;
			}
		}
		Delay_ms(10);
	}

	/*->*/fan_speed: while (1)
	{
		LCD_Clear();
		LCD_SetXY(0, 0);
		LCD_WriteString("======Ductedfan=======");
		LCD_SetXY(0, 1);
		LCD_WriteString("Speed_PWM:");
		LCD_WriteDouble(get_fan_duty(*CH));
		LCD_SetXY(0, 2);
		LCD_WriteString("1.SpeedUp");
		LCD_SetXY(0, 3);
		LCD_WriteString("2.SpeedDown");
		LCD_SetXY(0, 4);
		LCD_WriteString("3.Stop");

		if (rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
		{
			switch (key_value)
			{
			case key1:
				if (speed_pwm < 100)
					speed_pwm += 1;
				break;
			case key2:
				if (speed_pwm > 0)
					speed_pwm -= 1;
				break;
			case key3:
				speed_pwm = 0;
				break;
			case keyback:
				goto fan_test;
			}
			if (key_value == keyback)
			{
				Fan_Stop();
				break;
			}
			Fan_Duty(*CH, speed_pwm);
		}
		Delay_ms(10);
	}
}

/* 
 * 函数名: FanAccelerate
 * 描  述: 使用涵道风扇为机器人加速
 * 输  入: x y 方向 风扇速度
 * 输  出: 无
 * 调  用: 外部调用
 */
void FanAccelerate(double speed_x, double speed_y)
{
	double theta=0;
	double fan_y = 0, fan_x = 0;
	double error_angle;
	double Vout_A;
	double v=sqrt(speed_x*speed_x+speed_y*speed_y);
	unsigned char i,j;
static double lastpwm[4]={20,20,20,20};
	theta = atan2(speed_y, speed_x);

	error_angle = theta - GPS.radian;
	Vout_A = 1000 * AnglePID(error_angle, Pid_List[29]);
	switch ((int) (theta / (pi / 2.0)))
	{
	case 0:
		fan_y = -Vout_A;
		break;
	case 1:
		fan_x = Vout_A;
		break;
	case 2:
		fan_y = Vout_A;
		break;
	case 3:
		fan_x = -Vout_A;
		break;
	default:
		break;
	}

	theta -= pi / 4.0;
	
	fan_y += sin(theta)*v;
	fan_x += cos(theta)*v;
	
	fan_y=speed_y;
	fan_x=speed_x;
	
	if(fan_y>100.0||fan_y<-100.0) {
		fan_x*=100.0/Abs(fan_y);
		fan_y*=100.0/Abs(fan_y);
	}
	if(fan_x>100.0||fan_x<-100.0) {
		fan_y*=100.0/Abs(fan_x);
		fan_x*=100.0/Abs(fan_x);
	}
	pwm_duty[B_CH-1]= fan_y > 0 ? fan_y : 0;
	pwm_duty[F_CH-1]=fan_y > 0 ? 0 : -fan_y;
	pwm_duty[L_CH-1]=fan_x > 0 ? fan_x : 0;
	pwm_duty[R_CH-1]=fan_x > 0 ? 0 : -fan_x;

// for(i=0;i<4;i++){
// 	if(Abs(pwm_duty[i]-lastpwm[i])>0.5){
// 		for(j=0;j<4;j++) pwm_duty[j]=Abs(lastpwm[i]+0.5)/pwm_duty[j];
// }	
// }
	for(i=0;i<4;i++)	pwm_duty[i] = 20.0 + pwm_duty[i] * (43.0 - 20.0) / 100.0;
	
		W_PWM_FUNC(pwm_duty[0], pwm_duty[1], pwm_duty[2], pwm_duty[3]);
//	else j++;
}
