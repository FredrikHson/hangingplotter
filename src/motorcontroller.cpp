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

void motorcontroller::update(float seconds, float graphspeed)
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

    if((lastGraphTime + graphspeed) < internal_clock)
    {
        lastGraphTime = internal_clock;

        graphend = (graphend + 1) % GRAPH_LENGTH;
        anglegraph[graphend] = currentAngle;
        targetgraph[graphend] = targetAngle;
    }

    MeasureOvershoot();
    MeasureSteadyState();
}

void motorcontroller::setAngle(float angle)
{
    startAngle = m.getAngle();
    overshoot = 0;
    overshootValid = false;
    targetAngle = angle;
    changeTime = internal_clock;
}

float motorcontroller::getAngle()
{
    return targetAngle;
}
motorcontroller::motorcontroller()
{

    setRandom();
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
    if(overshootValid)
    {
        return overshoot;
    }
    else
    {
        return __FLT_MAX__;
    }

}
float motorcontroller::getSteadyState()
{
    if(changeTime + STEADY_TIME < internal_clock && steadystateValid)
    {
        return (targetAngle - minsteadystate) - (targetAngle - maxsteadystate);
    }
    else
    {
        return __FLT_MAX__;
    }
}

void motorcontroller::MeasureOvershoot()
{
    if(startAngle < targetAngle)
    {
        if(m.getAngle() > targetAngle)
        {
            float o = m.getAngle() - targetAngle;

            if(o > overshoot || !overshootValid)
            {
                overshoot = o;
                overshootValid = true;
            }
        }
    }
    else
    {
        if(m.getAngle() < targetAngle)
        {
            float o = targetAngle - m.getAngle();

            if(o > overshoot || !overshootValid)
            {
                overshoot = o;
                overshootValid = true;
            }
        }

    }
}

void motorcontroller::MeasureSteadyState()
{
    if(changeTime + STEADY_TIME < internal_clock && overshootValid)
    {
        steadystateValid = true;
        float m_angle = m.getAngle();

        if(m_angle > maxsteadystate)
        {
            maxsteadystate = m_angle;
        }

        if(m_angle < minsteadystate)
        {
            minsteadystate = m_angle;
        }
    }
    else
    {
        steadystateValid = false;
        minsteadystate = __FLT_MAX__;
        maxsteadystate = __FLT_MIN__;
    }
}

void motorcontroller::reset()
{
    errSum = 0;
    lastErr = 0;
    m.reset();
}


float motorcontroller::getScore()
{
    return getSteadyState() * 10 + getOverShoot();
}

void motorcontroller::setRandom()
{
    kp = (float)(rand() % 5000) / 100;
    ki = (float)(rand() % 5000) / 100;
    kd = (float)(rand() % 5000) / 100;
}
