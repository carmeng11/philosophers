/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cagomez- <cagomez-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 18:06:58 by cagomez-          #+#    #+#             */
/*   Updated: 2025/07/30 18:28:41 by cagomez-         ###   ########.fr       */
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
	// Actualizar tiempo de última comida y contador de comidas
	pthread_mutex_lock(&philo->meal_lock);
	philo->last_meal_time = get_current_time_ms();
	philo->meals_eaten++;
	pthread_mutex_unlock(&philo->meal_lock);
	// Dormir por tiempo de comida
	ft_usleep(philo->data->time_to_eat);
}

void	sleep_and_think(t_philo *philo)
{
	print_status(philo, SLEEPING);
	usleep(philo->data->time_to_sleep * 1000);
	print_status(philo, THINKING);
}