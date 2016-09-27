#define __USE_MINGW_ANSI_STDIO 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <errno.h>

#include "vector3d.h"
#include "json.h"

void errorCheck(int c, FILE* json, size_t line);
void tokenCheck(int c, char token, size_t line);
int jsonGetC(FILE* json, size_t* line);
void skipWhitespace(FILE* json, size_t* line);
void trailSpaceCheck(FILE* json, size_t* line);
char* nextString(FILE* json, size_t* line);
double nextNumber(FILE* json, size_t* line);
vector3d nextVector3d(FILE* json, size_t* line);
pixel nextColor(FILE* json, size_t* line);

jsonObj readScene(const char* path) {
    FILE* json = fopen(path, "r");
    if(json == NULL) {
        perror("Error: Opening input\n");
        exit(1);
    }

    camera camera = { 0, 0 };
    sceneObj* objs = NULL;
    size_t objsSize = 0;

    jsonObj jsonObj;
    jsonObj.camera = camera;
    jsonObj.objs = objs;
    jsonObj.objsSize = objsSize;

    int c;
    size_t line = 1;
    char* key, *type;
    vector3d zeroVector = { 0, 0, 0 };
    pixel zeroColor = { 0, 0, 0 };

    // Ignore beginning whitespace
    skipWhitespace(json, &line);

    c = jsonGetC(json, &line);
    tokenCheck(c, '[', line);

    skipWhitespace(json, &line);
    c = jsonGetC(json, &line);
    if(c == ']') {
        fprintf(stderr, "Warning: Line %zu: Empty array\n", line);

        trailSpaceCheck(json, &line);

        return jsonObj;
    }

    if(ungetc(c, json) == EOF) {
        fprintf(stderr, "Error: Line %zu: Read error\n", line);
        perror("");
        exit(1);
    }

    do {
        skipWhitespace(json, &line);
        c = jsonGetC(json, &line);
        tokenCheck(c, '{', line);

        skipWhitespace(json, &line);
        c = jsonGetC(json, &line);
        // Empty object, skip to next object without allocating for empty one.
        if(c == '}') {
            fprintf(stderr, "Warning: Line %zu: Empty object\n", line);
            skipWhitespace(json, &line);
            continue;
        }
        else if(ungetc(c, json) == EOF) {
            fprintf(stderr, "Error: Line %zu: Read error\n", line);
            perror("");
            exit(1);
        }

        key = nextString(json, &line);

        if(strcmp(key, "type") != 0) {
            fprintf(stderr, "Error: First key must be 'type'\n");
            exit(1);
        }

        skipWhitespace(json, &line);
        c = jsonGetC(json, &line);
        tokenCheck(c, ':', line);

        skipWhitespace(json, &line);
        type = nextString(json, &line);

        if(strcmp(type, "plane") == 0 || strcmp(type, "sphere") == 0) {
            if((objs = realloc(objs, ++objsSize)) == NULL) {
                fprintf(stderr, "Error: Line %zu: Memory reallocation error\n",
                    line);
                perror("");
                exit(1);
            }

            objs[objsSize - 1].type = type;
            objs[objsSize - 1].normal = zeroVector;
            objs[objsSize - 1].pos = zeroVector;
            objs[objsSize - 1].radius = 0;
            objs[objsSize - 1].color = zeroColor;
        }
        else if(strcmp(type, "camera") != 0) {
            fprintf(stderr, "Error: Line %zu: Unknown type %s", line,
                objs[objsSize - 1].type);
            exit(1);
        }

        skipWhitespace(json, &line);
        while((c = jsonGetC(json, &line)) == ',') {
            skipWhitespace(json, &line);
            // Get key
            key = nextString(json, &line);

            // Get ':' token
            skipWhitespace(json, &line);
            c = jsonGetC(json, &line);
            tokenCheck(c, ':', line);

            skipWhitespace(json, &line);
            // TODO: Find way to remove redundant code
            if(strcmp(type, "camera") == 0) {
                if(strcmp(key, "width") == 0) {
                    camera.width = nextNumber(json, &line);
                    if(camera.width < 0) {
                        fprintf(stderr, "Error: Line %zu: Width cannot be negative\n",
                            line);
                        exit(1);
                    }
                }
                else if(strcmp(key, "height") == 0) {
                    camera.height = nextNumber(json, &line);
                    if(camera.height < 0) {
                        fprintf(stderr, "Error: Line %zu: Height cannot be negative\n",
                            line);
                        exit(1);
                    }
                }
                else {
                    fprintf(stderr, "Error: Line %zu: Key '%s' not supported "
                        "under 'camera'\n", line, key);
                    exit(1);
                }
            }
            else if(strcmp(type, "sphere") == 0) {
                if(strcmp(key, "color") == 0) {
                    objs[objsSize - 1].color = nextColor(json, &line);
                }
                else if(strcmp(key, "position") == 0) {
                    objs[objsSize - 1].pos = nextVector3d(json, &line);
                }
                else if(strcmp(key, "radius") == 0) {
                    objs[objsSize - 1].radius = nextNumber(json, &line);
                    if(objs[objsSize - 1].radius < 0) {
                        fprintf(stderr, "Error: Line %zu: Radius cannot be "
                            "negative\n", line);
                        exit(1);
                    }
                }
                else {
                    fprintf(stderr, "Error: Line %zu: Key '%s' not supported "
                        "under 'sphere'\n", line, key);
                    exit(1);
                }
            }
            else if(strcmp(type, "plane") == 0) {
                if(strcmp(key, "color") == 0) {
                    objs[objsSize - 1].color = nextColor(json, &line);
                }
                else if(strcmp(key, "position") == 0) {
                    objs[objsSize - 1].pos = nextVector3d(json, &line);
                }
                else if(strcmp(key, "normal") == 0) {
                    objs[objsSize - 1].normal = nextVector3d(json, &line);
                }
                else {
                    fprintf(stderr, "Error: Line %zu: Key '%s' not supported "
                        "under 'plane'\n", line, key);
                    exit(1);
                }
            }

            skipWhitespace(json, &line);
        }

        tokenCheck(c, '}', line);

        skipWhitespace(json, &line);
    }
    while((c = jsonGetC(json, &line)) == ',');

    tokenCheck(c, ']', line);

    trailSpaceCheck(json, &line);

    jsonObj.camera = camera;
    jsonObj.objs = objs;
    jsonObj.objsSize = objsSize;

    return jsonObj;
}

