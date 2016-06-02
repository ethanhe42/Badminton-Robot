
/* Includes ------------------------------------------------------------------*/
#include "includes.h"
#include "M_TypeDef.h"
#include "M_List.h"

#include "M_Sys.h"
#include "M_Test.h"
#include "M_Route.h"
#include "liwenlin.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/*菜单*/
struct M_Type MainMenu[M_NUM];
struct M_Type My_Pro[M_NUM];
struct M_Type M_System[M_NUM];
struct M_Type M_Test[M_NUM];
struct M_Type M_Route[M_NUM];
struct M_Type M_SD[M_NUM];
struct M_Type M_Fxh[M_NUM];
struct M_Type M_liwenlin[M_NUM];

struct M_Type *CurrentMenu = MainMenu;//初始化当前菜单为主菜单


//主菜单下的子菜单
struct M_Type MainMenu[M_NUM]=
{   /*Name          菜单属性    	函数			节点*/
    {"1.System",	M_SMenu,		NULL,			{MainMenu, M_System} },
    {"2.Test",	  	M_SMenu,        NULL,     		{MainMenu, M_Test}   },
    {"3.Route",		M_SMenu,        NULL,	    	{MainMenu, M_Route}  },
    {"4.SD",        M_SMenu,        NULL,     		{MainMenu, M_SD}	 },
    {"5.MY_PRO",       M_SMenu,        NULL,     		{MainMenu, My_Pro}	 },
    {"6.liwenlin",  M_SMenu,        NULL,			{MainMenu, M_liwenlin} 	 },
		{" ",           M_Empty,        NULL,			     {MainMenu, NULL} },
		{" ",           M_Empty,        NULL,			     {MainMenu, NULL} },
};

struct M_Type My_Pro[M_NUM]=
{   /*Name          菜单属性    	函数			节点*/
    {"1.Catching",     M_UFunc,        Text_Catch,		 {MainMenu, NULL} },
    {"2.RecKin_Print",    M_UFunc,        RecKin_Print,			   {MainMenu, NULL} },
		{"3.Syn_Timer ",    M_UFunc,        sys_Timer,     {MainMenu, NULL} },
    {"4.Find_Handle_Key",           M_UFunc,        Find_Handle_Key,            {MainMenu, NULL} },
    {"6.GoRoute",   M_UFunc,        GoRoute,     	{MainMenu, NULL} 	 },
		{"7.LookIt",	  M_UFunc,        LookIt,     		{MainMenu, NULL}   },
    {"8.RPID"   ,   M_UFunc,        RotatePID,     		{MainMenu, NULL} },
};

//用来放系统配置函数
struct M_Type M_System[M_NUM]=
{   /*Name			菜单属性		函数			节点(上下级指针)*/
    {"1.Gyro",      M_UFunc,        sys_Gyro,   	{MainMenu, NULL} },
    {"2.Encoder",   M_UFunc,        sys_Encoder,	{MainMenu, NULL} },
    {"3.GPS",       M_UFunc,        sys_GPS,    	{MainMenu, NULL} },
    {"4.Swedish",   M_UFunc,        sys_Swedish,	{MainMenu, NULL} },
    {"5.SA-ST",     M_UFunc,        sys_SA_ST,  	{MainMenu, NULL} },
    {"6.CAN",       M_UFunc,        sys_CAN,    	{MainMenu, NULL} },
    {"7.Handle",    M_UFunc,        sys_Handle, 	{MainMenu, NULL} },
    {"8.fan",       M_UFunc,        sys_Fan,     	{MainMenu, NULL} },
};
//用来放外设测试函数
struct  M_Type M_Test[M_NUM]=
{   /*Name          菜单属性		函数		 	节点(上下级指针)*/
    {"1.DataBack",  M_UFunc,        DataBackTest,	{MainMenu, NULL} },
    {"2.Camera ",   M_UFunc,        CameraTest,		{MainMenu, NULL} },
    {"3.Laser",     M_UFunc,        LaserTest,		{MainMenu, NULL} },
    {"4.Hall",      M_UFunc,        HallTest,     	{MainMenu, NULL} },
    {"5.LB",        M_UFunc,        LBTest,			{MainMenu, NULL} },
    {"6.Sonic",     M_UFunc,        SonicTest,     	{MainMenu, NULL} },
    {"7.Radar",     M_UFunc,        RadarTest,		{MainMenu, NULL} },
    {"8.owen",      M_UFunc,        owenTest,		{MainMenu, NULL} },
};
//用来放路径相关函数
struct M_Type M_Route[M_NUM]=
{   /*Name          菜单属性		函数			节点(上下级指针)*/
    {"1.GoLine",    M_UFunc,        GoLineTest,		     {MainMenu, NULL} },
    {"2.Keep",  	M_UFunc,        KeepTest,		     {MainMenu, NULL} },
    {"3.GoCircle", 	M_UFunc,        GoCircleTest,        {MainMenu, NULL} },
    {"4.RouteTest", M_UFunc,        RouteTest,		     {MainMenu, NULL} },
    {"5.ArmTest",   M_UFunc,        ArmTest,			 {MainMenu, NULL} },
    {"6.MotorPos ", M_UFunc,        MotorPos,			 {MainMenu, NULL} },
    {" ",           M_Empty,        NULL,			     {MainMenu, NULL} },
    {" ",           M_Empty,        NULL,			     {MainMenu, NULL} },
};

