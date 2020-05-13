// Corona Simulation - basic simulation of a human transmissable virus
// Copyright (C) 2020  wbrinksma

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
#include <memory>

#include "MovementStrategy/MovementStrategyInterface.h"
#include "MovementStrategy/LockdownMovementStrategy.h"
#include "MovementStrategy/RegularMovementStrategy.h"

#include "subject.h"
#include <math.h>
#include <limits>       // std::numeric_limits
#include <iostream>

namespace corsim
{

//
// A. create strategy instance 
// const bool& standOrMove  = false -> stand still / true -> Move
//
// Subject class object is the context where an instance of one of MovementStrategyInterface
// derived classes (concrete strategies algorithms) are binded.
// Assuming developer is responsible to instantiate and bind only once there is no need -
// to delete the previous concrete startegy algorithm instance object. 
// you may need the commented section turned active if concrete strategy algorithm selection - 
// must be changed on run time more then once.
//
void Subject::SelectMovementStrategy(const bool& standOrMove)
{
  /* if(_movementStrategy == nullptr)
   {*/
      if(!standOrMove) //false -> stand still
      {
         _movementStrategy = std::make_shared< MovementStrategyInterface* >(new LockdownMovement());;
        // std::cout << "\n lockdown: " << _movementStrategy->IsStandStill()<<"\n";
      }
      else // true -> Move
      {
         _movementStrategy = std::make_shared< MovementStrategyInterface* >(new RegularMovement());
      }
 /*  }
   else if(_movementStrategy->IsStandStill() && standOrMove) // true -> Move , need to change strategy to RegularMovement Strategy
   {
      delete _movementStrategy;
      _movementStrategy = nullptr;
      _movementStrategy = new RegularMovement();
   }
   else if((!_movementStrategy->IsStandStill()) && (!standOrMove)) // false -> stand still , need to change strategy to LockdownMovement Strategy
   {
      delete _movementStrategy;
      _movementStrategy = nullptr;
      _movementStrategy = new LockdownMovement();
   }
   else
   {
      // no need to change
   }*/
}

   // -----------------------------------------------------------------------------------------------------------------------------------
   // Immunity struct constructor 
   // -----------------------------------------------------------------------------------------------------------------------------------
   // allowes setting 3 parameters
   // infection2immunityDuration = Time period from the moment a subject was infected until he gets immuned.
   // immunityDuration = Time period from the moment a subject was immuned until he lost his immunity (can be infected again).
   // tick_speed = speed of simulation. Remark : tick_speed must be same value as set in simulation class
   //              it is duplicate parameter to avoid changing simulation class (minimizing changes for the rest of the development team). 
   // -----------------------------------------------------------------------------------------------------------------------------------
Subject::Immunity::Immunity( std::shared_ptr< const int > infection2immunityDuration , std::shared_ptr< const int > immunityDuration, std::shared_ptr< const int > tick_speed ):
        _immuned (false),
        _timestampStart (std::numeric_limits<int>::max()),
        _timestampEnd (std::numeric_limits<int>::max()),
        _timestampInfection2immunityStart(std::numeric_limits<int>::max()) ,
        _timestampInfection2immunityEnd(std::numeric_limits<int>::max()) ,
        _infection2immunityDuration(infection2immunityDuration) ,
        _immunityDuration(immunityDuration) ,
        _tick_speed(tick_speed)
{
}

   // -----------------------------------------------------------------------------------------------------------------------------------
   // Immunity struct copy constructor 
   // -----------------------------------------------------------------------------------------------------------------------------------
   // deep copy requiered in casses where non reference object is passed to a function argument
   // or returned from assignment opertator, or returned from function. 
   // -----------------------------------------------------------------------------------------------------------------------------------
Subject::Immunity::Immunity(const Immunity& other):
        _immuned (other._immuned),
        _timestampStart (other._timestampStart),
        _timestampEnd (other._timestampEnd),
        _timestampInfection2immunityStart(other._timestampInfection2immunityStart) ,
        _timestampInfection2immunityEnd(other._timestampInfection2immunityEnd) ,
        _infection2immunityDuration(other._infection2immunityDuration) ,
        _immunityDuration(other._immunityDuration),
        _tick_speed(other._tick_speed)
{
}

