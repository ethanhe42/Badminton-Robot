#include "includes.h"
#include "math.h"

fp64 Cam_Angle[3];//摄像头返回的角度和距离数组
fp64 Cam_Dist[3];
s16 Cam_X,Cam_Y,Cam_Z,Cam_A;//交接摄像头返回的坐标
s16 Cam_Pole_X,Cam_Pole_Y;//看梅花桩盘子摄像头返回的坐标
u8 CamFlag;
u8 Cam_Status;
fp64 cam_angle;//由于摄像头并不装在正中心，所以车中心与圆相切时摄像头中心并不与圆相切
double CAM_R = 1400.0;

FIL cam_file;

camera_t camera_data1={0};
camera_t camera_data2={0};
camera_t camera_data3={0};
camera_t camera_data4={0};
camera_t camera_data5={0};
camera_t camera_data6={0};

Facility seesaw = {-3000,3000,1250,0};
Facility junglegym = {-6000,6000,1500,1};
Facility polewalk = {-9000,3000,1250,2};
Facility swing = {-3000,9000,1250,3};

struct Point Gps_assume[5];
extern double Aim_Angle;
extern double Aim_Dist;

u8 PLACE_HANDOVER = 3;
u8 PLACE_CIRCLE = 4;
float REL = 1.2f;
double Z_SPEED = 4000;

double acc_p = 1000;//依靠摄像头跑圆的加减速和最大速度(注意顺时针跟逆时针跑圆的差别)
double V_max = 2000;
double acc_n = 1200;	
u8 ea_flag;

fp64 SW_FORWARD_SPEED = 400;//依靠白线校到位置后再往前走一点的速度
fp64 SW_FORWARD_DIS = 150;//依靠白线校到位置后再往前走一点的距离

extern u8 Cam_F_DISABLE;
extern fp64 SW_SPEED;

extern u8 FilterFlag[6];
extern u8 Handover_Z_DISABLE;
extern u8 Cannot_See_Flag;
extern fp64 Handover_R;

s16 stop_ex_h = 15;
s16 stop_ey_h = 15;
s16 stop_ed1_w = 4 ;
s16 stop_ed2_w = 4 ;
s16 stop_ed = 1;
double stop_ea = 0.01;
double stop_ed_static = 0.4;
double stop_ea_static = 0.3;

double pole_k_y = -0.02;
double pole_k_x = 3;
double SPEED_MOTO = 2500;
double SPEED_DOWN = 500;
u8 REDFlag;
extern u8 NewPole;
extern int SwitchNum;

/*GPS校正函数：
输入：借助定位的设施：seesaw,junglegym,polewalk,swing，场地：GROUND_RED为红场，GROUND_BLUE为蓝场，模式：CIRCLE为以圆定位，LINE为以直线定位*/
void Gps_Camera(Facility facility,u8 ground,u8 mode)
{
     struct Point base_blue = {3728,8485.281};
     struct Point base_red = {-3728,8485.281};
     struct Point cam_gps,tmp;
     fp64 theta,beta;
     fp64 L1 = 643;
     fp64 L2 = 352.5;//分别为两摄像头中心到车体中心的距离，还待确定
     fp64 W1 = 360.0;
     fp64 L;   
     fp64 ratio = 3.33;   
     
     
     if(ground==RED)
     {
       polewalk.center.x = -3000;
       polewalk.center.y = 9000;
       swing.center.x = -9000;
       swing.center.y = 3000;
     }
     
     if(mode==CIRCLE)
     {
      
       cam_angle = -asin(W1/(ratio*Cam_Dist[Cam_Circle_Line]+facility.r));
       
       theta = Cam_Angle[Cam_Circle_Line]/180.0*pi;

       theta += GPS.radian; 
       
       cam_gps.x = facility.center.x-(facility.r+ratio*Cam_Dist[Cam_Circle_Line])*cos(theta);
       cam_gps.y = facility.center.y-(facility.r+ratio*Cam_Dist[Cam_Circle_Line])*sin(theta);
       
              
       if (CamFlag==1)
       {
         GPS.position.x = cam_gps.x-L1*cos(GPS.radian)+W1*sin(GPS.radian);
         GPS.position.y = cam_gps.y-L1*sin(GPS.radian)-W1*cos(GPS.radian);  
        }
        
         Gps_assume[facility.num].x = cam_gps.x-L1*cos(GPS.radian)+W1*sin(GPS.radian);
         Gps_assume[facility.num].y = cam_gps.y-L1*sin(GPS.radian)-W1*cos(GPS.radian);
     } 
     
     if(mode==LINE)
     {
       if(ground==BLUE)
       {
         theta = pi/4-GPS.radian;
         beta = atan(L2/L1);
         L = sqrt(L1*L1+L2*L2);
         beta = pi/2-theta-beta; 
         
         tmp.x = base_blue.x - ratio*Cam_Dist[Cam_Circle_Line];
         tmp.y = base_blue.y - ratio*Cam_Dist[Cam_Line_BLUE]-L*cos(beta);
         cam_gps.x = tmp.x*0.707 - tmp.y*0.707;
         cam_gps.y = tmp.x*0.707 + tmp.y*0.707;
       }
			 
       if(ground==RED)
       {
         theta = 1.25*pi + GPS.radian; 
         beta = atan(L1/L2);
         L = sqrt(L1*L1+L2*L2);
         beta = theta - beta; 
         
         tmp.x = base_red.x + ratio*Cam_Dist[Cam_Circle_Line];
         tmp.y = base_red.y - ratio*Cam_Dist[Cam_Line_BLUE] - L*sin(beta);
         cam_gps.x = tmp.x*0.707 - tmp.y*0.707;
         cam_gps.y = tmp.x*0.707 + tmp.y*0.707;
       }
       
       if(CamFlag==1)
       {
         GPS.position.x = cam_gps.x - L1*cos(GPS.radian);
         GPS.position.y = cam_gps.y - L1*sin(GPS.radian);
       }
         Gps_assume[swing.num].x = cam_gps.x - L1*cos(GPS.radian)+W1*sin(GPS.radian);
         Gps_assume[swing.num].y = cam_gps.y - L1*sin(GPS.radian)-W1*cos(GPS.radian);  
     }
 } 
 
 

void Go_CamCircle(void)
{
	 double error_angle,error_dis;
	 static u8 flag = 0;
	 
	 if(camera_data1.time_flag<100)
	 {
		 if(ea_flag)
			error_angle = cam_angle - Cam_Angle[Cam_Circle_Line]/180.0*pi;
		 else
			error_angle = (Aim_Angle - Cam_Angle[Cam_Circle_Line])/180.0*pi;
		 
		 error_dis = Aim_Dist - Cam_Dist[Cam_Circle_Line];
		 
		 if(error_angle > pi)
			 error_angle -= 2*pi;
		 if(error_angle < -pi)
			 error_angle += 2*pi;
		 
		 if(Abs(error_angle)<stop_ea)
			 Vout[4] = 0;
		 else
			 Vout[4] = 1000*AnglePID(error_angle,Pid_List[27]);
		 if(Abs(error_dis)<stop_ed)
			 Speed_X = 0;
		 else
			 Speed_X = DistancePID(error_dis,Pid_List[26]);
		 
		 Speed_Rotation = Vout[4] - Speed_Y*100/(CAM_R);
		 
		 
		 LCD_Clear();
		 LCD_SetXY(0,0);
		 LCD_Printf("c_d:%.2f  e_d:%.2f",Cam_Dist[Cam_Circle_Line],error_dis);	
		 LCD_SetXY(0,1);
		 LCD_Printf("c_a:%.2f  e_a:%.2f",Cam_Angle[Cam_Circle_Line],error_angle/pi*180.0);
		 LCD_SetXY(0,2);
		 LCD_Printf("s_x:%.2f",Speed_X);	 
		 LCD_SetXY(0,3);
		 LCD_Printf("s_r_amend:%.2f",Vout[4]);
		 LCD_SetXY(0,4);
		 LCD_Printf("s_r:%.2f",Speed_Rotation);	 
		 LCD_SetXY(0,5);	 
		 LCD_Printf("s_y:%.2f",Speed_Y);	 
		 LCD_SetXY(0,6);
		 LCD_Printf("angle:%.2f",Gyro.angle);
		 SD_Printf_MQ(&cam_file,"%.5f %.5f %.5f %.5f %.5f %.5f\r\n",Cam_Angle[0],Cam_Dist[0],Speed_X,Vout[4],Speed_Y,Speed_Rotation);
	 }
	 else 
	 {
		 if(!flag)
		 {
			 flag = 1;
			 SPEED_STOP;
		 }
		 Buz_On();
		 LCD_Clear();
		 LCD_SetXY(0,1);
		 LCD_Printf("Long time data not received!");
		 SD_Printf_MQ(&cam_file,"Timeout");
	 }
}

