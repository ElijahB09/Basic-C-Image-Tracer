#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

/* Do not modify write_pgm() or read_pgm() */
int write_pgm(char *file, void *image, uint32_t x, uint32_t y)
{
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
int read_pgm(char *file, void *image, uint32_t x, uint32_t y)
{
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

int convolution(void *out, void *image, void *K, uint32_t out_x, uint32_t out_y, uint32_t K_x, uint32_t K_y) {
    int accumulator;
    for(int r = 0; r < out_x; r++) {
        for(int c = 0; c < out_y; c++) {
            accumulator=0;
            for(int i = 0; i < K_x; i++) {
                for(int j = 0; j < K_y; j++) {
                    accumulator = accumulator + K[j][i] * image[r + (j - (int) ceil(3/2))][c + (int) (i - ceil(3/2))];
                }
            }
            out[r][c] = accumulator;
        }
    }
    return 1;
}

int main(int argc, char *argv[])
{
  int8_t image[1024][1024];
  int8_t out[1024][1024];
  int8_t out_x[1024][1024];
  int8_t out_y[1024][1024];
  
  /* Example usage of PGM functions */
  /* This assumes that motorcycle.pgm is a pgm image of size 1024x1024 */
  /* This method seems to copy what is in motorcycle.pgm and put it into image */
  read_pgm("motorcycle.pgm", image, 1024, 1024);

  int accumulator;
  int8_t K_x[3][3] = {
          {},
          {},
          {}
  };

  convolution(out_x, image, K_x, 1024, 1024, 3, 3);

  int8_t K_y[3][3] = {
          {},
          {},
          {}
  };
  convolution(out_y, image, K_y, 1024, 1024, 3, 3);

  /* After processing the image and storing your output in "out", write *
   * to motorcycle.edge.pgm.                                            */
    /* This method just writes from out to motorcycle.edge.pgm */
  write_pgm("motorcycle.edge.pgm", out, 1024, 1024);
  
  return 0;
}
