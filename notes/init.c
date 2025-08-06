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
		//pasa la dirección de memoria de cada filósofo (&data->philosophers[i]) como argumento al hilo.
        //Esto es necesario porque la función philo_routine espera un puntero a un filósofo (void *arg), 
		//para que cada hilo sepa a qué filósofo pertenece y pueda acceder a sus datos.
        //El parámetro de la función create_philosophers (t_data *data) es el puntero a la estructura general de datos, 
		//pero cada hilo debe recibir su propio filósofo (no toda la estructura), por eso se pasa la dirección de cada elemento del array philosophers.
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
	// Inicializar filósofos originalmente estaba así, pero tuve que dividirlo en dos funciones
	//la inicialización de los filosofos fue a create_philosophers. Al final del archivo estan las dos funciones separadas
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

//destruye los mutex de los tenedores y meal-lock para cada filósofo(en un loop)
//lueog destruye los mutex globales de impresión y juego, y libera la memoria asignada a los tenedores y filósofos
//la función clean_up se llama al final del programa para liberar todos los recursos utilizados.

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

int	init_data(t_data *data)
{
	int	i;

	data->philosophers = malloc(sizeof(t_philo) * data->num_philo);
	data->forks = malloc(sizeof(pthread_mutex_t) * data->num_philo);
	if (!data->philosophers || !data->forks)
		return (printf("Error: No se pudo asignar memoria\n"), 1);
	i = 0;
	while (i < data->num_philo)
	{
		if (pthread_mutex_init(&data->forks[i], NULL) != 0)
			return (printf("%s\n %d\n", ERROR_MUTEX, i), 1);
		i++;
	}
	if (pthread_mutex_init(&data->print_mutex, NULL) != 0
		|| pthread_mutex_init(&data->game_mutex, NULL) != 0)
		return (printf("Error: No se pudo inicializar mutexes globales\n"), 1);
	return (0);
}
// se iniiclizan los filósofos y luego se crean los hilos, no se puede hacer en el mismo bucle porque primero hay que crear los filósofos, sino se correría el riesgo
// de que algún filósofo ya creaado intentara acceder a un recurso que aun no ha sido creado al ejecutarse el hilo con la función philo_routine
int	create_philosophers(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->num_philo)//inicializa cada filosofo
	{
		data->philosophers[i].id = i + 1;
		data->philosophers[i].last_meal_time = data->start_time;//se supone que al inicio todos comen aunque no sea asi, coincide el tiempo de ultima comida con el inicial, luego se actualzia con el tiempo actual en la funcion eat
		data->philosophers[i].meals_eaten = 0;
		data->philosophers[i].data = data;
		data->philosophers[i].l_fork = &data->forks[i];
		data->philosophers[i].r_fork = &data->forks[(i + 1) % data->num_philo];
		if (pthread_mutex_init(&data->philosophers[i].meal_lock, NULL) != 0)
			return (printf("%s\n%d\n", ERROR_MEAL_LOCK, i), 1);
		i++;
	}
	i = 0;
	while (i < data->num_philo) //crea los hilos	
	{
		if (pthread_create(&data->philosophers[i].thread_id, NULL,
				philo_routine, &data->philosophers[i]) != 0)
			return (printf("%s\n %d\n", ERROR_THREAD, i), 1);
		i++;
	}
	return (0);
}