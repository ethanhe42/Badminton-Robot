
/* Includes ------------------------------------------------------------------*/
#include "string.h"
#include "includes.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
char DataConvBuff[25] = {0};

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*
 * 函数名: Int2Str
 * 描  述: 整型转换为字符串
 * 输  入: -IntData 整型数据
 * 输  出: 字符型指针
 * 调  用: 外部调用
 */
char *Int2Str(int IntData)
{
    int     i, d;
    int     flag = 0;
    char    *ptr = DataConvBuff;

    if (!IntData)
    {
        *ptr++ = 0x30;     
        *ptr = 0;       
        return DataConvBuff;
    }
    
    if (IntData < 0)
    {
        *ptr++ = '-';  
        IntData *= -1;
    }
    
    for (i = 1000000000; i > 0; i /= 10)
    {
        d = IntData / i;
        if (d || flag)
        {
            *ptr++ = (char)(d + 0x30);     
            IntData -= (d * i);     
            flag = 1;
        }
    }
    *ptr = 0;

    return DataConvBuff;
}


/*
 * 函数名: Float2Str
 * 描  述: 浮点数转换为字符串
 * 输  入: -FloatData 浮点数
 *         -PointNum 要保留的小数位数
 * 输  出: 字符型指针
 * 调  用: 外部调用
 */
char *Float2Str(float FloatData, int PointNum)
{
	char *ptr = DataConvBuff;
    char *temp;
    
    if(FloatData<0 && FloatData>-1)
		*ptr++ = '-';
    
    /*将整数部分存入*/
    temp = Int2Str((int)FloatData);
	strcpy(ptr, temp);
    ptr += strlen(temp);
	
    /*将小数部分存入*/
    if(PointNum>0)
	{
		int i;
		*ptr++ = '.';
		if(FloatData<0)
            {FloatData = -FloatData;}
		FloatData=FloatData-(int)FloatData;
		for(i=0; i<PointNum; i++)
		{
			FloatData *=  10;
			*ptr++     =  ((int)FloatData+'0');
			FloatData  =  FloatData - (int)FloatData;
		}
		*ptr = '\0';
	}
    return DataConvBuff;
}

/*
 * 函数名: GetSimpFile
 * 描  述: 得到__FILE__的文件路径的最后一个文件
 * 输  入: 
 * 输  出: 字符型指针
 * 调  用: 外部调用
 * 说  明: 例如：__FILE__ == "application\menuSys\MenuFunc.c",则返回“MenuFunc.c”
 */
char *GetSimpFile(char *str)
{
    char *ptr = str;
    
    /*指向该字符串的最后一个字符*/
    ptr += strlen(str) - 1;
    for(; ptr-str>0; ptr--)
        {if(*ptr == '\\')    break;}
        
    return ++ptr;
}
