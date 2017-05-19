#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <math.h>
#include <getopt.h>
#include <time.h>

#include "omp.h"
#include "../imagelib/imagelib.h"
#include "masker.h"

static int verbose_flag;
static int no_output_flag;

void usage() {
  puts("Usage: masker source destination");
  puts("Options:");
  puts("  -n, --niter: Number of iterations. Default: 10");
  puts("  -m, --mask: Path of the mask file.");
  puts("  -s, --masksize: Size of the default mask (blur). If a mask file is specified, this option is ignored. Default: 3");
  puts("  -t, --threads: Number of threads.");
  puts("    , --no-output: Don't write the output image.");
}

void vlog(const char *format, ...) {
  if (verbose_flag) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
  }
    
}

int main(int argc, char *argv[]) {

  int c;

  // Default values
  int niter = 10;
  char *mask_path = NULL; 
  int mask_size = 3;
  int threads = 1;

  while (1) {
      static struct option long_options[] =
        {
          {"verbose", no_argument, &verbose_flag, 1},
          {"no-output", no_argument, &no_output_flag, 1},
          {"help",  required_argument, 0, 'h'},
          {"niter",  required_argument, 0, 'n'},
          {"mask",  required_argument, 0, 'm'},
          {"masksize",    required_argument, 0, 's'},
          {"threads",    required_argument, 0, 't'},
          {0, 0, 0, 0}
        };
      /* getopt_long stores the option index here. */
      int option_index = 0;

      c = getopt_long (argc, argv, "hn:m:s:t:",
                       long_options, &option_index);

      /* Detect the end of the options. */
      if (c == -1)
        break;

      switch (c) {
        case 0:
          /* If this option set a flag, do nothing else now. */
          if (long_options[option_index].flag != 0)
            break;
          vlog("option %s", long_options[option_index].name);
          if (optarg)
            vlog(" with arg %s", optarg);
          vlog("\n");
          break;

        case 'h':
	  usage();
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

	case 't':
	  threads = atoi(optarg);
	  break;

        case '?':
          /* getopt_long already printed an error message. */
          break;

        default:
          abort ();
        }
    }


  char *src, *dest;
  if (optind < argc - 1) {
    src = argv[optind];
    dest = argv[optind + 1];
  } else if (no_output_flag && optind < argc) {
    src = argv[optind];
  } else {
    printf("You have to specify a source image and a destination.\n");
    usage();
    exit(1);
  }

  // Load the mask that will be applied.
  Mask *mask = calloc(1, sizeof(Mask));
  if (mask_path) {
    vlog("Loading mask file in %s\n", mask_path);
    load_mask(mask_path, mask);
  } else {
    vlog("Creating default mask of size %d\n", mask_size);
    init_mask(mask, mask_size, 1);
  }

  // Load image.
  vlog("Loading source image from %s\n", src);
  Image *image = calloc(1, sizeof(Image));
  load_image(src, image);

  // Temporal image for computations.
  Image *temp = calloc(1, sizeof(Image));
  init_image(temp, image->width, image->height);
 


  // Iterate 'niter' times.
  vlog("Applying mask with %d iterations\n", niter);
  double start = omp_get_wtime( );
  for (int i = 0; i < niter; i++) {

    // Apply mask to image, the result will be in 'temp'.
    apply_mask(threads, image, temp, mask);

    // Copy temp in image for the next iteration.
    cp_image(image, temp);
  }
  double end = omp_get_wtime( );  
  vlog("Time:\n");
  printf("%lf\n", end - start);

  // Write the final image.
  if (!no_output_flag) {
    vlog("Writing resulting image to %s\n", dest);
    write_image(dest, image);
  }

  // Let it go, let it goooooo
  free_mask(mask);
  free_image(image);
  free_image(temp);
  return 0;
}
