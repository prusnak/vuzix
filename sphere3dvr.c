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

#define RES_WIDTH  640
#define RES_HEIGHT 480

GLUquadricObj *sphere;
GLfloat yaw = 0.0f;
GLfloat roll = 0.0f;
GLfloat pitch = 0.0f;
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, sur->w, sur->h, 0, GL_BGR, GL_UNSIGNED_BYTE, sur->pixels);
    gluBuild2DMipmaps(*tex, 3, sur->w, sur->h, texture_format, GL_UNSIGNED_BYTE, sur->pixels);
    SDL_FreeSurface(sur);
}

void initGL(int width, int height)
{
    glViewport(0, 0, width, height);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (GLfloat)width/(GLfloat)height, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_TEXTURE_2D);

    sphere = gluNewQuadric();
    gluQuadricDrawStyle(sphere, GLU_FILL);
    gluQuadricTexture(sphere, GL_TRUE);
    gluQuadricNormals(sphere, GLU_SMOOTH);

    loadTexture("test.jpg", &texture);
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

    glTranslatef(0.0f, 0.0f, -6.0f);
    glBindTexture(GL_TEXTURE_2D, texture);
    gluSphere(sphere, 1.0f, 8, 4);

    SDL_GL_SwapBuffers();
}

int handleEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) return 1;
        if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_ESCAPE) return 1;
        }
    }
    return 0;
}

int main(int argc, char **argv)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Could not initialize SDL: %s\n", SDL_GetError());
        return 1;
    }

    if (SDL_SetVideoMode(RES_WIDTH, RES_HEIGHT, 0, SDL_OPENGL) == NULL) {
        fprintf(stderr, "Could not create OpenGL window: %s\n", SDL_GetError());
        SDL_Quit();
        return 2;
    }

    SDL_WM_SetCaption("Sphere3DVR", NULL);

    initGL(RES_WIDTH, RES_HEIGHT);
    for (;;) {
        drawGLScene();
        if (handleEvents()) break;
        SDL_Delay(1);
    }
    deinitGL();

    SDL_Quit();

    return 0;
}
