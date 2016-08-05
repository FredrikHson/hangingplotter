#ifndef __MOTORCONTROLLER_H__
#define __MOTORCONTROLLER_H__

#include "motor.h"

class motorcontroller
{
    motor m;
    float p;
    float i;
    float d;
    float lastGraphTime;
public:
    float targetAngle;
    float currentAngle;
    float anglegraph[200];
    int graphend;

    void setAngle(float angle);
    void update();
    void draw();
    motorcontroller();
    void setPid(float p, float i, float d);
};

#endif //__MOTORCONTROLLER_H__
