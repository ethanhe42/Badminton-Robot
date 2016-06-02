/* Includes ------------------------------------------------------------------*/
#include "includes.h"
#include "PointRoute.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*
 * 函数名: GoLineTest
 * 描  述: 走直线功能测试
 * 输  入: 无
 * 输  出: 无
 * 调  用: 外部调用
 */
void GoLineTest()
{
	rt_uint32_t key_value;
	static struct Point end_point={1000,0};
	static double aim_angle;
	static double speed_sta=100;
	static double speed_max=800;
	static double speed_end=0;
	static double err=5;
	extern double Acc_Rate;
	extern double Dec_Rate;
	
	static struct Input_ListValue Values[]=
	 {
		 {&Pid_List[2].p,	TYPE_FP64,	"AP"},
		 {&Pid_List[2].i,   TYPE_FP64,	"AI"},
		 {&Pid_List[2].d,	TYPE_FP64,	"AD"},
		 {&Pid_List[3].p,	TYPE_FP64,	"DP"},
		 {&Pid_List[3].i,	TYPE_FP64,	"DI"},
		 {&Pid_List[3].d,	TYPE_FP64,	"DD"},
		 {&end_point.x,		TYPE_FP64,	"X"},
		 {&end_point.y,		TYPE_FP64,	"Y"},
		 {&aim_angle,		TYPE_FP64,	"Aim_Angle"},
		 {&speed_sta,		TYPE_FP64,	"Speed_Sta"},
		 {&speed_max,		TYPE_FP64,	"Speed_Max"},
		 {&speed_end,		TYPE_FP64,	"Speed_End"},
		 {&Acc_Rate,		TYPE_FP64,	"Acc_Rate"},
		 {&Dec_Rate,		TYPE_FP64,	"Dec_Rate"},
		 {&err,				TYPE_FP64,	"err"},
	 };

	LCD_Clear();
	GPS_Clear();
	PID_Clear();
	 
	Input_List(Values,15);
	
	SetLine(end_point,aim_angle*pi/180,speed_sta,speed_max,speed_end);
	
	while(1)
	{
		GoLine();
		if(GetLength(GPS.position,end_point) < err)
		{
			SPEED_STOP;
			break;
		}
		Delay_ms(5);
	}
	
	if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_FOREVER) == RT_EOK);
	
}

/*
 * 函数名: KeepTest
 * 描  述: keep功能测试
 * 输  入: 无
 * 输  出: 无
 * 调  用: 外部调用
 */
void KeepTest()
{
	rt_uint32_t key_value;
	
	static struct Point end_point={500,500};
	static double aim_angle;
	struct Input_ListValue Values[]=
	 {
		 {&Pid_List[4].p,	TYPE_FP64,	"AP"},
		 {&Pid_List[4].i,   TYPE_FP64,	"AI"},
		 {&Pid_List[4].d,	TYPE_FP64,	"AD"},
		 {&Pid_List[5].p,	TYPE_FP64,	"DP"},
		 {&Pid_List[5].i,	TYPE_FP64,	"DI"},
		 {&Pid_List[5].d,	TYPE_FP64,	"DD"},
		 {&end_point.x,		TYPE_FP64,	"X"},
		 {&end_point.y,		TYPE_FP64,	"Y"},
		 {&aim_angle,		TYPE_FP64,	"Aim_Angle"},
	 };
	
	GPS_Clear();
	LCD_Clear();
	PID_Clear();
	
	Input_List(Values,9);
	
	SetKeep(end_point,aim_angle*pi/180,1000);
	
	if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_FOREVER) == RT_EOK);
	
}

/*
 * 函数名: GoCircleTest
 * 描  述: 跑圆功能测试
 * 输  入: 无
 * 输  出: 无
 * 调  用: 外部调用
 */
void GoCircleTest(void)
{
	rt_uint32_t key_value;
	
	static struct Point center={500,0};
	static double r=500;
	static double speed=-300;

	struct Input_ListValue Values[]=
	 {
		 {&Pid_List[0].p,	TYPE_FP64,	"AP"},
		 {&Pid_List[0].i,   TYPE_FP64,	"AI"},
		 {&Pid_List[0].d,	TYPE_FP64,	"AD"},
		 {&Pid_List[1].p,	TYPE_FP64,	"DP"},
		 {&Pid_List[1].i,	TYPE_FP64,	"DI"},
		 {&Pid_List[1].d,	TYPE_FP64,	"DD"},
		 {&center.x,		TYPE_FP64,	"Center_X"},
		 {&center.y,		TYPE_FP64,	"Center_Y"},
		 {&r,				TYPE_FP64,	"R"},
		 {&speed,			TYPE_FP64,	"Speed"},
	 };
	
	GPS_Clear();
	LCD_Clear();
	PID_Clear();
	
	Input_List(Values,10);
	
	SetCircle(center,r,speed);
	
	while(1)
	{
		GoCircle();
		if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
			break;
		Delay_ms(5);
	}
	
	if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_FOREVER) == RT_EOK);
	
}

