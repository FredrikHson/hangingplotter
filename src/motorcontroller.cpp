#include <SDL_opengl.h>
#include "globals.h"
#include <math.h>
#include "motorcontroller.h"
#include <stdio.h>
#include <stdlib.h>


void motorcontroller::draw()
{
    glPushMatrix();

    glBegin(GL_LINES);
    glColor3f(1, 0, 0);
    glVertex2f(0, 0);
    float radangle = targetAngle / (180.0f / M_PI);
    glVertex2f(sin(radangle) * 75, cos(radangle) * 75);
    glEnd();
    m.draw();
    glTranslatef(80, -60, 0);


    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBegin(GL_QUADS);
    glColor3f(1, 1, 1);
    glVertex2f(0, 0);
    glVertex2f(GRAPH_LENGTH, 0);
    glVertex2f(GRAPH_LENGTH, GRAPH_HEIGHT);
    glVertex2f(0, GRAPH_HEIGHT);
    glEnd();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    float gmin = anglegraph[0];
    float gmax = anglegraph[0];

    for(int j = 0; j < GRAPH_LENGTH; j++)
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

    for(int j = 0; j < GRAPH_LENGTH; j++)
    {
        if(gmin > targetgraph[j])
        {
            gmin = targetgraph[j];
        }

        if(gmax < targetgraph[j])
        {
            gmax = targetgraph[j];
        }
    }


    gmax += 0.1;
    gmin -= 0.1;

    glBegin(GL_LINE_STRIP);

    for(int j = 0; j < GRAPH_LENGTH; j++)
    {
        glColor3f(0, 0, 1);
        glVertex2f(j, ((anglegraph[j] - gmin) / (gmax - gmin)) * GRAPH_HEIGHT);
    }

    glEnd();
    glBegin(GL_LINE_STRIP);

    for(int j = 0; j < GRAPH_LENGTH; j++)
    {
        glColor3f(1, 0, 0);
        glVertex2f(j, ((targetgraph[j] - gmin) / (gmax - gmin)) * GRAPH_HEIGHT);
    }

    glEnd();
    glBegin(GL_LINES);
    glColor3f(0, 1, 0);
    glVertex2f(graphend + 1, 0);
    glVertex2f(graphend + 1, GRAPH_HEIGHT);
    glEnd();
    glColor3f(1, 1, 1);
    glPopMatrix();
}

void motorcontroller::update(float seconds)
{
    if(seconds < 0)
    {
        seconds = deltatime;
        internal_clock = abstime;
    }
    else
    {
        internal_clock += seconds;
    }

    currentAngle = m.getAngle();

    float error = targetAngle - currentAngle;
    errSum += error * seconds;
    float output = kp * error + ki * errSum + kd * ((error - lastErr) / seconds);
    lastErr = error;
    //printf("errsum=%f error:%f\n", errSum, error);
#define DEADZONE 0.001

    if(output > DEADZONE)
    {
        m.update(output);
    }
    else if(output < -DEADZONE)
    {
        m.update(output);
    }
    else
    {
        m.update(0);
    }

    if((lastGraphTime + 0.25) < internal_clock)
    {
        lastGraphTime = internal_clock;

        graphend = (graphend + 1) % GRAPH_LENGTH;
        anglegraph[graphend] = currentAngle;
        targetgraph[graphend] = targetAngle;
    }

    MeasureOvershoot();
}

void motorcontroller::setAngle(float angle)
{
    startAngle = m.getAngle();
    overshoot = 0;
    targetAngle = angle;
}

float motorcontroller::getAngle()
{
    return targetAngle;
}
motorcontroller::motorcontroller()
{
    this->kp = (float)(rand() % 200) / 10.0f;
    this->ki = (float)(rand() % 200) / 10.0f;
    this->kd = (float)(rand() % 200) / 10.0f;
    //this->kp = 5.6;
    //this->ki = 0.3;
    //this->kd = 0.2;

    this->graphend = 0;

    this->errSum        = 0;
    this->lastGraphTime = 0;

    for(int i = 0; i < GRAPH_LENGTH; i++)
    {
        anglegraph[i] = 0;
        targetgraph[i] = 0;
    }
}

void motorcontroller::setPid(float p, float i, float d)
{
    if(p != kp || ki != i || kd != d)
    {
        printf("p:%f,i:%f,d:%f\n", p, i, d);
    }

    this->kp = p;
    this->ki = i;
    this->kd = d;
}

float motorcontroller::getP()
{
    return kp;
}

float motorcontroller::getI()
{
    return ki;
}

float motorcontroller::getD()
{
    return kd;
}

float motorcontroller::getOverShoot()
{
    return overshoot;

}

void motorcontroller::MeasureOvershoot()
{
    if(startAngle < targetAngle)
    {
        if(m.getAngle() > targetAngle)
        {
            float o = m.getAngle() - targetAngle;

            if(o > overshoot)
            {
                overshoot = o;
            }
        }
    }
    else
    {
        if(m.getAngle() < targetAngle)
        {
            float o = targetAngle - m.getAngle();

            if(o > overshoot)
            {
                overshoot = o;
            }
        }

    }
}

bool motorcontroller::operator > (const motorcontroller& other) const
{
    return (overshoot > other.overshoot);
}
