#include "philo.h"

//originalmente philo_routine esra muy larga, se separó en las fucnones para el caso de 1 filósofo y el resto
//se mejoró la función poniendo el tiempo de espera el tiempo de comer dividivo entre 2
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
    //espera un poco más de ese tiempo, garantizando 
    //que el monitor detecte correctamente la muerte 
    //(evita condiciones de carrera o errores de timing)
	//la funcion ft_usleep usa ms, su utilizaramos us_sleep al usar microsegundos pondríamos + 1000
    pthread_mutex_unlock(philo->l_fork);
}

void    print_status(t_philo *philo, char *status_msg)
{
    long long   current_time;

    pthread_mutex_lock(&philo->data->print_mutex);
    //recibe la dirección de memoria del mutex porque la función pthread_mutex_lock espera un puntero a un pthread_mutex_t.
	//En este caso, philo->data->print_mutex es el mutex compartido para imprimir mensajes.
	//Se pasa su dirección para que la función pueda bloquear (lock) ese recurso y evitar que varios hilos impriman al mismo tiempo, asegurando que los mensajes no se mezclen en la consola.
    // Solo imprimir si el juego no ha terminado
    if (!is_game_over(philo->data))
    {
        current_time = get_current_time_ms() - philo->data->start_time;//El mensaje saca el tiempo actual menos el tiempo de inicio del juego, para que sea relativo al inicio del juego
        printf("%lld %d %s\n", current_time, philo->id, status_msg);
    }
    pthread_mutex_unlock(&philo->data->print_mutex);
}

// void	*philo_routine(void *arg)
// {
// 	t_philo *philo = (t_philo *)arg;

// 	if (philo->id % 2 == 0)
// 		ft_usleep(1);	// Para evitar que todos los filósofos intenten comer al mismo tiempo
// 		//ft_usleep(philo->data->time_to_eat / 2); // Para evitar que todos los filósofos intenten comer al mismo tiempo
// 	if (philo->data->num_philo == 1)
// 	{
// 		handle_one_philo(philo);
// 		return (NULL);
// 	}
// 	philo_loop(philo);
// 	return (NULL);
// }

void	*philo_routine(void *arg)
{
	t_philo *philo = (t_philo *)arg;

	// Mejor sincronización inicial
	if (philo->id % 2 == 0)
		ft_usleep(philo->data->time_to_eat / 2);  // Más tiempo para pares
	
	if (philo->data->num_philo == 1)
	{
		handle_one_philo(philo);
		return (NULL);
	}
	philo_loop(philo);
	return (NULL);
}