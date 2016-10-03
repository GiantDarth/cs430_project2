#define __USE_MINGW_ANSI_STDIO 1

#include "write.h"

int writeHeader(pnmHeader header, FILE* outputFd) {
    if(header.mode < 1 || header.mode > 7) {
        fprintf(stderr, "Error: Mode P%d not valid\n", header.mode);
        return -1;
    }

    if(header.mode != 3 && header.mode != 6) {
        fprintf(stderr, "Error: Mode P%d not supported\n", header.mode);
    }

    if(header.maxColorSize < CS430_PNM_MIN) {
        fprintf(stderr, "Error: Max color size must be at least %d\n",
            CS430_PNM_MIN);
        return -1;
    }

    if(header.maxColorSize > CS430_PNM_FULL_MAX) {
        fprintf(stderr, "Error: P%d only supports color size up to %d\n",
            header.mode, CS430_PNM_FULL_MAX);
        return -1;
    }

    fprintf(outputFd, "P%d\n", header.mode);

    fprintf(outputFd, "# Created with ppmrw (Christopher Philabaum <cp723@nau.edu>)\n");

    fprintf(outputFd, "%zu %zu\n", header.width, header.height);
    // If not P1 or P4, then write maxColorSize
    if(header.mode % 3 != 1) {
        fprintf(outputFd, "%zu\n", header.maxColorSize);
    }

    return 0;
}

int writeBody(pnmHeader header, pixel* pixels, FILE* outputFd) {
    if(header.mode < 1 || header.mode > 7) {
        fprintf(stderr, "Error: Mode P%d not valid\n", header.mode);
        return -1;
    }

    if(header.maxColorSize < CS430_PNM_MIN) {
        fprintf(stderr, "Error: Max color size must be at least %d\n",
            CS430_PNM_MIN);
        return -1;
    }

    // 2 byte channels are not supported in this program
    if(header.maxColorSize > 255) {
        fprintf(stderr, "Error: 2-byte colors not supported.\n");
        return -1;
    }

    // If P4 - P7, set write mode as binary.
    if(header.mode == 6) {
        // Loop through the image buffer like a grid, individually writing red,
        // green, and blue explicitly to avoid endianness errors (wrong byte order)
        for(size_t i = 0; i < header.height; i++) {
            for(size_t j = 0; j < header.width; j++) {
                fwrite(&(pixels[i * header.width + j].red), 1, 1, outputFd);
                fwrite(&(pixels[i * header.width + j].green), 1, 1, outputFd);
                fwrite(&(pixels[i * header.width + j].blue), 1, 1, outputFd);
            }
        }
    }
    else if(header.mode == 3) {
        for(size_t i = 0; i < header.height; i++) {
            for(size_t j = 0; j < header.width; j++) {
                // Write the channels as unsigned decimal values and space them
                // with single spaces
                fprintf(outputFd, "%u", pixels[i * header.width + j].red);
                fprintf(outputFd, " %u", pixels[i * header.width + j].green);
                fprintf(outputFd, " %u", pixels[i * header.width + j].blue);

                // For the end of every 5 pixels, end the line with a newline
                // to avoid lines >70 characters (based on the ppm documentation)
                if(j % 5 == 4) {
                    fprintf(outputFd, "\n");
                }
                // Otherwise, space inebtween each pixel 3 spaces
                else {
                    fprintf(outputFd, "   ");
                }
            }

            fprintf(outputFd, "\n");
        }
    }
    else {
        fprintf(stderr, "Error: Mode P%d not supported\n", header.mode);
        return -1;
    }

    return 0;
}
