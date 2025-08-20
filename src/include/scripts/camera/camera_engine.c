#include "camera/engine_camera.h"

#include <stdio.h>
#include <stdlib.h> 
#include <stdbool.h>
#include <string.h>

Camera CurrentCamera;
bool Inited = false;

Camera* InitCamera(float Position[3], float Orientation[2])
{
    memcpy(CurrentCamera.position, Position, sizeof(CurrentCamera.position));
    memcpy(CurrentCamera.orientation, Orientation, sizeof(CurrentCamera.orientation));

    Inited = true;

    return &CurrentCamera;
}


void TranslateCameraIncrement(float movement_added[3])
{
    if (Inited)
    {
        float new_array[3] = {
            CurrentCamera.position[0]-movement_added[0],
            CurrentCamera.position[1]-movement_added[1],
            CurrentCamera.position[2]+movement_added[2]
        };
        memcpy(CurrentCamera.position, new_array, sizeof(CurrentCamera.position));
    } else {
        printf("Warning! Camera System has not been Initialized");
    }

}

void RotateCameraIncrement(float orientation_added[2])
{
    if (Inited)
    {
        float new_array[2] = {
            CurrentCamera.orientation[0]+orientation_added[0],
            CurrentCamera.orientation[1]+orientation_added[1]
        };
        memcpy(CurrentCamera.orientation, new_array, sizeof(CurrentCamera.orientation));
    } else {
        printf("Warning! Camera System has not been Initialized");
    }
}

Camera* ObtainRecord()
{
    if (Inited)
    {
        return &CurrentCamera;
    } else {
        printf("Warning! Camera System has not been Initialized (ObtainRecord Failure)");
    }
}