#include  <iostream>
#include "BorderLimit.h"




	
int ReturnLimit(int py, int px){
	return border[py][px];
}

bool isWall(int py, int px)
{
	return (border[py][px]&walflagbit1)==walflagbit1;
}

bool  isDragon(int py, int px)
{

	return (border[py][px]&dragonflagbit2)==dragonflagbit2;
}

bool  isTrap(int py, int px){
	/*if (border[py][px]==4)
		return true;
	else 
		return false;*/
	return (border[py][px]&trapflagbit3)==trapflagbit3;
}
bool  isArrow(int py, int px){return (border[py][px]&arrowflagbit4)==arrowflagbit4;}
bool  isDoor(int py, int px){return (border[py][px]&doorflagbit5)==doorflagbit5;}

bool isHazzard(int py, int px)
{
	return isArrow(py,px)||isTrap(py,px);
}

void setWall(int py, int px){border[py][px]|=walflagbit1;}
void setDragon(int py, int px){border[py][px]|=dragonflagbit2;}
void setTrap(int py, int px){border[py][px]|=trapflagbit3;}
void setArrow(int py, int px){border[py][px]|=arrowflagbit4;}
void setDoor(int py, int px){border[py][px]|=doorflagbit5;}

void removeWall(int py, int px){border[py][px]&=~walflagbit1;}
void removeDragon(int py, int px){border[py][px]&=~dragonflagbit2;}
void removeTrap(int py, int px){border[py][px]&=~trapflagbit3;}
void removeArrow(int py, int px){border[py][px]&=~arrowflagbit4;}
void removeDoor(int py, int px){border[py][px]&=~doorflagbit5;}