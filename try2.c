#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
 
#include <errno.h>
 
#define handle_error_en(en, msg) \
        do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)
 
 
/*  
 
To start this with CPU affinity set in main(), uncomment line 99 and comment out line 100.
 
     99   if (pthread_create(&tid[i], NULL, thread_func_dont_setself, NULL) != 0) {
    100   //if (pthread_create(&tid[i], NULL, thread_func_setself, NULL) != 0) {
 
To let each thread set it's own affinity to `z` , comment out 99, and uncomment 100.
 
     99   //if (pthread_create(&tid[i], NULL, thread_func_dont_setself, NULL) != 0) {
    100   if (pthread_create(&tid[i], NULL, thread_func_setself, NULL) != 0) {
 
Then recompile and run.
 
Set `z` to a number 0 through in main(). I tried this on a quadcore.
 
*/
 
 
 
 
pthread_t tid[5];
int i, s, z;                
cpu_set_t cpuset;
pthread_t thread;
 
 
void *thread_func_setself(void *arg) {
  printf("Thread running...\n");
  int ss;
  // set affinity of single thread
  pthread_t thread2;
  cpu_set_t cpuset2;
 
  thread2 = pthread_self();
 
  printf("z = %d\n", z);
  CPU_ZERO(&cpuset2);
  CPU_SET(z, &cpuset2); //set affinity to cpu # (thread #)
 
  ss = pthread_setaffinity_np(thread2, sizeof(cpuset), &cpuset2);
  if (ss != 0)
    handle_error_en(ss, "pthread_setaffinity_np");
 
 
  ss = pthread_getaffinity_np(thread2, sizeof(cpuset), &cpuset2);
  if (ss != 0)
      handle_error_en(s, "pthread_getaffinity_np");
 
  printf("Set returned by pthread_getaffinity_np() contained:\n");
  for (int j = 0; j < CPU_SETSIZE; j++)
      if (CPU_ISSET(j, &cpuset2))
          printf("    CPU %d\n", j);
  system("dd if=/dev/urandom of=/dev/null bs=1M");
  return NULL;
}
 
 
void *thread_func_dont_setself(void *arg) {
  printf("Thread running...\n");
  printf("Set returned by pthread_getaffinity_np() contained:\n");
  for (int j = 0; j < CPU_SETSIZE; j++)
      if (CPU_ISSET(j, &cpuset))
          printf("    CPU %d\n", j);
  system("dd if=/dev/urandom of=/dev/null bs=1M");
  return NULL;
}
 
 
 
 
int main() {
  thread = pthread_self();
  z = 0; // latter thread affinity
 
 
  printf("Main thread started\n");
 
  // Create multiple threads
  for (i = 0; i < 5; i++) {
 
  if (pthread_create(&tid[i], NULL, thread_func_dont_setself, NULL) != 0) {
  //if (pthread_create(&tid[i], NULL, thread_func_setself, NULL) != 0) {
      printf("Error creating thread %d\n", i);
      return 1;
    }
    printf("New thread created with ID %ld\n", (long) tid[i]);
  }

  printf("adjusting main thread\n");

  CPU_ZERO(&cpuset);
  CPU_SET(3, &cpuset);

  s = pthread_setaffinity_np(thread, sizeof(cpuset), &cpuset);
  if (s != 0)
      handle_error_en(s, "pthread_setaffinity_np");


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
 
