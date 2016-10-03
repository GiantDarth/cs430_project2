#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "image.h"
#include "raycast.h"
#include "vector3d.h"

const sceneObj* shoot(ray ray, const sceneObj* objs, size_t objsSize);
pixel shade(sceneObj intersected);

void raycast(pixel* pixels, size_t width, size_t height, camera camera,
        const sceneObj* objs, size_t objsSize) {
    const vector3d center = { 0, 0, 0 };
    const double PIXEL_WIDTH = camera.width / (float)width;
    const double PIXEL_HEIGHT = camera.height / (float)height;

    vector3d point;
    const sceneObj* intersected;
    // Initialize ray as origin and dir of { 0, 0, 0 }
    ray ray = { 0 };
    point.z = 1;

    // Initialize all pixels to black
    memset(pixels, 0, sizeof(*pixels) * width * height);

    for(size_t i = 0; i < width; i++) {
        point.y = center.y - (camera.height / 2) + PIXEL_HEIGHT * (i + 0.5);
        for(size_t j = 0; j < height; j++) {
            point.x = center.x - (camera.width / 2) + PIXEL_WIDTH * (j + 0.5);
            ray.dir = vector3d_normalize(point);
            intersected = shoot(ray, objs, objsSize);
            if(intersected != NULL) {
                pixels[i * height + j] = shade(*intersected);
            }
        }
    }
}

const sceneObj* shoot(ray ray, const sceneObj* objs, size_t objsSize) {
    double closest = INFINITY;
    const sceneObj* closestObj = NULL;
    double t;

    for(size_t i = 0; i < objsSize; i++) {
        switch(objs[i].type) {
            case(TYPE_SPHERE):
                t = sphere_intersection(ray, objs[i]);
                break;
            case(TYPE_PLANE):
                t = plane_intersection(ray, objs[i]);
                break;
            default:
                fprintf(stderr, "Error: Invalid obj type\n");
                exit(1);
        }
        if(t > 0 && t < closest) {
            closest = t;
            closestObj = &(objs[i]);
        }
    }

    return closestObj;
}

pixel shade(sceneObj intersected) {
    return intersected.color;
}
