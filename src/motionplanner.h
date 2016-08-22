#ifndef __MOTIONPLANNER_H__
#define __MOTIONPLANNER_H__

/*
 *  types
 *  0 line
 *  1 lift pen
 *  2 drop pen
 *  3 circle cw
 *  4 circle ccw
 * */

//TODO: rewrite everything to use fixedpoint math

#include "motorcontroller.h"

struct command
{
    char type;
    unsigned short speed;
    float targetx;
    float targety;
};

class motionplanner
{

    float motorDistance;
    float mat[6]; // 3x2 matrix scale+rot+translation
    // init by setting 3 corners of the paper
    // rotation comes from the longest edge
    // also preprocess the entire motion path to get the
    // aspect ratio to determine the largest image that can be drawn
    float calibration[6];
    float calib_len[2];
    float spindleDiameter;

public:
    motorcontroller motors[2];
    motionplanner();
    ~motionplanner();
    void getXY(float len1, float len2, float& x, float& y) const;
    void getLengths(float x, float y, float& len1, float& len2) const;
    void setDistance(float distance);
    void setSpindleDiameter(float diam);
    void initMatrix(float x1, float y1,
                    float x2, float y2,
                    float x3, float y3);
    void fitBox(float minx, float miny,
                float maxx, float maxy);
    void vecmul(float inx, float iny,
                float& outx, float& outy) const;
    void debugDraw();
};


#endif //__MOTIONPLANNER_H__
