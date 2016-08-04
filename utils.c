#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

extern int32_t program(int32_t*);

char* toBits(int32_t value) {
  char* result = malloc(sizeof(char) * 33);
  result[32] = '\0';
  for (int i = 0; i < 32; i++) {
    result[31-i] = value & (1 << i) ? '1' : '0';
  }
  return result;
}

int main(int argc, char** argv) {
  int32_t ninputs = atoi(argv[1]);
  int32_t* inputs = malloc(sizeof(int32_t) * (argc-2));
  for (int i = 0; i < (argc-2)/ninputs; i++) {
    for (int j = 0; j < ninputs; j++)
      inputs[j] = atoi(argv[2+(i*ninputs)+j]);
    int32_t result = program(inputs);
    printf("%d ", result);
    printf("%s ", toBits(result));
  }
  return 0;
}
