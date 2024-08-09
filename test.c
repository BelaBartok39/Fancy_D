#include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <dirent.h>
// #include <sys/stat.h>
// #include <unistd.h>
// #include <getopt.h>

#define justmessingaround "END"

int main(){
  int x;

  printf("Number: ");
  scanf("%d", &x);
  
  int *ptr = &x;
  ++*ptr;

  printf("Test: %d\n", x);
  printf("Address: %p\n", &x);
  printf("Test: %d\n", *ptr);
  printf("Address: %p\n", ptr);
  printf("%s\n", justmessingaround);
}