void ChooseFunction(u8 function)
{
	MOSI_CAMERA1[0] = function;
	Write_Database(W_CAMERA1_ID);		
}
 
void ChooseGround_Pole(u8 ground)
{
	switch(ground)
	{
		case BLUE:
		  MOSI_CAMERA4[0] = 0x02;
		  Write_Database(W_CAMERA4_ID);		
		break;
	  
		case RED:
			MOSI_CAMERA4[0] = 0x01;
		  Write_Database(W_CAMERA4_ID);		
		break;
	}
}


 
void order_s16(s16 a[],int length)
{
	 int i,j;
	 s16 tmp;
	
	for(j=0;j<length;j++) 
	{ 
		for (i=0;i<length-j-1;i++) 
		if (a[i]>a[i+1]) 
		{ 
			tmp=a[i]; 
			a[i]=a[i+1]; 
			a[i+1]=tmp;
		}
	}
}
	
void order_fp64(fp64 a[],int length)
{
	 int i,j;
	 fp64 tmp;

	 for(j=0;j<length;j++) 
	{ 
		for (i=0;i<length-j-1;i++) 
		if (a[i]>a[i+1]) 
		{ 
			tmp=a[i]; 
			a[i]=a[i+1]; 
			a[i+1]=tmp;
		} 
	}
} 	
	
 
 //主要用来记录交接处摄像头坐标
void HandoverRecode(u8 place)
{
	 int i;
	 u8 res;
	 static s16 Cam_X_tmp[20] = {0};
	 static s16 Cam_Y_tmp[20] = {0};
	 static s16 Cam_Z_tmp[20] = {0};
	 static s16 Cam_A_tmp[20] = {0};

	 rt_uint32_t key_value;	
   float moto_pos_temp; 

	 moto_pos_temp = Motor_Pos[1]-REL;
	 Moto_Set_GPos(W_MOTOR2_OLD_ID,moto_pos_temp);
	 while(Abs(Motor_Pos[1]-moto_pos_temp)>0.1f)
	 {
		 LCD_Clear();
		 LCD_SetXY(0,0);
		 LCD_Printf("  Motor Pos  ");
		 LCD_SetXY(0,1);
		 LCD_Printf("Motor_Pos:%f",Motor_Pos[1]);
		 LCD_SetXY(0,2);
		 LCD_Printf("Aim_Pos:%f",moto_pos_temp);
		 if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
			 break;
		 Delay_ms(5);
	 }
		
/*->*/Recode:	 
	 for(i=0;i<20;i++)
	 {
		 Cam_X_tmp[i] = Cam_X;
		 Cam_Y_tmp[i] = Cam_Y;
		 Cam_Z_tmp[i] = Cam_Z;
		 Cam_A_tmp[i] = Cam_A;//记录摄像头返回的数据
		 Delay_ms(50);
	 }
	 
	 order_s16(Cam_X_tmp,20);
	 order_s16(Cam_Y_tmp,20);
	 order_s16(Cam_Z_tmp,20);
	 order_s16(Cam_A_tmp,20);
	 
	 LCD_Clear();
	 LCD_SetXY(0,0);
	 if(place == Aim_ss)
			LCD_Printf("HandOver SS");
   if(place == Aim_pw)
			LCD_Printf("HandOver PW");
	 if(place == Aim_sw)
			LCD_Printf("HandOver SW");
		 
	 LCD_SetXY(0,1);
	 LCD_Printf("A_X:%d",Cam_X_tmp[10]);
	 LCD_SetXY(0,2);
	 LCD_Printf("A_Y:%d",Cam_Y_tmp[10]);
	 LCD_SetXY(0,3);
	 LCD_Printf("A_Z:%d",Cam_Z_tmp[10]);
	 LCD_SetXY(0,4);
	 LCD_Printf("A_A:%d",Cam_A_tmp[10]);
	
Show:	 
   while(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) != RT_EOK)
	 {
		 LCD_SetXY(0,5);
		 LCD_Printf("Save/Image?");
		 LCD_SetXY(0,6);
		 LCD_Printf("1.Save  2.Retry");
		 LCD_SetXY(0,7);
		 LCD_Printf("3.Image  4.Done");
		 Delay_ms(20);
	 }
	 
	 switch(key_value)
	 {
		 case key1:
			 SetHandover(Cam_X_tmp[10],Cam_Y_tmp[10],Cam_Z_tmp[10],Cam_A_tmp[10],place);
			 if(place == Aim_ss)
				 Motor2_Aim_Pos[1] = Motor_Pos[1];
			 if(place == Aim_pw)
				 Motor2_Aim_Pos[3] = Motor_Pos[1];
			 
			 LCD_SetXY(13,5);
			 LCD_Printf("Done");
			 
			 goto Show;
			 
		 case key2:
			 goto Recode;			 
			 
		 case key3:
			 res = Save_Img("Handover", 5);
       _show_res(res);
		   LCD_Clear(); 
		   goto Show;
		 
		 case key4:
			 break;
		 
		 default:
			 goto Show;
	 } 
	 
	 LCD_Clear();
	 LCD_SetXY(0,1);
	 LCD_Printf("Done!");
	 Delay_ms(1000);
}


void PoleRecode(u8 ground)
{
	int i;
	u8 res;
	static s16  Cam_X_tmp[20] = {0};
	static s16  Cam_Y_tmp[20] = {0};
	static fp64 Gyro_A_tmp[20] = {0.0};
	rt_uint32_t key_value;	

	/*->*/Recode:
	for(i=0;i<20;i++)
	{
		Cam_X_tmp[i] = Cam_Pole_X;
		Cam_Y_tmp[i] = Cam_Pole_Y;//记录摄像头返回的数据
		Gyro_A_tmp[i] = Gyro.angle;
		Delay_ms(50);
	}

	order_s16(Cam_X_tmp,20);//排序
	order_s16(Cam_Y_tmp,20);
	order_fp64(Gyro_A_tmp,20);

	LCD_Clear();
	LCD_SetXY(0,0);
	LCD_Printf("PolePut");

	LCD_SetXY(0,1);
	LCD_Printf("A_X:%d",Cam_X_tmp[10]);
	LCD_SetXY(0,2);
	LCD_Printf("A_Y:%d",Cam_Y_tmp[10]);
	LCD_SetXY(0,3);
	LCD_Printf("A_A:%.2f",Gyro_A_tmp[10]);
	LCD_SetXY(0,4);
	LCD_Printf("M2:%f",Motor_Pos[1]);

Show:	
	while(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) != RT_EOK)
	{
		LCD_SetXY(0,5);
		LCD_Printf("Save/Image?");
		LCD_SetXY(0,6);
		LCD_Printf("1.Save   2.Retry");
		LCD_SetXY(0,7);
		LCD_Printf("3.Image  4.Done");
		Delay_ms(20);
	}
	 
	switch(key_value)
	{
	 case key1:
		 if(ground==RED)	
			 SetPole(Cam_X_tmp[10],Cam_Y_tmp[10],Gyro_A_tmp[10]);
		 if(ground==BLUE)
			 SetPole(Cam_X_tmp[10],Cam_Y_tmp[10],Gyro_A_tmp[10]);
		
		 Motor2_Aim_Pos[2] = Motor_Pos[1];
		 
		 LCD_SetXY(13,5);
		 LCD_Printf("Done");
		 
		 goto Show;

	 case key2:
		 goto Recode;
		 
	 case key3:
		 if(ground==RED)
			 res = Save_Img("r_pole", 4);
		 else
			 res = Save_Img("b_pole", 4);
		 
     _show_res(res);
		 LCD_Clear();
		 
		 goto Show; 
	 
	 case key4:
		 break;
	 
	 default:
		 goto Show;
	}
  
  
	LCD_Clear();
	LCD_SetXY(0,1);
	LCD_Printf("Done!");
	Delay_ms(1000);
} 
 
