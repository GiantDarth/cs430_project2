#ifndef CS430_SCENE_H
#define CS430_SCENE_H

typedef struct sceneObj {
    char* type;
    float width;
    float height;
    float radius;
    float pos[3];
    float color[3];
    float normal[3];
} sceneObj;

#endif // CS430_SCENE_H
