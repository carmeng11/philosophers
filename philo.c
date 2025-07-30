/* ================ PHILO.H ================ */
#ifndef PHILO_H
# define PHILO_H

# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/time.h>
# include <stdbool.h>

// Definiciones de estados para logs
# define TAKEN_FORK "has taken a fork"
# define EATING "is eating"
# define SLEEPING "is sleeping"
# define THINKING "is thinking"
# define DIED "died"

typedef struct s_data t_data;

typedef struct s_philo
{
    int             id;
    pthread_t       thread_id;
    pthread_mutex_t meal_lock;      // Mutex para proteger last_meal_time y meals_eaten
    pthread_mutex_t *l_fork;        // Puntero al tenedor izquierdo
    pthread_mutex_t *r_fork;        // Puntero al tenedor derecho
    long long       last_meal_time; // Protegido por meal_lock
    int             meals_eaten;    // Protegido por meal_lock
    t_data          *data;
}   t_philo;

typedef struct s_data
{
    int             num_philo;
    long long       time_to_die;
    long long       time_to_eat;
    long long       time_to_sleep;
    int             max_meals;
    long long       start_time;
    int             is_game_over;    // Protegido por game_mutex
    pthread_mutex_t *forks;
    pthread_mutex_t print_mutex;
    pthread_mutex_t game_mutex;      // Para proteger is_game_over
    t_philo         *philosophers;
}   t_data;

// Prototipos de funciones
int         main(int argc, char **argv);
int         init_data(t_data *data, char **argv, int argc);
int         create_philosophers(t_data *data);
void        *philo_routine(void *arg);
void        *monitor_routine(void *arg);
void        print_status(t_philo *philo, char *status_msg);
void        print_death(t_philo *philo);
long long   get_current_time_ms(void);
void        clean_up(t_data *data);
int         ft_atoi(char *str);
long long   ft_atoll(char *str);
void        *ft_calloc(size_t nmemb, size_t size);
int         is_game_over(t_data *data);
void        set_game_over(t_data *data);
int         validate_args(char **argv, int argc);
void        ft_usleep(long long time_ms);

#endif

/* ================ MAIN.C ================ */
#include "philo.h"

int main(int argc, char **argv)
{
    t_data      data;
    pthread_t   monitor_thread;
    int         i;

    if (argc < 5 || argc > 6)
    {
        printf("Uso: ./philo num_philosophers time_to_die time_to_eat time_to_sleep [num_times_to_eat]\n");
        return (1);
    }

    if (validate_args(argv, argc) != 0)
        return (1);

    if (init_data(&data, argv, argc) != 0)
        return (1);

    if (create_philosophers(&data) != 0)
    {
        clean_up(&data);
        return (1);
    }

    // Crear hilo monitor
    if (pthread_create(&monitor_thread, NULL, monitor_routine, &data) != 0)
    {
        clean_up(&data);
        return (1);
    }

    // Esperar a que todos los hilos terminen
    i = 0;
    while (i < data.num_philo)
    {
        pthread_join(data.philosophers[i].thread_id, NULL);
        i++;
    }

    pthread_join(monitor_thread, NULL);
    clean_up(&data);
    return (0);
}

/* ================ INIT.C ================ */
#include "philo.h"

int validate_args(char **argv, int argc)
{
    int i = 1;
    
    while (i < argc)
    {
        if (ft_atoll(argv[i]) <= 0)
        {
            printf("Error: Todos los argumentos deben ser números positivos\n");
            return (1);
        }
        i++;
    }
    
    if (ft_atoi(argv[1]) > 200)
    {
        printf("Error: Máximo 200 filósofos\n");
        return (1);
    }
    
    return (0);
}

