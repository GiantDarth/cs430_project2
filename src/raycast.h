#ifndef CS430_RAYCAST_H
#define CS430_RAYCAST_H

#include <stddef.h>

#include "scene.h"
#include "image.h"

void raycast(pixel* pixels, size_t width, size_t height, camera camera,
        const sceneObj* objs, size_t objsSize);

#endif // CS430_RAYCAST_H
