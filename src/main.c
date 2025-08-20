#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include "engine.h"

#include "camera/engine_camera.h"

Uint64 now, last;
double deltaTime;
double targetFrameTime = 1.0 / 60.0; // seconds per frame

int quit = 0;

CubeObj* TestSquare;
Camera* MyCamera;
float CameraMovementSpeed = 0.1f;

void program(int size[2] , SDL_Renderer* renderer){
  EngineDependencies dep = {
      .farPlane = 1000.0f,
      .nearPlane = 0.1f,
      .deltatime = &deltaTime
  };

  float p[3] = {0,0,0};
  float o[2] = {0,0};

  memcpy(dep.WindowSize, size, sizeof(dep.WindowSize));

  Init(&dep, renderer);
  MyCamera = InitCamera(p, o);

  float Vertices[8][3] = {
    {0,0,0},
    {1,0,0},
    {0,1,0},
    {1,1,0},
    {0,0,1},
    {1,0,1},
    {0,1,1},
    {1,1,1}
  };

  int Faces[6][4] = {
    {1,1,1,1},
    {1,1,1,1},
    {1,1,1,1},
    {1,1,1,1},
    {1,1,1,1},
    {1,1,1,1}
  };

  float Color[6][3] = {
    {255,255,255},
    {255,255,255},
    {255,255,255},
    {255,255,255},
    {255,255,255},
    {255,255,255}
  };

  int edges[12][2] = {
    {0, 1}, {1, 3}, {3, 2}, {2, 0}, 
    {4, 5}, {5, 7}, {7, 6}, {6, 4},  
    {0, 4}, {1, 5}, {2, 6}, {3, 7}   
  };

  float Position[3] = {0,0,0};
  float Orientation[2] = {0,0};

  TestSquare = CreateObject(Vertices, Faces, Color, Position, Orientation);

  EnableObjectLines(TestSquare, edges);
}

void run(float mouse_x, float mouse_y) {

  // MOVEMENT
  
  const Uint8 *state = SDL_GetKeyboardState(NULL);
  if (state[SDL_SCANCODE_S])
  {
    float move_back[3] = {0,0,-CameraMovementSpeed};
    TranslateCameraIncrement(move_back);
  } else if (state[SDL_SCANCODE_W]) {
    float move_forward[3] = {0,0,CameraMovementSpeed};
    TranslateCameraIncrement(move_forward);
  }

  if (state[SDL_SCANCODE_D])
  {
    float move_right[3] = {CameraMovementSpeed,0,0};
    TranslateCameraIncrement(move_right);
  } else if (state[SDL_SCANCODE_A]) {
    float move_left[3] = {-CameraMovementSpeed,0,0};
    TranslateCameraIncrement(move_left);
  }

  if (state[SDL_SCANCODE_E])
  {
    float move_up[3] = {0,CameraMovementSpeed,0};
    TranslateCameraIncrement(move_up);
  } else if (state[SDL_SCANCODE_Q]) {
    float move_down[3] = {0,-CameraMovementSpeed,0};
    TranslateCameraIncrement(move_down);
  }
  
  float Rotate[2] = {
    -mouse_y*0.002f, mouse_x*0.002f // Eventually make it Engine fix the flipping issue
  };
  RotateCameraIncrement(Rotate);

  Render(TestSquare);
}


int main(int argc, char* argv[]) {
    SDL_Window* window;
    SDL_Renderer* renderer;


    SDL_Event event;

    last = SDL_GetPerformanceCounter();

    int size[2] = {1280,720};

    int result = SDL_Init(SDL_INIT_EVERYTHING);
    window = SDL_CreateWindow("3D Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, size[0],size[1], SDL_WINDOW_SHOWN);

    if (window == NULL) goto CLEANUP_QUIT;

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (renderer == NULL) goto CLEANUP_QUIT;


    program(size, renderer);

    SDL_SetRelativeMouseMode(SDL_TRUE);

    while (!quit) {
        now = SDL_GetPerformanceCounter();
        deltaTime = (double)(now - last) / SDL_GetPerformanceFrequency();
        last = now;

        const Uint8 *state = SDL_GetKeyboardState(NULL);
        if (state[SDL_SCANCODE_ESCAPE]) {
            quit = 1;
            break;
        }

        // Always reset before polling
        float dx = 0;
        float dy = 0;

        while (SDL_PollEvent(&event) != 0) {
            switch (event.type) {
                case SDL_MOUSEMOTION:
                    dx += event.motion.xrel; // Change in X since last event
                    dy += event.motion.yrel;
                    break; // << important
                case SDL_QUIT:
                    quit = 1;
                    break;
            }
        }

        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        SDL_RenderClear(renderer);

        run(dx, dy);

        SDL_RenderPresent(renderer);

        double frameTime = (double)(SDL_GetPerformanceCounter() - now) / SDL_GetPerformanceFrequency();
        double delay = targetFrameTime - frameTime;
        if (delay > 0) {
            SDL_Delay((Uint32)(delay * 1000.0));
        }
    }

    CLEANUP_QUIT:
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    CleanEngine();

    SDL_Quit();

    return 0;
}