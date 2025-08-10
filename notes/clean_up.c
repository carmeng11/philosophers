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

1. ¿Qué es pthread_mutex_t *forks? Es un puntero a un array de mutexes.
En la práctica, se suele reservar memoria para num_philo mutexes y se accede como si fuera un array:
data->forks[i]Así que puedes decir:"Un puntero a un array de mutexes"O simplemente "un array de mutexes" 
(por cómo se usa en el código)
2. ¿Por qué se usa &data->print_mutex en pthread_mutex_destroy?
pthread_mutex_t print_mutex;Es una variable (no un puntero) que representa un mutex.Cuando llamas a 
pthread_mutex_destroy, necesitas pasarle la dirección del mutex:
pthread_mutex_destroy(&data->print_mutex);Esto es porque la función espera un puntero a un mutex, y 
print_mutex es una variable, así que necesitas el operador & para obtener su dirección.
3.¿Por qué no se usa & con punteros?Si tienes un puntero a mutex (por ejemplo, pthread_mutex_t *forks), 
ya tienes la dirección del primer mutex, así que puedes hacer:
pthread_mutex_destroy(&data->forks[i]);Aquí, data->forks[i] es un mutex, así que necesitas su dirección.
4. Variables mutex:
pthread_mutex_t_print_mutex;pthread_mutex_t game_mutex;Son variables, así que usas & para pasar su 
dirección.Array de mutexes:
pthread_mutex_t *forks;Es un puntero a un array, pero cada elemento es un mutex, así que también usas 
&data->forks[i] para pasar la dirección de cada mutex individual.En todos los casos, pthread_mutex_destroy 
espera un puntero a un mutex, por eso siempre pasas la dirección del mutex, sea variable o elemento de array.