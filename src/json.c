#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void errorCheck(int c, FILE* fp) {
    if(c == EOF) {
        if(feof(fp)) {
            fprintf(stderr, "Error: Premature end-of-file\n");
            exit(1);
        }
        else if(ferror(fp)) {
            fprintf(stderr, "Error: Read error\n");
            exit(1);
        }
    }
}

char* parseString(FILE* json) {
    size_t bufferSize = 64;
    size_t oldSize = bufferSize;
    char* buffer = malloc(bufferSize);
    if(buffer == NULL) {
        perror("Error: Memory allocation error\n");
        exit(1);
    }
    int c;
    size_t i;

    c = fgetc(json);
    if(c != '"') {
        fprintf(stderr, "Error: Expected string.\n");
        exit(1);
    }

    i = 0;
    while(i < bufferSize - 1 && (c = fgetc(json)) != '"' && c != EOF) {
        if(i == bufferSize - 2) {
            bufferSize *= 2;
            // Integer overflow
            if(oldSize != 0 && bufferSize / oldSize != 2) {
                fprintf(stderr, "Error: Integer overflow on size\n");
                exit(1);
            }

            oldSize = bufferSize;
            buffer = realloc(buffer, bufferSize);
            if(buffer == NULL) {
                perror("Error: Memory allocation error\n");
                exit(1);
            }
        }
        buffer[i++] = c;
    }
    buffer[i] = '\0';

    return strdup(buffer);
}

void skipWhitespace(FILE* fp) {
    int c;

    while((c = fgetc(fp)) != EOF || isspace(c));
    errorCheck(c, fp);
    if(ungetc(c, fp) == EOF) {
        fprintf(stderr, "Error: Read error ungetting character\n");
        exit(1);
    }
}

int main(int argc, char const *argv[]) {
    FILE* json = fopen("input.json", "r");
    if(json == NULL) {
        perror("Error: Opening input\n");
        return 1;
    }
    skipWhitespace(json);

    int c = fgetc(json);
    if(c != '[') {
        fprintf(stderr, "Error: Missing '['\n");
        return 1;
    }

    skipWhitespace(json);
    c = fgetc(json);
    errorCheck(c, json);
    if(c == ']') {
        return 0;
    }

    while(c == '{') {
        char* key, *type, *value, *endptr;
        float width, height, radius;
        float color[3], pos[3], normal[3];

        key = parseString(json);
        if(strcmp(key, "type") != 0) {
            fprintf(stderr, "Error: First key must be 'type'\n");
            return 1;
        }
        skipWhitespace(json);
        c = fgetc(json);
        errorCheck(c, json);
        if(c != ':') {
            return 1;
        }
        skipWhitespace(json);
        type = parseString(json);

        if(strcmp(type, "camera") == 0) {
            // Continue while there are still key-value pairs
            while((c = fgetc(json)) != EOF && c == ',');
            errorCheck(c, json);

            key = parseString(json);
            if(strcmp(type, "width")) {
                // #TODO
            }
            else if(strcmp(type, "height")) {
                /// #TODO
            }
            else {
                fprintf(stderr, "Error: Key not supported under 'camera'\n");
            }
        }
        else if(strcmp(type, "sphere") == 0) {
            // Continue while there are still key-value pairs
            while((c = fgetc(json)) != EOF && c == ',');
            errorCheck(c, json);

            key = parseString(json);
            if(strcmp(type, "color")) {
                // #TODO
            }
            else if(strcmp(type, "position")) {
                /// #TODO
            }
            else if(strcmp(type, "radius")) {
                // #TODO
            }
            else {
                fprintf(stderr, "Error: Key not supported under 'sphere'\n");
            }
        }
        else if(strcmp(type, "plane") == 0) {
            // Continue while there are still key-value pairs
            while((c = fgetc(json)) != EOF && c == ',');
            errorCheck(c, json);

            key = parseString(json);
            if(strcmp(type, "color")) {
                // #TODO
            }
            else if(strcmp(type, "position")) {
                /// #TODO
            }
            else if(strcmp(type, "normal")) {
                // #TODO
            }
            else {
                fprintf(stderr, "Error: Key not supported under 'plane'\n");
            }
        }
    }

    return 0;
}
