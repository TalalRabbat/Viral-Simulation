#include "MovementStrategyInterface.h"
#include "LockdownMovementStrategy.h"

LockdownMovement::~LockdownMovement()
{
}

bool LockdownMovement::IsStandStill()
{
   return true;
}

void LockdownMovement::Move(double & x ,double & y ,double & dx ,double & dy ,const bool& infected )
{
}