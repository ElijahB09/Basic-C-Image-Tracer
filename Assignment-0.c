#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

/* Do not modify write_pgm() or read_pgm() */
int write_pgm(char *file, void *image, uint32_t x, uint32_t y) {
    FILE *o;

    if (!(o = fopen(file, "w"))) {
        perror(file);

        return -1;
    }

    fprintf(o, "P5\n%u %u\n255\n", x, y);

    /* Assume input data is correctly formatted. *
     * There's no way to handle it, otherwise.   */

    if (fwrite(image, 1, x * y, o) != (x * y)) {
        perror("fwrite");
        fclose(o);

        return -1;
    }

    fclose(o);

    return 0;
}

/* A better implementation of this function would read the image dimensions *
 * from the input and allocate the storage, setting x and y so that the     *
 * user can determine the size of the file at runtime.  In order to         *
 * minimize complication, I've written this version to require the user to  *
 * know the size of the image in advance.                                   */
int read_pgm(char *file, void *image, uint32_t x, uint32_t y) {
    FILE *f;
    char s[80];
    unsigned i, j;

    if (!(f = fopen(file, "r"))) {
        perror(file);

        return -1;
    }

    if (!fgets(s, 80, f) || strncmp(s, "P5", 2)) {
        fprintf(stderr, "Expected P6\n");

        return -1;
    }

    /* Eat comments */
    do {
        fgets(s, 80, f);
    } while (s[0] == '#');

    if (sscanf(s, "%u %u", &i, &j) != 2 || i != x || j != y) {
        fprintf(stderr, "Expected x and y dimensions %u %u\n", x, y);
        fclose(f);

        return -1;
    }

    /* Eat comments */
    do {
        fgets(s, 80, f);
    } while (s[0] == '#');

    if (strncmp(s, "255", 3)) {
        fprintf(stderr, "Expected 255\n");
        fclose(f);

        return -1;
    }

    if (fread(image, 1, x * y, f) != x * y) {
        perror("fread");
        fclose(f);

        return -1;
    }

    fclose(f);

    return 0;
}

int main(int argc, char *argv[]) {
    uint8_t image[1024][1024];
    uint8_t out[1024][1024];
    int8_t K_x[3][3] = {
            {-1, 0, 1},
            {-2, 0, 2},
            {-1, 0, 1}
    };
    int accumulator_x;
    int8_t K_y[3][3] = {
            {-1, -2, -1},
            {0,  0,  0},
            {1,  2,  1}
    };
    int accumulator_y;

    /* Example usage of PGM functions */
    /* This assumes that motorcycle.pgm is a pgm image of size 1024x1024 */
    /* This method seems to copy what is in motorcycle.pgm and put it into image */
    read_pgm("motorcycle.pgm", image, 1024, 1024);

    for (int r = 0; r < 1024; r++) {
        for (int c = 0; c < 1024; c++) {
            accumulator_x = 0;
            accumulator_y = 0;
            for (int j = 0; j < 3; j++) {
                for (int i = 0; i < 3; i++) {
                    accumulator_x = accumulator_x +
                            K_x[j][i] * (int) (image[r + (j - (int) ceil(3 / 2))][c + (int) (i - ceil(3 / 2))]);
                    accumulator_y = accumulator_y +
                            K_y[j][i] * (int) (image[r + (j - (int) ceil(3 / 2))][c + (int) (i - ceil(3 / 2))]);
                }
            }
            if (accumulator_x > 255) {
                accumulator_x = 255;
            } else if (accumulator_x < 0) {
                accumulator_x = 0;
            }

            if (accumulator_y > 255) {
                accumulator_y = 255;
            } else if (accumulator_y < 0) {
                accumulator_y = 0;
            }

            out[r][c] = (uint8_t) sqrt((accumulator_x * accumulator_x) + (accumulator_y * accumulator_y));
        }
    }

    /* After processing the image and storing your output in "out", write *
     * to motorcycle.edge.pgm.                                            */
    /* This method just writes from out to motorcycle.edge.pgm */
    write_pgm("motorcycle.edge.pgm", out, 1024, 1024);

    return 0;
}
