#include "philo.h"

void	print_status(t_philo *philo, char *status_msg)
{
	long long	current_time;

	pthread_mutex_lock(&philo->data->print_mutex);
	// Solo imprimir si el juego no ha terminado
	if (!is_game_over(philo->data))
	{
		current_time = get_current_time_ms() - philo->data->start_time;
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
		ft_usleep(1);
	if (philo->data->num_philo == 1)
	{
		handle_one_philo(philo);
		return (NULL);
	}
	philo_loop(philo);
	return (NULL);
}