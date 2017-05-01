#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <getopt.h>
#include <time.h>

#include "omp.h"
#include "../imagelib/imagelib.h"
#include "masker.h"

static int parallel_flag;

int main(int argc, char *argv[]) {

  int c;

  // Default values
  int niter = 10;
  char *mask_path = NULL; 
  int mask_size = 3;

  while (1) {
      static struct option long_options[] =
        {
          {"parallel", no_argument,       &parallel_flag, 1},
          {"serial",   no_argument,       &parallel_flag, 0},
          {"help",  required_argument, 0, 'h'},
          {"niter",  required_argument, 0, 'n'},
          {"mask",  required_argument, 0, 'm'},
          {"masksize",    required_argument, 0, 's'},
          {0, 0, 0, 0}
        };
      /* getopt_long stores the option index here. */
      int option_index = 0;

      c = getopt_long (argc, argv, "hn:m:s:",
                       long_options, &option_index);

      /* Detect the end of the options. */
      if (c == -1)
        break;

      switch (c) {
        case 0:
          /* If this option set a flag, do nothing else now. */
          if (long_options[option_index].flag != 0)
            break;
          printf ("option %s", long_options[option_index].name);
          if (optarg)
            printf (" with arg %s", optarg);
          printf ("\n");
          break;

        case 'h':
          puts("Usage: masker source destination");
          puts("Options:");
          puts("  -n, --niter: Number of iterations. Default: 10");
          puts("  -m, --mask: Path of the mask file.");
          puts("  -s, --masksize: Size of the default mask (blur). If a mask file is specified, this option is ignored. Default: 3");
          exit(0);

        case 'n':
          niter = atoi(optarg);
          break;

        case 'm':
          mask_path = optarg;
          break;

        case 's':
          mask_size = atoi(optarg);
          break;

        case '?':
          /* getopt_long already printed an error message. */
          break;

        default:
          abort ();
        }
    }


  char *src, *dest;
  /* Print any remaining command line arguments (not options). */
  if (optind < argc - 1) {
    src = argv[optind];
    dest = argv[optind + 1];
  } else {
    printf("You have to specify a source image and a destination.\n");
    exit(1);
  }

  // Load the mask that will be applied.
  Mask *mask = calloc(1, sizeof(Mask));
  if (mask_path) {
    printf("Loading mask file in %s\n", mask_path);
    load_mask(mask_path, mask);
  } else {
    printf("Creating default mask of size %d\n", mask_size);
    init_mask(mask, mask_size, 1);
  }

  // Load image.
  printf("Loading source image from %s\n", src);
  Image *image = calloc(1, sizeof(Image));
  load_image(src, image);

  // Temporal image for computations.
  Image *temp = calloc(1, sizeof(Image));
  init_image(temp, image->width, image->height);
 


  // Iterate 'niter' times.
  printf("Applying mask with %d iterations\n", niter);
  double start = omp_get_wtime( );
  for (int i = 0; i < niter; i++) {

    // Apply mask to image, the result will be in 'temp'.
    apply_mask(image, temp, mask);

    // Copy temp in image for the next iteration.
    cp_image(image, temp);
  }
  double end = omp_get_wtime( );  
  printf("Masking took %lf seconds!\n", end - start);

  // Write the final image.
  printf("Writing resulting image to %s\n", dest);
  write_image(dest, image);

  // Let it go, let it goooooo
  free_mask(mask);
  free_image(image);
  free_image(temp);
  return 0;
}