/*
 * 函数名: GoLineTest
 * 描  述: 跑点阵路径功能测试
 * 输  入: 无
 * 输  出: 无
 * 调  用: 外部调用
 */
u8 HandoverFlag[3]={1,1,0};
u8 PoleFlag = 0;
u8 SwingFlag = 1;
u8 CircleFlag[4] = {0,0,0,0};
u8 Circle_dynFlag = 0;
u8 LaserFlag[] = {0,0,0};
u8 NewPole;

u8 WalkGround  = 0;
u8 TimeOnFlag = 1;
int RunTime;
extern int Handle_Off_Count;
extern int Handle_Off_Count2;

void RouteTest()
{
	rt_uint32_t key_value;

	static struct Input_ListValue Values[]=
	 {
		 {(void*)&Pid_List[6].p,	  TYPE_FP64,	"AP"}, 
		 {(void*)&Pid_List[6].i,	  TYPE_FP64,	"AI"}, 
		 {(void*)&Pid_List[6].d,	  TYPE_FP64,	"AD"},
		 {(void*)&Pid_List[7].p,	  TYPE_FP64,	"DP"}, 
		 {(void*)&Pid_List[7].i,	  TYPE_FP64,	"DI"}, 
		 {(void*)&Pid_List[7].d,	  TYPE_FP64,	"DD"},
		 {(void*)&HandoverFlag[0],	TYPE_UINT8,	"HandoverFlag_ss"},
		 {(void*)&HandoverFlag[1],  TYPE_UINT8,	"HandoverFlag_pw"},
		 {(void*)&PoleFlag,	        TYPE_UINT8,	"PoleFlag"}, 
		 {(void*)&NewPole,		      TYPE_UINT8,	  "NewPole"},
		 {(void*)&SwingFlag,	      TYPE_UINT8,	"SwingFlag"}, 	
		 {(void*)&Circle_dynFlag,	  TYPE_UINT8,	"Circle_dynFlag"},		 
		 {(void*)&CircleFlag[0],	  TYPE_UINT8,	"CircleFlag_ss1"},
		 {(void*)&CircleFlag[Circle_jg],	  TYPE_UINT8,	"CircleFlag_JG"},
		 {(void*)&LaserFlag[0],	      TYPE_UINT8,	"Laser_SS"},
		 {(void*)&LaserFlag[1],	      TYPE_UINT8,	"Laser_PW"},
		 {(void*)&LaserFlag[2],	      TYPE_UINT8,	"Laser_JG"},
		 {(void*)&WalkGround,	      TYPE_UINT8,	  "WalkGround"},
		 {(void*)&Route_Num,		    TYPE_UINT8,	  "Route_Num"},
		 
	 }; 
	 
/*->*/RouteTest:

	 while(1)
	 {
		 LCD_Clear();
		 LCD_SetXY(0,0);
		 LCD_Printf("1.RouteGo  2.Restart");
		 if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
		 {
				switch(key_value)
				{
					case key1:
						goto RouteGo;
					
					case key2:
						goto Restart;
					
					case keyback:
						return;
				}
		 }
		 Delay_ms(20);
	 }
	
/*->*/RouteGo:
	 
	Input_List(Values,19);
	
	while(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) != RT_EOK)
	{
		LCD_Clear();
		LCD_SetXY(0,0);
		if(Abs(Cam_Z-Aim_Z_Handover[Aim_start])>40)
			LCD_Printf("Focal Distance Error!");
		else
			LCD_Printf("Focal Distance OK!");
		
		LCD_SetXY(0,1);
		LCD_Printf("c_z:%d  e_z:%d",Cam_Z,Cam_Z-Aim_Z_Handover[Aim_start]);
		LCD_SetXY(0,2);
		LCD_Printf("c_x:%d  e_x:%d",Cam_X,Cam_X-Aim_X_Handover[Aim_start]);
		LCD_SetXY(0,3);
		LCD_Printf("c_y:%d  e_y:%d",Cam_Y,Cam_Y-Aim_Y_Handover[Aim_start]);
		LCD_SetXY(0,4);
		LCD_Printf("c_a:%d  e_a:%d",Cam_A,Cam_A-Aim_A_Handover[Aim_start]);
		Delay_ms(5);
	}
	
	LCD_Clear();
	LCD_SetXY(0,1);
	LCD_Printf("Ready to initialize the motor!");

	Wait_Button8_Change();
	Motor_Init();
	Hall_Send(1);//跷跷板初始化
	BufferOn();
	Delay_ms(50);
	FireOff();
