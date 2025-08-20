#include "engine_matrix.h"

#include <math.h>
#include <stdio.h>

#define PI 3.14159265358979323846

float farplane;
float nearplane;
float AR;

int WindowSize[2];

MatrixPoint* QuickPointerAsign(MatrixPoint* p, MatrixPoint v)
{
    p->x = v.x;
    p->y = v.y;
    p->z = v.z;
    return p;
}

void SetupPlane(float farPlane, float nearPlane, float AspectRatio, int WS[2])
{
    farplane = farPlane;
    nearplane = nearPlane;
    AR = AspectRatio;

    WindowSize[0] = WS[0];
    WindowSize[1] = WS[1];
}

MatrixPoint* TransformMatrix_X(MatrixPoint* p, float angle)
{
   MatrixPoint TransformedPoint = {.x = 0, .y = 0, .z = 0}; 

   TransformedPoint.x = cosf(angle)*p->x + 0 + sinf(angle)*p->z;
   TransformedPoint.y = 1*p->y;
   TransformedPoint.z = -sinf(angle)*p->x + 0 + cosf(angle)*p->z;

   return QuickPointerAsign(p, TransformedPoint);
}

MatrixPoint* TransformMatrix_Y(MatrixPoint* p, float angle)
{
    MatrixPoint TransformedPoint = {.x = 0, .y = 0, .z = 0}; 

    TransformedPoint.x = 1*p->x;
    TransformedPoint.y = 0 + cosf(angle)*p->y - sinf(angle)*p->z;
    TransformedPoint.z = 0 + sinf(angle)*p->y + cosf(angle)*p->z;

    return QuickPointerAsign(p, TransformedPoint);
}

MatrixPoint* TransformToProjection(MatrixPoint* p, float FOV)
{
    float focalPoint = 1 / (tanf(FOV * .5 / 180*PI));
    MatrixPoint TransformedPoint = {.x = 0, .y = 0, .z = 0}; 

    TransformedPoint.x = (AR * focalPoint * p->x);
    TransformedPoint.y = (focalPoint * p->y);
    TransformedPoint.z = ( (p->z * farplane / (farplane - nearplane)) - ((farplane * nearplane) / (farplane - nearplane)) );

    if (p->z != 0) {
        TransformedPoint.x /= p->z;
        TransformedPoint.y /= p->z;
        TransformedPoint.z /= p->z;
    }

    return QuickPointerAsign(p, TransformedPoint);
}

Point2D ToCartesianPoint(MatrixPoint* p)
{
    Point2D transformed;

    transformed.x = (p->x + 1) * (1280 / 2);
    transformed.y = (p->y + 1) * (720 / 2);

    return transformed;
}