#include <stdlib.h>
#include <math.h>
#include <float.h>

#include "raycast.h"

typedef struct vector3d {
    float x;
    float y;
    float z;
} vector3d;

typedef struct camera {
    float width;
    float height;
} camera;

vector3d normalize(vector3d vector);
vector3d getClosest(vector3d vector, const sceneObj* objs);

void raycast(pixel* pixels, size_t width, size_t height, camera* camera,
        const sceneObj* objs) {
    const vector3d ORIGIN = { 0, 0, 0 };
    const float PIXEL_WIDTH = camera->width / (float)width;
    const float PIXEL_HEIGHT = camera->height / (float)height;

    vector3d point;
    point.z = 1;

    for(size_t i = 0; i < width; i++) {
        point.y = ORIGIN.y - (camera->height / 2) + PIXEL_HEIGHT * (i + 0.5);
        for(size_t j = 0; j < height; j++) {
            point.x = ORIGIN.x - (camera->width / 2) + PIXEL_WIDTH * (j + 0.5);
            point = normalize(point);
            pixels[i * height + j];
        }
    }
}

vector3d normalize(vector3d vector) {
    float magnitude = sqrt(pow(vector.x, 2) + pow(vector.y, 2) + pow(vector.z, 2));
    vector3d normal = { vector.x / magnitude, vector.y / magnitude,
        vector.z / magnitude };

    return normal;
}

vector3d getClosest(vector3d vector, const sceneObj* objs) {
    vector3d closest = { FLT_MAX, FLT_MAX, FLT_MAX };

    return closest;
}
