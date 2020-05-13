// LockdownMovementStrategy.h
// /////////////////////////////////
class LockdownMovement : public MovementStrategyInterface
{
   public:
   virtual bool IsStandStill();
   virtual void Move(double & x ,double & y ,double & dx ,double & dy ,const bool& infected );
   virtual ~LockdownMovement();
};