#ifndef __MOTORCONTROLLER_H__
#define __MOTORCONTROLLER_H__

#include "motor.h"
#define GRAPH_LENGTH 100
#define GRAPH_HEIGHT 100
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

    float startAngle;
    float overshoot;
    float minsteadystate;
    float maxsteadystate;
    void MeasureOvershoot();

public:
    motor m;
    float targetAngle;
    float currentAngle;
    float anglegraph[GRAPH_LENGTH];
    float targetgraph[GRAPH_LENGTH];
    int graphend;

    void setAngle(float angle);
    float getAngle();
    void update(float seconds = -1.0f);
    void draw();
    motorcontroller();
    void setPid(float p, float i, float d);
    float getP();
    float getI();
    float getD();
    float getOverShoot();

    bool operator > (const motorcontroller& other) const; // for sorting
};

#endif //__MOTORCONTROLLER_H__
