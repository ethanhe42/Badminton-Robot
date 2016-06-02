#include "t_catching.h"
#include "includes.h"
#include "math.h"
#include "m_cock.h"
#define EFFECTIVE_RADIUS 1500 /*有效半径*/
/*设置速度*/
#define SPEED_A 3000
#define SPEED_B 3000
#define SPEED_C 3000
/*设度倍率*/
#define MULTIPLY_A 1 //(20/1)
#define MULTIPLY_B 1 //(10/1)
// #define MULTIPLY_C 1 //(10/1)

ALIGN(RT_ALIGN_SIZE)
static char thread_catch_stack[1024];
struct rt_thread thread_catch;

double Theta[2];
extern double length[3]; 

struct uart_data
{
/*	axis x            axis x              axis x*/    
	double x;         double y;           double z; /*Coordinate component*/
	double vx;        double vy;          double vz; /*velocity component*/
	double ax;        double ay;          double az;  /*component of acceleration*/
}uart_data;

/*实际上有两个电机控制的自由度，如果要添加3个自由度，除了把注释的去了，还得有算法*/

//计算获得的角度，并测试输入
struct catch_data
{
	double angle_A;
	double angle_B;
	double angle_C;
}catch_data;

//电机真正处理量position
struct pos_speed
{
	double pos_A,        pos_B,            pos_C;
	int      v_A,          v_B,              v_C;       
}pos_speed;

/*
 * 函数名: Data_Processing
 * 描  述: 处理Kinect数据得到position
 * 输  入: 无
 * 输  出: 无
 * 调  用: 外部调用
 */


void LookIt(void)
{
	rt_uint32_t key_value;
//	int i;
	
	while(1)
	{		
		LCD_Clear();
		LCD_SetXY(0,0);LCD_Printf("1.AngleSwing %d",AngleSwing);
		LCD_SetXY(0,1);LCD_Printf("2.AngleHit   %d",AngleHit);
		LCD_SetXY(0,2);LCD_Printf("3.pos_A      %f",pos_speed.pos_A);
		LCD_SetXY(0,3);LCD_Printf("4.pos_B      %f",pos_speed.pos_B);
		
		if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
		{
            if(key_value == keyback)
            {
                return ;
            }
		}
		Delay_ms(10);
	}
}

void Rec_Kin()
{
	double X[3]={1,0,430-370};
	getIntersection(X);
	Point2theta(X,Theta);
	catch_data.angle_B = Theta[0];
	catch_data.angle_A = Theta[1];
	pos_speed.pos_A = 180*catch_data.angle_A * MULTIPLY_A / pi / 360;
	pos_speed.pos_B = 180*catch_data.angle_B * MULTIPLY_B / pi / 360;
	//pos_speed.pos_C = 180*catch_data.angle_C * MULTIPLY_C / pi / 360;
}

void Data_Processing(void)
{
	double dim[3] = {13,37,44};//  0 vertical 1 horizon 2 incline
	//catch_data.angle_B=acos((dim[0]-uart_data.z)/dim[2]);
	//uart_data.y=(uart_data.y/fabs(uart_data.y))*sqrt(dim[1]*dim[1]+pow(dim[2]*sin(catch_data.angle_B),2));
	//catch_data.angle_A=atan2(uart_data.y,uart_data.x);
  catch_data.angle_A = AngleHit;
  catch_data.angle_B = AngleSwing;
	pos_speed.pos_A = catch_data.angle_A * MULTIPLY_A / 360;
	pos_speed.pos_B = catch_data.angle_B * MULTIPLY_B / 360;
	//pos_speed.pos_C = 180*catch_data.angle_C * MULTIPLY_C / pi / 360;
}

/*
 * 函数名: Data_Pro_Text
 * 描  述: 处理Kinect数据得到position，测试使用
 * 输  入: 无
 * 输  出: 无
 * 调  用: 外部调用
 */
void Data_Pro_Text(void)
{
	pos_speed.pos_A = catch_data.angle_A * MULTIPLY_A / 360;
	pos_speed.pos_B = catch_data.angle_B * MULTIPLY_B / 360;
// 	pos_speed.pos_C = catch_data.angle_C * MULTIPLY_C / 360;
}

