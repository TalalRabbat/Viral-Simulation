// RegularMovementStrategy.h
// /////////////////////////////////
class RegularMovement : public MovementStrategyInterface
{
   public:
   virtual bool IsStandStill();
   virtual void Move(double & x ,double & y ,double & dx ,double & dy ,const bool& infected );
   virtual ~RegularMovement();
};
