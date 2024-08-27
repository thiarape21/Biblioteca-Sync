#include "sync.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

// Global variables
pthread_mutex_t mutex;
rwlock rwlock_m;
semaphore sem_m;
barrier_t barrier_m;
int count = 0;
int odds = 0;

// Auxiliar functions
void *addOdds(void *args) {
  int num = *(int *)args;
  if(num % 2 == 1) {
    pthread_mutex_lock(&mutex);
    printf("Number %d is odd\n", num);
    odds++;
    pthread_mutex_unlock(&mutex);
  }
}

void *writers(void *args) {
  int *num = (int *)args;
  write_lock(&rwlock_m);
  *num += 5;
  printf("**Writer is writting**\n");
  write_unlock(&rwlock_m);
  return NULL;
}

void *readers(void *args) {
  read_lock(&rwlock_m);
  int *num = (int *)args;
  printf("Reading number: %d\n", *num);
  read_unlock(&rwlock_m);
  return NULL;
}

void *barrier_task(void *args) {
  int thread_num = *(int *)args;
  printf("Thread that reached the barrier: %d\n", thread_num);
  int is_leader = barrier_wait(&barrier_m);
  if (is_leader) {
    printf("Lead Thread: %d\n", thread_num);
  }
  printf("Thread that passed the barrier: %d\n", thread_num);
  return NULL;
}

// Test functions
void mutex_test() {
  int pt_num = 15;
  pthread_t threads[pt_num];

  for (int i = 0; i < pt_num; i++) {
    int *n = malloc(sizeof(int));
    *n = i + 1;
    pthread_create(&threads[i], NULL, addOdds, n);
  }

  for (int i = 0; i < pt_num; i++) {
    pthread_join(threads[i], NULL);
  }

  printf("Odds: %d\n", odds);
}

int rwlock_test() {
  rwlock_init(&rwlock_m);
  int actual_num = 0;
  int ptnum = 15;

  pthread_t hilos[ptnum];
  for (int i = 0; i < ptnum; i++) {
    if (i % 4 == 0) {
      pthread_create(&hilos[i], NULL, writers, &actual_num);
    } else {
      pthread_create(&hilos[i], NULL, readers, &actual_num);
    }
  }

  for (int i = 0; i < ptnum; i++) {
    pthread_join(hilos[i], NULL);
  }

  printf("End value: %d\n", actual_num);

  return 0;
}

void *count_multiplies(void *args) {
  int num = *(int *)args;
  if (num % 5 == 0) {
    printf("Thread %ld waiting for the semaphore...\n", pthread_self());
    semaphore_down(&sem_m);
    printf("Thread %ld has entered the critical section.\n", pthread_self());
    count++;
    printf("Thread %ld incremented count to %d\n", pthread_self(), count);
    printf("Thread %ld leaving the critical section.\n", pthread_self());
    semaphore_up(&sem_m);
  }

  return NULL;
}

// Función de prueba del semáforo
int semaphore_test() {
  semaphore_init(&sem_m, 4);
  int n = 15;
  int num_threads = 16;
  pthread_t hilos[num_threads];
  for (int i = 0; i < num_threads; i++) {
    int *num = malloc(sizeof(int));
    *num = i;
    pthread_create(&hilos[i], NULL, count_multiplies, num);
  }

  for (int i = 0; i < num_threads; i++) {
    pthread_join(hilos[i], NULL);
  }
  printf("Number of multiplies of 5 up to 15: %d\n", count);
  return 0;
}

int barrier_test() {
  int num_threads = 6;
  pthread_t threads[num_threads];
  int threads_ids[num_threads];
  barrier_init(&barrier_m, num_threads);

  for (int i = 0; i < num_threads; i++) {
    threads_ids[i] = i;
    pthread_create(&threads[i], NULL, barrier_task, &threads_ids[i]);
  }
  for (int i = 0; i < num_threads; i++) {
    pthread_join(threads[i], NULL);
  }
  barrier_destroy(&barrier_m);
  return 0;
}

int main(void) {
  printf("\n---------------------------\n");
  printf("Testing Mutex\n");
  printf("Description: Determines which and how many numbers are odd between 0 and 15.\n\n");
  mutex_test();

  printf("\n---------------------------\n");
  printf("Testing Semaphore\n");
  printf(
      "Description: The number of multiplies of 5 up to 15 is calculated.\n\n");
  semaphore_test();

  printf("\n---------------------------\n");
  printf("Testing rwlock\n");
  printf("Description: For each number divisible by 4 between 1 and 15, a "
         "writer will increment the shared variable\n\n");
  rwlock_test();

  printf("\n---------------------------\n");
  printf("Testing Barrier\n");
  printf("Description: Synchronizing threads to reach a common point\n\n");
  barrier_test();
}