/*
 * 函数名: Clear_Data
 * 描  述: 初始化数据
 * 输  入: 无
 * 输  出: 无
 * 调  用: 外部调用
 */
void Clear_Data(void)
{
	uart_data.x = 0;             uart_data.y = 0;               uart_data.z = 0;
	uart_data.vx = 0;            uart_data.vy = 0;              uart_data.vz = 0;
	uart_data.ax = 0;            uart_data.ay = 0;              uart_data.az = 0;
	catch_data.angle_A = 0;      catch_data.angle_B = 0;        catch_data.angle_C = 0;
	pos_speed.pos_A = 0;         pos_speed.pos_B = 0;           pos_speed.pos_C = 0;
	pos_speed.v_A = 0;           pos_speed.v_B = 0;             pos_speed.v_B = 0;
}

/*
 * 函数名: RecKin
 * 描  述: 测试 Kinect 接受
 * 输  入: 无
 * 输  出: 无
 * 调  用: 外部调用
 */
void RecKin_Print(void)
{
	rt_uint32_t key_value;
	
	while(1)
	{
		LCD_Clear();
		LCD_SetXY(0,0);
		LCD_WriteString("========Receiving========");
		LCD_SetXY(0,1);
		LCD_Printf("x = %d",(int)uart_data.x);
		LCD_SetXY(0,2);
		LCD_Printf("y = %d",(int)uart_data.y);
		LCD_SetXY(0,3);
		LCD_Printf("z = %d",(int)uart_data.z);
		LCD_SetXY(0,4);
		LCD_Printf("Now Angle:%g  %g  %g",Motor_Pos[W_MOTOR1_OLD_ID-W_MOTOR1_OLD_ID]*360,\
							Motor_Pos[W_MOTOR2_OLD_ID-W_MOTOR1_OLD_ID]*360,Motor_Pos[W_MOTOR3_OLD_ID-W_MOTOR1_OLD_ID]*360);
		LCD_SetXY(0,5);
		LCD_Printf("1.STOP %d %d %d",(int)(Motor_RealAim[W_MOTOR1_OLD_ID-W_MOTOR1_OLD_ID]*360/1000),\
							(int)(Motor_RealAim[W_MOTOR2_OLD_ID-W_MOTOR1_OLD_ID]*360/1000),(int)(Motor_RealAim[W_MOTOR3_OLD_ID-W_MOTOR1_OLD_ID]*360/1000));
		LCD_SetXY(0,7);
		LCD_WriteString("1.Set_XYZ");

		if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
		{
			switch(key_value)
			{
				case key1:
					Input_DoubleValue(&uart_data.x,"x"); 
					Input_DoubleValue(&uart_data.y,"y");
					Input_DoubleValue(&uart_data.z,"z");
					Input_IntValue(&pos_speed.v_A,"v_A");
					Input_IntValue(&pos_speed.v_B,"v_B");
// 					Input_IntValue(&pos_speed.v_C,"v_C");
					Data_Processing();
 					W_MOTOR_OLD_FUNC(W_MOTOR2_OLD_ID, (float)pos_speed.pos_A , (int16_t)pos_speed.v_A , CMD_SET_PSG);
					W_MOTOR_OLD_FUNC(W_MOTOR2_OLD_ID, (float)pos_speed.pos_B , (int16_t)pos_speed.v_B , CMD_SET_PSG);
// 					W_MOTOR_OLD_FUNC(W_MOTOR3_OLD_ID, (float)pos_speed.pos_C , (int16_t)pos_speed.v_C , CMD_SET_PSG);
					break;
				case keyback:
					return;
			}
		}
		Delay_ms(10);
	}
}

/*
 * 函数名: Text_Catch
 * 描  述: 测试电机和数据使用
 * 输  入: 无
 * 输  出: 无
 * 调  用: 外部调用
 */
