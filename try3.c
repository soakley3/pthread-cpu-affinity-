#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
 
void *thread_func(void *arg) {
  printf("Thread running...\n");
  system("dd if=/dev/urandom of=/dev/null bs=1M");
  return NULL;
}
 
int main() {
  pthread_t tid[4];
  int i;
 
  printf("Main thread started\n");
 
  // Create multiple threads
  for (i = 0; i < 4; i++) {
    
    if (pthread_create(&tid[i], NULL, thread_func, NULL) != 0) {
      printf("Error creating thread %d\n", i);
      return 1;
    }
    printf("New thread created with ID %ld\n", (long) tid[i]);
  }
 
  // Wait for all threads to complete
  for (i = 0; i < 5; i++) {
    if (pthread_join(tid[i], NULL) != 0) {
      printf("Error joining thread %d\n", i);
      return 1;
    }
    printf("Thread %d completed\n", i);
  }
 
  printf("Main thread completed\n");
 
  return 0;
}
