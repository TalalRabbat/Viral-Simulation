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

#include "MovementStrategy/MovementStrategyInterface.h"
#include "MovementStrategy/LockdownMovementStrategy.h"
#include "MovementStrategy/RegularMovementStrategy.h"


#include "simulation.h"
#include <iostream>
#include <random>
#include <math.h>
#include "html_canvas.h"
#include "ChartJS_handler.h"

//Constants to control the simulation
const int SUBJECT_COUNT = 200;
const int SIM_WIDTH = 800;
const int SIM_HEIGHT = 500;
const int SUBJECT_RADIUS = 2;

int main() {


    // ////////////////////////////////////////////////////////////
    // B.3. preper immunity periods parameters
    //
    std::shared_ptr<const int> infection2immunityDuration;
    std::shared_ptr<const int> immunityDurationShPtr;
    std::shared_ptr<const int> tick_speedShPtr;
    //const int tick_speed = 1000/30;
    //const int immunityDuration = 4 * tick_speed;

    tick_speedShPtr = std::make_shared<const int>(1000/30);
    infection2immunityDuration= std::make_shared<const int>(90 * (*(tick_speedShPtr.get())));
    immunityDurationShPtr = std::make_shared<const int>(210 * (*(tick_speedShPtr.get())));
    // END: B.3. preper immunity periods parameters
    // ////////////////////////////////////////////////////////////

    corsim::Simulation s(SIM_WIDTH,SIM_HEIGHT,std::make_unique<corsim::HTMLCanvas>(30,150,SIM_WIDTH,SIM_HEIGHT),
        std::make_unique<corsim::ChartJSHandler>());

    //Code to randomly generate certain numbers, which is done by using certain distributions
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist_w(1.0, SIM_WIDTH);
    std::uniform_real_distribution<double> dist_h(1.0, SIM_HEIGHT);
    std::uniform_real_distribution<double> dist_dx(-1.0, 1.0);
    std::uniform_real_distribution<double> dist_dy(-1.0, 1.0);

    /*std::uniform_real_distribution<double> dist_random_select(0.0, 1.0);
    int RandomSelectionIndex[SUBJECT_COUNT];
    for (int i = 0; i<SUBJECT_COUNT; ++i)
    {
       RandomSelectionIndex[ i ] = -1;
    }*/

    int limitLockDown = (int)std::floor(SUBJECT_COUNT * 0.75 ); // 75% are locked
    for (int i = 0; i<SUBJECT_COUNT; ++i)
    {
        // -------------------------------------------------------------------------
        // linear hash collision conflict resolution
        /*int trajectionIndex = (int)(dist_random_select(mt) * (double)SUBJECT_COUNT);
        bool isHit = false;
        do
        {
             if( RandomSelectionIndex[trajectionIndex] != -1 )
             {
                isHit = false;
                if(trajectionIndex < (SUBJECT_COUNT - 2))
                {
                   trajectionIndex++;
                }
                else
                {
                   trajectionIndex  = 0; // wrap arround
                }
             }
             else // hit an enpty cell
             {
                RandomSelectionIndex[trajectionIndex] = i;
                isHit = true;
             }
        }
        while( !isHit );*/
        

     
        // -------------------------------------------------------------------------

        double x = dist_w(mt); //Randomly generate x position
        double y = dist_h(mt); //Randomly generate y position
        
        corsim::Subject su(x,y,SUBJECT_RADIUS,false,infection2immunityDuration ,immunityDurationShPtr,tick_speedShPtr);

        su.set_dx(dist_dx(mt));
        su.set_dy(dist_dy(mt));

        if(i == SUBJECT_COUNT-1)
        {
            su.infect();
            su.StartInfection2immunityPeriodOn(0); // first subject infected on simukation time 0
        }

        if( i < limitLockDown )
        {
           // A. instantiate strategy
           su._movementStrategy = std::make_shared< MovementStrategyInterface* >(new LockdownMovement());
           //std::cout << "\n ("<<limitLockDown<<"/"<<SUBJECT_COUNT<<") lockdown["<< i <<"]: " << su.isStandStill()<<"\n";
        }
        else
        {
           // A. instantiate strategy
           su._movementStrategy = std::make_shared< MovementStrategyInterface* >(new RegularMovement());
           //std::cout << "\n ("<<limitLockDown<<"/"<<SUBJECT_COUNT<<") lockdown["<< i <<"]: " << su.isStandStill()<<"\n";
        }

        s.add_subject(std::move(su));
    }  

    s.run();
}
