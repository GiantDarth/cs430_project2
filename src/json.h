#ifndef CS430_JSON_H
#define CS430_JSON_H

#include <stddef.h>

#include "raycast.h"
#include "scene.h"

typedef struct jsonObj {
    camera camera;
    sceneObj* objs;
    size_t objsSize;
} jsonObj;

jsonObj readScene(const char* path);

#endif // CS430_JSON_H
