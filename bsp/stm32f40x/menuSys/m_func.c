
/* Includes ------------------------------------------------------------------*/
#include "includes.h"
#include "M_TypeDef.h"
#include "M_List.h"
#include "M_Func.h"

/* Private typedef -----------------------------------------------------------*/
//typedef void(* func)(void);

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*
 * 函数名: ShowMenuList
 * 描  述: 菜单显示
 * 输  入: 无
 * 输  出: 无
 * 调  用: 外部调用
 */
void ShowMenuList(void)
{
    int i=0,j=0;
    struct M_Type *M_TypePtr;
    char *str;
	
	LCD_Clear();
	
    for (i=0; i<2; i++)//两列显示，每列4个
        for(j=0;j<M_NUM/2;j++)
        {
            M_TypePtr = CurrentMenu + (i*(M_NUM/2) + j);
            str = M_TypePtr->Name;
						LCD_SetXY(i*10,j);
            LCD_WriteString(str);
        }
}

/*
 * 函数名: KeyNumOperate
 * 描  述: 按键响应
 * 输  入: 无
 * 输  出: 无
 * 调  用: 外部调用
 */
void KeyNumOperate(u8 KeyValue)
{
    switch (KeyValue)
    {           
        case key1:
			GoKeyNum(1);
            break;
        case key2:
			GoKeyNum(2);
            break;
        case key3:
			GoKeyNum(3);
            break;
        case key4:
			GoKeyNum(4);
            break;
        case 5:
            if(CurrentMenu != MainMenu)
                CurrentMenu = CurrentMenu->Mnode.prev;
            break;
        case key5:
			GoKeyNum(5);
            break;
        case key6:
			GoKeyNum(6);
            break;
        case key7:
			GoKeyNum(7);
            break;
        case key8:
			GoKeyNum(8);
            break;
        case key9:
			GoKeyNum(9);
            break;
        default:
            break;
    }
	ShowMenuList();
}

/*
 * 函数名: GoKeyNum
 * 描  述: 按键响应处理函数
 * 输  入: -KeyNum 传入的键值
 * 输  出: 无
 * 调  用: 内部调用
 */
void GoKeyNum(int KeyNum)
{
    struct M_Type *MenuTemp;
    
    MenuTemp = CurrentMenu + KeyNum -1;
    
    if(MenuTemp->M_Attrib == M_SMenu)//如果该子菜单的属性是菜单，显示该子菜单列表
        CurrentMenu = MenuTemp->Mnode.next;
    else if(MenuTemp->M_Attrib == M_UFunc)//属性是用户函数，则执行函数
			MenuTemp->MenuFunc();
    else if(MenuTemp->M_Attrib == M_Empty) {;}//属性是空，则执行一句空语句
}

/********************************    END OF FILE    ***************************/