struct M_Type M_SD[M_NUM]=
{   /*Name          菜单属性		函数			节点(上下级指针)*/
    {"1.Img",    	M_UFunc,    	sys_ImgSave,	{MainMenu, NULL} },
    {"2.ReInit",    M_UFunc,        SD_ReInit,		{MainMenu, NULL} },
    {"3.WalkGRD",   M_UFunc,        SD_Walkground,	{MainMenu, NULL} },
    {" ",           M_Empty,        NULL,			     {MainMenu, NULL} },
    {" ",           M_Empty,        NULL,			     {MainMenu, NULL} },
    {" ",           M_Empty,        NULL,			     {MainMenu, NULL} },
    {" ",           M_Empty,        NULL,			     {MainMenu, NULL} },
    {" ",           M_Empty,        NULL,			     {MainMenu, NULL} },
};

struct M_Type M_Laser[M_NUM];
struct M_Type M_Fxh[M_NUM]=
{   /*Name          菜单属性		函数			节点(上下级指针)*/
    {"1.laser",     M_SMenu,        NULL,		         {MainMenu, M_Laser} },
    {"2.keep",      M_UFunc,        sys_keep,			 {MainMenu, NULL} },
    {"3.debug",     M_UFunc,        sys_debug,		     {MainMenu, NULL} },
    {"4.sonic",     M_UFunc,        sys_sonic,		     {MainMenu, NULL} },
    {"5.misc",      M_UFunc,        sys_misc,            {MainMenu, NULL} },
    {"6.lb",        M_UFunc,        sys_lb,			     {MainMenu, NULL} },
    {" ",           M_Empty,        NULL,			     {MainMenu, NULL} },
    {" ",           M_Empty,        NULL,			     {MainMenu, NULL} },
};

struct M_Type M_Laser[M_NUM] = 
{
    {"1.lssp",      M_UFunc,        sys_lssp,		     {&M_Fxh[0], NULL} },
    {"2.lswg",      M_UFunc,        sys_lswg,			 {&M_Fxh[0], NULL} },
    {" ",           M_Empty,        NULL,			     {&M_Fxh[0], NULL} },
    {" ",           M_Empty,        NULL,			     {&M_Fxh[0], NULL} },
    {" ",           M_Empty,        NULL,			     {&M_Fxh[0], NULL} },
    {" ",           M_Empty,        NULL,			     {&M_Fxh[0], NULL} },
    {" ",           M_Empty,        NULL,			     {&M_Fxh[0], NULL} },
	{" ",           M_Empty,        NULL,			     {&M_Fxh[0], NULL} },
};

/****************控制上层电机，电磁阀，ＳＡ－ＳＴ******************/
struct M_Type M_liwenlin[M_NUM]=
{
    {"1.upmotors",      M_UFunc,        Up_motor,		     {MainMenu, NULL} },
    {"2.cylinder",      M_UFunc,        Cylinder,		  	 {MainMenu, NULL} },
    {"3.sa-st ",        M_UFunc,        Sa_St,			     {MainMenu, NULL} },
    {"4.stepper ",      M_UFunc,        NULL,			     {MainMenu, NULL} },
    {"5.st ",           M_Empty,        NULL,			     {MainMenu, NULL} },
    {"6.key",           M_UFunc,        key,			     {MainMenu, NULL} },
    {" ",           M_Empty,        NULL,			     {MainMenu, NULL} },
	{" ",           M_Empty,        NULL,			     {MainMenu, NULL} },
};

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/



/********************************    END OF FILE    ***************************/
