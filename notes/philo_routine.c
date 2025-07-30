#include "philo.h"

void	*philo_routine(void *arg)
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
			break ;
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

static void	handle_one_philo(t_philo *philo)
{
    print_status(philo, THINKING);
    pthread_mutex_lock(philo->l_fork);
    print_status(philo, TAKEN_FORK);
    ft_usleep(philo->data->time_to_die + 1);
    //Se suma +1 para asegurarse de que el filósofo 
    espera un poco más de ese tiempo, garantizando 
    que el monitor detecte correctamente la muerte 
    (evita condiciones de carrera o errores de timing).
    pthread_mutex_unlock(philo->l_fork);
}