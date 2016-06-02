/* Includes ------------------------------------------------------------------*/
#include "includes.h"
#include "math.h"

/* Private typedef -----------------------------------------------------------*/
struct
{
	/*	the size of buffer*/
	const unsigned char buf_s;
	/*	the size used 0~buf_s-1*/
	unsigned char data_s;
	unsigned char head;
	unsigned char end;
	struct
	{
		union fi64_to_u8 coord[9];
	} data[200];
} cock={200,0,200-1,0};

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

double length[3] = {430,130,370};//  0 vertical 1 horizon 2 incline

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


void short2double(uint16_t data){
	static unsigned char doublec=0;// 0-3
	static unsigned char coordc=0;// 0-9

	cock.data[cock.head + 1 == cock.buf_s ? 0 : cock.head + 1].coord[coordc].u16_data[doublec]=data;
	if (doublec == 3)
	{
		doublec = 0;
		if (coordc == 9)
		{
			coordc = 0;
			cock.head = cock.head + 1 == cock.buf_s ? 0 : cock.head++;
			if(cock.data_s==cock.buf_s-1){
				cock.end = cock.end + 1 == cock.end ? 0 : cock.end++;

			}
			else cock.data_s++;
		}
		else
			coordc++;
	}
	else
		doublec++;
}

//             k^2+l^2-(h-z).^2)=x^2+y^2
//                 coord     length         coeff
void expand2q(double c[9],double l[3],double a[5]){
	a[4] = c[6] * c[6] / 4 + c[7] * c[7] / 4 + c[8] * c[8] / 4;
	a[3] = c[3] * c[6] + c[4] * c[7] + c[5] * c[8];
	a[2] = c[3] * c[3] + c[4] * c[4] + c[5]*c[5] + c[0]*c[6] + c[1]*c[7] + c[2]*c[8] - c[8]*l[0];
	a[1] = 2 * c[0] * c[3] + 2 * c[1] * c[4] + 2 * c[2] * c[5] - 2 * c[5] * l[0];
	a[0] = c[0] * c[0] + c[1] * c[1] + c[2] * c[2] - 2 * c[2] * l[0] + l[0] * l[0] - l[1] * l[1] - l[2] * l[2];
}

double pflied(double coeff[],double t){
	return coeff[0] +coeff[3]*t+0.5*coeff[6]*t*t;
}

void getIntersection(double x[3]){
	double data[9];
	double coeffecient[5];
	struct Point root[4];

	double mint=100000;
	unsigned char i;
	if(cock.data_s==0){
		return;
	}
	for(i=0;i<8;i++){
		data[i]=cock.data[cock.head].coord[i].f64_data;
	}
	expand2q(data,length,coeffecient);
	solu_q(coeffecient,root);
	for (i = 0; i < 4; i++)
	{
		if (root[i].y != 0)
		{
			continue;
		}
		if (mint > root[i].x)
		{
			mint = root[i].x;
		}
	}
	if(mint==100000){
// all imaginary roots
		return;
	}
	for(i=0;i<3;i++) x[i]=pflied(&data[i],mint);
}

void Point2theta(double x[3],double theta[2]){
	theta[1]=acos((length[0]-x[2])/length[2]);
	theta[0]=atan2(x[1],x[0]);
}



