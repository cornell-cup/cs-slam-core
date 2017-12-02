#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>

void *loop1 (void *vargp) {
  printf("In loop 1\n");
  int l = 0;
  while (1) {
    int x = 0;
    for (int j = 0; j < 500; j++) {
      for(long long i = 0; i < 1000000; i++) {
        x += 1;
      }
    }
    printf("Done with loop %d\n", l++);
  }

  return NULL;
}

int main(int argc, char **argv) {
  pthread_t tid;
  pthread_create(&tid, NULL, loop1, NULL);
  while(1) {}
  // pthread_join(tid, NULL);
}