void CircleRecode(u8 place)
{
	 int i;
	 u8 res;
	 static fp64  Cam_D_tmp[20] = {0.0};
	 static fp64  Cam_A_tmp[20] = {0.0};
	 static fp64  Gyro_A_tmp[20] = {0.0};
	 rt_uint32_t key_value;	
	
	 ChooseFunction(Detect_Circle);	 
	 
/*->*/Recode:	 
	 for(i=0;i<20;i++)
	 {
		 Cam_D_tmp[i] = Cam_Dist[Cam_Circle_Line];
		 Cam_A_tmp[i] = Cam_Angle[Cam_Circle_Line];//记录摄像头返回的数据
		 Gyro_A_tmp[i] = Gyro.angle;
		 Delay_ms(50);
	 }
	 
	 order_fp64(Cam_D_tmp,20);//排序
	 order_fp64(Cam_A_tmp,20);
	 order_fp64(Gyro_A_tmp,20);
	 
	 LCD_Clear();
	 LCD_SetXY(0,0);
	 LCD_Printf("A_D:%.2f",Cam_D_tmp[10]);
	 LCD_SetXY(0,1);
	 LCD_Printf("A_C_A:%.2f",Cam_A_tmp[10]);
	 LCD_SetXY(0,2);
	 LCD_Printf("A_G_A:%.2f",Gyro_A_tmp[10]);

Show:	 
   while(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) != RT_EOK)
	 {
		 LCD_SetXY(0,4);
		 LCD_Printf("Save/Image?");
		 LCD_SetXY(0,5);
		 LCD_Printf("1.Save   2.Retry");
		 LCD_SetXY(0,6);
		 LCD_Printf("3.Image  4.Done");
		 Delay_ms(20);
	 }
		 
	 switch(key_value)
	 {
		 case key1:
			 SetCamCircle(Cam_A_tmp[10],Cam_D_tmp[10],Gyro_A_tmp[10],place);
			 LCD_SetXY(13,4);
			 LCD_Printf("Done");
			 goto Show;
		 
		 case key2:
			 goto Recode;
			 
		 case key3:
			 res = Save_Img("circle", 1);
			_show_res(res);
			 LCD_Clear();
			 
			 goto Show; 
		 
		 case key4:
			 break;
		 
		 default:
			 goto Show;
	 }
	 
	 LCD_Clear();
	 LCD_SetXY(0,1);
	 LCD_Printf("Done!");
	 Delay_ms(1000); 
			 
} 

void WhiteTapeRecode(u8 ground)
{
	 int i;
	 u8 res;
	 static fp64  Cam_D_F_tmp[20] = {0.0};
	 static fp64  Cam_D_S_tmp[20] = {0.0};
	 static fp64  Gyro_A_tmp[20] = {0.0};

	 rt_uint32_t key_value;	
    
	 ChooseFunction(Detect_WhiteTape);	 
	  
/*->*/Recode:	 
	 for(i=0;i<20;i++)
	 {
		 Cam_D_F_tmp[i] = Cam_Dist[Cam_Circle_Line];
		 if(ground==RED)
			 Cam_D_S_tmp[i] = Cam_Dist[Cam_Line_RED];
		 else 
		   Cam_D_S_tmp[i] = Cam_Dist[Cam_Line_BLUE];//记录摄像头返回的数据
		 Gyro_A_tmp[i] = Gyro.angle;
		 Delay_ms(50);
	 }
	 
	 order_fp64(Cam_D_F_tmp,20);//排序
	 order_fp64(Cam_D_S_tmp,20);
	 order_fp64(Gyro_A_tmp,20);
	 
	 LCD_Clear();
	 LCD_SetXY(0,0);
	 LCD_Printf("WhiteTape Recode");
   LCD_SetXY(0,1);
	 LCD_Printf("A_D_F:%.2f",Cam_D_F_tmp[10]);
	 LCD_SetXY(0,2);
	 LCD_Printf("A_D_S:%.2f",Cam_D_S_tmp[10]);
	 LCD_SetXY(0,3);
	 LCD_Printf("A_G_A:%.2f",Gyro_A_tmp[10]);
	 
Show:	 
   while(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) != RT_EOK)
	 {
		 LCD_SetXY(0,4);
		 LCD_Printf("Save/Image?");
		 LCD_SetXY(0,5);
		 LCD_Printf("1.Save  2.Image");
		 LCD_SetXY(0,6);
		 LCD_Printf("3.Retry  4.Done");
		 Delay_ms(20);
	 }
		 
	 switch(key_value)
	 {
		 case key1:
			 if(ground==RED)
				 SetSwing(Cam_D_F_tmp[10],Cam_D_S_tmp[10],Gyro_A_tmp[10],RED);
			 else
				 SetSwing(Cam_D_F_tmp[10],Cam_D_S_tmp[10],Gyro_A_tmp[10],BLUE);
			 
			 LCD_SetXY(13,4);
			 LCD_Printf("Done");
			 goto Show;
			
		 case key2:
			 if(ground==RED)
			 {
				 res = Save_Img("r_pole_f", 1);
				 res = Save_Img("r_pole_s", 2);
			 }
			 else
			 {
				 res = Save_Img("b_pole_f", 1);
				 res = Save_Img("b_pole_s", 2);
			 }
			 _show_res(res);		  
		 
			 goto Show;
			 
		 case key3:
			 goto Recode;
		 
		 case key4:
			 break;
		 
		 default:
			 goto Show;
	 }
    
   
	 	 
	 LCD_Clear();
	 LCD_SetXY(0,1);
	 LCD_Printf("Done!");
	 Delay_ms(1000); 
}
 

void PlaceDisplay(u8 function,u8 place)
{
	if(function==Recode_Circle)
	{
		switch(place)
		{
			case Circle_ss1:
				LCD_Printf("Circle_ss1");
			break;
			case Circle_ss2:
				LCD_Printf("Circle_ss2");
			break;
			case Circle_pw:
				LCD_Printf("Circle_pw");
			break;
			case Circle_jg:
				LCD_Printf("Circle_jg");
			break;		
			
		}
  }
	if(function==Recode_Handover)
	{
		switch(place)
		{
			case Aim_ss:
				LCD_Printf("Handover_ss");
			break;
			case Aim_pw:
				LCD_Printf("Handover_pw");
			break;
			case Aim_sw:
				LCD_Printf("Handover_sw");
			break;
		}
	}
}


void CamRecode(u8 function,u8 place,u8 FUNCTION)
{
	rt_uint32_t key_value;
	int nowpage = 0;
	int allpage = 3;
	
	if(FUNCTION==ARMACTION)
	{
		LCD_Clear();
		LCD_SetXY(0,0);
		LCD_Printf("======RECODE======");
		LCD_SetXY(0,2);
		LCD_Printf("To the right place");
		LCD_SetXY(0,3);
		LCD_Printf("Press any key");
		LCD_SetXY(0,1);
		switch(function)
		{
			case Recode_Circle:
				PlaceDisplay(function,place);
				ChooseFunction(Detect_Circle);
				if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_FOREVER) == RT_EOK)	
					CircleRecode(place);
				break;

			case Recode_Handover:
				PlaceDisplay(function,place);	
				if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_FOREVER) == RT_EOK)
					HandoverRecode(place);
				break;

			case Recode_Pole:
				LCD_Printf("PolePut");
				if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_FOREVER) == RT_EOK)
				{
					ChooseGround_Pole(GROUND);
					PoleRecode(GROUND);
				}
				break;

			case Recode_WhiteTape:
				LCD_Printf("WhiteTape");
				ChooseFunction(Detect_WhiteTape);
				if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_FOREVER) == RT_EOK)
          WhiteTapeRecode(GROUND);
				break;
		}
	}
	
	else if(FUNCTION==TEST)
	{
	  Show:
		while(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) != RT_EOK)
		{
			LCD_Clear();
			if(nowpage==0)
			{
				LCD_SetXY(0,0);
				LCD_Printf("HandoverRecode:");	
				LCD_SetXY(0,1);
				LCD_Printf("1.ss");
				LCD_SetXY(0,2);
				LCD_Printf("2.pw");
				LCD_SetXY(0,3);
				LCD_Printf("3.sw");
				LCD_SetXY(0,4);
				LCD_Printf("4.start");
			}
			else if(nowpage==1)
			{
				LCD_SetXY(0,0);				
				LCD_Printf("CircleRecode:");
				LCD_SetXY(0,1);
				LCD_Printf("1.ss1");
				LCD_SetXY(0,2);
				LCD_Printf("2.ss2");
				LCD_SetXY(0,3);
				LCD_Printf("3.pw");
				LCD_SetXY(0,4);
				LCD_Printf("4.jg");
			}
			else if(nowpage==2)
			{
				LCD_SetXY(0,0);
				LCD_Printf("WhiteTapeRecode:");
				LCD_SetXY(0,1);
				LCD_Printf("1.BLUE_GROUND");
				LCD_SetXY(0,2);
				LCD_Printf("2.RED_GROUND");
				LCD_SetXY(0,3);
				LCD_Printf("PoleRecode");
				LCD_SetXY(0,4);
				LCD_Printf("3.BLUE_GROUND");
				LCD_SetXY(0,5);
				LCD_Printf("4.RED_GROUND");
			}
			else
			{
				LCD_Clear();
				LCD_SetXY(0,1);
				LCD_Printf("page error!");
			}			
				LCD_SetXY(15,7);
				LCD_Printf("%d/%d",nowpage+1,allpage);
				Delay_ms(20);
		}
		 
		switch(key_value)
		{
			case key1:
				if(nowpage==0)
					HandoverRecode(Aim_ss);
				else if(nowpage==1)
					CircleRecode(Circle_ss1);
				else 
					WhiteTapeRecode(BLUE);
				break;
				
			case key2:
				if(nowpage==0)
					HandoverRecode(Aim_pw);
				else if(nowpage==1)
					CircleRecode(Circle_ss2);
				else 
					WhiteTapeRecode(RED);
				break;
				
			case key3:
				if(nowpage==0)
					HandoverRecode(Aim_sw);
				else if(nowpage==1)
					CircleRecode(Circle_pw);
				else
				{					
					ChooseGround_Pole(BLUE);
					PoleRecode(BLUE);
				}
				break;
				
			case key4:
				if(nowpage==0)
					HandoverRecode(Aim_start);
				else if(nowpage==1)
					CircleRecode(Circle_jg);
				else 
				{
					ChooseGround_Pole(RED);
					PoleRecode(RED);
				}
				break;								
				
			case pageup:
			case key9:
				nowpage--;
				if(nowpage<0)
					nowpage = allpage-1;
				goto Show;
				
			case pagedown:
			case key0:
				nowpage++;
				if(nowpage>=allpage)
					nowpage = 0;
				goto Show;
				
			case keyback:
				return;	
		}
	}
}

