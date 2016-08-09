#ifndef __MOTOR_H__
#define __MOTOR_H__

// simulated motor

#define MOTOR_BACKWARDS -1.0
#define MOTOR_STOP 0.0
#define MOTOR_FORWARDS 1.0

class motor
{
    int currdir;
    float angle;
public:
    float currentspeed;
    float acceleration;
    float deceleration;
    float maxspeed; // in rpm
    float lastDrawangle;

    motor();
    void draw();
    void update(float direction);
    float getAngle();
    void reset();
};

#endif //__MOTOR_H__
