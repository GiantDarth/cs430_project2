#ifndef CS430_SCENE_H
#define CS430_SCENE_H

#include "vector3d.h"
#include "image.h"

typedef struct sceneObj {
    char* type;
    double width;
    double height;
    double radius;
    vector3d pos;
    pixel color;
    vector3d normal;
} sceneObj;

#endif // CS430_SCENE_H
