
/* Includes ------------------------------------------------------------------*/
#include "includes.h"
#include <math.h>

/*
 *  fast sqrt root
 *
 * @author heyihui
 * @version 1.0
 * @see
 * @param x
 * @return x^0.5
 */
float MySqrt(float x){
	float xhalf = 0.5f*x;
	int i = *(int*)&x; // get bits for floating VALUE
	i = 0x5f375a86 - (i >> 1); // gives initial guess y0
	x = *(float*)&i; // convert bits BACK to float
	x = x*(1.5f - xhalf*x*x); // Newton step, repeating increases accuracy
	x = x*(1.5f - xhalf*x*x);
	x = x*(1.5f - xhalf*x*x);
	return 1 / x;
}

/*
 * solution to quadratic equation with one unknown
 *
 * @author heyihui
 * @version 1.0
 * @see MySqrt
 * @param ax2+bx+c=0
 * @return 2 root in Point form x+y*i;
 */

void solu_w(double a, double b, double c,struct Point root[2]){
	double tem, tem1;
	double m, n;
	tem1 = b*b - 4 * a*c;
	m = (-1)*b / (2 * a);
	if (tem1 >= 0)
	{
		tem = MySqrt(tem1);
		n = tem / (2 * a);
//		cout << "\n方程根为：" << '\n' << "\t X1 = " << m + n << '\n' << "\t X2 = " << m - n << '\n';
		root[0].x=m+n;
		root[1].x=m-n;
		root[0].y=root[1].y=0;

	}
	else
	{
		tem = MySqrt((-1)*tem1);
		n = tem / (2 * a);
//		cout << "\n方程根为：" << '\n' << "\t X1 = " << m << " + " << n << " i" << '\n' << "\t X2 = " << m << " - " << n << " i" << '\n' << endl;
		root[0].x=root[1].x=m;
		root[1].y=-(root[0].y=n);

	}
}

/*
 * nonhomogeneous second order system of linear equations
 *
 * @author heyihui
 * @version 1.0
 * @see
 * @param
 * @return
 */

void get_du_dv(double a, double b, double c, double d, double e, double f, double g[2]){
	g[0] = (b*d - a*e) / (c*e - b*f);
	g[1] = (c*d - a*f) / (b*f - c*e);
}
/*
 * quartic equation with one unknown
 *
 * @author heyihui
 * @version 1.0
 * @see get_du_dv
 * @param a[4]*x4+a[3]*x3+a[2]*x2+a[1]*x+a[0]=0
 * @return 4 roots in Point form
 */
void solu_q(double a[5], struct Point ro[4])
{
	double accuracy = 0.00000001;

	double u = -2, v = 1;
	double p0, p1, p2, r0, r1;
	double r0v, r1v, r0u, r1u;
	double d[2];
	unsigned char i;
	for (i = 0; i < 50; i++)
	{
		p2 = a[4];
		p1 = a[3] - u * p2;
		p0 = a[2] - v * p2 - u * p1;
		r0 = a[1] - v * p1 - u * p0;
		r1 = a[0] - v * p0;
		r0v = u * p2 - p1;
		r1v = v * p2 - p0;
		r0u = r1v - u * r0v;
		r1u = (-1) * v * r0v;
		get_du_dv(r0, r0u, r0v, r1, r1u, r1v, d);
		u = u + d[1];
		v = v + d[0];
		if ((Abs(d[1])) < accuracy && (Abs(d[0])) < accuracy)
		{
			solu_w(1, u, v, ro);
			solu_w(p2, p1, p0, ro + 2);
			break;
		}
	}
}

/*
 * 函数名: MaxFour
 * 描  述: 求四个数中的最大值
 * 输  入: -a -b -c -d 四个数据
 * 输  出: 四个数中的最大值
 * 调  用: 外部调用
 */
double MaxFour(double a,double b,double c,double d)
{
	double maxtemp1,maxtemp2;
	maxtemp1=Max(a,b);
	maxtemp2=Max(c,d);
	return Max(maxtemp1,maxtemp2);
}
 
/*
 * 函数名: GetLength
 * 描  述: 求两个点之间的距离
 * 输  入: -a -b 两个点
 * 输  出: 两个点之间的距离
 * 调  用: 外部调用
 */
double GetLength(struct Point a,struct Point b)
{
	double length;
	double dx,dy;
	dx = (a.x-b.x)*(a.x-b.x);
	dy = (a.y-b.y)*(a.y-b.y);
	length = sqrt(dx+dy);
	return length;
}

/*
 * 函数名: GetLineSlope
 * 描  述: 求两个点连线的斜率
 * 输  入: -a -b 两个点
 * 输  出: 两个点连线的斜率
 * 调  用: 外部调用
 */
