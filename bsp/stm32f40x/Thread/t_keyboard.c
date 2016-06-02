
#include "includes.h"
#include "M_Func.h"


ALIGN(RT_ALIGN_SIZE)
static char thread_keyboard_stack[1024*3];
struct rt_thread thread_keyboard;

double coeff_x,coeff_y,coeff_r;//赋速度时雅克比矩阵的系数

void TIM2_Init(void)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	u32 timerperiod;
	
	timerperiod = 0XFFFFFFFF;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	TIM_TimeBaseStructure.TIM_Period = timerperiod; 
	TIM_TimeBaseStructure.TIM_Prescaler = 84 - 1; 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); 
	
	
	TIM_Cmd(TIM2, ENABLE);  	
}



/*
 * 函数名: UserDevices_Init
 * 描  述: 开机时外设初始化函数
 * 输  入: 无
 * 输  出: 无
 * 调  用: 内部调用
 */
void UserDevices_Init()
{
	static double robo_L = 564.0;//左右轮距
	static double robo_W = 602.0;//前后轮距
	static double wheel_r = 76.0;
	
    /*ss交接抓取高度3.5
        记值高度2.37
        坐标（-24,35,833，-1）
      pw交接抓取高度3.75167
        记值高度2.50
        坐标（-19,67,816，-1）
    */
	
// 	SetPole(82,66,133.16);//初始化交接以及梅花桩处放自动机器人理想坐标，更改于5.6
//   SetHandover(-23,-30,869,-1,Aim_ss);					//更改于4.30
// 	SetHandover(0,-107,873,-2,Aim_pw);				//更改于4.30
// 	SetSwing(6.2,-0.9,45,BLUE);              //更改于4.23
// 	SetCamCircle(-5.8,27.8,-140.44,Circle_ss1);    //压跷跷板处摄像头圆定位   更改于4.25
// 	SetCamCircle(8.5,-6.1,130,Circle_jg);//初始化楼梯处放置自动机器人的理想坐标
// 	SetGoCamCircle(18,15);

	if(GROUND==BLUE)//changed in 5.12
	{
		SetPole(76,93,135.974157);
		SetHandover(8,6,912,4,Aim_ss);
		SetHandover(-1,-42,767,4,Aim_pw);
		SetSwing(36.000000,28.000000,47.706888,BLUE);
		SetCamCircle(-3.8,0.6,-119.68,Circle_ss1);	
		SetCamCircle(-3.2,7,134.6,Circle_jg);
	}
	else if(GROUND==RED)
	{
		SetPole(79,85,137.93314);
		//SetPole(96,86,140.9775);
		SetHandover(-12,-9,812,0,Aim_ss);//changed in 5.18
		SetHandover(-58,-58,757,-3,Aim_pw);
		SetSwing(31.000000,39.000000,-136.046388,RED);
		SetCamCircle(-5.1,1,53.5850,Circle_ss1);
		
	}
	ChooseGround_Pole(GROUND);
	coeff_x = coeff_y = 1/wheel_r/(2*pi)*60;
	coeff_r = (robo_L+robo_W)/2/wheel_r/(2*pi)*60;
	
	TIM2_Init();
	Fan_Set_Min();//初始化涵道风扇
	//这个函数要放在最后初始化
	Gyro_CheckFloat();
}

/*
 * 函数名: rt_thread_entry_keyboard
 * 描  述: 按键线程的入口函数
 * 输  入: 无
 * 输  出: 无
 * 调  用: 内部调用
 * 说  明: 在该线程内由菜单进入用户函数
 */
static void rt_thread_entry_keyboard(void* parameter)
{
	static u8 pow_on_flag=1;
	
  rt_uint32_t key_value;
	
	if(pow_on_flag)
	{
		pow_on_flag = 0;
		UserDevices_Init();
	}
	
	ShowMenuList();
	
    while (1)
    {
        if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_FOREVER) == RT_EOK)
            KeyNumOperate(key_value);
    }
}

/*
 * 函数名: rt_keyboard_init
 * 描  述: 按键线程的初始化
 * 输  入: 无
 * 输  出: 无
 * 调  用: 外部调用
 */
void rt_keyboard_init( void )
{
	rt_thread_init(&thread_keyboard,
                    "keyboard",
                    rt_thread_entry_keyboard,
                    RT_NULL,
					&thread_keyboard_stack[0], sizeof(thread_keyboard_stack),
					6,5);
    rt_thread_startup(&thread_keyboard);
}
