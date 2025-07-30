/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cagomez- <cagomez-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 20:40:00 by cagomez-          #+#    #+#             */
/*   Updated: 2025/07/30 19:33:07 by cagomez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
