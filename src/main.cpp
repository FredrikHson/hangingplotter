#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/glu.h>

#include "globals.h"
#include "motor.h"
#include "motorcontroller.h"

SDL_Window* displayWindow;
SDL_Renderer* displayRenderer;
SDL_RendererInfo displayRendererInfo;

float deltatime;
float abstime;
motorcontroller motors[10][11];
void Display_InitGL()
{
    SDL_GL_SetSwapInterval(1);
    glShadeModel(GL_SMOOTH);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glMatrixMode(GL_PROJECTION_MATRIX);
    glLoadIdentity();
    //gluPerspective(45.0f, ratio, 0.1f, 100.0f);
    glOrtho(0 , 200 , 200 , 0 , -100, 100);
    //glScalef(1,ratio,1);
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
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
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
            motors[i][j].draw();
            glPopMatrix();
        }

        glPopMatrix();
    }
}
int main(int argc, char* argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(1920, 1200, SDL_WINDOW_OPENGL, &displayWindow, &displayRenderer);
    SDL_GetRendererInfo(displayRenderer, &displayRendererInfo);

    if((displayRendererInfo.flags & SDL_RENDERER_ACCELERATED) == 0 ||
            (displayRendererInfo.flags & SDL_RENDERER_TARGETTEXTURE) == 0)
    {
    }

    Display_InitGL();
    Display_SetViewport(1920, 1200);

    bool quit = false;

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
                        int x = event.motion.x / 192;
                        int y = event.motion.y / 109;
                        //printf("mouseclicked:%i,%i\n", event.motion.x / 192, event.motion.y / 109);
                        printf("p:%f,i:%f,d:%f\n", motors[x][y].getP(), motors[x][y].getI(), motors[x][y].getD());
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


        long now = SDL_GetTicks();
        static long last = now;
        deltatime = ((float)(now - last)) / 1000.0f;;
        last = now;
        abstime += deltatime;
        static float lastsetTime = abstime;
        static float timetonextset = 0;

        if(abstime >= lastsetTime + timetonextset)
        {
            lastsetTime = abstime;
            timetonextset = (float)(rand() % 2000) / 100;
            float newangle = (float)(rand() % 360);

            for(int i = 0; i < 10; i++)
            {
                for(int j = 0; j < 11; j++)
                {
                    motors[i][j].setAngle(newangle);
                }
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


        //for(int i = 0; i < 10; i++)
        //{
        //for(int j = 0; j < 11; j++)
        //{
        //motors[i][j].setPid(kp, ki, kd);
        //}
        //}

        for(int i = 0; i < 10; i++)
        {
            for(int j = 0; j < 11; j++)
            {
                for(int k = 0; k < 10; k++)
                {

                    motors[i][j].update(deltatime);
                }
            }
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
