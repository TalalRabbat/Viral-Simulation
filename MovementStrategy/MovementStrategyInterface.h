// MovementStrategyInterface.h
// /////////////////////////////////
class MovementStrategyInterface
{
   public:
   virtual bool IsStandStill() = 0;
   virtual void Move(double & x ,double & y ,double & dx ,double & dy ,const bool& infected ) = 0;
   virtual ~MovementStrategyInterface(){};
};
