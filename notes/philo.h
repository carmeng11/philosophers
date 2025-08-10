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
	pthread_mutex_t		*forks; //Puntero a un array de mutexes
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