void errorCheck(int c, FILE* fp, size_t line) {
    if(c == EOF) {
        if(feof(fp)) {
            fprintf(stderr, "Error: Line %zu: Premature end-of-file\n", line);
            exit(1);
        }
        else if(ferror(fp)) {
            fprintf(stderr, "Error: Line %zu: Read error\n", line);
            perror("");
            exit(1);
        }
    }
}

void tokenCheck(int c, char token, size_t line) {
    if(c != token) {
        fprintf(stderr, "Error: Line %zu: Expected '%c'\n", line, token);
        exit(1);
    }
}

int jsonGetC(FILE* json, size_t* line) {
    int c = fgetc(json);
    errorCheck(c, json, *line);

    if(c == '\n') {
        *line += 1;
    }

    return c;
}

void skipWhitespace(FILE* json, size_t* line) {
    int c;

    c = jsonGetC(json, line);
    while(isspace(c)) {
        c = jsonGetC(json, line);
    }

    if(ungetc(c, json) == EOF) {
        fprintf(stderr, "Error: Line %zu: Read error\n", *line);
        perror("");
        exit(1);
    }
}


void trailSpaceCheck(FILE* json, size_t* line) {
    int c;

    // Manually get trailing whitespace
    while((c = fgetc(json)) != EOF && isspace(c)) {
        if(c == '\n') {
            *line += 1;
        }
    }

    if(c != EOF) {
        fprintf(stderr, "Error: Line %zu: Unkown token at end-of-file\n", *line);
        exit(1);
    }
    else if(!feof(json) && ferror(json)) {
        fprintf(stderr, "Error: Line %zu: Read error\n", *line);
        perror("");
        exit(1);
    }
}


