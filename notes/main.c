/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: carmen <carmen@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 18:57:02 by cagomez-          #+#    #+#             */
/*   Updated: 2025/08/10 12:39:05 by carmen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	ft_isdigit(int c)
{
	if (c >= 48 && c <= 57)
		return (1);//es digito
	return (0);//no es digito
}

int	check_digit(char *str)
{
	int	i;

	i = 0;
	while (str[i] != '\0')
	{
		if (!ft_isdigit(str[i]))
			return (0);// Si algún carácter NO es dígito, retorna 0
		i++;
	}
	return (1); // Si todos son dígitos, retorna 1
}

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