double GetLineSlope(struct Point a,struct Point b)
{
	double slope;
	if(Abs(a.x-b.x) <= 0.02)
		slope=pi/2;
	else
		slope = (b.y-a.y)/(b.x-a.x);
	
	return slope;
}

/*
 * 函数名: GetLineAngle
 * 描  述: 求两个点之间连线的倾角(弧度)
 * 输  入: -sta 起始点
 *		   -end 结束点
 * 输  出: 两个点连线的倾角(弧度-PI~PI)
 * 调  用: 外部调用
 */
double GetLineAngle(struct Point sta,struct Point end)
{
	double radian;
	if(Abs(sta.x-end.x) <= 0.02)
	{
		if(sta.y>end.y) radian=-pi/2;
		else radian=pi/2;
	}
	else
	{
		radian = atan((end.y-sta.y)/(end.x-sta.x));
		
		//atan值域为第四和第一象限，所以要通过判断分离出二、三象限的部分
		if(sta.x>end.x && sta.y<end.y) radian+=pi;
		else if(sta.x>end.x && sta.y>end.y) radian-=pi;
	}
	
	return radian;
}

/****************************各种点到直线方程求距离******************************/

/*
 * 函数名: GetDis_P2L
 * 描  述: 求点到直线的距离(已知一般式: ax+by+c=0)
 * 输  入: -p 点
 *		   -a -b -c 直线方程的系数
 * 输  出: 点到直线的距离
 * 调  用: 外部调用
 */
double GetDis_P2L(struct Point p,double a,double b,double c)
{
	return Abs(a*p.x+b*p.y+c)/(sqrt(a*a+b*b));
}

/*
 * 函数名: GetDis_P2L_PP
 * 描  述: 求点到直线的距离(已知两点式: (y-y1)(x2-x1)=(y2-y1)(x-x1))
 * 输  入: -p  点
 *		   -p1 -p2 直线上两点
 * 输  出: 点到直线的距离
 * 调  用: 外部调用
 */
double GetDis_P2L_PP(struct Point p,struct Point p1,struct Point p2)
{
	double a,b,c;
	a=p2.y-p1.y;
	b=p1.x-p2.x;
	c=p2.x*p1.y-p1.x*p2.y;
	
	return GetDis_P2L(p,a,b,c);
}

/*
 * 函数名: GetDis_P2L_PK
 * 描  述: 求点到直线的距离(已知点斜式: y-y0=k(x-x0))
 * 输  入: -p  点
 *		   -p0 直线上一点
		   -k  直线方程的斜率
 * 输  出: 点到直线的距离
 * 调  用: 外部调用
 */
double GetDis_P2L_PK(struct Point p,struct Point p0,double k)
{
	double a,b,c;
	a=k;
	b=-1;
	c=-k*p0.x+p0.y;
	
	return GetDis_P2L(p,a,b,c);
}

/****************************各种点到直线方程求垂足******************************/

/*
 * 函数名: GetFoot_P2L
 * 描  述: 求点到直线的垂足(已知一般式: ax+by+c=0)
 * 输  入: -p 点
 *		   -a -b -c 直线方程的系数
 * 输  出: 垂足
 * 调  用: 外部调用
 */
struct Point GetFoot_P2L(struct Point p,double a,double b,double c)
{
	struct Point foot;
	foot.x=(b*b*p.x-a*b*p.y-a*c)/(a*a+b*b);
	foot.y=(a*a*p.y-a*b*p.x-b*c)/(a*a+b*b);
	
	return foot;
}

/*
 * 函数名: GetFoot_P2L_PP
 * 描  述: 求点到直线的垂足(已知两点式: (y-y1)(x2-x1)=(y2-y1)(x-x1))
 * 输  入: -p  点
 *		   -p1 -p2 直线上两点
 * 输  出: 垂足
 * 调  用: 外部调用
 */
struct Point GetFoot_P2L_PP(struct Point p,struct Point p1,struct Point p2)
{
	struct Point foot;
	double a,b,c;
	a=p2.y-p1.y;
	b=p1.x-p2.x;
	c=p2.x*p1.y-p1.x*p2.y;
	
	foot = GetFoot_P2L(p,a,b,c);
	return foot;
}

/*
 * 函数名: GetFoot_P2L_PK
 * 描  述: 求点到直线的垂足(已知点斜式: y-y0=k(x-x0))
 * 输  入: -p  点
 *		   -p0 直线上一点
		   -k  直线方程的斜率
 * 输  出: 垂足
 * 调  用: 外部调用
 */
struct Point GetFoot_P2L_PK(struct Point p,struct Point p0,double k)
{
	struct Point foot;
	double a,b,c;
	a=k;
	b=-1;
	c=-k*p0.x+p0.y;
	
	foot = GetFoot_P2L(p,a,b,c);
	return foot;
}

/**********************************END OF FILE*********************************/

