#include <SDL_opengl.h>
#include "globals.h"
#include <math.h>
#include "motorcontroller.h"


void motorcontroller::draw()
{
    glPushMatrix();

    glBegin(GL_LINES);
    glColor3f(1,0,0);
    glVertex2f(0, 0);
    glVertex2f(sin(targetAngle) * 65, cos(targetAngle) * 65);
    glEnd();
    m.draw();
    glTranslatef(70, -60, 0);


    glBegin(GL_QUADS);
    glColor3f(1, 1, 1);
    glVertex2f(0, 0);
    glVertex2f(500, 0);
    glVertex2f(500, 400);
    glVertex2f(0, 400);
    glEnd();

    float gmin = anglegraph[0];
    float gmax = anglegraph[0];

    for(int j = 0; j < 500; j++)
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

    gmax += 0.1;
    gmin -= 0.1;

    glBegin(GL_LINE_STRIP);

    for(int j = 0; j < 500; j++)
    {
        glColor3f(0, 0, 1);
        glVertex2f(j, ((anglegraph[j] - gmin) / (gmax - gmin)) * 400);
    }

    glEnd();
    glBegin(GL_LINES);
    glColor3f(1, 0, 0);
    glVertex2f(0, ((targetAngle - gmin) / (gmax - gmin)) * 400);
    glVertex2f(500, ((targetAngle - gmin) / (gmax - gmin)) * 400);
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

    if(output > 0.0002)
    {
        m.update(1);
    }
    else if(output < -0.0002)
    {
        m.update(-1);
    }
    else
    {
        m.update(0);
    }

    if((lastGraphTime + 0.05) < abstime)
    {
        lastGraphTime = abstime;

        graphend = (graphend + 1) % 500;
        anglegraph[graphend] = currentAngle;
    }
}

void motorcontroller::setAngle(float angle)
{
    targetAngle = angle;
}

motorcontroller::motorcontroller()
{
    this->kp = 5.6;
    this->ki = 0.3;
    this->kd = 0.2;

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
