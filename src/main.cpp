#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/glu.h>
#ifdef _OPENMP
    #include <omp.h>
#else
    #define omp_get_thread_num() 0
    #define omp_get_num_threads() 1
#endif

#include "globals.h"
#include "motor.h"
#include "motorcontroller.h"

SDL_Window* displayWindow;
SDL_Renderer* displayRenderer;
SDL_RendererInfo displayRendererInfo;

float deltatime;
float abstime;
int zoomed = -1;
#define NUM_MOTORS 110
motorcontroller motors[NUM_MOTORS];


void sortMotors()
{

    motorcontroller tmp = motors[0];

    for(int i = 0; i < NUM_MOTORS; i++)
    {
        for(int j = i + 1; j < NUM_MOTORS; j++)
        {

            if(motors[i].getScore() > motors[j].getScore())
            {
                tmp = motors[j];
                motors[j] = motors[i];
                motors[i] = tmp;
            }
        }
    }
}

void breedMotors() // kills the last 10
{
    float newangle = (float)(rand() % 360) + 50;

    for(int i = 40; i < 80; i++)
    {
        motors[i].reset();
        motors[i].setAngle(newangle);
        float kp = 0;
        float ki = 0;
        float kd = 0;

        for(int j = 0; j < 10; j++)
        {
            int bm = rand() % 40;
            kp += motors[bm].getP();
            ki += motors[bm].getI();
            kd += motors[bm].getD();
        }

        kp /= 10;
        ki /= 10;
        kd /= 10;


        motors[i].setPid(kp, ki, kd);
    }

    for(int i = 80; i < NUM_MOTORS; i++)
    {
        motors[i].reset();
        motors[i].setRandom();
    }

    for(int i = 0; i < NUM_MOTORS; i++)
    {
        motors[i].setAngle(newangle);
    }
}

void Display_InitGL()
{
    SDL_GL_SetSwapInterval(1);
    glEnable(GL_MULTISAMPLE);

    glShadeModel(GL_SMOOTH);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glMatrixMode(GL_PROJECTION_MATRIX);
    glLoadIdentity();
    glOrtho(0 , 200 , 200 , 0 , -100, 100);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int Display_SetViewport(int width, int height)
{
    GLfloat ratio;

    if(height == 0)
    {
        height = 1;
    }

    ratio = (GLfloat)width / (GLfloat)height;
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    return 1;
}

void Display_Render()
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    if(zoomed == -1)
    {
        glBegin(GL_LINES);

        for(int i = 0; i < 10; i++)
        {
            glVertex2f(i * 192, 0);
            glVertex2f(i * 192, 1200);
        }

        for(int i = 0; i < 11; i++)
        {
            glVertex2f(0, i * 109);
            glVertex2f(1920, i * 109);
        }

        glEnd();

        glTranslatef(60, -50, 0);

        for(int i = 0; i < 10; i++)
        {
            glPushMatrix();
            glTranslatef(i * 192, 0, 0);

            for(int j = 0; j < 11; j++)
            {

                glTranslatef(0, 109, 0);
                glPushMatrix();
                glScalef(0.7, 0.7, 0.7);
                motors[i + j * 10].draw();
                glPopMatrix();
            }

            glPopMatrix();
        }
    }
    else
    {
        glScalef(8, 8, 1);
        glTranslatef(60, 80, 0);
        motors[zoomed].draw();
    }
}
int main(int argc, char* argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);
    SDL_CreateWindowAndRenderer(1920, 1200, SDL_WINDOW_OPENGL, &displayWindow, &displayRenderer);
    SDL_GetRendererInfo(displayRenderer, &displayRendererInfo);

    Display_InitGL();
    Display_SetViewport(1920, 1200);

    bool quit = false;

    for(int i = 0; i < NUM_MOTORS; i++)
    {
        motors[i].setAngle(180);
    }

    while(!quit)
    {

        SDL_Event event;

        while(SDL_PollEvent(&event))
        {

            switch(event.type)
            {
                case SDL_QUIT:
                {
                    quit = true;
                    break;
                }

                case SDL_MOUSEBUTTONDOWN:
                {
                    if(event.button.button == SDL_BUTTON_LEFT)
                    {
                        if(zoomed == -1)
                        {
                            int x = event.motion.x / 192;
                            int y = event.motion.y / 109;
                            zoomed = x + y * 10;
                            printf("p:%f,i:%f,d:%f\t\tovershoot:%f steady_error:%f\n", motors[zoomed].getP(), motors[zoomed].getI(), motors[zoomed].getD(), motors[zoomed].getOverShoot(), motors[zoomed].getSteadyState());
                        }
                        else
                        {
                            zoomed = -1;
                        }

                    }

                    break;
                }

            }
        }

        const unsigned char* state = SDL_GetKeyboardState(0);

        if(state[SDL_SCANCODE_ESCAPE])
        {
            quit = true;
        }

        static bool space_held = false;

        if(state[SDL_SCANCODE_SPACE] && !space_held)
        {
            space_held = true;
            sortMotors();
            breedMotors();
        }
        else if(state[SDL_SCANCODE_SPACE] != true)
        {
            space_held = false;
        }


        long now = SDL_GetTicks();
        static long last = now;
        deltatime = ((float)(now - last)) / 1000.0;
        last = now;
        abstime += deltatime;
        static float lastsetTime = abstime + 0.2;

        if(abstime >= lastsetTime)
        {
            lastsetTime = abstime + 0.2;

            if(zoomed == -1)
            {
                sortMotors();
                breedMotors();
            }

        }


        static float kp = 3;
        static float ki = 0.5;
        static float kd = 0.5;

        if(state[SDL_SCANCODE_U])
        {
            kp += deltatime;
        }

        if(state[SDL_SCANCODE_J])
        {
            kp -= deltatime;

            if(kp < 0)
            {
                kp = 0;
            }
        }

        if(state[SDL_SCANCODE_I])
        {
            ki += deltatime;
        }

        if(state[SDL_SCANCODE_K])
        {
            ki -= deltatime;

            if(ki < 0)
            {
                ki = 0;
            }
        }

        if(state[SDL_SCANCODE_O])
        {
            kd += deltatime;
        }

        if(state[SDL_SCANCODE_L])
        {
            kd -= deltatime;

            if(kd < 0)
            {
                kd = 0;
            }
        }


        if(zoomed == -1)
        {
            #pragma omp parallel
            #pragma omp for

            for(int i = 0; i < NUM_MOTORS; i++)
            {
                for(int k = 0; k < 10000; k++)
                {

                    motors[i].update(deltatime);
                }
            }
        }
        else
        {
            float angle = motors[zoomed].getAngle();

            if(state[SDL_SCANCODE_LEFT])
            {
                angle += deltatime * 20;
                motors[zoomed].setAngle(angle, false);
            }

            if(state[SDL_SCANCODE_RIGHT])
            {
                angle -= deltatime * 20;
                motors[zoomed].setAngle(angle, false);
            }

            motors[zoomed].update(deltatime, 0.05);
        }

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, 1920  , 1200, 0 , -100, 100);
        glMatrixMode(GL_MODELVIEW);

        Display_Render();
        SDL_RenderPresent(displayRenderer);
    }

    SDL_Quit();
    return 0;
}