void Handover(u8 function,u8 place)
{
    u8 temp_state;
    rt_uint32_t key_value;	
    u8 state_count = 0;

    static struct Input_ListValue Values[]=
		{   
			{(void*)&Pid_List[14].p,	    TYPE_FP64,	  "UP_P"}, 
			{(void*)&Pid_List[14].i,      TYPE_FP64,	  "UP_I"}, 
			{(void*)&Pid_List[14].d,      TYPE_FP64,	  "UP_D"},
			
			{(void*)&Pid_List[15].p,	    TYPE_FP64,	  "DOWN_P"}, 
			{(void*)&Pid_List[15].i,      TYPE_FP64,	  "DOWN_I"}, 
			{(void*)&Pid_List[15].d,      TYPE_FP64,	  "DOWN_D"},
			
			{(void*)&Pid_List[16].p,	    TYPE_FP64,	  "Z_P"}, 
			{(void*)&Pid_List[16].i,      TYPE_FP64,	  "Z_I"}, 
			{(void*)&Pid_List[16].d,	    TYPE_FP64,	  "Z_D"},
			
			{(void*)&Pid_List[23].p,	    TYPE_FP64,	  "SW_UP_P"}, 
			{(void*)&Pid_List[23].i,      TYPE_FP64,	  "SW_UP_I"}, 
			{(void*)&Pid_List[23].d,      TYPE_FP64,	  "SW_UP_D"},
			
			{(void*)&Pid_List[24].p,	    TYPE_FP64,	  "SW_DOWN_P"}, 
			{(void*)&Pid_List[24].i,      TYPE_FP64,	  "SW_DOWN_I"}, 
			{(void*)&Pid_List[24].d,      TYPE_FP64,	  "SW_DOWN_D"},
	
			{(void*)&Pid_List[25].p,	    TYPE_FP64,	  "SW_Z_P"}, 
			{(void*)&Pid_List[25].i,      TYPE_FP64,	  "SW_Z_I"}, 
			{(void*)&Pid_List[25].d,      TYPE_FP64,	  "SW_Z_D"},			
			
			{(void*)&Pid_List[17].p,	    TYPE_FP64,	  "Cam_A_P"}, 
			{(void*)&Pid_List[17].i,      TYPE_FP64,	  "Cam_A_I"}, 
      {(void*)&Pid_List[17].d,	    TYPE_FP64,	  "Cam_A_D"},
			
			{(void*)&Aim_X_Handover[0],		 TYPE_INT16,	 "Ax_ss"},
			{(void*)&Aim_Y_Handover[0],		 TYPE_INT16,	 "Ay_ss"},
			{(void*)&Aim_Z_Handover[0],		 TYPE_INT16,	 "Az_ss"},
			{(void*)&Aim_A_Handover[0],		 TYPE_INT16,	 "Aa_ss"},	
			
			{(void*)&Aim_X_Handover[1],		 TYPE_INT16,	 "Ax_pw"},
			{(void*)&Aim_Y_Handover[1],		 TYPE_INT16,	 "Ay_pw"},
			{(void*)&Aim_Z_Handover[1],		 TYPE_INT16,	 "Az_pw"},
      {(void*)&Aim_A_Handover[1],		 TYPE_INT16,	 "Aa_pw"},	
			
			{(void*)&Aim_X_Handover[2],		 TYPE_INT16,	 "Ax_sw"},
			{(void*)&Aim_Y_Handover[2],		 TYPE_INT16,	 "Ay_sw"},
			{(void*)&Aim_Z_Handover[2],		 TYPE_INT16,	 "Az_sw"},
      {(void*)&Aim_A_Handover[2],		 TYPE_INT16,	 "Aa_sw"},										   			
			
			{(void*)&PLACE_HANDOVER,		   TYPE_UINT8,	 "place"},
			{(void*)&Z_SPEED,		           TYPE_FP64,	   "Z_SPEED"},			
			{(void*)&Handover_R,		       TYPE_FP64,	   "Radius"},			
			{(void*)&stop_ex_h,		       TYPE_INT16,	 "stop_err_x"},
			{(void*)&stop_ey_h,		       TYPE_INT16,	 "stop_err_y"},			
			{(void*)&REL,		               TYPE_FP32,	   "rel"},
			
		}; 

    PID_Clear();
    FilterFlag[5] = 1;
    Handover_Z_DISABLE = 1;    
		CamLED_Off();
		
    switch(function)
    {
			case TEST:
				Input_List(Values,39);
				if(PLACE_HANDOVER==Aim_ss)
				{
						HandUD(Motor2_Aim_Pos[1],WAIT);
						SD_Printf_MQ(&cam_file,"HandOver Test Aim_ss\r\n");
				}
				if(PLACE_HANDOVER==Aim_pw)
				{
						HandUD(Motor2_Aim_Pos[3],WAIT);
						SD_Printf_MQ(&cam_file,"HandOver Test Aim_pw\r\n");
				}
				if(PLACE_HANDOVER==Aim_sw)
				{
					  SD_Printf_MQ(&cam_file,"HandOver Test Aim_sw\r\n");
				}					
				while(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) != RT_EOK)				
				{
					Keep_Handover(PLACE_HANDOVER);
					Delay_ms(50);
				}
				
		   	break;
				
			case ARMACTION:
				temp_state = Button8_State;
			  if(place==Aim_ss)
				  SD_Printf_MQ(&cam_file,"HandOver armaction Aim_ss\r\n");
				if(place==Aim_pw)
					SD_Printf_MQ(&cam_file,"HandOver armaction Aim_pw\r\n");
				if(place==Aim_sw)
					SD_Printf_MQ(&cam_file,"HandOver armaction Aim_sw\r\n");
				while(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) != RT_EOK)				
				{
					Keep_Handover(place);			
					if((Abs(Cam_X-Aim_X_Handover[place])<stop_ex_h)&&(Abs(Cam_Y-Aim_Y_Handover[place])<stop_ey_h)&&Abs(Cam_A-Aim_A_Handover[place])<4)
							break;		
					if(Button8_State!=temp_state)
							break;
					Delay_ms(50);
				}						
			  break;
		}

    SPEED_STOP;
	  Moto_Stop(W_MOTOR1_OLD_ID);
		temp_state = Button8_State;
	
		Moto_Set_GSpeed(W_MOTOR2_OLD_ID,Z_SPEED);  
		
		while(CHILD_OFF&&Motor_Pos[1]<4.0f)
		{
			if(Button8_State!=temp_state)
			{
				state_count++;
				temp_state = Button8_State;
			}
			if(state_count==2)
				break;
			
			LCD_Clear();
			LCD_SetXY(0,1);
			LCD_Printf("   Descending   ");
			LCD_SetXY(0,2);
			LCD_Printf("state_count:%d",state_count);
      Delay_ms(5);
		}
		
    Moto_Stop(W_MOTOR2_OLD_ID);
    CamLED_On();
		
    LCD_Clear();
    LCD_SetXY(0,1);
    LCD_Printf("Handver Done!");
		SD_Printf_MQ(&cam_file,"HandOver end\r\n");
		SD_Printf_MQ(&cam_file,"%d %d %d %d %d\r\n",Aim_X_Handover[place],Aim_Y_Handover[place],Aim_Z_Handover[place],Aim_A_Handover[place],state_count);
		
    FilterFlag[5] = 0;
}

