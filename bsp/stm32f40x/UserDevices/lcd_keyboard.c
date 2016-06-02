
#include "includes.h"
#include "t_lcd_refresh.h"


void LCD_SetXY(u8 x_index,u8 y_index)
{
    LCD_Index = y_index*20 + x_index;
}

void LCD_Clear(void)
{
    int i;
    for(i=0; i<LCD_LEN; i++)
		LCD_Table[i] = ' ';
}

void LCD_WriteChar(char ch)
{
    LCD_Table[LCD_Index] = ch;
	LCD_Index ++;
}

void LCD_WriteString(char *str)
{   
    while(*str)
    {              
        LCD_WriteChar(*str);
        str++;
    }
}

// void LCD_Printf(const char *fmt,...)
// {
//     va_list ap;
//     char string[160];

//     va_start(ap,fmt);
// //     vsprintf(string,fmt,ap);
// 	rt_vsnprintf(string, sizeof(string) - 1, fmt, ap);
//     va_end(ap);
// 	Dma_Printf(3, "%d, \n",strlen(string));
// 	LCD_WriteString(string);
// }
#include "stdarg.h"
#include "stdio.h"
void LCD_Printf(const char *fmt,...)
{
    va_list ap;
    char string[128];

    va_start(ap,fmt);
    vsprintf(string,fmt,ap);
    va_end(ap);
	LCD_WriteString(string);
}

void LCD_WriteInt(int val)
{
    LCD_WriteString(Int2Str(val));
}

void LCD_WriteDouble(double val)
{
	//char string[20];
	//double2char(val,string,'g');
	//LCD_WriteString(string);
	//LCD_Printf("%f",val);
	LCD_WriteNum(val);
}

void LCD_WriteNum(double data)//写double型数，小数点后保留6位，从高位向低位写
{
	char str[5];
	int flag=0,i;
	if(data<0)
	{
		LCD_WriteChar('-');//写负号
		data=-data;
	}
	LCD_WriteInt((int)data);//写整数部分
	data=data-(int)data;
	for(i=0;i<5;i++)//将小数部分保留在一个字符串内
	{
		data=data*10;
		str[i]=(int)data+'0';
		if(str[i]!='0')
		{
			flag=i+1;
		}
		data=data-(int)data;

	}
	if(flag!=0)
		LCD_WriteChar('.');//写小数点
	for(i=0;i<flag;i++)
	{
		LCD_WriteChar(str[i]);//写小数部分
	}	
	for(i=flag;i<6;i++)
	{
		LCD_WriteChar(' ');//小数点后不够6位则补空格
	}
}

void Input_IntValue(int *address,char *name)//整型输入
{
	u32 key_value;
    
	char str[20]={'\0'};
	int temp;
	int i,j;

	str[0]=' ';

	i=1;

	for(;;)
	{
		LCD_Clear();
		LCD_SetXY(0,0);
		LCD_WriteString(name);
		LCD_SetXY(0,1);
		LCD_WriteString("old:");
		LCD_WriteInt(*address);
		LCD_SetXY(0,2);
		LCD_WriteString("new:");
		LCD_WriteString(str);
		
		if(i<=1)
		{
			LCD_SetXY(5,2);
			LCD_WriteString("Value=0!!!");
		}
		
		Delay_ms(5);
		
        if(rt_mb_recv(&Mb_Key, (rt_uint32_t*)&key_value, 0)==RT_EOK)
		{
            switch(key_value)
            {
                case 1://1
                    if(i<14)
                        str[i++]='1';
                break;
                case 2://2
                    if(i<14)
                        str[i++]='2';
                break;
                case 3://3
                    if(i<14)
                        str[i++]='3';
                break;
                case 4://4
                    if(i<14)
                        str[i++]='4';
                break;
                case 7://5
                    if(i<14)
                        str[i++]='5';
                break;
                case 8://6
                    if(i<14)
                        str[i++]='6';
                break;
                case 9://7
                    if(i<14)
                        str[i++]='7';
                break;
                case 10://8
                    if(i<14)
                        str[i++]='8';
                break;
                case 13://9
                    if(i<14)
                        str[i++]='9';
                break;
                case 14://0
                    if(i<14)
                        str[i++]='0';
                break;
				case 16://-
                    if(str[0] == ' ')
                        str[0] = '-';
                    else
                        str[0] = ' ';
                break;
                case 17://del
                    if(i>1)
                    {
                        str[--i]='\0';
                    }
                break;
                case 18://ok
                    temp=0;
                    
                    for(j=1;j<i;j++)//计算输入值
                    {
                        temp=temp*10+str[j]-'0';
                    }
                    if(str[0]=='-')
                        temp=-temp;
                    *address=temp;
                    return;
                
                case 5://cancel
                    return;
            }
        }
	}
}