void Text_Catch(void)
{
	rt_uint32_t key_value;
	while(1)
	{
		LCD_Clear();
		LCD_SetXY(0,0);
		LCD_Printf("=======Catch=======");
		LCD_SetXY(0,1);
		LCD_Printf("1.STOP %d %d %d",(int)(Motor_RealAim[W_MOTOR1_OLD_ID-W_MOTOR1_OLD_ID]*360/1000),\
							(int)(Motor_RealAim[W_MOTOR2_OLD_ID-W_MOTOR1_OLD_ID]*360/1000),(int)(Motor_RealAim[W_MOTOR3_OLD_ID-W_MOTOR1_OLD_ID]*360/1000));
		LCD_SetXY(0,2);
		LCD_WriteString("2.Set Angle");
		LCD_SetXY(0,3);
		LCD_WriteString("3.Set Speed");
		LCD_SetXY(0,4);
		LCD_WriteString("4.Set Angle and Speed");
		LCD_SetXY(0,5);
		LCD_WriteString("5.Clear");
		LCD_SetXY(12,5);
		LCD_WriteString("6.Can");
		LCD_SetXY(0,6);
		LCD_Printf("Now Angle:%g  %g  %g",Motor_Pos[W_MOTOR1_OLD_ID-W_MOTOR1_OLD_ID]*360,\
							Motor_Pos[W_MOTOR2_OLD_ID-W_MOTOR1_OLD_ID]*360,Motor_Pos[W_MOTOR3_OLD_ID-W_MOTOR1_OLD_ID]*360);
		
		LCD_SetXY(0,7);
		LCD_Printf("IOA:%d%d%d%d",M_IO_READ(W_MOTOR1_OLD_ID,1),M_IO_READ(W_MOTOR1_OLD_ID,2),\
							M_IO_READ(W_MOTOR1_OLD_ID,3),M_IO_READ(W_MOTOR1_OLD_ID,4));
		LCD_SetXY(9,7);
		LCD_Printf("IOB:%d%d%d%d",M_IO_READ(W_MOTOR2_OLD_ID,1),M_IO_READ(W_MOTOR2_OLD_ID,2),\
							M_IO_READ(W_MOTOR2_OLD_ID,3),M_IO_READ(W_MOTOR2_OLD_ID,4));
// 		LCD_SetXY(0,8);
// 		LCD_Printf("IOC:%d%d%d%d",M_IO_READ(W_MOTOR3_OLD_ID,1),M_IO_READ(W_MOTOR3_OLD_ID,2),\
// 							M_IO_READ(W_MOTOR3_OLD_ID,3),M_IO_READ(W_MOTOR3_OLD_ID,4));	
										
		if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
		{
			switch(key_value)
			{
				case key1:
					Moto_Stop(W_MOTOR1_OLD_ID);
					Moto_Stop(W_MOTOR2_OLD_ID);  
// 					Moto_Stop(W_MOTOR3_OLD_ID);
					break;
				case key2:
					Input_DoubleValue(&catch_data.angle_A,"Goal angle_A");
					Input_DoubleValue(&catch_data.angle_B,"Goal angle_B");
// 					Input_DoubleValue(&catch_data.angle_C,"Goal angle_C");
					Data_Pro_Text();
					Moto_Set_GPos(W_MOTOR1_OLD_ID,(float)pos_speed.pos_A);
					Moto_Set_GPos(W_MOTOR2_OLD_ID,(float)pos_speed.pos_B);
// 					Moto_Set_GPos(W_MOTOR3_OLD_ID,(float)pos_speed.pos_C);
					break;
				case key3:
					Input_IntValue(&pos_speed.v_A,"Goal Speed_A");
					Input_IntValue(&pos_speed.v_B,"Goal Speed_B");
// 					Input_IntValue(&pos_speed.v_C,"Goal Speed_C");
					Moto_Set_GSpeed(W_MOTOR1_OLD_ID,(int16_t)pos_speed.v_A);
					Moto_Set_GSpeed(W_MOTOR2_OLD_ID,(int16_t)pos_speed.v_B);
// 					Moto_Set_GSpeed(W_MOTOR3_OLD_ID,(int16_t)pos_speed.v_C);
					break;
				case key4:
					Input_DoubleValue(&catch_data.angle_A,"Goal angle_A");
					Input_IntValue(&pos_speed.v_A,"Goal Speed");
					Input_DoubleValue(&catch_data.angle_B,"Goal angle_B");
					Input_IntValue(&pos_speed.v_B,"Goal Speed");				
// 					Input_DoubleValue(&catch_data.angle_C,"Goal angle_C");
// 					Input_IntValue(&pos_speed.v_C,"Goal Speed");				
					Data_Pro_Text();
					W_MOTOR_OLD_FUNC(W_MOTOR1_OLD_ID, (float)pos_speed.pos_A , (int16_t)pos_speed.v_A , CMD_SET_PSG);
					W_MOTOR_OLD_FUNC(W_MOTOR2_OLD_ID, (float)pos_speed.pos_B , (int16_t)pos_speed.v_B , CMD_SET_PSG);
// 					W_MOTOR_OLD_FUNC(W_MOTOR3_OLD_ID, (float)pos_speed.pos_C , (int16_t)pos_speed.v_C , CMD_SET_PSG);
					break;
				case key5:
					Moto_Clear_NPos(W_MOTOR1_OLD_ID);
					Moto_Clear_NPos(W_MOTOR2_OLD_ID);
// 					Moto_Clear_NPos(W_MOTOR3_OLD_ID);
					break;
				case key6:
					W_MOTOR_OLD_FUNC(W_MOTOR1_OLD_ID, 0 , 0 , CMD_INIT_CAN);
					W_MOTOR_OLD_FUNC(W_MOTOR2_OLD_ID, 0 , 0 , CMD_INIT_CAN);
// 					W_MOTOR_OLD_FUNC(W_MOTOR3_OLD_ID, 0 , 0 , CMD_INIT_CAN);
					break;
				case keyback:
					return;
			}
		}
		Delay_ms(10);
	}
}

