#include <stdio.h>

#include "json.h"
#include "scene.h"

int main(int argc, char const *argv[]) {
    if(argc < 5) {
        fprintf(stderr, "usage: raycast width height /path/to/input.json "
            "/path/to/output.ppm\n");
        return 1;
    }
    sceneObj* objs = readScene(argv[3]);
    if(objs == NULL) {
        return 0;
    }

    return 0;
}
