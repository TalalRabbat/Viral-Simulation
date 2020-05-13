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

#pragma once 

namespace corsim
{
    
/**
 * A subject is an entity within the simulation. It is modeled as a
 * circle in 2D and can be infected.
 */
class Subject
{
    public:
        //
        // Subject class constructor 
        // allowes setting 3 parameters
        // infection2immunityDuration = Time period from the moment a subject was infected until he gets immuned.
        // immunityDuration = Time period from the moment a subject was immuned until he lost his immunity (can be infected again).
        // tick_speed = speed of simulation. Remark : tick_speed must be same value as set in simulation class
        //              it is duplicate parameter to avoid changing simulation class (minimizing changes for the rest of the development team). 
        //
        Subject(int x, int y, int radius, bool infected, std::shared_ptr<const int> infection2immunityDuration , std::shared_ptr<const int> immunityDuration , std::shared_ptr<const int> tick_speed);

        //
        // Subject class copy constructor 
        //
        Subject(const Subject& other);

        //
        // Subject class assignment operator 
        //
        Subject& operator=(Subject const &rhs);

        //
        // Fields getters and setters 
        //
        double x();
        double y();
        void set_x(double x);
        void set_y(double y);
        double dx(); //Speed on x axis
        double dy(); //Speed on y axis
        int radius(); //Radius needed for collisions
        void set_dx(double dx);
        void set_dy(double dy);
        bool infected();
        void infect();
        double angle();
        double speed();
 
        //
        // member function DoTick Allow propogate timestamp (tick counter )
        // to each single subject
        //
        void DoTick(const int& counter);

        // B.3 Immunity control member functions

        // 
        // member function StartImmunityOn
        // start counting immunity duration until expiration
        //
        void StartImmunityOn(const int& counter);
        // 
        // member function StartInfection2immunityPeriodOn
        // start counting infection to immunity duration
        //
        void StartInfection2immunityPeriodOn(const int& counter);

        // 
        // member function EndImmunityOff
        // End counting immunity duration reach expiration tick
        //
        void EndImmunityOff();

        // 
        // member function isImmunityOn
        // reflect status of subject immuned / not immuned
        //    
        bool isImmunityOn();

        // A. Lock down strategy control member functions
        // create strategy instance true -> stand still / false Move
        void SelectMovementStrategy(const bool& standOrMove);


        // 
        // member function isStandStill
        // reflect status of subject if is locked down / regular movement manner
        //   
        bool isStandStill();

    private:
        double _x = 0,_y = 0, _dx = 0, _dy = 0;
        bool _infected = false;
        int _radius = 0;

        //
        // B.3 immunity nested class / struct  decleration
        //
        struct Immunity
        {
            // Immunity struct constructor 
            // allowes setting 3 parameters
            // infection2immunityDuration = Time period from the moment a subject was infected until he gets immuned.
            // immunityDuration = Time period from the moment a subject was immuned until he lost his immunity (can be infected again).
            // tick_speed = speed of simulation. Remark : tick_speed must be same value as set in simulation class
            //              it is duplicate parameter to avoid changing simulation class (minimizing changes for the rest of the development team). 
            Immunity( std::shared_ptr<const int> infection2immunityDuration , std::shared_ptr<const int> immunityDuration, std::shared_ptr<const int> tick_speed);
            // Immunity struct copy constructor 
            Immunity(const Immunity& other);
            // Immunity assignment operator  
            Immunity& operator=(Immunity const &rhs);
            // immunity flag (true = immuned)
            bool _immuned = false;
            // immunity start time window 
            int _timestampStart = 0;
            // immunity End when immunity will expire
            int _timestampEnd = 0;
            // moment the subject get infected start counting until Immuned 
            int _timestampInfection2immunityStart = 0;
            // passed illness time counting until Immuned
            int _timestampInfection2immunityEnd = 0;
            // Time period from the moment a subject was infected until he gets immuned.
            std::shared_ptr<const int> _infection2immunityDuration;
            // Time period from the moment a subject was immuned until he lost his immunity (can be infected again).
            std::shared_ptr<const int> _immunityDuration;
            // speed of simulation. Remark : tick_speed must be same value as set in simulation class
            //              it is duplicate parameter to avoid changing simulation class (minimizing changes for the rest of the development team). 
            std::shared_ptr<const int> _tick_speed; // milliseconds per tick
        };

        //
        // B.3 The actual instance of  Immunity object
        //
        Immunity _immunity;
   public:

        //
        // A. Polimorphic class interface (RAII style - auto release using shared_ptr) MovementStrategyInterface
        // selecting in real time one algorithm out of strategy RegularMovementStrategy or LockdownMovementStrategy
        // concrete algorithms implementations. 
        //
        std::shared_ptr< MovementStrategyInterface* > _movementStrategy = nullptr; 
};

};
