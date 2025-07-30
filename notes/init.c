int	init_data(t_data *data, int argc, char **argv)
{
	int	i;

	// Inicializar valores básicos
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
	//Asignar memoria
	data->philosophers = malloc(sizeof(t_philo) * data->num_philo);
	data->forks = malloc(sizeof(pthread_mutex_t) * data->num_philo);
	if (!data->philosophers || !data->forks)
	{
		printf("Error: No se pudo asignar memoria\n");
		return (1);
	}
	// Inicializar mutexes de tenedores
	i = 0;
	while (i < data->num_philo)
	{
		if (pthread_mutex_init(&data->forks[i], NULL) != 0)
		{
			printf("Error: No se pudo inicializar mutex de tenedor %d\n", i);
			return (1);
		}
		i++;
	}
	// Inicializar mutexes globales
	if (pthread_mutex_init(&data->print_mutex, NULL) != 0
		|| pthread_mutex_init(&data->game_mutex, NULL) != 0)
	{
		printf("Error: No se pudo inicializar mutexes globales\n");
		return (1);
	}
	// Inicializar filósofos
	i = 0;
	while (i < data->num_philo)
	{
		data->philosophers[i].id = i + 1;
		data->philosophers[i].last_meal_time = data->start_time;
		data->philosophers[i].meals_eaten = 0;
		data->philosophers[i].data = data;
		// Asignar tenedores
		data->philosophers[i].l_fork = &data->forks[i];
		data->philosophers[i].r_fork = &data->forks[(i + 1) % data->num_philo];
		// Inicializar mutex personal
		if (pthread_mutex_init(&data->philosophers[i].meal_lock, NULL) != 0)
		{
			printf("Error: No se pudo inicializar meal_lock para filósofo %d\n",
				i);
			return (1);
		}
		i++;
	}
	return (0);
}