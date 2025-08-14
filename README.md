# Philosophers - 42 Madrid

## Índice

- [1. Introducción](#1-introducción)
  - [1.1 Conceptos generales](#11-conceptos-generales)
    - [Hilos](#hilos)
    - [Concurrencia](#concurrencia)
    - [Paralelismo](#paralelismo)
    - [¿Cómo se crea un hilo para cada filósofo?](#cómo-se-crea-un-hilo-para-cada-filósofo)
    - [Mutex](#mutex)
  - [1.2 Esquema de funciones](#12-esquema-de-funciones)
    - [1.2.1. Función main (Punto de entrada)](#121-función-main-punto-de-entrada)
    - [1.2.2 Validación e inicialización](#122-validación-e-inicialización)
      - [validate_args(&data, argv, argc)](#validate_argsdata-argv-argc)
      - [parse_input(data, argv, argc)](#parse_inputdata-argv-argc)
      - [init_data(data, argv, argc)](#init_datadata-argv-argc)
    - [1.2.3. Creación de hilos](#123-creación-de-hilos)
      - [create_philosophers(data)](#create_philosophersdata)
    - [1.2.4. Rutina de los filósofos](#124-rutina-de-los-filósofos)
      - [philo_routine(void *arg)](#philo_routinevoid-arg)
      - [Funciones auxiliares de philo_routine](#funciones-auxiliares-de-philo_routine)
        - [take_forks(philo)](#take_forksphilo)
        - [eat(philo)](#eatphilo)
        - [drop_forks(philo)](#drop_forksphilo)
        - [sleep_and_think(philo)](#sleep_and_thinkphilo)
    - [1.2.5. Hilo monitor](#125-hilo-monitor)
      - [monitor_routine(void *arg)](#monitor_routinevoid-arg)
      - [check_philosopher_death(data)](#check_philosopher_deathdata)
      - [check_all_philosophers_fed(data)](#check_all_philosophers_feddata)
    - [1.2.6. Funciones de impresión y utilidad](#126-funciones-de-impresión-y-utilidad)
      - [print_status(philo, status_msg)](#print_statusphilo-status_msg)
      - [print_death(philo) - NUEVA FUNCIÓN](#print_deathphilo---nueva-función)
      - [get_current_time_ms()](#get_current_time_ms)
      - [ft_usleep(time_ms) - FUNCIÓN OPTIMIZADA](#ft_usleeptime_ms---función-optimizada)
      - [is_game_over(data) y set_game_over(data)](#is_game_overdata-y-set_game_overdata)
      - [clean_up(data)](#clean_updata)
    - [1.2.7. Funciones auxiliares](#127-funciones-auxiliares)
      - [ft_atoi(str) y ft_atoll(str)](#ft_atoistr-y-ft_atollstr)
      - [ft_calloc(nmemb, size) - NO UTILIZADA EN ESTA VERSIÓN](#ft_callocnmemb-size---no-utilizada-en-esta-versión)
    - [1.2.8. Flujo completo de ejecución](#128-flujo-completo-de-ejecución)
    - [1.2.9. Sincronización y mutexes](#129-sincronización-y-mutexes)
    - [1.2.10. Mejoras implementadas en esta versión](#1210-mejoras-implementadas-en-esta-versión)
- [2. Defensa de la evaluación](#2-defensa-de-la-evaluación)
  - [2.1 Verifica que hay un hilo por filósofo](#21-verifica-que-hay-un-hilo-por-filósofo)
  - [2.2 Solo un tenedor por filósofo](#22-solo-un-tenedor-por-filósofo)
  - [2.3 Mutex por tenedor para verificar/cambiar valor](#23-mutex-por-tenedor-para-verificarcambiar-valor)
  - [2.4 Las salidas nunca se mezclan](#24-las-salidas-nunca-se-mezclan)
  - [2.5 Verificación de muerte y prevención de condiciones de carrera](#25-verificación-de-muerte-y-prevención-de-condiciones-de-carrera)
- [3. Asignación de tenedores](#3-asignación-de-tenedores)
- [4. Preguntas/dudas surgidas](#4-preguntasdudas-surgidas)
  - [4.1 No imprime mensaje de muerte](#41-no-imprime-mensaje-de-muerte)
  - [4.2 ¿Dónde se calcula/actualiza el tiempo de última comida?](#42-dónde-se-calculaactualiza-el-tiempo-de-última-comida)
  - [4.3 ¿Qué desbloquea pthread_mutex_unlock(&data->philosophers[i].meal_lock)?](#43-qué-desbloquea-pthread_mutex_unlockdata-philosophersimeal_lock)
  - [4.4 ¿Se para el juego antes de imprimir la muerte?](#44-se-para-el-juego-antes-de-imprimir-la-muerte)
  - [4.5 ¿Por qué es necesario el mutex de meal_lock en la función monitor?](#45-por-qué-es-necesario-el-mutex-de-meal_lock-en-la-función-monitor)
  - [4.6 Funciones fin de juego (is_game_over y set_game_over)](#46-funciones-fin-de-juego-is_game_over-y-set_game_over)
  - [4.7 Explicación al bucle de fin de juego](#47-explicación-al-bucle-de-fin-de-juego)
  - [4.8 Función pthread_mutex_lock en take_forks, check_philosopher_death, print_status y print_death](#48-función-pthread_mutex_lock-en-take_forks-check_philosopher_death-print_status-y-print_death)
  - [4.9 ¿Por qué es necesaria la función pthread_join, qué hace exactamente?](#49-por-qué-es-necesaria-la-función-pthread_join-qué-hace-exactamente)
- [5. Plan de Testing para Proyecto de Filósofos](#5-plan-de-testing-para-proyecto-de-filósofos)
  - [Restricciones de Testing](#restricciones-de-testing)
  - [Casos de Prueba](#casos-de-prueba)
    - [1. CASOS SIN MUERTE (Supervivencia)](#1-casos-sin-muerte-supervivencia)
    - [2. CASOS CON MUERTE (Testing de precisión)](#2-casos-con-muerte-testing-de-precisión)
    - [3. CASOS ESPECIALES](#3-casos-especiales)
    - [4. CASOS DE ESTRÉS](#4-casos-de-estrés)

---

## 1. Introducción

### 1.1 Conceptos generales

**Filósofos N:** 0 1 2 3 4 ...  
El contador empieza en 0 por lo que en los mensajes de salida se pondrá i + 1. N = T

**Tenedores T:** 0 1 2 3 4 ...  
Los tenedores se representan con un array de mutexes con el número de tenedores igual al de filósofos: `pthread_mutex_t forks[N]`

**Contador de filósofos y tenedores:**
- F0 → T0 T1
- F1 → T1 T2
- F2 → T2 T3
- F3 → T3 T4
- F4 → T4 T0

**Fórmula de acceso a los tenedores:**
- Tenedor izquierdo: `i`
- Tenedor derecho: `(i + 1) % N` (el módulo asegura que el último filósofo tome el primer tenedor cerrando el círculo)

**Orden de obtención de tenedores:**
Una opción es que:
- Filósofo par: primero cojo izdo `i` luego cojo derecho `(i + 1) % N`
- Filósofo impar: primero derecho `(i + 1) % N` luego izdo `i`

#### Hilos

Un **hilo es una unidad de ejecución dentro de un proceso**.  
Permite que varias tareas se ejecuten "al mismo tiempo" **(en paralelo o concurrentemente)** dentro del mismo programa, compartiendo memoria y recursos.

#### Concurrencia

La **concurrencia** se refiere a la capacidad de un sistema para gestionar múltiples tareas, permitiendo que todas ellas progresen, aunque no se estén ejecutando exactamente en el mismo instante. 

Imagina a un chef en una cocina con un solo fogón. El chef puede picar verduras, poner el agua a hervir y preparar la salsa, cambiando rápidamente entre estas tareas. El chef no está haciendo todo a la vez, pero gestiona el progreso de cada tarea de manera que parece que están ocurriendo simultáneamente. 

En programación, esto se logra a menudo en sistemas con un solo núcleo de CPU, donde el sistema operativo cambia rápidamente entre hilos (lo que se conoce como context switching).

#### Paralelismo

El **paralelismo**, por otro lado, es la ejecución de múltiples tareas **literalmente al mismo tiempo.** Esto requiere un hardware con múltiples núcleos de procesamiento (o múltiples procesadores). 

Siguiendo el ejemplo del chef, el paralelismo sería tener dos o más chefs trabajando en la cocina, cada uno con su propio fogón. Pueden picar verduras y preparar la salsa de forma simultánea e independiente. 

En programación, un sistema paralelo asigna diferentes hilos a diferentes núcleos de CPU, lo que permite una verdadera ejecución en paralelo y una mejora significativa en el rendimiento, especialmente en tareas que se pueden dividir fácilmente.

En el problema de los filósofos, cada filósofo es un hilo:  
Cada uno ejecuta su rutina (pensar, comer, dormir) de forma independiente.

#### ¿Cómo se crea un hilo para cada filósofo?

La línea:
```c
if (pthread_create(&data->philosophers[i].thread_id, NULL, 
                   philo_routine, &data->philosophers[i]) != 0)
```

hace lo siguiente:

1. Crea un nuevo hilo usando la función `pthread_create`.
2. El hilo ejecutará la función `philo_routine`.
3. Se le pasa como argumento la dirección del filósofo correspondiente: `&data->philosophers[i]`.
4. El identificador del hilo se guarda en `data->philosophers[i].thread_id`.

Así, **cada filósofo tiene su propio hilo y ejecuta su rutina de manera independiente, pero todos comparten la misma estructura de datos principal (data).**

#### Mutex

Un mutex (abreviatura de mutual exclusion, exclusión mutua) en programación es un **mecanismo de sincronización utilizado para controlar el acceso a recursos compartidos** por múltiples hilos o procesos concurrentes. Su propósito principal es asegurar que solo un hilo pueda acceder a una sección crítica (una porción de código donde se manipulan datos compartidos) en un momento dado, previniendo así las condiciones de carrera y garantizando la integridad de los datos.

**Analogía del baño:**
Imagina un baño público con una sola puerta y un cerrojo. Solo una persona puede estar dentro a la vez. Cuando alguien entra, cierra el cerrojo, impidiendo que otros entren. Al salir, abre el cerrojo para el siguiente. En esta analogía:

- Las personas son los hilos (o procesos).
- El baño es el recurso compartido (por ejemplo, una variable, una base de datos, un archivo).
- El cerrojo es el mutex.
- El acto de entrar y cerrar el cerrojo es "adquirir" o "bloquear" el mutex.
- El acto de salir y abrir el cerrojo es "liberar" o "desbloquear" el mutex.

**Asignación de Tenedores con Mutexes:**

En el contexto del problema de los filósofos comensales, cada tenedor se modela como un **mutex**. Para que un filósofo pueda comer, debe adquirir (bloquear) el mutex que representa el tenedor a su izquierda y el mutex que representa el tenedor a su derecha. Una vez que ha terminado de comer, debe liberar (desbloquear) ambos mutexes.

**Para romper el interbloqueo:**

1. **Ordenamiento jerárquico de recursos:** Asignar un orden a los tenedores (por ejemplo, del 0 al 4). Los filósofos siempre intentan adquirir el tenedor con el número más bajo primero y luego el tenedor con el número más alto. El último filósofo es la excepción, ya que tiene a su izda el tenedor con orden más alto y a su dcha tiene el primer tenedor con el orden más bajo, cerrando así el círculo.

2. **Filósofo impar/par:** Los filósofos con número impar toman primero el tenedor izquierdo y luego el derecho. Los filósofos con número par lo harían en el orden inverso, toman primero el derecho y luego el izquierdo. Esto rompe la simetría que causa el interbloqueo.

**¿Por qué defino una estructura donde los tenedores son un puntero?**

Si definieras los tenedores directamente dentro de la estructura `t_philosopher` (por ejemplo, `t_fork left_fork`), cada filósofo tendría su **propia copia independiente** del tenedor izquierdo y del tenedor derecho. Esto anularía completamente la idea de que los tenedores son recursos compartidos y que necesitan ser protegidos por mutexes. Un filósofo no podría bloquear el "mismo" tenedor que otro filósofo porque cada uno tendría su propia versión.

Al usar **punteros** `t_fork *l_fork;` y `t_fork *r_fork`, cada filósofo no tiene una copia del tenedor, sino una **referencia a la ubicación de memoria de un tenedor específico** que existe en el array `program_data.forks`.

### 1.2 Esquema de funciones

**Esquema Detallado - Proyecto de los Filósofos**

**Mejoras principales implementadas:**
- Nueva función `print_death()` específica para muertes
- Prevención de deadlock mejorada usando comparación de punteros
- Sincronización optimizada en la rutina del filósofo
- Mejor manejo del caso de un solo filósofo
- Función `ft_usleep()` personalizada para mayor precisión

**Estructuras principales en el fichero philo.h:**

`t_philo`: contiene los datos individuales de cada filósofo (id, hilos, mutex de comida, punteros a los tenedores, tiempos, número de comidas, y un puntero a los datos globales `t_data`).

`t_data`: contiene los datos generales y compartidos por todos los filósofos (número de filósofos, tiempos, mutex globales(print y game), array de tenedores, array de filósofos, etc.). El array de tenedores lo definimos con una variable de tipo `pthread_mutex_t` y el array de filósofos con la **estructura t_philo**.

**¿Es necesario que t_data tenga un array de t_philo?**

No es estrictamente obligatorio, pero es la forma más práctica y clara. Así, puedes acceder fácilmente a todos los filósofos desde la estructura global y gestionar su ciclo de vida (creación, destrucción, monitoreo, etc.).

Es la forma más adecuada y habitual en este tipo de proyectos en C. Mantener un array de `t_philo` dentro de `t_data`, lo que permite:
- Acceso centralizado a todos los filósofos.
- Gestión sencilla de memoria y sincronización.
- Mejor legibilidad y organización del código.

#### 1.2.1. Función main (Punto de entrada)

```
main(argc, argv)
├── Validación de argumentos (5 o 6 parámetros)
├── validate_args() - Valida que todos sean números positivos y límites
├── init_data() - Inicializa toda la estructura de datos
├── create_philosophers() - Crea los hilos de los filósofos
├── pthread_create(monitor_thread) - Crea el hilo monitor
├── Loop: pthread_join() para cada filósofo - Espera que terminen
├── pthread_join(monitor_thread) - Espera que termine el monitor
└── clean_up() - Destruye mutexes y libera memoria
```

**Detalles de main():**
- Verifica que haya entre 5 y 6 argumentos con mensaje de uso específico
- Si falla cualquier inicialización, limpia y retorna error
- Coordina todo el programa desde el inicio hasta el final

#### 1.2.2 Validación e inicialización

##### validate_args(&data, argv, argc)

```
validate_args()
├── Recorre todos los argumentos (argv[1] a argv[argc-1])
├── Convierte cada uno con ft_atoll() y verifica que sea > 0
├── Verifica que el número de filósofos no sea > 200 (límite de seguridad)
└── Retorna 0 si todo OK, 1 si hay error con mensajes específicos
```

##### parse_input(data, argv, argc)

```
├── Parsea argumentos:
│   ├── num_philo = ft_atoi(argv[1])
│   ├── time_to_die = ft_atoll(argv[2])
│   ├── time_to_eat = ft_atoll(argv[3])
│   ├── time_to_sleep = ft_atoll(argv[4])
│   └── max_meals = ft_atoi(argv[5]) o -1 si no existe
│
├── Inicializa variables de control:
│   ├── is_game_over = 0
│   └── start_time = get_current_time_ms()
```

##### init_data(data, argv, argc)

```
init_data()
├── Asigna memoria:
│   ├── malloc() para array de filósofos
│   └── malloc() para array de tenedores (mutexes)
│   └── Verificación de memoria asignada
│
├── Inicializa mutexes de tenedores:
│   └── Loop: pthread_mutex_init() para cada tenedor con verificación
│
├── Inicializa mutexes globales:
│   ├── pthread_mutex_init(&print_mutex)
│   └── pthread_mutex_init(&game_mutex)
```

**Detalles importantes:**
- Verificación exhaustiva de inicialización de mutexes
- Manejo de errores con mensajes descriptivos
- Los tenedores son mutexes compartidos entre filósofos adyacentes

#### 1.2.3. Creación de hilos

##### create_philosophers(data)

```
└── Configura cada filósofo:
    ├── id = i + 1
    ├── last_meal_time = start_time
    ├── meals_eaten = 0
    ├── data = puntero a estructura principal
    ├── l_fork = &forks[i] (tenedor izquierdo)
    ├── r_fork = &forks[(i+1) % num_philo] (tenedor derecho)
    └── pthread_mutex_init(&meal_lock) con verificación

create_philosophers()
└── Loop para cada filósofo:
    ├── pthread_create(&philosophers[i].thread_id, NULL, philo_routine, &philosophers[i])
    └── Verificación de error con mensaje específico por filósofo
```

**Detalles importantes:**
- Cada hilo ejecuta `philo_routine()` con un puntero al filósofo específico
- Si falla la creación de algún hilo, retorna error

#### 1.2.4. Rutina de los filósofos

##### philo_routine(void *arg)

```
philo_routine(philosopher_pointer)
├── Casting: philo = (t_philo *)arg
├── Si es filósofo par: ft_usleep(philo->data->time_to_eat / 2)
│   - Desincronización inicial
│
├── CASO ESPECIAL - Un solo filósofo:
│   ├── pthread_mutex_lock(l_fork) - Toma su único tenedor
│   ├── print_status(TAKEN_FORK)
│   ├── ft_usleep(time_to_die + 1) - Espera hasta morir
│   └── pthread_mutex_unlock(l_fork) - Libera antes de terminar
│
└── CICLO PRINCIPAL - Mientras !is_game_over():
    ├── take_forks() - Toma ambos tenedores
    ├── Verificación: if (is_game_over()) { drop_forks(); break; }
    ├── eat() - Come y actualiza datos
    ├── drop_forks() - Suelta tenedores
    └── sleep_and_think() - Duerme y piensa
```

##### Funciones auxiliares de philo_routine

###### take_forks(philo)

```
take_forks()
├── Prevención de deadlock - Orden por DIRECCIÓN DE MEMORIA:
│   ├── Si l_fork < r_fork:
│   │   ├── pthread_mutex_lock(r_fork) [NOTA: orden cambiado]
│   │   ├── print_status(TAKEN_FORK)
│   │   ├── pthread_mutex_lock(l_fork)
│   │   └── print_status(TAKEN_FORK)
│   └── Si l_fork > r_fork:
│       ├── pthread_mutex_lock(l_fork)
│       ├── print_status(TAKEN_FORK)
│       ├── pthread_mutex_lock(r_fork)
│       └── print_status(TAKEN_FORK)
```

**Detalle clave:** La comparación directa de punteros `l_fork < r_fork` garantiza un orden global consistente, evitando deadlocks.

###### eat(philo)

```
eat()
├── print_status(EATING)
├── pthread_mutex_lock(&meal_lock)
├── last_meal_time = get_current_time_ms() - Actualiza tiempo última comida
├── meals_eaten++ - Incrementa contador
├── pthread_mutex_unlock(&meal_lock)
└── ft_usleep(time_to_eat) - Función personalizada de sleep
```

###### drop_forks(philo)

```
drop_forks()
├── pthread_mutex_unlock(l_fork)
└── pthread_mutex_unlock(r_fork)
```

###### sleep_and_think(philo)

```
sleep_and_think()
├── print_status(SLEEPING)
├── ft_usleep(time_to_sleep)
└── print_status(THINKING)
```

#### 1.2.5. Hilo monitor

##### monitor_routine(void *arg)

```
monitor_routine(data_pointer)
└── CICLO - Mientras !is_game_over():
    ├── check_philosopher_death() - Verifica si alguien murió
    ├── Si alguien murió: break
    ├── check_all_philosophers_fed() - Verifica si todos comieron suficiente
    ├── Si todos comieron suficiente: break
    └── ft_usleep(1) - Pausa mínima optimizada
```

##### check_philosopher_death(data)

```
check_philosopher_death()
└── Loop para cada filósofo:
    ├── current_time = get_current_time_ms()
    ├── pthread_mutex_lock(&philosophers[i].meal_lock)
    ├── last_meal = philosophers[i].last_meal_time
    ├── pthread_mutex_unlock(&philosophers[i].meal_lock)
    ├── Si (current_time - last_meal > time_to_die):
    │   ├── set_game_over()
    │   ├── print_death(&philosophers[i]) - Función específica para muerte
    │   └── return 1
    └── return 0 si nadie murió
```

##### check_all_philosophers_fed(data)

```
check_all_philosophers_fed()
├── Si max_meals == -1: return 0 (sin límite de comidas)
├── all_fed = 1
└── Loop para cada filósofo:
    ├── pthread_mutex_lock(&philosophers[i].meal_lock)
    ├── meals_eaten = philosophers[i].meals_eaten
    ├── pthread_mutex_unlock(&philosophers[i].meal_lock)
    ├── Si meals_eaten < max_meals:
    │   ├── all_fed = 0
    │   └── break
    └── Si all_fed == 1:
        ├── set_game_over()
        └── return 1
```

#### 1.2.6. Funciones de impresión y utilidad

##### print_status(philo, status_msg)

```
print_status()
├── pthread_mutex_lock(&print_mutex) - Exclusión mutua para imprimir
├── Si !is_game_over():
│   ├── current_time = get_current_time_ms() - start_time
│   └── printf("%lld %d %s\n", current_time, philo->id, status_msg)
└── pthread_mutex_unlock(&print_mutex)
```

##### print_death(philo) - NUEVA FUNCIÓN

```
print_death()
├── pthread_mutex_lock(&print_mutex) - Exclusión mutua para imprimir
├── current_time = get_current_time_ms() - start_time
├── printf("%lld %d %s\n", current_time, philo->id, DIED)
└── pthread_mutex_unlock(&print_mutex)
```

**Detalle importante:** Esta función NO verifica `is_game_over()` porque debe imprimir la muerte sin importar el estado del juego.

##### get_current_time_ms()

```
get_current_time_ms()
├── gettimeofday(&tv, NULL) - Obtiene tiempo del sistema
└── return (tv.tv_sec * 1000 + tv.tv_usec / 1000) - Convierte a milisegundos
```

##### ft_usleep(time_ms) - FUNCIÓN OPTIMIZADA

```
ft_usleep()
├── start = get_current_time_ms()
└── while ((get_current_time_ms() - start) < time_ms):
    └── usleep(100) - Micro-sleeps para mayor precisión
```

**Ventaja:** Mucho más preciso que `usleep()` estándar para tiempos largos.

##### is_game_over(data) y set_game_over(data)

```
is_game_over()
├── pthread_mutex_lock(&game_mutex)
├── result = data->is_game_over
├── pthread_mutex_unlock(&game_mutex)
└── return result

set_game_over()
├── pthread_mutex_lock(&game_mutex)
├── data->is_game_over = 1
└── pthread_mutex_unlock(&game_mutex)
```

##### clean_up(data)

```
clean_up()
├── Loop: pthread_mutex_destroy() para cada tenedor
├── Loop: pthread_mutex_destroy() para cada meal_lock
├── pthread_mutex_destroy(&print_mutex)
├── pthread_mutex_destroy(&game_mutex)
├── free(forks)
└── free(philosophers)
```

#### 1.2.7. Funciones auxiliares

##### ft_atoi(str) y ft_atoll(str)

```
ft_atoi() / ft_atoll()
├── Salta espacios y caracteres de control
├── Maneja signo (+ o -)
├── Convierte dígitos a número
└── Aplica signo y retorna resultado
```

##### ft_calloc(nmemb, size) - NO UTILIZADA EN ESTA VERSIÓN

```
ft_calloc()
├── str = malloc(nmemb * size)
├── Si malloc falla: return NULL
├── Loop: inicializa todos los bytes a 0
└── return str
```

#### 1.2.8. Flujo completo de ejecución

1. `main()` inicia
2. Valida argumentos con límites específicos
3. `init_data()` configura todo con verificaciones exhaustivas
4. `create_philosophers()` crea N hilos filósofos
5. Cada hilo filósofo ejecuta `philo_routine()` en paralelo
6. Se crea hilo monitor que ejecuta `monitor_routine()`
7. `main()` espera con `pthread_join()` a que terminen todos los hilos
8. `clean_up()` libera recursos
9. `main()` termina

#### 1.2.9. Sincronización y mutexes

**Mutexes utilizados:**
- `forks[i]`: Protegen cada tenedor (compartidos entre filósofos adyacentes)
- `meal_lock`: Protege `last_meal_time` y `meals_eaten` de cada filósofo
- `print_mutex`: Exclusión mutua para imprimir (evita mensajes mezclados)
- `game_mutex`: Protege la variable `is_game_over`

**Prevención de deadlock mejorada:**
- Los tenedores se toman en orden por **dirección de memoria** (`l_fork < r_fork`)
- Los filósofos pares esperan 1ms al inicio para desincronizar
- Verificación de `is_game_over()` antes de comer para evitar operaciones innecesarias

**Condiciones de terminación:**
1. Un filósofo muere (tiempo desde última comida > time_to_die) - **Impresión garantizada**
2. Todos los filósofos han comido al menos max_meals veces (si se especifica)

#### 1.2.10. Mejoras implementadas en esta versión

1. **Función print_death() específica**: Garantiza que siempre se imprima cuando un filósofo muere
2. **Prevención de deadlock optimizada**: Usar comparación directa de punteros es más claro y eficiente
3. **Función ft_usleep() personalizada**: Mayor precisión en los tiempos de espera
4. **Mejor manejo del caso de un solo filósofo**: El filósofo toma el tenedor y espera a morir correctamente
5. **Verificaciones exhaustivas**: Mejor manejo de errores en inicialización
6. **Sincronización optimizada**: Verificación de is_game_over() en puntos estratégicos

Este diseño evita condiciones de carrera, deadlocks y garantiza que el programa termine correctamente en ambos casos, con una impresión clara y precisa de todos los eventos.

## 2. Defensa de la evaluación

### 2.1 Verifica que hay un hilo por filósofo

**Ubicación en el código:**

```c
// En create_philosophers()
for (i = 0; i < data->num_philo; i++)
{
    if (pthread_create(&data->philosophers[i].thread_id, NULL, 
                       philo_routine, &data->philosophers[i]) != 0)
    {
        printf("Error: Failed to create thread for philosopher %d\n", i + 1);
        return (1);
    }
}
```

**Explicación:** Se crea exactamente un hilo por cada filósofo usando `pthread_create()`. Cada filósofo tiene su propio `thread_id` almacenado en la estructura `t_philo`, y todos ejecutan la misma función `philo_routine` pero con diferentes argumentos (cada uno recibe un puntero a su estructura `t_philo` específica).

### 2.2 Solo un tenedor por filósofo

**Ubicación en el código:**

```c
// En init_data()
data->forks = malloc(sizeof(pthread_mutex_t) * data->num_philo);
if (!data->forks)
    return (1);

// Inicialización de tenedores
for (i = 0; i < data->num_philo; i++)
{
    if (pthread_mutex_init(&data->forks[i], NULL) != 0)
        return (1);
}

// En create_philosophers() - asignación de tenedores
philosophers[i].l_fork = &data->forks[i];
philosophers[i].r_fork = &data->forks[(i + 1) % data->num_philo];
```

**Explicación:** Se crean exactamente `num_philo` tenedores (array `forks`). Cada filósofo tiene acceso a dos tenedores: el izquierdo (índice `i`) y el derecho (índice `(i+1) % num_philo`). El uso del módulo asegura que el último filósofo comparta el tenedor con el primero, formando un círculo.

### 2.3 Mutex por tenedor para verificar/cambiar valor

**Ubicación en el código:**

```c
// En take_forks()
void take_forks(t_philo *philo)
{
    if (philo->l_fork < philo->r_fork)
    {
        pthread_mutex_lock(philo->r_fork);
        print_status(philo, TAKEN_FORK);
        pthread_mutex_lock(philo->l_fork);
        print_status(philo, TAKEN_FORK);
    }
    else
    {
        pthread_mutex_lock(philo->l_fork);
        print_status(philo, TAKEN_FORK);
        pthread_mutex_lock(philo->r_fork);
        print_status(philo, TAKEN_FORK);
    }
}

// En drop_forks()
void drop_forks(t_philo *philo)
{
    pthread_mutex_unlock(philo->l_fork);
    pthread_mutex_unlock(philo->r_fork);
}
```

**Explicación:** Cada tenedor está protegido por su propio mutex. Cuando un filósofo quiere tomar un tenedor, debe hacer `pthread_mutex_lock()` y cuando termina de usarlo, hace `pthread_mutex_unlock()`. **El orden de toma de tenedores (por dirección de memoria) previene deadlocks**.

### 2.4 Las salidas nunca se mezclan

**Ubicación en el código:**

```c
// En print_status()
void print_status(t_philo *philo, char *status_msg)
{
    pthread_mutex_lock(&philo->data->print_mutex);
    if (!is_game_over(philo->data))
    {
        long long current_time = get_current_time_ms() - philo->data->start_time;
        printf("%lld %d %s\n", current_time, philo->id, status_msg);
    }
    pthread_mutex_unlock(&philo->data->print_mutex);
}

// En print_death()
void print_death(t_philo *philo)
{
    pthread_mutex_lock(&philo->data->print_mutex);
    long long current_time = get_current_time_ms() - philo->data->start_time;
    printf("%lld %d %s\n", current_time, philo->id, DIED);
    pthread_mutex_unlock(&philo->data->print_mutex);
}
```

**Explicación:** Todas las salidas están protegidas por el mutex `print_mutex`. Esto garantiza que solo un hilo puede imprimir a la vez, evitando que los mensajes se mezclen o aparezcan parcialmente entrelazados.

**Start_time** es el tiempo en milisegundos(ms) inicial cuando empezó la simulación, así en los mensajes que se van imprimiendo se ve la secuencia temporal. Al principio aparecerá un tiempo cercano a 0, que se va incrementando progresivamente.

### 2.5 Verificación de muerte y prevención de condiciones de carrera

**Ubicación en el código:**

Monitor de muertes:
```c
// En check_philosopher_death()
int check_philosopher_death(t_data *data)
{
    int i;
    long long current_time;
    long long last_meal;

    i = 0;
    while (i < data->num_philo)
    {
        current_time = get_current_time_ms();
        pthread_mutex_lock(&data->philosophers[i].meal_lock);
        last_meal = data->philosophers[i].last_meal_time;
        pthread_mutex_unlock(&data->philosophers[i].meal_lock);
        
        if (current_time - last_meal > data->time_to_die)
        {
            set_game_over(data);
            print_death(&data->philosophers[i]);
            return (1);
        }
        i++;
    }
    return (0);
}
```

Protección de datos críticos:
```c
// En eat()
void eat(t_philo *philo)
{
    print_status(philo, EATING);
    pthread_mutex_lock(&philo->meal_lock);
    philo->last_meal_time = get_current_time_ms();
    philo->meals_eaten++;
    pthread_mutex_unlock(&philo->meal_lock);
    ft_usleep(philo->data->time_to_eat);
}
```

**Explicación:**

- **Detección de muerte:** Un hilo monitor separado verifica constantemente si algún filósofo ha muerto comparando el tiempo actual con `last_meal_time`.

- **Prevención de condiciones de carrera:** Se usan dos mutexes críticos:
  - `meal_lock`: Protege `last_meal_time` y `meals_eaten` para cada filósofo
  - `game_mutex`: Protege la variable `is_game_over` que indica si la simulación debe terminar

- **Sincronización:** Cuando se detecta una muerte, se marca `is_game_over = 1` y todos los hilos verifican esta variable regularmente para terminar ordenadamente.

Esta implementación evita que un filósofo pueda morir y comenzar a comer simultáneamente, ya que el tiempo de última comida se actualiza de forma atómica y el monitor puede detectar la muerte inmediatamente.

## 3. Asignación de tenedores

Hay dos maneras de prevenir el **deadlock**: el orden en la toma de tenedores dependiendo si es par/impar el filósofo, y la dirección de memoria de los forks, que garantiza que el último filósofo cambie el orden, previniendo el interbloqueo.

**Estrategia 1:** (diferente orden dependiendo de si es par/impar)  
`if (philo->id % 2 == 0)`

**Estrategia 2:** (por dirección de memoria). Explicaremos esta opción en detalle  
`if (philo->l_fork < philo->r_fork)`

Aquí **NO** se está comparando IDs de filósofos, sino **direcciones de memoria** de los mutexes.

### ¿Cómo funciona?

Los tenedores son un array de mutexes: 

Ejemplo con 5 filósofos y por tanto 5 tenedores:
```
forks[0]  forks[1]  forks[2]  forks[3]  forks[4]
```

| Filósofo | l_fork     | r_fork     | ¿l_fork < r_fork? | Orden de toma |
|----------|------------|------------|-------------------|---------------|
| 1        | forks[0]   | forks[1]   | ✅ SÍ             | Izq → Der     |
| 2        | forks[1]   | forks[2]   | ✅ SÍ             | Izq → Der     |
| 3        | forks[2]   | forks[3]   | ✅ SÍ             | Izq → Der     |
| 4        | forks[3]   | forks[4]   | ✅ SÍ             | Izq → Der     |
| 5        | forks[4]   | forks[0]   | ❌ NO             | Der → Izq     |

**El filósofo 5 es especial**: su tenedor derecho (`forks[0]`) tiene menor dirección que su izquierdo (`forks[4]`), entonces toma primero el derecho.

### ¿Por qué ambas funcionan?

**El principio clave**: Evitar que **todos** los filósofos tomen tenedores en el **mismo orden**.

Si todos tomaran primero el izquierdo:
1. Todos toman su tenedor izquierdo ✅
2. Todos intentan tomar su tenedor derecho ❌ **DEADLOCK**

Al hacer que **al menos uno** tome en orden diferente, se rompe el ciclo.

### ¿Cuál es mejor?

**La estrategia (por ID) es más clara y didáctica:**
- Más fácil de entender
- Más explícita en la intención
- Funcionalmente equivalente

**La estrategia (por dirección) es más "elegante":**
- No depende de IDs específicos
- Funciona independientemente de la numeración
- Es una técnica más general en programación concurrente

## 4. Preguntas/dudas surgidas

### 4.1 No imprime mensaje de muerte

El problema está en la función `print_status()` y en cómo se maneja la detección de muerte. Cuando un filósofo muere, el `is_game_over()` se activa inmediatamente y bloquea la impresión del mensaje de muerte.

**Principales cambios realizados:**

#### 1. Nueva función print_death()
- Función específica para imprimir el mensaje de muerte
- **NO** verifica `is_game_over()` antes de imprimir
- Se llama directamente desde el monitor cuando detecta una muerte

#### 2. Función ft_usleep() mejorada
- Reemplaza `usleep()` para mayor precisión
- Usa un bucle activo con verificaciones frecuentes
- Más precisa para tiempos pequeños

#### 3. Estrategia de toma de tenedores por ID
- Cambio de estrategia: filósofos pares toman primero el derecho, impares el izquierdo
- Más clara y fácil de entender

#### 4. Mejor manejo de la terminación
- El monitor imprime la muerte antes de que otros hilos vean `is_game_over()`
- Los filósofos verifican `is_game_over()` después de tomar tenedores

#### 5. Tiempos más precisos
- Todas las esperas usan `ft_usleep()` en lugar de `usleep()`
- Mejor control de timing para detectar muertes dentro de los 10ms requeridos

**Pruebas que ahora deberían funcionar:**
- `./philo 1 800 200 200` → El filósofo debe morir y mostrar mensaje
- `./philo 4 310 200 100` → Un filósofo debe morir y mostrar mensaje
- `./philo 5 800 200 200` → Ningún filósofo debe morir
- `./philo 5 800 200 200 7` → Para cuando todos comen 7 veces

El mensaje de muerte ahora debería aparecer correctamente porque `print_death()` no está bloqueada por `is_game_over()`.

### 4.2 ¿Dónde se calcula/actualiza el tiempo de última comida?

El `last_meal_time` se **actualiza** (no se calcula) en la función `eat()`:

```c
void eat(t_philo *philo)
{
    print_status(philo, EATING);
    pthread_mutex_lock(&philo->meal_lock);
    philo->last_meal_time = get_current_time_ms();
    philo->meals_eaten++;
    pthread_mutex_unlock(&philo->meal_lock);
    ft_usleep(philo->data->time_to_eat);
}
```

**También se inicializa** al crear cada filósofo:

```c
philosophers[i].last_meal_time = data->start_time;
```

**Explicación:** Cada vez que un filósofo comienza a comer, actualiza su `last_meal_time` con el tiempo actual. Esto marca cuándo fue su última comida.

### 4.3 ¿Qué desbloquea pthread_mutex_unlock(&data->philosophers[i].meal_lock)?

Este unlock desbloquea el **mismo mutex** que se bloqueó en la línea anterior:

```c
pthread_mutex_lock(&data->philosophers[i].meal_lock);
last_meal = data->philosophers[i].last_meal_time;
pthread_mutex_unlock(&data->philosophers[i].meal_lock);
```

**¿Por qué es necesario?**

- El `meal_lock` protege tanto `last_meal_time` como `meals_eaten` de cada filósofo
- Sin este mutex, podría haber una **condición de carrera**: el monitor podría leer `last_meal_time` justo cuando el filósofo lo está actualizando, obteniendo un valor parcialmente escrito o incorrecto
- El lock/unlock garantiza que la lectura sea **atómica**

### 4.4 ¿Se para el juego antes de imprimir la muerte?

**SÍ, exactamente.** Análisis de la secuencia:

```c
if (current_time - last_meal > data->time_to_die)
{
    set_game_over(data);      // 1. Para el juego
    print_death(&data->philosophers[i]);  // 2. Imprime muerte
    return (1);               // 3. Sale del monitor
}
```

**¿Por qué este orden es importante?**

1. **set_game_over(data):** Marca `is_game_over = 1` de forma thread-safe
2. **print_death():** Imprime el mensaje de muerte
3. **return (1):** Sale del monitor

**Consecuencia:** Una vez que `is_game_over = 1`, todos los filósofos verifican esta condición en su bucle principal:

```c
while (!is_game_over(philo->data))
{
    // Rutina del filósofo
}
```

**Y también verifican antes de comer:**

```c
if (is_game_over(philo->data))
{
    drop_forks(philo);
    break;
}
```

**Flujo temporal completo:**

1. **Monitor detecta muerte** → `set_game_over(1)`
2. **Imprime mensaje de muerte**
3. **Todos los filósofos leen is_game_over = 1**
4. **Los filósofos terminan sus hilos ordenadamente**
5. **El programa termina**

Esto evita que aparezcan mensajes después de la muerte y garantiza una terminación limpia del programa.

### 4.5 ¿Por qué es necesario el mutex de meal_lock en la función monitor?

#### 4.5.1 ¿Cuándo se actualiza last_meal_time?

Según el subject, el tiempo de muerte se calcula desde el inicio de la última comida.

Si en la función `eat` se actualiza `last_meal_time` justo al bloquear el `meal_lock` y antes de empezar a comer, está correcto y cumple con el subject.

Esto asegura que el tiempo de referencia para la muerte es el momento en que el filósofo empieza a comer, no cuando termina.

#### 4.5.2 Ejemplo del monitor sin mutex

Supón que el monitor lee `last_meal_time = 1000ms` (sin mutex).

Justo después, el filósofo empieza a comer y actualiza `last_meal_time = 2000ms`.

El monitor, usando el valor antiguo (1000ms), calcula que el filósofo lleva demasiado tiempo sin comer y lo declara muerto, aunque en realidad acaba de empezar a comer.

Esto es un falso positivo: el monitor no ve el valor actualizado porque no hay sincronización (mutex).

**Por eso se usa el mutex:**

El monitor y el filósofo acceden a `last_meal_time` de forma protegida, evitando leer/escribir valores obsoletos o inconsistentes.

**Resumen:**

- El código está correcto si actualizas `last_meal_time` al inicio de la comida (cuando bloqueas el mutex).
- El ejemplo del monitor muestra por qué es necesario el mutex: sin él, el monitor puede leer un valor antiguo y declarar muerto a un filósofo que realmente está comiendo.
- El tiempo de referencia debe ser cuando el filósofo empieza a comer.

### 4.6 Funciones fin de juego (is_game_over y set_game_over)

**is_game_over:**

Actúa como booleano para indicar si el juego ha terminado:
- Bloquea el mutex de `game_mutex`
- `result = data->is_game_over`
- En la variable result se almacena el valor de `data->is_game_over`:
  - 0 = el juego continúa (valor por defecto inicializado a 0)
  - 1 = el juego ha terminado
- Desbloquea el mutex de `game_mutex`

**set_game_over:**

No retorna nada(void) y se encarga de marcar el final del juego:
- Bloquea el mutex de `game_mutex`
- Marca el final del juego: `data->is_game_over = 1`
- Desbloquea el mutex de `game_mutex`

**¿En qué funciones chequea en el bucle el fin del juego con is_game_over?**

- En `parse_input` se inicializa `data->is_game_over = 0`
- En `monitor_routine` mientras el juego no haya acabado chequea la muerte de los filósofos y que todos hayan comido
- En `philo_routine` (caso de más de 1 filósofo) mientras el juego no haya acabado toma tenedores, come, suelta tenedores y duerme y piensa. Después de coger tenedores hay otro if de chequeo de fin del juego. Si el juego ha acabado entra en la condición, suelta tenedores y sale del bucle.
- En `print_status` bloquea el mutex de impresión, luego mientras el juego no haya acabado entra en el bucle establece el tiempo que imprime como la resta del tiempo actual menos el tiempo de cuando empezó la simulación

### 4.7 Explicación al bucle de fin de juego

**Análisis del operador ! (NOT):**

```c
while (!is_game_over(data))
```

Esto significa: "mientras el juego NO haya terminado, continúa el bucle"

- `is_game_over(data)` retorna 0 (falso) cuando el juego continúa
- `!0` = `!falso` = `verdadero` → **ENTRA en el bucle**
- `is_game_over(data)` retorna 1 (verdadero) cuando el juego termina  
- `!1` = `!verdadero` = `falso` → **SALE del bucle**

**Ejemplo práctico:**

```c
// Al inicio del programa
data->is_game_over = 0;  // El juego continúa

// En el bucle principal
while (!is_game_over(data))  // while(!0) = while(true) = ENTRA
{
    // Ejecuta la rutina del filósofo
}

// Cuando alguien muere o todos comen suficiente
set_game_over(data);  // Marca data->is_game_over = 1

// En el siguiente ciclo del bucle
while (!is_game_over(data))  // while(!1) = while(false) = SALE
{
    // NO se ejecuta más
}
```

**Comparación de sintaxis:**

Estas expresiones son **equivalentes**:

```c
while (!is_game_over(data))
while (is_game_over(data) == 0)
while (is_game_over(data) != 1)
```

**¿Por qué no `while(0)`?**

- `while(0)` es una condición **constante** que nunca se ejecuta
- `while(!is_game_over(data))` es una condición **variable** que cambia durante la ejecución

### 4.8 Función pthread_mutex_lock en take_forks, check_philosopher_death, print_status y print_death

La duda con esta función es por qué a veces se mete el parámetro sin dirección de memoria y otras veces con dirección de memoria.

**Regla simple:**
- Siempre se pasa la **dirección del mutex** que quieres bloquear.
- Si tienes un **puntero a mutex** (`pthread_mutex_t *`), pasas el puntero directamente.
- Si tienes un **mutex como variable** (`pthread_mutex_t`), pasas su dirección con `&`.

**En take_forks:**

```c
pthread_mutex_lock(philo->l_fork);
```

- `philo->l_fork` es un puntero a mutex (`pthread_mutex_t *`), así que lo pasas directamente.
- No necesitas indicar el filósofo porque cada hilo ejecuta su propia rutina y tiene su propio `philo` (estructura de filósofo).
- `take_forks` se ejecuta en `philo_routine` cuando se crean los hilos y está dentro de un bucle, por lo que se ejecuta para cada filósofo, no hay que indicarle cual es.

**En check_philosopher_death:**

```c
pthread_mutex_lock(&data->philosophers[i].meal_lock);
```

- `meal_lock` es una variable de tipo `pthread_mutex_t` dentro de cada filósofo.
- Por eso necesitas pasar la dirección con `&`.

**En print_status y print_death:**

```c
pthread_mutex_lock(&philo->data->print_mutex);
```

- `print_mutex` es una variable de tipo `pthread_mutex_t` dentro de la estructura `t_data`.
- Por eso necesitas pasar la dirección con `&`.

**Resumen:**
- Siempre pasas la dirección del mutex a `pthread_mutex_lock`.
- Si tienes un puntero a mutex, lo pasas tal cual.
- Si tienes la variable mutex, pasas su dirección con `&`.
- No necesitas indicar el filósofo en `take_forks` porque el puntero ya apunta al mutex correcto.

### 4.9 ¿Por qué es necesaria la función pthread_join, qué hace exactamente?

La función `pthread_join` es fundamental cuando trabajas con hilos (threads) en C usando la biblioteca POSIX (pthread). Su propósito es esperar a que un hilo termine su ejecución antes de que el programa continúe.

**¿Qué hace exactamente?**

```c
pthread_join(thread_id, NULL);
```

Hace que el hilo que llama (en este caso, el hilo principal del programa) espere hasta que el hilo identificado por `thread_id` termine.

Si no usas `pthread_join`, el hilo principal podría terminar y liberar recursos antes de que los hilos secundarios (filósofos o monitor) hayan acabado, lo que puede causar errores, fugas de memoria o comportamientos inesperados.

```c
// En main()
for (i = 0; i < data.num_philo; i++)
    pthread_join(data.philosophers[i].thread_id, NULL);
pthread_join(monitor_thread, NULL);
```

- El bucle espera a que todos los hilos de los filósofos terminen.
- Luego espera a que termine el hilo monitor.
- Así, el programa solo limpia y termina cuando todos los hilos han acabado correctamente.

**Resumen:**  
`pthread_join` asegura que el hilo principal espere a que los hilos secundarios terminen, evitando que el programa finalice prematuramente y garantizando una correcta gestión de recursos.

## 5. Plan de Testing para Proyecto de Filósofos

### Restricciones de Testing

- **Máximo 200 filósofos**
- **Tiempos mínimos: 60ms** para time_to_die, time_to_eat, time_to_sleep
- **Precisión de muerte: <10ms** de retraso
- **Verificar**: No robo de tenedores, muerte en tiempo correcto

### Casos de Prueba

#### 1. CASOS SIN MUERTE (Supervivencia)

##### Caso 1A: Básico - Tiempo abundante
```bash
./philo 5 800 200 200
```
- **Esperado**: Ningún filósofo muere
- **Razón**: time_to_die (800ms) > time_to_eat (200ms) + time_to_sleep (200ms) = 400ms
- **Verificar**: Ejecución continua sin muertes

##### Caso 1B: Con límite de comidas
```bash
./philo 5 800 200 200 7
```
- **Esperado**: Ningún filósofo muere, simulación termina cuando todos comen 7 veces
- **Verificar**: Conteo correcto de comidas, terminación limpia

##### Caso 1C: Margen justo pero suficiente
```bash
./philo 4 410 200 200
```
- **Esperado**: Ningún filósofo muere
- **Razón**: 410ms > 400ms (margen de 10ms)
- **Verificar**: Ejecución estable sin muertes

##### Caso 1D: Muchos filósofos - estabilidad
```bash
./philo 10 600 150 150
```
- **Esperado**: Ningún filósofo muere
- **Verificar**: Manejo correcto de muchos hilos

##### Caso 1E: Tiempos mínimos seguros
```bash
./philo 3 200 60 60
```
- **Esperado**: Ningún filósofo muere
- **Razón**: 200ms > 120ms (margen de 80ms)

#### 2. CASOS CON MUERTE (Testing de precisión)

##### Caso 2A: Muerte clara - margen insuficiente
```bash
./philo 4 310 200 100
```
- **Esperado**: UN filósofo muere
- **Razón**: 310ms < 400ms (eat + sleep = 300ms + tiempo de competencia)
- **Verificar**: Muerte en ~310ms ± 10ms

##### Caso 2B: Muerte rápida - tiempos ajustados
```bash
./philo 3 150 80 60
```
- **Esperado**: UN filósofo muere
- **Razón**: 150ms < 140ms + overhead
- **Verificar**: Muerte muy precisa

##### Caso 2C: Caso límite - muerte por milisegundos
```bash
./philo 4 305 200 100
```
- **Esperado**: UN filósofo muere (muy ajustado)
- **Verificar**: Precisión extrema del monitor

##### Caso 2D: Muchos filósofos - competencia extrema
```bash
./philo 8 250 100 80
```
- **Esperado**: AL MENOS un filósofo muere

#### 3. CASOS ESPECIALES

##### Caso 3A: Un solo filósofo
```bash
./philo 1 200 100 100
./philo 1 800 200 200
```
- **Esperado**: El filósofo muere (no puede comer sin dos tenedores)
- **Verificar**: Manejo correcto del caso especial

##### Caso 3B: Dos filósofos - caso crítico
```bash
./philo 2 250 100 50
```
- **Esperado**: Ambos pueden sobrevivir alternándose
- **Verificar**: Sincronización correcta

##### Caso 3C: Dos filósofos - muerte garantizada
```bash
./philo 2 120 80 60
```
- **Esperado**: UN filósofo muere
- **Razón**: 120ms < 140ms

#### 4. CASOS DE ESTRÉS

##### Caso 4A: Máximo filósofos permitidos
```bash
./philo 200 800 200 200
```
- **Esperado**: Ejecución estable
- **Verificar**: Límite máximo

##### Caso 4B: Tiempos muy largos
```bash
./philo 5 5000 1000 1000
```
- **Esperado**: Ejecución estable y lenta
- **Verificar**: No hay overflow de tiempo

### Casos adicionales de muerte segura

```bash
# MUERTE SEGURA - márgenes insuficientes claros
./philo 4 250 200 100  # 250 < 300ms → muerte segura
./philo 3 140 80 70    # 140 < 150ms → muerte segura
./philo 2 100 80 50    # 100 < 130ms → muerte segura
```

### Temas que pueden surgir en la evaluación

**Si me ponen un argumento que sea 0:**

Por ejemplo: `./philo 5 0 200 200` no hay que gestionarlo.

**Para defenderlo:**

**Razones técnicas:**

1. **El subject NO menciona restricciones de 60ms**: Solo dice que los argumentos deben ser números positivos

2. **La hoja de evaluación es información adicional**: No forma parte de los requisitos oficiales del proyecto

3. **Matemáticamente, 0 NO es positivo**: Los números positivos son > 0, no ≥ 0

**La implementación es correcta:**

```c
if (ft_atoll(argv[i]) <= 0) // Correcto: 0 no es positivo
{
    printf("Error: Todos los argumentos deben ser números positivos\n");
    return (1);
}
```

**Opciones de mensaje (si quisieras ser más específico):**

```c
// Opción 1: Más preciso matemáticamente
"Error: Todos los argumentos deben ser números positivos (> 0)\n"

// Opción 2: Tu mensaje actual (perfectamente válido)
"Error: Todos los argumentos deben ser números positivos\n"
```

**¿Por qué no gestionar los 60ms?**

1. **Inconsistencia**: Solo aplicaría a algunos argumentos, no todos
2. **Información no oficial**: No está en el subject
3. **Complejidad innecesaria**: Tendrías que validar diferente cada argumento

**Durante la evaluación:**

- Si te preguntan por el caso `5 0 200 200`, explica que "0 no es un número positivo matemáticamente"
- Si mencionan los 60ms, di que "esa restricción no aparece en el subject, solo en la hoja de evaluación"
- Tu implementación es técnicamente correcta según los requisitos oficiales

---


