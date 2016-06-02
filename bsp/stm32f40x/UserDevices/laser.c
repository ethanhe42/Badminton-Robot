#include "includes.h"
#include "math.h"

int32_t Laser_Dist;

fp64 error_a=0;
fp64 error_dx=-50;
fp64 error_dy=22;
u8  laserflag=0;

s32 Laser_Dist;

void Gps_Laser(s32 dist,u8 Laser_XY)
{
	//Laser_Dist=LaserRead();
  if(laserflag==1)
  {
    if(Laser_XY==Laser_X) 
     GPS.position.x=-(dist*sin(GPS.radian+error_a/180.0*pi)+(error_dx+340)*sin(GPS.radian)-error_dy*cos(GPS.radian));
    if(Laser_XY==Laser_Y)
     GPS.position.y=dist*sin(GPS.radian+error_a/180.0*pi)+(error_dx+340)*sin(GPS.radian)-error_dy*cos(GPS.radian);  
  }
  else
  {
    if(Laser_XY==Laser_X)
     Gps_assume[4].x=-(dist*sin(GPS.radian+error_a/180.0*pi)+(error_dx+340)*sin(GPS.radian)-error_dy*cos(GPS.radian));
    if(Laser_XY==Laser_Y)
     Gps_assume[4].y=dist*sin(GPS.radian+error_a/180.0*pi)+(error_dx+340)*sin(GPS.radian)-error_dy*cos(GPS.radian);
  }
}
