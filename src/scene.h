#ifndef CS430_SCENE_H
#define CS430_SCENE_H

#include "vector3d.h"

#define TYPE_SPHERE 0
#define TYPE_PLANE 1

typedef struct pixel {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
} pixel;

typedef struct sceneObj {
    int type;
    pixel color;
    union {
        struct {
            vector3d pos;
            double radius;
        } sphere;
        struct {
            vector3d pos;
            vector3d normal;
        } plane;
        struct {
            vector3d pos;
            double radius;
            double height;
        } cylinder;
    };
} sceneObj;

typedef struct camera {
    float width;
    float height;
} camera;

typedef struct ray {
    vector3d origin;
    vector3d dir;
} ray;

double sphere_intersection(ray ray, sceneObj obj);
double plane_intersection(ray ray, sceneObj obj);
double cylinder_intersection(ray ray, sceneObj obj);

#endif // CS430_SCENE_H
