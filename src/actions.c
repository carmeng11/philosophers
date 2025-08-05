/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: carmen <carmen@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 18:06:58 by cagomez-          #+#    #+#             */
/*   Updated: 2025/08/05 17:12:20 by carmen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	take_forks(t_philo *philo)
{
	// Evitar deadlock: el filósofo con ID menor toma primero el tenedor con índice menor
	if (philo->l_fork < philo->r_fork)
	{
		pthread_mutex_lock(philo->l_fork);
		print_status(philo, TAKEN_FORK);
		pthread_mutex_lock(philo->r_fork);
		print_status(philo, TAKEN_FORK);
	}
	else
	{
		pthread_mutex_lock(philo->r_fork);
		print_status(philo, TAKEN_FORK);
		pthread_mutex_lock(philo->l_fork);
		print_status(philo, TAKEN_FORK);
	}
}

void	drop_forks(t_philo *philo)
{
	pthread_mutex_unlock(philo->l_fork);
	pthread_mutex_unlock(philo->r_fork);
}

void	eat(t_philo *philo)
{
	print_status(philo, EATING);
	pthread_mutex_lock(&philo->meal_lock);
	philo->last_meal_time = get_current_time_ms();
	philo->meals_eaten++;
	pthread_mutex_unlock(&philo->meal_lock);
	ft_usleep(philo->data->time_to_eat);
}

// void	sleep_and_think(t_philo *philo)
// {
// 	print_status(philo, SLEEPING);
// 	usleep(philo->data->time_to_sleep * 1000);
// 	//ft_usleep(philo->data->time_to_sleep);
// 	print_status(philo, THINKING);
// }

void	sleep_and_think(t_philo *philo)
{
	print_status(philo, SLEEPING);
	ft_usleep(philo->data->time_to_sleep);  // Cambiar esta línea
	print_status(philo, THINKING);
	
	// Agregar espera inteligente para casos impares
	if (philo->data->num_philo % 2 != 0)
	{
		long t_think = philo->data->time_to_eat * 2 - philo->data->time_to_sleep;
		if (t_think > 0)
			ft_usleep(t_think / 2);  // Ajuste más conservador que 0.42
	}
}