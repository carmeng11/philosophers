
## Traducción de las preguntas:

1. **Asegúrate de que el código de philo cumple con los siguientes requisitos y pide explicaciones.**
2. **Verifica que hay un hilo por filósofo.**
3. **Verifica que hay solo un tenedor por filósofo.**
4. **Verifica si hay un mutex por tenedor y que se usa para verificar el valor del tenedor y/o cambiarlo.**
5. **Verifica que las salidas nunca se mezclan.**
6. **Verifica cómo se verifica la muerte de un filósofo y si hay un mutex para evitar que un filósofo muera 
y comience a comer al mismo tiempo.**

## Respuestas detalladas:

### 1. Cumplimiento de requisitos generales

Tu código implementa correctamente el problema de los filósofos comensales usando hilos (pthreads) y mutexes 
para sincronización. La estructura está bien organizada en múltiples archivos con responsabilidades claras.

### 2. Un hilo por filósofo

**Ubicación en el código:**
```c
// En init.c, función create_philosophers()
int create_philosophers(t_data *data)
{
    int i = 0;
    while (i < data->num_philo)
    {
        if (pthread_create(&data->philosophers[i].thread_id, NULL,
                          philo_routine, &data->philosophers[i]) != 0)
        {
            printf("Error: No se pudo crear hilo para filósofo %d\n", i);
            return (1);
        }
        i++;
    }
    return (0);
}
```

**Explicación:** Se crea exactamente un hilo por cada filósofo usando `pthread_create()`. Cada filósofo tiene 
su propio `thread_id` almacenado en la estructura `t_philo`, y todos ejecutan la misma función `philo_routine` 
pero con diferentes argumentos (cada uno recibe un puntero a su estructura `t_philo` específica).

### 3. Solo un tenedor por filósofo

**Ubicación en el código:**
```c
// En init.c, inicialización de tenedores
data->forks = malloc(sizeof(pthread_mutex_t) * data->num_philo);

// Inicialización de mutexes de tenedores
i = 0;
while (i < data->num_philo)
{
    if (pthread_mutex_init(&data->forks[i], NULL) != 0)
        return (1);
    i++;
}

// Asignación de tenedores a filósofos
data->philosophers[i].l_fork = &data->forks[i];
data->philosophers[i].r_fork = &data->forks[(i + 1) % data->num_philo];
```

**Explicación:** Se crean exactamente `num_philo` tenedores (array `forks`). Cada filósofo tiene acceso a dos tenedores: 
el izquierdo (índice `i`) y el derecho (índice `(i+1) % num_philo`). El uso del módulo asegura que el último filósofo comparta 
el tenedor con el primero, formando un círculo.

### 4. Mutex por tenedor para verificar/cambiar valor

**Ubicación en el código:**
```c
// Función take_forks() en philo_routine.c
static void take_forks(t_philo *philo)
{
    if (philo->id % 2 == 0)
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

static void drop_forks(t_philo *philo)
{
    pthread_mutex_unlock(philo->l_fork);
    pthread_mutex_unlock(philo->r_fork);
}
```

**Explicación:** Cada tenedor está protegido por su propio mutex. Cuando un filósofo quiere tomar un tenedor, debe hacer 
`pthread_mutex_lock()` y cuando termina de usarlo, hace `pthread_mutex_unlock()`. El orden de toma de tenedores (par/impar) previene deadlocks.

### 5. Las salidas nunca se mezclan

**Ubicación en el código:**
```c
// En philo_routine.c
void print_status(t_philo *philo, char *status_msg)
{
    long long current_time;

    pthread_mutex_lock(&philo->data->print_mutex);
    
    if (!is_game_over(philo->data))
    {
        current_time = get_current_time_ms() - philo->data->start_time;
        printf("%lld %d %s\n", current_time, philo->id, status_msg);
    }
    
    pthread_mutex_unlock(&philo->data->print_mutex);
}

void print_death(t_philo *philo)
{
    long long current_time;

    pthread_mutex_lock(&philo->data->print_mutex);
    current_time = get_current_time_ms() - philo->data->start_time;
    printf("%lld %d %s\n", current_time, philo->id, DIED);
    pthread_mutex_unlock(&philo->data->print_mutex);
}
```

**Explicación:** Todas las salidas están protegidas por el mutex `print_mutex`. 
Esto garantiza que solo un hilo puede imprimir a la vez, evitando que los mensajes se mezclen o aparezcan parcialmente entrelazados.

### 6. Verificación de muerte y prevención de condiciones de carrera

**Ubicación en el código:**

**Monitor de muertes:**
```c
// En monitor.c
static int check_philosopher_death(t_data *data)
{
    int i = 0;
    long long current_time;
    long long last_meal;

    while (i < data->num_philo)
    {
        current_time = get_current_time_ms();
        
        // Leer last_meal_time de forma segura
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

**Protección de datos críticos:**
```c
// Actualización segura del tiempo de última comida
pthread_mutex_lock(&philo->meal_lock);
philo->last_meal_time = get_current_time_ms();
philo->meals_eaten++;
pthread_mutex_unlock(&philo->meal_lock);

