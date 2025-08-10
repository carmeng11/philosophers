int	parse_input(t_data *data, int argc, char **argv)
{
	// Inicializar valores básicos
	data->num_philo = ft_atoll(argv[1]);
	data->time_to_die = ft_atoll(argv[2]);
	data->time_to_eat = ft_atoll(argv[3]);
	data->time_to_sleep = ft_atoll(argv[4]);
	if (argc == 6)
		data->max_meals = ft_atoll(argv[5]);
	else
		data->max_meals = -1;
	// Inicializa variables de control del juego
	data->is_game_over = 0; // Inicializar estado del juego
	data->start_time = get_current_time_ms(); // Obtener el tiempo de inicio de la simulación
	return (0);
}

