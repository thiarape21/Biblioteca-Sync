# Biblioteca Sync
## Manual de Herramientas de Sincronización 

### Tabla de contenidos
- [Introducción](#introducción)
- [Herramienta de trabajo utilizada ](#herramienta)
- [Implementación General](#implementación-general)
- [Semáforo](#semáforo)
- [Read/Write Lock](#readwrite-lock)
- [Barrera](#barrera)
- [Conclusión](#conclusión)
- [Última actualización](#ultima-actualización)
- [Créditos](#créditos)
- [Referencias](#referencias)

### Introducción 

En la programación multiproceso, la administración del acceso simultáneo a los recursos compartidos y la sincronización de la ejecución de subprocesos son cruciales para garantizar la eficiencia de sus programas. La biblioteca de sincronización que se mostrará a continuación proporciona un conjunto de herramientas esenciales para abordar estos desafíos en C mediante la biblioteca pthread.

Las aplicaciones multiproceso pueden experimentar problemas como condiciones de carrera, interbloqueos y uso ineficiente de recursos si no se implementan los mecanismos de sincronización adecuados. Esta biblioteca tiene como objetivo simplificar estos problemas proporcionando herramientas de sincronización con el fin de coordinar subprocesos de forma eficaz y segura.

Para ello se utilizarán las siguientes herramientas de sincronización: 
1. Semáforo
2. Read/Write Lock
3. Barrera

### Mutex
El mutex es una herramienta que permite el bloqueo de exclusión mutua. Bajo este bloqueo, solamente un hilo puede retener el bloqueo y ejecutar la región crítica. 

Se presenta un ejemplo del funcionamiento del mutex, donde se cuentan los números impares en el rango entre 1 y 15. 
```c
void *addOdds(void *args) {
  int num = *(int *)args;
  if(num % 2 == 1) {
    pthread_mutex_lock(&mutex);
    printf("Number %d is odd\n", num);
    odds++;
    pthread_mutex_unlock(&mutex);
  }
}

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
```

Esta herramienta es utilizada en el proceso de implementación de los próximos algoritmos de sincronización. 

### Herramienta de trabajo utilizada 
La herramienta que se utilizó para realizar este trabajo fue Replit. Esta plataforma en línea permite una fácil creación, edición y ejecución de proyectos de programación.

### Implementación General
Se crearon dos archivos, los cuales son **main.c** y  **sync.h**, estos  contienen lo siguiente:
- **main.c**: Variables globales, funciones auxiliares, las funciones de prueba y el main.
- **sync.h**: Estructuras de datos de cada herramienta y sus respectivas funciones.

#### Semáforo

#### Read/Write Lock
Mecanismo de bloqueo que permite una lectura simultánea de los hilos, pero brinda un bloqueo a los hilos escritores. 

##### Implementación del Read/Write Lock y cómo usarlo en la biblioteca

###### 1. Estructura de datos

El archivo sync.h ofrece la estructura del Read/Write Lock: 

```c
typedef struct {
  int readers;              //Contador de hilos de lectura
  pthread_mutex_t w_lock;   //Mutex para hilos escritores
  pthread_mutex_t r_lock;   //Mutex para hilos lectores

} rwlock;
```

###### 2. Inicialización del Read/Write Lock
Se inicializan los mutex para hilos lectores y escritores, además del contador de los hilos lectores. 

```c
void rwlock_init(rwlock *lock) {
  lock->readers = 0;
  pthread_mutex_init(&lock->w_lock, NULL);
  pthread_mutex_init(&lock->r_lock, NULL);
}
```

###### 3. Bloqueo y desbloqueo del Read/Write Lock

El bloqueo de un hilo lector incrementa el contador de lectores. Además, en caso de ser el primer hilo lector, se bloquea el mutex de los hilos escritores. 
```c
void read_lock(rwlock *lock) {
  pthread_mutex_lock(&lock->r_lock);
  lock->readers++;
  if (lock->readers == 1) {
    pthread_mutex_lock(&lock->w_lock);
  }
  pthread_mutex_unlock(&lock->r_lock);
}
```

El desbloqueo de un hilo lector decrementa el contador de lectores. Igualmente, en caso de ser el último lector contado, se desbloquea el mutex de los hilos escritores. 
```c
void read_unlock(rwlock *lock) {
  pthread_mutex_lock(&lock->r_lock);
  lock->readers--;
  if (lock->readers == 0) {
    pthread_mutex_unlock(&lock->w_lock);
  }
  pthread_mutex_unlock(&lock->r_lock);
}
```

El bloqueo y desbloqueo de los hilos escritores se implementa con un mutex. 
```c
void write_lock(rwlock *lock) {
  pthread_mutex_lock(&lock->w_lock); 
}

void write_unlock(rwlock *lock) { 
  pthread_mutex_unlock(&lock->w_lock); 
}
```
###### 4. Pruebas del Read/Write Lock
Se implementan dos funciones auxiliares que los hilos ejecutan, una para escritores y otra para lectores. La funcion writers incrementa el valor de la variable compartida, mientras que la función readers obtiene e imprime el valor compartido. 

```c
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
```

La función rwlock_test crea los hilos. Por cada número divisible por 4 entre 0 y 15 se crea un hilo escritor, y por cada número que no es múltiplo de 4 se crea un hilo lector. La función indica el valor final de la variable compartida.  

```c
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
```

#### Barrera

La herramienta de sincronización  **Barrera** es un mecanismo que asegura que múltiples hilos se detengan y esperen hasta que todos lleguen a un punto específico de su ejecución, antes de que cualquiera de ellos avance.

##### Implementación de la Barrera y cómo usarla en la biblioteca 

###### 1. Estructura de datos

Se debe definir la estructura de datos que tendrá la herramienta de sincronización Barrera en el archivo sync.h:

```c
typedef struct {
    int count;              // Contador de hilos en la barrera
    int total;              // Número total de hilos que deben alcanzar la barrera
    int phase;              // Maneja las fases 
    pthread_mutex_t mutex; // Mutex para sincronizar el acceso a la barrera
    pthread_cond_t cond;   // Variable de condición para manejar la espera
    int leader;          // Identificador del hilo que liberará la barrera
} barrier_t;
```

###### 2. Inicialización de la barrera
Se inicializa la barrera definiendo el número total de hilos configurando el mutex y la condición para la sincronización en el archivo sync.h.

```c
void barrier_init (barrier_t *barrier, int total){
  barrier->count= 0;         // Se establece el contador de hilos que han llegado a la barrera
  barrier->total= total;     // Se define el número total de hilos
  barrier->phase= 0;         // Fase actual
  pthread_mutex_init(&barrier->mutex, NULL); // Inialización del mutex
  pthread_cond_init(&barrier->cond, NULL);   // Inialización de la condición para la sincronización
}
```

###### 3. Sincronización de hilos
Se sincronizan los hilos en el archivo sync.h y va sucediendo lo siguiente: 
- Los hilos van incrementando el contador cada vez que llegan a la barrera. 
- Una vez que el contador alcanza el número total de hilos, se considera que todos han llegado. 
- Se incrementa la fase, se reinicia el contador y los hilos ya podrán continuar. 
- El hilo "líder" recibe el valor de 1 para indicar que en ese momento es el "encargado", esto por si se llegara a necesitar alguna acción adicional (si fuera necesario)

```c
int barrier_wait (barrier_t *barrier){
  pthread_mutex_lock(&barrier->mutex);
  int phase= barrier->phase;
  barrier->count++;
  
  if(barrier->count == barrier->total){
    barrier->phase++;
    barrier->count= 0;
    pthread_cond_broadcast(&barrier->cond);
    pthread_mutex_unlock(&barrier->mutex);
    return 1;
  }else{
    while(barrier->phase == phase){
      pthread_cond_wait(&barrier->cond, &barrier->mutex);
    }
    pthread_mutex_unlock(&barrier->mutex);
    return 0;
  }
```

###### 4. Destrucción de la barrera
Una vez que la barrera ya no es necesaria, se liberan los recursos asociados utilizando la siguiente función en el archivo sync.h : 
```c
void barrier_destroy(barrier_t *barrier){
  pthread_mutex_destroy(&barrier->mutex);
  pthread_cond_destroy(&barrier->cond);}
```

###### 5. Declaración de la barrera 
Se declara la herramienta barrera de manera global en el archivo main.c : 
```c
// Global variables
barrier_t barrier_m;
```
###### 6. Inicialización en el código principal
Se inicializa la herramienta barrera en el archivo main.c (en este caso se mostrará como se inicializa con la prueba): 
```c
  printf("Testing Barrier\n");
  printf("Description: Synchronizing threads to reach a common point\n\n");
  barrier_test();
}
```
### Conclusión

### Última actualización 
Este README se actualizó por última vez el [26 de agosto del 2024].

### Créditos
Gracias a los siguientes colaboradores:
- Sebastián Fernández Valerio - 2021144538
- Jostin Retana Ledezma - 2021039736
- Thiara Prendas Espinoza - 2021457056

### Referencias 
- Delgado, J. M. (2024). Qué son los hilos de un procesador, diferencias con los núcleos y qué es más importante. Recuperado de: [Qué son los hilos de un procesador, diferencias con los núcleos y qué es más importante (computerhoy.com)](https://computerhoy.com/pc/hilos-procesador-diferencias-nucleos-1378276).

- Castillo, J. A. (2019). Qué son los hilos de un procesador? Diferencias con los núcleos. Recuperado de: [Qué son los hilos de un procesador ? Diferencias con los núcleos (profesionalreview.com)](https://www.profesionalreview.com/2019/04/03/que-son-los-hilos-de-un-procesador/).

- IBM. (2023-a). Utilización de bloqueos de lectura/grabación. [Documentación IBM](https://www.ibm.com/docs/es/aix/7.3?topic=programming-using-readwrite-locks) 

- IBM. (2023-b). Utilización de exclusiones mutuas. [Documentación IBM](https://www.ibm.com/docs/es/aix/7.3?topic=programming-using-mutexes)

- LinkedIn. (2023). ¿Cuál es el propósito de un semáforo en un sistema operativo? [LinkedIn](https://es.linkedin.com/advice/0/what-purpose-semaphore-operating-system-skills-computer-science-ozdaf?lang=es)

- Stevewhims. (2023). Barreras de sincronización. Recuperado de: [Barreras de sincronización - Win32 apps | Microsoft Learn](https://learn.microsoft.com/es-es/windows/win32/sync/synchronization-barriers).

