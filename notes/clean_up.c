void	clean_up(t_data *data)
{
	int i;

    i = 0;

	// Destruir mutexes de tenedores
	while (i < data->num_philo)
	{
		pthread_mutex_destroy(&data->forks[i]);
		pthread_mutex_destroy(&data->philosophers[i].meal_lock);
		i++;
	}

	// Destruir mutexes globales
	pthread_mutex_destroy(&data->print_mutex);
	pthread_mutex_destroy(&data->game_mutex);

	// Liberar memoria
	free(data->forks);
	free(data->philosophers);
}