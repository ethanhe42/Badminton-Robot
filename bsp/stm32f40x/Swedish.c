
/* Includes ------------------------------------------------------------------*/
#include "includes.h"
#include "math.h"
#define ACC 500
#define Acc_Limit_Enable 0
double RATE = 0.14297486;

int Swe_Acc = 6;//加速度上限
int Swe_Acc_d = 50;//减速加速度上限

//下面的坐标系是机器人的局部坐标系
double Speed_X;				// mm/s
double Speed_Y;				// mm/s
double Speed_Rotation;		// (rad/s)/100

extern double coeff_x,coeff_y,coeff_r; //雅克比矩阵化简后的系数


struct Swedish_State Swe_FL={3, -1};		//四轮的状态参数
struct Swedish_State Swe_FR={2, 1};
struct Swedish_State Swe_BL={1, -1};
struct Swedish_State Swe_BR={4, 1};

/*
 * 函数名: Swedish_test
 * 描  述: 轮子调试
 * 输  入: 轮子状态信息
 * 输  出: 无
 * 调  用: 外部调用
 */
void Swedish_test(struct Swedish_State *swe)
{
	rt_uint32_t key_value;
	double speed_pwm=50;

/*->*/swe_test:
	while(1)
	{
		LCD_Clear();
		LCD_SetXY(0,0);
		LCD_WriteString("======Swedish=======");
		LCD_SetXY(0,1);
		LCD_WriteString("1.Speed");
		LCD_SetXY(0,2);
		LCD_WriteString("2.ChangeArg");
		LCD_SetXY(0,3);
		LCD_WriteString("3.ChangePort now:");
		LCD_WriteInt((*swe).port);

		
		if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
		{
			switch(key_value)
			{
				case key1:
					goto swe_speed;
				case key2:
					Input_IntValue(&(*swe).arg,"Argument");
					break;
				case key3:
					Input_IntValue(&(*swe).port,"Port");
					break;

				case keyback:
					return;
			}
		}
		Delay_ms(10);
	}
	
/*->*/swe_speed:
	while(1)
	{
		LCD_Clear();
		LCD_SetXY(0,0);
		LCD_WriteString("======Swedish=======");
		LCD_SetXY(0,1);
		LCD_WriteString("Speed_PWM:");
		LCD_WriteDouble(PWM_GetDuty((*swe).port));
		LCD_SetXY(0,2);
		LCD_WriteString("1.SpeedUp");
		LCD_SetXY(0,3);
		LCD_WriteString("2.SpeedDown");
		LCD_SetXY(0,4);
		LCD_WriteString("3.Stop");
		
		if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
		{
			switch(key_value)
			{
				case key1:
					if(speed_pwm<100)
						speed_pwm+=1;
					break;
				case key2:
					if(speed_pwm>0)
						speed_pwm-=1;
					break;
				case key3:
					speed_pwm=50;
					break;
				case keyback:
					goto swe_test;
			}
			if(key_value == keyback)
			{
				PWM_SetDuty((*swe).port,50);
				break;
			}
			PWM_SetDuty((*swe).port,speed_pwm);
		}
		Delay_ms(10);
	}
}


/*
 * 函数名: SetSpeed
 * 描  述: 为机器人赋速度
 * 输  入: 机器人坐标系X方向速度(mm/s)，Y方向速度(mm/s)和自转速度(rad/s)/100
 * 输  出: 无
 * 调  用: 外部调用
 */

