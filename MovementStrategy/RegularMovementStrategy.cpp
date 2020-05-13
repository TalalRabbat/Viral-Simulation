#include "MovementStrategyInterface.h"
#include "RegularMovementStrategy.h"

RegularMovement::~RegularMovement()
{
}

bool RegularMovement::IsStandStill()
{
   return false;
}

void RegularMovement::Move(double & x ,double & y ,double & dx ,double & dy ,const bool& infected )
{
}
