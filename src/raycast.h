#ifndef CS430_RAYCAST_H
#define CS430_RAYCAST_H

#include <stddef.h>

#include "scene.h"
#include "image.h"

void raycast(pixel* pixels, size_t width, size_t height, const sceneObj* objs);

#endif // CS430_RAYCAST_H