//	Delay_ms(50);
//	BufferOff();
	W_MOTOR_OLD_FUNC(W_MOTOR1_OLD_ID, 0.5 , 1000 , CMD_SET_PSG);
	Moto_Set_GPos(W_MOTOR2_OLD_ID,2.55);
	
	LCD_Clear();
	LCD_SetXY(0,1);
	LCD_Printf("Ready to go!");

	ChooseGround_Pole(GROUND);
	
	Wait_Button8_Change();
	
	GPS_Init(PointRoute[Route[Route_Num-1]].position,PointRoute[Route[Route_Num-1]].selfdir);

	rt_mb_send (&Mb_Auto, Route_Num);
	
	Handle_Off_Count = 0;
	
	if(Route_Num==1)
	{
		RunTime = 0;
		TimeOnFlag = 1;
	}
	

	while(1)
	{
		if(TimeOnFlag)
			RunTime++;
		Delay_ms(5);
	}
	
/*->*/Restart:
	 
	 LCD_Clear();
	 Delay_ms(600);
	 LCD_SetXY(0,1);
	 LCD_WriteString("restart done!");
	 Delay_ms(400);
	 
	 goto RouteTest;
}


/*
 * 函数名: ArmactionTest
 * 描  述: ArmactionTest功能测试
 * 输  入: 无
 * 输  出: 无
 * 调  用: 外部调用
 */
u8 armtest_flag=1;

void ArmTest(void)
{
	int num = 0;
	armtest_flag = 0;
	
	RouteFinish = 0;
	
	Input_IntValue(&num,"num");
	
	rt_mb_send (&Mb_Arm, num);
			
	LCD_Clear();
	LCD_SetXY(0,1);
	LCD_Printf("Maybe need a Button Push!");
	while(1)
		Delay_ms(5);
}

extern double motor1_speed;
void MotorPos(void)
{
	rt_uint32_t key_value;

	static struct Input_ListValue Values[]=
	 {
		 {(void*)&Motor1_Aim_Pos[0],	  TYPE_FP32,	"ss_put"},
     {(void*)&Motor1_Aim_Pos[1],	  TYPE_FP32,	"ss_get"},
     {(void*)&Motor1_Aim_Pos[2],	  TYPE_FP32,	"pw_put"},
		 {(void*)&Motor1_Aim_Pos[3],	  TYPE_FP32,	"pw_get"},
		 {(void*)&Motor1_Aim_Pos[4],	  TYPE_FP32,	"sw_put"},
		 {(void*)&Motor1_Aim_Pos[5],	  TYPE_FP32,	"jg_put"},  	 
		 {(void*)&motor1_speed,	        TYPE_FP64,	"motor_speed1"},  	 		 
	 };
	 
	static struct Input_ListValue Values1[]=
	 {
		 {(void*)&Motor2_Aim_Pos[0],	  TYPE_FP32,	"ss_put"},
     {(void*)&Motor2_Aim_Pos[1],	  TYPE_FP32,	"ss_get"},
     {(void*)&Motor2_Aim_Pos[2],	  TYPE_FP32,	"pw_put"},
		 {(void*)&Motor2_Aim_Pos[3],	  TYPE_FP32,	"pw_get"},
		 {(void*)&Motor2_Aim_Pos[4],	  TYPE_FP32,	"sw_put"},
		 {(void*)&Motor2_Aim_Pos[5],	  TYPE_FP32,	"jg_put"},  	 
	 }; 	 
	 
	static struct Input_ListValue Values2[]=
	 {
		 {(void*)&Motor3_Aim_Pos,	  TYPE_FP32,	"ss_pos"}, 
	 }; 	 
	 
/*->*/motorpos:	 
	while(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) != RT_EOK)
	{
		LCD_Clear();
		LCD_SetXY(0,0);
		LCD_Printf("======MotorPos======");
		LCD_SetXY(0,1);
		LCD_Printf("1.Motor1");
		LCD_SetXY(0,2);
		LCD_Printf("2.Motor2");
		LCD_SetXY(0,3);
		LCD_Printf("3.Motor3");
		Delay_ms(20);
	}
	switch(key_value)
	{
		case key1:
			goto Motor1;
		case key2:
			goto Motor2;
		case key3:
			goto Motor3;
	}
	