void ShowInfo(void)
{
	rt_uint32_t key_value;	
	
  while(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) != RT_EOK)
	{
		LCD_Clear();
		LCD_SetXY(0,0);
    LCD_Printf("H_X:%d",Cam_X);
		LCD_SetXY(10,0);
    LCD_Printf("H_Y:%d",Cam_Y);	
		LCD_SetXY(0,1);
    LCD_Printf("H_Z:%d",Cam_Z);
		LCD_SetXY(10,1);
    LCD_Printf("H_A:%d",Cam_A);	
		LCD_SetXY(0,2);
    LCD_Printf("P_X:%d",Cam_Pole_X);
		LCD_SetXY(10,2);
    LCD_Printf("P_Y:%d",Cam_Pole_Y);	
		LCD_SetXY(0,3);
		LCD_Printf("a_f:%.2f",Cam_Angle[Cam_Circle_Line]);
		LCD_SetXY(10,3);
		LCD_Printf("d_f:%.2f",Cam_Dist[Cam_Circle_Line]);
		LCD_SetXY(0,4);
		LCD_Printf("a_b:%.2f",Cam_Angle[Cam_Line_BLUE]);
		LCD_SetXY(10,4);
		LCD_Printf("d_b:%.2f",Cam_Dist[Cam_Line_BLUE]);
		LCD_SetXY(0,5);
		LCD_Printf("a_r:%.2f",Cam_Angle[Cam_Line_RED]);
		LCD_SetXY(10,5);
		LCD_Printf("d_r:%.2f",Cam_Dist[Cam_Line_RED]);
		Delay_ms(20);
	}
}

void PolePut(u8 function)
{
	u8 temp_state;
	rt_uint32_t key_value;	

  int i;	
	float motor_pos_temp;
	float error_motor;
	s16 error_x,error_y;
	static s16 Cam_X_temp[2] = {0};
  static s16 Cam_Y_temp[2] = {0};
	
  static  struct Input_ListValue Values[]=
	{
		  {(void*)&Pid_List[18].p,	  TYPE_FP64,	    "Cam_X_P"}, 
			{(void*)&Pid_List[18].i,    TYPE_FP64,	    "Cam_X_I"}, 
			{(void*)&Pid_List[18].d,    TYPE_FP64,	    "Cam_X_D"},
			{(void*)&Pid_List[19].p,	  TYPE_FP64,	    "Cam_Y_P"}, 
			{(void*)&Pid_List[19].i,    TYPE_FP64,	    "Cam_Y_I"}, 
			{(void*)&Pid_List[19].d,    TYPE_FP64,	    "Cam_Y_D"},
			{(void*)&Pid_List[20].p,  	TYPE_FP64,	    "Angle_P"}, 
			{(void*)&Pid_List[20].i,    TYPE_FP64,	    "Angle_I"}, 
			{(void*)&Pid_List[20].d,    TYPE_FP64,	    "Angle_D"},
			{(void*)&Aim_X_Pole,		    TYPE_INT16,	        "APx"},
			{(void*)&Aim_Y_Pole,		    TYPE_INT16,	        "APy"},
			{(void*)&Aim_A_Pole,		    TYPE_FP64,	        "APa"},
			{(void*)&REDFlag,		        TYPE_UINT8,	    "REDFlag"},
	};

	FilterFlag[4] = 1;
	PID_Clear();	
  CamLED_Off();	
	
	switch(function)
	{
		case ARMACTION:
			ChooseGround_Pole(GROUND);
		  SD_Printf_MQ(&cam_file,"PolePut armaction\r\n");	
			temp_state = Button8_State;
			if(NewPole)
			{
				while(1)
				{
					Keep_Angle(Angle_Pole);
					if(Abs(GPS.radian - Aim_A_Pole/180.0*pi)<0.015)
						break;
					Delay_ms(5);
				}
				
				Sample:
				for(i=0;i<2;i++)
				{
					Cam_X_temp[i] = Cam_Pole_X;
					Cam_Y_temp[i] = Cam_Pole_Y;
					Delay_ms(150);
				}
				LCD_Clear();
				if(Abs(Cam_X_temp[1]-Cam_X_temp[0])>10||Abs(Cam_Y_temp[1]-Cam_Y_temp[0])>10)
				{
					LCD_SetXY(0,1);
					LCD_Printf("sample error!");
					goto Sample;
				}
				
			 	error_x = Cam_X_temp[0] - Aim_X_Pole;
				error_y = Cam_Y_temp[0] - Aim_Y_Pole;
				
				error_motor = error_y*pole_k_y;
				
				motor_pos_temp = Motor_Pos[0] + error_motor;
				W_MOTOR_OLD_FUNC(W_MOTOR1_OLD_ID,motor_pos_temp,SPEED_MOTO,CMD_SET_PSG);
				
				
				while(Abs(Motor_Pos[0]-motor_pos_temp)>0.1f)
				{
					LCD_Clear();
					LCD_SetXY(0,0);
					LCD_Printf("error_y:%d",error_y);
					LCD_SetXY(0,1);
					LCD_Printf("error_motor:%f",error_motor);	
					LCD_SetXY(0,3);
					LCD_Printf("Motor_Pos:%f",Motor_Pos[0]);	
					LCD_SetXY(0,4);
					LCD_Printf("Aim_Pos:%f",motor_pos_temp);
					if(temp_state!=Button8_State)
						break;
					Delay_ms(5);
				}
				temp_state = Button8_State;
				while(1)
				{
					Keep_Pole();
					if(temp_state!=Button8_State)
						break;
					if( Abs(Cam_Pole_X - Aim_X_Pole)<=2
											&&Abs(Cam_Pole_Y - Aim_Y_Pole)<=2
											&&Abs(GPS.radian - Aim_A_Pole/180.0*pi)<0.015)
						break;
					Delay_ms(50);
				}
			}
			else
			{		
				while(1)
				{
					Keep_Pole();
					if( Abs(Cam_Pole_X - Aim_X_Pole)<=2
											&&Abs(Cam_Pole_Y - Aim_Y_Pole)<=2
											&&Abs(GPS.radian - Aim_A_Pole/180.0*pi)<0.015)
						break;
					if(temp_state!=Button8_State)
						break;
					Delay_ms(50);
				}
		  }
			break;
			
		case TEST:
			Input_List(Values,12);
		  HandUD(Motor2_Aim_Pos[2],WAIT);
		  SD_Printf_MQ(&cam_file,"PolePut test\r\n");
		  if(REDFlag)
				ChooseGround_Pole(RED);
			else 
				ChooseGround_Pole(BLUE);	
     
			while(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) != RT_EOK)
			{
				Keep_Pole();
				Delay_ms(50);
			}
			break;
  }
	
	CamLED_On();
	SPEED_STOP;
	Moto_Stop(W_MOTOR1_OLD_ID);
	Moto_Stop(W_MOTOR2_OLD_ID);
	
	FilterFlag[4] = 0;

  SD_Printf_MQ(&cam_file,"PolePut end\r\n");	
	SD_Printf_MQ(&cam_file,"%d %d %.5f\r\n",Aim_X_Pole,Aim_Y_Pole,Aim_A_Pole);	
	
}

