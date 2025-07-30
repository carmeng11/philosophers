/* ================ PHILO.H ================ */
#ifndef PHILO_H
# define PHILO_H

# include <pthread.h>
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/time.h>
# include <unistd.h>

// Definiciones de estados para logs
# define TAKEN_FORK "has taken a fork"
# define EATING "is eating"
# define SLEEPING "is sleeping"
# define THINKING "is thinking"
# define DIED "died"

// Definiciones parseo
# define ERROR_ARG "Uso: ./philo num_philosophers time_to_die time_to_eat time_to_sleep [num_times_to_eat]"
# define ERROR_ARG_POS "Error: Todos los argumentos deben ser números positivos"
# define ERROR_MEAL_LOCK "Error: No se pudo inicializar meal_lock para filósofo"
# define ERROR_MUTEX "Error: No se pudo inicializar mutex de tenedor"
# define ERROR_THREAD "Error: No se pudo crear hilo para filósofo"

typedef struct s_data	t_data;

typedef struct s_philo
{
	int					id;
	pthread_t			thread_id;
	pthread_mutex_t		meal_lock;
	// Mutex para proteger last_meal_time y meals_eaten
	pthread_mutex_t *l_fork;  // Puntero al tenedor izquierdo
	pthread_mutex_t *r_fork;  // Puntero al tenedor derecho
	long long last_meal_time; // Protegido por meal_lock
	int meals_eaten;          // Protegido por meal_lock
	t_data				*data;
}						t_philo;

typedef struct s_data
{
	int					num_philo;
	long long			time_to_die;
	long long			time_to_eat;
	long long			time_to_sleep;
	int					max_meals;
	long long			start_time;
	int is_game_over; // Protegido por game_mutex
	pthread_mutex_t		*forks;
	pthread_mutex_t		print_mutex;
	pthread_mutex_t game_mutex; // Para proteger is_game_over
	t_philo				*philosophers;
}						t_data;

// Prototipos de funciones
int						main(int argc, char **argv);
int						init_data(t_data *data);
// int		init_data(t_data *data, int argc, char **argv);
int						create_philosophers(t_data *data);
void					*philo_routine(void *arg);
void					*monitor_routine(void *arg);
void					print_status(t_philo *philo, char *status_msg);
void					print_death(t_philo *philo);
long long				get_current_time_ms(void);
void					clean_up(t_data *data);
// int      ft_atoi(char *str);
long long				ft_atoll(char *str);
// void     *ft_calloc(size_t nmemb, size_t size);
int						is_game_over(t_data *data);
void					set_game_over(t_data *data);
// int		validate_args(char **argv, int argc);
int						validate_args(t_data *data, char **argv, int argc);
void					ft_usleep(long long time_ms);
// int		parse_input(t_data *data);
int						parse_input(t_data *data, int argc, char **argv);
void					drop_forks(t_philo *philo);
void					take_forks(t_philo *philo);
void					eat(t_philo *philo);
void					sleep_and_think(t_philo *philo);

#endif

/* ================ MAIN.C ================ */
#include "philo.h"

int	parse_input(t_data *data, int argc, char **argv)
{
	data->num_philo = ft_atoll(argv[1]);
	data->time_to_die = ft_atoll(argv[2]);
	data->time_to_eat = ft_atoll(argv[3]);
	data->time_to_sleep = ft_atoll(argv[4]);
	if (argc == 6)
		data->max_meals = ft_atoll(argv[5]);
	else
		data->max_meals = -1;
	data->is_game_over = 0;
	data->start_time = get_current_time_ms();
	return (0);
}

int	validate_args(t_data *data, char **argv, int argc)
{
	int	i;

	i = 1;
	while (i < argc)
	{
		if (ft_atoll(argv[i]) <= 0)
			return (printf("%s\n", ERROR_ARG_POS), 1);
		i++;
	}
	if (ft_atoll(argv[1]) > 200)
		return (printf("Error: Máximo 200 filósofos\n"), 1);
	if (parse_input(data, argc, argv) != 0)
		return (1);
	return (0);
}