/*->*/Motor1:
	Input_List(Values,7);
	goto motorpos;
/*->*/Motor2:
  Input_List(Values1,6);
  goto motorpos;	
/*->*/Motor3:
  Input_List(Values2,1);	
	goto motorpos;
}


/*
 * 函数名: GoRoute
 * 描  述: 直接走对应路径
 * 输  入: 无
 * 输  出: 无
 * 调  用: 外部调用
 */
void GoRoute(void)
{
	rt_uint32_t key_value;
//	int i;
	
	while(1)
	{
		LCD_Clear();
		LCD_SetXY(0,0);LCD_Printf("1.GoRoute1");
		LCD_SetXY(0,1);LCD_Printf("2.GoRoute2");
		LCD_SetXY(0,2);LCD_Printf("3.GoRoute3");
		LCD_SetXY(0,3);LCD_Printf("4.GoRoute4");
		LCD_SetXY(0,4);LCD_Printf("5.GoRoute5");
		LCD_SetXY(0,5);LCD_Printf("6.GoRoute6");
		LCD_SetXY(0,6);LCD_Printf("7.GoRoute7");
		LCD_SetXY(0,7);LCD_Printf("8.Walkground %d",WalkGround);
//		
//		for(i=0;i<7;i++)
//		{
//			LCD_SetXY(0,i);
//			LCD_Printf("%d.GoRoute%d",i+1,i+1);
//		}
		
		if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
		{
            if(key_value == key8)
            {
                if(WalkGround == 1)
                    WalkGround = 0;
                else
                    WalkGround = 1;
            }
            else
            {
                LCD_Clear();
                LCD_SetXY(0,1);
                LCD_Printf("Ready to initialize the motor!");
                Wait_Button8_Change();
                Motor_Init();
                FireOff();
                BufferOn();
                ChooseGround_Pole(GROUND);
                W_MOTOR_OLD_FUNC(W_MOTOR1_OLD_ID, 0.5 , 1000 , CMD_SET_PSG);
                Moto_Set_GPos(W_MOTOR2_OLD_ID,2.55);
                LCD_Clear();
                LCD_SetXY(0,1);
                LCD_Printf("Ready to go!");
                Wait_Button8_Change();
			
                switch(key_value)
                {
                    case key1:
                        GPS_Init(PointRoute[Route[0]].position,PointRoute[Route[0]].selfdir);
                        rt_mb_send (&Mb_Auto, 1);
                        break;
                    case key2:
                        GPS_Init(PointRoute[Route[1]].position,PointRoute[Route[1]].selfdir);
                        rt_mb_send (&Mb_Auto, 2);
                        break;
                    case key3:
                        GPS_Init(PointRoute[Route[2]].position,PointRoute[Route[2]].selfdir);
                        rt_mb_send (&Mb_Auto, 3);
                        break;
                    case key4:
                        GPS_Init(PointRoute[Route[3]].position,PointRoute[Route[3]].selfdir);
                        rt_mb_send (&Mb_Auto, 4);
                        break;
                    case key5:
                        GPS_Init(PointRoute[Route[4]].position,PointRoute[Route[4]].selfdir);
                        rt_mb_send (&Mb_Auto, 5);
                        break;
                    case key6:
                        GPS_Init(PointRoute[Route[5]].position,PointRoute[Route[5]].selfdir);
                        rt_mb_send (&Mb_Auto, 6);
                        break;
                    case key7:
                        GPS_Init(PointRoute[Route[8]].position,PointRoute[Route[8]].selfdir);
                        rt_mb_send (&Mb_Auto, 7);
                        break;
                    case keyback:
                        return;
                }
                
                Handle_Off_Count = 0;
                RunTime = 0;
                TimeOnFlag = 1;
                
                while(1)
                {
                    if(TimeOnFlag)
                        RunTime++;
                    Delay_ms(5);
                }
            }
		}
		Delay_ms(10);
	}
}



/********************************    END OF FILE    ***************************/
