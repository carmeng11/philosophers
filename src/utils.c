/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cagomez- <cagomez-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 19:14:02 by cagomez-          #+#    #+#             */
/*   Updated: 2025/07/28 19:15:18 by cagomez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

long long	get_current_time_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

void	ft_usleep(long long time_ms)
{
	long long	start;

	start = get_current_time_ms();
	while ((get_current_time_ms() - start) < time_ms)
		usleep(100);
}

int	is_game_over(t_data *data)
{
	int	result;

	pthread_mutex_lock(&data->game_mutex);
	result = data->is_game_over;
	pthread_mutex_unlock(&data->game_mutex);
	return (result);
}

void	set_game_over(t_data *data)
{
	pthread_mutex_lock(&data->game_mutex);
	data->is_game_over = 1;
	pthread_mutex_unlock(&data->game_mutex);
}

long long	ft_atoll(char *str)
{
	long long	result;
	int			sign;

	result = 0;
	sign = 1;
	while (*str == ' ' || (*str >= 9 && *str <= 13))
		str++;
	if (*str == '-')
		sign = -1;
	if (*str == '-' || *str == '+')
		str++;
	while (*str >= '0' && *str <= '9')
	{
		result = result * 10 + *str - '0';
		str++;
	}
	return (sign * result);
}

