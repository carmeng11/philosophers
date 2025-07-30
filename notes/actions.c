/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cagomez- <cagomez-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 18:06:58 by cagomez-          #+#    #+#             */
/*   Updated: 2025/07/30 20:08:03 by cagomez-         ###   ########.fr       */
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
	pthread_mutex_lock(&philo->meal_lock);// Bloquea el mutex meal_lock del filósofo. 
	//Este mutex protege las variables críticas relacionadas con las comidas (tiempo 
	//de última comida y contador de comidas) 
	//para evitar condiciones de carrera cuando múltiples hilos acceden a estos datos.
	philo->last_meal_time = get_current_time_ms();
	//Actualiza el tiempo de la última comida del filósofo al tiempo actual en milisegundos. 
	//Esto es crucial para el monitor que verifica si algún filósofo ha muerto de hambre.
	philo->meals_eaten++;//Incrementa el contador de comidas del filósofo. Esto se usa 
	//para determinar si el filósofo ha alcanzado el número mínimo de comidas requeridas (si se especifica).
	pthread_mutex_unlock(&philo->meal_lock);
	// Dormir por tiempo de comida
	ft_usleep(philo->data->time_to_eat);
	//Simula el tiempo que tarda el filósofo en comer usando ft_usleep. Durante este tiempo, 
	//el filósofo mantiene los tenedores bloqueados (aunque no se ve en esta función, los 
	//tenedores se toman antes de llamar a eat y se liberan después).
}

void	sleep_and_think(t_philo *philo)
{
	print_status(philo, SLEEPING);
	usleep(philo->data->time_to_sleep * 1000);//Hace que el filósofo duerma por time_to_sleep milisegundos. 
	//Se multiplica por 1000 porque usleep espera microsegundos. Nota: Aquí se usa usleep 
	//directamente en lugar de ft_usleep, lo cual podría ser inconsistente con el resto del código.
	print_status(philo, THINKING);
}