void CamCircle_Static(u8 function,u8 place)
{
	u8 temp_state;
	rt_uint32_t key_value;	
	static  struct Input_ListValue Values[]=
	{
		  {(void*)&Pid_List[12].p,	             TYPE_FP64,	        "DP"}, 
			{(void*)&Pid_List[12].i,               TYPE_FP64,	        "DI"}, 
			{(void*)&Pid_List[12].d,               TYPE_FP64,	        "DD"},			
			{(void*)&Pid_List[13].p,	             TYPE_FP64,	        "C_AP"}, 
			{(void*)&Pid_List[13].i,               TYPE_FP64,	        "C_AI"}, 
			{(void*)&Pid_List[13].d,               TYPE_FP64,	        "C_AD"},
			{(void*)&Pid_List[20].p,	             TYPE_FP64,	        "G_AP"}, 
			{(void*)&Pid_List[20].i,               TYPE_FP64,	        "G_AI"}, 
			{(void*)&Pid_List[20].d,               TYPE_FP64,	        "G_AD"},						
			{(void*)&Aim_A_Circle[Circle_ss1],     TYPE_FP64,	  "Aim_A_ss1"},
			{(void*)&Aim_D_Circle[Circle_ss1],     TYPE_FP64,	  "Aim_D_ss1"},		
			{(void*)&Aim_A_G_Circle[Circle_ss1],   TYPE_FP64,	  "Aim_A_G_ss1"},
			{(void*)&Aim_A_Circle[Circle_ss2],     TYPE_FP64,	  "Aim_A_ss2"},
			{(void*)&Aim_D_Circle[Circle_ss2],     TYPE_FP64,	  "Aim_D_ss2"},		
			{(void*)&Aim_A_G_Circle[Circle_ss2],   TYPE_FP64,	  "Aim_A_G_ss2"},			
			{(void*)&Aim_A_Circle[Circle_pw],      TYPE_FP64,	    "Aim_A_pw"},
			{(void*)&Aim_D_Circle[Circle_pw],      TYPE_FP64,	    "Aim_D_pw"},		
			{(void*)&Aim_A_G_Circle[Circle_pw],    TYPE_FP64,	    "Aim_A_G_pw"},
			{(void*)&Aim_A_Circle[Circle_jg],      TYPE_FP64,	    "Aim_A_jg"},
			{(void*)&Aim_D_Circle[Circle_jg],      TYPE_FP64,	    "Aim_D_jg"},		
			{(void*)&Aim_A_G_Circle[Circle_jg],    TYPE_FP64,	    "Aim_A_G_jg"},			
			{(void*)&PLACE_CIRCLE,		             TYPE_UINT8,    "place"},				
			{(void*)&stop_ea_static,		           TYPE_FP64,    "stop_ea"},
			{(void*)&stop_ed_static,		           TYPE_FP64,    "stop_ed"},							
			{(void*)&SwitchNum,		                 TYPE_INT32,   "SwitchNum"},							
	};	
	
	PID_Clear();
	ChooseFunction(Detect_Circle);	
	
	switch(function)
	{
		case TEST:
			Input_List(Values,25);	
			while(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) != RT_EOK)
			{
				Keep_Circle(PLACE_CIRCLE);
				Delay_ms(5);
			}
			break;
				
		case ARMACTION:
			temp_state = Button8_State;
			while(1)
			{
				Keep_Circle(place);
				if( Abs(Cam_Dist[Cam_Circle_Line] - Aim_D_Circle[place])<stop_ea_static
                    &&Abs(GPS.radian - Aim_A_G_Circle[place]/180.0*pi)<0.02
                    &&Abs(Cam_Angle[Cam_Circle_Line] - Aim_A_Circle[place])<stop_ed_static)
					 break;
				
				if(temp_state!=Button8_State)
					 break;
				Delay_ms(5);
			}
			break;
	}
	
	LCD_Clear();
	LCD_SetXY(0,1);
	LCD_Printf("Circle Done!");
	SPEED_STOP;
}

void CamCircle_Dynamic(u8 function)
{
	rt_uint32_t key_value;	
	int time_interval = 5;
	u8 acc_flag = 0;
	u8 temp_state;
	
  static struct Input_ListValue Values[]=
	{
		  {(void*)&Pid_List[26].p,	  TYPE_FP64,	  "DP"}, 
			{(void*)&Pid_List[26].i,    TYPE_FP64,	  "DI"}, 
			{(void*)&Pid_List[26].d,    TYPE_FP64,	  "DD"},			
			{(void*)&Pid_List[27].p,	  TYPE_FP64,	  "AP"}, 
			{(void*)&Pid_List[27].i,    TYPE_FP64,	  "AI"}, 
			{(void*)&Pid_List[27].d,    TYPE_FP64,	  "AD"},					
			{(void*)&Aim_Angle,         TYPE_FP64,	  "Aim_A"},
			{(void*)&Aim_Dist,          TYPE_FP64,	  "Aim_D"},		
			{(void*)&V_max,             TYPE_FP64,	  "Vmax"},			
			{(void*)&acc_p,             TYPE_FP64,	  "+Acc"},	
			{(void*)&acc_n,             TYPE_FP64,	  "-Acc"},				
			{(void*)&CAM_R,             TYPE_FP64,	  "CAM_R"},
			{(void*)&ea_flag,           TYPE_UINT8,	  "EA_Flag"},					
			{(void*)&stop_ea,           TYPE_FP64,	  "stop_ea"},					
			{(void*)&stop_ed,           TYPE_INT16,	  "stop_ed"},					
	};
	
	ChooseFunction(Detect_Circle);	
	PID_Clear();
	temp_state = Button8_State;

	switch(function)
	{
		case TEST:
		   Input_List(Values,15);
		   temp_state = Button8_State;
		   SD_Printf_MQ(&cam_file,"Circle_Dynamic Test\r\n");
       while(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) != RT_EOK)
			 {
					 Go_CamCircle();
					 Delay_ms(time_interval);
					 if(!acc_flag)
						 Speed_Y += time_interval*acc_p/1000;
					 if(Abs(Speed_Y)>Abs(V_max))
						 Speed_Y = V_max;
					 if(Button8_State!=temp_state)
						 acc_flag = 1;
					 if(acc_flag)
						 Speed_Y -= time_interval*acc_n/1000;
					 if(V_max>0)
					 {
						 if(Speed_Y<0)
							 Speed_Y = 0;			 
					 }
					 else
					 {		
						 if(Speed_Y>0)
							 Speed_Y = 0;
					 }
			 }
			 break;
	 }
		 
	 SPEED_STOP;
}