char* nextString(FILE* json, size_t* line) {
    size_t bufferSize = 64;
    size_t oldSize = bufferSize;
    char* buffer = malloc(bufferSize);
    if(buffer == NULL) {
        fprintf(stderr, "Error: Line %zu: Memory allocation error\n", *line);
        perror("");
        exit(1);
    }
    int c;
    size_t i;

    c = jsonGetC(json, line);
    tokenCheck(c, '"', *line);

    i = 0;
    while(i < bufferSize - 1 && (c = jsonGetC(json, line)) != '"') {
        if(i == bufferSize - 2) {
            bufferSize *= 2;
            // Integer overflow
            if(oldSize != 0 && bufferSize / oldSize != 2) {
                fprintf(stderr, "Error: Line %zu: Integer overflow on size\n",
                    *line);
                exit(1);
            }

            oldSize = bufferSize;
            buffer = realloc(buffer, bufferSize);
            if(buffer == NULL) {
                fprintf(stderr, "Error: Line %zu: Memory reallocation error\n",
                    *line);
                perror("");
                exit(1);
            }
        }
        buffer[i++] = c;
    }
    buffer[i] = '\0';

    return buffer;
}

double nextNumber(FILE* json, size_t* line) {
    double value;
    int status = fscanf(json, "%lf", &value);
    errorCheck(status, json, *line);
    if(status < 1) {
        fprintf(stderr, "Error: Line %zu: Invalid number\n", *line);
        exit(1);
    }

    if(errno == ERANGE) {
        if(value == 0) {
            fprintf(stderr, "Error: Line %zu: Number underflow\n", *line);
            exit(1);
        }
        if(value == HUGE_VAL || value == -HUGE_VAL) {
            fprintf(stderr, "Error: Line %zu: Number overflow\n", *line);
            exit(1);
        }
    }

    return value;
}

vector3d nextVector3d(FILE* json, size_t* line) {
    vector3d vector;

    int c = jsonGetC(json, line);
    tokenCheck(c, '[', *line);

    skipWhitespace(json, line);
    vector.x = nextNumber(json, line);

    skipWhitespace(json, line);
    c = jsonGetC(json, line);
    tokenCheck(c, ',', *line);

    skipWhitespace(json, line);
    vector.y = nextNumber(json, line);

    skipWhitespace(json, line);
    c = jsonGetC(json, line);
    tokenCheck(c, ',', *line);

    skipWhitespace(json, line);
    vector.z = nextNumber(json, line);

    skipWhitespace(json, line);
    c = jsonGetC(json, line);
    tokenCheck(c, ']', *line);

    return vector;
}

pixel nextColor(FILE* json, size_t* line) {
    double color;
    pixel pixel;

    int c = jsonGetC(json, line);
    tokenCheck(c, '[', *line);

    skipWhitespace(json, line);
    color = nextNumber(json, line);
    if(color < 0 || color > 1.0) {
        fprintf(stderr, "Error: Line %zu: Color must be between 0.0 and 1.0\n",
            *line);
        exit(1);
    }
    pixel.red = color * 255;

    skipWhitespace(json, line);
    c = jsonGetC(json, line);
    tokenCheck(c, ',', *line);

    skipWhitespace(json, line);
    color = nextNumber(json, line);
    if(color < 0 || color > 1.0) {
        fprintf(stderr, "Error: Line %zu: Color must be between 0.0 and 1.0\n",
            *line);
        exit(1);
    }
    pixel.green = color * 255;


    skipWhitespace(json, line);
    c = jsonGetC(json, line);
    tokenCheck(c, ',', *line);

    skipWhitespace(json, line);
    color = nextNumber(json, line);
    if(color < 0 || color > 1.0) {
        fprintf(stderr, "Error: Line %zu: Color must be between 0.0 and 1.0\n",
            *line);
        exit(1);
    }
    pixel.blue = color * 255;

    skipWhitespace(json, line);
    c = jsonGetC(json, line);
    tokenCheck(c, ']', *line);

    return pixel;
}