int init_data(t_data *data, char **argv, int argc)
{
    int i;

    // Inicializar valores básicos
    data->num_philo = ft_atoi(argv[1]);
    data->time_to_die = ft_atoll(argv[2]);
    data->time_to_eat = ft_atoll(argv[3]);
    data->time_to_sleep = ft_atoll(argv[4]);
    data->max_meals = (argc == 6) ? ft_atoi(argv[5]) : -1;
    data->is_game_over = 0;
    data->start_time = get_current_time_ms();

    // Asignar memoria
    data->philosophers = malloc(sizeof(t_philo) * data->num_philo);
    data->forks = malloc(sizeof(pthread_mutex_t) * data->num_philo);
    
    if (!data->philosophers || !data->forks)
    {
        printf("Error: No se pudo asignar memoria\n");
        return (1);
    }

    // Inicializar mutexes de tenedores
    i = 0;
    while (i < data->num_philo)
    {
        if (pthread_mutex_init(&data->forks[i], NULL) != 0)
        {
            printf("Error: No se pudo inicializar mutex de tenedor %d\n", i);
            return (1);
        }
        i++;
    }

    // Inicializar mutexes globales
    if (pthread_mutex_init(&data->print_mutex, NULL) != 0 ||
        pthread_mutex_init(&data->game_mutex, NULL) != 0)
    {
        printf("Error: No se pudo inicializar mutexes globales\n");
        return (1);
    }

    // Inicializar filósofos
    i = 0;
    while (i < data->num_philo)
    {
        data->philosophers[i].id = i + 1;
        data->philosophers[i].last_meal_time = data->start_time;
        data->philosophers[i].meals_eaten = 0;
        data->philosophers[i].data = data;
        
        // Asignar tenedores
        data->philosophers[i].l_fork = &data->forks[i];
        data->philosophers[i].r_fork = &data->forks[(i + 1) % data->num_philo];
        
        // Inicializar mutex personal
        if (pthread_mutex_init(&data->philosophers[i].meal_lock, NULL) != 0)
        {
            printf("Error: No se pudo inicializar meal_lock para filósofo %d\n", i);
            return (1);
        }
        
        i++;
    }
    
    return (0);
}

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

void clean_up(t_data *data)
{
    int i = 0;

    // Destruir mutexes de tenedores
    while (i < data->num_philo)
    {
        pthread_mutex_destroy(&data->forks[i]);
        pthread_mutex_destroy(&data->philosophers[i].meal_lock);
        i++;
    }
    
    // Destruir mutexes globales
    pthread_mutex_destroy(&data->print_mutex);
    pthread_mutex_destroy(&data->game_mutex);
    
    // Liberar memoria
    free(data->forks);
    free(data->philosophers);
}

/* ================ PHILO_ROUTINE.C ================ */
#include "philo.h"

