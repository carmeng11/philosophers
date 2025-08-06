/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cagomez- <cagomez-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 20:40:00 by cagomez-          #+#    #+#             */
/*   Updated: 2025/08/06 18:19:36 by cagomez-         ###   ########.fr       */
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

int	create_philosophers(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->num_philo)
	{
		data->philosophers[i].id = i + 1;
		data->philosophers[i].last_meal_time = data->start_time;
		data->philosophers[i].meals_eaten = 0;
		data->philosophers[i].data = data;
		data->philosophers[i].l_fork = &data->forks[i];
		data->philosophers[i].r_fork = &data->forks[(i + 1) % data->num_philo];
		if (pthread_mutex_init(&data->philosophers[i].meal_lock, NULL) != 0)
			return (printf("%s\n%d\n", ERROR_MEAL_LOCK, i), 1);
		i++;
	}
	i = 0;
	while (i < data->num_philo)
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
