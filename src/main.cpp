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
motorcontroller mot1;

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
    glOrtho(-100 , 100 , 100 , -100 , -100, 100);
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
    glColor3f(1, 1, 1);
    mot1.draw();
    glPointSize(5);
    glBegin(GL_POINTS);

    for(int i = -10; i < 10; i++)
    {
        for(int j = -10; j < 10; j++)
        {
            glVertex2f(i * 100, j * 100);
        }
    }

    glColor3f(0, 0, 1);
    glVertex2f(0, 0);
    glEnd();
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
                    quit = true;
                    break;
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
            mot1.setAngle(rand()%50);
        }

        mot1.update();

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-1920 / 2 , 1920 / 2 , 600, -600 , -100, 100);
        glMatrixMode(GL_MODELVIEW);

        Display_Render();
        SDL_RenderPresent(displayRenderer);
    }

    SDL_Quit();
    return 0;
}