void print_status(t_philo *philo, char *status_msg)
{
    long long current_time;

    pthread_mutex_lock(&philo->data->print_mutex);
    
    // Solo imprimir si el juego no ha terminado
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

static void take_forks(t_philo *philo)
{
    // Evitar deadlock usando el ID del filósofo (más claro que direcciones de memoria)
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

static void eat(t_philo *philo)
{
    print_status(philo, EATING);
    
    // Actualizar tiempo de última comida y contador de comidas
    pthread_mutex_lock(&philo->meal_lock);
    philo->last_meal_time = get_current_time_ms();
    philo->meals_eaten++;
    pthread_mutex_unlock(&philo->meal_lock);
    
    // Dormir por tiempo de comida
    ft_usleep(philo->data->time_to_eat);
}

static void sleep_and_think(t_philo *philo)
{
    print_status(philo, SLEEPING);
    ft_usleep(philo->data->time_to_sleep);
    print_status(philo, THINKING);
}

void *philo_routine(void *arg)
{
    t_philo *philo = (t_philo *)arg;
    
    // Los filósofos pares esperan un poco para evitar que todos tomen tenedores al mismo tiempo
    if (philo->id % 2 == 0)
        ft_usleep(1);

    // Caso especial: un solo filósofo
    if (philo->data->num_philo == 1)
    {
        print_status(philo, THINKING);
        pthread_mutex_lock(philo->l_fork);
        print_status(philo, TAKEN_FORK);
        ft_usleep(philo->data->time_to_die + 1);
        pthread_mutex_unlock(philo->l_fork);
        return (NULL);
    }

    // Ciclo principal
    while (!is_game_over(philo->data))
    {
        // 1. Pensar
        print_status(philo, THINKING);
        
        // 2. Tomar tenedores
        take_forks(philo);
        
        // Verificar si el juego terminó mientras esperaba los tenedores
        if (is_game_over(philo->data))
        {
            drop_forks(philo);
            break;
        }
        
        // 3. Comer
        eat(philo);
        
        // 4. Soltar tenedores
        drop_forks(philo);
        
        // 5. Dormir y pensar
        sleep_and_think(philo);
    }
    
    return (NULL);
}

/* ================ MONITOR.C ================ */
#include "philo.h"

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
        
        // Verificar si el filósofo ha muerto
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

static int check_all_philosophers_fed(t_data *data)
{
    int i = 0;
    int meals_eaten;
    int all_fed = 1;

    if (data->max_meals == -1)
        return (0);

    while (i < data->num_philo)
    {
        pthread_mutex_lock(&data->philosophers[i].meal_lock);
        meals_eaten = data->philosophers[i].meals_eaten;
        pthread_mutex_unlock(&data->philosophers[i].meal_lock);
        
        if (meals_eaten < data->max_meals)
        {
            all_fed = 0;
            break;
        }
        i++;
    }
    
    if (all_fed)
    {
        set_game_over(data);
        return (1);
    }
    
    return (0);
}

void *monitor_routine(void *arg)
{
    t_data *data = (t_data *)arg;

    while (!is_game_over(data))
    {
        // Verificar muertes
        if (check_philosopher_death(data))
            break;
            
        // Verificar si todos han comido suficiente
        if (check_all_philosophers_fed(data))
            break;
            
        // Pausa pequeña para no sobrecargar CPU
        ft_usleep(1);
    }
    
    return (NULL);
}

/* ================ UTILS.C ================ */
#include "philo.h"

long long get_current_time_ms(void)
{
    struct timeval tv;
    
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

void ft_usleep(long long time_ms)
{
    long long start;
    
    start = get_current_time_ms();
    while ((get_current_time_ms() - start) < time_ms)
        usleep(100);
}

int is_game_over(t_data *data)
{
    int result;
    
    pthread_mutex_lock(&data->game_mutex);
    result = data->is_game_over;
    pthread_mutex_unlock(&data->game_mutex);
    
    return (result);
}

void set_game_over(t_data *data)
{
    pthread_mutex_lock(&data->game_mutex);
    data->is_game_over = 1;
    pthread_mutex_unlock(&data->game_mutex);
}

int ft_atoi(char *str)
{
    int result = 0;
    int sign = 1;

    while (*str == ' ' || (*str >= 9 && *str <= 13))
        str++;
        
    if (*str == '-')
        sign = -1;
        
    if (*str == '-' || *str == '+')
        str++;
        
    while (*str >= '0' && *str <= '9')
    {
        result = result * 10 + *str - '0';
        str++;
    }
    
    return (sign * result);
}

long long ft_atoll(char *str)
{
    long long result = 0;
    int sign = 1;

    while (*str == ' ' || (*str >= 9 && *str <= 13))
        str++;
        
    if (*str == '-')
        sign = -1;
        
    if (*str == '-' || *str == '+')
        str++;
        
    while (*str >= '0' && *str <= '9')
    {
        result = result * 10 + *str - '0';
        str++;
    }
    
    return (sign * result);
}

void *ft_calloc(size_t nmemb, size_t size)
{
    unsigned char *str;
    size_t i = 0;

    str = malloc(nmemb * size);
    if (!str)
        return (NULL);
        
    while (i < nmemb * size)
        str[i++] = 0;
        
    return (str);
}