#include <stdio.h>
#include <SDL.h>
#include <SDL_image.h>
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#if defined(__APPLE__) && defined(__MACH__)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#include "vuzix.h"

#define RES_WIDTH  1024
#define RES_HEIGHT 768

GLUquadricObj *sphere;
GLfloat yaw = 0.0f, raw_yaw = 0.0f, zero_yaw = 0.0f;
GLfloat roll = 0.0f, raw_roll = 0.0f, zero_roll = 0.0f;
GLfloat pitch = 0.0f, raw_pitch = 0.0f, zero_pitch = 0.0f;
GLuint texture;

void loadTexture(const char *filename, GLuint *tex)
{
    GLenum texture_format;
    GLint nOfColors;
    SDL_Surface *sur = IMG_Load(filename);
    if (!sur) return;
    nOfColors = sur->format->BytesPerPixel;
    if (nOfColors == 4)
    {
        if (sur->format->Rmask == 0x000000ff)
            texture_format = GL_RGBA;
        else
            texture_format = GL_BGRA;
    } else if (nOfColors == 3) {
        if (sur->format->Rmask == 0x000000ff)
            texture_format = GL_RGB;
        else
            texture_format = GL_BGR;
    } else {
        return;
    }
    glGenTextures(1, tex);
    glBindTexture(GL_TEXTURE_2D, *tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, sur->w, sur->h, 0, texture_format, GL_UNSIGNED_BYTE, sur->pixels);
    SDL_FreeSurface(sur);
}

void initGL(int width, int height, const char *filename)
{
    glViewport(0, 0, width, height);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(32.0f, (GLfloat)width/(GLfloat)height, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_TEXTURE_2D);

    sphere = gluNewQuadric();
    gluQuadricDrawStyle(sphere, GLU_FILL);
    gluQuadricTexture(sphere, GL_TRUE);
    gluQuadricNormals(sphere, GLU_SMOOTH);

    loadTexture(filename, &texture);
}

void deinitGL()
{
    gluDeleteQuadric(sphere);
    glDeleteTextures(1, &texture);
}

void drawGLScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);

    glBindTexture(GL_TEXTURE_2D, texture);
    glRotatef(pitch, 1.0f, 0.0f, 0.0f);
    glRotatef(roll, 0.0f, 1.0f, 0.0f);
    glRotatef(yaw, 0.0f, 0.0f, 1.0f);
    gluSphere(sphere, 10.0f, 16, 16);

    SDL_GL_SwapBuffers();
}

int handleEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) return 1;
        if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_ESCAPE) return 1;
            if (event.key.keysym.sym == SDLK_r) {
                zero_yaw = raw_yaw;
                zero_roll = raw_roll;
                zero_pitch = raw_pitch;
                return 0;
            }
        }
    }
    return 0;
}

void readAngles()
{
    vuzix_read(&raw_pitch, &raw_roll, &raw_yaw);
    // printf("%f %f %f\n", yaw, pitch, roll);
    yaw   = raw_yaw - zero_yaw;
    pitch = raw_pitch - zero_pitch;
    roll  = raw_roll - zero_roll;
}

int main(int argc, char **argv)
{

    if (argc < 2) {
        printf("Usage: spherevr texture\n");
        return 3;
    }

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Could not initialize SDL: %s\n", SDL_GetError());
        return 1;
    }

    if (SDL_SetVideoMode(RES_WIDTH, RES_HEIGHT, 0, SDL_OPENGL) == NULL) {
        fprintf(stderr, "Could not create OpenGL window: %s\n", SDL_GetError());
        SDL_Quit();
        return 2;
    }

    vuzix_open("/dev/hidraw0");

    SDL_WM_SetCaption("SphereVR", NULL);

    initGL(RES_WIDTH, RES_HEIGHT, argv[1]);
    for (;;) {
        readAngles();
        drawGLScene();
        if (handleEvents()) break;
        SDL_Delay(1);
    }
    deinitGL();

    vuzix_close();

    SDL_Quit();

    return 0;
}
