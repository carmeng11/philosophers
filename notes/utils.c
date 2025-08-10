/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: carmen <carmen@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 19:14:02 by cagomez-          #+#    #+#             */
/*   Updated: 2025/08/10 12:51:30 by carmen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
//Origen: El tiempo se cuenta desde 1970 (Epoch Unix).
//Precisión: Milisegundos.

long long   get_current_time_ms(void)
{
    struct timeval  tv;
    //Declara una estructura que tiene dos campos:
    //tv_sec: segundos desde 1970-01-01 00:00:00 UTC.
    //tv_usec: microsegundos adicionales (1 segundo = 1,000,000 microsegundos).

    gettimeofday(&tv, NULL);//Llena la estructura tv con el tiempo actual.
    return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

void	ft_usleep(long long time_ms)
{
	long long	start;

	start = get_current_time_ms();
	while ((get_current_time_ms() - start) < time_ms)
		usleep(100);
}
//Declaración de la función que retorna un int (actuando como booleano) 
//para indicar si el juego ha terminado.

int	is_game_over(t_data *data)
{
	int	result;//Declara una variable local result para almacenar 
	//temporalmente el valor del estado del juego.

	pthread_mutex_lock(&data->game_mutex);// Bloquea el mutex game_mutex antes de acceder 
	//a la variable compartida is_game_over. Esto previene condiciones 
	//de carrera donde múltiples hilos podrían leer/escribir esta variable simultáneamente.
	result = data->is_game_over;//Lee de forma segura el valor de data->is_game_over y lo 
	//almacena en result. Esta variable indica si:
	//0 = El juego continúa
	//1 = El juego ha terminado (por muerte de un filósofo o porque todos han comido suficiente)
	pthread_mutex_unlock(&data->game_mutex);//Libera el mutex inmediatamente después de leer el valor, 
	//permitiendo que otros hilos puedan acceder a esta variable.
	return (result);
}
//Declaración de la función que no retorna nada (void) y se encarga de marcar el final del juego.
void	set_game_over(t_data *data)
{
	pthread_mutex_lock(&data->game_mutex);//Bloquea el mismo mutex game_mutex para acceso exclusivo a la variable compartida. 
	//Esto garantiza que solo un hilo pueda modificar el estado del juego a la vez.
	data->is_game_over = 1;//Establece la flag de fin de juego a 1, indicando que la simulación 
	//debe terminar. Esto sucede cuando:
	//Un filósofo muere de hambre
	//Todos los filósofos han completado el número requerido de comidas
	//Se detecta cualquier condición de finalización
	pthread_mutex_unlock(&data->game_mutex);//Libera el mutex inmediatamente después de escribir el valor, 
	//permitiendo que otros hilos puedan leer el nuevo estado.
}
Importancia de estas funciones:
Patrón Thread-Safe:
Ambas funciones implementan el patrón clásico de acceso seguro a variables compartidas:

Lock → Read/Write → Unlock
Minimiza el tiempo que el mutex está bloqueado
Previene condiciones de carrera

Uso en el contexto del proyecto:

is_game_over() se llama constantemente en los bucles de los filósofos para verificar si deben continuar
set_game_over() se llama desde el monitor cuando detecta una condición de finalización

Eficiencia:

Se usa una variable local (result) para evitar mantener el mutex bloqueado durante el retorno
El acceso a la flag es muy rápido, minimizando la contención entre hilos

Estas funciones son fundamentales para la terminación ordenada de todos los hilos cuando la simulación
debe finalizar, evitando que algunos filósofos continúen ejecutándose mientras otros ya han terminado.



long long	ft_atoll(char *str)
{
	long long	result;
	int			sign;

	result = 0;
	sign = 1;
	while (*str == ' ' || (*str >= 9 && *str <= 13))
		str++;
	if (*str == '-')
		sign = -1;
	if (*str == '-' || *str == '+')
		str++;
	while (*str >= '0' && *str <= '9')
	{
		result = result * 10 + *str - '0';
		str++;
	}
	return (sign * result);
}

//con ft_atoll si pongo un string me va a dar 0 por tanto no necesito hacer una función tipo is_digit para validar los argumentos,
// puesto que//en la función validate_args indico que si el resultado del atoll es menor o igual a 0 me retorne un error indicando
// que los argumentos deben ser num positivos. El único caso que atoll no contempla es cuando se ponen caracteres después de los números
//donde los descarata y si convierte a numero lo anterior, es decir si meto un parámetro 800jjj atoll no me dará error, me cogerá el 800
//si quiero que me de error si debo ser más estricto y chequear con is_digit.