   // -----------------------------------------------------------------------------------------------------------------------------------
   // Immunity copy assignment operator  
   // -----------------------------------------------------------------------------------------------------------------------------------
   // deep copy requiered in casses where non reference object is assigned
   // -----------------------------------------------------------------------------------------------------------------------------------
Subject::Immunity& Subject::Immunity::operator=(Immunity const &rhs)
{
     if (this != &rhs) {
        _immuned = rhs._immuned;
        _timestampStart = rhs._timestampStart;
        _timestampEnd = rhs._timestampEnd;
        _timestampInfection2immunityStart = rhs._timestampInfection2immunityStart ,
        _timestampInfection2immunityEnd = rhs._timestampInfection2immunityEnd ,
        _infection2immunityDuration = rhs._infection2immunityDuration ,
        _immunityDuration = rhs._immunityDuration;
        _tick_speed = rhs._tick_speed;
     }
     return *this;
}


   // -----------------------------------------------------------------------------------------------------------------------------------
   // Subject class copy constructor 
   // -----------------------------------------------------------------------------------------------------------------------------------
   // deep copy requiered in casses where non reference object is passed to a function argument
   // or returned from assignment opertator, or returned from function. 
   // -----------------------------------------------------------------------------------------------------------------------------------
Subject::Subject(const Subject& other) :
        _x(other._x),_y(other._y), _dx(other._dx), _dy(other._dy),
        _infected(other._infected),
        _radius(other._radius),
        _immunity(other._immunity),
        _movementStrategy(other._movementStrategy)
{
}


   // -----------------------------------------------------------------------------------------------------------------------------------
   // Subject class constructor
   // -----------------------------------------------------------------------------------------------------------------------------------
   // allowes setting 3 parameters (except the trivial ones)
   // infection2immunityDuration = Time period from the moment a subject was infected until he gets immuned.
   // immunityDuration = Time period from the moment a subject was immuned until he lost his immunity (can be infected again).
   // tick_speed = speed of simulation. Remark : tick_speed must be same value as set in simulation class
   //              it is duplicate parameter to avoid changing simulation class (minimizing changes for the rest of the development team). 
   // -----------------------------------------------------------------------------------------------------------------------------------
Subject::Subject(int x, int y, int radius, bool infected, std::shared_ptr< const int > infection2immunityDuration, std::shared_ptr<const int> immunityDuration , std::shared_ptr<const int> tick_speed):
_immunity( infection2immunityDuration , immunityDuration , tick_speed ),
_movementStrategy(nullptr)
{
    this->_x = x;
    this->_y = y;
    this->_radius = radius;
    this->_infected = infected;
}

   // -----------------------------------------------------------------------------------------------------------------------------------
   // Subject class assignment operator 
   // -----------------------------------------------------------------------------------------------------------------------------------
Subject& Subject::operator=(Subject const &rhs) 
{
     if (this != &rhs) {
       this->_x = rhs._x;
       this->_y = rhs._y;
       this->_radius = rhs._radius;
       this->_infected = rhs._infected;
       this->_immunity = rhs._immunity;
       this->_movementStrategy = rhs._movementStrategy;
     }
     return *this;
}


   // -----------------------------------------------------------------------------------------------------------------------------------
   // B.3. member function DoTick 
   // -----------------------------------------------------------------------------------------------------------------------------------
   // Allow propogate timestamp (tick counter )
   // to each single subject.
   // Track and change (trnsition) immunity state of this Subject Instance (B.3.).
   // -----------------------------------------------------------------------------------------------------------------------------------
void Subject::DoTick(const int& counter) 
{
   int current_time = counter * (*(_immunity._tick_speed.get()));
   //
   // B.3. is this subject instance in immuned state
   //
   if( infected() && !isImmunityOn() ) // subject is infected test when it will get immuned
   {
     // did infection to immunity period expiered? 
     if( current_time >= (_immunity._timestampInfection2immunityEnd) )
     {
       StartImmunityOn(counter);
       this->_infected = false; // immuned are not infected
     }
     else
     {
       // yet immuned subject
     }
   }
   else if ( isImmunityOn() )// immunity is on
   {
     // did wait for immunity period expiered? 
     if( current_time >= (_immunity._timestampEnd) )
     {
       EndImmunityOff();
     }
     else
     {
       // yet immuned subject
     }
   }
   else // not immuned and not infected
   {
   }
}

   // -----------------------------------------------------------------------------------------------------------------------------------
   // 
   // B.3. member function StartImmunityOn
   // start counting immunity duration until expiration
   //
   // -----------------------------------------------------------------------------------------------------------------------------------
void Subject::StartImmunityOn(const int& counter)
{
   _immunity._immuned = true;
}

