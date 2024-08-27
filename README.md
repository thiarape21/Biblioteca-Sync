# Biblioteca Sync
## Manual de Herramientas de Sincronización 

### Tabla de contenidos
- [Introducción](#introducción)
- [Herramienta de trabajo utilizada ](#herramienta)
- [Implementación General](#implementación-general)
- [Semáforo](#semáforo)
- [Read/Write Lock](#read-Write-Lock)
- [Barrera](#barrera)
- [Conclusión](#conclusión)
- [Última actualización](#ultima-actualización)
- [Créditos](#créditos)
- [Referencías](#referencias)

### Introducción 

En la programación multiproceso, la administración del acceso simultáneo a los recursos compartidos y la sincronización de la ejecución de subprocesos son cruciales para garantizar la eficiencia de sus programas. La biblioteca de sincronización que se mostrará a continuación proporciona un conjunto de herramientas esenciales para abordar estos desafíos en C mediante la biblioteca pthread.

Las aplicaciones multiproceso pueden experimentar problemas como condiciones de carrera, interbloqueos y uso ineficiente de recursos si no se implementan los mecanismos de sincronización adecuados. Esta biblioteca tiene como objetivo simplificar estos problemas proporcionando herramientas de sincronización con el fin de coordinar subprocesos de forma eficaz y segura.

Para ello se utilizarán las siguientes herramientas de sincronización: 
1. Semáforo
2. Read/Write Lock
3. Barrera

### Herramienta de trabajo utilizada 
La herramienta que se utilizó para realizar este trabajo fue Replit. Esta plataforma en línea permite una fácil creación, edición y ejecución de proyectos de programación.

### Implementación General
Se crearon dos archivos, los cuales son **main.c** y  **sync.h**, estos  contienen lo siguiente:
- **main.c**: Variables globales, funciones auxiliares, las funciones de prueba y el main.
- **sync.h**: Estructuras de datos de cada herramienta y sus respectivas funciones.

#### Semáforo

#### Read/Write Lock


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

