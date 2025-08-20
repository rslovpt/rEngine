#include "engine.h"
#include "matrix/engine_matrix.h"
#include "camera/engine_camera.h"

#include <stdio.h>
#include <stdlib.h> 
#include <stdbool.h>
#include <SDL.h>
#include <SDL.h>


EngineDependencies* EngineDep;
SDL_Renderer* renderer;
CubeObj* WorldObjects = NULL;
int ObjCount = 0;

float tempAngle = 24;
int FOV = 90;
double *deltatime;

int engine_z_offset = 3;

bool EdgeLineEnabled = false;


typedef struct {
    int vertices[4];  
    float depth;      
} FaceDepth;


int DrawPoint(float x, float y, int s)
{

    int CommonErrorCode = 0;
    for (int y_ = 0; y_ < s; y_++)
    {
        for (int x_ = 0; x_ < s; x_++)
        {
            CommonErrorCode += SDL_RenderDrawPoint(renderer, x+x_, y+y_);
        }
    }

    if (CommonErrorCode != 0)
    {
        CommonErrorCode = 1;
    } else {
        CommonErrorCode = 0;
    }
    return CommonErrorCode;
}

float AverageFaceZ(MatrixPoint RotatedPoints[], int face[4]) {
    float average = 0.0f;
    for (int i = 0; i < 4; i++) {
        average += RotatedPoints[face[i]].z;
    }
    return average / 4.0f;
}

int CompareFaces(const void *a, const void *b) {
    FaceDepth *fa = (FaceDepth*)a;
    FaceDepth *fb = (FaceDepth*)b;
    if (fa->depth < fb->depth) return 1;  
    if (fa->depth > fb->depth) return -1;
    return 0;
}

void ObjectSimpleFaceHandle(CubeObj* Object, MatrixPoint RotatedPoints[]) {
    int numFaces = sizeof(Object->Faces) / sizeof(Object->Faces[0]);

    FaceDepth faces[numFaces];

    for (int i = 0; i < numFaces; i++) {
        for (int j = 0; j < 4; j++) {
            faces[i].vertices[j] = Object->Faces[i][j];
        }
        faces[i].depth = AverageFaceZ(RotatedPoints, Object->Faces[i]);
    }

    qsort(faces, numFaces, sizeof(FaceDepth), CompareFaces);

    for (int i = 0; i < numFaces; i++) {
        int a = faces[i].vertices[0];
        int b = faces[i].vertices[1];
        int c = faces[i].vertices[2];
        int d = faces[i].vertices[3];

        // draw_quad(...) equivalent
    }
}

void Init(EngineDependencies* EngDependency, SDL_Renderer* Wrenderer) 
{
    EngineDep = EngDependency;
    renderer = Wrenderer;

    deltatime = EngineDep->deltatime;
    float AspectRatio = (float)EngineDep->WindowSize[1] / EngineDep->WindowSize[0];

    SetupPlane(EngineDep->farPlane, EngineDep->nearPlane, AspectRatio, EngineDep->WindowSize);
}

void EnableObjectLines(CubeObj* Object, int Edges[12][2])
{
    
    memcpy(Object->Edges, Edges, sizeof(Object->Edges));
    EdgeLineEnabled = true;
}

CubeObj* CreateObject(float Vertices[8][3], int Faces[6][4], float ColorPallete[6][3], float Position[3], float Orientation[2])
{
    CubeObj object;

    memcpy(object.Vertices, Vertices, sizeof(object.Vertices));
    memcpy(object.Faces, Faces, sizeof(object.Faces));
    memcpy(object.ColorPallete, ColorPallete, sizeof(object.ColorPallete));
    memcpy(object.Position, Position, sizeof(object.Position));
    memcpy(object.Orientation, Orientation, sizeof(object.Orientation));

    ObjCount++;
    WorldObjects = realloc(WorldObjects, ObjCount * sizeof(CubeObj));
    WorldObjects[ObjCount-1] = object;

    return &WorldObjects[ObjCount-1];
}

void Render(CubeObj* Object) // Render a Cube Object
{   
    float cx = 0.5f;
    float cy = 0.5f;
    float cz = 0.5f;

    SDL_SetRenderDrawColor(renderer, 255,255,255,255);

    bool OffScreen = false;

    Point2D TransformedPoints[sizeof(Object->Vertices) / sizeof(Object->Vertices[0])];
    MatrixPoint PreRotatedPoints[sizeof(Object->Vertices) / sizeof(Object->Vertices[0])];
    MatrixPoint RotatedPoints[sizeof(Object->Vertices) / sizeof(Object->Vertices[0])];

    Camera* CurrentCam = ObtainRecord();

    for (int i = 0; i < sizeof(Object->Vertices) / sizeof(Object->Vertices[0]); i++)
    {
        MatrixPoint Point = {
            .x = Object->Vertices[i][0] - cx,
            .y = Object->Vertices[i][1] - cy,
            .z = Object->Vertices[i][2] - cz
        };
            
        TransformMatrix_X(&Point, Object->Orientation[0]); // Change both to Temp Angle for demo
        TransformMatrix_Y(&Point, Object->Orientation[1]);
        
        // WORLD POINT
        Point.x += Object->Position[0]; 
        Point.y += Object->Position[1];
        Point.z += Object->Position[2];

        // CAM_REL
        Point.x -= CurrentCam->position[0];
        Point.y -= CurrentCam->position[1];
        Point.z -= CurrentCam->position[2] - engine_z_offset;
        
        TransformMatrix_X(&Point, -CurrentCam->orientation[1]); 
        TransformMatrix_Y(&Point, -CurrentCam->orientation[0]);  

        PreRotatedPoints[i] = Point;
        TransformToProjection(&Point, FOV); // Transformed , Temp FOV
        RotatedPoints[i] = Point;

        if (Point.z >= 1)
        {
            OffScreen = true;
        } else {
            Point2D CPoint = ToCartesianPoint(&Point);
            //printf("%f, %f \n", CPoint.x, CPoint.y);
            int error = DrawPoint(CPoint.x, CPoint.y, 4);
            TransformedPoints[i] = CPoint; 
            
            if (error != 0)
            {
                printf("DrawPoint failed: %s \n", SDL_GetError());
            }
        } 
    }

    if (!OffScreen)
    {
        if (EdgeLineEnabled)
        {
            for (int i = 0; i < sizeof(Object->Edges) / sizeof(Object->Edges[0]); i++)
            {
                int a = Object->Edges[i][0]; int b = Object->Edges[i][1];
                SDL_RenderDrawLine(renderer, TransformedPoints[a].x, TransformedPoints[a].y,TransformedPoints[b].x,TransformedPoints[b].y);
            }
        }


        // Face Functions

        ObjectSimpleFaceHandle(Object, RotatedPoints);
    }

}

void RenderAll() // Render all Cube Objects (Utilizes WorldObject array)
{

}

void CleanEngine() 
{
    if (WorldObjects != NULL) {
        free(WorldObjects);
        WorldObjects = NULL;
    }

    printf("CleanEngine Done");
}