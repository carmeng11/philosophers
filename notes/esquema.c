# Esquema Detallado - Proyecto de los Filósofos viene de philo_corregido, esta es al versión 2
Las partes más importantes a entender son:

La sincronización: Cada tipo de mutex tiene un propósito específico y protege diferentes recursos compartidos.
La prevención de deadlock: El truco está en el orden de toma de tenedores por dirección de memoria en take_forks().
El monitor: Es un hilo separado que constantemente verifica las dos condiciones de terminación del programa.
La rutina del filósofo: Es el ciclo infinito que cada filósofo ejecuta hasta que el juego termine.


## 1. FUNCIÓN MAIN (Punto de entrada)

```
main(argc, argv)
├── Validación de argumentos (5 o 6 parámetros)
├── validate_args() - Valida que todos sean números positivos
├── init_data() - Inicializa toda la estructura de datos
├── create_philosophers() - Crea los hilos de los filósofos
├── pthread_create(monitor_thread) - Crea el hilo monitor
├── Loop: pthread_join() para cada filósofo - Espera que terminen
├── pthread_join(monitor_thread) - Espera que termine el monitor
└── clean_up() - Libera memoria y destruye mutexes
```

**Detalles de main():**
- Verifica que haya entre 5 y 6 argumentos
- Si falla cualquier inicialización, limpia y retorna error
- Coordina todo el programa desde el inicio hasta el final

---

## 2. VALIDACIÓN E INICIALIZACIÓN

### validate_args(argv, argc)
```
validate_args()
├── Recorre todos los argumentos (argv[1] a argv[argc-1])
├── Convierte cada uno con ft_atoll() y verifica que sea > 0
├── Verifica que el número de filósofos no sea > 200
└── Retorna 0 si todo OK, 1 si hay error
```

### init_data(data, argv, argc)
```
init_data()
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
│
├── Asigna memoria:
│   ├── malloc() para array de filósofos
│   └── malloc() para array de tenedores (mutexes)
│
├── Inicializa mutexes:
│   ├── Loop: pthread_mutex_init() para cada tenedor
│   ├── pthread_mutex_init(&print_mutex)
│   └── pthread_mutex_init(&game_mutex)
│
└── Configura cada filósofo:
    ├── id = i + 1
    ├── last_meal_time = start_time
    ├── meals_eaten = 0
    ├── data = puntero a estructura principal
    ├── l_fork = &forks[i] (tenedor izquierdo)
    ├── r_fork = &forks[(i+1) % num_philo] (tenedor derecho)
    └── pthread_mutex_init(&meal_lock)
```

**Detalles importantes:**
- Los tenedores son mutexes compartidos entre filósofos adyacentes
- Cada filósofo tiene su propio `meal_lock` para proteger sus datos personales
- El tiempo se maneja en milisegundos

---

## 3. CREACIÓN DE HILOS

### create_philosophers(data)
```
create_philosophers()
└── Loop para cada filósofo:
    └── pthread_create(&philosophers[i].thread_id, NULL, philo_routine, &philosophers[i])
```

**Detalles:**
- Cada hilo ejecuta `philo_routine()` con un puntero al filósofo específico
- Si falla la creación de algún hilo, retorna error

---

## 4. RUTINA DE LOS FILÓSOFOS

### philo_routine(void *arg)
```
philo_routine(philosopher_pointer)
├── Casting: philo = (t_philo *)arg
├── Si es filósofo par: usleep(1000) - Evita rush inicial
│
├── CASO ESPECIAL - Un solo filósofo:
│   ├── print_status(THINKING)
│   ├── pthread_mutex_lock(l_fork) - Toma su único tenedor
│   ├── print_status(TAKEN_FORK)
│   ├── usleep(time_to_die + 1000) - Espera hasta morir
│   └── pthread_mutex_unlock(l_fork)
│
└── CICLO PRINCIPAL - Mientras !is_game_over():
    ├── print_status(THINKING)
    ├── take_forks() - Toma ambos tenedores
    ├── eat() - Come y actualiza datos
    ├── drop_forks() - Suelta tenedores
    ├── sleep_and_think() - Duerme y piensa
    └── usleep(100) - Pequeña pausa
```

### Funciones auxiliares de philo_routine:

#### take_forks(philo)
```
take_forks()
├── Prevención de deadlock - Orden por dirección de memoria:
│   ├── Si l_fork < r_fork:
│   │   ├── pthread_mutex_lock(l_fork)
│   │   ├── print_status(TAKEN_FORK)
│   │   ├── pthread_mutex_lock(r_fork)
│   │   └── print_status(TAKEN_FORK)
│   └── Si r_fork < l_fork:
│       ├── pthread_mutex_lock(r_fork)
│       ├── print_status(TAKEN_FORK)
│       ├── pthread_mutex_lock(l_fork)
│       └── print_status(TAKEN_FORK)
```

