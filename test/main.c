#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(void) {
  char e[] = "GETFILE FILE_NOT_FOUND 184467440737095516165\r\n\r\n";
  char* x;
  char* token;
  size_t filelen;

  token = strtok_r(e, " ", &x);
  printf("TOKEN: %s\n", token);
  token = strtok_r(NULL, " ", &x);
  printf("TOKEN: %s\n", token);
  token = strtok_r(NULL, "/r/n/r/n", &x);
  printf("TOKEN: %s\n", token);
  sscanf(token, "%zu", &filelen);
  printf("FILESIZE :%zu\n", filelen);

  char* a = malloc(sizeof(char) * 48);
  memset(a, '\0', sizeof(char) * 48);
  printf("Size of string : %zu\n", strlen(a));
  char str[10] = "GETFILE GE";
  for (int i = 0; i < 10; ++i) {
    a[i] = str[i];
  }
  printf("Size of  %s is : %zu\n", a, strlen(a));

  char b[8] = "T OK 123";
  memcpy(a+strlen(a), b, 8);

  printf("Size of  %s is : %zu\n", a, strlen(a));

  int j = 0;
  while (1) {
      printf("J: %d\n", j);
      for (int i = 0; i < 100; ++i) {
        if (i % 2 == 0) {
          printf("Even\n");
        }
        if (i == 60) {
          printf("I is 60");
          return 0;
        }
      }
  }
  return 1;
}