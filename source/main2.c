#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include "utils.h"

struct sharedMem {

  int a;
  int b;
};

int main(int argc, char *argv[]) {
  int id, err;
  struct sharedMem *hello;
  if (argc <= 1){printf("need the shared memory segment id\n"); exit(1);}

  sscanf(argv[1], "%d", &id);
  printf("id is %d\n", id );

  hello = (struct sharedMem *)shmat(id, (void*) 0, 0);
  if((int) hello == -1) perror("attachment fail\n");
  else printf("attached memory\n");
  printf("hello is a=%d b=%d\n", hello->a, hello->b);
  
  hello->a = 10;  
  hello->b = 10;
  printf("chaged hello to a=%d b=%d\n", hello->a, hello->b);

  err = shmdt((void*) hello);
  if (err == -1) perror("Detachment\n");
  else printf("succesfully detached\n");
  return 0;

};
