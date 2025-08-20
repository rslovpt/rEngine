#include <SDL.h>

#ifndef ENGINE_H
#define ENGINE_H

typedef struct {
    float farPlane;
    float nearPlane;

    int WindowSize[2];
    double *deltatime;
} EngineDependencies;

typedef struct {
    float Vertices[8][3];
    int Faces[6][4];
    float ColorPallete[6][3];
    int Edges[12][2];

    float Position[3];
    float Orientation[2];
} CubeObj;

void Init(EngineDependencies* EngDependency, SDL_Renderer* Wrenderer);

CubeObj* CreateObject(float Vertices[8][3],
                int Faces[6][4],
                float ColorPallete[6][3],
                float Position[3],
                float Orientation[2]
            );

void RenderAll();

void Render(CubeObj* Object);
void EnableObjectLines(CubeObj* Object, int Edges[12][2]);

void CleanEngine();

#endif