   // -----------------------------------------------------------------------------------------------------------------------------------
   // 
   // B.3. member function StartInfection2immunityPeriodOn
   // start counting infection to immunity duration
   //
   // -----------------------------------------------------------------------------------------------------------------------------------
void Subject::StartInfection2immunityPeriodOn(const int& counter)
{
   if(!isImmunityOn()) // only NOT-immuned subject can get infected 
   {                   // so they can start Infection2immunityPeriod 
                       // immunity hatching period.
      // counter must be converted to milliseconds (1/100 seconds) count 
      _immunity._timestampInfection2immunityStart = counter * (*(_immunity._tick_speed.get()));
      _immunity._timestampInfection2immunityEnd = _immunity._timestampInfection2immunityStart + (*(_immunity._infection2immunityDuration.get()));

      _immunity._timestampStart = _immunity._timestampInfection2immunityEnd;
      _immunity._timestampEnd = _immunity._timestampStart + (*(_immunity._immunityDuration.get()));
    }
    else
    { 
      // if a person is  already immuned but gets a dose of new infecting virus 
      // pathogens - do we have to start new immunity period?
      // does immunity expiration related to mutation of virus to a new RNA?
    }
//   _immunity._immuned = false;
}

   // -----------------------------------------------------------------------------------------------------------------------------------
   // 
   // B.3. member function EndImmunityOff
   // End counting immunity duration reach expiration tick
   //
   // -----------------------------------------------------------------------------------------------------------------------------------
void Subject::EndImmunityOff()
{
      // before immunity
      _immunity._timestampInfection2immunityStart = std::numeric_limits<int>::max();
      _immunity._timestampInfection2immunityEnd = std::numeric_limits<int>::max();
     
      // when immuned
      _immunity._timestampStart = std::numeric_limits<int>::max();
      _immunity._timestampEnd = std::numeric_limits<int>::max();

      // reset immunity (after expiration period)
      _immunity._immuned = false;
}

   // -----------------------------------------------------------------------------------------------------------------------------------
   // 
   // member function isImmunityOn
   // reflect status of subject immuned / not immuned
   //  
   // -----------------------------------------------------------------------------------------------------------------------------------
bool Subject::isImmunityOn()
{
   return _immunity._immuned;
}

   // -----------------------------------------------------------------------------------------------------------------------------------
   // 
   // A. member function isStandStill
   // reflect status of subject if is locked down / regular movement manner
   //   
   // -----------------------------------------------------------------------------------------------------------------------------------
bool Subject::isStandStill()
{
    if(nullptr == *_movementStrategy )
    {
       return false; // default is regular (not LockDown)
    }
    else if (!(*_movementStrategy)->IsStandStill())
    {
       return false; // _movementStrategy Is Not Stand Still so subject is moving
    }
    else
    {
       return true; // _movementStrategy Is Stand Still so LockDown subject stop moving
    }
}

double Subject::x()
{
    return this->_x;
}

double Subject::y()
{
    return this->_y;
}

void Subject::set_x(double x)
{
    //
    // A. _movementStrategy : polymorphic algorithm Lock down subject movement
    // according to type of derived class binded in this instance context (subject class)
    //
    if(nullptr == _movementStrategy )
    {
      this->_x = x;
    }
    else if (!((*_movementStrategy)->IsStandStill()))
    {
      this->_x = x;
    }
    else
    {
      // ignore movement
    }

}


void Subject::set_y(double y)
{
    //
    // A. _movementStrategy : polymorphic algorithm Lock down subject movement
    // according to type of derived class binded in this instance context (subject class)
    //
    if(nullptr == _movementStrategy )
    {
       this->_y = y;
    }
    else if (!((*_movementStrategy)->IsStandStill()))
    {
       this->_y = y;
    }
    else
    {
      // ignore movement
    }
}

double Subject::dx()
{
    return this->_dx;
}

double Subject::dy()
{
    return this->_dy;
}

void Subject::set_dx(double dx)
{
    this->_dx = dx;
}

void Subject::set_dy(double dy)
{
    this->_dy = dy;
}

int Subject::radius()
{
    return this->_radius;
}

bool Subject::infected()
{
    return this->_infected;
}

void Subject::infect()
{
    //
    // B.3. if immuned cant be infected
    //
    if(!isImmunityOn())
    {
       //StartInfection2immunityPeriodOn();
       this->_infected = true;
    }
}

double Subject::angle()
{
    return atan2(_dy,_dx);
}

double Subject::speed()
{
    return sqrt(_dx * _dx + _dy * _dy);
}

}
