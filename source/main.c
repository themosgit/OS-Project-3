#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <stdio.h>
#include "utils.h"


int main(int argc, char *argv[]) {
  int id = 0, err = 0;
  int *mem;
  SharedMem hello;

  id = shmget(IPC_PRIVATE, sizeof(SharedMem), 0666); 
  if (id == -1) perror("Creation of shared memory failed..\n");
  else printf("share memory segment created with id %d\n", id);

  hello = (SharedMem) shmat(id, (void*)0, 0);
  if (*(int*)hello == -1) printf("error in attachment\n");
  else printf("attachment succesfull\n");

  hello.a = 1;
  hello.b = 1;
  
  printf("start process two\n"); getchar();

  printf("hello is now a=%d b=%d\n", hello->a, hello->b);
  
  err = shmctl(id, IPC_RMID, 0);
  if (err == -1) perror("error in memory detaching\n");
  else printf("memory detached succesfully %d\n", (int)(err));
  return 0;
}
