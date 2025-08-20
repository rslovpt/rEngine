#ifndef ENGINE_CAMERA
#define ENGINE_CAMERA

typedef struct {
    float orientation[2];
    float position[3];
} Camera;

Camera* InitCamera(float Position[3], float Orientation[2]);

void TranslateCameraIncrement(float movement_added[3]);
void RotateCameraIncrement(float orientation_added[2]);

Camera* ObtainRecord();

#endif