int	main(int argc, char **argv)
{
	t_data		data;
	pthread_t	monitor_thread;
	int			i;

	if (argc < 5 || argc > 6)
		return (printf("%s\n", ERROR_ARG), 1);
	if (validate_args(&data, argv, argc) != 0)
		return (1);
	if (init_data(&data) != 0)
		return (1);
	if (create_philosophers(&data) != 0)
		return (clean_up(&data), 1);
	if (pthread_create(&monitor_thread, NULL, monitor_routine, &data) != 0)
		return (clean_up(&data), 1);
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

int	init_data(t_data *data)
{
	int	i;

	data->philosophers = malloc(sizeof(t_philo) * data->num_philo);
	data->forks = malloc(sizeof(pthread_mutex_t) * data->num_philo);
	if (!data->philosophers || !data->forks)
		return (printf("Error: No se pudo asignar memoria\n"), 1);
	i = 0;
	while (i < data->num_philo)
	{
		if (pthread_mutex_init(&data->forks[i], NULL) != 0)
			return (printf("%s\n %d\n", ERROR_MUTEX, i), 1);
		i++;
	}
	if (pthread_mutex_init(&data->print_mutex, NULL) != 0
		|| pthread_mutex_init(&data->game_mutex, NULL) != 0)
		return (printf("Error: No se pudo inicializar mutexes globales\n"), 1);
	return (0);
}
// se iniiclizan los filósofos y luego se crean los hilos, no se puede hacer en el mismo bucle porque primero hay que crear los filósofos, sino se correría el riesgo
// de que algún filósofo ya creaado intentara acceder a un recurso que aun no ha sido creado al ejecutarse el hilo con la función philo_routine
int	create_philosophers(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->num_philo)//inicializa cada filosofo
	{
		data->philosophers[i].id = i + 1;
		data->philosophers[i].last_meal_time = data->start_time;//se supone que al inicio todos comen aunque no sea asi, coincide el tiempo de ultima comida con el inicial, luego se actualzia con el tiempo actual en la funcion eat
		data->philosophers[i].meals_eaten = 0;
		data->philosophers[i].data = data;
		data->philosophers[i].l_fork = &data->forks[i];
		data->philosophers[i].r_fork = &data->forks[(i + 1) % data->num_philo];
		if (pthread_mutex_init(&data->philosophers[i].meal_lock, NULL) != 0)
			return (printf("%s\n%d\n", ERROR_MEAL_LOCK, i), 1);
		i++;
	}
	i = 0;
	while (i < data->num_philo) //crea los hilos	
	{
		if (pthread_create(&data->philosophers[i].thread_id, NULL,
				philo_routine, &data->philosophers[i]) != 0)
			return (printf("%s\n %d\n", ERROR_THREAD, i), 1);
		i++;
	}
	return (0);
}

void	clean_up(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->num_philo)
	{
		pthread_mutex_destroy(&data->forks[i]);
		pthread_mutex_destroy(&data->philosophers[i].meal_lock);
		i++;
	}
	pthread_mutex_destroy(&data->print_mutex);
	pthread_mutex_destroy(&data->game_mutex);
	free(data->forks);
	free(data->philosophers);
}


/* ================ PHILO_ROUTINE.C ================ */
#include "philo.h"

void	print_status(t_philo *philo, char *status_msg)
{
	long long	current_time;

	pthread_mutex_lock(&philo->data->print_mutex);
	// Solo imprimir si el juego no ha terminado
	if (!is_game_over(philo->data))
	{
		current_time = get_current_time_ms() - philo->data->start_time;//El mensaje saca el tiempo actual menos el tiempo de inicio del juego, para que sea relativo al inicio del juego
		printf("%lld %d %s\n", current_time, philo->id, status_msg);
	}
	pthread_mutex_unlock(&philo->data->print_mutex);
}
void	print_death(t_philo *philo)
{
	long long	current_time;

	pthread_mutex_lock(&philo->data->print_mutex);
	current_time = get_current_time_ms() - philo->data->start_time;
	printf("%lld %d %s\n", current_time, philo->id, DIED);
	pthread_mutex_unlock(&philo->data->print_mutex);
}

#include "philo.h"

