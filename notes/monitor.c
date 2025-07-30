#include "philo.h"

static int check_philosopher_death(t_data *data)
{
    int i = 0;
    long long current_time;
    long long last_meal;

    while (i < data->num_philo)
    {
        current_time = get_current_time_ms();
        
        // Leer last_meal_time de forma segura
        pthread_mutex_lock(&data->philosophers[i].meal_lock);
        last_meal = data->philosophers[i].last_meal_time;
        pthread_mutex_unlock(&data->philosophers[i].meal_lock);
        
        // Verificar si el filósofo ha muerto
        if (current_time - last_meal > data->time_to_die)
        {
            set_game_over(data);
            print_death(&data->philosophers[i]);
            return (1);
        }
        i++;
    }
    return (0);
}

static int check_all_philosophers_fed(t_data *data)
{
    int i = 0;
    int meals_eaten;
    int all_fed = 1;

    if (data->max_meals == -1)
        return (0);

    while (i < data->num_philo)
    {
        pthread_mutex_lock(&data->philosophers[i].meal_lock);
        meals_eaten = data->philosophers[i].meals_eaten;
        pthread_mutex_unlock(&data->philosophers[i].meal_lock);
        
        if (meals_eaten < data->max_meals)
        {
            all_fed = 0;
            break;
        }
        i++;
    }
    
    if (all_fed)
    {
        set_game_over(data);
        return (1);
    }
    
    return (0);
}

void *monitor_routine(void *arg)
{
    t_data *data = (t_data *)arg;

    while (!is_game_over(data))
    {
        // Verificar muertes
        if (check_philosopher_death(data))
            break;
            
        // Verificar si todos han comido suficiente
        if (check_all_philosophers_fed(data))
            break;
            
        // Pausa pequeña para no sobrecargar CPU
        ft_usleep(1);
    }
    
    return (NULL);
}