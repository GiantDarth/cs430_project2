#include <stdio.h>

#include "json.h"
#include "scene.h"

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


    return 0;
}