void Input_DoubleValue(double * address,char *name)//浮点输入
{
	u32 key_value;
    u32 point=0;
	char str[20]={'\0'};
	float temp;
	int i,j;

	str[0]=' ';
	
	i=1;
	point=0;
	for(;;)
	{
		LCD_Clear();
		LCD_SetXY(0,0);
		LCD_WriteString(name);
		LCD_SetXY(0,1);
		LCD_WriteString("old:");
		LCD_WriteNum(*address);
		LCD_SetXY(0,2);
		LCD_WriteString("new:");
		LCD_WriteString(str);
		
		if(i<=1)
		{
			LCD_SetXY(5,2);
			LCD_WriteString("Value=0!!!");
		}
		
		Delay_ms(20);
		
        if(rt_mb_recv(&Mb_Key, (rt_uint32_t*)&key_value, 0)==RT_EOK)
        {
            switch(key_value)
            {
                case 1://1
                    if(i<14)
                        str[i++]='1';
                break;
                case 2://2
                    if(i<14)
                        str[i++]='2';
                break;
                case 3://3
                    if(i<14)
                        str[i++]='3';
                break;
                case 4://4
                    if(i<14)
                        str[i++]='4';
                break;
                case 7://5
                    if(i<14)
                        str[i++]='5';
                break;
                case 8://6
                    if(i<14)
                        str[i++]='6';
                break;
                case 9://7
                    if(i<14)
                        str[i++]='7';
                break;
                case 10://8
                    if(i<14)
                        str[i++]='8';
                break;
                case 13://9
                    if(i<14)
                        str[i++]='9';
                break;
                case 14://0
                    if(i<14)
                        str[i++]='0';
                break;
                case 15://.
                    if(point==0&&i<14)
                    {
                        str[i++]='.';
                        point=1;
                    }
                break;
				case 16://-
                    if(str[0] == ' ')
                        str[0] = '-';
                    else
                        str[0] = ' ';
                break;
                case 17://del
                    if(i>1)
                    {
                        if(str[i-1]=='.')
                            point=0;
                        str[--i]='\0';
                    }
                break;
                case 18://ok
                    temp=0;
                    point=0;
                    for(j=1;j<i;j++)// 
                    {
                        if(str[j]=='.')
                        {
                            point=10;
                        }
                        else if(point==0)
                        {
                            temp=temp*10+str[j]-'0';
                        }
						else
						{
							temp+=(str[j]-'0')/(float)point;
							point=point*10;
						}
                    }
                    if(str[0]=='-')
                        temp=-temp;
                    *address=temp;
                    return;
                
                case 5://cancel
                    return;
            }
        }
	}
}

void Input_FloatValue(float * address,char *name)//浮点输入
{
	u32 key_value;
    u32 point=0;
	char str[20]={'\0'};
	float temp;
	int i,j;

	str[0]=' ';
	
	i=1;
	point=0;
	for(;;)
	{
		LCD_Clear();
		LCD_SetXY(0,0);
		LCD_WriteString(name);
		LCD_SetXY(0,1);
		LCD_WriteString("old:");
		LCD_WriteNum(*address);
		LCD_SetXY(0,2);
		LCD_WriteString("new:");
		LCD_WriteString(str);
		
		if(i<=1)
		{
			LCD_SetXY(5,2);
			LCD_WriteString("Value=0!!!");
		}
		
		Delay_ms(20);
		
        if(rt_mb_recv(&Mb_Key, (rt_uint32_t*)&key_value, 0)==RT_EOK)
        {
            switch(key_value)
            {
                case 1://1
                    if(i<14)
                        str[i++]='1';
                break;
                case 2://2
                    if(i<14)
                        str[i++]='2';
                break;
                case 3://3
                    if(i<14)
                        str[i++]='3';
                break;
                case 4://4
                    if(i<14)
                        str[i++]='4';
                break;
                case 7://5
                    if(i<14)
                        str[i++]='5';
                break;
                case 8://6
                    if(i<14)
                        str[i++]='6';
                break;
                case 9://7
                    if(i<14)
                        str[i++]='7';
                break;
                case 10://8
                    if(i<14)
                        str[i++]='8';
                break;
                case 13://9
                    if(i<14)
                        str[i++]='9';
                break;
                case 14://0
                    if(i<14)
                        str[i++]='0';
                break;
                case 15://.
                    if(point==0&&i<14)
                    {
                        str[i++]='.';
                        point=1;
                    }
                break;
				case 16://-
                    if(str[0] == ' ')
                        str[0] = '-';
                    else
                        str[0] = ' ';
                break;
                case 17://del
                    if(i>1)
                    {
                        if(str[i-1]=='.')
                            point=0;
                        str[--i]='\0';
                    }
                break;
                case 18://ok
                    temp=0;
                    point=0;
                    for(j=1;j<i;j++)// 
                    {
                        if(str[j]=='.')
                        {
                            point=10;
                        }
                        else if(point==0)
                        {
                            temp=temp*10+str[j]-'0';
                        }
						else
						{
							temp+=(str[j]-'0')/(float)point;
							point=point*10;
						}
                    }
                    if(str[0]=='-')
                        temp=-temp;
                    *address=temp;
                    return;
                
                case 5://cancel
                    return;
            }
        }
	}
}