**Detalle clave:** El orden de toma de tenedores por dirección de memoria previene el deadlock circular.

#### eat(philo)
```
eat()
├── print_status(EATING)
├── pthread_mutex_lock(&meal_lock)
├── last_meal_time = get_current_time_ms() - Actualiza tiempo última comida
├── meals_eaten++ - Incrementa contador
├── pthread_mutex_unlock(&meal_lock)
└── usleep(time_to_eat * 1000) - Simula tiempo comiendo
```

#### drop_forks(philo)
```
drop_forks()
├── pthread_mutex_unlock(l_fork)
└── pthread_mutex_unlock(r_fork)
```

#### sleep_and_think(philo)
```
sleep_and_think()
├── print_status(SLEEPING)
├── usleep(time_to_sleep * 1000)
└── print_status(THINKING)
```

---

## 5. HILO MONITOR

### monitor_routine(void *arg)
```
monitor_routine(data_pointer)
└── CICLO - Mientras !is_game_over():
    ├── check_philosopher_death() - Verifica si alguien murió
    ├── Si alguien murió: break
    ├── check_all_philosophers_fed() - Verifica si todos comieron suficiente
    ├── Si todos comieron suficiente: break
    └── usleep(1000) - Pausa para no sobrecargar CPU
```

#### check_philosopher_death(data)
```
check_philosopher_death()
└── Loop para cada filósofo:
    ├── current_time = get_current_time_ms()
    ├── pthread_mutex_lock(&philosophers[i].meal_lock)
    ├── last_meal = philosophers[i].last_meal_time
    ├── pthread_mutex_unlock(&philosophers[i].meal_lock)
    ├── Si (current_time - last_meal > time_to_die):
    │   ├── set_game_over()
    │   ├── print_status(DIED)
    │   └── return 1
    └── return 0 si nadie murió
```

#### check_all_philosophers_fed(data)
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

---

## 6. FUNCIONES DE UTILIDAD

### print_status(philo, status_msg)
```
print_status()
├── pthread_mutex_lock(&print_mutex) - Exclusión mutua para imprimir
├── Si !is_game_over():
│   ├── current_time = get_current_time_ms() - start_time
│   └── printf("%lld %d %s\n", current_time, philo->id, status_msg)
└── pthread_mutex_unlock(&print_mutex)
```

### get_current_time_ms()
```
get_current_time_ms()
├── gettimeofday(&tv, NULL) - Obtiene tiempo del sistema
└── return (tv.tv_sec * 1000 + tv.tv_usec / 1000) - Convierte a milisegundos
```

### is_game_over(data) y set_game_over(data)
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

### clean_up(data)
```
clean_up()
├── Loop: pthread_mutex_destroy() para cada tenedor
├── Loop: pthread_mutex_destroy() para cada meal_lock
├── pthread_mutex_destroy(&print_mutex)
├── pthread_mutex_destroy(&game_mutex)
├── free(forks)
└── free(philosophers)
```

---

## 7. FLUJO COMPLETO DE EJECUCIÓN

```
1. main() inicia
2. Valida argumentos
3. init_data() configura todo
4. create_philosophers() crea N hilos filósofos
5. Cada hilo filósofo ejecuta philo_routine() en paralelo
6. Se crea hilo monitor que ejecuta monitor_routine()
7. main() espera con pthread_join() a que terminen todos los hilos
8. clean_up() libera recursos
9. main() termina
```

## 8. SINCRONIZACIÓN Y MUTEXES

**Mutexes utilizados:**
- `forks[i]`: Protegen cada tenedor (compartidos entre filósofos adyacentes)
- `meal_lock`: Protege `last_meal_time` y `meals_eaten` de cada filósofo
- `print_mutex`: Exclusión mutua para imprimir (evita mensajes mezclados)
- `game_mutex`: Protege la variable `is_game_over`

**Prevención de deadlock:**
- Los tenedores se toman en orden por dirección de memoria
- Los filósofos pares esperan 1ms al inicio para desincronizar

**Condiciones de terminación:**
1. Un filósofo muere (tiempo desde última comida > time_to_die)
2. Todos los filósofos han comido al menos max_meals veces (si se especifica)

Este diseño evita condiciones de carrera, deadlocks y garantiza que el programa termine correctamente en ambos casos.