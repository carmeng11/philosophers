int	create_philosophers(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->num_philo)
	{
		data->philosophers[i].id = i + 1;
		data->philosophers[i].last_meal_time = data->start_time; // Cada vez que un filósofo comienza a comer, actualiza su last_meal_time 
		//con el tiempo actual
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