void CamWhiteTape(u8 function)
{
	u8 temp_state;
	rt_uint32_t key_value;	
// 	u8 angle_ok = 0;
	struct GPS_State temp_cam_gps;
	struct Point temp_stop_point;
	int flag = 0;
    
	
  static struct Input_ListValue Values[]=
	{
		  {(void*)&Pid_List[21].p,	    TYPE_FP64,	  "DP_F"}, 
			{(void*)&Pid_List[21].i,    TYPE_FP64,	  "DI_F"}, 
			{(void*)&Pid_List[21].d,    TYPE_FP64,	  "DD_F"},			
			{(void*)&Pid_List[22].p,	TYPE_FP64,	  "DP_S"}, 
			{(void*)&Pid_List[22].i,    TYPE_FP64,	  "DI_S"}, 
			{(void*)&Pid_List[22].d,    TYPE_FP64,	  "DD_S"},		
			{(void*)&Pid_List[20].p,	TYPE_FP64,	  "AP"}, 
			{(void*)&Pid_List[20].i,    TYPE_FP64,	  "AI"}, 
			{(void*)&Pid_List[20].d,    TYPE_FP64,	  "AD"},
			{(void*)&Pid_List[5].p, 	TYPE_FP64,	  "DP_K"}, 
			{(void*)&Pid_List[5].i,     TYPE_FP64,	  "DI_K"}, 
			{(void*)&Pid_List[5].d,     TYPE_FP64,	  "DD_K"},
      {(void*)&Pid_List[4].p, 	TYPE_FP64,	  "AP_K"}, 
			{(void*)&Pid_List[4].i,     TYPE_FP64,	  "AI_K"}, 
			{(void*)&Pid_List[4].d,     TYPE_FP64,	  "AD_K"},	
			{(void*)&Aim_D_Swing[Cam_Circle_Line],   TYPE_FP64,	  "Aim_D_F"},
			{(void*)&Aim_D_Swing[Cam_Line_BLUE],     TYPE_FP64,	  "Aim_D_S_B"},		
			{(void*)&Aim_D_Swing[Cam_Line_RED],      TYPE_FP64,	  "Aim_D_S_R"},		
			{(void*)&Aim_A_Swing[BLUE],              TYPE_FP64,	  "Aim_A_BLUE"},
      {(void*)&Aim_A_Swing[RED],               TYPE_FP64,	  "Aim_A_RED"},			
			{(void*)&SW_SPEED,                 TYPE_FP64,	  "SW_SPEED"},	
			{(void*)&SW_FORWARD_SPEED,         TYPE_FP64,	  "SW_F_SPEED"},
			{(void*)&SW_FORWARD_DIS,           TYPE_FP64,	  "SW_F_DIS"},
			{(void*)&REDFlag,                  TYPE_INT8,	  "REDFlag"},
			{(void*)&stop_ed1_w,               TYPE_INT16,	"stop_ed1"},
			{(void*)&stop_ed2_w,               TYPE_INT16,	"stop_ed2"},			
	};
	
	ChooseFunction(Detect_WhiteTape);
	PID_Clear();
	FilterFlag[1] = 1;//开始过滤摄像头的数据
	FilterFlag[2] = 1;//开始过滤摄像头的数据
	FilterFlag[3] = 1;//开始过滤摄像头的数据
	
	switch(function)
	{
		case TEST:
			SD_Printf_MQ(&cam_file,"WhiteTape Test\r\n");
			Input_List(Values,26);
				
// 			while(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) != RT_EOK)
// 			{
// 	          if(REDFlag)
// 					 	  Keep_Angle(Angle_Swing_RED);
// 						else 
// 							Keep_Angle(Angle_Swing_BLUE);
// 					if(Abs(GPS.radian-Aim_A_Swing/180.0*pi)<0.01)
// 					{
// 							if(angle_ok==10)
// 								break;	
// 							else
// 								angle_ok++;
// 					}else
// 					{
// 							angle_ok = 0;
// 					}
// 					Delay_ms(5);
// 			}
			
			while(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) != RT_EOK)
			{
				 Keep_Swing(REDFlag);
				 if((Abs(Cam_Dist[Cam_Circle_Line]-Aim_D_Swing[Cam_Circle_Line])<stop_ed1_w)
									&&(Abs(Cam_Dist[REDFlag+1]-Aim_D_Swing[REDFlag+1])<stop_ed2_w)
									&&(Abs(GPS.radian-Aim_A_Swing[REDFlag]/180.0*pi)<0.01))
				 break;
				 Delay_ms(50);
			}
            
			SPEED_STOP;
			
			LCD_Clear();
			LCD_SetXY(0,1);
			LCD_Printf("Modification Done Half!");
			LCD_SetXY(0,2);
			LCD_Printf("Wait for the Button Push!");
      PID_Clear();
            
			Wait_Button8_Change();
			
			temp_cam_gps.position.x = GPS.position.x + SW_FORWARD_DIS*cos(GPS.radian);
			temp_cam_gps.position.y = GPS.position.y + SW_FORWARD_DIS*sin(GPS.radian);
			
			
// 			Cam_F_DISABLE = 1;

// 			while(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) != RT_EOK)
// 			{
// 				Keep_Swing(REDFlag);
// 				Speed_X = SW_FORWARD_SPEED;
// 				if(GetLength(GPS.position,temp_cam_gps.position)>SW_FORWARD_DIS)
// 					break;
// 				Delay_ms(5);
// 			}
			SetKeep(temp_cam_gps.position,GPS.radian,SW_FORWARD_SPEED);
			while(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) != RT_EOK)
			{
				Keep();
				if(GetLength(GPS.position,temp_cam_gps.position)<5)
					break;
				Delay_ms(5);
			}
			
			Cam_F_DISABLE = 0;
			SPEED_STOP;
			LCD_Clear();
			LCD_SetXY(0,1);
			LCD_Printf("Modification Done!");
			
			break;
			
		case ARMACTION:
			SD_Printf_MQ(&cam_file,"WhiteTape Armaction\r\n");
      PID_Clear();
			temp_state = Button8_State;
      temp_stop_point = GPS.position;
			Cam_F_DISABLE = 0;
			while(1)
			{
				if(!Cam_F_DISABLE)
					Keep_Swing(GROUND);
				
				if((Abs(Cam_Dist[Cam_Circle_Line]-Aim_D_Swing[Cam_Circle_Line])<stop_ed1_w)
									&&(Abs(Cam_Dist[GROUND+1]-Aim_D_Swing[GROUND+1])<stop_ed2_w)
									&&(Abs(GPS.radian-Aim_A_Swing[GROUND]/180.0*pi)<0.01)
									&&(Cam_F_DISABLE==0))					
				{
						Cam_F_DISABLE = 1;
						temp_cam_gps = GPS;
				}
							
				if(Cam_F_DISABLE)
				{	
						if(!flag)
						{
							flag = 1;
							PID_Clear();
							HandFB(Motor1_Aim_Pos[4],NO,3500);//放置自动机器人
							temp_cam_gps.position.x = GPS.position.x + SW_FORWARD_DIS*cos(GPS.radian);
							temp_cam_gps.position.y = GPS.position.y + SW_FORWARD_DIS*sin(GPS.radian);
							SetKeep(temp_cam_gps.position,GPS.radian,SW_FORWARD_SPEED);
						}
						
						Keep();
						
						if(GetLength(GPS.position,temp_cam_gps.position)<5)
							SPEED_STOP;
						if(GetLength(GPS.position,temp_cam_gps.position)<5&&Abs(Motor_Pos[0]-Motor1_Aim_Pos[4])<0.1f)
							break;	
						
				}
				
				if(Abs(temp_stop_point.x-GPS.position.x)>300)
				{
						Speed_X = 0;
						LCD_SetXY(0,8);
						LCD_Printf("X out!");
				}
				
				if(Abs(temp_stop_point.y-GPS.position.y)>300)
				{
						Speed_Y = 0;
						LCD_SetXY(10,8);
						LCD_Printf("Y out!");
				}
							
				if(temp_state!=Button8_State)
					 break;
				
				Delay_ms(5);
			}
			
			Cam_F_DISABLE = 0;
			SPEED_STOP;
			LCD_Clear();
			LCD_SetXY(0,1);
			LCD_Printf("Modification Done!");
			
			break;
  }
	
	FilterFlag[1] = 0;
	FilterFlag[2] = 0;
	FilterFlag[3] = 0;
	buz_msg(0,0,0,200);
	
	SD_Printf_MQ(&cam_file,"WhiteTape end\r\n");
	SD_Printf_MQ(&cam_file,"%.5f %.5f %.5f %.5f %.5f\r\n",Aim_A_Swing[BLUE],Aim_A_Swing[RED],Aim_D_Swing[Cam_Circle_Line],Aim_D_Swing[Cam_Line_BLUE],Aim_D_Swing[Cam_Line_RED]);
}
void ShowData(void)
{
  rt_uint32_t key_value;	
	int nowpage = 0;
	int allpage = 5;
	
  ShowData:	
	while(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) != RT_EOK)
	{
		LCD_Clear();
		LCD_SetXY(6,0);
		LCD_Printf("ShowData");
		LCD_SetXY(0,1);
		LCD_Printf("1.Aim");
		LCD_SetXY(0,2);
		LCD_Printf("2.F_Close");
		Delay_ms(20);
	}
	
	switch(key_value)
	{
		case key1:
			goto AimData;
		case key2:
			SD_Printf_MQ(&cam_file,"THE END");
	    LCD_Clear();
		  LCD_SetXY(0,1);
		  LCD_Printf("Done!");
		  Delay_ms(1000);
			goto ShowData;
		case keyback:
			return;
	}
	
AimData:
   while(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) != RT_EOK)
	 {
		 LCD_Clear();
		 if(nowpage==0)
		 {
			 LCD_SetXY(0,0);
			 LCD_Printf("Aim_Handover_ss");
			 LCD_SetXY(0,1);
			 LCD_Printf("Aim_X:%d",Aim_X_Handover[Aim_ss]);
			 LCD_SetXY(0,2);
			 LCD_Printf("Aim_Y:%d",Aim_Y_Handover[Aim_ss]);
			 LCD_SetXY(0,3);
			 LCD_Printf("Aim_Z:%d",Aim_Z_Handover[Aim_ss]);
			 LCD_SetXY(0,4);
			 LCD_Printf("Aim_A:%d",Aim_A_Handover[Aim_ss]);
		 }
		 if(nowpage==1)
		 {
			 LCD_SetXY(0,0);
			 LCD_Printf("Aim_Handover_pw");
			 LCD_SetXY(0,1);
			 LCD_Printf("Aim_X:%d",Aim_X_Handover[Aim_pw]);
			 LCD_SetXY(0,2);
			 LCD_Printf("Aim_Y:%d",Aim_Y_Handover[Aim_pw]);
			 LCD_SetXY(0,3);
			 LCD_Printf("Aim_Z:%d",Aim_Z_Handover[Aim_pw]);
			 LCD_SetXY(0,4);
			 LCD_Printf("Aim_A:%d",Aim_A_Handover[Aim_pw]);
		 }
		 if(nowpage==2)
		 {
			 LCD_SetXY(0,0);
			 LCD_Printf("Aim_PolePut");
			 LCD_SetXY(0,1);
			 LCD_Printf("Aim_X:%d",Aim_X_Pole);
			 LCD_SetXY(0,2);
			 LCD_Printf("Aim_Y:%d",Aim_Y_Pole);
			 LCD_SetXY(0,3);
			 LCD_Printf("Aim_A:%.2f",Aim_A_Pole);
		 }
		 if(nowpage==3)
		 {
			 LCD_SetXY(0,0);
			 LCD_Printf("Aim_WhiteTape");
			 LCD_SetXY(0,1);
			 LCD_Printf("Aim_D_F:%.2f",Aim_D_Swing[Cam_Circle_Line]);
			 LCD_SetXY(0,2);
			 LCD_Printf("Aim_D_S_B:%.2f",Aim_D_Swing[Cam_Line_BLUE]);
			 LCD_SetXY(0,3);
			 LCD_Printf("Aim_D_S_R:%.2f",Aim_D_Swing[Cam_Line_RED]);
			 LCD_SetXY(0,4);
			 LCD_Printf("Aim_A_B:%.2f",Aim_A_Swing[BLUE]);
			 LCD_SetXY(0,5);
			 LCD_Printf("Aim_A_R:%.2f",Aim_A_Swing[RED]);
		 }
		 if(nowpage==4)
		 {
			 LCD_SetXY(0,0);
			 LCD_Printf("Aim_Circle_ss");
			 LCD_SetXY(0,1);
			 LCD_Printf("Aim_D:%.2f",Aim_D_Circle[Circle_ss1]);
			 LCD_SetXY(0,2);
			 LCD_Printf("Aim_A_C:%.2f",Aim_A_Circle[Circle_ss1]);
			 LCD_SetXY(0,3);
			 LCD_Printf("Aim_A_G:%.2f",Aim_A_G_Circle[Circle_ss1]);			 
		 }
		 Delay_ms(20);	
	 }		 
	 switch(key_value)
	 {
			case pageup:
			case key9:
				nowpage--;
				if(nowpage<0)
					nowpage = allpage-1;
				goto AimData;
				
			case pagedown:
			case key0:
				nowpage++;
				if(nowpage>=allpage)
					nowpage = 0;
				goto AimData;
				
			case keyback:
				goto ShowData;
	 }
	 
}


