
#include "philo.h"

void *philo_routine(void *arg)
{
    t_philo *philo = (t_philo *)arg;

    if (philo->id % 2 == 0)
        ft_usleep(1);
    if (philo->data->num_philo == 1)
    {
        print_status(philo, THINKING);
        pthread_mutex_lock(philo->l_fork);
        print_status(philo, TAKEN_FORK);
        ft_usleep(philo->data->time_to_die + 1);
        pthread_mutex_unlock(philo->l_fork);
        return (NULL);
    }
    while (!is_game_over(philo->data))
    {
        print_status(philo, THINKING);
        take_forks(philo);
        if (is_game_over(philo->data))
        {
            drop_forks(philo);
            break;
        }
        eat(philo);
        drop_forks(philo);
        sleep_and_think(philo);
    }
    return (NULL);
}

#include "philo.h"

static void	handle_one_philo(t_philo *philo)
{
    print_status(philo, THINKING);
    pthread_mutex_lock(philo->l_fork);
    print_status(philo, TAKEN_FORK);
    ft_usleep(philo->data->time_to_die + 1);
    //Se suma +1 para asegurarse de que el filósofo 
    espera un poco más de ese tiempo, garantizando 
    que el monitor detecte correctamente la muerte 
    (evita condiciones de carrera o errores de timing).
    pthread_mutex_unlock(philo->l_fork);
}

static void	philo_loop(t_philo *philo)
{
    while (!is_game_over(philo->data))
    {
        print_status(philo, THINKING);
        take_forks(philo);
        if (is_game_over(philo->data))
        {
            drop_forks(philo);
            break;
        }
        eat(philo);
        drop_forks(philo);
        sleep_and_think(philo);
    }
}

void	*philo_routine(void *arg)
{
    t_philo	*philo = (t_philo *)arg;

    if (philo->id % 2 == 0)
        ft_usleep(1);
    if (philo->data->num_philo == 1)
    {
        handle_one_philo(philo);
        return (NULL);
    }
    philo_loop(philo);
    return (NULL);
}


En philo_routine.c (5 funciones):

philo_routine
philo_loop
handle_one_philo
print_death
print_status

is_game_over (si la tienes aquí, si no, deja solo las 3 primeras y otra auxiliar si necesitas)
(is game over no está)

En actions.c (5 funciones):

take_forks
drop_forks
eat
sleep_and_think