/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cagomez- <cagomez-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 21:09:47 by cagomez-          #+#    #+#             */
/*   Updated: 2025/07/28 21:12:07 by cagomez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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