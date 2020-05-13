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
#include <emscripten.h>
#include <math.h>

namespace corsim
{

Simulation::Simulation(int width, int height, std::unique_ptr<Canvas> canvas, std::unique_ptr<StatisticsHandler> sh) : 
    _sim_width{width}, _sim_height{height}, _canvas{std::move(canvas)}, _sh{std::move(sh)} {}

void Simulation::add_subject(Subject&& s)
{
    this->_subjects.emplace_back(std::move(s));
}

void Simulation::run()
{
      if(running)
    {
        return;
    }

    running = true;

    while(true)
    {
        this->tick();
        emscripten_sleep(tick_speed);
    }
}

int counter  = 0;

void Simulation::tick()
{
   counter++;

    double dt = tick_speed / 10.0;

    std::vector<Subject*> collision_checker;

    for(Subject& s : _subjects)
    {
        // ----------------------------------------
        // B.3. promote immunity strategy one tick
        s.DoTick(counter);
        // ----------------------------------------

        collision_checker.emplace_back(&s);

        wall_collision(s);
    }

    for(int i = collision_checker.size()-1; i < collision_checker.size(); i--)
    {
        Subject* current_checking = collision_checker.at(i);

        collision_checker.erase(collision_checker.end());

        for(Subject* s : collision_checker)
        {
        // ----------------------------------------
        // B.3. subject_collision testing will consider immunity strategy one tick
            subject_collision(*current_checking, *s, counter);
        // ----------------------------------------
        }
    }

    int numberInfected = 0;

   for(Subject& s : _subjects)
    {
        //
        // A. this is the only point where LockDown / Regular movement startegy is applied!!!
        //
        // set_x , set_y member functions will consider lockdown strategy per instance.
        //
        s.set_x(s.x() + s.dx() * dt);
        s.set_y(s.y() + s.dy() * dt);

        if(s.infected())
        {
            numberInfected++;
        }
    }

    if(counter % 30 == 0)
    {
        _sh.get()->communicate_number_infected(counter/30,numberInfected);
    }
    

    draw_to_canvas();
}

void Simulation::draw_to_canvas()
{
    _canvas.get()->clear();
    _canvas.get()->draw_rectangle(0,0,1,_sim_height,BLACK);
    _canvas.get()->draw_rectangle(0,0,_sim_width,1,BLACK);
    _canvas.get()->draw_rectangle(0,_sim_height-1,_sim_width,1,BLACK);
    _canvas.get()->draw_rectangle(_sim_width-1,0,1,_sim_height,BLACK);

    for(Subject& s : _subjects)
    {
        CanvasColor c = BLUE;

        if(s.infected())
        {
            c = RED;
        }

        if (s.isStandStill())
        {
            CanvasColor c2 = MAGENTA;
            _canvas.get()->draw_ellipse(s.x(), s.y(), s.radius() + 2 , c2);
        }

        if(s.isImmunityOn())
        {
            c = GREEN;
        }

        _canvas.get()->draw_ellipse(s.x(), s.y(), s.radius(), c);

    }
}

void Simulation::wall_collision(Subject& s)
{
    if (s.x() - s.radius() + s.dx() < 0 ||
        s.x() + s.radius() + s.dx() > _sim_width) {
        s.set_dx(s.dx() * -1);
    }
    if (s.y() - s.radius() + s.dy() < 0 ||
        s.y() + s.radius() + s.dy() > _sim_height) {
        s.set_dy(s.dy() * -1);
    }
    if (s.y() + s.radius() > _sim_height) {
        s.set_y(_sim_height - s.radius());
    }
    if (s.y() - s.radius() < 0) {
        s.set_y(s.radius());
    }
    if (s.x() + s.radius() > _sim_width) {
        s.set_x(_sim_width - s.radius());
    }
    if (s.x() - s.radius() < 0) {
        s.set_x(s.radius());
    }
}

double distance(Subject& s1, Subject& s2)
{
    return sqrt(pow(s1.x() - s2.x(),2) + pow(s1.y() - s2.y(),2));
}

void Simulation::subject_collision(Subject& s1, Subject& s2 ,const int& _counterIn)
{
    double dist = distance(s1, s2);

    if(dist < s1.radius() + s2.radius())
    {
        // can immuned subject infect other subject?
        if(s1.infected() || s2.infected())
        {
            //
            // B.3. Don't reinfect if immuned
            //
            if(!s1.isImmunityOn())
            {
              s1.infect();
              //
              // B.3. start counting time until immunity starts for s1 subject instance
              //
              s1.StartInfection2immunityPeriodOn(_counterIn);
            }
            if(!s2.isImmunityOn())
            {
              s2.infect();
              // B.3. start counting time until immunity starts for s2 subject instance
              s2.StartInfection2immunityPeriodOn(_counterIn);
            }
        }        

        double theta1 = s1.angle();
        double theta2 = s2.angle();
        double phi = atan2(s1.x() - s2.x(), s1.y() - s2.y());

        double dx1F = ((2.0*cos(theta2 - phi)) / 2) * cos(phi) + sin(theta1-phi) * cos(phi+M_PI/2.0);
        double dy1F = ((2.0*cos(theta2 - phi)) / 2) * sin(phi) + sin(theta1-phi) * sin(phi+M_PI/2.0);
        
        double dx2F = ((2.0*cos(theta1 - phi)) / 2) * cos(phi) + sin(theta2-phi) * cos(phi+M_PI/2.0);
        double dy2F = ((2.0*cos(theta1 - phi)) / 2) * sin(phi) + sin(theta2-phi) * sin(phi+M_PI/2.0);

        s1.set_dx(dx1F);                
        s1.set_dy(dy1F);                
        s2.set_dx(dx2F);                
        s2.set_dy(dy2F);

        static_collision(s1, s2, false);
    }
}

void Simulation::static_collision(Subject& s1, Subject& s2, bool emergency)
{
    double overlap = s1.radius() + s2.radius() - distance(s1, s2);
    Subject& smallerObject = s1.radius() < s2.radius() ? s1 : s2;
    Subject& biggerObject = s1.radius() > s2.radius() ? s1 : s2;

    if(emergency)
    {
        Subject& temp = smallerObject;
        smallerObject = biggerObject;
        biggerObject = temp;
    }

    double theta = atan2((biggerObject.y() - smallerObject.y()), (biggerObject.x() - smallerObject.x()));
    smallerObject.set_x(smallerObject.x() - overlap * cos(theta));
    smallerObject.set_y(smallerObject.y() - overlap * sin(theta));

    if (distance(s1, s2) < s1.radius() + s2.radius()) {
        if (!emergency)
        {
            static_collision(s1, s2, true);
        }
    }
}

}