/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MENUCOCK_H
#define __MENUCOCK_H
/* Includes ------------------------------------------------------------------*/
/* variable ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
void short2double(uint16_t data);
void RecPoint(void);
void expand2q(double c[9],double l[3],double a[5]);
double pflied(double coeff[],double t);
void getIntersection(double x[3]);
void Point2theta(double x[3],double theta[2]);
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/

#endif

/**********************************END OF FILE*********************************/
