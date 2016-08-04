#include <stdlib.h>
#include <stdint.h>

char* toBits(int32_t value) {
  char* result = malloc(sizeof(char) * 33);
  result[32] = '\0';
  for (int i = 0; i < 32; i++) {
    result[31-i] = value & (1 << i) ? '1' : '0';
  }
  return result;
}
