/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: carmen <carmen@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 18:58:13 by cagomez-          #+#    #+#             */
/*   Updated: 2025/08/10 15:00:39 by carmen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	ft_isdigit(int c)
{
	if (c >= 48 && c <= 57)
		return (1);
	return (0);
}

int	check_digit(char *str)
{
	int	i;

	i = 0;
	while (str[i] != '\0')
	{
		if (!ft_isdigit(str[i]))
			return (0);
		i++;
	}
	return (1);
}

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
		if (ft_atoll(argv[i]) <= 0 || !check_digit(argv[i]))
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
