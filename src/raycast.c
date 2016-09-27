#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <string.h>

#include "image.h"
#include "raycast.h"
#include "vector3d.h"

typedef struct camera {
    float width;
    float height;
} camera;

typedef struct ray {
    vector3d origin;
    vector3d dir;
} ray;

vector3d shoot(ray ray, const sceneObj* objs, size_t objsSize);
pixel shade(vector3d intersection);

void raycast(pixel* pixels, size_t width, size_t height, camera* camera,
        const sceneObj* objs, size_t objsSize) {
    const vector3d center = { 0, 0, 0 };
    const double PIXEL_WIDTH = camera->width / (float)width;
    const double PIXEL_HEIGHT = camera->height / (float)height;

    vector3d point, intersection;
    ray ray;
    point.z = 1;

    for(size_t i = 0; i < width; i++) {
        point.y = center.y - (camera->height / 2) + PIXEL_HEIGHT * (i + 0.5);
        for(size_t j = 0; j < height; j++) {
            point.x = center.x - (camera->width / 2) + PIXEL_WIDTH * (j + 0.5);
            ray.origin = point;
            ray.dir = normalize(point);
            intersection = shoot(ray, objs, objsSize);
            pixels[i * height + j] = shade(intersection);
        }
    }
}



vector3d shoot(ray ray, const sceneObj* objs, size_t objsSize) {
    vector3d closest = { FLT_MAX, FLT_MAX, FLT_MAX };
    vector3d point;
    double value;

    for(size_t i = 0; i < objsSize; i++) {
        if(strcmp(objs[i].type, "sphere") == 0) {

        }
        else if(strcmp(objs[i].type, "plane") == 0) {
            value = - (vector3d_dot(objs[i].normal, vector3d_sub(ray.origin, objs[i].pos)) /
                (vector3d_dot(objs[i].normal, ray.dir)));
            point = vector3d_add(ray.origin, vector3d_scale(ray.dir, value));
        }
    }

    return closest;
}

pixel shade(vector3d intersection) {

}
