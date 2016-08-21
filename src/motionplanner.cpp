#include "motionplanner.h"
#include <math.h>
#include "sword.h"
#include <SDL_opengl.h>
#include <GL/glu.h>
#include <stdio.h>
#include "globals.h"


motionplanner::motionplanner()
{
    motorDistance = 1000;
    spindleDiameter = 10;
}

motionplanner::~motionplanner()
{

}

void motionplanner::getXY(float len1, float len2, float& x, float& y)
{
    if(len1 == motorDistance / 2 && len2 == motorDistance / 2)
    {
        x = motorDistance / 2;
        y = 0;
        return;
    }

    x = (len1 * len1 - len2 * len2 + motorDistance * motorDistance) / (motorDistance * 2);
    y = -sqrt(len1 * len1 - x * x);

}

void motionplanner::getLengths(float x, float y, float& len1, float& len2)
{
    len1 = sqrt(x * x + y * y);
    len2 = sqrt((x - motorDistance) * (x - motorDistance) + y * y);
}

void motionplanner::initMatrix(float x1, float y1,
                               float x2, float y2,
                               float x3, float y3)
{
    calibration[0] = x1;
    calibration[1] = y1;
    calibration[2] = x2;
    calibration[3] = y2;
    calibration[4] = x3;
    calibration[5] = y3;

    float c1 = x2 - x1;
    float s1 = y2 - y1;
    float c2 = x3 - x1;
    float s2 = y3 - y1;
    float len1 = sqrt(c1 * c1 + s1 * s1);
    float len2 = sqrt(c2 * c2 + s2 * s2);

    if(len1 < len2)
    {
        calib_len[0] = len2;
        calib_len[1] = len1;
        c2 /= len2;
        s2 /= len2;
        mat[0] = c2;
        mat[1] = -s2;
        mat[2] = -x1;
        mat[3] = s2;
        mat[4] = c2;
        mat[5] = -y1;
    }
    else
    {
        calib_len[0] = len1;
        calib_len[1] = len2;
        c1 /= len1;
        s1 /= len1;
        mat[0] = c1;
        mat[1] = -s1;
        mat[2] = -x1;
        mat[3] = s1;
        mat[4] = c1;
        mat[5] = -y1;
    }

}

void motionplanner::vecmul(float inx, float iny, float& outx, float& outy)
{
    outx    = inx * mat[0] + iny * mat[1] + mat[2];
    outy    = inx * mat[3] + iny * mat[4] + mat[5];
}

// fit longest edge towards the longest edge and center the box to the calibration
void motionplanner::fitBox(float minx, float miny, float maxx, float maxy)
{
    float xlen = abs(maxx - minx);
    float ylen = abs(maxy - miny);
    float aspectcalib = calib_len[0] / calib_len[1];
    float scalefactor = 1;

    float aspectbox = xlen / ylen;

    if(aspectbox > aspectcalib)
    {
        if(calib_len[0] / xlen < calib_len[1] / ylen)
        {
            scalefactor = calib_len[0] / xlen;
        }
        else
        {
            scalefactor = calib_len[1] / ylen;
        }

        float tmp = minx;
        minx = maxx;
        maxx = tmp;
    }
    else
    {

        if(calib_len[1] / xlen < calib_len[0] / ylen)
        {
            scalefactor = calib_len[1] / xlen;
        }
        else
        {
            scalefactor = calib_len[0] / ylen;
        }

        float tmp = mat[0];
        mat[0] = -mat[3];
        mat[3] = tmp;

        tmp = mat[1];
        mat[1] = -mat[4];
        mat[4] = tmp;
    }

    float tmpx, tmpy;
    vecmul((minx + maxx) / 2 * scalefactor, (miny + maxy) / 2 * scalefactor, tmpx, tmpy);
    mat[0] *= scalefactor;
    mat[1] *= scalefactor;
    mat[3] *= scalefactor;
    mat[4] *= scalefactor;

    mat[2] -= tmpx;
    mat[5] -= tmpy;
    mat[2] += calibration[0];
    mat[5] += calibration[1];
    mat[2] += (calibration[2] - calibration[0]) / 2;
    mat[5] += (calibration[3] - calibration[1]) / 2;
    mat[2] += (calibration[4] - calibration[0]) / 2;
    mat[5] += (calibration[5] - calibration[1]) / 2;

}

void motionplanner::setSpindleDiameter(float diam)
{
    spindleDiameter = diam;
}

