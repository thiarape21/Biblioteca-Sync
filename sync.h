#include <pthread.h>
#include <stdio.h>

// Semaphore
typedef struct {
  int counter;
  pthread_mutex_t mutex;
  pthread_cond_t cond;
} semaphore;

void semaphore_init(semaphore *sem, int counter) {
  sem->counter = counter;
  pthread_mutex_init(&sem->mutex, NULL);
  pthread_cond_init(&sem->cond, NULL);
}

void semaphore_down(semaphore *sem) {
  pthread_mutex_lock(&sem->mutex);
  if (sem->counter <= 0) {  
    pthread_cond_wait(&sem->cond, &sem->mutex);
  }
  sem->counter--;
  pthread_mutex_unlock(&sem->mutex);
}

void semaphore_up(semaphore *sem) {
  pthread_mutex_lock(&sem->mutex);
  sem->counter++;
  pthread_cond_signal(&sem->cond);
  pthread_mutex_unlock(&sem->mutex);
}

// Read/Write lock
typedef struct {
  int readers;
  pthread_mutex_t w_lock;
  pthread_mutex_t r_lock;

} rwlock;

void rwlock_init(rwlock *lock) {
  lock->readers = 0;
  pthread_mutex_init(&lock->w_lock, NULL);
  pthread_mutex_init(&lock->r_lock, NULL);
}

void read_lock(rwlock *lock) {
  pthread_mutex_lock(&lock->r_lock);
  lock->readers++;
  if (lock->readers == 1) {
    pthread_mutex_lock(&lock->w_lock);
  }
  pthread_mutex_unlock(&lock->r_lock);
}

void read_unlock(rwlock *lock) {
  pthread_mutex_lock(&lock->r_lock);
  lock->readers--;
  if (lock->readers == 0) {
    pthread_mutex_unlock(&lock->w_lock);
  }
  pthread_mutex_unlock(&lock->r_lock);
}

void write_lock(rwlock *lock) {
  pthread_mutex_lock(&lock->w_lock); 
}

void write_unlock(rwlock *lock) { 
  pthread_mutex_unlock(&lock->w_lock); 
}

// Barrier
typedef struct {
  int count;
  int total;
  int phase;
  pthread_mutex_t mutex;
  pthread_cond_t cond;
} barrier_t;

void barrier_init(barrier_t *barrier, int total) {
  barrier->count = 0;
  barrier->total = total;
  barrier->phase = 0;
  pthread_mutex_init(&barrier->mutex, NULL);
  pthread_cond_init(&barrier->cond, NULL);
}

int barrier_wait(barrier_t *barrier) {
  pthread_mutex_lock(&barrier->mutex);
  int phase = barrier->phase;
  barrier->count++;

  if (barrier->count == barrier->total) {
    barrier->phase++;
    barrier->count = 0;
    pthread_cond_broadcast(&barrier->cond);
    pthread_mutex_unlock(&barrier->mutex);
    return 1;
  } else {
    while (barrier->phase == phase) {
      pthread_cond_wait(&barrier->cond, &barrier->mutex);
    }
    pthread_mutex_unlock(&barrier->mutex);
    return 0;
  }
}

void barrier_destroy(barrier_t *barrier) {
  pthread_mutex_destroy(&barrier->mutex);
  pthread_cond_destroy(&barrier->cond);
}