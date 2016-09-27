#include <stdio.h>

#include "json.h"
#include "scene.h"
#include "raycast.h"

int main(int argc, char const *argv[]) {
    if(argc < 5) {
        fprintf(stderr, "usage: raycast width height /path/to/input.json "
            "/path/to/output.ppm\n");
        return 1;
    }
    jsonObj jsonObj = readScene(argv[3]);
    if(jsonObj.objsSize == 0) {
        return 0;
    }

    vector3d zeroVector = { 0, 0, 0 };

    for(size_t i = 0; i < jsonObj.objsSize; i++) {
        if(vector3d_compare(jsonObj.objs[i].normal, zeroVector) != 0) {
            vector3d_normalize(jsonObj.objs[i].normal);
        }
    }

    pixel* pixels;
    size_t width;
    size_t height;

    raycast(pixels, width, height, jsonObj.camera, jsonObj.objs, jsonObj.objsSize);

    return 0;
}
