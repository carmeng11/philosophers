/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   validate_arg.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cagomez- <cagomez-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 18:12:02 by cagomez-          #+#    #+#             */
/*   Updated: 2025/07/28 18:33:02 by cagomez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"



int	validate_args(char **argv, int argc)
{
	int	i;

	i = 1;
	while (i < argc)
	{
		if (ft_atoll(argv[i]) <= 0)
		{
			printf("Error: Todos los argumentos deben ser números positivos\n");
			return (1);
		}
		i++;
	}
	if (ft_atoi(argv[1]) > 200)
	{
		printf("Error: Máximo 200 filósofos\n");
		return (1);
	}
	return (0);
}