/**--------------------------------------------------+
  | 使用说明： 1：文件在 t_Fatfs 中已经创建并且打开     |
  |           2：调用f_printf(&cam_file, "");写入数据 |
  |           3：当不在往里面写数据，请在合适的时间将文 |
  |              件关闭，调用f_close(&cam_file);      |
  |           4: 最好在一个菜单中调用关闭函数，这样可以 |
  |              避免出意外无法关闭文件                |
  +--------------------------------------------------*/

/**----------------------------------------------------+
  |     create a file to save camera data and open it  |
  +----------------------------------------------------*/

void cam_filcrt(void)   /* camera file create */
{
    /* create */
    f_open(&cam_file, "cam.txt", FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
    f_close(&cam_file);
    f_timestamp("cam.txt");
    f_open(&cam_file, "cam.txt", FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
}


void PolePutTest(void)
{
	s16 error_x,error_y;
	float error_motor;
	double error_down;
  rt_uint32_t key_value;
  int i;	
	float motor_pos_temp;
	double encoder_temp;

	static s16 Cam_X_temp[2] = {0};
  static s16 Cam_Y_temp[2] = {0};

  static  struct Input_ListValue Values[]=
	{
		  {(void*)&Pid_List[18].p,	  TYPE_FP64,	    "Cam_X_P"}, 
			{(void*)&Pid_List[18].i,    TYPE_FP64,	    "Cam_X_I"}, 
			{(void*)&Pid_List[18].d,    TYPE_FP64,	    "Cam_X_D"},
			{(void*)&Pid_List[19].p,	  TYPE_FP64,	    "Cam_Y_P"}, 
			{(void*)&Pid_List[19].i,    TYPE_FP64,	    "Cam_Y_I"}, 
			{(void*)&Pid_List[19].d,    TYPE_FP64,	    "Cam_Y_D"},
			{(void*)&Pid_List[20].p,  	TYPE_FP64,	    "Angle_P"}, 
			{(void*)&Pid_List[20].i,    TYPE_FP64,	    "Angle_I"}, 
			{(void*)&Pid_List[20].d,    TYPE_FP64,	    "Angle_D"},
			{(void*)&Aim_X_Pole,		    TYPE_INT16,	        "APx"},
			{(void*)&Aim_Y_Pole,		    TYPE_INT16,	        "APy"},
			{(void*)&Aim_A_Pole,		    TYPE_FP64,	        "APa"},
			{(void*)&pole_k_y,		      TYPE_FP64,	   "pole_k_y"},
		  {(void*)&pole_k_x,		      TYPE_FP64,	   "pole_k_x"},
			{(void*)&SPEED_MOTO,		    TYPE_FP64,	       "V_up"},
			{(void*)&SPEED_DOWN,		    TYPE_FP64,	     "V_down"},
			
	};

	PID_Clear();
	FilterFlag[4] = 1;
	Input_List(Values,16);
	
	while(1)
	{
		Keep_Angle(Angle_Pole);
		if(Abs(GPS.radian - Aim_A_Pole/180.0*pi)<0.02)
			break;
		Delay_ms(5);
	}

	while(camera_data4.time_flag>100)
	{
		Buz_On();
		Delay_ms(5);
	}
	
	
	Sample:
  for(i=0;i<2;i++)
  {
		Cam_X_temp[i] = Cam_Pole_X;
		Cam_Y_temp[i] = Cam_Pole_Y;
		Delay_ms(150);
  }
	if(Abs(Cam_X_temp[1]-Cam_X_temp[0])>10||Abs(Cam_Y_temp[1]-Cam_Y_temp[0])>10)
		goto Sample;
	
 	error_x = Cam_X_temp[0] - Aim_X_Pole;
	error_y = Cam_Y_temp[0] - Aim_Y_Pole;
	
	error_motor = error_y*pole_k_y;
	error_down = error_x*pole_k_x;

	motor_pos_temp = Motor_Pos[0] + error_motor;
	W_MOTOR_OLD_FUNC(W_MOTOR1_OLD_ID,motor_pos_temp,SPEED_MOTO,CMD_SET_PSG);
	Speed_Y = SPEED_DOWN;
	encoder_temp = Encoder[0].dis;
	while(Abs(Encoder[0].dis-encoder_temp)>error_down)
		Delay_ms(5);
	
// 	while(Abs(Motor_Pos[0]-motor_pos_temp)>0.1f)
// 	{
// 		LCD_Clear();
// 		LCD_SetXY(0,0);
// 		LCD_Printf("error_y:%d",error_y);
// 	  LCD_SetXY(0,1);
// 	  LCD_Printf("error_motor:%f",error_motor);	
// 		LCD_SetXY(0,3);
// 	  LCD_Printf("Motor_Pos:%f",Motor_Pos[0]);	
// 		LCD_SetXY(0,4);
// 		LCD_Printf("Aim_Pos:%f",motor_pos_temp);
// 		if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
// 			break;
// 		Delay_ms(5);
// 	}
	
	while(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) != RT_EOK)
	{
		Keep_Pole();
		Delay_ms(50);
	}
	
	SPEED_STOP;
	Moto_Stop(W_MOTOR1_OLD_ID);
	FilterFlag[4] = 0;
	
}

void CamLED_Toggle(void)
{
	if(relay_attain(22))
		Relay_Clear(22);
	else
		Relay_Set(22);
}

void CamLED_On(void)
{
	Relay_Set(22);
}

void CamLED_Off(void)
{
	Relay_Clear(22);
}
/**along with sys_ImgSave()
  *-----------------------------------------------* 
  | @attention: only called by sys_ImgSave()      |
  *-----------------------------------------------*/
void _show_res(u8 res)
{
    if(res!=camErr_OK)
    {
        u8 line = 1;
        
        LCD_Clear();
        
        if(res & camErr_Timeout)
        {
            line ++;
            LCD_SetXY(0, line);
            LCD_Printf("Err_Timeout");
        }
        if(res & camErr_Param)
        {
            line ++;
            LCD_SetXY(0, line);
            LCD_Printf("Err_Param");
        }
        if(res & camErr_it)
        {
            line ++;
            LCD_SetXY(0, line);
            LCD_Printf("Err_it");
        }
        if(res & camErr_bufof)
        {
            line ++;
            LCD_SetXY(0, line);
            LCD_Printf("Err_bufof");
        }
        if(res & camErr_frame)
        {
            line ++;
            LCD_SetXY(0, line);
            LCD_Printf("Err_frame");
        }
        if(res & camErr_file)
        {
            line ++;
            LCD_SetXY(0, line);
            LCD_Printf("Err_file");
        }
        if(res & camErr_can)
        {
            line ++;
            LCD_SetXY(0, line);
            LCD_Printf("Err_can");
        }
        Delay_ms(3000);
    }
    else
    {
        LCD_Clear();
        LCD_SetXY(0, 3);
        LCD_Printf("transmit OK ^_^");
        Delay_ms(3000);
    }
}