// Control de fin de juego
int is_game_over(t_data *data)
{
    int result;
    pthread_mutex_lock(&data->game_mutex);
    result = data->is_game_over;
    pthread_mutex_unlock(&data->game_mutex);
    return (result);
}
```

**Explicación:** 
- **Detección de muerte:** Un hilo monitor separado verifica constantemente si algún filósofo ha muerto comparando el 
tiempo actual con `last_meal_time`.
- **Prevención de condiciones de carrera:** Se usan dos mutexes críticos:
  - `meal_lock`: Protege `last_meal_time` y `meals_eaten` para cada filósofo
  - `game_mutex`: Protege la variable `is_game_over` que indica si la simulación debe terminar
- **Sincronización:** Cuando se detecta una muerte, se marca `is_game_over = 1` y todos los hilos verifican esta 
variable regularmente para terminar ordenadamente.

Esta implementación evita que un filósofo pueda morir y comenzar a comer simultáneamente, ya que el tiempo de 
última comida se actualiza de forma atómica 
y el monitor puede detectar la muerte inmediatamente.

ok, me puedes explicar donde calcula el tiempo de última comida de cada filósofo? last_meal = data->philosophers[i].last_meal_time;
luego tampoco entiendo bien que mutex desbloquea:
pthread_mutex_unlock(&data->philosophers[i].meal_lock);
y cuando verifica si un filósofo ha muerto, antes de la impresión se para el juego? Es lo que indica este if?
 if (current_time - last_meal > data->time_to_die) { set_game_over(data); print_death(&data->philosophers[i]); return (1); }
 EditarExcelentes preguntas. Te explico cada punto en detalle:
1. ¿Dónde se calcula/actualiza el tiempo de última comida?
El last_meal_time se actualiza (no se calcula) en la función eat() en philo_routine.c:
cstatic void eat(t_philo *philo)
{
    print_status(philo, EATING);
    
    // Actualizar tiempo de última comida y contador de comidas
    pthread_mutex_lock(&philo->meal_lock);
    philo->last_meal_time = get_current_time_ms(); // ← AQUÍ se actualiza
    philo->meals_eaten++;
    pthread_mutex_unlock(&philo->meal_lock);
    
    // Dormir por tiempo de comida
    ft_usleep(philo->data->time_to_eat);
}
También se inicializa al crear cada filósofo en init.c:
c// Inicializar filósofos
data->philosophers[i].last_meal_time = data->start_time; // ← Inicialización
Explicación: Cada vez que un filósofo comienza a comer, actualiza su last_meal_time con el tiempo actual. Esto marca cuándo fue su última comida.
2. ¿Qué mutex desbloquea pthread_mutex_unlock(&data->philosophers[i].meal_lock)?
Este unlock desbloquea el mismo mutex que se bloqueó en la línea anterior:
c// 1. BLOQUEA el mutex meal_lock del filósofo i
pthread_mutex_lock(&data->philosophers[i].meal_lock);

// 2. Lee el valor de forma segura
last_meal = data->philosophers[i].last_meal_time;

// 3. DESBLOQUEA el mismo mutex meal_lock del filósofo i
pthread_mutex_unlock(&data->philosophers[i].meal_lock);
¿Por qué es necesario?

El meal_lock protege tanto last_meal_time como meals_eaten de cada filósofo
Sin este mutex, podría haber una condición de carrera: el monitor podría leer last_meal_time justo cuando el filósofo lo está actualizando, 
obteniendo un valor parcialmente escrito o incorrecto
El lock/unlock garantiza que la lectura sea atómica

3. ¿Se para el juego antes de imprimir la muerte?
SÍ, exactamente. Analicemos la secuencia:
cif (current_time - last_meal > data->time_to_die) 
{
    set_game_over(data);        // ← 1. PRIMERO para el juego
    print_death(&data->philosophers[i]); // ← 2. DESPUÉS imprime muerte
    return (1);                 // ← 3. Sale de la función
}
¿Por qué este orden es importante?

set_game_over(data): Marca is_game_over = 1 de forma thread-safe
print_death(): Imprime el mensaje de muerte
return (1): Sale del monitor

Consecuencia: Una vez que is_game_over = 1, todos los filósofos verifican esta condición en su bucle principal:
c// En philo_routine()
while (!is_game_over(philo->data)) // ← Esta condición se vuelve false
{
    // ... ciclo de vida del filósofo
}
Y también verifican antes de comer:
c// Verificar si el juego terminó mientras esperaba los tenedores
if (is_game_over(philo->data))
{
    drop_forks(philo);
    break; // ← Sale del bucle
}
Flujo temporal completo:

Monitor detecta muerte → set_game_over(1)
Imprime mensaje de muerte
Todos los filósofos leen is_game_over = 1
Los filósofos terminan sus hilos ordenadamente
El programa termina

Esto evita que aparezcan mensajes después de la muerte y garantiza una terminación limpia del programa.