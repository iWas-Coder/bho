#include <stdio.h>
#include <string.h>
#include <sha-256.h>


int main(int argc, char **argv) {
  if (argc != 2) {
    printf("Usage: ./bho <string>");
    return 1;
  }

  unsigned char text[strlen(argv[1]) + 1];
  strcpy((char*) text, argv[1]);
  unsigned char hash[SHA256_BLOCK_SIZE];

  sha256_compute(text, hash);
  for (int i = 0; i < SHA256_BLOCK_SIZE; ++i) {
    printf("%02x", hash[i]);
  }
  printf("\n");

  return 0;
}
