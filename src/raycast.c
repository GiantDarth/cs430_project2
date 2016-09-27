#include <stdlib.h>
#include <math.h>
#include <float.h>
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
    ray ray;
    point.z = 1;

    for(size_t i = 0; i < width; i++) {
        point.y = center.y - (camera.height / 2) + PIXEL_HEIGHT * (i + 0.5);
        for(size_t j = 0; j < height; j++) {
            point.x = center.x - (camera.width / 2) + PIXEL_WIDTH * (j + 0.5);
            ray.origin = point;
            ray.dir = vector3d_normalize(point);
            intersected = shoot(ray, objs, objsSize);
            pixels[i * height + j] = shade(*intersected);
        }
    }
}



const sceneObj* shoot(ray ray, const sceneObj* objs, size_t objsSize) {
    vector3d closest = { FLT_MAX, FLT_MAX, FLT_MAX };
    const sceneObj* closestObj = NULL;
    vector3d point;
    double t_close, value, length;

    for(size_t i = 0; i < objsSize; i++) {
        if(strcmp(objs[i].type, "sphere") == 0) {
            t_close = vector3d_dot(ray.dir, vector3d_sub(objs[i].pos, ray.origin));
            point = vector3d_add(ray.origin, vector3d_scale(ray.dir, t_close));
            length = vector3d_magnitude(vector3d_sub(point, objs[i].pos));
            if(length < objs[i].radius) {
                value = sqrt(pow(objs[i].radius, 2) - pow(length, 2));
                point = vector3d_add(ray.origin,
                    vector3d_scale(ray.dir, t_close - value));
                if(vector3d_compare(point, closest) < 0) {
                    closest = point;
                    closestObj = &(objs[i]);
                }
            }
            else if(length == objs[i].radius) {
                if(vector3d_compare(point, closest) < 0) {
                    closest = point;
                    closestObj = &(objs[i]);
                }
            }
            // Else no intersection
        }
        else if(strcmp(objs[i].type, "plane") == 0) {
            value = - (vector3d_dot(objs[i].normal, vector3d_sub(ray.origin, objs[i].pos)) /
                (vector3d_dot(objs[i].normal, ray.dir)));
            point = vector3d_add(ray.origin, vector3d_scale(ray.dir, value));
            if(vector3d_compare(point, closest) < 0) {
                closest = point;
                closestObj = &(objs[i]);
            }
        }
    }

    return closestObj;
}

pixel shade(sceneObj intersected) {
    return intersected.color;
}