//void SetSpeed(double fl_speed,double fr_speed,double bl_speed,double br_speed)
void SetSpeed(double speed_x,double speed_y,double speed_rotation)
{
	double fl_speed;
	double fr_speed;
	double bl_speed;
	double br_speed;
	
	double fl_delta;
	double fr_delta;
	double bl_delta;
	double br_delta;
	
	static double fl_last=0;	//储存四轮的速度
	static double fr_last=0;
	static double bl_last=0;
	static double br_last=0;
	
	double delta_max;			//速度变化量最大值
	double PWM_Max;				//生成的PWM后对应的速度最大值
	
	double PWM_FL,PWM_FR,PWM_BL,PWM_BR;
//*********************anti-slip*******************//
	double speed_x_rep,speed_y_rep,speed_rep;
	double rate_x,rate_y;
	
	speed_x_rep=speed_x;
	speed_y_rep=speed_y;
	speed_rep=sqrt(pow(speed_x_rep,2)+pow(speed_y_rep,2));
	
	if(speed_rep!=0)
	{
		rate_x=speed_x_rep/speed_rep;
		rate_y=speed_y_rep/speed_rep;
	}
	else
	{
		rate_x=1;
		rate_y=1;
	}
	
	if(speed_rep-realspe>=0)
	{
		if(speed_rep-realspe>=ACC)
		{
			speed_x=(realspe+ACC)*rate_x;
			speed_y=(realspe+ACC)*rate_y;
		}
		else
		{	
			speed_x=speed_x_rep;
			speed_y=speed_y_rep;
		}	
	}
	else
	{
		if(realspe<=3000)
		{
			if(realspe<2000)
			{
				if(realspe<=100)
					speed_rep=0;
				else
					speed_rep=100;
			}
			else
			speed_rep=2000;
		}
		else 
			speed_rep=3000;
	}
	
	/****************************通过雅克比矩阵把速度分解到各个轮子******************************/
/*正方向*/
	coeff_x=coeff_y=0.70684;
	coeff_r=39.07974580567134;
	fl_speed =  coeff_x*speed_x + coeff_y*speed_y - coeff_r*speed_rotation/100 ;
	fr_speed = -coeff_x*speed_x + coeff_y*speed_y + coeff_r*speed_rotation/100 ;
	bl_speed = -coeff_x*speed_x + coeff_y*speed_y - coeff_r*speed_rotation/100 ;
	br_speed =  coeff_x*speed_x + coeff_y*speed_y + coeff_r*speed_rotation/100 ;
/*旋转45度*/
// fl_speed = speed_y - coeff_r*speed_rotation/100;
// fr_speed = speed_y + coeff_r*speed_rotation/100;
// bl_speed = speed_x - coeff_r*speed_rotation/100;
// br_speed = speed_x + coeff_r*speed_rotation/100;

	/****************************对加速度进行限制(以理论速度限制赋速度)******************************/
	
	fl_delta = fl_speed - fl_last;
	fr_delta = fr_speed - fr_last;
	bl_delta = bl_speed - bl_last;
	br_delta = br_speed - br_last;
	
	delta_max=MaxFour(Abs(fl_delta),Abs(fr_delta),Abs(bl_delta),Abs(br_delta));
	
	if((delta_max >= Swe_Acc) && (Acc_Limit_Enable==1))
	{
		fl_last += Swe_Acc*(fl_delta/delta_max);
		fr_last += Swe_Acc*(fr_delta/delta_max);
		bl_last += Swe_Acc*(bl_delta/delta_max);
		br_last += Swe_Acc*(br_delta/delta_max);
	}
	else if((-delta_max <= -Swe_Acc_d) && (Acc_Limit_Enable==1))
	{
		fl_last -= Swe_Acc_d*(fl_delta/delta_max);
		fr_last -= Swe_Acc_d*(fr_delta/delta_max);
		bl_last -= Swe_Acc_d*(bl_delta/delta_max);
		br_last -= Swe_Acc_d*(br_delta/delta_max);
	}
	else
	{
		fl_last = fl_speed;
		fr_last = fr_speed;
		bl_last = bl_speed;
		br_last = br_speed;
	}
	
	/****************************转速转换为占空比*****************************

	             PWM%=(50.0/speed_max)*speed*电机的减速比+50
	
	 *************************************************************************/	
	fl_speed = (fl_last * Swe_FL.arg)*RATE;
	fr_speed = (fr_last * Swe_FR.arg)*RATE;
	bl_speed = (bl_last * Swe_BL.arg)*RATE;
	br_speed = (br_last * Swe_BR.arg)*RATE;
	
	PWM_FL = (50.0/6000)*14*fl_speed+50;
	PWM_FR = (50.0/6000)*14*fr_speed+50;
	PWM_BL = (50.0/6000)*14*bl_speed+50;
	PWM_BR = (50.0/6000)*14*br_speed+50;
	
	/************************对最终速度再次进行限制****************************/
	
	PWM_Max=MaxFour(Abs(PWM_FL-50),Abs(PWM_FR-50),Abs(PWM_BL-50),Abs(PWM_BR-50));
	if(PWM_Max >= 48)
	{
		PWM_FL = 48*(PWM_FL-50)/PWM_Max+50;
		PWM_FR = 48*(PWM_FR-50)/PWM_Max+50;
		PWM_BL = 48*(PWM_BL-50)/PWM_Max+50;
		PWM_BR = 48*(PWM_BR-50)/PWM_Max+50;
	}
	/************************给相应的端口进行赋值******************************/

  PWM_SetDuty(Swe_FL.port,PWM_FL);
	PWM_SetDuty(Swe_FR.port,PWM_FR);
	PWM_SetDuty(Swe_BL.port,PWM_BL);
	PWM_SetDuty(Swe_BR.port,PWM_BR);
}

/**********************************END OF FILE*********************************/
