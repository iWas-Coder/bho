#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sha-256.h>


#define ARG_STDIN_BUF_SIZE 64


int main(int argc, char **argv) {
  unsigned char *arg = NULL;
  unsigned char hash[SHA256_DIGEST_SIZE];
  
  // Argument handling
  if (argc == 2) {
    // Read 'arg' from argv
    arg = malloc(strlen(argv[1]) + 1);
    if (!arg) {
      fprintf(stderr, "ERROR: Memory allocation (malloc) failed.\n");
      return 1;
    }
    strcpy((char *) arg, argv[1]);
    #ifdef DEBUG
    printf("DEBUG: Argument read from CLI (argv).\n");
    #endif
  }
  else {
    // Read 'arg' from stdin
    unsigned char buf[ARG_STDIN_BUF_SIZE];
    size_t arg_total_size = 0;
    size_t arg_chunk_size;

    while ((arg_chunk_size = fread(buf, 1, sizeof(buf), stdin)) > 0) {
      unsigned char *tmp = realloc(arg, arg_total_size + arg_chunk_size);
      if (!tmp) {
	fprintf(stderr, "ERROR: Memory allocation (malloc) failed.\n");
	if (arg) free(arg);
	return 1;
      }
      arg = tmp;
      memcpy(arg + arg_total_size, buf, arg_chunk_size);
      arg_total_size += arg_chunk_size;
    }
    #ifdef DEBUG
    printf("DEBUG: Argument read from STDIN.\n");
    #endif
  }

  // Compute hash
  if (access((char *) arg, R_OK) == 0) {
    // 'arg' is file
    FILE *fd = fopen((char *) arg, "rb");
    if (!fd) {
      fprintf(stderr, "ERROR: File '%s' could not be read.\n", arg);
      free(arg);
      return 1;
    }
    #ifdef DEBUG
    printf("DEBUG: Treating '%s' as file.\n", arg);
    #endif
    sha256_file_compute(fd, hash);
    fclose(fd);
  }
  else {
    // 'arg' is text
    #ifdef DEBUG
    printf("DEBUG: Treating '%s' as text.\n", arg);
    #endif
    sha256_compute(arg, hash);
  }

  // Print hash to stdout
  sha256_print(hash);

  free(arg);
  return 0;
}