static void	handle_one_philo(t_philo *philo)
{
	print_status(philo, THINKING);
	pthread_mutex_lock(philo->l_fork);
	print_status(philo, TAKEN_FORK);
	ft_usleep(philo->data->time_to_die + 1);
	pthread_mutex_unlock(philo->l_fork);
}

static void	philo_loop(t_philo *philo)
{
	while (!is_game_over(philo->data))
	{
		print_status(philo, THINKING);
		take_forks(philo);
		if (is_game_over(philo->data))
		{
			drop_forks(philo);
			break ;
		}
		eat(philo);
		drop_forks(philo);
		sleep_and_think(philo);
	}
}

void	*philo_routine(void *arg)
{
	t_philo *philo = (t_philo *)arg;

	if (philo->id % 2 == 0)
		ft_usleep(1);	// Para evitar que todos los filósofos intenten comer al mismo tiempo
		//ft_usleep(philo->data->time_to_eat / 2); // Para evitar que todos los filósofos intenten comer al mismo tiempo
	if (philo->data->num_philo == 1)
	{
		handle_one_philo(philo);
		return (NULL);
	}
	philo_loop(philo);
	return (NULL);
}
/* ================ MONITOR.C ================ */
#include "philo.h"

static int	check_philosopher_death(t_data *data)
{
	int			i;
	long long	current_time;
	long long	last_meal;

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

static int	check_all_philosophers_fed(t_data *data)
{
	int	i;
	int	meals_eaten;
	int	all_fed;

	i = 0;
	all_fed = 1;
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
			break ;
		}
		i++;
	}
	if (all_fed)
		return (set_game_over(data), 1);
	return (0);
}

void	*monitor_routine(void *arg)
{
	t_data  *data = (t_data *)arg;

	while (!is_game_over(data))
	{
		if (check_philosopher_death(data))
			break ;
		if (check_all_philosophers_fed(data))
			break ;
		ft_usleep(1);
	}
	return (NULL);
}
/* ================ UTILS.C ================ */
#include "philo.h"

long long	get_current_time_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

void	ft_usleep(long long time_ms)
{
	long long	start;

	start = get_current_time_ms();
	while ((get_current_time_ms() - start) < time_ms)
		usleep(100);
}

int	is_game_over(t_data *data)
{
	int	result;

	pthread_mutex_lock(&data->game_mutex);
	result = data->is_game_over;
	pthread_mutex_unlock(&data->game_mutex);
	return (result);
}

void	set_game_over(t_data *data)
{
	pthread_mutex_lock(&data->game_mutex);
	data->is_game_over = 1;
	pthread_mutex_unlock(&data->game_mutex);
}

long long	ft_atoll(char *str)
{
	long long	result;
	int			sign;

	result = 0;
	sign = 1;
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

/* ================ ACCTNS.C ================ */
#include "philo.h"

void	take_forks(t_philo *philo)
{
	// Evitar deadlock: el filósofo con ID menor toma primero el tenedor con índice menor
	if (philo->l_fork < philo->r_fork)
	{
		pthread_mutex_lock(philo->l_fork);
		print_status(philo, TAKEN_FORK);
		pthread_mutex_lock(philo->r_fork);
		print_status(philo, TAKEN_FORK);
	}
	else
	{
		pthread_mutex_lock(philo->r_fork);
		print_status(philo, TAKEN_FORK);
		pthread_mutex_lock(philo->l_fork);
		print_status(philo, TAKEN_FORK);
	}
}

void	drop_forks(t_philo *philo)
{
	pthread_mutex_unlock(philo->l_fork);
	pthread_mutex_unlock(philo->r_fork);
}

void	eat(t_philo *philo)
{
	print_status(philo, EATING);
	pthread_mutex_lock(&philo->meal_lock);
	philo->last_meal_time = get_current_time_ms();
	philo->meals_eaten++;
	pthread_mutex_unlock(&philo->meal_lock);
	ft_usleep(philo->data->time_to_eat);
}

void	sleep_and_think(t_philo *philo)
{
	print_status(philo, SLEEPING);
	usleep(philo->data->time_to_sleep * 1000);
	//ft_usleep(philo->data->time_to_sleep);
	print_status(philo, THINKING);
}