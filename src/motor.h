#ifndef __MOTOR_H__
#define __MOTOR_H__

// simulated motor

#define MOTOR_BACKWARDS -1.0f
#define MOTOR_STOP 0.0f
#define MOTOR_FORWARDS 1.0f

class motor
{
    int currdir;
public:
    float currentspeed;
    float acceleration;
    float deceleration;
    float maxspeed; // in rpm
    float angle;
    float lastDrawangle;

    motor();
    void draw();
    void update(float direction);
};

#endif //__MOTOR_H__