void motionplanner::debugDraw() // for now to debug the matrix init and vecmul
{
    static float angle = 0;
    angle += deltatime;
    float c = cos(angle);
    float s = sin(angle);
    float h = (float)view_height;
    float w = (float)view_width;

    float viewportpoints[8] =
    {
        //(-200 * c + 200 * s), (-200 * c + -200 * s),
        //(h * c - (-200) * s), ((-200) * c + h * s),
        //((-200) * c - w * s), (w * c + (-200) * s),
        //(h * c - w * s), (w * c + h * s)
        156.7, -150.2,
        598.5, -74.2,
        286.2, -901.6,
        727.9, -825.5
    };

    initMatrix(viewportpoints[0], viewportpoints[1],
               viewportpoints[2], viewportpoints[3],
               viewportpoints[4], viewportpoints[5]);

    float minx = __FLT_MAX__;
    float maxx = __FLT_MIN__;
    float miny = __FLT_MAX__;
    float maxy = __FLT_MIN__;


    for(int i = 0; i < 286 * 2; i += 2)
    {
        if(sword[i] < minx)
        {
            minx = sword[i];
        }

        if(sword[i] > maxx)
        {
            maxx = sword[i];
        }

        if(sword[i + 1] < miny)
        {
            miny = sword[i + 1];
        }

        if(sword[i + 1] > maxy)
        {
            maxy = sword[i + 1];
        }
    }

    fitBox(minx, miny, maxx, maxy);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    glOrtho(-500, 1500,  100 - 2000 * ((float)view_height / (float)view_width), 100, 1, -1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glPushMatrix();
    motors[0].draw();
    glTranslatef(motorDistance, 0, 0);
    motors[1].draw();
    glPopMatrix();

    float im_x = (float)mouse_x / view_width * 2 - 1;
    float im_y = (float)mouse_y / view_height * -2 + 1;
    // taken from the inverted matrix
    float m_x = 1000 * im_x + 500;
    float m_y = 625 * im_y - 525;

    glBegin(GL_LINES);
    {
        glColor3f(1, 0, 1);
        glVertex2f(m_x, m_y);
        glVertex2f(0, 0);
        glVertex2f(m_x, m_y);
        glVertex2f(motorDistance, 0);

        glColor3f(1, 1, 1);

        glVertex2f(0, 0);
        glVertex2f(-100, -100);
        glVertex2f(0, 0);
        glVertex2f(-100, -100);
        glVertex2f(0, 0);
        glVertex2f(100, -100);
        glVertex2f(0, 0);
        glVertex2f(100, 100);
        glVertex2f(0, 0);
        glVertex2f(-100, 100);
        glColor3f(1, 0, 0);
        glVertex2f(viewportpoints[0], viewportpoints[1]);
        glVertex2f(viewportpoints[2], viewportpoints[3]);
        glColor3f(0, 1, 0);
        glVertex2f(viewportpoints[0], viewportpoints[1]);
        glVertex2f(viewportpoints[4], viewportpoints[5]);
        glColor3f(1, 0, 0);
        glVertex2f(viewportpoints[4], viewportpoints[5]);
        glVertex2f(viewportpoints[6], viewportpoints[7]);
        glColor3f(0, 1, 0);
        glVertex2f(viewportpoints[2], viewportpoints[3]);
        glVertex2f(viewportpoints[6], viewportpoints[7]);
    }
    glEnd();
    //glPopMatrix();
    //glScalef(30, 30, 1);
    glBegin(GL_LINES);

    {
        float xa;
        float ya;
        float xb;
        float yb;
        glColor3f(0, 0, 1);
        vecmul(minx, miny, xa, ya);
        vecmul(minx, maxy, xb, yb);
        glVertex2f(xa, ya);
        glVertex2f(xb, yb);
        vecmul(minx, maxy, xa, ya);
        vecmul(maxx, maxy, xb, yb);
        glVertex2f(xa, ya);
        glVertex2f(xb, yb);
        vecmul(maxx, maxy, xa, ya);
        vecmul(maxx, miny, xb, yb);
        glVertex2f(xa, ya);
        glVertex2f(xb, yb);
        vecmul(maxx, miny, xa, ya);
        vecmul(minx, miny, xb, yb);
        glVertex2f(xa, ya);
        glVertex2f(xb, yb);
    }
    glColor3f(1, 1, 1);

    for(int i = 0; i < 286 * 2; i += 2)
    {
        float x = sword[i];
        float y = sword[i + 1];
        vecmul(x, y, x, y);
        glVertex2f(x, y);
    }

    glEnd();
    glPointSize(10);
    glBegin(GL_POINTS);
    {
        float x, y;
        float l1 = sqrt(m_x * m_x + m_y * m_y);
        float l2 = sqrt((m_x - motorDistance) * (m_x - motorDistance) + m_y * m_y);
        getLengths(m_x, m_y, l1, l2);
        getXY(l1, l2, x, y);
        printf("%f %f x:%f y:%f\n", l1, l2, x, y);
        glVertex2f(x, y);

        motors[0].setAngle(l1 / spindleDiameter / 2 * (180 / M_PI));
        motors[1].setAngle(-l2 / spindleDiameter / 2 * (180 / M_PI));
        l1 = motors[0].m.getAngle() * spindleDiameter * 2 / (180 / M_PI);
        l2 = motors[1].m.getAngle() * spindleDiameter * 2 / (180 / M_PI);

        getXY(l1, l2, x, y);
        printf("%f %f x:%f y:%f\n", l1, l2, x, y);
        glColor3f(1, 1, 0);
        glVertex2f(x, y);
    }
    glEnd();
    motors[0].update(deltatime);
    motors[1].update(deltatime);

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glLoadIdentity();
}
