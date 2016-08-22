#ifndef __MOTORCONTROLLER_H__
#define __MOTORCONTROLLER_H__

#include "motor.h"
#define GRAPH_LENGTH 500
#define GRAPH_HEIGHT 100
#define STEADY_TIME 1.0

class motorcontroller
{
    float kp;
    float ki;
    float kd;
    float lastGraphTime;

    float errSum;
    float lastErr;
    float internal_clock;

    // performance metrics

    float changeTime;
    float startAngle;
    float overshoot;
    bool overshootValid;
    float minsteadystate;
    float maxsteadystate;
    bool steadystateValid;
    void MeasureOvershoot();
    void MeasureSteadyState();

public:
    motor m;
    float targetAngle;
    float currentAngle;
    float anglegraph[GRAPH_LENGTH];
    float targetgraph[GRAPH_LENGTH];
    int graphend;

    void setAngle(float angle, bool resetInternalClock = true);
    float getAngle() const;
    void update(float seconds = -1.0, float graphspeed = 0.75);
    void draw();
    motorcontroller();
    void setPid(float p, float i, float d);
    float getP() const;
    float getI() const;
    float getD() const;
    float getOverShoot() const;
    float getSteadyState() const;


    float getScore() const;
    void reset();
    void setRandom();

};

#endif //__MOTORCONTROLLER_H__
