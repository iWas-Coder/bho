#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sha-256.h>


#define ARG_STDIN_BUF_SIZE 64


unsigned char *file_to_hex(unsigned char *filename) {
  FILE *fd = fopen((char *) filename, "rb");
  if (!fd) return NULL;

  fseek(fd, 0, SEEK_END);
  size_t len = ftell(fd);
  fseek(fd, 0, SEEK_SET);

  unsigned char *file_contents = malloc((len * 2) + 1);
  if (!file_contents) {
    fclose(fd);
    return NULL;
  }

  unsigned char buf;
  size_t i = 0;
  while (fread(&buf, sizeof(buf), 1, fd) == 1) {
    sprintf((char *) &file_contents[i], "%02x", buf);
    i += 2;
  }
  file_contents[i] = '\0';
  
  fclose(fd);
  return file_contents;
}


int main(int argc, char **argv) {
  unsigned char *arg = NULL;
  unsigned char hash[SHA256_BLOCK_SIZE];
  
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
    unsigned char *file_contents = file_to_hex(arg);
    if (!file_contents) {
      fprintf(stderr, "ERROR: File '%s' could not be read.\n", arg);
      return 1;
    }
    #ifdef DEBUG
    printf("DEBUG: Treating '%s' as file.\n", arg);
    #endif
    sha256_compute(file_contents, hash);
    free(file_contents);
  }
  else {
    // 'arg' is text
    #ifdef DEBUG
    printf("DEBUG: Treating '%s' as text.\n", arg);
    #endif
    sha256_compute(arg, hash);
  }

  // Output hash
  for (int i = 0; i < SHA256_BLOCK_SIZE; ++i) {
    printf("%02x", hash[i]);
  }
  printf("\n");

  free(arg);
  return 0;
}
