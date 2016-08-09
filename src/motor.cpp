#include "motor.h"
#include <SDL_opengl.h>
#include <math.h>
#include "globals.h"

motor::motor()
{
    this->angle         = 0;
    this->currentspeed  = 0;
    this->acceleration  = 50;
    this->deceleration  = this->acceleration * 0.5;
    this->maxspeed      = 60;
    this->lastDrawangle = 0;
    this->currdir       = 0;
}


void motor::draw()
{
    glBegin(GL_LINES);
    glVertex2f(0, 0);
    glVertex2f(sin(angle) * 65, cos(angle) * 65);

    for(float i = 0; i < 1; i += 0.01)
    {
        float a2 = angle * i + lastDrawangle * (1 - i);
        glColor3f(i, i, i);
        glVertex2f(0, 0);
        glVertex2f(sin(a2) * 65, cos(a2) * 65);

    }

    glEnd();
    glBegin(GL_LINE_LOOP);

    if(currdir == 0)
    {
        glColor3f(1, 0, 0);
    }
    else if(currdir == 1)
    {
        glColor3f(0, 0, 1);
    }
    else
    {
        glColor3f(0, 1, 0);
    }

    for(float i = 0; i < M_PI * 2; i += 0.050)
    {
        glVertex2f(sin(i) * 50, cos(i) * 50);
    }

    glEnd();
    glColor3f(1, 1, 1);
    lastDrawangle = angle;
}
void motor::update(float direction)
{
    if(direction > 1)
    {
        direction = 1;
    }

    if(direction < -1)
    {
        direction = -1;
    }

    if(direction > 0.0005)
    {
        currdir = 1;
        currentspeed += acceleration * deltatime * direction;
    }
    else if(direction < -0.0005)
    {
        currdir = -1;
        currentspeed += acceleration * deltatime * direction;
    }
    else
    {
        currdir = 0;
        direction = 0;

        if(currentspeed > 0)
        {
            currentspeed -= deceleration * deltatime;

            if(currentspeed < 0)
            {
                currentspeed = 0;
            }
        }
        else
        {
            currentspeed += deceleration * deltatime;

            if(currentspeed > 0)
            {
                currentspeed = 0;
            }
        }
    }


    if(currentspeed > maxspeed)
    {
        currentspeed = maxspeed;
    }
    else if(currentspeed < -maxspeed)
    {
        currentspeed = -maxspeed;
    }

    angle += currentspeed * deltatime;

}

float motor::getAngle()
{
    return angle * (180.0 / M_PI);
}
void motor::reset()
{
    currentspeed = 0;
    angle        = 0;

}
