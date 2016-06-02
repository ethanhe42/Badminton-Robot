/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MYMATH_H
#define __MYMATH_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "MyTypeDef.h"
/* Exported macro ------------------------------------------------------------*/
#define Max(a,b) (a>b?a:b)
#define Min(a,b) (a<b?a:b)

#define Abs(a)   ((a)>0?(a):-(a))

#define pi		 3.14159265359
/* Exported variables --------------------------------------------------------*/
/* Exported function prototype -----------------------------------------------*/

float MySqrt(float x);
void solu_w(double a, double b, double c,struct Point root[2]);
void get_du_dv(double a, double b, double c, double d, double e, double f, double g[2]);
void solu_q(double a[5],struct Point ro[4]);

double MaxFour(double a,double b,double c,double d);
double GetLength(struct Point a,struct Point b);
double GetLineSlope(struct Point a,struct Point b);
double GetLineAngle(struct Point sta,struct Point end);

double GetDis_P2L(struct Point p,double a,double b,double c);
double GetDis_P2L_PK(struct Point p,struct Point p0,double k);
double GetDis_P2L_PP(struct Point p,struct Point p1,struct Point p2);

struct Point GetFoot_P2L(struct Point p,double a,double b,double c);
struct Point GetFoot_P2L_PK(struct Point p,struct Point p0,double k);
struct Point GetFoot_P2L_PP(struct Point p,struct Point p1,struct Point p2);

#endif 

/**********************************END OF FILE*********************************/