void Input_List(struct Input_ListValue *list,int num)
{
	rt_uint32_t key_value;
	int i,index;
	int allpage=(num-1)/3;
	int nowpage=0;
	fp64 out;

	while(1)
	{
		LCD_Clear();	
		for(i=0;i<3;i++)
		{
			index=nowpage*3+i;
			if(index<num)
			{
				LCD_SetXY(0,i);
				LCD_Printf("%d.",i+1);
				LCD_WriteString(list[index].name);
				LCD_WriteString(":");
				switch(list[index].type)
				{
					case TYPE_INT8:
						LCD_WriteNum(*(int8_t*)(list[index].value));
					break;
					case TYPE_UINT8:
						LCD_WriteNum(*(uint8_t*)(list[index].value));
					break;
					case TYPE_INT16:
						LCD_WriteNum(*(int16_t*)(list[index].value));
					break;
					case TYPE_UINT16:
						LCD_WriteNum(*(uint16_t*)(list[index].value));
					break;
					case TYPE_INT32:
						LCD_WriteNum(*(int32_t*)(list[index].value));
					break;
					case TYPE_UINT32:
						LCD_WriteNum(*(uint32_t*)(list[index].value));
					break;
					case TYPE_INT64:
						LCD_WriteNum(*(int64_t*)(list[index].value));
					break;
					case TYPE_UINT64:
						LCD_WriteNum(*(uint64_t*)(list[index].value));
					break;
					case TYPE_FP32:
					  LCD_WriteNum(*(fp32*)(list[index].value));
					break;
					case TYPE_FP64:
						LCD_WriteNum(*(fp64*)(list[index].value));
					break;
				}
			}
		  else
			{
				LCD_SetXY(0,i);
				LCD_WriteString("                    ");
			}
			LCD_SetXY(0,3);
			LCD_Printf("5.save 7.go %d/%d ",nowpage+1,allpage+1);
		}
		
		if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
		{
			switch(key_value)
			{
				case pageup:
					if(nowpage>0)
						nowpage--;
					else
						nowpage=allpage;
				break;
				case pagedown:
					if(nowpage<allpage)
						nowpage++;
					else
						nowpage=0;
				break;
				case 1:
				case 2:
				case 3:
					index=key_value+nowpage*3-1;
					switch(list[index].type)
					{
						case TYPE_INT8:
							out=(*(int8_t*)(list[index].value));
						break;
						case TYPE_UINT8:
							out=(*(uint8_t*)(list[index].value));
						break;
						case TYPE_INT16:
							out=(*(int16_t*)(list[index].value));
						break;
						case TYPE_UINT16:
							out=(*(uint16_t*)(list[index].value));
						break;
						case TYPE_INT32:
							out=(*(int32_t*)(list[index].value));
						break;
						case TYPE_UINT32:
							out=(*(uint32_t*)(list[index].value));
						break;
						case TYPE_INT64:
							out=(*(int64_t*)(list[index].value));
						break;
						case TYPE_UINT64:
							out=(*(uint64_t*)(list[index].value));
						break;
						case TYPE_FP32:
							out=(*(fp32*)(list[index].value));
						break;
						case TYPE_FP64:
							out=(*(fp64*)(list[index].value));
						break;
					}
					Input_DoubleValue(&out,list[index].name);
					switch(list[index].type)
					{
						case TYPE_INT8:
							(*(int8_t*)(list[index].value))=out;
						break;
						case TYPE_UINT8:
							(*(uint8_t*)(list[index].value))=out;
						break;
						case TYPE_INT16:
							(*(int16_t*)(list[index].value))=out;
						break;
						case TYPE_UINT16:
							(*(uint16_t*)(list[index].value))=out;
						break;
						case TYPE_INT32:
							(*(int32_t*)(list[index].value))=out;
						break;
						case TYPE_UINT32:
							(*(uint32_t*)(list[index].value))=out;
						break;
						case TYPE_INT64:
							(*(int64_t*)(list[index].value))=out;
						break;
						case TYPE_UINT64:
							(*(uint64_t*)(list[index].value))=out;
						break;
						case TYPE_FP32:
							(*(fp32*)(list[index].value))=out;
						break;
						case TYPE_FP64:
							(*(fp64*)(list[index].value))=out;
						break;
					}
				break;
				case key7:
					return;
			}
		}
		Delay_ms(5);
	}
}



