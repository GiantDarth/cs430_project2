#define __USE_MINGW_ANSI_STDIO 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <errno.h>

#include "scene.h"

void errorCheck(int c, FILE* json, size_t line);
void tokenCheck(int c, char token, size_t line);
int jsonGetC(FILE* json, size_t* line);
void skipWhitespace(FILE* json, size_t* line);
char* nextString(FILE* json, size_t* line);
double nextNumber(FILE* json, size_t* line);
double* nextVector3d(FILE* json, size_t* line);

sceneObj* readScene(const char* path) {
    FILE* json = fopen(path, "r");
    if(json == NULL) {
        perror("Error: Opening input\n");
        exit(1);
    }
    size_t line = 1;
    sceneObj* objs = NULL;
    size_t objsSize = 0;

    // Ignore beginning whitespace
    skipWhitespace(json, &line);

    int c = jsonGetC(json, &line);
    tokenCheck(c, '[', line);

    skipWhitespace(json, &line);
    c = jsonGetC(json, &line);
    if(c == ']') {
        fprintf(stderr, "Warning: Line %zu: Empty file\n", line);
        return NULL;
    }

    do {
        char* key;
        if((objs = realloc(objs, ++objsSize)) == NULL) {
            fprintf(stderr, "Error: Line %zu: Memory reallocation error\n", line);
            perror("");
            exit(1);
        }

        skipWhitespace(json, &line);
        key = nextString(json, &line);
        if(strcmp(key, "type") != 0) {
            fprintf(stderr, "Error: First key must be 'type'\n");
            exit(1);
        }

        skipWhitespace(json, &line);
        c = fgetc(json);
        errorCheck(c, json, line);
        tokenCheck(c, ':', line);

        skipWhitespace(json, &line);
        objs[objsSize - 1].type = nextString(json, &line);

        while((c = jsonGetC(json, &line) == ',')) {
            key = nextString(json, &line);
            if(strcmp(objs[objsSize - 1].type, "camera") == 0) {
                if(strcmp(key, "width")) {
                    // #TODO
                }
                else if(strcmp(key, "height")) {
                    /// #TODO
                }
                else {
                    fprintf(stderr, "Error: Line %zu: Key not supported under "
                        "'camera'\n", line);
                    exit(1);
                }
            }
            else if(strcmp(objs[objsSize - 1].type, "sphere") == 0) {
                if(strcmp(key, "color")) {
                    // #TODO
                }
                else if(strcmp(key, "position")) {
                    /// #TODO
                }
                else if(strcmp(key, "radius")) {
                    // #TODO
                }
                else {
                    fprintf(stderr, "Error: Line %zu: Key not supported under "
                        "'sphere'\n", line);
                    exit(1);
                }
            }
            else if(strcmp(objs[objsSize - 1].type, "plane") == 0) {
                if(strcmp(key, "color")) {
                    // #TODO
                }
                else if(strcmp(key, "position")) {
                    /// #TODO
                }
                else if(strcmp(key, "normal")) {
                    // #TODO
                }
                else {
                    fprintf(stderr, "Error: Line %zu: Key not supported under "
                        "'plane'\n", line);
                    exit(1);
                }
            }
            else {
                fprintf(stderr, "Error: Line %zu: Unknown type %s", line,
                    objs[objsSize - 1].type);
                exit(1);
            }
        }
    }
    while(c == ',');

    return 0;
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

    do {
        c = jsonGetC(json, line);
    }
    while(isspace(c));

    if(ungetc(c, json) == EOF) {
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

double* nextVector3d(FILE* json, size_t* line) {
    const size_t SIZE = 3;
    double* vector = malloc(SIZE * sizeof(*vector));
    if(vector == NULL) {
        fprintf(stderr, "Error: Line %zu: Memory allocation error\n", *line);
        perror("");
        exit(1);
    }
    int c = jsonGetC(json, line);
    tokenCheck(c, '[', *line);

    skipWhitespace(json, line);
    for(size_t i = 0; i < SIZE; i++) {
        vector[i] = nextNumber(json, line);
        skipWhitespace(json, line);

        if(i < SIZE - 1) {
            c = jsonGetC(json, line);
            tokenCheck(c, ',', *line);
            skipWhitespace(json, line);
        }
    }

    c = jsonGetC(json, line);
    tokenCheck(c, ']', *line);

    return vector;
}
