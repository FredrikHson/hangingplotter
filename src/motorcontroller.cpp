#include <SDL_opengl.h>
#include "globals.h"
#include <math.h>
#include "motorcontroller.h"


void motorcontroller::draw()
{
    glPushMatrix();
    m.draw();

    glTranslatef(60, -60, 0);


    glBegin(GL_QUADS);
    glColor3f(1, 1, 1);
    glVertex2f(0, 0);
    glVertex2f(200, 0);
    glVertex2f(200, 150);
    glVertex2f(0, 150);
    glEnd();

    float gmin = anglegraph[0];
    float gmax = anglegraph[0];

    for(int j = 0; j < 200; j++)
    {
        if(gmin > anglegraph[j])
        {
            gmin = anglegraph[j];
        }

        if(gmax < anglegraph[j])
        {
            gmax = anglegraph[j];
        }
    }

    if(targetAngle < gmin)
    {
        gmin = targetAngle;
    }

    if(targetAngle > gmax)
    {
        gmax = targetAngle;
    }

    gmax += 10;
    gmin -= 10;

    glBegin(GL_LINE_STRIP);

    for(int j = 0; j < 200; j++)
    {
        glColor3f(0, 0, 1);
        glVertex2f(j, ((anglegraph[j] - gmin) / (gmax - gmin)) * 150);
    }

    glEnd();
    glBegin(GL_LINES);
    glColor3f(1, 0, 0);
    glVertex2f(0, ((targetAngle - gmin) / (gmax - gmin)) * 150);
    glVertex2f(200, ((targetAngle - gmin) / (gmax - gmin)) * 150);
    glEnd();
    glColor3f(1, 1, 1);
    glPopMatrix();
}

void motorcontroller::update()
{
    currentAngle = m.getAngle();

    float error = targetAngle - currentAngle;
    errSum += error * deltatime;
    float output = kp * error + ki * errSum + kd * ((error - lastErr) / deltatime);
    lastErr=error;

    if(output > 0)
    {
        m.update(1);
    }
    else if(output < 0)
    {
        m.update(-1);
    }
    else
    {
        m.update(0);
    }

    if((lastGraphTime + 0.1) < abstime)
    {
        lastGraphTime = abstime;

        graphend = (graphend + 1) % 200;
        anglegraph[graphend] = currentAngle;
    }
}

void motorcontroller::setAngle(float angle)
{
    targetAngle = angle;
}

motorcontroller::motorcontroller()
{
    this->kp = 0.6;
    this->ki = 0.5;
    this->kd = 0.5;

    this->graphend = 0;

    this->errSum        = 0;
    this->lastGraphTime = 0;
}

void motorcontroller::setPid(float p, float i, float d)
{
    this->kp = p;
    this->ki = i;
    this->kd = d;
}