/*
 * 函数名: rt_thread_entry_catch
 * 描  述: 线程入口函数，用来实时处理数据并完成接球动作
 * 输  入: 无
 * 输  出: 无
 * 调  用: 外部调用
 */
static void rt_thread_entry_catch(void* parameter)
{
	/*data receive*/
// 	rt_uint8_t dataqueue[300];
// 	rt_uint8_t buffer[64];
// 	int head=-1;
// 	int end=0;

	/*  model*/
//  l[2]2+l[1]2=x2+y2+(l[0]-z)2
	/*-------------------------------------------------------*/
// 	double Current_Radius = 0;
// 	int Step = 0;
// 	Clear_Data();
	pos_speed.v_A = SPEED_A;
	pos_speed.v_B = SPEED_B;
	pos_speed.v_C = SPEED_C;
	while(1)
	{
// 		Step = 0;
// 		Current_Radius = sqrt(pow(uart_data.x,2)+pow(uart_data.y,2)+pow(uart_data.z,2));
// 		/*判断：等待数据落入有效半径之内*/
// 		while(EFFECTIVE_RADIUS>Current_Radius&&Step<1000)
// 		{
// 			Current_Radius = sqrt(pow(uart_data.x,2)+pow(uart_data.y,2)+pow(uart_data.z,2));
			Data_Processing();
			W_MOTOR_OLD_FUNC(W_MOTOR1_OLD_ID, (float)pos_speed.pos_A , (int16_t)pos_speed.v_A , CMD_SET_PSG);
			W_MOTOR_OLD_FUNC(W_MOTOR2_OLD_ID, (float)pos_speed.pos_B , (int16_t)pos_speed.v_B , CMD_SET_PSG);
// // 			W_MOTOR_OLD_FUNC(W_MOTOR3_OLD_ID, (float)pos_speed.pos_C , (int16_t)pos_speed.v_C , CMD_SET_PSG);
// 			/*状态值，用于监控*/
// 			
// 			Step++;
// 		}
	}

}



int rt_thread_catch_init(void)
{
    rt_err_t result;
    
	result = rt_thread_init(&thread_catch, 
                            "thread_catch",
                            rt_thread_entry_catch,
                            RT_NULL,
                            &thread_catch_stack[0], sizeof(thread_catch_stack),
							9, 5);
    if (result == RT_EOK)
        rt_thread_startup(&thread_catch);
    else
        return RT_ERROR;

    return RT_EOK;
}

/********************************    END OF FILE    ***************************/
