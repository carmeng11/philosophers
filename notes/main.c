/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cagomez- <cagomez-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 18:57:02 by cagomez-          #+#    #+#             */
/*   Updated: 2025/07/28 18:57:03 by cagomez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	main(int argc, char **argv)
{
	t_data		data;
	pthread_t	monitor_thread;
	int			i;

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